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
#include "projectexplorer/basemanager.h"
#include "projectexplorer/pbtpinfoset.h"
#include "projectexplorer/pbtpbay.h"
#include "projectexplorer/pbtpdevice.h"
#include "projectexplorer/pbtpport.h"

#include "propertydlgtpinfoset.h"

using namespace BayTemplate::Internal;

typedef QPair<ProjectExplorer::PbTpPort*, ProjectExplorer::PbTpPort*> PortPair;

static QList<PortPair> GetPairPorts(const QList<ProjectExplorer::PbTpPort*> &lstPorts)
{
    QList<ProjectExplorer::PbTpPort*> lstSortedPorts = lstPorts;
    qSort(lstSortedPorts.begin(), lstSortedPorts.end(), ProjectExplorer::PbTpPort::CompareGroup);

    QList<PortPair> lstPairPorts;
    while(!lstSortedPorts.isEmpty())
    {
        PortPair pair;

        ProjectExplorer::PbTpPort *pPort = lstSortedPorts.takeFirst();
        ProjectExplorer::PbTpPort *pPortPair = pPort->GetPairPort();
        if(pPortPair)
            lstSortedPorts.removeOne(pPortPair);

        if(pPort->GetPortDirection() == ProjectExplorer::PbTpPort::pdTx)
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

static QString GetPortPath(ProjectExplorer::PbTpPort *pPortTx, ProjectExplorer::PbTpPort *pPortRx)
{
    QString strPortPath;

    if(pPortTx && pPortRx)
    {
        if(ProjectExplorer::PbTpDevice *pDevice = pPortTx->GetParentDevice())
            strPortPath = QString("%1<%2/%3>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetDisplayName()).arg(pPortRx->GetDisplayName());
    }
    else if(pPortTx && !pPortRx)
    {
        if(ProjectExplorer::PbTpDevice *pDevice = pPortTx->GetParentDevice())
            strPortPath = QString("%1<%2>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetDisplayName());
    }
    else if(pPortRx && !pPortTx)
    {
        if(ProjectExplorer::PbTpDevice *pDevice = pPortRx->GetParentDevice())
            strPortPath = QString("%1<%2>").arg(pDevice->GetDisplayName()).arg(pPortRx->GetDisplayName());
    }

    return strPortPath;
}

static ProjectExplorer::PbTpInfoSet* FindIdenticalTpInfoSet(ProjectExplorer::PbTpInfoSet *pTpInfoSet, const QList<ProjectExplorer::PbTpInfoSet*> lstTpInfoSets)
{
    bool bNeedToCheckPath = !(pTpInfoSet->GetInfoSetType() == ProjectExplorer::PbTpInfoSet::itNone ||
                              (pTpInfoSet->GetTxIED() && !pTpInfoSet->GetTxPort()) ||
                              (pTpInfoSet->GetRxIED() && !pTpInfoSet->GetRxPort()) ||
                              (pTpInfoSet->GetSwitch1() && (!pTpInfoSet->GetSwitch1TxPort() || !pTpInfoSet->GetSwitch1RxPort())) ||
                              (pTpInfoSet->GetSwitch2() && (!pTpInfoSet->GetSwitch2TxPort() || !pTpInfoSet->GetSwitch2RxPort())) ||
                              (pTpInfoSet->GetSwitch3() && (!pTpInfoSet->GetSwitch3TxPort() || !pTpInfoSet->GetSwitch3RxPort())) ||
                              (pTpInfoSet->GetSwitch4() && (!pTpInfoSet->GetSwitch4TxPort() || !pTpInfoSet->GetSwitch4RxPort())));
    if(bNeedToCheckPath)
    {
        foreach(ProjectExplorer::PbTpInfoSet *pTpInfoSetTemp, lstTpInfoSets)
        {
            if(pTpInfoSetTemp->GetId() == pTpInfoSet->GetId())
                continue;

            if(pTpInfoSetTemp->GetInfoSetType() == pTpInfoSet->GetInfoSetType() &&
               pTpInfoSetTemp->GetTxIED() == pTpInfoSet->GetTxIED() &&
               pTpInfoSetTemp->GetTxPort() == pTpInfoSet->GetTxPort() &&
               pTpInfoSetTemp->GetRxIED() == pTpInfoSet->GetRxIED() &&
               pTpInfoSetTemp->GetRxPort() == pTpInfoSet->GetRxPort() &&
               pTpInfoSetTemp->GetSwitch1() == pTpInfoSet->GetSwitch1() &&
               pTpInfoSetTemp->GetSwitch1TxPort() == pTpInfoSet->GetSwitch1TxPort() &&
               pTpInfoSetTemp->GetSwitch1RxPort() == pTpInfoSet->GetSwitch1RxPort() &&
               pTpInfoSetTemp->GetSwitch2() == pTpInfoSet->GetSwitch2() &&
               pTpInfoSetTemp->GetSwitch2TxPort() == pTpInfoSet->GetSwitch2TxPort() &&
               pTpInfoSetTemp->GetSwitch2RxPort() == pTpInfoSet->GetSwitch2RxPort() &&
               pTpInfoSetTemp->GetSwitch3() == pTpInfoSet->GetSwitch3() &&
               pTpInfoSetTemp->GetSwitch3TxPort() == pTpInfoSet->GetSwitch3TxPort() &&
               pTpInfoSetTemp->GetSwitch3RxPort() == pTpInfoSet->GetSwitch3RxPort() &&
               pTpInfoSetTemp->GetSwitch4() == pTpInfoSet->GetSwitch4() &&
               pTpInfoSetTemp->GetSwitch4TxPort() == pTpInfoSet->GetSwitch4TxPort() &&
               pTpInfoSetTemp->GetSwitch4RxPort() == pTpInfoSet->GetSwitch4RxPort())
            {
                return pTpInfoSetTemp;
            }
        }
    }

    return 0;
}

static void FindPortConnectionInfo(ProjectExplorer::PbTpPort *pPort,
                                   const QList<ProjectExplorer::PbTpInfoSet*> &lstAllTpInfoSets,
                                   QList<ProjectExplorer::PbTpPort*> &lstConnectedPorts,
                                   QList<ProjectExplorer::PbTpInfoSet*> &lstTpInfoSetsForPort,
                                   QList<ProjectExplorer::PbTpInfoSet*> &lstTpInfoSetsForConnectedPort)
{
    foreach(ProjectExplorer::PbTpInfoSet *pTpInfoSet, lstAllTpInfoSets)
    {
        ProjectExplorer::PbTpPort *pPortConnected = 0;

        if(pTpInfoSet->IsContaintPort(pPort, &pPortConnected))
        {
            ProjectExplorer::PbTpInfoSet *pTpInfoSetForPort = pTpInfoSet;
            if(pTpInfoSetForPort->GetInfoSetType() == ProjectExplorer::PbTpInfoSet::itNone)
                pTpInfoSetForPort = pTpInfoSetForPort->GetPairInfoSet();

            if(!pTpInfoSetForPort)
                continue;

            if(!lstTpInfoSetsForPort.contains(pTpInfoSetForPort))
                lstTpInfoSetsForPort.append(pTpInfoSetForPort);
        }

        if(pPortConnected)
        {
            if(!lstConnectedPorts.contains(pPortConnected))
                lstConnectedPorts.append(pPortConnected);

            if(!lstTpInfoSetsForConnectedPort.contains(pTpInfoSet))
                lstTpInfoSetsForConnectedPort.append(pTpInfoSet);
        }
    }
}

static bool ValidatePortConnection(ProjectExplorer::PbTpInfoSet *pTpInfoSet,
                                   ProjectExplorer::PbTpPort *pPort,
                                   QList<ProjectExplorer::PbTpInfoSet> &lstAffectTpInfoSets,
                                   const QList<ProjectExplorer::PbTpInfoSet*> &lstAllTpInfoSets)
{
    QList<ProjectExplorer::PbTpPort*> lstConnectedPorts;
    QList<ProjectExplorer::PbTpInfoSet*> lstTpInfoSetsForPort;
    QList<ProjectExplorer::PbTpInfoSet*> lstTpInfoSetsForConnectedPort;

    FindPortConnectionInfo(pPort, lstAllTpInfoSets, lstConnectedPorts, lstTpInfoSetsForPort, lstTpInfoSetsForConnectedPort);
    if(lstConnectedPorts.isEmpty())
        return true;

    if(lstConnectedPorts.size() == 1)
    {
        ProjectExplorer::PbTpPort *pPortConnectedOld = lstConnectedPorts.first();

        ProjectExplorer::PbTpPort *pPortConnectedNew = 0;
        pTpInfoSet->IsContaintPort(pPort, &pPortConnectedNew);

        if(pPortConnectedOld == pPortConnectedNew)
            return true;

        if(pTpInfoSet->GetId() != ProjectExplorer::PbBaseObject::m_iInvalidObjectId)
        {
            foreach(ProjectExplorer::PbTpInfoSet *pTpInfoSetForConnectedPort, lstTpInfoSetsForConnectedPort)
            {
                if(pTpInfoSetForConnectedPort->GetId() == pTpInfoSet->GetId())
                    continue;

                ProjectExplorer::PbTpInfoSet *pTpInfoSetAffected = 0;
                for(int i = 0; i < lstAffectTpInfoSets.size(); i++)
                {
                    ProjectExplorer::PbTpInfoSet &TpInfoSetAffected = lstAffectTpInfoSets[i];
                    if(TpInfoSetAffected.GetId() == pTpInfoSetForConnectedPort->GetId())
                    {
                        pTpInfoSetAffected = &TpInfoSetAffected;
                        break;
                    }
                }

                if(pTpInfoSetAffected)
                {
                    pTpInfoSetAffected->ReplacePort(pPortConnectedOld, 0);//pPortConnectedNew);
                }
                else
                {
                    ProjectExplorer::PbTpInfoSet TpInfoSet(*pTpInfoSetForConnectedPort);
                    TpInfoSet.ReplacePort(pPortConnectedOld, 0);//pPortConnectedNew);
                    lstAffectTpInfoSets.append(TpInfoSet);
                }
            }

            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////
// SelectTpInfoSetDlg member functions
///////////////////////////////////////////////////////////////////////
SelectTpInfoSetDlg::SelectTpInfoSetDlg(const QList<ProjectExplorer::PbTpInfoSet*> &lstTpInfoSets, QWidget *pParent)
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

    foreach(ProjectExplorer::PbTpInfoSet *pTpInfoSet, lstTpInfoSets)
    {
        QList<QStandardItem*> lstItems;

        QStandardItem *pItem = new QStandardItem(pTpInfoSet->GetDisplayIcon(), pTpInfoSet->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pTpInfoSet));
        lstItems.append(pItem);

        QString strTxIED = "";
        if(ProjectExplorer::PbTpDevice *pTxIED = pTpInfoSet->GetTxIED())
        {
            strTxIED = pTxIED->GetDisplayName();
            if(ProjectExplorer::PbTpPort *pTxPort = pTpInfoSet->GetTxPort())
                strTxIED = QString("%1<%2>").arg(strTxIED).arg(pTxPort->GetDisplayName());
        }
        pItem = new QStandardItem(strTxIED);
        pItem->setData(reinterpret_cast<int>(pTpInfoSet));
        lstItems.append(pItem);

        QString strRxIED = "";
        if(ProjectExplorer::PbTpDevice *pRxIED = pTpInfoSet->GetRxIED())
        {
            strRxIED = pRxIED->GetDisplayName();
            if(ProjectExplorer::PbTpPort *pRxPort = pTpInfoSet->GetRxPort())
                strRxIED = QString("%1<%2>").arg(strRxIED).arg(pRxPort->GetDisplayName());
        }
        pItem = new QStandardItem(strRxIED);
        pItem->setData(reinterpret_cast<int>(pTpInfoSet));
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

ProjectExplorer::PbTpInfoSet* SelectTpInfoSetDlg::GetSelectedTpInfoSet() const
{
    QItemSelectionModel *pSelectionModel = m_pView->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
    if(lstSelectedIndex.isEmpty())
        return 0;

    QStandardItem *pItem = m_pModel->itemFromIndex(lstSelectedIndex.first());
    if(!pItem)
        return 0;

    return reinterpret_cast<ProjectExplorer::PbTpInfoSet*>(pItem->data().toInt());
}

void SelectTpInfoSetDlg::SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    QItemSelectionModel *pSelectionModel = m_pView->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();

    m_pPushButtonOk->setEnabled(lstSelectedIndex.size() == 1);
}

void SelectTpInfoSetDlg::SlotViewDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    accept();
}

///////////////////////////////////////////////////////////////////////
// TreeWidgetItemPort member functions
///////////////////////////////////////////////////////////////////////
TreeWidgetItemPort::TreeWidgetItemPort(ProjectExplorer::PbTpPort *pPortTx, ProjectExplorer::PbTpPort *pPortRx, bool bSwitchPort) :
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

ProjectExplorer::PbTpPort* TreeWidgetItemPort::GetPortTx() const
{
    return m_pPortTx;
}

ProjectExplorer::PbTpPort* TreeWidgetItemPort::GetPortRx() const
{
    return m_pPortRx;
}

QList<ProjectExplorer::PbTpInfoSet*> TreeWidgetItemPort::GetTpInfoSets() const
{
    return m_lstTpInfoSets;
}

void TreeWidgetItemPort::UpdateConnectionInfo(const QList<ProjectExplorer::PbTpInfoSet*> &lstTpInfoSets)
{
    QList<ProjectExplorer::PbTpPort*> lstConnectedPorts;
    QList<ProjectExplorer::PbTpInfoSet*> lstTpInfoSetsForConnectedPort;

    FindPortConnectionInfo(m_pPortTx, lstTpInfoSets, lstConnectedPorts, m_lstTpInfoSets, lstTpInfoSetsForConnectedPort);
    if(!lstConnectedPorts.isEmpty())
        m_pPortConnectedRx = lstConnectedPorts.first();

    lstConnectedPorts.clear();
    FindPortConnectionInfo(m_pPortRx, lstTpInfoSets, lstConnectedPorts, m_lstTpInfoSets, lstTpInfoSetsForConnectedPort);
    if(!lstConnectedPorts.isEmpty())
        m_pPortConnectedTx = lstConnectedPorts.first();

    qSort(m_lstTpInfoSets.begin(), m_lstTpInfoSets.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);

    QString strConnectedPortPath = GetPortPath(m_pPortConnectedTx, m_pPortConnectedRx);
    if(!strConnectedPortPath.isEmpty())
        strConnectedPortPath = "<--->" + strConnectedPortPath;

    QString strTpInfoSets;
    foreach(ProjectExplorer::PbTpInfoSet *pTpInfoSet, m_lstTpInfoSets)
        strTpInfoSets = QString("%1\n%2").arg(strTpInfoSets).arg(pTpInfoSet->GetDisplayName());

    QString strToolTip = strTpInfoSets.isEmpty() ? strConnectedPortPath : strConnectedPortPath + "\n" + strTpInfoSets;
    setToolTip(0, strToolTip );
    if(m_bSwitchPort)
        setToolTip(1, strToolTip);

    MarkConnectionInfo(false);
}

void TreeWidgetItemPort::CheckDirecPortConnection(const QList<ProjectExplorer::PbTpDevice*> &lstDevices)
{
    if(lstDevices.isEmpty())
    {
        MarkConnectionInfo(false);
        return;
    }

    ProjectExplorer::PbTpDevice *pDeviceConnectedRx = m_pPortConnectedRx ? m_pPortConnectedRx->GetParentDevice() : 0;
    ProjectExplorer::PbTpDevice *pDeviceConnectedTx = m_pPortConnectedTx ? m_pPortConnectedTx->GetParentDevice() : 0;

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
    if(!m_lstTpInfoSets.isEmpty())
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
PropertyDlgTpInfoSet::PropertyDlgTpInfoSet(ProjectExplorer::PbTpInfoSet *pTpInfoSetForward, ProjectExplorer::PbTpInfoSet *pTpInfoSetBackward, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pTpInfoSetForward(pTpInfoSetForward), m_pTpInfoSetBackward(pTpInfoSetBackward)
{
    setFixedSize(QSize(1100, 550));

    m_pTpBay = m_pTpInfoSetForward->GetParentBay();
    Q_ASSERT(m_pTpBay);

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

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addLayout(pHBoxLayoutGeneral);
    pLayout->addSpacing(10);
    pLayout->addLayout(m_pHBoxLayoutDetails);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    m_pComboBoxDeviceForward->addItem("", 0);
    m_pComboBoxDeviceBackward->addItem("", 0);
    m_pComboBoxSwitch1->addItem("", 0);
    m_pComboBoxSwitch2->addItem("", 0);
    m_pComboBoxSwitch3->addItem("", 0);
    m_pComboBoxSwitch4->addItem("", 0);

    QList<ProjectExplorer::PbTpDevice*> lstDevices = m_pTpBay->GetChildDevices();
    qSort(lstDevices.begin(), lstDevices.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    foreach(ProjectExplorer::PbTpDevice *pDevice, lstDevices)
    {
        if(pDevice->GetDeviceType() == ProjectExplorer::PbTpDevice::dtIED)
        {
            m_pComboBoxDeviceForward->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));
            m_pComboBoxDeviceBackward->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));
        }
        else if(pDevice->GetDeviceType() == ProjectExplorer::PbTpDevice::dtSwitch)
        {
            m_pComboBoxSwitch1->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));
            m_pComboBoxSwitch2->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));
            m_pComboBoxSwitch3->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));
            m_pComboBoxSwitch4->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));
        }
    }

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

    SetUiData(m_pTpInfoSetForward, m_pTpInfoSetBackward);
}

