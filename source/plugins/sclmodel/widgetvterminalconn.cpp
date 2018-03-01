#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QStandardItemModel>
#include <QButtonGroup>
#include <QRadioButton>
#include <QMessageBox>
#include <QAction>

#include "utils/waitcursor.h"
#include "utils/searchcombobox.h"
#include "utils/readonlyview.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peinfoset.h"
#include "projectexplorer/pevterminal.h"
#include "projectexplorer/pestrap.h"
#include "projectexplorer/pevterminalconn.h"

#include "widgetvterminalconn.h"
#include "dialogautoconnect.h"

using namespace SclModel::Internal;

///////////////////////////////////////////////////////////////////////
// FilterProxyModel member functions
///////////////////////////////////////////////////////////////////////
FilterProxyModel::FilterProxyModel(QObject *pParent)
        : QSortFilterProxyModel(pParent)
{
}

bool FilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    QStandardItemModel *pModel = qobject_cast<QStandardItemModel*>(sourceModel());
    if(!pModel)
        return true;

    if(QStandardItem *pItem = pModel->item(sourceRow))
    {
        ProjectExplorer::PeVTerminal *pVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItem->data().toInt());
        if(!pVTerminal)
            return true;

        if(pVTerminal->GetDirection() != m_iDirection)
            return false;

        if(m_iType != 2 && pVTerminal->GetType() != m_iType)
            return false;

        if(m_iLevel == 0 && pVTerminal->GetDAName().isEmpty() ||
           m_iLevel == 1 && !pVTerminal->GetDAName().isEmpty())
            return false;

        if(m_strFilter.trimmed().isEmpty())
            return true;

        return pItem->text().contains(QRegExp(m_strFilter));
    }

    return true;
}

void FilterProxyModel::SlotSetType(int iType)
{
    if(m_iType != iType)
    {
        m_iType = iType;
        invalidateFilter();
    }
}

void FilterProxyModel::SlotSetDirection(int iDirection)
{
    if(m_iDirection != iDirection)
    {
        m_iDirection = iDirection;
        invalidateFilter();
    }
}

void FilterProxyModel::SlotSetLevel(int iLevel)
{
    if(m_iLevel != iLevel)
    {
        m_iLevel = iLevel;
        invalidateFilter();
    }
}

void FilterProxyModel::SlotSetFilter(const QString &strFilter)
{
    if(m_strFilter != strFilter)
    {
        m_strFilter = strFilter;
        invalidateFilter();
    }
}

///////////////////////////////////////////////////////////////////////
// WidgetVTerminalConn member functions
///////////////////////////////////////////////////////////////////////
WidgetVTerminalConn::WidgetVTerminalConn(QWidget *pParent) :
    StyledUi::StyledWidget(pParent), m_pProjectVersion(0)
{
    QHBoxLayout *pHBoxLayout = new QHBoxLayout(this);
    pHBoxLayout->setContentsMargins(5, 10, 5, 5);
    pHBoxLayout->addWidget(SetupCurrentIED());
    pHBoxLayout->addLayout(SetupCenterLayout());
    pHBoxLayout->addWidget(SetupSideIED());

    connect(m_pComboBoxCurrentIED, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentIEDChanged(int)));
    connect(m_pComboBoxSideIED, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotSideIEDChanged(int)));
    connect(m_pButtonGroupDirection, SIGNAL(buttonClicked(int)),
            this, SLOT(SlotSetDirection(int)));
    connect(m_pButtonGroupType, SIGNAL(buttonClicked(int)),
            this, SLOT(SlotSetType(int)));
    connect(m_pButtonGroupLevel, SIGNAL(buttonClicked(int)),
            m_pFilterProxyModelCurrent, SLOT(SlotSetLevel(int)));
    connect(m_pButtonGroupLevel, SIGNAL(buttonClicked(int)),
            m_pFilterProxyModelSide, SLOT(SlotSetLevel(int)));
    connect(m_pLineEditCurrentFilter, SIGNAL(textChanged(QString)),
            m_pFilterProxyModelCurrent, SLOT(SlotSetFilter(QString)));
    connect(m_pLineEditSideFilter, SIGNAL(textChanged(QString)),
            m_pFilterProxyModelSide, SLOT(SlotSetFilter(QString)));
    connect(m_pListViewCurrent->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&,const QModelIndex&)),
            this, SLOT(SlotViewCurrentChanged(const QModelIndex&,const QModelIndex&)));
    connect(m_pListViewSide->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&,const QModelIndex&)),
            this, SLOT(SlotViewSideChanged(const QModelIndex&,const QModelIndex&)));
    connect(m_pListViewCurrent, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewDoubleClicked(const QModelIndex&)));
    connect(m_pListViewSide, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewDoubleClicked(const QModelIndex&)));
    connect(m_pToolButtonCheckConnect, SIGNAL(clicked()),
            this, SLOT(SlotCheckConnectClicked()));
    connect(m_pToolButtonProDescCurrent, SIGNAL(clicked()),
            this, SLOT(SlotProDescClicked()));
    connect(m_pToolButtonProDescSide, SIGNAL(clicked()),
            this, SLOT(SlotProDescClicked()));
    connect(m_pPushButtonConnect, SIGNAL(clicked()),
            this, SLOT(SlotConnectClicked()));
    connect(m_pPushButtonConnectAuto, SIGNAL(clicked()),
            this, SLOT(SlotConnectAutoClicked()));

    m_pFilterProxyModelCurrent->SlotSetType(m_pButtonGroupType->checkedId());
    m_pFilterProxyModelSide->SlotSetType(m_pButtonGroupType->checkedId());
    SlotSetDirection(m_pButtonGroupDirection->checkedId());
}

WidgetVTerminalConn::~WidgetVTerminalConn()
{
}

void WidgetVTerminalConn::SetProjectVersion(ProjectExplorer::PeProjectVersion *pProjectVersion, ProjectExplorer::PeDevice *pDevice)
{
    if(m_pProjectVersion != pProjectVersion)
    {
        if(m_pProjectVersion)
            disconnect(m_pProjectVersion, 0, this, 0);

        m_pProjectVersion = pProjectVersion;

        BlockSignals(true);
        m_pComboBoxCurrentIED->clear();
        m_pComboBoxCurrentStrap->clear();
        m_pModelCurrent->clear();
        m_pComboBoxSideIED->clear();
        m_pComboBoxSideStrap->clear();
        m_pModelSide->clear();

        if(m_pProjectVersion)
        {
            connect(m_pProjectVersion, SIGNAL(sigObjectCreated(ProjectExplorer::PeProjectObject*)),
                    this, SLOT(SlotProjectObjectCreated(ProjectExplorer::PeProjectObject*)));
            connect(m_pProjectVersion, SIGNAL(sigObjectPropertyChanged(ProjectExplorer::PeProjectObject*)),
                    this, SLOT(SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject*)));
            connect(m_pProjectVersion, SIGNAL(sigObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject*)),
                    this, SLOT(SlotProjectObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject*)));
            connect(m_pProjectVersion, SIGNAL(sigDeviceModelChanged(ProjectExplorer::PeDevice*)),
                    this, SLOT(SlotDeviceModelChanged(ProjectExplorer::PeDevice*)));

            FillCurrentIED();
        }

        BlockSignals(false);
    }

    if(pDevice)
    {
        int iIndex = m_pComboBoxCurrentIED->findData(reinterpret_cast<int>(pDevice));
        if(iIndex >= 0)
            m_pComboBoxCurrentIED->setCurrentIndex(iIndex);
    }
}

