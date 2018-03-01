#include <QApplication>
#include <QVBoxLayout>
#include <QAction>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QTreeWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QStandardItemModel>

#include "utils/waitcursor.h"
#include "utils/readonlyview.h"
#include "utils/searchcombobox.h"
#include "projectexplorer/peinfoset.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/peport.h"

#include "infosetdlg.h"

using namespace BayCopy::Internal;

typedef QPair<ProjectExplorer::PePort*, ProjectExplorer::PePort*> PortPair;

static QList<PortPair> GetPairPorts(const QList<ProjectExplorer::PePort*> &lstPorts)
{
    QList<ProjectExplorer::PePort*> lstSortedPorts = lstPorts;
    qSort(lstSortedPorts.begin(), lstSortedPorts.end(), ProjectExplorer::PePort::CompareGroup);

    QList<PortPair> lstPairPorts;
    while(!lstSortedPorts.isEmpty())
    {
        PortPair pair;

        ProjectExplorer::PePort *pPort = lstSortedPorts.takeFirst();
        ProjectExplorer::PePort *pPortPair = pPort->GetPairPort();
        if(pPortPair)
            lstSortedPorts.removeOne(pPortPair);

        if(pPort->GetPortDirection() == ProjectExplorer::PePort::pdTx)
        {
            pair.first = pPort;
            pair.second = pPortPair;
        }
        else
        {
            pair.first = pPortPair;
            pair.second = pPort;
        }

        lstPairPorts.append(pair);
    }

    return lstPairPorts;
}

static QString GetPortPath(ProjectExplorer::PePort *pPortTx, ProjectExplorer::PePort *pPortRx)
{
    QString strPortPath;

    if(pPortTx && pPortRx)
    {
        if(pPortTx->GetParentBoard() == pPortRx->GetParentBoard())
        {
            if(ProjectExplorer::PeDevice *pDevice = pPortTx->GetParentDevice())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    strPortPath = QString("%1<%2:%3/%4>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetParentBoard()->GetDisplayName()).arg(pPortTx->GetDisplayName()).arg(pPortRx->GetDisplayName());
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                    strPortPath = QString("%1<%2/%3>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetDisplayName()).arg(pPortRx->GetDisplayName());
            }
        }
        else
        {
            if(ProjectExplorer::PeDevice *pDevice = pPortTx->GetParentDevice())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    strPortPath = QString("%1<%2:%3>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetParentBoard()->GetDisplayName()).arg(pPortTx->GetDisplayName());
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                    strPortPath = QString("%1<%2>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetDisplayName());
            }

            if(ProjectExplorer::PeDevice *pDevice = pPortRx->GetParentDevice())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    strPortPath = QString("%1\n%2<%3:%4>").arg(strPortPath).arg(pDevice->GetDisplayName()).arg(pPortRx->GetParentBoard()->GetDisplayName()).arg(pPortRx->GetDisplayName());
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                    strPortPath = QString("%1\n%2<%3>").arg(strPortPath).arg(pDevice->GetDisplayName()).arg(pPortRx->GetDisplayName());
            }
        }
    }
    else if(pPortTx && !pPortRx)
    {
        if(ProjectExplorer::PeDevice *pDevice = pPortTx->GetParentDevice())
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                strPortPath = QString("%1<%2:%3>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetParentBoard()->GetDisplayName()).arg(pPortTx->GetDisplayName());
            else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                strPortPath = QString("%1<%2>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetDisplayName());
        }
    }
    else if(pPortRx && !pPortTx)
    {
        if(ProjectExplorer::PeDevice *pDevice = pPortRx->GetParentDevice())
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                strPortPath = QString("%1<%2:%3>").arg(pDevice->GetDisplayName()).arg(pPortRx->GetParentBoard()->GetDisplayName()).arg(pPortRx->GetDisplayName());
            else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                strPortPath = QString("%1<%2>").arg(pDevice->GetDisplayName()).arg(pPortRx->GetDisplayName());
        }
    }

    return strPortPath;
}

static void FindPortConnectionInfo(ProjectExplorer::PePort *pPort,
                                   const QList<ProjectExplorer::PeInfoSet*> &lstAllInfoSets,
                                   QList<ProjectExplorer::PePort*> &lstConnectedPorts,
                                   QList<ProjectExplorer::PeInfoSet*> &lstInfoSetsForPort,
                                   QList<ProjectExplorer::PeInfoSet*> &lstInfoSetsForConnectedPort)
{
    foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstAllInfoSets)
    {
        ProjectExplorer::PePort *pPortConnected = 0;

        if(pInfoSet->IsContaintPort(pPort, &pPortConnected))
        {
            ProjectExplorer::PeInfoSet *pInfoSetForPort = pInfoSet;
            if(pInfoSetForPort->GetInfoSetType() == ProjectExplorer::PeInfoSet::itNone)
                pInfoSetForPort = pInfoSetForPort->GetPairInfoSet();

            if(!pInfoSetForPort)
                continue;

            if(!lstInfoSetsForPort.contains(pInfoSetForPort))
                lstInfoSetsForPort.append(pInfoSetForPort);
        }

        if(pPortConnected)
        {
            if(!lstConnectedPorts.contains(pPortConnected))
                lstConnectedPorts.append(pPortConnected);

            if(!lstInfoSetsForConnectedPort.contains(pInfoSet))
                lstInfoSetsForConnectedPort.append(pInfoSet);
        }
    }
}

