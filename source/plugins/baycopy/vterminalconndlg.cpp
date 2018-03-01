#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QToolButton>
#include <QDialogButtonBox>
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

#include "vterminalconndlg.h"

using namespace BayCopy::Internal;

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
// VTerminalConnDlg member functions
///////////////////////////////////////////////////////////////////////
VTerminalConnDlg::VTerminalConnDlg(ProjectExplorer::PeVTerminalConn *pVTerminalConn, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pVTerminalConn(pVTerminalConn)
{
    setFixedSize(QSize(1024, 500));

    QHBoxLayout *pHBoxLayout = new QHBoxLayout;
    pHBoxLayout->setContentsMargins(5, 5, 5, 5);
    pHBoxLayout->addWidget(SetupCurrentIED());
    pHBoxLayout->addSpacing(5);
    pHBoxLayout->addLayout(SetupCenterLayout());
    pHBoxLayout->addSpacing(5);
    pHBoxLayout->addWidget(SetupSideIED());

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pVBoxLayout = GetClientLayout();
    pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    pVBoxLayout->addLayout(pHBoxLayout);
    pVBoxLayout->addSpacing(5);
    pVBoxLayout->addWidget(pDialogButtonBox);

    ProjectExplorer::PeVTerminal *pTxVTerminal = m_pVTerminalConn->GetTxVTerminal();
    ProjectExplorer::PeVTerminal *pRxVTerminal = m_pVTerminalConn->GetRxVTerminal();
    ProjectExplorer::PeStrap *pTxStrap = m_pVTerminalConn->GetTxStrap();
    ProjectExplorer::PeStrap *pRxStrap = m_pVTerminalConn->GetRxStrap();
    ProjectExplorer::PeDevice *pTxIED = pTxVTerminal->GetParentDevice();
    ProjectExplorer::PeDevice *pRxIED = pRxVTerminal->GetParentDevice();

    // Set Central
    m_pButtonGroupDirection->button(1)->setChecked(true);
    m_pButtonGroupType->button(pTxVTerminal->GetType() == ProjectExplorer::PeVTerminal::ttGoose ? 0 : 1)->setChecked(true);
    m_pButtonGroupLevel->button(pTxVTerminal->GetDAName().isEmpty() ? 1 : 0)->setChecked(true);

    // Set Current
    m_pComboBoxCurrentIED->addItem(pTxIED->GetDisplayIcon(), pTxIED->GetDisplayName(), reinterpret_cast<int>(pTxIED));

    QList<ProjectExplorer::PeVTerminal*> lstTxVTerminals = pTxIED->GetVTerminals();
    qSort(lstTxVTerminals.begin(), lstTxVTerminals.end(), ProjectExplorer::PeVTerminal::ComparNumber);
    foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstTxVTerminals)
    {
        QStandardItem *pItemVTerminal = new QStandardItem(pVTerminal->GetDisplayIcon(), pVTerminal->GetDisplayName());
        pItemVTerminal->setEditable(false);
        pItemVTerminal->setData(reinterpret_cast<int>(pVTerminal));

        m_pModelCurrent->appendRow(pItemVTerminal);
        m_mapCurrentVTerminalToItem.insert(pVTerminal, pItemVTerminal);
    }

    QList<ProjectExplorer::PeStrap*> lstTxStraps = pTxIED->GetStraps();
    qSort(lstTxStraps.begin(), lstTxStraps.end(), ProjectExplorer::PeStrap::ComparNumber);
    m_pComboBoxCurrentStrap->addItem("", 0);
    foreach(ProjectExplorer::PeStrap *pStrap, lstTxStraps)
        m_pComboBoxCurrentStrap->addItem(pStrap->GetDisplayIcon(), pStrap->GetDisplayName(), reinterpret_cast<int>(pStrap));

    // Set Side
    m_pComboBoxSideIED->addItem(pRxIED->GetDisplayIcon(), pRxIED->GetDisplayName(), reinterpret_cast<int>(pRxIED));

    QList<ProjectExplorer::PeVTerminal*> lstRxVTerminals = pRxIED->GetVTerminals();
    qSort(lstRxVTerminals.begin(), lstRxVTerminals.end(), ProjectExplorer::PeVTerminal::ComparNumber);
    foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstRxVTerminals)
    {
        QStandardItem *pItemVTerminal = new QStandardItem(pVTerminal->GetDisplayIcon(), pVTerminal->GetDisplayName());
        pItemVTerminal->setEditable(false);
        pItemVTerminal->setData(reinterpret_cast<int>(pVTerminal));

        m_pModelSide->appendRow(pItemVTerminal);
        m_mapSideVTerminalToItem.insert(pVTerminal, pItemVTerminal);
    }

    QList<ProjectExplorer::PeStrap*> lstRxStraps = pRxIED->GetStraps();
    qSort(lstRxStraps.begin(), lstRxStraps.end(), ProjectExplorer::PeStrap::ComparNumber);
    m_pComboBoxSideStrap->addItem("", 0);
    foreach(ProjectExplorer::PeStrap *pStrap, lstRxStraps)
        m_pComboBoxSideStrap->addItem(pStrap->GetDisplayIcon(), pStrap->GetDisplayName(), reinterpret_cast<int>(pStrap));

    connect(m_pLineEditCurrentFilter, SIGNAL(textChanged(QString)),
            m_pFilterProxyModelCurrent, SLOT(SlotSetFilter(QString)));
    connect(m_pLineEditSideFilter, SIGNAL(textChanged(QString)),
            m_pFilterProxyModelSide, SLOT(SlotSetFilter(QString)));
    connect(m_pListViewCurrent->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&,const QModelIndex&)),
            this, SLOT(SlotViewCurrentChanged(const QModelIndex&,const QModelIndex&)));
    connect(m_pListViewSide->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&,const QModelIndex&)),
            this, SLOT(SlotViewSideChanged(const QModelIndex&,const QModelIndex&)));
    connect(m_pToolButtonProDescCurrent, SIGNAL(clicked()),
            this, SLOT(SlotProDescClicked()));
    connect(m_pToolButtonProDescSide, SIGNAL(clicked()),
            this, SLOT(SlotProDescClicked()));

    // Set Selected Data
    m_pFilterProxyModelCurrent->SlotSetType(m_pButtonGroupType->checkedId());
    m_pFilterProxyModelSide->SlotSetType(m_pButtonGroupType->checkedId());
    m_pFilterProxyModelCurrent->SlotSetLevel(m_pButtonGroupLevel->checkedId());
    m_pFilterProxyModelSide->SlotSetLevel(m_pButtonGroupLevel->checkedId());
    m_pFilterProxyModelCurrent->SlotSetDirection(1);
    m_pFilterProxyModelSide->SlotSetDirection(0);

    if(QStandardItem *pItemVTermianlCurrent = m_mapCurrentVTerminalToItem.value(pTxVTerminal, 0))
    {
        QFont font = pItemVTermianlCurrent->font();
        font.setBold(true);
        pItemVTermianlCurrent->setFont(font);

        QModelIndex indexVTermianlCurrent = m_pFilterProxyModelCurrent->mapFromSource(pItemVTermianlCurrent->index());
        if(indexVTermianlCurrent.isValid())
        {
            m_pListViewCurrent->scrollTo(indexVTermianlCurrent, QAbstractItemView::PositionAtCenter);
            m_pListViewCurrent->setCurrentIndex(indexVTermianlCurrent);
        }
    }

    int iStrapIndex = m_pComboBoxCurrentStrap->findData(reinterpret_cast<int>(pTxStrap));
    if(iStrapIndex >= 0)
        m_pComboBoxCurrentStrap->setCurrentIndex(iStrapIndex);

    if(QStandardItem *pItemVTermianlSide = m_mapSideVTerminalToItem.value(pRxVTerminal, 0))
    {
        QFont font = pItemVTermianlSide->font();
        font.setBold(true);
        pItemVTermianlSide->setFont(font);

        QModelIndex indexVTermianlSide = m_pFilterProxyModelSide->mapFromSource(pItemVTermianlSide->index());
        if(indexVTermianlSide.isValid())
        {
            m_pListViewSide->scrollTo(indexVTermianlSide, QAbstractItemView::PositionAtCenter);
            m_pListViewSide->setCurrentIndex(indexVTermianlSide);
        }
    }

    iStrapIndex = m_pComboBoxSideStrap->findData(reinterpret_cast<int>(pRxStrap));
    if(iStrapIndex >= 0)
        m_pComboBoxSideStrap->setCurrentIndex(iStrapIndex);

    UpdateVTerminalStatus(0, m_pVTerminalConn->GetProjectVersion()->GetAllVTerminalConns());
}