QWidget* WidgetVTerminalConn::SetupCurrentIED()
{
    m_pComboBoxCurrentIED = new Utils::SearchComboBox(this);
    m_pLineEditCurrentFilter = new QLineEdit(this);
    m_pLineEditCurrentFilter->setPlaceholderText(tr("Signal Filter"));
    m_pLineEditCurrentIEDDesc = new QLineEdit(this);
    m_pLineEditCurrentIEDDesc->setReadOnly(true);
    m_pLineEditCurrentProDesc = new QLineEdit(this);
    m_pComboBoxCurrentStrap = new QComboBox(this);
    m_pComboBoxCurrentStrap->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pToolButtonProDescCurrent = new QToolButton(this);
    m_pToolButtonProDescCurrent->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_pToolButtonProDescCurrent->setToolTip(tr("Modify"));
    m_pToolButtonProDescCurrent->setIcon(QIcon(":/sclmodel/images/oper_edit.png"));

    m_pActionViewConnectionCurrent = new QAction(tr("View Connection"), this);
    connect(m_pActionViewConnectionCurrent, SIGNAL(triggered()), this, SLOT(SlotActionViewConnectionTriggered()));

    m_pListViewCurrent = new Utils::ReadOnlyListView(this);
    m_pListViewCurrent->setObjectName("Styled_ListView");
    m_pListViewCurrent->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pListViewCurrent->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pListViewCurrent->addAction(m_pActionViewConnectionCurrent);
    m_pModelCurrent = new QStandardItemModel(this);
    m_pFilterProxyModelCurrent = new FilterProxyModel(this);
    m_pFilterProxyModelCurrent->setSourceModel(m_pModelCurrent);
    m_pListViewCurrent->setModel(m_pFilterProxyModelCurrent);

    QHBoxLayout *pHBoxLayoutStrap = new QHBoxLayout;
    pHBoxLayoutStrap->addWidget(new QLabel(tr("Through Strap:"), this));
    pHBoxLayoutStrap->addWidget(m_pComboBoxCurrentStrap);

    QHBoxLayout *pHBoxLayoutProDesc = new QHBoxLayout;
    pHBoxLayoutProDesc->setContentsMargins(0, 0, 0, 0);
    pHBoxLayoutProDesc->setSpacing(0);
    pHBoxLayoutProDesc->addWidget(m_pLineEditCurrentProDesc);
    pHBoxLayoutProDesc->addSpacing(2);
    pHBoxLayoutProDesc->addWidget(m_pToolButtonProDescCurrent);

    QGridLayout *pGridLayoutDesc = new QGridLayout;
    pGridLayoutDesc->addWidget(new QLabel(tr("IED Description:"), this), 0, 0);
    pGridLayoutDesc->addWidget(m_pLineEditCurrentIEDDesc, 0, 1);
    pGridLayoutDesc->addWidget(new QLabel(tr("Project Description:"), this), 1, 0);
    pGridLayoutDesc->addLayout(pHBoxLayoutProDesc, 1, 1);

    QFrame* pFrameLine1 = new QFrame(this);
    pFrameLine1->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QFrame* pFrameLine2 = new QFrame(this);
    pFrameLine2->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGroupBox *pGroupBoxIED = new QGroupBox(tr("Current IED"), this);
    QVBoxLayout *pVBoxLayoutMain = new QVBoxLayout(pGroupBoxIED);
    pVBoxLayoutMain->addWidget(m_pComboBoxCurrentIED);
    pVBoxLayoutMain->addWidget(pFrameLine1);
    pVBoxLayoutMain->addWidget(m_pLineEditCurrentFilter);
    pVBoxLayoutMain->addWidget(m_pListViewCurrent);
    pVBoxLayoutMain->addLayout(pHBoxLayoutStrap);
    pVBoxLayoutMain->addWidget(pFrameLine2);
    pVBoxLayoutMain->addLayout(pGridLayoutDesc);

    return pGroupBoxIED;
}