static void UpdateAffectedInfoSets(ProjectExplorer::PeInfoSet *pInfoSet,
                                   ProjectExplorer::PePort *pPort,
                                   QList<ProjectExplorer::PeInfoSet*> &lstAffectInfoSets,
                                   const QList<ProjectExplorer::PeInfoSet*> &lstAllInfoSets)
{
    ProjectExplorer::PePort *pPortConnectedNew = 0;
    pInfoSet->IsContaintPort(pPort, &pPortConnectedNew);

    foreach(ProjectExplorer::PeInfoSet *pAffectInfoSet, lstAllInfoSets)
    {
        if(pAffectInfoSet == pInfoSet)
            continue;

        ProjectExplorer::PePort *pPortConnectedOld = 0;
        if(pAffectInfoSet->IsContaintPort(pPort, &pPortConnectedOld))
        {
            if(pPortConnectedOld != pPortConnectedNew)
            {
                pAffectInfoSet->ReplacePort(pPortConnectedOld, pPortConnectedNew);
                if(!lstAffectInfoSets.contains(pAffectInfoSet))
                    lstAffectInfoSets.append(pAffectInfoSet);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////
// TreeWidgetItemPort member functions
///////////////////////////////////////////////////////////////////////
TreeWidgetItemPort::TreeWidgetItemPort(ProjectExplorer::PePort *pPortTx, ProjectExplorer::PePort *pPortRx, bool bSwitchPort) :
        QTreeWidgetItem(), m_pPortTx(pPortTx), m_pPortRx(pPortRx), m_bSwitchPort(bSwitchPort), m_pPortConnectedTx(0), m_pPortConnectedRx(0)
{
    setCheckState(0, Qt::Unchecked);
    if(m_bSwitchPort)
        setCheckState(1, Qt::Unchecked);

    if(pPortTx && pPortRx)
    {
        if(pPortTx == pPortRx)
            setText(0, pPortTx->GetDisplayName());
        else
            setText(0, QString("%1/%2").arg(pPortTx->GetDisplayName()).arg(pPortRx->GetDisplayName()));
    }
    else if(pPortTx && !pPortRx)
    {
        setText(0, pPortTx->GetDisplayName());
    }
    else if(!pPortTx && pPortRx)
    {
        setText(0, pPortRx->GetDisplayName());
    }
}

void TreeWidgetItemPort::setData(int column, int role, const QVariant &value)
{
    if(data(column, role) == value)
        return;

    QTreeWidgetItem::setData(column, role, value);

    if(role == Qt::CheckStateRole)
    {
        const bool bChecked = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked);
        if(bChecked)
        {
            if(m_bSwitchPort)
            {
                if(column == 0)
                    setCheckState(1, Qt::Unchecked);
                else if(column == 1)
                    setCheckState(0, Qt::Unchecked);
            }

            if(QTreeWidget *pTreeWidget = treeWidget())
            {
                for(int i = 0; i < pTreeWidget->topLevelItemCount(); i++)
                {
                    QTreeWidgetItem *pItem = pTreeWidget->topLevelItem(i);
                    if(TreeWidgetItemPort *pItemPortOther = dynamic_cast<TreeWidgetItemPort*>(pItem))
                    {
                        if(pItemPortOther->checkState(column) == Qt::Checked && pItemPortOther != this)
                            pItemPortOther->setCheckState(column, Qt::Unchecked);
                    }

                    for(int j = 0; j < pItem->childCount(); j++)
                    {
                        if(TreeWidgetItemPort *pItemPortOther = dynamic_cast<TreeWidgetItemPort*>(pItem->child(j)))
                        {
                            if(pItemPortOther->checkState(column) == Qt::Checked && pItemPortOther != this)
                                pItemPortOther->setCheckState(column, Qt::Unchecked);
                        }
                    }
                }
            }
        }

        QFont ft = font(0);
        ft.setBold(bChecked);
        setFont(0, ft);
    }
}

ProjectExplorer::PePort* TreeWidgetItemPort::GetPortTx() const
{
    return m_pPortTx;
}

ProjectExplorer::PePort* TreeWidgetItemPort::GetPortRx() const
{
    return m_pPortRx;
}

QList<ProjectExplorer::PeInfoSet*> TreeWidgetItemPort::GetInfoSets() const
{
    return m_lstInfoSets;
}

void TreeWidgetItemPort::UpdateConnectionInfo(const QList<ProjectExplorer::PeInfoSet*> &lstInfoSets)
{
    QList<ProjectExplorer::PePort*> lstConnectedPorts;
    QList<ProjectExplorer::PeInfoSet*> lstInfoSetsForConnectedPort;

    FindPortConnectionInfo(m_pPortTx, lstInfoSets, lstConnectedPorts, m_lstInfoSets, lstInfoSetsForConnectedPort);
    if(!lstConnectedPorts.isEmpty())
        m_pPortConnectedRx = lstConnectedPorts.first();

    lstConnectedPorts.clear();
    FindPortConnectionInfo(m_pPortRx, lstInfoSets, lstConnectedPorts, m_lstInfoSets, lstInfoSetsForConnectedPort);
    if(!lstConnectedPorts.isEmpty())
        m_pPortConnectedTx = lstConnectedPorts.first();

    qSort(m_lstInfoSets.begin(), m_lstInfoSets.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    QString strConnectedPortPath = GetPortPath(m_pPortConnectedTx, m_pPortConnectedRx);
    if(!strConnectedPortPath.isEmpty())
        strConnectedPortPath = "<--->" + strConnectedPortPath;

    QString strInfoSets;
    foreach(ProjectExplorer::PeInfoSet *pInfoSet, m_lstInfoSets)
        strInfoSets = QString("%1\n%2").arg(strInfoSets).arg(pInfoSet->GetDisplayName());

    QString strToolTip = strInfoSets.isEmpty() ? strConnectedPortPath : strConnectedPortPath + "\n" + strInfoSets;
    setToolTip(0, strToolTip );
    if(m_bSwitchPort)
        setToolTip(1, strToolTip);

    MarkConnectionInfo(false);
}

void TreeWidgetItemPort::CheckDirecPortConnection(const QList<ProjectExplorer::PeDevice*> &lstDevices)
{
    if(lstDevices.isEmpty())
    {
        MarkConnectionInfo(false);
        return;
    }

    ProjectExplorer::PeDevice *pDeviceConnectedRx = m_pPortConnectedRx ? m_pPortConnectedRx->GetParentDevice() : 0;
    ProjectExplorer::PeDevice *pDeviceConnectedTx = m_pPortConnectedTx ? m_pPortConnectedTx->GetParentDevice() : 0;

    if((pDeviceConnectedRx && lstDevices.contains(pDeviceConnectedRx)) ||
       (pDeviceConnectedTx && lstDevices.contains(pDeviceConnectedTx)))
    {
        MarkConnectionInfo(true);
    }
    else
    {
        MarkConnectionInfo(false);
    }
}

void TreeWidgetItemPort::MarkConnectionInfo(bool bDirect)
{
    if(!m_lstInfoSets.isEmpty())
    {
        const QColor crBackground = bDirect ? Qt::red : Qt::gray;
        setBackgroundColor(0, crBackground);
        if(m_bSwitchPort)
            setBackgroundColor(1, crBackground);
    }
}

///////////////////////////////////////////////////////////////////////
// InfoSetDlg member functions
///////////////////////////////////////////////////////////////////////
InfoSetDlg::InfoSetDlg(ProjectExplorer::PeInfoSet *pInfoSetForward, ProjectExplorer::PeInfoSet *pInfoSetBackward, const QList<ProjectExplorer::PeInfoSet*> &lstAllInfoSets, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pInfoSetForward(pInfoSetForward), m_pInfoSetBackward(pInfoSetBackward), m_lstAllInfoSets(lstAllInfoSets)
{
    setFixedSize(QSize(1100, 550));

    QHBoxLayout *pHBoxLayoutGeneral = new QHBoxLayout;
    pHBoxLayoutGeneral->addWidget(SetupGeneralForward());
    pHBoxLayoutGeneral->addWidget(SetupGeneralBackward());

    m_pHBoxLayoutDetails = new QHBoxLayout;
    m_pHBoxLayoutDetails->addWidget(SetupDeviceForward());
    m_pHBoxLayoutDetails->addWidget(SetupSwitch1());
    m_pHBoxLayoutDetails->addWidget(SetupSwitch2());
    m_pHBoxLayoutDetails->addWidget(SetupSwitch3());
    m_pHBoxLayoutDetails->addWidget(SetupSwitch4());
    m_pHBoxLayoutDetails->addWidget(SetupDeviceBackward());

    m_pComboBoxDeviceForward->setMinimumWidth(50);
    m_pComboBoxSwitch1->setMinimumWidth(50);
    m_pComboBoxSwitch2->setMinimumWidth(50);
    m_pComboBoxSwitch3->setMinimumWidth(50);
    m_pComboBoxSwitch4->setMinimumWidth(50);
    m_pComboBoxDeviceBackward->setMinimumWidth(50);

    m_pCheckBoxUpdateAffectedInfoSets = new QCheckBox(tr("Update port connection of information sets which conflict with current infomation set"), this);
    m_pCheckBoxUpdateAffectedInfoSets->setChecked(true);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QHBoxLayout *pHBoxLayoutBottom = new QHBoxLayout;
    pHBoxLayoutBottom->setContentsMargins(2, 10, 2, 10);
    pHBoxLayoutBottom->addWidget(m_pCheckBoxUpdateAffectedInfoSets);
    pHBoxLayoutBottom->addStretch(1);
    pHBoxLayoutBottom->addWidget(pDialogButtonBox);

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(5, 10, 5, 5);
    pLayout->addLayout(pHBoxLayoutGeneral);
    pLayout->addSpacing(10);
    pLayout->addLayout(m_pHBoxLayoutDetails);
    pLayout->addSpacing(5);
    pLayout->addLayout(pHBoxLayoutBottom);

    connect(m_pComboBoxDeviceForward, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentForwardDeviceChanged(int)));
    connect(m_pComboBoxDeviceBackward, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentBackwardDeviceChanged(int)));
    connect(m_pComboBoxSwitch1, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentSwitchChanged(int)));
    connect(m_pComboBoxSwitch2, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentSwitchChanged(int)));
    connect(m_pComboBoxSwitch3, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentSwitchChanged(int)));
    connect(m_pComboBoxSwitch4, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentSwitchChanged(int)));

    SetUiData();
}

InfoSetDlg::~InfoSetDlg()
{
}

QList<ProjectExplorer::PeInfoSet*> InfoSetDlg::GetAffectedInfoSets() const
{
    return m_lstAffectedInfoSets;
}

QWidget* InfoSetDlg::SetupGeneralForward()
{
    m_pLineEditNameForward = new QLineEdit(this);
    m_pLineEditNameForward->setEnabled(false);
    m_pComboBoxTypeForward = new QComboBox(this);
    m_pComboBoxTypeForward->setFixedWidth(150);
    m_pLineEditDescForward = new QLineEdit(this);

    m_pComboBoxTypeForward->addItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(ProjectExplorer::PeInfoSet::itGoose), ProjectExplorer::PeInfoSet::itGoose);
    m_pComboBoxTypeForward->addItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(ProjectExplorer::PeInfoSet::itSv), ProjectExplorer::PeInfoSet::itSv);
    m_pComboBoxTypeForward->addItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(ProjectExplorer::PeInfoSet::itTime), ProjectExplorer::PeInfoSet::itTime);
    m_pComboBoxTypeForward->addItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(ProjectExplorer::PeInfoSet::itMix), ProjectExplorer::PeInfoSet::itMix);

    m_pGroupBoxForward = new QGroupBox(tr("Forward Information Logic"), this);
    m_pGroupBoxForward->setCheckable(true);
    m_pGroupBoxForward->setEnabled(false);
    QGridLayout *pGridLayout = new QGridLayout(m_pGroupBoxForward);

    pGridLayout->addWidget(new QLabel(tr("Name:"), this), 0, 0);
    pGridLayout->addWidget(m_pLineEditNameForward, 0, 1);
    pGridLayout->addWidget(new QLabel(tr("Type:"), this), 0, 2);
    pGridLayout->addWidget(m_pComboBoxTypeForward, 0, 3);
    pGridLayout->addWidget(new QLabel(tr("Description:"), this), 1, 0);
    pGridLayout->addWidget(m_pLineEditDescForward, 1, 1, 1, 3);

    return m_pGroupBoxForward;
}