PropertyDlgTpInfoSet::~PropertyDlgTpInfoSet()
{
}

QList<QAction*> PropertyDlgTpInfoSet::GetActions() const
{
    return QList<QAction*>() << m_pActionCreate << m_pActionModify;
}

QWidget* PropertyDlgTpInfoSet::SetupGeneralForward()
{
    m_pLineEditNameForward = new QLineEdit(this);
    m_pComboBoxTypeForward = new QComboBox(this);
    m_pComboBoxTypeForward->setFixedWidth(150);

    m_pComboBoxTypeForward->addItem(ProjectExplorer::PbTpInfoSet::GetInfoSetTypeName(ProjectExplorer::PbTpInfoSet::itGoose), ProjectExplorer::PbTpInfoSet::itGoose);
    m_pComboBoxTypeForward->addItem(ProjectExplorer::PbTpInfoSet::GetInfoSetTypeName(ProjectExplorer::PbTpInfoSet::itSv), ProjectExplorer::PbTpInfoSet::itSv);
    m_pComboBoxTypeForward->addItem(ProjectExplorer::PbTpInfoSet::GetInfoSetTypeName(ProjectExplorer::PbTpInfoSet::itTime), ProjectExplorer::PbTpInfoSet::itTime);
    m_pComboBoxTypeForward->addItem(ProjectExplorer::PbTpInfoSet::GetInfoSetTypeName(ProjectExplorer::PbTpInfoSet::itMix), ProjectExplorer::PbTpInfoSet::itMix);

    m_pGroupBoxForward = new QGroupBox(tr("Forward Information Logic"), this);
    m_pGroupBoxForward->setCheckable(true);
    QGridLayout *pGridLayout = new QGridLayout(m_pGroupBoxForward);

    pGridLayout->addWidget(new QLabel(tr("Name:"), this), 0, 0);
    pGridLayout->addWidget(m_pLineEditNameForward, 0, 1);
    pGridLayout->addWidget(new QLabel(tr("Type:"), this), 0, 2);
    pGridLayout->addWidget(m_pComboBoxTypeForward, 0, 3);

    return m_pGroupBoxForward;
}