QWidget* WidgetVTerminalConn::SetupSideIED()
{
    m_pComboBoxSideIED = new Utils::SearchComboBox(this);
    m_pToolButtonCheckConnect = new QToolButton(this);
    m_pToolButtonCheckConnect->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_pToolButtonCheckConnect->setToolTip(tr("Check Link"));
    m_pToolButtonCheckConnect->setIcon(QIcon(":/sclmodel/images/oper_checklink.png"));
    m_pToolButtonCheckConnect->setCheckable(true);
    m_pLineEditSideFilter = new QLineEdit(this);
    m_pLineEditSideFilter->setPlaceholderText(tr("Signal Filter"));
    m_pLineEditSideIEDDesc = new QLineEdit(this);
    m_pLineEditSideIEDDesc->setReadOnly(true);
    m_pLineEditSideProDesc = new QLineEdit(this);
    m_pComboBoxSideStrap = new QComboBox(this);
    m_pComboBoxSideStrap->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pToolButtonProDescSide = new QToolButton(this);
    m_pToolButtonProDescSide->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_pToolButtonProDescSide->setToolTip(tr("Modify"));
    m_pToolButtonProDescSide->setIcon(QIcon(":/sclmodel/images/oper_edit.png"));

    m_pActionViewConnectionSide = new QAction(tr("View Connection"), this);
    connect(m_pActionViewConnectionSide, SIGNAL(triggered()), this, SLOT(SlotActionViewConnectionTriggered()));

    m_pListViewSide = new Utils::ReadOnlyListView(this);
    m_pListViewSide->setObjectName("Styled_ListView");
    m_pListViewSide->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pListViewSide->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pListViewSide->addAction(m_pActionViewConnectionSide);
    m_pModelSide = new QStandardItemModel(this);
    m_pFilterProxyModelSide = new FilterProxyModel(this);
    m_pFilterProxyModelSide->setSourceModel(m_pModelSide);
    m_pListViewSide->setModel(m_pFilterProxyModelSide);

    QHBoxLayout *pHBoxLayoutIED = new QHBoxLayout;
    pHBoxLayoutIED->addWidget(m_pComboBoxSideIED);
    pHBoxLayoutIED->addWidget(m_pToolButtonCheckConnect);

    QHBoxLayout *pHBoxLayoutStrap = new QHBoxLayout;
    pHBoxLayoutStrap->addWidget(new QLabel(tr("Through Strap:"), this));
    pHBoxLayoutStrap->addWidget(m_pComboBoxSideStrap);

    QHBoxLayout *pHBoxLayoutProDesc = new QHBoxLayout;
    pHBoxLayoutProDesc->setContentsMargins(0, 0, 0, 0);
    pHBoxLayoutProDesc->setSpacing(0);
    pHBoxLayoutProDesc->addWidget(m_pLineEditSideProDesc);
    pHBoxLayoutProDesc->addSpacing(2);
    pHBoxLayoutProDesc->addWidget(m_pToolButtonProDescSide);

    QGridLayout *pGridLayoutDesc = new QGridLayout;
    pGridLayoutDesc->addWidget(new QLabel(tr("IED Description:"), this), 0, 0);
    pGridLayoutDesc->addWidget(m_pLineEditSideIEDDesc, 0, 1);
    pGridLayoutDesc->addWidget(new QLabel(tr("Project Description:"), this), 1, 0);
    pGridLayoutDesc->addLayout(pHBoxLayoutProDesc, 1, 1);

    QFrame* pFrameLine1 = new QFrame(this);
    pFrameLine1->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QFrame* pFrameLine2 = new QFrame(this);
    pFrameLine2->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGroupBox *pGroupBoxIED = new QGroupBox(tr("Side IED"), this);
    QVBoxLayout *pVBoxLayoutMain = new QVBoxLayout(pGroupBoxIED);
    pVBoxLayoutMain->addLayout(pHBoxLayoutIED);
    pVBoxLayoutMain->addWidget(pFrameLine1);
    pVBoxLayoutMain->addWidget(m_pLineEditSideFilter);
    pVBoxLayoutMain->addWidget(m_pListViewSide);
    pVBoxLayoutMain->addLayout(pHBoxLayoutStrap);
    pVBoxLayoutMain->addWidget(pFrameLine2);
    pVBoxLayoutMain->addLayout(pGridLayoutDesc);

    return pGroupBoxIED;
}

QLayout* WidgetVTerminalConn::SetupCenterLayout()
{
    m_pButtonGroupDirection = new QButtonGroup(this);
    m_pButtonGroupDirection->addButton(new QRadioButton(tr("Tx") + " >>> " + tr("Rx"), this), 1);
    m_pButtonGroupDirection->addButton(new QRadioButton(tr("Rx") + " <<< " + tr("Tx"), this), 0);
    m_pButtonGroupDirection->button(1)->setChecked(true);

    QGroupBox *pGroupBoxDirection = new QGroupBox(tr("Signal Direction"), this);
    QVBoxLayout *pVBoxLayoutDirection = new QVBoxLayout(pGroupBoxDirection);
    pVBoxLayoutDirection->addWidget(m_pButtonGroupDirection->button(1));
    pVBoxLayoutDirection->addWidget(m_pButtonGroupDirection->button(0));

    m_pButtonGroupType = new QButtonGroup(this);
    m_pButtonGroupType->addButton(new QRadioButton("GOOSE", this), 0);
    m_pButtonGroupType->addButton(new QRadioButton("SV", this), 1);
    m_pButtonGroupType->addButton(new QRadioButton(tr("All"), this), 2);
    m_pButtonGroupType->button(2)->setChecked(true);

    QGroupBox *pGroupBoxType = new QGroupBox(tr("Signal Type"), this);
    QVBoxLayout *pVBoxLayoutType = new QVBoxLayout(pGroupBoxType);
    pVBoxLayoutType->addWidget(m_pButtonGroupType->button(0));
    pVBoxLayoutType->addWidget(m_pButtonGroupType->button(1));
    pVBoxLayoutType->addWidget(m_pButtonGroupType->button(2));

    m_pButtonGroupLevel = new QButtonGroup(this);
    m_pButtonGroupLevel->addButton(new QRadioButton("DA", this), 0);
    m_pButtonGroupLevel->addButton(new QRadioButton("DO", this), 1);
    m_pButtonGroupLevel->addButton(new QRadioButton(tr("All"), this), 2);
    m_pButtonGroupLevel->button(2)->setChecked(true);

    QGroupBox *pGroupBoxLevel = new QGroupBox(tr("Signal Level"), this);
    QVBoxLayout *pVBoxLayoutLevel = new QVBoxLayout(pGroupBoxLevel);
    pVBoxLayoutLevel->addWidget(m_pButtonGroupLevel->button(0));
    pVBoxLayoutLevel->addWidget(m_pButtonGroupLevel->button(1));
    pVBoxLayoutLevel->addWidget(m_pButtonGroupLevel->button(2));

    m_pPushButtonConnect = new QPushButton(QIcon(":/sclmodel/images/oper_link.png"), tr("Connect"), this);
    m_pPushButtonConnect->setIconSize(QSize(24, 24));

    m_pPushButtonConnectAuto = new QPushButton(QIcon(":/sclmodel/images/open_autolink.png"), tr("Auto Connect"), this);
    m_pPushButtonConnectAuto->setIconSize(QSize(24, 24));

    QVBoxLayout *pVBoxLayout = new QVBoxLayout;
    pVBoxLayout->addStretch(1);
    pVBoxLayout->addWidget(pGroupBoxDirection);
    pVBoxLayout->addWidget(pGroupBoxType);
    pVBoxLayout->addWidget(pGroupBoxLevel);
    pVBoxLayout->addSpacing(20);
    pVBoxLayout->addWidget(m_pPushButtonConnect);
    pVBoxLayout->addWidget(m_pPushButtonConnectAuto);
    pVBoxLayout->addStretch(1);

    return pVBoxLayout;
}

void WidgetVTerminalConn::BlockSignals(bool bBlock)
{
    m_pComboBoxCurrentIED->blockSignals(bBlock);
    m_pComboBoxCurrentStrap->blockSignals(bBlock);
    m_pListViewCurrent->blockSignals(bBlock);
    m_pComboBoxSideIED->blockSignals(bBlock);
    m_pComboBoxSideStrap->blockSignals(bBlock);
    m_pListViewSide->blockSignals(bBlock);
}