QWidget* InfoSetDlg::SetupGeneralBackward()
{
    m_pLineEditNameBackward = new QLineEdit(this);
    m_pLineEditNameBackward->setEnabled(false);
    m_pComboBoxTypeBackward = new QComboBox(this);
    m_pComboBoxTypeBackward->setFixedWidth(150);
    m_pLineEditDescBackward = new QLineEdit(this);

    m_pComboBoxTypeBackward->addItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(ProjectExplorer::PeInfoSet::itGoose), ProjectExplorer::PeInfoSet::itGoose);
    m_pComboBoxTypeBackward->addItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(ProjectExplorer::PeInfoSet::itSv), ProjectExplorer::PeInfoSet::itSv);
    m_pComboBoxTypeBackward->addItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(ProjectExplorer::PeInfoSet::itTime), ProjectExplorer::PeInfoSet::itTime);
    m_pComboBoxTypeBackward->addItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(ProjectExplorer::PeInfoSet::itMix), ProjectExplorer::PeInfoSet::itMix);

    m_pGroupBoxBackward = new QGroupBox(tr("Backward Information Logic"), this);
    m_pGroupBoxBackward->setCheckable(true);
    m_pGroupBoxBackward->setEnabled(false);
    QGridLayout *pGridLayout = new QGridLayout(m_pGroupBoxBackward);

    pGridLayout->addWidget(new QLabel(tr("Name:"), this), 0, 0);
    pGridLayout->addWidget(m_pLineEditNameBackward, 0, 1);
    pGridLayout->addWidget(new QLabel(tr("Type:"), this), 0, 2);
    pGridLayout->addWidget(m_pComboBoxTypeBackward, 0, 3);
    pGridLayout->addWidget(new QLabel(tr("Description:"), this), 1, 0);
    pGridLayout->addWidget(m_pLineEditDescBackward, 1, 1, 1, 3);

    return m_pGroupBoxBackward;
}

QWidget* InfoSetDlg::SetupDeviceForward()
{
    m_pComboBoxDeviceForward = new Utils::SearchComboBox(this);

    m_pTreeWidgetPortForward = new QTreeWidget(this);
    m_pTreeWidgetPortForward->setHeaderHidden(true);
    m_pTreeWidgetPortForward->setIndentation(m_pTreeWidgetPortForward->indentation() * 0.7);

    m_pGroupBoxDeviceForward = new QGroupBox(tr("Current Device"), this);
    QVBoxLayout *pVBoxLayoutTxIED = new QVBoxLayout(m_pGroupBoxDeviceForward);

    pVBoxLayoutTxIED->addWidget(m_pComboBoxDeviceForward);
    pVBoxLayoutTxIED->addWidget(m_pTreeWidgetPortForward);

    return m_pGroupBoxDeviceForward;
}

QWidget* InfoSetDlg::SetupDeviceBackward()
{
    m_pComboBoxDeviceBackward = new Utils::SearchComboBox(this);

    m_pTreeWidgetPortBackward = new QTreeWidget(this);
    m_pTreeWidgetPortBackward->setHeaderHidden(true);
    m_pTreeWidgetPortBackward->setIndentation(m_pTreeWidgetPortBackward->indentation() * 0.7);

    m_pGroupBoxDeviceBackward = new QGroupBox(tr("Side Device"), this);
    QVBoxLayout *pVBoxLayoutRxIED = new QVBoxLayout(m_pGroupBoxDeviceBackward);

    pVBoxLayoutRxIED->addWidget(m_pComboBoxDeviceBackward);
    pVBoxLayoutRxIED->addWidget(m_pTreeWidgetPortBackward);

    return m_pGroupBoxDeviceBackward;
}

QWidget* InfoSetDlg::SetupSwitch1()
{
    m_pComboBoxSwitch1 = new Utils::SearchComboBox(this);

    m_pTreeWidgetPortSwitch1 = new QTreeWidget(this);
    m_pTreeWidgetPortSwitch1->setColumnCount(2);
    m_pTreeWidgetPortSwitch1->setHeaderHidden(true);
    m_pTreeWidgetPortSwitch1->setIndentation(0);
    m_pTreeWidgetPortSwitch1->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
   m_pTreeWidgetPortSwitch1->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_pTreeWidgetPortSwitch1->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    m_pGroupBoxSwitch1 = new QGroupBox(tr("First Level Switch"), this);
    m_pGroupBoxSwitch1->setVisible(false);

    QVBoxLayout *pVBoxLayoutSwitch1 = new QVBoxLayout(m_pGroupBoxSwitch1);
    pVBoxLayoutSwitch1->addWidget(m_pComboBoxSwitch1);
    pVBoxLayoutSwitch1->addWidget(m_pTreeWidgetPortSwitch1);

    return m_pGroupBoxSwitch1;
}

QWidget* InfoSetDlg::SetupSwitch2()
{
    m_pComboBoxSwitch2 = new Utils::SearchComboBox(this);

    m_pTreeWidgetPortSwitch2 = new QTreeWidget(this);
    m_pTreeWidgetPortSwitch2->setColumnCount(2);
    m_pTreeWidgetPortSwitch2->setHeaderHidden(true);
    m_pTreeWidgetPortSwitch2->setIndentation(0);
    m_pTreeWidgetPortSwitch2->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
   m_pTreeWidgetPortSwitch2->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_pTreeWidgetPortSwitch2->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    m_pGroupBoxSwitch2 = new QGroupBox(tr("Second Level Switch"), this);
    m_pGroupBoxSwitch2->setVisible(false);

    QVBoxLayout *pVBoxLayoutSwitch2 = new QVBoxLayout(m_pGroupBoxSwitch2);
    pVBoxLayoutSwitch2->addWidget(m_pComboBoxSwitch2);
    pVBoxLayoutSwitch2->addWidget(m_pTreeWidgetPortSwitch2);

    return m_pGroupBoxSwitch2;
}

QWidget* InfoSetDlg::SetupSwitch3()
{
    m_pComboBoxSwitch3 = new Utils::SearchComboBox(this);

    m_pTreeWidgetPortSwitch3 = new QTreeWidget(this);
    m_pTreeWidgetPortSwitch3->setColumnCount(2);
    m_pTreeWidgetPortSwitch3->setHeaderHidden(true);
    m_pTreeWidgetPortSwitch3->setIndentation(0);
    m_pTreeWidgetPortSwitch3->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
   m_pTreeWidgetPortSwitch3->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_pTreeWidgetPortSwitch3->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    m_pGroupBoxSwitch3 = new QGroupBox(tr("Third Level Switch"), this);
    m_pGroupBoxSwitch3->setVisible(false);

    QVBoxLayout *pVBoxLayoutSwitch3 = new QVBoxLayout(m_pGroupBoxSwitch3);
    pVBoxLayoutSwitch3->addWidget(m_pComboBoxSwitch3);
    pVBoxLayoutSwitch3->addWidget(m_pTreeWidgetPortSwitch3);

    return m_pGroupBoxSwitch3;
}

QWidget* InfoSetDlg::SetupSwitch4()
{
    m_pComboBoxSwitch4 = new Utils::SearchComboBox(this);

    m_pTreeWidgetPortSwitch4 = new QTreeWidget(this);
    m_pTreeWidgetPortSwitch4->setColumnCount(2);
    m_pTreeWidgetPortSwitch4->setHeaderHidden(true);
    m_pTreeWidgetPortSwitch4->setIndentation(0);
    m_pTreeWidgetPortSwitch4->header()->setStretchLastSection(false);
#if QT_VERSION >= 0x050000
   m_pTreeWidgetPortSwitch4->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_pTreeWidgetPortSwitch4->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    m_pGroupBoxSwitch4 = new QGroupBox(tr("Fourth Level Switch"), this);
    m_pGroupBoxSwitch4->setVisible(false);

    QVBoxLayout *pVBoxLayoutSwitch4 = new QVBoxLayout(m_pGroupBoxSwitch4);
    pVBoxLayoutSwitch4->addWidget(m_pComboBoxSwitch4);
    pVBoxLayoutSwitch4->addWidget(m_pTreeWidgetPortSwitch4);

    return m_pGroupBoxSwitch4;
}