QWidget* PropertyDlgTpInfoSet::SetupGeneralBackward()
{
    m_pLineEditNameBackward = new QLineEdit(this);
    m_pComboBoxTypeBackward = new QComboBox(this);
    m_pComboBoxTypeBackward->setFixedWidth(150);

    m_pComboBoxTypeBackward->addItem(ProjectExplorer::PbTpInfoSet::GetInfoSetTypeName(ProjectExplorer::PbTpInfoSet::itGoose), ProjectExplorer::PbTpInfoSet::itGoose);
    m_pComboBoxTypeBackward->addItem(ProjectExplorer::PbTpInfoSet::GetInfoSetTypeName(ProjectExplorer::PbTpInfoSet::itSv), ProjectExplorer::PbTpInfoSet::itSv);
    m_pComboBoxTypeBackward->addItem(ProjectExplorer::PbTpInfoSet::GetInfoSetTypeName(ProjectExplorer::PbTpInfoSet::itTime), ProjectExplorer::PbTpInfoSet::itTime);
    m_pComboBoxTypeBackward->addItem(ProjectExplorer::PbTpInfoSet::GetInfoSetTypeName(ProjectExplorer::PbTpInfoSet::itMix), ProjectExplorer::PbTpInfoSet::itMix);

    m_pGroupBoxBackward = new QGroupBox(tr("Backward Information Logic"), this);
    m_pGroupBoxBackward->setCheckable(true);
    QGridLayout *pGridLayout = new QGridLayout(m_pGroupBoxBackward);

    pGridLayout->addWidget(new QLabel(tr("Name:"), this), 0, 0);
    pGridLayout->addWidget(m_pLineEditNameBackward, 0, 1);
    pGridLayout->addWidget(new QLabel(tr("Type:"), this), 0, 2);
    pGridLayout->addWidget(m_pComboBoxTypeBackward, 0, 3);

    return m_pGroupBoxBackward;
}

QWidget* PropertyDlgTpInfoSet::SetupDeviceForward()
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

QWidget* PropertyDlgTpInfoSet::SetupDeviceBackward()
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

QWidget* PropertyDlgTpInfoSet::SetupSwitch1()
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

QWidget* PropertyDlgTpInfoSet::SetupSwitch2()
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

QWidget* PropertyDlgTpInfoSet::SetupSwitch3()
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

QWidget* PropertyDlgTpInfoSet::SetupSwitch4()
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

TreeWidgetItemPort* PropertyDlgTpInfoSet::GetCheckedItem(QTreeWidget *pTreeWidget, int iColumn)
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

bool PropertyDlgTpInfoSet::SetCheckedPort(QTreeWidget *pTreeWidget, int iColumn, ProjectExplorer::PbTpPort *pPort)
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

void PropertyDlgTpInfoSet::MarkConnectedPorts()
{
    int iCurrentIndex = m_pComboBoxDeviceForward->currentIndex();
    ProjectExplorer::PbTpDevice *pTxIED = iCurrentIndex > 0 ? reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxDeviceForward->itemData(iCurrentIndex).toInt()) : 0;

    iCurrentIndex = m_pComboBoxDeviceBackward->currentIndex();
    ProjectExplorer::PbTpDevice *pRxIED = iCurrentIndex > 0 ? reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxDeviceBackward->itemData(iCurrentIndex).toInt()) : 0;

    iCurrentIndex = m_pComboBoxSwitch1->currentIndex();
    ProjectExplorer::PbTpDevice *pSwitch1 = iCurrentIndex > 0 ? reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxSwitch1->itemData(iCurrentIndex).toInt()) : 0;

    iCurrentIndex = m_pComboBoxSwitch2->currentIndex();
    ProjectExplorer::PbTpDevice *pSwitch2 = iCurrentIndex > 0 ? reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxSwitch2->itemData(iCurrentIndex).toInt()) : 0;

    iCurrentIndex = m_pComboBoxSwitch3->currentIndex();
    ProjectExplorer::PbTpDevice *pSwitch3 = iCurrentIndex > 0 ? reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxSwitch3->itemData(iCurrentIndex).toInt()) : 0;

    iCurrentIndex = m_pComboBoxSwitch4->currentIndex();
    ProjectExplorer::PbTpDevice *pSwitch4 = iCurrentIndex > 0 ? reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxSwitch4->itemData(iCurrentIndex).toInt()) : 0;

    QList<ProjectExplorer::PbTpDevice*> lstConnectedDevices;
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