void WidgetVTerminalConn::FillCurrentIED()
{
    m_pComboBoxCurrentIED->blockSignals(true);

    ProjectExplorer::PeDevice *pIEDOldSelected = GetCurrentIED();

    m_pComboBoxCurrentIED->clear();

    foreach(ProjectExplorer::PeDevice *pIED, GetAllIEDs(0))
        m_pComboBoxCurrentIED->addItem(pIED->GetDisplayIcon(), pIED->GetDisplayName(), reinterpret_cast<int>(pIED));

    int iIndex = m_pComboBoxCurrentIED->findData(reinterpret_cast<int>(pIEDOldSelected));
    if(iIndex >= 0)
        m_pComboBoxCurrentIED->setCurrentIndex(iIndex);

    if(GetCurrentIED() != pIEDOldSelected)
        SlotCurrentIEDChanged(-1);

    m_pComboBoxCurrentIED->blockSignals(false);
}

void WidgetVTerminalConn::FillSideIED()
{
    m_pComboBoxSideIED->blockSignals(true);

    ProjectExplorer::PeDevice *pIEDOldSelected = GetSideIED();
    ProjectExplorer::PeDevice *pIEDConnected = 0;
    if(m_pToolButtonCheckConnect->isChecked())
        pIEDConnected = GetCurrentIED();

    m_pComboBoxSideIED->clear();

    if(!m_pToolButtonCheckConnect->isChecked() || pIEDConnected)
    {
        foreach(ProjectExplorer::PeDevice *pIED, GetAllIEDs(pIEDConnected))
            m_pComboBoxSideIED->addItem(pIED->GetDisplayIcon(), pIED->GetDisplayName(), reinterpret_cast<int>(pIED));

        int iIndex = m_pComboBoxSideIED->findData(reinterpret_cast<int>(pIEDOldSelected));
        if(iIndex >= 0)
            m_pComboBoxSideIED->setCurrentIndex(iIndex);
    }

    if(GetSideIED() != pIEDOldSelected)
        SlotSideIEDChanged(-1);

    m_pComboBoxSideIED->blockSignals(false);
}

ProjectExplorer::PeDevice* WidgetVTerminalConn::GetCurrentIED() const
{
    ProjectExplorer::PeDevice *pIEDCurrent = 0;
    if(m_pComboBoxCurrentIED->currentIndex() >= 0)
        pIEDCurrent = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxCurrentIED->itemData(m_pComboBoxCurrentIED->currentIndex()).toInt());

    return pIEDCurrent;
}

ProjectExplorer::PeDevice* WidgetVTerminalConn::GetSideIED() const
{
    ProjectExplorer::PeDevice *pIEDSide = 0;
    if(m_pComboBoxSideIED->currentIndex() >= 0)
        pIEDSide = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSideIED->itemData(m_pComboBoxSideIED->currentIndex()).toInt());

    return pIEDSide;
}

QList<ProjectExplorer::PeDevice*> WidgetVTerminalConn::GetAllIEDs(ProjectExplorer::PeDevice *pIEDConnected)
{
    QList<ProjectExplorer::PeDevice*> lstIEDs;

    if(!pIEDConnected)
    {
        foreach(ProjectExplorer::PeDevice *pDevice, m_pProjectVersion->GetAllDevices())
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                lstIEDs.append(pDevice);
        }
    }
    else
    {
        foreach(ProjectExplorer::PeInfoSet *pInfoSet, m_pProjectVersion->GetAllInfoSets())
        {
            ProjectExplorer::PeDevice *pTxIED = pInfoSet->GetTxIED();
            ProjectExplorer::PeDevice *pRxIED = pInfoSet->GetRxIED();
            if(!pTxIED || !pRxIED || pTxIED == pRxIED)
                continue;

            if(pTxIED == pIEDConnected && !lstIEDs.contains(pRxIED))
                lstIEDs.append(pRxIED);
            else if(pRxIED == pIEDConnected && !lstIEDs.contains(pTxIED))
                lstIEDs.append(pTxIED);
        }
    }

    qSort(lstIEDs.begin(), lstIEDs.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    return lstIEDs;
}

void WidgetVTerminalConn::UpdateVTerminalStatus(QStandardItem *pItemVTerminal, const QList<ProjectExplorer::PeVTerminalConn*> &lstVTerminalConns)
{
    QList<QStandardItem*> lstVTerminalItems;
    if(pItemVTerminal)
    {
        lstVTerminalItems << pItemVTerminal;
    }
    else
    {
        for(int i = 0; i < m_pModelCurrent->rowCount(); i++)
            lstVTerminalItems << m_pModelCurrent->item(i);

        for(int i = 0; i < m_pModelSide->rowCount(); i++)
            lstVTerminalItems << m_pModelSide->item(i);
    }

    ProjectExplorer::PeDevice *pDeviceCurrent = GetCurrentIED();
    ProjectExplorer::PeDevice *pDeviceSide = GetSideIED();

    foreach(QStandardItem *pItem, lstVTerminalItems)
    {
        ProjectExplorer::PeVTerminal *pVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItem->data().toInt());
        if(!pVTerminal)
            continue;

        bool bUsed = false;
        bool bDirect = false;
        QList<ProjectExplorer::PeVTerminal*> lstConectedVTerminals;
        foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, lstVTerminalConns)
        {
            ProjectExplorer::PeVTerminal *pVTerminalConnected = 0;
            if(pVTerminalConn->GetTxVTerminal() == pVTerminal)
                pVTerminalConnected = pVTerminalConn->GetRxVTerminal();
            else if(pVTerminalConn->GetRxVTerminal() == pVTerminal)
                pVTerminalConnected = pVTerminalConn->GetTxVTerminal();

            if(pVTerminalConnected)
            {
                bUsed = true;

                if(!lstConectedVTerminals.contains(pVTerminalConnected))
                    lstConectedVTerminals.append(pVTerminalConnected);

                if(!bDirect && pDeviceCurrent && pDeviceSide)
                {
                    if((pVTerminal->GetParentDevice() == pDeviceCurrent && pVTerminalConnected->GetParentDevice() == pDeviceSide) ||
                       (pVTerminal->GetParentDevice() == pDeviceSide && pVTerminalConnected->GetParentDevice() == pDeviceCurrent))
                    {
                        bDirect = true;
                    }
                }
            }
        }

        if(bUsed)
        {
            QString strTooltip;
            foreach(ProjectExplorer::PeVTerminal *pVTerminalConnected, lstConectedVTerminals)
            {
                if(strTooltip.isEmpty())
                    strTooltip = QString("%1(%2)").arg(pVTerminalConnected->GetDisplayName())
                                                  .arg(pVTerminalConnected->GetParentDevice()->GetDisplayName());
                else
                    strTooltip = QString("%1\n%2(%3)").arg(strTooltip)
                                                      .arg(pVTerminalConnected->GetDisplayName())
                                                      .arg(pVTerminalConnected->GetParentDevice()->GetDisplayName());
            }


            const QColor crBackground = bDirect ? Qt::red : Qt::gray;
            pItem->setBackground(crBackground);
            pItem->setData(true, Qt::UserRole + 2);
            pItem->setToolTip(strTooltip);
        }
        else
        {
            pItem->setBackground(Qt::white);
            pItem->setData(false, Qt::UserRole + 2);
            pItem->setToolTip("");
        }
    }
}