TreeWidgetItemPort* InfoSetDlg::GetCheckedItem(QTreeWidget *pTreeWidget, int iColumn)
{
    for(int i = 0; i < pTreeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *pItem = pTreeWidget->topLevelItem(i);
        if(TreeWidgetItemPort *pItemPort = dynamic_cast<TreeWidgetItemPort*>(pItem))
        {
            if(pItemPort->checkState(iColumn) == Qt::Checked)
                return pItemPort;
        }

        for(int j = 0; j < pItem->childCount(); j++)
        {
            if(TreeWidgetItemPort *pItemPort = dynamic_cast<TreeWidgetItemPort*>(pItem->child(j)))
            {
                if(pItemPort->checkState(iColumn) == Qt::Checked)
                    return pItemPort;
            }
        }
    }

    return 0;
}

bool InfoSetDlg::SetCheckedPort(QTreeWidget *pTreeWidget, int iColumn, ProjectExplorer::PePort *pPort)
{
    if(!pPort)
        return false;

    TreeWidgetItemPort *pItemPortFound = 0;
    for(int i = 0; i < pTreeWidget->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *pItem = pTreeWidget->topLevelItem(i);
        if(TreeWidgetItemPort *pItemPort = dynamic_cast<TreeWidgetItemPort*>(pItem))
        {
            if(pItemPort->GetPortTx() == pPort || pItemPort->GetPortRx() == pPort)
            {
                pItemPortFound = pItemPort;
                break;
            }
        }

        for(int j = 0; j < pItem->childCount(); j++)
        {
            if(TreeWidgetItemPort *pItemPort = dynamic_cast<TreeWidgetItemPort*>(pItem->child(j)))
            {
                if(pItemPort->GetPortTx() == pPort || pItemPort->GetPortRx() == pPort)
                {
                    pItemPortFound = pItemPort;
                    break;
                }
            }
        }

        if(pItemPortFound)
            break;
    }

    if(pItemPortFound)
    {
        pItemPortFound->setCheckState(iColumn, Qt::Checked);
        return true;
    }

    return false;
}

void InfoSetDlg::MarkConnectedPorts()
{
    int iCurrentIndex = m_pComboBoxDeviceForward->currentIndex();
    ProjectExplorer::PeDevice *pTxIED = iCurrentIndex > 0 ? reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxDeviceForward->itemData(iCurrentIndex).toInt()) : 0;

    iCurrentIndex = m_pComboBoxDeviceBackward->currentIndex();
    ProjectExplorer::PeDevice *pRxIED = iCurrentIndex > 0 ? reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxDeviceBackward->itemData(iCurrentIndex).toInt()) : 0;

    iCurrentIndex = m_pComboBoxSwitch1->currentIndex();
    ProjectExplorer::PeDevice *pSwitch1 = iCurrentIndex > 0 ? reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitch1->itemData(iCurrentIndex).toInt()) : 0;

    iCurrentIndex = m_pComboBoxSwitch2->currentIndex();
    ProjectExplorer::PeDevice *pSwitch2 = iCurrentIndex > 0 ? reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitch2->itemData(iCurrentIndex).toInt()) : 0;

    iCurrentIndex = m_pComboBoxSwitch3->currentIndex();
    ProjectExplorer::PeDevice *pSwitch3 = iCurrentIndex > 0 ? reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitch3->itemData(iCurrentIndex).toInt()) : 0;

    iCurrentIndex = m_pComboBoxSwitch4->currentIndex();
    ProjectExplorer::PeDevice *pSwitch4 = iCurrentIndex > 0 ? reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitch4->itemData(iCurrentIndex).toInt()) : 0;

    QList<ProjectExplorer::PeDevice*> lstConnectedDevices;
    if(pSwitch1)
        lstConnectedDevices.append(pSwitch1);
    else if(pRxIED)
        lstConnectedDevices.append(pRxIED);
    for(int i = 0; i < m_pTreeWidgetPortForward->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *pItem = m_pTreeWidgetPortForward->topLevelItem(i);
        for(int j = 0; j < pItem->childCount(); j++)
            dynamic_cast<TreeWidgetItemPort*>(pItem->child(j))->CheckDirecPortConnection(lstConnectedDevices);
    }

    lstConnectedDevices.clear();
    if(pSwitch4)
        lstConnectedDevices.append(pSwitch4);
    else if(pSwitch3)
        lstConnectedDevices.append(pSwitch3);
    else if(pSwitch2)
        lstConnectedDevices.append(pSwitch2);
    else if(pSwitch1)
        lstConnectedDevices.append(pSwitch1);
    else if(pTxIED)
        lstConnectedDevices.append(pTxIED);
    for(int i = 0; i < m_pTreeWidgetPortBackward->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *pItem = m_pTreeWidgetPortBackward->topLevelItem(i);
        for(int j = 0; j < pItem->childCount(); j++)
            dynamic_cast<TreeWidgetItemPort*>(pItem->child(j))->CheckDirecPortConnection(lstConnectedDevices);
    }

    lstConnectedDevices.clear();
    if(pTxIED)
        lstConnectedDevices.append(pTxIED);
    if(pSwitch2)
        lstConnectedDevices.append(pSwitch2);
    else if(pRxIED)
        lstConnectedDevices.append(pRxIED);
    for(int i = 0; i < m_pTreeWidgetPortSwitch1->topLevelItemCount(); i++)
        dynamic_cast<TreeWidgetItemPort*>(m_pTreeWidgetPortSwitch1->topLevelItem(i))->CheckDirecPortConnection(lstConnectedDevices);

    lstConnectedDevices.clear();
    if(pSwitch1)
        lstConnectedDevices.append(pSwitch1);
    if(pSwitch3)
        lstConnectedDevices.append(pSwitch3);
    else if(pRxIED)
        lstConnectedDevices.append(pRxIED);
    for(int i = 0; i < m_pTreeWidgetPortSwitch2->topLevelItemCount(); i++)
        dynamic_cast<TreeWidgetItemPort*>(m_pTreeWidgetPortSwitch2->topLevelItem(i))->CheckDirecPortConnection(lstConnectedDevices);

    lstConnectedDevices.clear();
    if(pSwitch2)
        lstConnectedDevices.append(pSwitch2);
    if(pSwitch4)
        lstConnectedDevices.append(pSwitch4);
    else if(pRxIED)
        lstConnectedDevices.append(pRxIED);
    for(int i = 0; i < m_pTreeWidgetPortSwitch3->topLevelItemCount(); i++)
        dynamic_cast<TreeWidgetItemPort*>(m_pTreeWidgetPortSwitch3->topLevelItem(i))->CheckDirecPortConnection(lstConnectedDevices);

    lstConnectedDevices.clear();
    if(pSwitch3)
        lstConnectedDevices.append(pSwitch3);
    if(pRxIED)
        lstConnectedDevices.append(pRxIED);
    for(int i = 0; i < m_pTreeWidgetPortSwitch4->topLevelItemCount(); i++)
        dynamic_cast<TreeWidgetItemPort*>(m_pTreeWidgetPortSwitch4->topLevelItem(i))->CheckDirecPortConnection(lstConnectedDevices);
}