bool PropertyDlgTpInfoSet::BlockSignals(bool bBlock)
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

void PropertyDlgTpInfoSet::SetUiData(ProjectExplorer::PbTpInfoSet *pTpInfoSetForward, ProjectExplorer::PbTpInfoSet *pTpInfoSetBackward)
{
    if(!pTpInfoSetForward || !pTpInfoSetBackward)
    {
        m_pGroupBoxForward->setChecked(false);
        m_pLineEditNameForward->clear();
        m_pComboBoxTypeForward->setCurrentIndex(0);

        m_pGroupBoxBackward->setChecked(false);
        m_pLineEditNameBackward->clear();
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
    if(pTpInfoSetForward->GetInfoSetType() != ProjectExplorer::PbTpInfoSet::itNone)
    {
        m_pGroupBoxForward->setChecked(true);

        m_pLineEditNameForward->setText(pTpInfoSetForward->GetName());
        int iCurrentIndex = m_pComboBoxTypeForward->findData(pTpInfoSetForward->GetInfoSetType());
        m_pComboBoxTypeForward->setCurrentIndex(iCurrentIndex == -1 ? 0 : iCurrentIndex);
    }
    else
    {
        m_pGroupBoxForward->setChecked(false);
        m_pLineEditNameForward->clear();
        m_pComboBoxTypeForward->setCurrentIndex(0);
    }

    /////////////////////////////////////////
    // For pair infoset
    /////////////////////////////////////////
    if(pTpInfoSetBackward->GetInfoSetType() != ProjectExplorer::PbTpInfoSet::itNone)
    {
        m_pGroupBoxBackward->setChecked(true);

        m_pLineEditNameBackward->setText(pTpInfoSetBackward->GetName());
        int iCurrentIndex = m_pComboBoxTypeBackward->findData(pTpInfoSetBackward->GetInfoSetType());
        m_pComboBoxTypeBackward->setCurrentIndex(iCurrentIndex == -1 ? 0 : iCurrentIndex);
    }
    else
    {
        m_pGroupBoxBackward->setChecked(false);
        m_pLineEditNameBackward->clear();
        m_pGroupBoxBackward->setChecked(false);
    }

    /////////////////////////////////////////
    // Set device and port
    /////////////////////////////////////////
    ProjectExplorer::PbTpDevice *pIEDForward = 0;
    ProjectExplorer::PbTpPort *pPortIEDForward = 0;
    ProjectExplorer::PbTpDevice *pIEDBackward = 0;
    ProjectExplorer::PbTpPort *pPortIEDBackward = 0;
    ProjectExplorer::PbTpDevice *pSwitch1 = 0;
    ProjectExplorer::PbTpPort *pPortSwitch1Left = 0;
    ProjectExplorer::PbTpPort *pPortSwitch1Right = 0;
    ProjectExplorer::PbTpDevice *pSwitch2 = 0;
    ProjectExplorer::PbTpPort *pPortSwitch2Left = 0;
    ProjectExplorer::PbTpPort *pPortSwitch2Right = 0;
    ProjectExplorer::PbTpDevice *pSwitch3 = 0;
    ProjectExplorer::PbTpPort *pPortSwitch3Left = 0;
    ProjectExplorer::PbTpPort *pPortSwitch3Right = 0;
    ProjectExplorer::PbTpDevice *pSwitch4 = 0;
    ProjectExplorer::PbTpPort *pPortSwitch4Left = 0;
    ProjectExplorer::PbTpPort *pPortSwitch4Right = 0;

    if(pTpInfoSetForward->GetInfoSetType() != ProjectExplorer::PbTpInfoSet::itNone)
    {
        pIEDForward = pTpInfoSetForward->GetTxIED();
        pPortIEDForward = pTpInfoSetForward->GetTxPort();
        pIEDBackward = pTpInfoSetForward->GetRxIED();
        pPortIEDBackward = pTpInfoSetForward->GetRxPort();
        pSwitch1 = pTpInfoSetForward->GetSwitch1();
        pPortSwitch1Left = pTpInfoSetForward->GetSwitch1RxPort();
        pPortSwitch1Right = pTpInfoSetForward->GetSwitch1TxPort();
        pSwitch2 = pTpInfoSetForward->GetSwitch2();
        pPortSwitch2Left = pTpInfoSetForward->GetSwitch2RxPort();
        pPortSwitch2Right = pTpInfoSetForward->GetSwitch2TxPort();
        pSwitch3 = pTpInfoSetForward->GetSwitch3();
        pPortSwitch3Left = pTpInfoSetForward->GetSwitch3RxPort();
        pPortSwitch3Right = pTpInfoSetForward->GetSwitch3TxPort();
        pSwitch4 = pTpInfoSetForward->GetSwitch4();
        pPortSwitch4Left = pTpInfoSetForward->GetSwitch4RxPort();
        pPortSwitch4Right = pTpInfoSetForward->GetSwitch4TxPort();
    }
    else if(pTpInfoSetBackward->GetInfoSetType() != ProjectExplorer::PbTpInfoSet::itNone)
    {
        pIEDForward = pTpInfoSetBackward->GetRxIED();
        pPortIEDForward = pTpInfoSetBackward->GetRxPort();
        pIEDBackward = pTpInfoSetBackward->GetTxIED();
        pPortIEDBackward = pTpInfoSetBackward->GetTxPort();

        if(pTpInfoSetBackward->GetSwitch4())
        {
            pSwitch1 = pTpInfoSetBackward->GetSwitch4();
            pPortSwitch1Left = pTpInfoSetBackward->GetSwitch4TxPort();
            pPortSwitch1Right = pTpInfoSetBackward->GetSwitch4RxPort();
            pSwitch2 = pTpInfoSetBackward->GetSwitch3();
            pPortSwitch2Left = pTpInfoSetBackward->GetSwitch3TxPort();
            pPortSwitch2Right = pTpInfoSetBackward->GetSwitch3RxPort();
            pSwitch3 = pTpInfoSetBackward->GetSwitch2();
            pPortSwitch3Left = pTpInfoSetBackward->GetSwitch2TxPort();
            pPortSwitch3Right = pTpInfoSetBackward->GetSwitch2RxPort();
            pSwitch4 = pTpInfoSetBackward->GetSwitch1();
            pPortSwitch4Left = pTpInfoSetBackward->GetSwitch1TxPort();
            pPortSwitch4Right = pTpInfoSetBackward->GetSwitch1RxPort();
        }
        else if(pTpInfoSetBackward->GetSwitch3())
        {
            pSwitch1 = pTpInfoSetBackward->GetSwitch3();
            pPortSwitch1Left = pTpInfoSetBackward->GetSwitch3TxPort();
            pPortSwitch1Right = pTpInfoSetBackward->GetSwitch3RxPort();
            pSwitch2 = pTpInfoSetBackward->GetSwitch2();
            pPortSwitch2Left = pTpInfoSetBackward->GetSwitch2TxPort();
            pPortSwitch2Right = pTpInfoSetBackward->GetSwitch2RxPort();
            pSwitch3 = pTpInfoSetBackward->GetSwitch1();
            pPortSwitch3Left = pTpInfoSetBackward->GetSwitch1TxPort();
            pPortSwitch3Right = pTpInfoSetBackward->GetSwitch1RxPort();
        }
        else if(pTpInfoSetBackward->GetSwitch2())
        {
            pSwitch1 = pTpInfoSetBackward->GetSwitch2();
            pPortSwitch1Left = pTpInfoSetBackward->GetSwitch2TxPort();
            pPortSwitch1Right = pTpInfoSetBackward->GetSwitch2RxPort();
            pSwitch2 = pTpInfoSetBackward->GetSwitch1();
            pPortSwitch2Left = pTpInfoSetBackward->GetSwitch1TxPort();
            pPortSwitch2Right = pTpInfoSetBackward->GetSwitch1RxPort();
        }
        else if(pTpInfoSetBackward->GetSwitch1())
        {
            pSwitch1 = pTpInfoSetBackward->GetSwitch1();
            pPortSwitch1Left = pTpInfoSetBackward->GetSwitch1TxPort();
            pPortSwitch1Right = pTpInfoSetBackward->GetSwitch1RxPort();
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

bool PropertyDlgTpInfoSet::GetUiData(ProjectExplorer::PbTpInfoSet *pTpInfoSetForward, ProjectExplorer::PbTpInfoSet *pTpInfoSetBackward, QList<ProjectExplorer::PbTpInfoSet> &lstAffectTpInfoSets)
{
    if(!pTpInfoSetForward || !pTpInfoSetBackward)
        return false;

    const bool bHasForwardTpInfoSet = m_pGroupBoxForward->isChecked();
    const bool bHasBackwardTpInfoSet = m_pGroupBoxBackward->isChecked();
    if(!bHasForwardTpInfoSet && !bHasBackwardTpInfoSet)
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
    ProjectExplorer::PbTpDevice *pTxIED = reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxDeviceForward->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxDeviceBackward->currentIndex();
    if(iCurrentIndex <= 0)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxDeviceBackward->objectName()));

        m_pComboBoxDeviceBackward->setFocus();
        return false;
    }
    ProjectExplorer::PbTpDevice *pRxIED = reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxDeviceBackward->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxSwitch1->currentIndex();
    if(iCurrentIndex <= 0 && m_pGroupBoxSwitch1->isChecked())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxSwitch1->objectName()));

        m_pComboBoxSwitch1->setFocus();
        return false;
    }
    ProjectExplorer::PbTpDevice *pSwitch1 = iCurrentIndex <= 0 ? 0 : reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxSwitch1->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxSwitch2->currentIndex();
    if(iCurrentIndex <= 0 && m_pGroupBoxSwitch2->isChecked())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxSwitch2->objectName()));

        m_pComboBoxSwitch2->setFocus();
        return false;
    }
    ProjectExplorer::PbTpDevice *pSwitch2 = iCurrentIndex <= 0 ? 0 : reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxSwitch2->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxSwitch3->currentIndex();
    if(iCurrentIndex <= 0 && m_pGroupBoxSwitch3->isChecked())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxSwitch3->objectName()));

        m_pComboBoxSwitch3->setFocus();
        return false;
    }
    ProjectExplorer::PbTpDevice *pSwitch3 = iCurrentIndex <= 0 ? 0 : reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxSwitch3->itemData(iCurrentIndex).toInt());

    iCurrentIndex = m_pComboBoxSwitch4->currentIndex();
    if(iCurrentIndex <= 0 && m_pGroupBoxSwitch4->isChecked())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxSwitch4->objectName()));

        m_pComboBoxSwitch4->setFocus();
        return false;
    }
    ProjectExplorer::PbTpDevice *pSwitch4 = iCurrentIndex <= 0 ? 0 : reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxSwitch4->itemData(iCurrentIndex).toInt());

    /////////////////////////////////////////
    // Validate Port
    /////////////////////////////////////////
    const QList<ProjectExplorer::PbTpInfoSet*> lstAllTpInfoSets = m_pTpBay->GetChildInfoSets();

    // Forward IED port
    TreeWidgetItemPort *pItemPortForwardIED = GetCheckedItem(m_pTreeWidgetPortForward, 0);
    ProjectExplorer::PbTpPort *pPortForwardTx = pItemPortForwardIED ? pItemPortForwardIED->GetPortTx() : 0;
    ProjectExplorer::PbTpPort *pPortForwardRx = pItemPortForwardIED ? pItemPortForwardIED->GetPortRx() : 0;

    // Forward IED port
    TreeWidgetItemPort *pItemPortBackwardIED = GetCheckedItem(m_pTreeWidgetPortBackward, 0);
    ProjectExplorer::PbTpPort *pPortBackwardTx = pItemPortBackwardIED ? pItemPortBackwardIED->GetPortTx() : 0;
    ProjectExplorer::PbTpPort *pPortBackwardRx = pItemPortBackwardIED ? pItemPortBackwardIED->GetPortRx() : 0;

    // Switch1 port
    TreeWidgetItemPort *pItemPortSwitch1Left = GetCheckedItem(m_pTreeWidgetPortSwitch1, 0);
    ProjectExplorer::PbTpPort *pPortSwitch1LeftTx = pItemPortSwitch1Left ? pItemPortSwitch1Left->GetPortTx() : 0;
    ProjectExplorer::PbTpPort *pPortSwitch1LeftRx = pItemPortSwitch1Left ? pItemPortSwitch1Left->GetPortRx() : 0;

    TreeWidgetItemPort *pItemPortSwitch1Right = GetCheckedItem(m_pTreeWidgetPortSwitch1, 1);
    ProjectExplorer::PbTpPort *pPortSwitch1RightTx = pItemPortSwitch1Right ? pItemPortSwitch1Right->GetPortTx() : 0;
    ProjectExplorer::PbTpPort *pPortSwitch1RightRx = pItemPortSwitch1Right ? pItemPortSwitch1Right->GetPortRx() : 0;

    // Switch2 port
    TreeWidgetItemPort *pItemPortSwitch2Left = GetCheckedItem(m_pTreeWidgetPortSwitch2, 0);
    ProjectExplorer::PbTpPort *pPortSwitch2LeftTx = pItemPortSwitch2Left ? pItemPortSwitch2Left->GetPortTx() : 0;
    ProjectExplorer::PbTpPort *pPortSwitch2LeftRx = pItemPortSwitch2Left ? pItemPortSwitch2Left->GetPortRx() : 0;
    TreeWidgetItemPort *pItemPortSwitch2Right = GetCheckedItem(m_pTreeWidgetPortSwitch2, 1);
    ProjectExplorer::PbTpPort *pPortSwitch2RightTx = pItemPortSwitch2Right ? pItemPortSwitch2Right->GetPortTx() : 0;
    ProjectExplorer::PbTpPort *pPortSwitch2RightRx = pItemPortSwitch2Right ? pItemPortSwitch2Right->GetPortRx() : 0;

    // Switch3 port
    TreeWidgetItemPort *pItemPortSwitch3Left = GetCheckedItem(m_pTreeWidgetPortSwitch3, 0);
    ProjectExplorer::PbTpPort *pPortSwitch3LeftTx = pItemPortSwitch3Left ? pItemPortSwitch3Left->GetPortTx() : 0;
    ProjectExplorer::PbTpPort *pPortSwitch3LeftRx = pItemPortSwitch3Left ? pItemPortSwitch3Left->GetPortRx() : 0;
    TreeWidgetItemPort *pItemPortSwitch3Right = GetCheckedItem(m_pTreeWidgetPortSwitch3, 1);
    ProjectExplorer::PbTpPort *pPortSwitch3RightTx = pItemPortSwitch3Right ? pItemPortSwitch3Right->GetPortTx() : 0;
    ProjectExplorer::PbTpPort *pPortSwitch3RightRx = pItemPortSwitch3Right ? pItemPortSwitch3Right->GetPortRx() : 0;

    // Switch4 port
    TreeWidgetItemPort *pItemPortSwitch4Left = GetCheckedItem(m_pTreeWidgetPortSwitch4, 0);
    ProjectExplorer::PbTpPort *pPortSwitch4LeftTx = pItemPortSwitch4Left ? pItemPortSwitch4Left->GetPortTx() : 0;
    ProjectExplorer::PbTpPort *pPortSwitch4LeftRx = pItemPortSwitch4Left ? pItemPortSwitch4Left->GetPortRx() : 0;
    TreeWidgetItemPort *pItemPortSwitch4Right = GetCheckedItem(m_pTreeWidgetPortSwitch4, 1);
    ProjectExplorer::PbTpPort *pPortSwitch4RightTx = pItemPortSwitch4Right ? pItemPortSwitch4Right->GetPortTx() : 0;
    ProjectExplorer::PbTpPort *pPortSwitch4RightRx = pItemPortSwitch4Right ? pItemPortSwitch4Right->GetPortRx() : 0;

    if(bHasForwardTpInfoSet)
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
    if(bHasBackwardTpInfoSet)
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
    pTpInfoSetForward->SetTxIED(pTxIED);
    pTpInfoSetForward->SetTxPort(pPortForwardTx);
    pTpInfoSetForward->SetRxIED(pRxIED);
    pTpInfoSetForward->SetRxPort(pPortBackwardRx);
    pTpInfoSetForward->SetSwitch1(pSwitch1);
    pTpInfoSetForward->SetSwitch1RxPort(pPortSwitch1LeftRx);
    pTpInfoSetForward->SetSwitch1TxPort(pPortSwitch1RightTx);
    pTpInfoSetForward->SetSwitch2(pSwitch2);
    pTpInfoSetForward->SetSwitch2RxPort(pPortSwitch2LeftRx);
    pTpInfoSetForward->SetSwitch2TxPort(pPortSwitch2RightTx);
    pTpInfoSetForward->SetSwitch3(pSwitch3);
    pTpInfoSetForward->SetSwitch3RxPort(pPortSwitch3LeftRx);
    pTpInfoSetForward->SetSwitch3TxPort(pPortSwitch3RightTx);
    pTpInfoSetForward->SetSwitch4(pSwitch4);
    pTpInfoSetForward->SetSwitch4RxPort(pPortSwitch4LeftRx);
    pTpInfoSetForward->SetSwitch4TxPort(pPortSwitch4RightTx);

    if(bHasForwardTpInfoSet)
    {
        const QString strName = m_pLineEditNameForward->text().trimmed();
        if(strName.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The name of forward information logic can NOT be empty."));
            return false;
        }

        pTpInfoSetForward->SetName(strName);
        pTpInfoSetForward->SetInfoSetType(ProjectExplorer::PbTpInfoSet::InfoSetType(m_pComboBoxTypeForward->itemData(m_pComboBoxTypeForward->currentIndex()).toInt()));

        if(ProjectExplorer::PbTpInfoSet *pTpInfoSetFound = FindIdenticalTpInfoSet(pTpInfoSetForward, lstAllTpInfoSets))
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The path of forward information logic is identical to the existed information logic '%1'").arg(pTpInfoSetFound->GetName()));

            return false;
        }
    }
    else
    {
        pTpInfoSetForward->SetName("");
        pTpInfoSetForward->SetInfoSetType(ProjectExplorer::PbTpInfoSet::itNone);
    }

    /////////////////////////////////////////
    // For backward infoset
    /////////////////////////////////////////
    pTpInfoSetBackward->SetTxIED(pRxIED);
    pTpInfoSetBackward->SetTxPort(pPortBackwardTx);
    pTpInfoSetBackward->SetRxIED(pTxIED);
    pTpInfoSetBackward->SetRxPort(pPortForwardRx);
    pTpInfoSetBackward->SetSwitch1(0);
    pTpInfoSetBackward->SetSwitch1TxPort(0);
    pTpInfoSetBackward->SetSwitch1RxPort(0);
    pTpInfoSetBackward->SetSwitch2(0);
    pTpInfoSetBackward->SetSwitch2TxPort(0);
    pTpInfoSetBackward->SetSwitch2RxPort(0);
    pTpInfoSetBackward->SetSwitch3(0);
    pTpInfoSetBackward->SetSwitch3TxPort(0);
    pTpInfoSetBackward->SetSwitch3RxPort(0);
    pTpInfoSetBackward->SetSwitch4(0);
    pTpInfoSetBackward->SetSwitch4TxPort(0);
    pTpInfoSetBackward->SetSwitch4RxPort(0);
    if(pSwitch4)
    {
        pTpInfoSetBackward->SetSwitch1(pSwitch4);
        pTpInfoSetBackward->SetSwitch1RxPort(pPortSwitch4RightRx);
        pTpInfoSetBackward->SetSwitch1TxPort(pPortSwitch4LeftTx);

        pTpInfoSetBackward->SetSwitch2(pSwitch3);
        pTpInfoSetBackward->SetSwitch2RxPort(pPortSwitch3RightRx);
        pTpInfoSetBackward->SetSwitch2TxPort(pPortSwitch3LeftTx);

        pTpInfoSetBackward->SetSwitch3(pSwitch2);
        pTpInfoSetBackward->SetSwitch3RxPort(pPortSwitch2RightRx);
        pTpInfoSetBackward->SetSwitch3TxPort(pPortSwitch2LeftTx);

        pTpInfoSetBackward->SetSwitch4(pSwitch1);
        pTpInfoSetBackward->SetSwitch4RxPort(pPortSwitch1RightRx);
        pTpInfoSetBackward->SetSwitch4TxPort(pPortSwitch1LeftTx);
    }
    else if(pSwitch3)
    {
        pTpInfoSetBackward->SetSwitch1(pSwitch3);
        pTpInfoSetBackward->SetSwitch1RxPort(pPortSwitch3RightRx);
        pTpInfoSetBackward->SetSwitch1TxPort(pPortSwitch3LeftTx);

        pTpInfoSetBackward->SetSwitch2(pSwitch2);
        pTpInfoSetBackward->SetSwitch2RxPort(pPortSwitch2RightRx);
        pTpInfoSetBackward->SetSwitch2TxPort(pPortSwitch2LeftTx);

        pTpInfoSetBackward->SetSwitch3(pSwitch1);
        pTpInfoSetBackward->SetSwitch3RxPort(pPortSwitch1RightRx);
        pTpInfoSetBackward->SetSwitch3TxPort(pPortSwitch1LeftTx);
    }
    else if(pSwitch2)
    {
        pTpInfoSetBackward->SetSwitch1(pSwitch2);
        pTpInfoSetBackward->SetSwitch1RxPort(pPortSwitch2RightRx);
        pTpInfoSetBackward->SetSwitch1TxPort(pPortSwitch2LeftTx);

        pTpInfoSetBackward->SetSwitch2(pSwitch1);
        pTpInfoSetBackward->SetSwitch2RxPort(pPortSwitch1RightRx);
        pTpInfoSetBackward->SetSwitch2TxPort(pPortSwitch1LeftTx);
    }
    else if(pSwitch1)
    {
        pTpInfoSetBackward->SetSwitch1(pSwitch1);
        pTpInfoSetBackward->SetSwitch1RxPort(pPortSwitch1RightRx);
        pTpInfoSetBackward->SetSwitch1TxPort(pPortSwitch1LeftTx);
    }

    if(bHasBackwardTpInfoSet)
    {
        const QString strName = m_pLineEditNameBackward->text().trimmed();
        if(strName.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The name of backward information logic can NOT be empty."));
            return false;
        }

        pTpInfoSetBackward->SetName(strName);
        pTpInfoSetBackward->SetInfoSetType(ProjectExplorer::PbTpInfoSet::InfoSetType(m_pComboBoxTypeBackward->itemData(m_pComboBoxTypeBackward->currentIndex()).toInt()));

        if(ProjectExplorer::PbTpInfoSet *pTpInfoSetFound = FindIdenticalTpInfoSet(pTpInfoSetBackward, lstAllTpInfoSets))
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The path of backward information logic is identical to the existed information logic '%1'").arg(pTpInfoSetFound->GetName()));

            return false;
        }
    }
    else
    {
        pTpInfoSetBackward->SetName("");
        pTpInfoSetBackward->SetInfoSetType(ProjectExplorer::PbTpInfoSet::itNone);
    }

    /////////////////////////////////////////
    // Validate port connection
    /////////////////////////////////////////
    ProjectExplorer::PbTpPort *pPortError = 0;
    if(pPortForwardTx && !ValidatePortConnection(pTpInfoSetForward, pPortForwardTx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortForwardTx;
    else if(pPortBackwardRx && !ValidatePortConnection(pTpInfoSetForward, pPortBackwardRx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortBackwardRx;
    else if(pPortSwitch1LeftRx && !ValidatePortConnection(pTpInfoSetForward, pPortSwitch1LeftRx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch1LeftRx;
    else if(pPortSwitch1RightTx && !ValidatePortConnection(pTpInfoSetForward, pPortSwitch1RightTx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch1RightTx;
    else if(pPortSwitch2LeftRx && !ValidatePortConnection(pTpInfoSetForward, pPortSwitch2LeftRx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch2LeftRx;
    else if(pPortSwitch2RightTx && !ValidatePortConnection(pTpInfoSetForward, pPortSwitch2RightTx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch2RightTx;
    else if(pPortSwitch3LeftRx && !ValidatePortConnection(pTpInfoSetForward, pPortSwitch3LeftRx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch3LeftRx;
    else if(pPortSwitch3RightTx && !ValidatePortConnection(pTpInfoSetForward, pPortSwitch3RightTx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch3RightTx;
    else if(pPortSwitch4LeftRx && !ValidatePortConnection(pTpInfoSetForward, pPortSwitch4LeftRx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch4LeftRx;
    else if(pPortSwitch4RightTx && !ValidatePortConnection(pTpInfoSetForward, pPortSwitch4RightTx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch4RightTx;
    else if(pPortForwardRx && !ValidatePortConnection(pTpInfoSetBackward, pPortForwardRx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortForwardRx;
    else if(pPortBackwardTx && !ValidatePortConnection(pTpInfoSetBackward, pPortBackwardTx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortBackwardTx;
    else if(pPortSwitch1LeftTx && !ValidatePortConnection(pTpInfoSetBackward, pPortSwitch1LeftTx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch1LeftTx;
    else if(pPortSwitch1RightRx && !ValidatePortConnection(pTpInfoSetBackward, pPortSwitch1RightRx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch1RightRx;
    else if(pPortSwitch2LeftTx && !ValidatePortConnection(pTpInfoSetBackward, pPortSwitch2LeftTx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch2LeftTx;
    else if(pPortSwitch2RightRx && !ValidatePortConnection(pTpInfoSetBackward, pPortSwitch2RightRx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch2RightRx;
    else if(pPortSwitch3LeftTx && !ValidatePortConnection(pTpInfoSetBackward, pPortSwitch3LeftTx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch3LeftTx;
    else if(pPortSwitch3RightRx && !ValidatePortConnection(pTpInfoSetBackward, pPortSwitch3RightRx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch3RightRx;
    else if(pPortSwitch4LeftTx && !ValidatePortConnection(pTpInfoSetBackward, pPortSwitch4LeftTx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch4LeftTx;
    else if(pPortSwitch4RightRx && !ValidatePortConnection(pTpInfoSetBackward, pPortSwitch4RightRx, lstAffectTpInfoSets, lstAllTpInfoSets))
        pPortError = pPortSwitch4RightRx;

    if(pPortError)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The port '%1' has connected to a different port in other information logic.").arg(GetPortPath(pPortError, 0)));
        return false;
    }

    QString strAffctedTpInfoSets;
    foreach(ProjectExplorer::PbTpInfoSet TpInfoSet, lstAffectTpInfoSets)
    {
        if(TpInfoSet.GetInfoSetType() != ProjectExplorer::PbTpInfoSet::itNone)
            strAffctedTpInfoSets = QString("%1\n%2").arg(strAffctedTpInfoSets).arg(TpInfoSet.GetDisplayName());
    }

    if(!strAffctedTpInfoSets.isEmpty())
    {
        if(QMessageBox::question(this,
                                 tr("Modify Confirmation"),
                                 tr("The following information logic will be affected, are you sure you want to modify the current information logic?") + "\n" + strAffctedTpInfoSets,
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No) == QMessageBox::No)
        {
            return false;
        }
    }

    return true;
}

void PropertyDlgTpInfoSet::accept()
{
    ProjectExplorer::BaseManager *pBaseManager = ProjectExplorer::BaseManager::Instance();
    if(!pBaseManager)
        return;

    if((m_pTpInfoSetForward->GetId() == ProjectExplorer::PbBaseObject::m_iInvalidObjectId) && (m_pTpInfoSetBackward->GetId() == ProjectExplorer::PbBaseObject::m_iInvalidObjectId))
    {
        if(!GetUiData(m_pTpInfoSetForward, m_pTpInfoSetBackward))
            return;

        int iGroup = pBaseManager->DbGenerateNewTpInfoSetGroup();
        if(iGroup <= 0)
            return;

        m_pTpInfoSetForward->SetGroup(iGroup);
        m_pTpInfoSetBackward->SetGroup(iGroup);

        Utils::WaitCursor cursor;
        Q_UNUSED(cursor)

        if(!pBaseManager->DbCreateObject(*m_pTpInfoSetForward))
            return;

        if(!pBaseManager->DbCreateObject(*m_pTpInfoSetBackward))
            return;

        pBaseManager->CreateObject(*m_pTpInfoSetForward);
        pBaseManager->CreateObject(*m_pTpInfoSetBackward);

        StyledUi::StyledDialog::accept();
    }
    else if((m_pTpInfoSetForward->GetId() != ProjectExplorer::PbBaseObject::m_iInvalidObjectId) && (m_pTpInfoSetBackward->GetId() != ProjectExplorer::PbBaseObject::m_iInvalidObjectId))
    {
        QList<ProjectExplorer::PbTpInfoSet> lstAffectTpInfoSets;
        if(!GetUiData(m_pTpInfoSetForward, m_pTpInfoSetBackward, lstAffectTpInfoSets))
            return;

        Utils::WaitCursor cursor;
        Q_UNUSED(cursor)

        if(!pBaseManager->DbUpdateObject(*m_pTpInfoSetForward))
            return;

        if(!pBaseManager->DbUpdateObject(*m_pTpInfoSetBackward))
            return;

        foreach(ProjectExplorer::PbTpInfoSet TpInfoSetAffected, lstAffectTpInfoSets)
        {
            if(!pBaseManager->DbUpdateObject(TpInfoSetAffected))
                return;
        }

        pBaseManager->UpdateObject(*m_pTpInfoSetForward);
        pBaseManager->UpdateObject(*m_pTpInfoSetBackward);
        foreach(ProjectExplorer::PbTpInfoSet TpInfoSetAffected, lstAffectTpInfoSets)
            pBaseManager->UpdateObject(TpInfoSetAffected);

        StyledUi::StyledDialog::accept();
    }
}

void PropertyDlgTpInfoSet::SlotSetTpInfoSet(ProjectExplorer::PbTpInfoSet *pTpInfoSet)
{
    Utils::WaitCursor waitcursor;
    Q_UNUSED(waitcursor)

    m_pTpInfoSetForward = m_pTpInfoSetBackward = 0;
    if(ProjectExplorer::PbTpInfoSet *pTpInfoSetPair = (pTpInfoSet ? pTpInfoSet->GetPairInfoSet() : 0))
    {
        if(pTpInfoSet->GetInfoSetType() == ProjectExplorer::PbTpInfoSet::itNone)
        {
            ProjectExplorer::PbTpInfoSet *pTpInfoSetTemp = pTpInfoSet;
            pTpInfoSet = pTpInfoSetPair;
            pTpInfoSetPair = pTpInfoSetTemp;
        }

        ProjectExplorer::PbTpDevice *pTxIED = pTpInfoSet->GetTxIED();
        ProjectExplorer::PbTpDevice *pRxIED = pTpInfoSet->GetRxIED();
        if(pTxIED && pRxIED)
        {
            if(pTxIED->GetParentBay() == m_pTpBay)
            {
                m_pTpInfoSetForward = pTpInfoSet;
                m_pTpInfoSetBackward = pTpInfoSetPair;
            }
            else if(pRxIED->GetParentBay() == m_pTpBay)
            {
                m_pTpInfoSetForward = pTpInfoSetPair;
                m_pTpInfoSetBackward = pTpInfoSet;
            }
            else if(pTxIED->GetParentBay())
            {
                m_pTpInfoSetForward = pTpInfoSet;
                m_pTpInfoSetBackward = pTpInfoSetPair;
            }
            else if(pRxIED->GetParentBay())
            {
                m_pTpInfoSetForward = pTpInfoSetPair;
                m_pTpInfoSetBackward = pTpInfoSet;
            }
        }
    }

    bool bBlock = BlockSignals(true);
    SetUiData(m_pTpInfoSetForward, m_pTpInfoSetBackward);
    BlockSignals(bBlock);

    m_pActionCreate->setEnabled(m_pTpBay);
    m_pActionModify->setEnabled(m_pTpInfoSetForward && m_pTpInfoSetBackward);
}

void PropertyDlgTpInfoSet::SlotCurrentForwardDeviceChanged(int iIndex)
{
    m_pTreeWidgetPortForward->clear();

    if(iIndex <= 0)
        return;

    ProjectExplorer::PbTpDevice *pDevice = reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxDeviceForward->itemData(iIndex).toInt());
    if(!pDevice)
        return;

    foreach(const PortPair &pari, GetPairPorts(pDevice->GetChildPorts()))
    {
        ProjectExplorer::PbTpPort *pPortTx = pari.first;
        ProjectExplorer::PbTpPort *pPortRx = pari.second;

        TreeWidgetItemPort *pItemPort = new TreeWidgetItemPort(pPortTx, pPortRx, false);
        pItemPort->UpdateConnectionInfo(m_pTpBay->GetChildInfoSets());
        m_pTreeWidgetPortForward->addTopLevelItem(pItemPort);
    }

    m_pTreeWidgetPortForward->expandAll();

    MarkConnectedPorts();
}

void PropertyDlgTpInfoSet::SlotCurrentBackwardDeviceChanged(int iIndex)
{
    m_pTreeWidgetPortBackward->clear();

    if(iIndex <= 0)
        return;

    ProjectExplorer::PbTpDevice *pDevice = reinterpret_cast<ProjectExplorer::PbTpDevice*>(m_pComboBoxDeviceBackward->itemData(iIndex).toInt());
    if(!pDevice)
        return;

    foreach(const PortPair &pari, GetPairPorts(pDevice->GetChildPorts()))
    {
        ProjectExplorer::PbTpPort *pPortTx = pari.first;
        ProjectExplorer::PbTpPort *pPortRx = pari.second;

        TreeWidgetItemPort *pItemPort = new TreeWidgetItemPort(pPortTx, pPortRx, false);
        pItemPort->UpdateConnectionInfo(m_pTpBay->GetChildInfoSets());
        m_pTreeWidgetPortBackward->addTopLevelItem(pItemPort);
    }

    m_pTreeWidgetPortBackward->expandAll();

    MarkConnectedPorts();
}

void PropertyDlgTpInfoSet::SlotCurrentSwitchChanged(int iIndex, QComboBox *pComboBoxSwitch)
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

    ProjectExplorer::PbTpDevice *pDevice = reinterpret_cast<ProjectExplorer::PbTpDevice*>(pComboBoxSwitch->itemData(iIndex).toInt());
    if(!pDevice)
        return;

    foreach(const PortPair &pari, GetPairPorts(pDevice->GetChildPorts()))
    {
        ProjectExplorer::PbTpPort *pPortTx = pari.first;
        ProjectExplorer::PbTpPort *pPortRx = pari.second;

        TreeWidgetItemPort *pItemPort = new TreeWidgetItemPort(pPortTx, pPortRx, true);
        pItemPort->UpdateConnectionInfo(m_pTpBay->GetChildInfoSets());
        pTreeWidgetSwitchPort->addTopLevelItem(pItemPort);
    }

    pTreeWidgetSwitchPort->expandAll();
    MarkConnectedPorts();
}

void PropertyDlgTpInfoSet::SlotSwitchToggled(bool bToggled)
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

void PropertyDlgTpInfoSet::SlotPortDoubleClicked(QTreeWidgetItem *pItem, int iColumn)
{
    Q_UNUSED(iColumn)

    TreeWidgetItemPort *pItemPort = dynamic_cast<TreeWidgetItemPort*>(pItem);
    if(!pItemPort)
        return;

    QList<ProjectExplorer::PbTpInfoSet*> lstTpInfoSets = pItemPort->GetTpInfoSets();
    lstTpInfoSets.removeAll(m_pTpInfoSetForward);
    lstTpInfoSets.removeAll(m_pTpInfoSetBackward);
    if(lstTpInfoSets.isEmpty())
        return;

    ProjectExplorer::PbTpInfoSet *pTpInfoSet = 0;
    if(lstTpInfoSets.size() == 1)
    {
        pTpInfoSet = lstTpInfoSets.first();
    }
    else
    {
        SelectTpInfoSetDlg dlg(lstTpInfoSets, this);
        if(dlg.exec() == QDialog::Accepted)
            pTpInfoSet = dlg.GetSelectedTpInfoSet();
    }

    if(pTpInfoSet)
        SlotSetTpInfoSet(pTpInfoSet);
}

void PropertyDlgTpInfoSet::SlotActionModify()
{
//    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pTpBay->GetProjectVersion();
//    if(!pProjectVersion)
//        return;

//    if(!m_pTpInfoSetForward || !m_pTpInfoSetBackward)
//        return;

//    ProjectExplorer::PbTpInfoSet TpInfoSetForward(*m_pTpInfoSetForward);
//    ProjectExplorer::PbTpInfoSet TpInfoSetBackward(*m_pTpInfoSetBackward);
//    QList<ProjectExplorer::PbTpInfoSet> lstAffectTpInfoSets;

//    if(!GetUiData(&TpInfoSetForward, &TpInfoSetBackward, lstAffectTpInfoSets))
//        return;

//    Utils::WaitCursor cursor;
//    Q_UNUSED(cursor)

//    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
//    if(!pProjectVersion->DbUpdateObject(TpInfoSetForward, false))
//    {
//        DbTrans.Rollback();
//        return;
//    }
//    if(!pProjectVersion->DbUpdateObject(TpInfoSetBackward, false))
//    {
//        DbTrans.Rollback();
//        return;
//    }
//    foreach(ProjectExplorer::PbTpInfoSet TpInfoSetAffected, lstAffectTpInfoSets)
//    {
//        if(!pProjectVersion->DbUpdateObject(TpInfoSetAffected, false))
//        {
//            DbTrans.Rollback();
//            return;
//        }
//    }

//    if(!DbTrans.Commit())
//        return;

//    if(pProjectVersion->UpdateObject(TpInfoSetForward))
//    {
//        pProjectVersion->UpdateObject(TpInfoSetBackward);

//        foreach(ProjectExplorer::PbTpInfoSet TpInfoSetAffected, lstAffectTpInfoSets)
//            pProjectVersion->UpdateObject(TpInfoSetAffected);

//        SlotSetTpInfoSet(m_pTpInfoSetForward);
//    }
}

void PropertyDlgTpInfoSet::SlotActionCreate()
{
//    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pTpBay->GetProjectVersion();
//    if(!pProjectVersion)
//        return;

//    ProjectExplorer::PbTpInfoSet TpInfoSetForward;
//    TpInfoSetForward.SetProjectVersion(pProjectVersion);

//    ProjectExplorer::PbTpInfoSet TpInfoSetBackward;
//    TpInfoSetBackward.SetProjectVersion(pProjectVersion);

//    if(!GetUiData(&TpInfoSetForward, &TpInfoSetBackward))
//        return;

//    int iGroup = pProjectVersion->DbGenerateNewTpInfoSetGroup();
//    if(iGroup <= 0)
//        return;

//    TpInfoSetForward.SetGroup(iGroup);
//    TpInfoSetBackward.SetGroup(iGroup);

//    Utils::WaitCursor cursor;
//    Q_UNUSED(cursor)

//    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
//    if(!pProjectVersion->DbCreateObject(TpInfoSetForward, false))
//    {
//        DbTrans.Rollback();
//        return;
//    }
//    if(!pProjectVersion->DbCreateObject(TpInfoSetBackward, false))
//    {
//        DbTrans.Rollback();
//        return;
//    }

//    if(!DbTrans.Commit())
//        return;

//    if(ProjectExplorer::PbTpInfoSet *pTpInfoSetForward = qobject_cast<ProjectExplorer::PbTpInfoSet*>(pProjectVersion->CreateObject(TpInfoSetForward)))
//    {
//        pProjectVersion->CreateObject(TpInfoSetBackward);
//        SlotSetTpInfoSet(pTpInfoSetForward);
//    }
}