QList<ProjectExplorer::PeVTerminalConn> WidgetVTerminalConn::GetVTerminalConns()
{
    QList<ProjectExplorer::PeVTerminalConn> lstVTerminalConns;

    QModelIndexList lstCurrentSelected = m_pListViewCurrent->selectionModel()->selectedRows();
    QModelIndexList lstSideSelected = m_pListViewSide->selectionModel()->selectedRows();

    if(lstCurrentSelected.isEmpty() || lstSideSelected.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("No signal selected."));

        return lstVTerminalConns;
    }

    if(lstCurrentSelected.size() != lstSideSelected.size())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The number of signal is unmatched (Left signnals: %1, Right signals: %2)").arg(lstCurrentSelected.size()).arg(lstSideSelected.size()));

        return lstVTerminalConns;
    }

    ProjectExplorer::PeStrap *pStrapCurrent = 0;
    if(m_pComboBoxCurrentStrap->currentIndex() >= 0)
        pStrapCurrent = reinterpret_cast<ProjectExplorer::PeStrap*>(m_pComboBoxCurrentStrap->itemData(m_pComboBoxCurrentStrap->currentIndex()).toInt());

    ProjectExplorer::PeStrap *pStrapSide = 0;
    if(m_pComboBoxSideStrap->currentIndex())
        pStrapSide = reinterpret_cast<ProjectExplorer::PeStrap*>(m_pComboBoxSideStrap->itemData(m_pComboBoxSideStrap->currentIndex()).toInt());

    qSort(lstCurrentSelected);
    qSort(lstSideSelected);

    const bool bTx = (m_pButtonGroupDirection->checkedId() == 1);
    for(int i = 0; i < lstCurrentSelected.size(); i++)
    {
        QStandardItem *pItemCurrent = m_pModelCurrent->itemFromIndex(m_pFilterProxyModelCurrent->mapToSource(lstCurrentSelected.at(i)));
        QStandardItem *pItemSide = m_pModelSide->itemFromIndex(m_pFilterProxyModelSide->mapToSource(lstSideSelected.at(i)));

        ProjectExplorer::PeVTerminal *pVTerminalCurrent = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItemCurrent->data().toInt());
        ProjectExplorer::PeVTerminal *pVTerminalSide = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItemSide->data().toInt());
        if(!pVTerminalCurrent || !pVTerminalSide)
            continue;

        ProjectExplorer::PeVTerminal *pRxVTerminalConnected = 0;
        if(pVTerminalCurrent->GetDirection() == ProjectExplorer::PeVTerminal::tdIn && pItemCurrent->data(Qt::UserRole + 2).toBool())
            pRxVTerminalConnected = pVTerminalCurrent;
        else if(pVTerminalSide->GetDirection() == ProjectExplorer::PeVTerminal::tdIn && pItemSide->data(Qt::UserRole + 2).toBool())
            pRxVTerminalConnected = pVTerminalSide;

        if(pRxVTerminalConnected)
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The signal '%1' has been used").arg(pRxVTerminalConnected->GetDisplayName()));

            lstVTerminalConns.clear();
            return lstVTerminalConns;
        }

        if(pVTerminalCurrent->GetParentDevice() == pVTerminalSide->GetParentDevice())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The signal '%1' and '%2' belong to the same IED").arg(pVTerminalCurrent->GetDisplayName()).arg(pVTerminalSide->GetDisplayName()));

            lstVTerminalConns.clear();
            return lstVTerminalConns;
        }

        if(pVTerminalCurrent->GetType() != pVTerminalSide->GetType())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The type of signal '%1' and '%2' is unmatched").arg(pVTerminalCurrent->GetDisplayName()).arg(pVTerminalSide->GetDisplayName()));

            lstVTerminalConns.clear();
            return lstVTerminalConns;
        }

        if((pVTerminalCurrent->GetDAName().isEmpty() && !pVTerminalSide->GetDAName().isEmpty()) ||
           (!pVTerminalCurrent->GetDAName().isEmpty() && pVTerminalSide->GetDAName().isEmpty()))
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The level of signal '%1' and '%2' is unmatched").arg(pVTerminalCurrent->GetDisplayName()).arg(pVTerminalSide->GetDisplayName()));

            lstVTerminalConns.clear();
            return lstVTerminalConns;
        }

        ProjectExplorer::PeVTerminalConn VTerminalConn;
        VTerminalConn.SetProjectVersion(m_pProjectVersion);
        VTerminalConn.SetTxVTerminal(bTx ? pVTerminalCurrent : pVTerminalSide);
        VTerminalConn.SetTxStrap(bTx ? pStrapCurrent : pStrapSide);
        VTerminalConn.SetRxVTerminal(bTx ? pVTerminalSide : pVTerminalCurrent);
        VTerminalConn.SetRxStrap(bTx ? pStrapSide : pStrapCurrent);
        VTerminalConn.SetStraight(false);
        lstVTerminalConns.append(VTerminalConn);
    }

    return lstVTerminalConns;
}

void WidgetVTerminalConn::SlotCurrentIEDChanged(int iIndex)
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(iIndex < 0)
        iIndex = m_pComboBoxCurrentIED->currentIndex();

    m_pModelCurrent->clear();
    m_pComboBoxCurrentStrap->clear();
    m_mapCurrentVTerminalToItem.clear();

    ProjectExplorer::PeDevice *pCurrentDevice = iIndex >= 0 ? reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxCurrentIED->itemData(iIndex).toInt()) : 0;
    if(pCurrentDevice)
    {
        QList<ProjectExplorer::PeVTerminal*> lstVTerminals = pCurrentDevice->GetVTerminals();
        qSort(lstVTerminals.begin(), lstVTerminals.end(), ProjectExplorer::PeVTerminal::ComparNumber);
        foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstVTerminals)
        {
            QStandardItem *pItemVTerminal = new QStandardItem(pVTerminal->GetDisplayIcon(), pVTerminal->GetDisplayName());
            pItemVTerminal->setEditable(false);
            pItemVTerminal->setData(reinterpret_cast<int>(pVTerminal));

            m_pModelCurrent->appendRow(pItemVTerminal);
            m_mapCurrentVTerminalToItem.insert(pVTerminal, pItemVTerminal);
        }

        QList<ProjectExplorer::PeStrap*> lstStraps = pCurrentDevice->GetStraps();
        qSort(lstStraps.begin(), lstStraps.end(), ProjectExplorer::PeStrap::ComparNumber);
        m_pComboBoxCurrentStrap->addItem("", 0);
        foreach(ProjectExplorer::PeStrap *pStrap, lstStraps)
            m_pComboBoxCurrentStrap->addItem(pStrap->GetDisplayIcon(), pStrap->GetDisplayName(), reinterpret_cast<int>(pStrap));
    }
    UpdateVTerminalStatus(0, m_pProjectVersion->GetAllVTerminalConns());

    FillSideIED();

    SlotViewCurrentChanged(QModelIndex(), QModelIndex());
    emit sigCurrentDeviceChanged(pCurrentDevice);
}