void InfoSetDlg::SetUiData()
{
    if(!m_pInfoSetForward || !m_pInfoSetBackward)
    {
        m_pGroupBoxForward->setChecked(false);
        m_pLineEditNameForward->clear();
        m_pLineEditDescForward->clear();
        m_pComboBoxTypeForward->setCurrentIndex(0);

        m_pGroupBoxBackward->setChecked(false);
        m_pLineEditNameBackward->clear();
        m_pLineEditDescBackward->clear();
        m_pComboBoxTypeBackward->setCurrentIndex(0);

        m_pComboBoxDeviceForward->setCurrentIndex(0);
        m_pComboBoxDeviceBackward->setCurrentIndex(0);
        m_pGroupBoxSwitch1->setChecked(false);
        m_pGroupBoxSwitch2->setChecked(false);
        m_pGroupBoxSwitch3->setChecked(false);
        m_pGroupBoxSwitch4->setChecked(false);

        m_pTreeWidgetPortForward->clear();
        m_pTreeWidgetPortBackward->clear();
        m_pTreeWidgetPortSwitch1->clear();
        m_pTreeWidgetPortSwitch2->clear();
        m_pTreeWidgetPortSwitch3->clear();
        m_pTreeWidgetPortSwitch4->clear();

        return;
    }

    /////////////////////////////////////////
    // For forward infoset
    /////////////////////////////////////////
    if(m_pInfoSetForward->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
    {
        m_pGroupBoxForward->setChecked(true);

        m_pLineEditNameForward->setText(m_pInfoSetForward->GetName());
        m_pLineEditDescForward->setText(m_pInfoSetForward->GetDescription());
        int iCurrentIndex = m_pComboBoxTypeForward->findData(m_pInfoSetForward->GetInfoSetType());
        m_pComboBoxTypeForward->setCurrentIndex(iCurrentIndex == -1 ? 0 : iCurrentIndex);        
    }
    else
    {
        m_pGroupBoxForward->setChecked(false);
        m_pLineEditNameForward->clear();
        m_pLineEditDescForward->clear();
        m_pComboBoxTypeForward->setCurrentIndex(0);
    }

    /////////////////////////////////////////
    // For pair infoset
    /////////////////////////////////////////
    if(m_pInfoSetBackward->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
    {
        m_pGroupBoxBackward->setChecked(true);

        m_pLineEditNameBackward->setText(m_pInfoSetBackward->GetName());
        m_pLineEditDescBackward->setText(m_pInfoSetBackward->GetDescription());
        int iCurrentIndex = m_pComboBoxTypeBackward->findData(m_pInfoSetBackward->GetInfoSetType());
        m_pComboBoxTypeBackward->setCurrentIndex(iCurrentIndex == -1 ? 0 : iCurrentIndex);
    }
    else
    {
        m_pGroupBoxBackward->setChecked(false);
        m_pLineEditNameBackward->clear();
        m_pLineEditDescBackward->clear();
        m_pGroupBoxBackward->setChecked(false);
    }

    /////////////////////////////////////////
    // Set device and port
    /////////////////////////////////////////
    ProjectExplorer::PeDevice *pIEDForward = 0;
    ProjectExplorer::PePort *pPortIEDForward = 0;
    ProjectExplorer::PeDevice *pIEDBackward = 0;
    ProjectExplorer::PePort *pPortIEDBackward = 0;
    ProjectExplorer::PeDevice *pSwitch1 = 0;
    ProjectExplorer::PePort *pPortSwitch1Left = 0;
    ProjectExplorer::PePort *pPortSwitch1Right = 0;
    ProjectExplorer::PeDevice *pSwitch2 = 0;
    ProjectExplorer::PePort *pPortSwitch2Left = 0;
    ProjectExplorer::PePort *pPortSwitch2Right = 0;
    ProjectExplorer::PeDevice *pSwitch3 = 0;
    ProjectExplorer::PePort *pPortSwitch3Left = 0;
    ProjectExplorer::PePort *pPortSwitch3Right = 0;
    ProjectExplorer::PeDevice *pSwitch4 = 0;
    ProjectExplorer::PePort *pPortSwitch4Left = 0;
    ProjectExplorer::PePort *pPortSwitch4Right = 0;

    if(m_pInfoSetForward->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
    {
        pIEDForward = m_pInfoSetForward->GetTxIED();
        pPortIEDForward = m_pInfoSetForward->GetTxPort();
        pIEDBackward = m_pInfoSetForward->GetRxIED();
        pPortIEDBackward = m_pInfoSetForward->GetRxPort();
        pSwitch1 = m_pInfoSetForward->GetSwitch1();
        pPortSwitch1Left = m_pInfoSetForward->GetSwitch1RxPort();
        pPortSwitch1Right = m_pInfoSetForward->GetSwitch1TxPort();
        pSwitch2 = m_pInfoSetForward->GetSwitch2();
        pPortSwitch2Left = m_pInfoSetForward->GetSwitch2RxPort();
        pPortSwitch2Right = m_pInfoSetForward->GetSwitch2TxPort();
        pSwitch3 = m_pInfoSetForward->GetSwitch3();
        pPortSwitch3Left = m_pInfoSetForward->GetSwitch3RxPort();
        pPortSwitch3Right = m_pInfoSetForward->GetSwitch3TxPort();
        pSwitch4 = m_pInfoSetForward->GetSwitch4();
        pPortSwitch4Left = m_pInfoSetForward->GetSwitch4RxPort();
        pPortSwitch4Right = m_pInfoSetForward->GetSwitch4TxPort();
    }
    else if(m_pInfoSetBackward->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
    {
        pIEDForward = m_pInfoSetBackward->GetRxIED();
        pPortIEDForward = m_pInfoSetBackward->GetRxPort();
        pIEDBackward = m_pInfoSetBackward->GetTxIED();
        pPortIEDBackward = m_pInfoSetBackward->GetTxPort();

        if(m_pInfoSetBackward->GetSwitch4())
        {
            pSwitch1 = m_pInfoSetBackward->GetSwitch4();
            pPortSwitch1Left = m_pInfoSetBackward->GetSwitch4TxPort();
            pPortSwitch1Right = m_pInfoSetBackward->GetSwitch4RxPort();
            pSwitch2 = m_pInfoSetBackward->GetSwitch3();
            pPortSwitch2Left = m_pInfoSetBackward->GetSwitch3TxPort();
            pPortSwitch2Right = m_pInfoSetBackward->GetSwitch3RxPort();
            pSwitch3 = m_pInfoSetBackward->GetSwitch2();
            pPortSwitch3Left = m_pInfoSetBackward->GetSwitch2TxPort();
            pPortSwitch3Right = m_pInfoSetBackward->GetSwitch2RxPort();
            pSwitch4 = m_pInfoSetBackward->GetSwitch1();
            pPortSwitch4Left = m_pInfoSetBackward->GetSwitch1TxPort();
            pPortSwitch4Right = m_pInfoSetBackward->GetSwitch1RxPort();
        }
        else if(m_pInfoSetBackward->GetSwitch3())
        {
            pSwitch1 = m_pInfoSetBackward->GetSwitch3();
            pPortSwitch1Left = m_pInfoSetBackward->GetSwitch3TxPort();
            pPortSwitch1Right = m_pInfoSetBackward->GetSwitch3RxPort();
            pSwitch2 = m_pInfoSetBackward->GetSwitch2();
            pPortSwitch2Left = m_pInfoSetBackward->GetSwitch2TxPort();
            pPortSwitch2Right = m_pInfoSetBackward->GetSwitch2RxPort();
            pSwitch3 = m_pInfoSetBackward->GetSwitch1();
            pPortSwitch3Left = m_pInfoSetBackward->GetSwitch1TxPort();
            pPortSwitch3Right = m_pInfoSetBackward->GetSwitch1RxPort();
        }
        else if(m_pInfoSetBackward->GetSwitch2())
        {
            pSwitch1 = m_pInfoSetBackward->GetSwitch2();
            pPortSwitch1Left = m_pInfoSetBackward->GetSwitch2TxPort();
            pPortSwitch1Right = m_pInfoSetBackward->GetSwitch2RxPort();
            pSwitch2 = m_pInfoSetBackward->GetSwitch1();
            pPortSwitch2Left = m_pInfoSetBackward->GetSwitch1TxPort();
            pPortSwitch2Right = m_pInfoSetBackward->GetSwitch1RxPort();
        }
        else if(m_pInfoSetBackward->GetSwitch1())
        {
            pSwitch1 = m_pInfoSetBackward->GetSwitch1();
            pPortSwitch1Left = m_pInfoSetBackward->GetSwitch1TxPort();
            pPortSwitch1Right = m_pInfoSetBackward->GetSwitch1RxPort();
        }
    }

    if(pIEDForward)
    {
        m_pComboBoxDeviceForward->addItem(pIEDForward->GetDisplayIcon(), pIEDForward->GetDisplayName(), reinterpret_cast<int>(pIEDForward));
        SlotCurrentForwardDeviceChanged(0);
        if(pPortIEDForward)
            SetCheckedPort(m_pTreeWidgetPortForward, 0, pPortIEDForward);

        m_pComboBoxDeviceForward->setEnabled(pIEDForward->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        m_pTreeWidgetPortForward->setEnabled(pIEDForward->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    }

    if(pIEDBackward)
    {
        m_pComboBoxDeviceBackward->addItem(pIEDBackward->GetDisplayIcon(), pIEDBackward->GetDisplayName(), reinterpret_cast<int>(pIEDBackward));
        SlotCurrentBackwardDeviceChanged(0);
        if(pPortIEDBackward)
            SetCheckedPort(m_pTreeWidgetPortBackward, 0, pPortIEDBackward);

        m_pComboBoxDeviceBackward->setEnabled(pIEDBackward->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        m_pTreeWidgetPortBackward->setEnabled(pIEDBackward->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    }

    if(pSwitch1)
    {
        m_pGroupBoxSwitch1->setVisible(true);
        m_pComboBoxSwitch1->addItem(pSwitch1->GetDisplayIcon(), pSwitch1->GetDisplayName(), reinterpret_cast<int>(pSwitch1));
        SlotCurrentSwitchChanged(0, m_pComboBoxSwitch1);
        if(pPortSwitch1Left)
            SetCheckedPort(m_pTreeWidgetPortSwitch1, 0, pPortSwitch1Left);
        if(pPortSwitch1Right)
            SetCheckedPort(m_pTreeWidgetPortSwitch1, 1, pPortSwitch1Right);

        m_pComboBoxSwitch1->setEnabled(pSwitch1->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        m_pTreeWidgetPortSwitch1->setEnabled(pSwitch1->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    }

    if(pSwitch2)
    {
        m_pGroupBoxSwitch2->setVisible(true);
        m_pComboBoxSwitch2->addItem(pSwitch2->GetDisplayIcon(), pSwitch2->GetDisplayName(), reinterpret_cast<int>(pSwitch2));
        SlotCurrentSwitchChanged(0, m_pComboBoxSwitch2);
        if(pPortSwitch2Left)
            SetCheckedPort(m_pTreeWidgetPortSwitch2, 0, pPortSwitch2Left);
        if(pPortSwitch2Right)
            SetCheckedPort(m_pTreeWidgetPortSwitch2, 1, pPortSwitch2Right);

        m_pComboBoxSwitch2->setEnabled(pSwitch2->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        m_pTreeWidgetPortSwitch2->setEnabled(pSwitch2->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    }

    if(pSwitch3)
    {
        m_pGroupBoxSwitch3->setVisible(true);
        m_pComboBoxSwitch3->addItem(pSwitch3->GetDisplayIcon(), pSwitch3->GetDisplayName(), reinterpret_cast<int>(pSwitch3));
        SlotCurrentSwitchChanged(0, m_pComboBoxSwitch3);
        if(pPortSwitch3Left)
            SetCheckedPort(m_pTreeWidgetPortSwitch3, 0, pPortSwitch3Left);
        if(pPortSwitch3Right)
            SetCheckedPort(m_pTreeWidgetPortSwitch3, 1, pPortSwitch3Right);

        m_pComboBoxSwitch3->setEnabled(pSwitch3->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        m_pTreeWidgetPortSwitch3->setEnabled(pSwitch3->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    }

    if(pSwitch4)
    {
        m_pGroupBoxSwitch4->setVisible(true);
        m_pComboBoxSwitch4->addItem(pSwitch4->GetDisplayIcon(), pSwitch4->GetDisplayName(), reinterpret_cast<int>(pSwitch4));
        SlotCurrentSwitchChanged(0, m_pComboBoxSwitch4);
        if(pPortSwitch4Left)
            SetCheckedPort(m_pTreeWidgetPortSwitch4, 0, pPortSwitch4Left);
        if(pPortSwitch4Right)
            SetCheckedPort(m_pTreeWidgetPortSwitch4, 1, pPortSwitch4Right);

        m_pComboBoxSwitch4->setEnabled(false);
        m_pTreeWidgetPortSwitch4->setEnabled(pSwitch4->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    }
}

bool InfoSetDlg::GetUiData()
{
    if(!m_pInfoSetForward || !m_pInfoSetBackward)
        return false;

    const bool bHasForwardInfoSet = m_pGroupBoxForward->isChecked();
    const bool bHasBackwardInfoSet = m_pGroupBoxBackward->isChecked();
    if(!bHasForwardInfoSet && !bHasBackwardInfoSet)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Please specify at least one information logic."));
        return false;
    }

    /////////////////////////////////////////
    // Validate Device
    /////////////////////////////////////////
    int iCurrentIndex = m_pComboBoxDeviceForward->currentIndex();
    if(iCurrentIndex < 0)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxDeviceForward->objectName()));
        return false;
    }
    ProjectExplorer::PeDevice *pTxIED = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxDeviceForward->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxDeviceBackward->currentIndex();
    if(iCurrentIndex < 0)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxDeviceBackward->objectName()));

        m_pComboBoxDeviceBackward->setFocus();
        return false;
    }
    ProjectExplorer::PeDevice *pRxIED = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxDeviceBackward->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxSwitch1->currentIndex();
    if(iCurrentIndex < 0 && m_pGroupBoxSwitch1->isVisible())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxSwitch1->objectName()));

        m_pComboBoxSwitch1->setFocus();
        return false;
    }
    ProjectExplorer::PeDevice *pSwitch1 = iCurrentIndex < 0 ? 0 : reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitch1->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxSwitch2->currentIndex();
    if(iCurrentIndex < 0 && m_pGroupBoxSwitch2->isVisible())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxSwitch2->objectName()));

        m_pComboBoxSwitch2->setFocus();
        return false;
    }
    ProjectExplorer::PeDevice *pSwitch2 = iCurrentIndex < 0 ? 0 : reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitch2->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxSwitch3->currentIndex();
    if(iCurrentIndex < 0 && m_pGroupBoxSwitch3->isVisible())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxSwitch3->objectName()));

        m_pComboBoxSwitch3->setFocus();
        return false;
    }
    ProjectExplorer::PeDevice *pSwitch3 = iCurrentIndex < 0 ? 0 : reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitch3->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxSwitch4->currentIndex();
    if(iCurrentIndex < 0 && m_pGroupBoxSwitch4->isVisible())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxSwitch4->objectName()));

        m_pComboBoxSwitch4->setFocus();
        return false;
    }
    ProjectExplorer::PeDevice *pSwitch4 = iCurrentIndex < 0 ? 0 : reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitch4->itemData(iCurrentIndex).toInt());

    /////////////////////////////////////////
    // Validate Port
    /////////////////////////////////////////

    // Forward IED port
    TreeWidgetItemPort *pItemPortForwardIED = GetCheckedItem(m_pTreeWidgetPortForward, 0);
    ProjectExplorer::PePort *pPortForwardTx = pItemPortForwardIED ? pItemPortForwardIED->GetPortTx() : 0;
    ProjectExplorer::PePort *pPortForwardRx = pItemPortForwardIED ? pItemPortForwardIED->GetPortRx() : 0;

    // Forward IED port
    TreeWidgetItemPort *pItemPortBackwardIED = GetCheckedItem(m_pTreeWidgetPortBackward, 0);
    ProjectExplorer::PePort *pPortBackwardTx = pItemPortBackwardIED ? pItemPortBackwardIED->GetPortTx() : 0;
    ProjectExplorer::PePort *pPortBackwardRx = pItemPortBackwardIED ? pItemPortBackwardIED->GetPortRx() : 0;

    // Switch1 port
    TreeWidgetItemPort *pItemPortSwitch1Left = GetCheckedItem(m_pTreeWidgetPortSwitch1, 0);
    ProjectExplorer::PePort *pPortSwitch1LeftTx = pItemPortSwitch1Left ? pItemPortSwitch1Left->GetPortTx() : 0;
    ProjectExplorer::PePort *pPortSwitch1LeftRx = pItemPortSwitch1Left ? pItemPortSwitch1Left->GetPortRx() : 0;

    TreeWidgetItemPort *pItemPortSwitch1Right = GetCheckedItem(m_pTreeWidgetPortSwitch1, 1);
    ProjectExplorer::PePort *pPortSwitch1RightTx = pItemPortSwitch1Right ? pItemPortSwitch1Right->GetPortTx() : 0;
    ProjectExplorer::PePort *pPortSwitch1RightRx = pItemPortSwitch1Right ? pItemPortSwitch1Right->GetPortRx() : 0;

    // Switch2 port
    TreeWidgetItemPort *pItemPortSwitch2Left = GetCheckedItem(m_pTreeWidgetPortSwitch2, 0);
    ProjectExplorer::PePort *pPortSwitch2LeftTx = pItemPortSwitch2Left ? pItemPortSwitch2Left->GetPortTx() : 0;
    ProjectExplorer::PePort *pPortSwitch2LeftRx = pItemPortSwitch2Left ? pItemPortSwitch2Left->GetPortRx() : 0;
    TreeWidgetItemPort *pItemPortSwitch2Right = GetCheckedItem(m_pTreeWidgetPortSwitch2, 1);
    ProjectExplorer::PePort *pPortSwitch2RightTx = pItemPortSwitch2Right ? pItemPortSwitch2Right->GetPortTx() : 0;
    ProjectExplorer::PePort *pPortSwitch2RightRx = pItemPortSwitch2Right ? pItemPortSwitch2Right->GetPortRx() : 0;

    // Switch3 port
    TreeWidgetItemPort *pItemPortSwitch3Left = GetCheckedItem(m_pTreeWidgetPortSwitch3, 0);
    ProjectExplorer::PePort *pPortSwitch3LeftTx = pItemPortSwitch3Left ? pItemPortSwitch3Left->GetPortTx() : 0;
    ProjectExplorer::PePort *pPortSwitch3LeftRx = pItemPortSwitch3Left ? pItemPortSwitch3Left->GetPortRx() : 0;
    TreeWidgetItemPort *pItemPortSwitch3Right = GetCheckedItem(m_pTreeWidgetPortSwitch3, 1);
    ProjectExplorer::PePort *pPortSwitch3RightTx = pItemPortSwitch3Right ? pItemPortSwitch3Right->GetPortTx() : 0;
    ProjectExplorer::PePort *pPortSwitch3RightRx = pItemPortSwitch3Right ? pItemPortSwitch3Right->GetPortRx() : 0;

    // Switch4 port
    TreeWidgetItemPort *pItemPortSwitch4Left = GetCheckedItem(m_pTreeWidgetPortSwitch4, 0);
    ProjectExplorer::PePort *pPortSwitch4LeftTx = pItemPortSwitch4Left ? pItemPortSwitch4Left->GetPortTx() : 0;
    ProjectExplorer::PePort *pPortSwitch4LeftRx = pItemPortSwitch4Left ? pItemPortSwitch4Left->GetPortRx() : 0;
    TreeWidgetItemPort *pItemPortSwitch4Right = GetCheckedItem(m_pTreeWidgetPortSwitch4, 1);
    ProjectExplorer::PePort *pPortSwitch4RightTx = pItemPortSwitch4Right ? pItemPortSwitch4Right->GetPortTx() : 0;
    ProjectExplorer::PePort *pPortSwitch4RightRx = pItemPortSwitch4Right ? pItemPortSwitch4Right->GetPortRx() : 0;

    if(bHasForwardInfoSet)
    {
        if((pItemPortForwardIED && !pPortForwardTx) ||
           (pItemPortBackwardIED && !pPortBackwardRx) ||
           (pItemPortSwitch1Left && !pPortSwitch1LeftRx) ||
           (pItemPortSwitch1Right && !pPortSwitch1RightTx) ||
           (pItemPortSwitch2Left && !pPortSwitch2LeftRx) ||
           (pItemPortSwitch2Right && !pPortSwitch2RightTx) ||
           (pItemPortSwitch3Left && !pPortSwitch3LeftRx) ||
           (pItemPortSwitch3Right && !pPortSwitch3RightTx) ||
           (pItemPortSwitch4Left && !pPortSwitch4LeftRx) ||
           (pItemPortSwitch4Right && !pPortSwitch4RightTx))
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Can not create forward information logic."));
            return false;
        }
    }
    if(bHasBackwardInfoSet)
    {
        if((pItemPortForwardIED && !pPortForwardRx) ||
           (pItemPortBackwardIED && !pPortBackwardTx) ||
           (pItemPortSwitch1Left && !pPortSwitch1LeftTx) ||
           (pItemPortSwitch1Right && !pPortSwitch1RightRx) ||
           (pItemPortSwitch2Left && !pPortSwitch2LeftTx) ||
           (pItemPortSwitch2Right && !pPortSwitch2RightRx) ||
           (pItemPortSwitch3Left && !pPortSwitch3LeftTx) ||
           (pItemPortSwitch3Right && !pPortSwitch3RightRx) ||
           (pItemPortSwitch4Left && !pPortSwitch4LeftTx) ||
           (pItemPortSwitch4Right && !pPortSwitch4RightRx))
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Can not create backward information logic."));
            return false;
        }
    }

    /////////////////////////////////////////
    // For forward infoset
    /////////////////////////////////////////
    m_pInfoSetForward->SetTxIED(pTxIED);
    m_pInfoSetForward->SetTxPort(pPortForwardTx);
    m_pInfoSetForward->SetRxIED(pRxIED);
    m_pInfoSetForward->SetRxPort(pPortBackwardRx);
    m_pInfoSetForward->SetSwitch1(pSwitch1);
    m_pInfoSetForward->SetSwitch1RxPort(pPortSwitch1LeftRx);
    m_pInfoSetForward->SetSwitch1TxPort(pPortSwitch1RightTx);
    m_pInfoSetForward->SetSwitch2(pSwitch2);
    m_pInfoSetForward->SetSwitch2RxPort(pPortSwitch2LeftRx);
    m_pInfoSetForward->SetSwitch2TxPort(pPortSwitch2RightTx);
    m_pInfoSetForward->SetSwitch3(pSwitch3);
    m_pInfoSetForward->SetSwitch3RxPort(pPortSwitch3LeftRx);
    m_pInfoSetForward->SetSwitch3TxPort(pPortSwitch3RightTx);
    m_pInfoSetForward->SetSwitch4(pSwitch4);
    m_pInfoSetForward->SetSwitch4RxPort(pPortSwitch4LeftRx);
    m_pInfoSetForward->SetSwitch4TxPort(pPortSwitch4RightTx);

    if(bHasForwardInfoSet)
    {
        m_pInfoSetForward->SetInfoSetType(ProjectExplorer::PeInfoSet::InfoSetType(m_pComboBoxTypeForward->itemData(m_pComboBoxTypeForward->currentIndex()).toInt()));
        m_pInfoSetForward->SetDescription(m_pLineEditDescForward->text().trimmed());
    }
    else
    {
        m_pInfoSetForward->SetName("");
        m_pInfoSetForward->SetInfoSetType(ProjectExplorer::PeInfoSet::itNone);
        m_pInfoSetForward->SetDescription("");
    }

    /////////////////////////////////////////
    // For backward infoset
    /////////////////////////////////////////
    m_pInfoSetBackward->SetTxIED(pRxIED);
    m_pInfoSetBackward->SetTxPort(pPortBackwardTx);
    m_pInfoSetBackward->SetRxIED(pTxIED);
    m_pInfoSetBackward->SetRxPort(pPortForwardRx);
    m_pInfoSetBackward->SetSwitch1(0);
    m_pInfoSetBackward->SetSwitch1TxPort(0);
    m_pInfoSetBackward->SetSwitch1RxPort(0);
    m_pInfoSetBackward->SetSwitch2(0);
    m_pInfoSetBackward->SetSwitch2TxPort(0);
    m_pInfoSetBackward->SetSwitch2RxPort(0);
    m_pInfoSetBackward->SetSwitch3(0);
    m_pInfoSetBackward->SetSwitch3TxPort(0);
    m_pInfoSetBackward->SetSwitch3RxPort(0);
    m_pInfoSetBackward->SetSwitch4(0);
    m_pInfoSetBackward->SetSwitch4TxPort(0);
    m_pInfoSetBackward->SetSwitch4RxPort(0);
    if(pSwitch4)
    {
        m_pInfoSetBackward->SetSwitch1(pSwitch4);
        m_pInfoSetBackward->SetSwitch1RxPort(pPortSwitch4RightRx);
        m_pInfoSetBackward->SetSwitch1TxPort(pPortSwitch4LeftTx);

        m_pInfoSetBackward->SetSwitch2(pSwitch3);
        m_pInfoSetBackward->SetSwitch2RxPort(pPortSwitch3RightRx);
        m_pInfoSetBackward->SetSwitch2TxPort(pPortSwitch3LeftTx);

        m_pInfoSetBackward->SetSwitch3(pSwitch2);
        m_pInfoSetBackward->SetSwitch3RxPort(pPortSwitch2RightRx);
        m_pInfoSetBackward->SetSwitch3TxPort(pPortSwitch2LeftTx);

        m_pInfoSetBackward->SetSwitch4(pSwitch1);
        m_pInfoSetBackward->SetSwitch4RxPort(pPortSwitch1RightRx);
        m_pInfoSetBackward->SetSwitch4TxPort(pPortSwitch1LeftTx);
    }
    else if(pSwitch3)
    {
        m_pInfoSetBackward->SetSwitch1(pSwitch3);
        m_pInfoSetBackward->SetSwitch1RxPort(pPortSwitch3RightRx);
        m_pInfoSetBackward->SetSwitch1TxPort(pPortSwitch3LeftTx);

        m_pInfoSetBackward->SetSwitch2(pSwitch2);
        m_pInfoSetBackward->SetSwitch2RxPort(pPortSwitch2RightRx);
        m_pInfoSetBackward->SetSwitch2TxPort(pPortSwitch2LeftTx);

        m_pInfoSetBackward->SetSwitch3(pSwitch1);
        m_pInfoSetBackward->SetSwitch3RxPort(pPortSwitch1RightRx);
        m_pInfoSetBackward->SetSwitch3TxPort(pPortSwitch1LeftTx);
    }
    else if(pSwitch2)
    {
        m_pInfoSetBackward->SetSwitch1(pSwitch2);
        m_pInfoSetBackward->SetSwitch1RxPort(pPortSwitch2RightRx);
        m_pInfoSetBackward->SetSwitch1TxPort(pPortSwitch2LeftTx);

        m_pInfoSetBackward->SetSwitch2(pSwitch1);
        m_pInfoSetBackward->SetSwitch2RxPort(pPortSwitch1RightRx);
        m_pInfoSetBackward->SetSwitch2TxPort(pPortSwitch1LeftTx);
    }
    else if(pSwitch1)
    {
        m_pInfoSetBackward->SetSwitch1(pSwitch1);
        m_pInfoSetBackward->SetSwitch1RxPort(pPortSwitch1RightRx);
        m_pInfoSetBackward->SetSwitch1TxPort(pPortSwitch1LeftTx);
    }

    if(bHasBackwardInfoSet)
    {
        m_pInfoSetBackward->SetInfoSetType(ProjectExplorer::PeInfoSet::InfoSetType(m_pComboBoxTypeBackward->itemData(m_pComboBoxTypeBackward->currentIndex()).toInt()));
        m_pInfoSetBackward->SetDescription(m_pLineEditDescBackward->text().trimmed());
    }
    else
    {
        m_pInfoSetBackward->SetName("");
        m_pInfoSetBackward->SetInfoSetType(ProjectExplorer::PeInfoSet::itNone);
        m_pInfoSetBackward->SetDescription("");
    }

    /////////////////////////////////////////
    // Update affected infosets
    /////////////////////////////////////////
    if(m_pCheckBoxUpdateAffectedInfoSets->isChecked())
    {
        if(pPortForwardTx)
            UpdateAffectedInfoSets(m_pInfoSetForward, pPortForwardTx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortBackwardRx)
            UpdateAffectedInfoSets(m_pInfoSetForward, pPortBackwardRx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch1LeftRx)
            UpdateAffectedInfoSets(m_pInfoSetForward, pPortSwitch1LeftRx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch1RightTx)
            UpdateAffectedInfoSets(m_pInfoSetForward, pPortSwitch1RightTx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch2LeftRx)
            UpdateAffectedInfoSets(m_pInfoSetForward, pPortSwitch2LeftRx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch2RightTx)
            UpdateAffectedInfoSets(m_pInfoSetForward, pPortSwitch2RightTx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch3LeftRx)
            UpdateAffectedInfoSets(m_pInfoSetForward, pPortSwitch3LeftRx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch3RightTx)
            UpdateAffectedInfoSets(m_pInfoSetForward, pPortSwitch3RightTx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch4LeftRx)
            UpdateAffectedInfoSets(m_pInfoSetForward, pPortSwitch4LeftRx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch4RightTx)
            UpdateAffectedInfoSets(m_pInfoSetForward, pPortSwitch4RightTx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortForwardRx)
            UpdateAffectedInfoSets(m_pInfoSetBackward, pPortForwardRx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortBackwardTx)
            UpdateAffectedInfoSets(m_pInfoSetBackward, pPortBackwardTx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch1LeftTx)
            UpdateAffectedInfoSets(m_pInfoSetBackward, pPortSwitch1LeftTx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch1RightRx)
            UpdateAffectedInfoSets(m_pInfoSetBackward, pPortSwitch1RightRx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch2LeftTx)
            UpdateAffectedInfoSets(m_pInfoSetBackward, pPortSwitch2LeftTx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch2RightRx)
            UpdateAffectedInfoSets(m_pInfoSetBackward, pPortSwitch2RightRx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch3LeftTx)
            UpdateAffectedInfoSets(m_pInfoSetBackward, pPortSwitch3LeftTx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch3RightRx)
            UpdateAffectedInfoSets(m_pInfoSetBackward, pPortSwitch3RightRx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch4LeftTx)
            UpdateAffectedInfoSets(m_pInfoSetBackward, pPortSwitch4LeftTx, m_lstAffectedInfoSets, m_lstAllInfoSets);
        if(pPortSwitch4RightRx)
            UpdateAffectedInfoSets(m_pInfoSetBackward, pPortSwitch4RightRx, m_lstAffectedInfoSets, m_lstAllInfoSets);
    }

    return true;
}