VTerminalConnDlg::~VTerminalConnDlg()
{
}

QWidget* VTerminalConnDlg::SetupCurrentIED()
{
    m_pComboBoxCurrentIED = new Utils::SearchComboBox(this);
    m_pComboBoxCurrentIED->setEnabled(false);
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

    m_pListViewCurrent = new Utils::ReadOnlyListView(this);
    m_pListViewCurrent->setSelectionMode(QAbstractItemView::SingleSelection);
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

    QGroupBox *pGroupBoxIED = new QGroupBox(tr("Tx IED"), this);
    QVBoxLayout *pVBoxLayoutMain = new QVBoxLayout(pGroupBoxIED);
    pVBoxLayoutMain->addWidget(m_pComboBoxCurrentIED);
    pVBoxLayoutMain->addWidget(pFrameLine1);
    pVBoxLayoutMain->addWidget(m_pLineEditCurrentFilter);
    pVBoxLayoutMain->addWidget(m_pListViewCurrent);
    pVBoxLayoutMain->addLayout(pHBoxLayoutStrap);
    pVBoxLayoutMain->addWidget(pFrameLine2);
    pVBoxLayoutMain->addLayout(pGridLayoutDesc);

    pGroupBoxIED->setEnabled(m_pVTerminalConn->GetTxVTerminal()->GetParentDevice()->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    return pGroupBoxIED;
}

QWidget* VTerminalConnDlg::SetupSideIED()
{
    m_pComboBoxSideIED = new Utils::SearchComboBox(this);
    m_pComboBoxSideIED->setEnabled(false);
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

    m_pListViewSide = new Utils::ReadOnlyListView(this);
    m_pListViewSide->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pModelSide = new QStandardItemModel(this);
    m_pFilterProxyModelSide = new FilterProxyModel(this);
    m_pFilterProxyModelSide->setSourceModel(m_pModelSide);
    m_pListViewSide->setModel(m_pFilterProxyModelSide);

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

    QGroupBox *pGroupBoxIED = new QGroupBox(tr("Rx IED"), this);
    QVBoxLayout *pVBoxLayoutMain = new QVBoxLayout(pGroupBoxIED);
    pVBoxLayoutMain->addWidget(m_pComboBoxSideIED);
    pVBoxLayoutMain->addWidget(pFrameLine1);
    pVBoxLayoutMain->addWidget(m_pLineEditSideFilter);
    pVBoxLayoutMain->addWidget(m_pListViewSide);
    pVBoxLayoutMain->addLayout(pHBoxLayoutStrap);
    pVBoxLayoutMain->addWidget(pFrameLine2);
    pVBoxLayoutMain->addLayout(pGridLayoutDesc);

    pGroupBoxIED->setEnabled(m_pVTerminalConn->GetRxVTerminal()->GetParentDevice()->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    return pGroupBoxIED;
}

QLayout* VTerminalConnDlg::SetupCenterLayout()
{
    m_pButtonGroupDirection = new QButtonGroup(this);
    m_pButtonGroupDirection->addButton(new QRadioButton(tr("Tx") + " >>> " + tr("Rx"), this), 1);
    m_pButtonGroupDirection->addButton(new QRadioButton(tr("Rx") + " <<< " + tr("Tx"), this), 0);
    m_pButtonGroupDirection->button(1)->setChecked(true);

    QGroupBox *pGroupBoxDirection = new QGroupBox(tr("Signal Direction"), this);
    pGroupBoxDirection->setEnabled(false);
    QVBoxLayout *pVBoxLayoutDirection = new QVBoxLayout(pGroupBoxDirection);
    pVBoxLayoutDirection->addWidget(m_pButtonGroupDirection->button(1));
    pVBoxLayoutDirection->addWidget(m_pButtonGroupDirection->button(0));

    m_pButtonGroupType = new QButtonGroup(this);
    m_pButtonGroupType->addButton(new QRadioButton("GOOSE", this), 0);
    m_pButtonGroupType->addButton(new QRadioButton("SV", this), 1);
    m_pButtonGroupType->addButton(new QRadioButton(tr("All"), this), 2);
    m_pButtonGroupType->button(2)->setChecked(true);

    QGroupBox *pGroupBoxType = new QGroupBox(tr("Signal Type"), this);
    pGroupBoxType->setEnabled(false);
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
    pGroupBoxLevel->setEnabled(false);
    QVBoxLayout *pVBoxLayoutLevel = new QVBoxLayout(pGroupBoxLevel);
    pVBoxLayoutLevel->addWidget(m_pButtonGroupLevel->button(0));
    pVBoxLayoutLevel->addWidget(m_pButtonGroupLevel->button(1));
    pVBoxLayoutLevel->addWidget(m_pButtonGroupLevel->button(2));

    QVBoxLayout *pVBoxLayout = new QVBoxLayout;
    pVBoxLayout->addStretch(1);
    pVBoxLayout->addWidget(pGroupBoxDirection);
    pVBoxLayout->addSpacing(5);
    pVBoxLayout->addWidget(pGroupBoxType);
    pVBoxLayout->addSpacing(5);
    pVBoxLayout->addWidget(pGroupBoxLevel);
    pVBoxLayout->addStretch(1);

    return pVBoxLayout;
}

ProjectExplorer::PeDevice* VTerminalConnDlg::GetCurrentIED() const
{
    ProjectExplorer::PeDevice *pIEDCurrent = 0;
    if(m_pComboBoxCurrentIED->currentIndex() >= 0)
        pIEDCurrent = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxCurrentIED->itemData(m_pComboBoxCurrentIED->currentIndex()).toInt());

    return pIEDCurrent;
}

ProjectExplorer::PeDevice* VTerminalConnDlg::GetSideIED() const
{
    ProjectExplorer::PeDevice *pIEDSide = 0;
    if(m_pComboBoxSideIED->currentIndex() >= 0)
        pIEDSide = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSideIED->itemData(m_pComboBoxSideIED->currentIndex()).toInt());

    return pIEDSide;
}