void WidgetVTerminalConn::SlotSetVTerminalConn(ProjectExplorer::PeVTerminalConn *pVTerminalConn)
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    ProjectExplorer::PeVTerminal *pVTermianlTx = pVTerminalConn->GetTxVTerminal();
    ProjectExplorer::PeStrap *pStrapTx = pVTerminalConn->GetTxStrap();

    ProjectExplorer::PeVTerminal *pVTermianlRx = pVTerminalConn->GetRxVTerminal();
    ProjectExplorer::PeStrap *pStrapRx = pVTerminalConn->GetRxStrap();

    if(!pVTermianlTx || !pVTermianlRx)
        return;

    ProjectExplorer::PeDevice *pDeviceTx = pVTermianlTx->GetParentDevice();
    ProjectExplorer::PeDevice *pDeviceRx = pVTermianlRx->GetParentDevice();
    if(!pDeviceTx || !pDeviceRx)
        return;

    ProjectExplorer::PeDevice *pDeviceCurrent = 0, *pDeviceSide = 0;
    ProjectExplorer::PeVTerminal *pVTermianlCurrent = 0, *pVTermianlSide = 0;
    ProjectExplorer::PeStrap *pStrapCurrent = 0, *pStrapSide = 0;
    if(GetCurrentIED() == pDeviceTx)
    {
        pDeviceCurrent = pDeviceTx;
        pDeviceSide = pDeviceRx;

        pVTermianlCurrent = pVTermianlTx;
        pVTermianlSide = pVTermianlRx;

        pStrapCurrent = pStrapTx;
        pStrapSide = pStrapRx;

        m_pButtonGroupDirection->button(1)->setChecked(true);
        SlotSetDirection(1);
    }
    else if(GetCurrentIED() == pDeviceRx)
    {
        pDeviceCurrent = pDeviceRx;
        pDeviceSide = pDeviceTx;

        pVTermianlCurrent = pVTermianlRx;
        pVTermianlSide = pVTermianlTx;

        pStrapCurrent = pStrapRx;
        pStrapSide = pStrapTx;

        m_pButtonGroupDirection->button(0)->setChecked(true);
        SlotSetDirection(0);
    }

    if(!pDeviceCurrent || !pDeviceSide)
        return;

    m_pButtonGroupType->button(2)->setChecked(true);
    m_pFilterProxyModelCurrent->SlotSetType(2);
    m_pFilterProxyModelSide->SlotSetType(2);

    m_pButtonGroupLevel->button(2)->setChecked(true);
    m_pFilterProxyModelCurrent->SlotSetLevel(2);
    m_pFilterProxyModelSide->SlotSetLevel(2);

    int iIndex = m_pComboBoxCurrentIED->findData(reinterpret_cast<int>(pDeviceCurrent));
    if(iIndex >= 0)
    {
        m_pComboBoxCurrentIED->setCurrentIndex(iIndex);

        if(QStandardItem *pItemVTermianlCurrent = m_mapCurrentVTerminalToItem.value(pVTermianlCurrent, 0))
        {
            QModelIndex indexVTermianlCurrent = m_pFilterProxyModelCurrent->mapFromSource(pItemVTermianlCurrent->index());
            if(indexVTermianlCurrent.isValid())
            {
                m_pListViewCurrent->scrollTo(indexVTermianlCurrent, QAbstractItemView::PositionAtCenter);
                m_pListViewCurrent->setCurrentIndex(indexVTermianlCurrent);
            }
        }

        int iStrapIndex = m_pComboBoxCurrentStrap->findData(reinterpret_cast<int>(pStrapCurrent));
        if(iStrapIndex >= 0)
            m_pComboBoxCurrentStrap->setCurrentIndex(iStrapIndex);
    }

    iIndex = m_pComboBoxSideIED->findData(reinterpret_cast<int>(pDeviceSide));
    if(iIndex >= 0)
    {
        m_pComboBoxSideIED->setCurrentIndex(iIndex);

        if(QStandardItem *pItemVTermianlSide = m_mapSideVTerminalToItem.value(pVTermianlSide, 0))
        {
            QModelIndex indexVTermianlSide = m_pFilterProxyModelSide->mapFromSource(pItemVTermianlSide->index());
            if(indexVTermianlSide.isValid())
            {
                m_pListViewSide->scrollTo(indexVTermianlSide, QAbstractItemView::PositionAtCenter);
                m_pListViewSide->setCurrentIndex(indexVTermianlSide);
            }
        }

        int iStrapIndex = m_pComboBoxSideStrap->findData(reinterpret_cast<int>(pStrapSide));
        if(iStrapIndex >= 0)
            m_pComboBoxSideStrap->setCurrentIndex(iStrapIndex);
    }
}

void WidgetVTerminalConn::SlotSideIEDChanged(int iIndex)
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(iIndex < 0)
        iIndex = m_pComboBoxSideIED->currentIndex();

    m_pModelSide->clear();
    m_pComboBoxSideStrap->clear();
    m_mapSideVTerminalToItem.clear();

    ProjectExplorer::PeDevice *pSideDevice = iIndex >= 0 ? reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSideIED->itemData(iIndex).toInt()) : 0;
    if(pSideDevice)
    {
        QList<ProjectExplorer::PeVTerminal*> lstVTerminals = pSideDevice->GetVTerminals();
        qSort(lstVTerminals.begin(), lstVTerminals.end(), ProjectExplorer::PeVTerminal::ComparNumber);
        foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstVTerminals)
        {
            QStandardItem *pItemVTerminal = new QStandardItem(pVTerminal->GetDisplayIcon(), pVTerminal->GetDisplayName());
            pItemVTerminal->setEditable(false);
            pItemVTerminal->setData(reinterpret_cast<int>(pVTerminal));

            m_pModelSide->appendRow(pItemVTerminal);
            m_mapSideVTerminalToItem.insert(pVTerminal, pItemVTerminal);
        }

        QList<ProjectExplorer::PeStrap*> lstStraps = pSideDevice->GetStraps();
        qSort(lstStraps.begin(), lstStraps.end(), ProjectExplorer::PeStrap::ComparNumber);
        m_pComboBoxSideStrap->addItem("", 0);
        foreach(ProjectExplorer::PeStrap *pStrap, lstStraps)
            m_pComboBoxSideStrap->addItem(pStrap->GetDisplayIcon(), pStrap->GetDisplayName(), reinterpret_cast<int>(pStrap));
    }
    UpdateVTerminalStatus(0, m_pProjectVersion->GetAllVTerminalConns());

    SlotViewSideChanged(QModelIndex(), QModelIndex());

    emit sigSideDeviceChanged(pSideDevice);
}