void InfoSetDlg::accept()
{
    if(GetUiData())
        StyledUi::StyledDialog::accept();
}

void InfoSetDlg::SlotCurrentForwardDeviceChanged(int iIndex)
{
    m_pTreeWidgetPortForward->clear();

    if(iIndex < 0)
        return;

    ProjectExplorer::PeDevice *pDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxDeviceForward->itemData(iIndex).toInt());
    if(!pDevice)
        return;

    QList<ProjectExplorer::PeBoard*> lstBoards = pDevice->GetChildBoards();
    qSort(lstBoards.begin(), lstBoards.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    foreach(ProjectExplorer::PeBoard *pBoard, lstBoards)
    {
        QTreeWidgetItem *pItemBoard = new QTreeWidgetItem;
        pItemBoard->setIcon(0, pBoard->GetDisplayIcon());
        pItemBoard->setText(0, pBoard->GetDisplayName());
        m_pTreeWidgetPortForward->addTopLevelItem(pItemBoard);

        foreach(const PortPair &pari, GetPairPorts(pBoard->GetChildPorts()))
        {
            ProjectExplorer::PePort *pPortTx = pari.first;
            ProjectExplorer::PePort *pPortRx = pari.second;

            TreeWidgetItemPort *pItemPort = new TreeWidgetItemPort(pPortTx, pPortRx, false);
            pItemPort->UpdateConnectionInfo(m_pInfoSetForward->GetProjectVersion()->GetAllInfoSets());
            pItemBoard->addChild(pItemPort);
        }
    }

    m_pTreeWidgetPortForward->expandAll();

    MarkConnectedPorts();
}

