#include <QApplication>
#include <QVBoxLayout>
#include <QAction>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLineEdit>
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

#include "widgetinfoset.h"

using namespace PhyNetwork::Internal;

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

static ProjectExplorer::PeInfoSet* FindIdenticalInfoSet(ProjectExplorer::PeInfoSet *pInfoSet, const QList<ProjectExplorer::PeInfoSet*> lstInfoSets)
{
    bool bNeedToCheckPath = !(pInfoSet->GetInfoSetType() == ProjectExplorer::PeInfoSet::itNone ||
                              (pInfoSet->GetTxIED() && !pInfoSet->GetTxPort()) ||
                              (pInfoSet->GetRxIED() && !pInfoSet->GetRxPort()) ||
                              (pInfoSet->GetSwitch1() && (!pInfoSet->GetSwitch1TxPort() || !pInfoSet->GetSwitch1RxPort())) ||
                              (pInfoSet->GetSwitch2() && (!pInfoSet->GetSwitch2TxPort() || !pInfoSet->GetSwitch2RxPort())) ||
                              (pInfoSet->GetSwitch3() && (!pInfoSet->GetSwitch3TxPort() || !pInfoSet->GetSwitch3RxPort())) ||
                              (pInfoSet->GetSwitch4() && (!pInfoSet->GetSwitch4TxPort() || !pInfoSet->GetSwitch4RxPort())));
    if(bNeedToCheckPath)
    {
        foreach(ProjectExplorer::PeInfoSet *pInfoSetTemp, lstInfoSets)
        {
            if(pInfoSetTemp->GetId() == pInfoSet->GetId())
                continue;

            if(pInfoSetTemp->GetInfoSetType() == pInfoSet->GetInfoSetType() &&
               pInfoSetTemp->GetTxIED() == pInfoSet->GetTxIED() &&
               pInfoSetTemp->GetTxPort() == pInfoSet->GetTxPort() &&
               pInfoSetTemp->GetRxIED() == pInfoSet->GetRxIED() &&
               pInfoSetTemp->GetRxPort() == pInfoSet->GetRxPort() &&
               pInfoSetTemp->GetSwitch1() == pInfoSet->GetSwitch1() &&
               pInfoSetTemp->GetSwitch1TxPort() == pInfoSet->GetSwitch1TxPort() &&
               pInfoSetTemp->GetSwitch1RxPort() == pInfoSet->GetSwitch1RxPort() &&
               pInfoSetTemp->GetSwitch2() == pInfoSet->GetSwitch2() &&
               pInfoSetTemp->GetSwitch2TxPort() == pInfoSet->GetSwitch2TxPort() &&
               pInfoSetTemp->GetSwitch2RxPort() == pInfoSet->GetSwitch2RxPort() &&
               pInfoSetTemp->GetSwitch3() == pInfoSet->GetSwitch3() &&
               pInfoSetTemp->GetSwitch3TxPort() == pInfoSet->GetSwitch3TxPort() &&
               pInfoSetTemp->GetSwitch3RxPort() == pInfoSet->GetSwitch3RxPort() &&
               pInfoSetTemp->GetSwitch4() == pInfoSet->GetSwitch4() &&
               pInfoSetTemp->GetSwitch4TxPort() == pInfoSet->GetSwitch4TxPort() &&
               pInfoSetTemp->GetSwitch4RxPort() == pInfoSet->GetSwitch4RxPort())
            {
                return pInfoSetTemp;
            }
        }
    }

    return 0;
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

static bool ValidatePortConnection(ProjectExplorer::PeInfoSet *pInfoSet,
                                   ProjectExplorer::PePort *pPort,
                                   QList<ProjectExplorer::PeInfoSet> &lstAffectInfoSets,
                                   const QList<ProjectExplorer::PeInfoSet*> &lstAllInfoSets)
{
    QList<ProjectExplorer::PePort*> lstConnectedPorts;
    QList<ProjectExplorer::PeInfoSet*> lstInfoSetsForPort;
    QList<ProjectExplorer::PeInfoSet*> lstInfoSetsForConnectedPort;

    FindPortConnectionInfo(pPort, lstAllInfoSets, lstConnectedPorts, lstInfoSetsForPort, lstInfoSetsForConnectedPort);
    if(lstConnectedPorts.isEmpty())
        return true;

    if(lstConnectedPorts.size() == 1)
    {
        ProjectExplorer::PePort *pPortConnectedOld = lstConnectedPorts.first();

        ProjectExplorer::PePort *pPortConnectedNew = 0;
        pInfoSet->IsContaintPort(pPort, &pPortConnectedNew);

        if(pPortConnectedOld == pPortConnectedNew)
            return true;

        if(pInfoSet->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        {
            foreach(ProjectExplorer::PeInfoSet *pInfoSetForConnectedPort, lstInfoSetsForConnectedPort)
            {
                if(pInfoSetForConnectedPort->GetId() == pInfoSet->GetId())
                    continue;

                ProjectExplorer::PeInfoSet *pInfoSetAffected = 0;
                for(int i = 0; i < lstAffectInfoSets.size(); i++)
                {
                    ProjectExplorer::PeInfoSet &InfoSetAffected = lstAffectInfoSets[i];
                    if(InfoSetAffected.GetId() == pInfoSetForConnectedPort->GetId())
                    {
                        pInfoSetAffected = &InfoSetAffected;
                        break;
                    }
                }

                if(pInfoSetAffected)
                {
                    pInfoSetAffected->ReplacePort(pPortConnectedOld, 0);//pPortConnectedNew);
                }
                else
                {
                    ProjectExplorer::PeInfoSet InfoSet(*pInfoSetForConnectedPort);
                    InfoSet.ReplacePort(pPortConnectedOld, 0);//pPortConnectedNew);
                    lstAffectInfoSets.append(InfoSet);
                }
            }

            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////
// SelectInfoSetDlg member functions
///////////////////////////////////////////////////////////////////////
SelectInfoSetDlg::SelectInfoSetDlg(const QList<ProjectExplorer::PeInfoSet*> &lstInfoSets, QWidget *pParent)
    : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Select Information Logic"));

    // Create Model
    m_pModel = new QStandardItemModel(0, 3, this);
    m_pModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_pModel->setHeaderData(1, Qt::Horizontal, tr("Send IED"));
    m_pModel->setHeaderData(2, Qt::Horizontal, tr("Receive IED"));

    // Create View
    m_pView = new Utils::ReadOnlyTableView(this);
    m_pView->setAlternatingRowColors(true);
    m_pView->setSortingEnabled(true);
    m_pView->setModel(m_pModel);
    m_pView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pView->horizontalHeader()->setHighlightSections(false);
    m_pView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pView->horizontalHeader()->setStretchLastSection(true);
    m_pView->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pView->verticalHeader()->hide();
    m_pView->setShowGrid(false);
    m_pView->setColumnWidth(0, 275);
    m_pView->setColumnWidth(1, 250);
    m_pView->setColumnWidth(2, 250);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    m_pPushButtonOk = pDialogButtonBox->button(QDialogButtonBox::Ok);

    QVBoxLayout *pVBoxLayout = GetClientLayout();
    pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    pVBoxLayout->addWidget(m_pView);
    pVBoxLayout->addSpacing(15);
    pVBoxLayout->addWidget(pDialogButtonBox);

    foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstInfoSets)
    {
        QList<QStandardItem*> lstItems;

        QStandardItem *pItem = new QStandardItem(pInfoSet->GetDisplayIcon(), pInfoSet->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pInfoSet));
        lstItems.append(pItem);

        QString strTxIED = "";
        if(ProjectExplorer::PeDevice *pTxIED = pInfoSet->GetTxIED())
        {
            strTxIED = pTxIED->GetDisplayName();
            if(ProjectExplorer::PePort *pTxPort = pInfoSet->GetTxPort())
                strTxIED = QString("%1<%2:%3>").arg(strTxIED).arg(pTxPort->GetParentBoard()->GetDisplayName()).arg(pTxPort->GetDisplayName());
        }
        pItem = new QStandardItem(strTxIED);
        pItem->setData(reinterpret_cast<int>(pInfoSet));
        lstItems.append(pItem);

        QString strRxIED = "";
        if(ProjectExplorer::PeDevice *pRxIED = pInfoSet->GetRxIED())
        {
            strRxIED = pRxIED->GetDisplayName();
            if(ProjectExplorer::PePort *pRxPort = pInfoSet->GetRxPort())
                strRxIED = QString("%1<%2:%3>").arg(strRxIED).arg(pRxPort->GetParentBoard()->GetDisplayName()).arg(pRxPort->GetDisplayName());
        }
        pItem = new QStandardItem(strRxIED);
        pItem->setData(reinterpret_cast<int>(pInfoSet));
        lstItems.append(pItem);

        m_pModel->appendRow(lstItems);
    }

    connect(pDialogButtonBox, SIGNAL(accepted()),
            this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()),
            this, SLOT(reject()));
    connect(m_pView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(m_pView, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewDoubleClicked(const QModelIndex&)));

    SlotViewSelectionChanged(QItemSelection(), QItemSelection());
}

ProjectExplorer::PeInfoSet* SelectInfoSetDlg::GetSelectedInfoSet() const
{
    QItemSelectionModel *pSelectionModel = m_pView->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
    if(lstSelectedIndex.isEmpty())
        return 0;

    QStandardItem *pItem = m_pModel->itemFromIndex(lstSelectedIndex.first());
    if(!pItem)
        return 0;

    return reinterpret_cast<ProjectExplorer::PeInfoSet*>(pItem->data().toInt());
}

void SelectInfoSetDlg::SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    QItemSelectionModel *pSelectionModel = m_pView->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();

    m_pPushButtonOk->setEnabled(lstSelectedIndex.size() == 1);
}