void VTerminalConnDlg::UpdateVTerminalStatus(QStandardItem *pItemVTerminal, const QList<ProjectExplorer::PeVTerminalConn*> &lstVTerminalConns)
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

void VTerminalConnDlg::accept()
{
    QModelIndexList lstCurrentSelected = m_pListViewCurrent->selectionModel()->selectedRows();
    QModelIndexList lstSideSelected = m_pListViewSide->selectionModel()->selectedRows();

    if(lstCurrentSelected.isEmpty() || lstSideSelected.isEmpty())
        return;

    QStandardItem *pItemTx = m_pModelCurrent->itemFromIndex(m_pFilterProxyModelCurrent->mapToSource(lstCurrentSelected.first()));
    QStandardItem *pItemRx = m_pModelSide->itemFromIndex(m_pFilterProxyModelSide->mapToSource(lstSideSelected.first()));

    ProjectExplorer::PeVTerminal *pTxVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItemTx->data().toInt());
    ProjectExplorer::PeVTerminal *pRxVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItemRx->data().toInt());
    if(!pTxVTerminal || !pRxVTerminal)
        return;

    ProjectExplorer::PeStrap *pTxStrap = 0;
    if(m_pComboBoxCurrentStrap->currentIndex() >= 0)
        pTxStrap = reinterpret_cast<ProjectExplorer::PeStrap*>(m_pComboBoxCurrentStrap->itemData(m_pComboBoxCurrentStrap->currentIndex()).toInt());

    ProjectExplorer::PeStrap *pRxStrap = 0;
    if(m_pComboBoxSideStrap->currentIndex())
        pRxStrap = reinterpret_cast<ProjectExplorer::PeStrap*>(m_pComboBoxSideStrap->itemData(m_pComboBoxSideStrap->currentIndex()).toInt());

    m_pVTerminalConn->SetTxVTerminal(pTxVTerminal);
    m_pVTerminalConn->SetTxStrap(pTxStrap);
    m_pVTerminalConn->SetRxVTerminal(pRxVTerminal);
    m_pVTerminalConn->SetRxStrap(pRxStrap);

    StyledUi::StyledDialog::accept();
}