void WidgetVTerminalConn::SlotSetDirection(int iDirection)
{
    if(iDirection == 0)
    {
        m_pFilterProxyModelCurrent->SlotSetDirection(0);
        m_pFilterProxyModelSide->SlotSetDirection(1);
    }
    else if(iDirection == 1)
    {
        m_pFilterProxyModelCurrent->SlotSetDirection(1);
        m_pFilterProxyModelSide->SlotSetDirection(0);
    }
}

void WidgetVTerminalConn::SlotSetType(int iType)
{
    if(iType == 0)
        m_pButtonGroupLevel->button(0)->setChecked(true);
    else if(iType == 1)
        m_pButtonGroupLevel->button(1)->setChecked(true);

    m_pFilterProxyModelCurrent->SlotSetLevel(m_pButtonGroupLevel->checkedId());
    m_pFilterProxyModelSide->SlotSetLevel(m_pButtonGroupLevel->checkedId());
    m_pFilterProxyModelCurrent->SlotSetType(iType);
    m_pFilterProxyModelSide->SlotSetType(iType);
}

void WidgetVTerminalConn::SlotViewCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(current)
    Q_UNUSED(previous)

    m_pLineEditCurrentIEDDesc->setText("");
    m_pLineEditCurrentProDesc->setText("");
    m_pToolButtonProDescCurrent->setEnabled(false);

    QModelIndex index = m_pFilterProxyModelCurrent->mapToSource(m_pListViewCurrent->selectionModel()->currentIndex());
    if(QStandardItem *pItem = m_pModelCurrent->itemFromIndex(index))
    {
        if(ProjectExplorer::PeVTerminal *pVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItem->data().toInt()))
        {
            m_pLineEditCurrentIEDDesc->setText(pVTerminal->GetIEDDesc());
            m_pLineEditCurrentProDesc->setText(pVTerminal->GetProDesc());
            m_pToolButtonProDescCurrent->setEnabled(true);
        }
    }
}

void WidgetVTerminalConn::SlotViewSideChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(current)
    Q_UNUSED(previous)

    m_pLineEditSideIEDDesc->setText("");
    m_pLineEditSideProDesc->setText("");
    m_pToolButtonProDescSide->setEnabled(false);

    QModelIndex index = m_pFilterProxyModelSide->mapToSource(m_pListViewSide->selectionModel()->currentIndex());
    if(QStandardItem *pItem = m_pModelSide->itemFromIndex(index))
    {
        if(ProjectExplorer::PeVTerminal *pVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItem->data().toInt()))
        {
            m_pLineEditSideIEDDesc->setText(pVTerminal->GetIEDDesc());
            m_pLineEditSideProDesc->setText(pVTerminal->GetProDesc());
            m_pToolButtonProDescSide->setEnabled(true);
        }
    }
}

void WidgetVTerminalConn::SlotViewDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    SlotConnectClicked();
}

void WidgetVTerminalConn::SlotActionViewConnectionTriggered()
{
    QAction *pAction = qobject_cast<QAction*>(sender());

    Utils::ReadOnlyListView *pListView = 0;
    if(pAction == m_pActionViewConnectionCurrent)
        pListView = m_pListViewCurrent;
    else if(pAction == m_pActionViewConnectionSide)
        pListView = m_pListViewSide;

    if(!pListView)
        return;

    FilterProxyModel *pFilterProxyModel = qobject_cast<FilterProxyModel*>(pListView->model());
    if(!pFilterProxyModel)
        return;

    QStandardItemModel *pModel = qobject_cast<QStandardItemModel*>(pFilterProxyModel->sourceModel());
    if(!pModel)
        return;

    QStandardItem *pItem = pModel->itemFromIndex(pFilterProxyModel->mapToSource(pListView->currentIndex()));
    if(!pItem)
        return;

    ProjectExplorer::PeVTerminal *pVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItem->data().toInt());
    if(!pVTerminal)
        return;

    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, m_pProjectVersion->GetAllVTerminalConns())
    {
        if(pVTerminalConn->GetTxVTerminal() == pVTerminal || pVTerminalConn->GetRxVTerminal() == pVTerminal)
        {
            if(pListView == m_pListViewSide)
            {
                if(pVTerminalConn->GetTxVTerminal() == pVTerminal)
                    SetProjectVersion(pVTerminalConn->GetRxVTerminal()->GetProjectVersion(), pVTerminalConn->GetRxVTerminal()->GetParentDevice());
                else
                    SetProjectVersion(pVTerminalConn->GetRxVTerminal()->GetProjectVersion(), pVTerminalConn->GetTxVTerminal()->GetParentDevice());
            }

            SlotSetVTerminalConn(pVTerminalConn);
            emit sigViewConnection(pVTerminalConn);

            return;
        }
    }
}

void WidgetVTerminalConn::SlotCheckConnectClicked()
{
    FillSideIED();
}

void WidgetVTerminalConn::SlotProDescClicked()
{
    ProjectExplorer::PeVTerminal *pVTerminalUpdate = 0;
    QString strProDesc;

    if(sender() == m_pToolButtonProDescCurrent)
    {
        QModelIndex index = m_pFilterProxyModelCurrent->mapToSource(m_pListViewCurrent->selectionModel()->currentIndex());
        if(QStandardItem *pItem = m_pModelCurrent->itemFromIndex(index))
        {
            if(pVTerminalUpdate = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItem->data().toInt()))
                strProDesc = m_pLineEditCurrentProDesc->text();
        }
    }
    else if(sender() == m_pToolButtonProDescSide)
    {
        QModelIndex index = m_pFilterProxyModelSide->mapToSource(m_pListViewSide->selectionModel()->currentIndex());
        if(QStandardItem *pItem = m_pModelSide->itemFromIndex(index))
        {
            if(pVTerminalUpdate = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItem->data().toInt()))
                strProDesc = m_pLineEditSideProDesc->text();
        }
    }

    if(pVTerminalUpdate)
    {
        QList<ProjectExplorer::PeVTerminal*> lstUpdatedVTerminals;
        lstUpdatedVTerminals.append(pVTerminalUpdate);

        if(ProjectExplorer::PeDevice *pDevice = pVTerminalUpdate->GetParentDevice())
        {
            foreach(ProjectExplorer::PeVTerminal *pVTerminal, pDevice->GetVTerminals())
            {
                if(pVTerminal == pVTerminalUpdate)
                    continue;

                if(pVTerminal->GetLDInst() == pVTerminalUpdate->GetLDInst() &&
                   pVTerminal->GetLNPrefix() == pVTerminalUpdate->GetLNPrefix() &&
                   pVTerminal->GetLNClass() == pVTerminalUpdate->GetLNClass() &&
                   pVTerminal->GetLNInst() == pVTerminalUpdate->GetLNInst() &&
                   pVTerminal->GetDOName() == pVTerminalUpdate->GetDOName())
                {
                    lstUpdatedVTerminals.append(pVTerminal);
                }
            }
        }

        foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstUpdatedVTerminals)
        {
            ProjectExplorer::PeVTerminal vterminal(*pVTerminal);
            vterminal.SetProDesc(strProDesc);

            if(m_pProjectVersion->DbUpdateObject(vterminal, true))
                m_pProjectVersion->UpdateObject(vterminal);
        }
    }
}