void SelectInfoSetDlg::SlotViewDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    accept();
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
// ConfigRow member functions
///////////////////////////////////////////////////////////////////////
WidgetInfoSet::WidgetInfoSet(QWidget *pParent) :
    StyledUi::StyledWidget(pParent), m_pBay(0), m_pInfoSetForward(0), m_pInfoSetBackward(0)
{
    m_pActionCreate = new QAction(QIcon(":/phynetwork/images/oper_add.png"), tr("Create"), this);
    m_pActionCreate->setEnabled(false);
    connect(m_pActionCreate, SIGNAL(triggered()), this, SLOT(SlotActionCreate()));
    m_pActionModify = new QAction(QIcon(":/phynetwork/images/oper_edit.png"), tr("Modify"), this);
    m_pActionModify->setEnabled(false);
    connect(m_pActionModify, SIGNAL(triggered()), this, SLOT(SlotActionModify()));

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

    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->setContentsMargins(5, 10, 5, 5);
    pLayout->addLayout(pHBoxLayoutGeneral);
    pLayout->addSpacing(10);
    pLayout->addLayout(m_pHBoxLayoutDetails);

    connect(m_pComboBoxDeviceForward, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentForwardDeviceChanged(int)));
    connect(m_pComboBoxDeviceBackward, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentBackwardDeviceChanged(int)));
    connect(m_pComboBoxSwitch1, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentSwitchChanged(int)));
    connect(m_pGroupBoxSwitch1, SIGNAL(toggled(bool)),
            this, SLOT(SlotSwitchToggled(bool)));
    connect(m_pComboBoxSwitch2, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentSwitchChanged(int)));
    connect(m_pGroupBoxSwitch2, SIGNAL(toggled(bool)),
            this, SLOT(SlotSwitchToggled(bool)));
    connect(m_pComboBoxSwitch3, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentSwitchChanged(int)));
    connect(m_pGroupBoxSwitch3, SIGNAL(toggled(bool)),
            this, SLOT(SlotSwitchToggled(bool)));
    connect(m_pComboBoxSwitch4, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentSwitchChanged(int)));
    connect(m_pGroupBoxSwitch4, SIGNAL(toggled(bool)),
            this, SLOT(SlotSwitchToggled(bool)));
    connect(m_pTreeWidgetPortForward, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(SlotPortDoubleClicked(QTreeWidgetItem*,int)));
    connect(m_pTreeWidgetPortBackward, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(SlotPortDoubleClicked(QTreeWidgetItem*,int)));
    connect(m_pTreeWidgetPortSwitch1, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(SlotPortDoubleClicked(QTreeWidgetItem*,int)));
    connect(m_pTreeWidgetPortSwitch2, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(SlotPortDoubleClicked(QTreeWidgetItem*,int)));
    connect(m_pTreeWidgetPortSwitch3, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(SlotPortDoubleClicked(QTreeWidgetItem*,int)));
    connect(m_pTreeWidgetPortSwitch4, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(SlotPortDoubleClicked(QTreeWidgetItem*,int)));
}

WidgetInfoSet::~WidgetInfoSet()
{
}

void WidgetInfoSet::SetBay(ProjectExplorer::PeBay *pBay, ProjectExplorer::PeDevice *pDevice)
{
    if(m_pBay != pBay)
    {
        m_pBay = pBay;

        bool bBlock = BlockSignals(true);

        m_pComboBoxDeviceForward->clear();
        m_pComboBoxDeviceBackward->clear();
        m_pComboBoxSwitch1->clear();
        m_pComboBoxSwitch2->clear();
        m_pComboBoxSwitch3->clear();
        m_pComboBoxSwitch4->clear();

        if(m_pBay)
        {
            m_pComboBoxDeviceForward->addItem("", 0);
            m_pComboBoxDeviceBackward->addItem("", 0);
            m_pComboBoxSwitch1->addItem("", 0);
            m_pComboBoxSwitch2->addItem("", 0);
            m_pComboBoxSwitch3->addItem("", 0);
            m_pComboBoxSwitch4->addItem("", 0);

            QList<ProjectExplorer::PeDevice*> lstDevices = m_pBay->GetProjectVersion()->GetAllDevices();
            qSort(lstDevices.begin(), lstDevices.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
            foreach(ProjectExplorer::PeDevice *pDevice, lstDevices)
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                {
                    if(pDevice->GetParentBay() == m_pBay)
                        m_pComboBoxDeviceForward->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));

                    m_pComboBoxDeviceBackward->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));
                }
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                {
                    m_pComboBoxSwitch1->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));
                    m_pComboBoxSwitch2->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));
                    m_pComboBoxSwitch3->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));
                    m_pComboBoxSwitch4->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));
                }
            }
        }

        AdjustComboBoxContentWidth(m_pComboBoxDeviceForward);
        AdjustComboBoxContentWidth(m_pComboBoxDeviceBackward);
        AdjustComboBoxContentWidth(m_pComboBoxSwitch1);
        AdjustComboBoxContentWidth(m_pComboBoxSwitch2);
        AdjustComboBoxContentWidth(m_pComboBoxSwitch3);
        AdjustComboBoxContentWidth(m_pComboBoxSwitch4);

        BlockSignals(bBlock);

        SlotSetInfoSet(0);
    }

    if(pDevice)
    {
        int iIndex = m_pComboBoxDeviceForward->findData(reinterpret_cast<int>(pDevice));
        if(iIndex >= 0)
            m_pComboBoxDeviceForward->setCurrentIndex(iIndex);
    }
}