void InfoSetDlg::SlotCurrentBackwardDeviceChanged(int iIndex)
{
    m_pTreeWidgetPortBackward->clear();

    if(iIndex < 0)
        return;

    ProjectExplorer::PeDevice *pDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxDeviceBackward->itemData(iIndex).toInt());
    if(!pDevice)
        return;

    QList<ProjectExplorer::PeBoard*> lstBoards = pDevice->GetChildBoards();
    qSort(lstBoards.begin(), lstBoards.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    foreach(ProjectExplorer::PeBoard *pBoard, lstBoards)
    {
        QTreeWidgetItem *pItemBoard = new QTreeWidgetItem;
        pItemBoard->setIcon(0, pBoard->GetDisplayIcon());
        pItemBoard->setText(0, pBoard->GetDisplayName());
        m_pTreeWidgetPortBackward->addTopLevelItem(pItemBoard);

        foreach(const PortPair &pari, GetPairPorts(pBoard->GetChildPorts()))
        {
            ProjectExplorer::PePort *pPortTx = pari.first;
            ProjectExplorer::PePort *pPortRx = pari.second;

            TreeWidgetItemPort *pItemPort = new TreeWidgetItemPort(pPortTx, pPortRx, false);
            pItemPort->UpdateConnectionInfo(m_pInfoSetForward->GetProjectVersion()->GetAllInfoSets());
            pItemBoard->addChild(pItemPort);
        }
    }

    m_pTreeWidgetPortBackward->expandAll();

    MarkConnectedPorts();
}