void VTerminalConnDlg::SlotViewCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
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

void VTerminalConnDlg::SlotViewSideChanged(const QModelIndex &current, const QModelIndex &previous)
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

void VTerminalConnDlg::SlotProDescClicked()
{
    if(sender() == m_pToolButtonProDescCurrent)
    {
        QModelIndex index = m_pFilterProxyModelCurrent->mapToSource(m_pListViewCurrent->selectionModel()->currentIndex());
        if(QStandardItem *pItem = m_pModelCurrent->itemFromIndex(index))
        {
            if(ProjectExplorer::PeVTerminal *pVTerminalUpdate = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItem->data().toInt()))
            {
                QString strProDesc = m_pLineEditCurrentProDesc->text();

                if(ProjectExplorer::PeDevice *pDevice = pVTerminalUpdate->GetParentDevice())
                {
                    foreach(ProjectExplorer::PeVTerminal *pVTerminal, pDevice->GetVTerminals())
                    {
                        if(pVTerminal->GetLDInst() == pVTerminalUpdate->GetLDInst() &&
                           pVTerminal->GetLNPrefix() == pVTerminalUpdate->GetLNPrefix() &&
                           pVTerminal->GetLNClass() == pVTerminalUpdate->GetLNClass() &&
                           pVTerminal->GetLNInst() == pVTerminalUpdate->GetLNInst() &&
                           pVTerminal->GetDOName() == pVTerminalUpdate->GetDOName())
                        {
                            pVTerminal->SetProDesc(strProDesc);
                            if(QStandardItem *pItem = m_mapCurrentVTerminalToItem.value(pVTerminal, 0))
                                pItem->setText(pVTerminal->GetDisplayName());
                        }
                    }
                }
            }
        }
    }
    else if(sender() == m_pToolButtonProDescSide)
    {
        QModelIndex index = m_pFilterProxyModelSide->mapToSource(m_pListViewSide->selectionModel()->currentIndex());
        if(QStandardItem *pItem = m_pModelSide->itemFromIndex(index))
        {
            if(ProjectExplorer::PeVTerminal *pVTerminalUpdate = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItem->data().toInt()))
            {
                QString strProDesc = m_pLineEditSideProDesc->text();

                if(ProjectExplorer::PeDevice *pDevice = pVTerminalUpdate->GetParentDevice())
                {
                    foreach(ProjectExplorer::PeVTerminal *pVTerminal, pDevice->GetVTerminals())
                    {
                        if(pVTerminal->GetLDInst() == pVTerminalUpdate->GetLDInst() &&
                           pVTerminal->GetLNPrefix() == pVTerminalUpdate->GetLNPrefix() &&
                           pVTerminal->GetLNClass() == pVTerminalUpdate->GetLNClass() &&
                           pVTerminal->GetLNInst() == pVTerminalUpdate->GetLNInst() &&
                           pVTerminal->GetDOName() == pVTerminalUpdate->GetDOName())
                        {
                            pVTerminal->SetProDesc(strProDesc);
                            if(QStandardItem *pItem = m_mapSideVTerminalToItem.value(pVTerminal, 0))
                                pItem->setText(pVTerminal->GetDisplayName());
                        }
                    }
                }
            }
        }
    }
}

void VTerminalConnDlg::SlotConnectClicked()
{
//    const QList<ProjectExplorer::PeVTerminalConn> lstVTerminalConns = GetVTerminalConns();
//    if(lstVTerminalConns.isEmpty())
//        return;

//    Utils::WaitCursor cursor;
//    Q_UNUSED(cursor)

//    foreach(ProjectExplorer::PeVTerminalConn VTerminalConn, lstVTerminalConns)
//    {
//        if(m_pProjectVersion->DbCreateObject(VTerminalConn))
//            m_pProjectVersion->CreateObject(VTerminalConn);
//    }
}