QList<QAction*> WidgetInfoSet::GetActions() const
{
    return QList<QAction*>() << m_pActionCreate << m_pActionModify;
}

QWidget* WidgetInfoSet::SetupGeneralForward()
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
    QGridLayout *pGridLayout = new QGridLayout(m_pGroupBoxForward);

    pGridLayout->addWidget(new QLabel(tr("Name:"), this), 0, 0);
    pGridLayout->addWidget(m_pLineEditNameForward, 0, 1);
    pGridLayout->addWidget(new QLabel(tr("Type:"), this), 0, 2);
    pGridLayout->addWidget(m_pComboBoxTypeForward, 0, 3);
    pGridLayout->addWidget(new QLabel(tr("Description:"), this), 1, 0);
    pGridLayout->addWidget(m_pLineEditDescForward, 1, 1, 1, 3);

    return m_pGroupBoxForward;
}

QWidget* WidgetInfoSet::SetupGeneralBackward()
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
    QGridLayout *pGridLayout = new QGridLayout(m_pGroupBoxBackward);

    pGridLayout->addWidget(new QLabel(tr("Name:"), this), 0, 0);
    pGridLayout->addWidget(m_pLineEditNameBackward, 0, 1);
    pGridLayout->addWidget(new QLabel(tr("Type:"), this), 0, 2);
    pGridLayout->addWidget(m_pComboBoxTypeBackward, 0, 3);
    pGridLayout->addWidget(new QLabel(tr("Description:"), this), 1, 0);
    pGridLayout->addWidget(m_pLineEditDescBackward, 1, 1, 1, 3);

    return m_pGroupBoxBackward;
}

QWidget* WidgetInfoSet::SetupDeviceForward()
{
    m_pComboBoxDeviceForward = new Utils::SearchComboBox(this);

    m_pTreeWidgetPortForward = new QTreeWidget(this);
    m_pTreeWidgetPortForward->setObjectName("Styled_TreeView");
    m_pTreeWidgetPortForward->setHeaderHidden(true);
    m_pTreeWidgetPortForward->setIndentation(m_pTreeWidgetPortForward->indentation() * 0.7);

    m_pGroupBoxDeviceForward = new QGroupBox(tr("Current Device"), this);
    QVBoxLayout *pVBoxLayoutTxIED = new QVBoxLayout(m_pGroupBoxDeviceForward);

    pVBoxLayoutTxIED->addWidget(m_pComboBoxDeviceForward);
    pVBoxLayoutTxIED->addWidget(m_pTreeWidgetPortForward);

    return m_pGroupBoxDeviceForward;
}

QWidget* WidgetInfoSet::SetupDeviceBackward()
{
    m_pComboBoxDeviceBackward = new Utils::SearchComboBox(this);

    m_pTreeWidgetPortBackward = new QTreeWidget(this);
    m_pTreeWidgetPortBackward->setObjectName("Styled_TreeView");
    m_pTreeWidgetPortBackward->setHeaderHidden(true);
    m_pTreeWidgetPortBackward->setIndentation(m_pTreeWidgetPortBackward->indentation() * 0.7);

    m_pGroupBoxDeviceBackward = new QGroupBox(tr("Side Device"), this);
    QVBoxLayout *pVBoxLayoutRxIED = new QVBoxLayout(m_pGroupBoxDeviceBackward);

    pVBoxLayoutRxIED->addWidget(m_pComboBoxDeviceBackward);
    pVBoxLayoutRxIED->addWidget(m_pTreeWidgetPortBackward);

    return m_pGroupBoxDeviceBackward;
}

QWidget* WidgetInfoSet::SetupSwitch1()
{
    m_pComboBoxSwitch1 = new Utils::SearchComboBox(this);

    m_pTreeWidgetPortSwitch1 = new QTreeWidget(this);
    m_pTreeWidgetPortSwitch1->setObjectName("Styled_TreeView");
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
    m_pGroupBoxSwitch1->setCheckable(true);
    m_pGroupBoxSwitch1->setChecked(false);
    QVBoxLayout *pVBoxLayoutSwitch1 = new QVBoxLayout(m_pGroupBoxSwitch1);

    pVBoxLayoutSwitch1->addWidget(m_pComboBoxSwitch1);
    pVBoxLayoutSwitch1->addWidget(m_pTreeWidgetPortSwitch1);

    return m_pGroupBoxSwitch1;
}

QWidget* WidgetInfoSet::SetupSwitch2()
{
    m_pComboBoxSwitch2 = new Utils::SearchComboBox(this);

    m_pTreeWidgetPortSwitch2 = new QTreeWidget(this);
    m_pTreeWidgetPortSwitch2->setObjectName("Styled_TreeView");
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
    m_pGroupBoxSwitch2->setCheckable(true);
    m_pGroupBoxSwitch2->setChecked(false);
    m_pGroupBoxSwitch2->setVisible(false);
    QVBoxLayout *pVBoxLayoutSwitch2 = new QVBoxLayout(m_pGroupBoxSwitch2);

    pVBoxLayoutSwitch2->addWidget(m_pComboBoxSwitch2);
    pVBoxLayoutSwitch2->addWidget(m_pTreeWidgetPortSwitch2);

    return m_pGroupBoxSwitch2;
}

QWidget* WidgetInfoSet::SetupSwitch3()
{
    m_pComboBoxSwitch3 = new Utils::SearchComboBox(this);

    m_pTreeWidgetPortSwitch3 = new QTreeWidget(this);
    m_pTreeWidgetPortSwitch3->setObjectName("Styled_TreeView");
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
    m_pGroupBoxSwitch3->setCheckable(true);
    m_pGroupBoxSwitch3->setChecked(false);
    m_pGroupBoxSwitch3->setVisible(false);
    QVBoxLayout *pVBoxLayoutSwitch3 = new QVBoxLayout(m_pGroupBoxSwitch3);

    pVBoxLayoutSwitch3->addWidget(m_pComboBoxSwitch3);
    pVBoxLayoutSwitch3->addWidget(m_pTreeWidgetPortSwitch3);

    return m_pGroupBoxSwitch3;
}