void WidgetVTerminalConn::SlotConnectClicked()
{
    const QList<ProjectExplorer::PeVTerminalConn> lstVTerminalConns = GetVTerminalConns();
    if(lstVTerminalConns.isEmpty())
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    foreach(ProjectExplorer::PeVTerminalConn VTerminalConn, lstVTerminalConns)
    {
        if(m_pProjectVersion->DbCreateObject(VTerminalConn, true))
            m_pProjectVersion->CreateObject(VTerminalConn);
    }
}

void WidgetVTerminalConn::SlotConnectAutoClicked()
{
    ProjectExplorer::PeDevice *pDeviceTx = 0, *pDeviceRx = 0;
    if(m_pButtonGroupDirection->checkedId() == 0)
    {
        pDeviceRx = GetCurrentIED();
        pDeviceTx = GetSideIED();
    }
    else
    {
        pDeviceRx = GetSideIED();
        pDeviceTx = GetCurrentIED();
    }

    if(!pDeviceTx || !pDeviceRx)
        return;

    DialogAutoConnect dlg(pDeviceTx, pDeviceRx, this);
    dlg.exec();
}

void WidgetVTerminalConn::SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!pProjectObject)
        return;

    const ProjectExplorer::PeProjectObject::ObjectType eObjectType = pProjectObject->GetObjectType();
    if(eObjectType == ProjectExplorer::PeProjectObject::otDevice)
    {
        if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                FillCurrentIED();
        }
    }
    else if(eObjectType == ProjectExplorer::PeProjectObject::otVTerminalConn)
    {
        if(ProjectExplorer::PeVTerminalConn *pVTerminalConn = qobject_cast<ProjectExplorer::PeVTerminalConn*>(pProjectObject))
        {
            const QList<ProjectExplorer::PeVTerminalConn*> lstVTerminalConns = m_pProjectVersion->GetAllVTerminalConns();

            ProjectExplorer::PeVTerminal *pTxVTerminal = pVTerminalConn->GetTxVTerminal();
            QStandardItem *pItemTxVTerminal = m_mapCurrentVTerminalToItem.value(pTxVTerminal, 0);
            if(!pItemTxVTerminal)
                pItemTxVTerminal = m_mapSideVTerminalToItem.value(pTxVTerminal, 0);
            if(pItemTxVTerminal)
                UpdateVTerminalStatus(pItemTxVTerminal, lstVTerminalConns);

            ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();
            QStandardItem *pItemRxVTerminal = m_mapCurrentVTerminalToItem.value(pRxVTerminal, 0);
            if(!pItemRxVTerminal)
                pItemRxVTerminal = m_mapSideVTerminalToItem.value(pRxVTerminal, 0);
            if(pItemRxVTerminal)
                UpdateVTerminalStatus(pItemRxVTerminal, lstVTerminalConns);
        }
    }
}

void WidgetVTerminalConn::SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject)
{
    const ProjectExplorer::PeProjectObject::ObjectType eObjectType = pProjectObject->GetObjectType();
    if(eObjectType == ProjectExplorer::PeProjectObject::otDevice)
    {
        if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                FillCurrentIED();
        }
    }
    else if(eObjectType == ProjectExplorer::PeProjectObject::otVTerminal)
    {
        if(ProjectExplorer::PeVTerminal *pVTerminal = qobject_cast<ProjectExplorer::PeVTerminal*>(pProjectObject))
        {
            QStandardItem *pItemVTerminal = m_mapCurrentVTerminalToItem.value(pVTerminal, 0);
            if(!pItemVTerminal)
                pItemVTerminal = m_mapSideVTerminalToItem.value(pVTerminal, 0);

            if(pItemVTerminal)
                pItemVTerminal->setText(pVTerminal->GetDisplayName());
        }
    }
}

void WidgetVTerminalConn::SlotProjectObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject *pProjectObject)
{
    const ProjectExplorer::PeProjectObject::ObjectType eObjectType = pProjectObject->GetObjectType();
    if(eObjectType == ProjectExplorer::PeProjectObject::otDevice)
    {
        if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                FillCurrentIED();
        }
    }
    else if(eObjectType == ProjectExplorer::PeProjectObject::otVTerminalConn)
    {
        if(ProjectExplorer::PeVTerminalConn *pVTerminalConn = qobject_cast<ProjectExplorer::PeVTerminalConn*>(pProjectObject))
        {
            QList<ProjectExplorer::PeVTerminalConn*> lstVTerminalConns = m_pProjectVersion->GetAllVTerminalConns();
            lstVTerminalConns.removeOne(pVTerminalConn);

            ProjectExplorer::PeVTerminal *pTxVTerminal = pVTerminalConn->GetTxVTerminal();
            QStandardItem *pItemTxVTerminal = m_mapCurrentVTerminalToItem.value(pTxVTerminal, 0);
            if(!pItemTxVTerminal)
                pItemTxVTerminal = m_mapSideVTerminalToItem.value(pTxVTerminal, 0);
            if(pItemTxVTerminal)
                UpdateVTerminalStatus(pItemTxVTerminal, lstVTerminalConns);

            ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();
            QStandardItem *pItemRxVTerminal = m_mapCurrentVTerminalToItem.value(pRxVTerminal, 0);
            if(!pItemRxVTerminal)
                pItemRxVTerminal = m_mapSideVTerminalToItem.value(pRxVTerminal, 0);
            if(pItemRxVTerminal)
                UpdateVTerminalStatus(pItemRxVTerminal, lstVTerminalConns);
        }
    }
}

void WidgetVTerminalConn::SlotDeviceModelChanged(ProjectExplorer::PeDevice *pDevice)
{
    if(GetCurrentIED() == pDevice)
        SlotCurrentIEDChanged(-1);
    else if(GetSideIED() == pDevice)
        SlotSideIEDChanged(-1);
}