void InfoSetDlg::SlotCurrentSwitchChanged(int iIndex, QComboBox *pComboBoxSwitch)
{
    if(iIndex < 0)
        return;

    if(!pComboBoxSwitch)
        pComboBoxSwitch = qobject_cast<QComboBox*>(sender());

    QTreeWidget *pTreeWidgetSwitchPort = 0;
    if(pComboBoxSwitch == m_pComboBoxSwitch1)
        pTreeWidgetSwitchPort = m_pTreeWidgetPortSwitch1;
    else if(pComboBoxSwitch == m_pComboBoxSwitch2)
        pTreeWidgetSwitchPort = m_pTreeWidgetPortSwitch2;
    else if(pComboBoxSwitch == m_pComboBoxSwitch3)
        pTreeWidgetSwitchPort = m_pTreeWidgetPortSwitch3;
    else if(pComboBoxSwitch == m_pComboBoxSwitch4)
        pTreeWidgetSwitchPort = m_pTreeWidgetPortSwitch4;

    if(!pComboBoxSwitch || !pTreeWidgetSwitchPort)
        return;

    pTreeWidgetSwitchPort->clear();

    ProjectExplorer::PeDevice *pDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(pComboBoxSwitch->itemData(iIndex).toInt());
    if(!pDevice)
        return;

    QList<ProjectExplorer::PePort*> lstPorts;
    foreach(ProjectExplorer::PeBoard *pBoard, pDevice->GetChildBoards())
        lstPorts.append(pBoard->GetChildPorts());

    foreach(const PortPair &pari, GetPairPorts(lstPorts))
    {
        ProjectExplorer::PePort *pPortTx = pari.first;
        ProjectExplorer::PePort *pPortRx = pari.second;

        TreeWidgetItemPort *pItemPort = new TreeWidgetItemPort(pPortTx, pPortRx, true);
        pItemPort->UpdateConnectionInfo(m_pInfoSetForward->GetProjectVersion()->GetAllInfoSets());
        pTreeWidgetSwitchPort->addTopLevelItem(pItemPort);
    }

    pTreeWidgetSwitchPort->expandAll();
    MarkConnectedPorts();
}