QWidget* WidgetInfoSet::SetupSwitch4()
{
    m_pComboBoxSwitch4 = new Utils::SearchComboBox(this);

    m_pTreeWidgetPortSwitch4 = new QTreeWidget(this);
    m_pTreeWidgetPortSwitch4->setObjectName("Styled_TreeView");
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
    m_pGroupBoxSwitch4->setCheckable(true);
    m_pGroupBoxSwitch4->setChecked(false);
    m_pGroupBoxSwitch4->setVisible(false);
    QVBoxLayout *pVBoxLayoutSwitch4 = new QVBoxLayout(m_pGroupBoxSwitch4);

    pVBoxLayoutSwitch4->addWidget(m_pComboBoxSwitch4);
    pVBoxLayoutSwitch4->addWidget(m_pTreeWidgetPortSwitch4);

    return m_pGroupBoxSwitch4;
}

TreeWidgetItemPort* WidgetInfoSet::GetCheckedItem(QTreeWidget *pTreeWidget, int iColumn)
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

bool WidgetInfoSet::SetCheckedPort(QTreeWidget *pTreeWidget, int iColumn, ProjectExplorer::PePort *pPort)
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

void WidgetInfoSet::MarkConnectedPorts()
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

void WidgetInfoSet::AdjustComboBoxContentWidth(QComboBox *pComboBox)
{
    pComboBox->view()->setFixedWidth(qMax(pComboBox->width(), pComboBox->view()->sizeHint().width()));
}

bool WidgetInfoSet::BlockSignals(bool bBlock)
{
    return m_pComboBoxDeviceForward->blockSignals(bBlock) &&
           m_pComboBoxDeviceBackward->blockSignals(bBlock) &&
           m_pComboBoxSwitch1->blockSignals(bBlock) &&
           m_pGroupBoxSwitch1->blockSignals(bBlock) &&
           m_pComboBoxSwitch2->blockSignals(bBlock) &&
           m_pGroupBoxSwitch2->blockSignals(bBlock) &&
           m_pComboBoxSwitch3->blockSignals(bBlock) &&
           m_pGroupBoxSwitch3->blockSignals(bBlock) &&
           m_pComboBoxSwitch4->blockSignals(bBlock) &&
           m_pGroupBoxSwitch4->blockSignals(bBlock) &&
           m_pTreeWidgetPortForward->blockSignals(bBlock) &&
           m_pTreeWidgetPortBackward->blockSignals(bBlock) &&
           m_pTreeWidgetPortSwitch1->blockSignals(bBlock) &&
           m_pTreeWidgetPortSwitch2->blockSignals(bBlock) &&
           m_pTreeWidgetPortSwitch3->blockSignals(bBlock) &&
           m_pTreeWidgetPortSwitch4->blockSignals(bBlock);
}

void WidgetInfoSet::SetUiData(ProjectExplorer::PeInfoSet *pInfoSetForward, ProjectExplorer::PeInfoSet *pInfoSetBackward)
{
    if(!pInfoSetForward || !pInfoSetBackward)
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
    if(pInfoSetForward->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
    {
        m_pGroupBoxForward->setChecked(true);

        m_pLineEditNameForward->setText(pInfoSetForward->GetName());
        m_pLineEditDescForward->setText(pInfoSetForward->GetDescription());
        int iCurrentIndex = m_pComboBoxTypeForward->findData(pInfoSetForward->GetInfoSetType());
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
    if(pInfoSetBackward->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
    {
        m_pGroupBoxBackward->setChecked(true);

        m_pLineEditNameBackward->setText(pInfoSetBackward->GetName());
        m_pLineEditDescBackward->setText(pInfoSetBackward->GetDescription());
        int iCurrentIndex = m_pComboBoxTypeBackward->findData(pInfoSetBackward->GetInfoSetType());
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

    if(pInfoSetForward->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
    {
        pIEDForward = pInfoSetForward->GetTxIED();
        pPortIEDForward = pInfoSetForward->GetTxPort();
        pIEDBackward = pInfoSetForward->GetRxIED();
        pPortIEDBackward = pInfoSetForward->GetRxPort();
        pSwitch1 = pInfoSetForward->GetSwitch1();
        pPortSwitch1Left = pInfoSetForward->GetSwitch1RxPort();
        pPortSwitch1Right = pInfoSetForward->GetSwitch1TxPort();
        pSwitch2 = pInfoSetForward->GetSwitch2();
        pPortSwitch2Left = pInfoSetForward->GetSwitch2RxPort();
        pPortSwitch2Right = pInfoSetForward->GetSwitch2TxPort();
        pSwitch3 = pInfoSetForward->GetSwitch3();
        pPortSwitch3Left = pInfoSetForward->GetSwitch3RxPort();
        pPortSwitch3Right = pInfoSetForward->GetSwitch3TxPort();
        pSwitch4 = pInfoSetForward->GetSwitch4();
        pPortSwitch4Left = pInfoSetForward->GetSwitch4RxPort();
        pPortSwitch4Right = pInfoSetForward->GetSwitch4TxPort();
    }
    else if(pInfoSetBackward->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
    {
        pIEDForward = pInfoSetBackward->GetRxIED();
        pPortIEDForward = pInfoSetBackward->GetRxPort();
        pIEDBackward = pInfoSetBackward->GetTxIED();
        pPortIEDBackward = pInfoSetBackward->GetTxPort();

        if(pInfoSetBackward->GetSwitch4())
        {
            pSwitch1 = pInfoSetBackward->GetSwitch4();
            pPortSwitch1Left = pInfoSetBackward->GetSwitch4TxPort();
            pPortSwitch1Right = pInfoSetBackward->GetSwitch4RxPort();
            pSwitch2 = pInfoSetBackward->GetSwitch3();
            pPortSwitch2Left = pInfoSetBackward->GetSwitch3TxPort();
            pPortSwitch2Right = pInfoSetBackward->GetSwitch3RxPort();
            pSwitch3 = pInfoSetBackward->GetSwitch2();
            pPortSwitch3Left = pInfoSetBackward->GetSwitch2TxPort();
            pPortSwitch3Right = pInfoSetBackward->GetSwitch2RxPort();
            pSwitch4 = pInfoSetBackward->GetSwitch1();
            pPortSwitch4Left = pInfoSetBackward->GetSwitch1TxPort();
            pPortSwitch4Right = pInfoSetBackward->GetSwitch1RxPort();
        }
        else if(pInfoSetBackward->GetSwitch3())
        {
            pSwitch1 = pInfoSetBackward->GetSwitch3();
            pPortSwitch1Left = pInfoSetBackward->GetSwitch3TxPort();
            pPortSwitch1Right = pInfoSetBackward->GetSwitch3RxPort();
            pSwitch2 = pInfoSetBackward->GetSwitch2();
            pPortSwitch2Left = pInfoSetBackward->GetSwitch2TxPort();
            pPortSwitch2Right = pInfoSetBackward->GetSwitch2RxPort();
            pSwitch3 = pInfoSetBackward->GetSwitch1();
            pPortSwitch3Left = pInfoSetBackward->GetSwitch1TxPort();
            pPortSwitch3Right = pInfoSetBackward->GetSwitch1RxPort();
        }
        else if(pInfoSetBackward->GetSwitch2())
        {
            pSwitch1 = pInfoSetBackward->GetSwitch2();
            pPortSwitch1Left = pInfoSetBackward->GetSwitch2TxPort();
            pPortSwitch1Right = pInfoSetBackward->GetSwitch2RxPort();
            pSwitch2 = pInfoSetBackward->GetSwitch1();
            pPortSwitch2Left = pInfoSetBackward->GetSwitch1TxPort();
            pPortSwitch2Right = pInfoSetBackward->GetSwitch1RxPort();
        }
        else if(pInfoSetBackward->GetSwitch1())
        {
            pSwitch1 = pInfoSetBackward->GetSwitch1();
            pPortSwitch1Left = pInfoSetBackward->GetSwitch1TxPort();
            pPortSwitch1Right = pInfoSetBackward->GetSwitch1RxPort();
        }
    }

    int iCurrentIndex = m_pComboBoxDeviceForward->findData(reinterpret_cast<int>(pIEDForward));
    if(iCurrentIndex > 0)
    {
        m_pComboBoxDeviceForward->setCurrentIndex(iCurrentIndex);
        SlotCurrentForwardDeviceChanged(iCurrentIndex);
        if(pPortIEDForward)
            SetCheckedPort(m_pTreeWidgetPortForward, 0, pPortIEDForward);
    }
    else
    {
        m_pComboBoxDeviceForward->setCurrentIndex(0);
    }

    iCurrentIndex = m_pComboBoxDeviceBackward->findData(reinterpret_cast<int>(pIEDBackward));
    if(iCurrentIndex > 0)
    {
        m_pComboBoxDeviceBackward->setCurrentIndex(iCurrentIndex);
        SlotCurrentBackwardDeviceChanged(iCurrentIndex);
        if(pPortIEDBackward)
            SetCheckedPort(m_pTreeWidgetPortBackward, 0, pPortIEDBackward);
    }
    else
    {
        m_pComboBoxDeviceBackward->setCurrentIndex(0);
    }

    iCurrentIndex = m_pComboBoxSwitch1->findData(reinterpret_cast<int>(pSwitch1));
    if(iCurrentIndex > 0)
    {
        m_pGroupBoxSwitch1->setChecked(true);
        m_pComboBoxSwitch1->setCurrentIndex(iCurrentIndex);
        SlotCurrentSwitchChanged(iCurrentIndex, m_pComboBoxSwitch1);
        if(pPortSwitch1Left)
            SetCheckedPort(m_pTreeWidgetPortSwitch1, 0, pPortSwitch1Left);
        if(pPortSwitch1Right)
            SetCheckedPort(m_pTreeWidgetPortSwitch1, 1, pPortSwitch1Right);
    }
    else
    {
        m_pGroupBoxSwitch1->setChecked(false);
        m_pComboBoxSwitch1->setCurrentIndex(0);
    }

    iCurrentIndex = m_pComboBoxSwitch2->findData(reinterpret_cast<int>(pSwitch2));
    if(iCurrentIndex > 0)
    {
        m_pGroupBoxSwitch2->setChecked(true);
        m_pComboBoxSwitch2->setCurrentIndex(iCurrentIndex);
        SlotCurrentSwitchChanged(iCurrentIndex, m_pComboBoxSwitch2);
        if(pPortSwitch2Left)
            SetCheckedPort(m_pTreeWidgetPortSwitch2, 0, pPortSwitch2Left);
        if(pPortSwitch2Right)
            SetCheckedPort(m_pTreeWidgetPortSwitch2, 1, pPortSwitch2Right);
    }
    else
    {
        m_pGroupBoxSwitch2->setChecked(false);
        m_pComboBoxSwitch2->setCurrentIndex(0);
    }

    iCurrentIndex = m_pComboBoxSwitch3->findData(reinterpret_cast<int>(pSwitch3));
    if(iCurrentIndex > 0)
    {
        m_pGroupBoxSwitch3->setChecked(true);
        m_pComboBoxSwitch3->setCurrentIndex(iCurrentIndex);
        SlotCurrentSwitchChanged(iCurrentIndex, m_pComboBoxSwitch3);
        if(pPortSwitch3Left)
            SetCheckedPort(m_pTreeWidgetPortSwitch3, 0, pPortSwitch3Left);
        if(pPortSwitch3Right)
            SetCheckedPort(m_pTreeWidgetPortSwitch3, 1, pPortSwitch3Right);
    }
    else
    {
        m_pGroupBoxSwitch3->setChecked(false);
        m_pComboBoxSwitch3->setCurrentIndex(0);
    }

    iCurrentIndex = m_pComboBoxSwitch4->findData(reinterpret_cast<int>(pSwitch4));
    if(iCurrentIndex > 0)
    {
        m_pGroupBoxSwitch4->setChecked(true);
        m_pComboBoxSwitch4->setCurrentIndex(iCurrentIndex);
        SlotCurrentSwitchChanged(iCurrentIndex, m_pComboBoxSwitch4);
        if(pPortSwitch4Left)
            SetCheckedPort(m_pTreeWidgetPortSwitch4, 0, pPortSwitch4Left);
        if(pPortSwitch4Right)
            SetCheckedPort(m_pTreeWidgetPortSwitch4, 1, pPortSwitch4Right);
    }
    else
    {
        m_pGroupBoxSwitch4->setChecked(false);
        m_pComboBoxSwitch4->setCurrentIndex(0);
    }
}

bool WidgetInfoSet::GetUiData(ProjectExplorer::PeInfoSet *pInfoSetForward, ProjectExplorer::PeInfoSet *pInfoSetBackward, QList<ProjectExplorer::PeInfoSet> &lstAffectInfoSets)
{
    if(!pInfoSetForward || !pInfoSetBackward)
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
    if(iCurrentIndex <= 0)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxDeviceForward->objectName()));
        return false;
    }
    ProjectExplorer::PeDevice *pTxIED = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxDeviceForward->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxDeviceBackward->currentIndex();
    if(iCurrentIndex <= 0)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxDeviceBackward->objectName()));

        m_pComboBoxDeviceBackward->setFocus();
        return false;
    }
    ProjectExplorer::PeDevice *pRxIED = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxDeviceBackward->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxSwitch1->currentIndex();
    if(iCurrentIndex <= 0 && m_pGroupBoxSwitch1->isChecked())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxSwitch1->objectName()));

        m_pComboBoxSwitch1->setFocus();
        return false;
    }
    ProjectExplorer::PeDevice *pSwitch1 = iCurrentIndex <= 0 ? 0 : reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitch1->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxSwitch2->currentIndex();
    if(iCurrentIndex <= 0 && m_pGroupBoxSwitch2->isChecked())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxSwitch2->objectName()));

        m_pComboBoxSwitch2->setFocus();
        return false;
    }
    ProjectExplorer::PeDevice *pSwitch2 = iCurrentIndex <= 0 ? 0 : reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitch2->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxSwitch3->currentIndex();
    if(iCurrentIndex <= 0 && m_pGroupBoxSwitch3->isChecked())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxSwitch3->objectName()));

        m_pComboBoxSwitch3->setFocus();
        return false;
    }
    ProjectExplorer::PeDevice *pSwitch3 = iCurrentIndex <= 0 ? 0 : reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitch3->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxSwitch4->currentIndex();
    if(iCurrentIndex <= 0 && m_pGroupBoxSwitch4->isChecked())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxSwitch4->objectName()));

        m_pComboBoxSwitch4->setFocus();
        return false;
    }
    ProjectExplorer::PeDevice *pSwitch4 = iCurrentIndex <= 0 ? 0 : reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitch4->itemData(iCurrentIndex).toInt());

    /////////////////////////////////////////
    // Validate Port
    /////////////////////////////////////////
    const QList<ProjectExplorer::PeInfoSet*> lstAllInfoSets = m_pBay->GetProjectVersion()->GetAllInfoSets();

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
    pInfoSetForward->SetTxIED(pTxIED);
    pInfoSetForward->SetTxPort(pPortForwardTx);
    pInfoSetForward->SetRxIED(pRxIED);
    pInfoSetForward->SetRxPort(pPortBackwardRx);
    pInfoSetForward->SetSwitch1(pSwitch1);
    pInfoSetForward->SetSwitch1RxPort(pPortSwitch1LeftRx);
    pInfoSetForward->SetSwitch1TxPort(pPortSwitch1RightTx);
    pInfoSetForward->SetSwitch2(pSwitch2);
    pInfoSetForward->SetSwitch2RxPort(pPortSwitch2LeftRx);
    pInfoSetForward->SetSwitch2TxPort(pPortSwitch2RightTx);
    pInfoSetForward->SetSwitch3(pSwitch3);
    pInfoSetForward->SetSwitch3RxPort(pPortSwitch3LeftRx);
    pInfoSetForward->SetSwitch3TxPort(pPortSwitch3RightTx);
    pInfoSetForward->SetSwitch4(pSwitch4);
    pInfoSetForward->SetSwitch4RxPort(pPortSwitch4LeftRx);
    pInfoSetForward->SetSwitch4TxPort(pPortSwitch4RightTx);

    if(bHasForwardInfoSet)
    {
        pInfoSetForward->SetInfoSetType(ProjectExplorer::PeInfoSet::InfoSetType(m_pComboBoxTypeForward->itemData(m_pComboBoxTypeForward->currentIndex()).toInt()));
        pInfoSetForward->SetDescription(m_pLineEditDescForward->text().trimmed());

        if(ProjectExplorer::PeInfoSet *pInfoSetFound = FindIdenticalInfoSet(pInfoSetForward, lstAllInfoSets))
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The path of forward information logic is identical to the existed information logic '%1'").arg(pInfoSetFound->GetName()));

            return false;
        }

        pInfoSetForward->UpdateName();
        if(pInfoSetForward->GetName().isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Can not calculate the name forward information logic."));
            return false;
        }
    }
    else
    {
        pInfoSetForward->SetName("");
        pInfoSetForward->SetInfoSetType(ProjectExplorer::PeInfoSet::itNone);
        pInfoSetForward->SetDescription("");
    }

    /////////////////////////////////////////
    // For backward infoset
    /////////////////////////////////////////
    pInfoSetBackward->SetTxIED(pRxIED);
    pInfoSetBackward->SetTxPort(pPortBackwardTx);
    pInfoSetBackward->SetRxIED(pTxIED);
    pInfoSetBackward->SetRxPort(pPortForwardRx);
    pInfoSetBackward->SetSwitch1(0);
    pInfoSetBackward->SetSwitch1TxPort(0);
    pInfoSetBackward->SetSwitch1RxPort(0);
    pInfoSetBackward->SetSwitch2(0);
    pInfoSetBackward->SetSwitch2TxPort(0);
    pInfoSetBackward->SetSwitch2RxPort(0);
    pInfoSetBackward->SetSwitch3(0);
    pInfoSetBackward->SetSwitch3TxPort(0);
    pInfoSetBackward->SetSwitch3RxPort(0);
    pInfoSetBackward->SetSwitch4(0);
    pInfoSetBackward->SetSwitch4TxPort(0);
    pInfoSetBackward->SetSwitch4RxPort(0);
    if(pSwitch4)
    {
        pInfoSetBackward->SetSwitch1(pSwitch4);
        pInfoSetBackward->SetSwitch1RxPort(pPortSwitch4RightRx);
        pInfoSetBackward->SetSwitch1TxPort(pPortSwitch4LeftTx);

        pInfoSetBackward->SetSwitch2(pSwitch3);
        pInfoSetBackward->SetSwitch2RxPort(pPortSwitch3RightRx);
        pInfoSetBackward->SetSwitch2TxPort(pPortSwitch3LeftTx);

        pInfoSetBackward->SetSwitch3(pSwitch2);
        pInfoSetBackward->SetSwitch3RxPort(pPortSwitch2RightRx);
        pInfoSetBackward->SetSwitch3TxPort(pPortSwitch2LeftTx);

        pInfoSetBackward->SetSwitch4(pSwitch1);
        pInfoSetBackward->SetSwitch4RxPort(pPortSwitch1RightRx);
        pInfoSetBackward->SetSwitch4TxPort(pPortSwitch1LeftTx);
    }
    else if(pSwitch3)
    {
        pInfoSetBackward->SetSwitch1(pSwitch3);
        pInfoSetBackward->SetSwitch1RxPort(pPortSwitch3RightRx);
        pInfoSetBackward->SetSwitch1TxPort(pPortSwitch3LeftTx);

        pInfoSetBackward->SetSwitch2(pSwitch2);
        pInfoSetBackward->SetSwitch2RxPort(pPortSwitch2RightRx);
        pInfoSetBackward->SetSwitch2TxPort(pPortSwitch2LeftTx);

        pInfoSetBackward->SetSwitch3(pSwitch1);
        pInfoSetBackward->SetSwitch3RxPort(pPortSwitch1RightRx);
        pInfoSetBackward->SetSwitch3TxPort(pPortSwitch1LeftTx);
    }
    else if(pSwitch2)
    {
        pInfoSetBackward->SetSwitch1(pSwitch2);
        pInfoSetBackward->SetSwitch1RxPort(pPortSwitch2RightRx);
        pInfoSetBackward->SetSwitch1TxPort(pPortSwitch2LeftTx);

        pInfoSetBackward->SetSwitch2(pSwitch1);
        pInfoSetBackward->SetSwitch2RxPort(pPortSwitch1RightRx);
        pInfoSetBackward->SetSwitch2TxPort(pPortSwitch1LeftTx);
    }
    else if(pSwitch1)
    {
        pInfoSetBackward->SetSwitch1(pSwitch1);
        pInfoSetBackward->SetSwitch1RxPort(pPortSwitch1RightRx);
        pInfoSetBackward->SetSwitch1TxPort(pPortSwitch1LeftTx);
    }

    if(bHasBackwardInfoSet)
    {
        pInfoSetBackward->SetInfoSetType(ProjectExplorer::PeInfoSet::InfoSetType(m_pComboBoxTypeBackward->itemData(m_pComboBoxTypeBackward->currentIndex()).toInt()));
        pInfoSetBackward->SetDescription(m_pLineEditDescBackward->text().trimmed());

        if(ProjectExplorer::PeInfoSet *pInfoSetFound = FindIdenticalInfoSet(pInfoSetBackward, lstAllInfoSets))
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The path of backward information logic is identical to the existed information logic '%1'").arg(pInfoSetFound->GetName()));

            return false;
        }

        pInfoSetBackward->UpdateName(QStringList() << pInfoSetForward->GetName());
        if(pInfoSetBackward->GetName().isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Can not calculate the name backward information logic."));
            return false;
        }
    }
    else
    {
        pInfoSetBackward->SetName("");
        pInfoSetBackward->SetInfoSetType(ProjectExplorer::PeInfoSet::itNone);
        pInfoSetBackward->SetDescription("");
    }

    /////////////////////////////////////////
    // Validate port connection
    /////////////////////////////////////////
    ProjectExplorer::PePort *pPortError = 0;
    if(pPortForwardTx && !ValidatePortConnection(pInfoSetForward, pPortForwardTx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortForwardTx;
    else if(pPortBackwardRx && !ValidatePortConnection(pInfoSetForward, pPortBackwardRx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortBackwardRx;
    else if(pPortSwitch1LeftRx && !ValidatePortConnection(pInfoSetForward, pPortSwitch1LeftRx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch1LeftRx;
    else if(pPortSwitch1RightTx && !ValidatePortConnection(pInfoSetForward, pPortSwitch1RightTx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch1RightTx;
    else if(pPortSwitch2LeftRx && !ValidatePortConnection(pInfoSetForward, pPortSwitch2LeftRx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch2LeftRx;
    else if(pPortSwitch2RightTx && !ValidatePortConnection(pInfoSetForward, pPortSwitch2RightTx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch2RightTx;
    else if(pPortSwitch3LeftRx && !ValidatePortConnection(pInfoSetForward, pPortSwitch3LeftRx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch3LeftRx;
    else if(pPortSwitch3RightTx && !ValidatePortConnection(pInfoSetForward, pPortSwitch3RightTx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch3RightTx;
    else if(pPortSwitch4LeftRx && !ValidatePortConnection(pInfoSetForward, pPortSwitch4LeftRx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch4LeftRx;
    else if(pPortSwitch4RightTx && !ValidatePortConnection(pInfoSetForward, pPortSwitch4RightTx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch4RightTx;
    else if(pPortForwardRx && !ValidatePortConnection(pInfoSetBackward, pPortForwardRx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortForwardRx;
    else if(pPortBackwardTx && !ValidatePortConnection(pInfoSetBackward, pPortBackwardTx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortBackwardTx;
    else if(pPortSwitch1LeftTx && !ValidatePortConnection(pInfoSetBackward, pPortSwitch1LeftTx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch1LeftTx;
    else if(pPortSwitch1RightRx && !ValidatePortConnection(pInfoSetBackward, pPortSwitch1RightRx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch1RightRx;
    else if(pPortSwitch2LeftTx && !ValidatePortConnection(pInfoSetBackward, pPortSwitch2LeftTx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch2LeftTx;
    else if(pPortSwitch2RightRx && !ValidatePortConnection(pInfoSetBackward, pPortSwitch2RightRx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch2RightRx;
    else if(pPortSwitch3LeftTx && !ValidatePortConnection(pInfoSetBackward, pPortSwitch3LeftTx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch3LeftTx;
    else if(pPortSwitch3RightRx && !ValidatePortConnection(pInfoSetBackward, pPortSwitch3RightRx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch3RightRx;
    else if(pPortSwitch4LeftTx && !ValidatePortConnection(pInfoSetBackward, pPortSwitch4LeftTx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch4LeftTx;
    else if(pPortSwitch4RightRx && !ValidatePortConnection(pInfoSetBackward, pPortSwitch4RightRx, lstAffectInfoSets, lstAllInfoSets))
        pPortError = pPortSwitch4RightRx;

    if(pPortError)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The port '%1' has connected to a different port in other information logic.").arg(GetPortPath(pPortError, 0)));
        return false;
    }

    QString strAffctedInfoSets;
    foreach(ProjectExplorer::PeInfoSet InfoSet, lstAffectInfoSets)
    {
        if(InfoSet.GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
            strAffctedInfoSets = QString("%1\n%2").arg(strAffctedInfoSets).arg(InfoSet.GetDisplayName());
    }

    if(!strAffctedInfoSets.isEmpty())
    {
        if(QMessageBox::question(this,
                                 tr("Modify Confirmation"),
                                 tr("The following information logic will be affected, are you sure you want to modify the current information logic?") + "\n" + strAffctedInfoSets,
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No) == QMessageBox::No)
        {
            return false;
        }
    }

    return true;
}

void WidgetInfoSet::SlotSetInfoSet(ProjectExplorer::PeInfoSet *pInfoSet)
{
    Utils::WaitCursor waitcursor;
    Q_UNUSED(waitcursor)

    m_pInfoSetForward = m_pInfoSetBackward = 0;
    if(ProjectExplorer::PeInfoSet *pInfoSetPair = (pInfoSet ? pInfoSet->GetPairInfoSet() : 0))
    {
        if(pInfoSet->GetInfoSetType() == ProjectExplorer::PeInfoSet::itNone)
        {
            ProjectExplorer::PeInfoSet *pInfoSetTemp = pInfoSet;
            pInfoSet = pInfoSetPair;
            pInfoSetPair = pInfoSetTemp;
        }

        ProjectExplorer::PeDevice *pTxIED = pInfoSet->GetTxIED();
        ProjectExplorer::PeDevice *pRxIED = pInfoSet->GetRxIED();
        if(pTxIED && pRxIED)
        {
            if(pTxIED->GetParentBay() == m_pBay)
            {
                m_pInfoSetForward = pInfoSet;
                m_pInfoSetBackward = pInfoSetPair;
            }
            else if(pRxIED->GetParentBay() == m_pBay)
            {
                m_pInfoSetForward = pInfoSetPair;
                m_pInfoSetBackward = pInfoSet;
            }
            else if(pTxIED->GetParentBay())
            {
                SetBay(pTxIED->GetParentBay(), 0);
                m_pInfoSetForward = pInfoSet;
                m_pInfoSetBackward = pInfoSetPair;
            }
            else if(pRxIED->GetParentBay())
            {
                SetBay(pRxIED->GetParentBay(), 0);
                m_pInfoSetForward = pInfoSetPair;
                m_pInfoSetBackward = pInfoSet;
            }
        }
    }

    bool bBlock = BlockSignals(true);
    SetUiData(m_pInfoSetForward, m_pInfoSetBackward);
    BlockSignals(bBlock);

    m_pActionCreate->setEnabled(m_pBay);
    m_pActionModify->setEnabled(m_pInfoSetForward && m_pInfoSetBackward);
}

void WidgetInfoSet::SlotCurrentForwardDeviceChanged(int iIndex)
{
    m_pTreeWidgetPortForward->clear();

    if(iIndex <= 0)
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
            pItemPort->UpdateConnectionInfo(m_pBay->GetProjectVersion()->GetAllInfoSets());
            pItemBoard->addChild(pItemPort);
        }
    }

    m_pTreeWidgetPortForward->expandAll();

    MarkConnectedPorts();
}

void WidgetInfoSet::SlotCurrentBackwardDeviceChanged(int iIndex)
{
    m_pTreeWidgetPortBackward->clear();

    if(iIndex <= 0)
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
            pItemPort->UpdateConnectionInfo(m_pBay->GetProjectVersion()->GetAllInfoSets());
            pItemBoard->addChild(pItemPort);
        }
    }

    m_pTreeWidgetPortBackward->expandAll();

    MarkConnectedPorts();
}

void WidgetInfoSet::SlotCurrentSwitchChanged(int iIndex, QComboBox *pComboBoxSwitch)
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
        pItemPort->UpdateConnectionInfo(m_pBay->GetProjectVersion()->GetAllInfoSets());
        pTreeWidgetSwitchPort->addTopLevelItem(pItemPort);
    }

    pTreeWidgetSwitchPort->expandAll();
    MarkConnectedPorts();
}

void WidgetInfoSet::SlotSwitchToggled(bool bToggled)
{
    Q_UNUSED(bToggled)

    if(!m_pGroupBoxSwitch1->isChecked())
        m_pComboBoxSwitch1->setCurrentIndex(0);
    if(!m_pGroupBoxSwitch2->isChecked())
        m_pComboBoxSwitch2->setCurrentIndex(0);
    if(!m_pGroupBoxSwitch3->isChecked())
        m_pComboBoxSwitch3->setCurrentIndex(0);
    if(!m_pGroupBoxSwitch4->isChecked())
        m_pComboBoxSwitch4->setCurrentIndex(0);

    m_pGroupBoxSwitch2->setVisible(m_pGroupBoxSwitch1->isChecked());
    if(!m_pGroupBoxSwitch2->isVisible())
    {
        m_pGroupBoxSwitch2->setChecked(false);
        m_pGroupBoxSwitch3->setChecked(false);
        m_pGroupBoxSwitch4->setChecked(false);
    }

    m_pGroupBoxSwitch3->setVisible(m_pGroupBoxSwitch1->isChecked() && m_pGroupBoxSwitch2->isChecked());
    if(!m_pGroupBoxSwitch3->isVisible())
    {
        m_pGroupBoxSwitch3->setChecked(false);
        m_pGroupBoxSwitch4->setChecked(false);
    }

    m_pGroupBoxSwitch4->setVisible(m_pGroupBoxSwitch1->isChecked() && m_pGroupBoxSwitch2->isChecked() && m_pGroupBoxSwitch3->isChecked());
    if(!m_pGroupBoxSwitch4->isVisible())
    {
        m_pGroupBoxSwitch4->setChecked(false);
    }

    MarkConnectedPorts();
}

void WidgetInfoSet::SlotPortDoubleClicked(QTreeWidgetItem *pItem, int iColumn)
{
    Q_UNUSED(iColumn)

    TreeWidgetItemPort *pItemPort = dynamic_cast<TreeWidgetItemPort*>(pItem);
    if(!pItemPort)
        return;

    QList<ProjectExplorer::PeInfoSet*> lstInfoSets = pItemPort->GetInfoSets();
    lstInfoSets.removeAll(m_pInfoSetForward);
    lstInfoSets.removeAll(m_pInfoSetBackward);
    if(lstInfoSets.isEmpty())
        return;

    ProjectExplorer::PeInfoSet *pInfoSet = 0;
    if(lstInfoSets.size() == 1)
    {
        pInfoSet = lstInfoSets.first();
    }
    else
    {
        SelectInfoSetDlg dlg(lstInfoSets, this);
        if(dlg.exec() == QDialog::Accepted)
            pInfoSet = dlg.GetSelectedInfoSet();
    }

    if(pInfoSet)
        SlotSetInfoSet(pInfoSet);
}

void WidgetInfoSet::SlotActionModify()
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pBay->GetProjectVersion();
    if(!pProjectVersion)
        return;

    if(!m_pInfoSetForward || !m_pInfoSetBackward)
        return;

    ProjectExplorer::PeInfoSet InfoSetForward(*m_pInfoSetForward);
    ProjectExplorer::PeInfoSet InfoSetBackward(*m_pInfoSetBackward);
    QList<ProjectExplorer::PeInfoSet> lstAffectInfoSets;

    if(!GetUiData(&InfoSetForward, &InfoSetBackward, lstAffectInfoSets))
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
    if(!pProjectVersion->DbUpdateObject(InfoSetForward, false))
    {
        DbTrans.Rollback();
        return;
    }
    if(!pProjectVersion->DbUpdateObject(InfoSetBackward, false))
    {
        DbTrans.Rollback();
        return;
    }
    foreach(ProjectExplorer::PeInfoSet InfoSetAffected, lstAffectInfoSets)
    {
        if(!pProjectVersion->DbUpdateObject(InfoSetAffected, false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    if(!DbTrans.Commit())
        return;

    if(pProjectVersion->UpdateObject(InfoSetForward))
    {
        pProjectVersion->UpdateObject(InfoSetBackward);

        foreach(ProjectExplorer::PeInfoSet InfoSetAffected, lstAffectInfoSets)
            pProjectVersion->UpdateObject(InfoSetAffected);

        SlotSetInfoSet(m_pInfoSetForward);
    }
}

void WidgetInfoSet::SlotActionCreate()
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pBay->GetProjectVersion();
    if(!pProjectVersion)
        return;

    ProjectExplorer::PeInfoSet InfoSetForward;
    InfoSetForward.SetProjectVersion(pProjectVersion);

    ProjectExplorer::PeInfoSet InfoSetBackward;
    InfoSetBackward.SetProjectVersion(pProjectVersion);

    if(!GetUiData(&InfoSetForward, &InfoSetBackward))
        return;

    int iGroup = pProjectVersion->DbGenerateNewInfoSetGroup();
    if(iGroup <= 0)
        return;

    InfoSetForward.SetGroup(iGroup);
    InfoSetBackward.SetGroup(iGroup);

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
    if(!pProjectVersion->DbCreateObject(InfoSetForward, false))
    {
        DbTrans.Rollback();
        return;
    }
    if(!pProjectVersion->DbCreateObject(InfoSetBackward, false))
    {
        DbTrans.Rollback();
        return;
    }

    if(!DbTrans.Commit())
        return;

    if(ProjectExplorer::PeInfoSet *pInfoSetForward = qobject_cast<ProjectExplorer::PeInfoSet*>(pProjectVersion->CreateObject(InfoSetForward)))
    {
        pProjectVersion->CreateObject(InfoSetBackward);
        SlotSetInfoSet(pInfoSetForward);
    }
}
