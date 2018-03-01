#include <QStandardItemModel>
#include <QAction>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QApplication>

#include "utils/readonlyview.h"
#include "utils/waitcursor.h"
#include "projectexplorer/basemanager.h"
#include "projectexplorer/pbtpbay.h"
#include "projectexplorer/pbtpdevice.h"
#include "projectexplorer/pbtpport.h"
#include "projectexplorer/pbtpinfoset.h"

#include "configwidgettpinfoset.h"
#include "propertydlgtpinfoset.h"

using namespace BayTemplate::Internal;

ConfigWidgetTpInfoSet::ConfigWidgetTpInfoSet(ProjectExplorer::PbTpBay *pTpBay, QWidget *pParent) :
    QWidget(pParent), m_pTpBay(pTpBay)
{
    setMinimumSize(QSize(1200, 500));

    // Create Model
    m_pModel = new QStandardItemModel(0, 8);
    m_pModel->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Type") << tr("Send IED")
                                                      << tr("First Level Switch") << tr("Second Level Switch")
                                                      << tr("Third Level Switch") << tr("Fourth Level Switch") << tr("Receive IED"));
    m_pView = new Utils::ReadOnlyTableView(this);
    m_pView->setMinimumHeight(250);
    m_pView->setAlternatingRowColors(true);
    m_pView->setSortingEnabled(true);
    m_pView->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pView->setModel(m_pModel);
    m_pView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pView->horizontalHeader()->setHighlightSections(false);
    m_pView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pView->horizontalHeader()->setStretchLastSection(true);
    m_pView->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pView->verticalHeader()->hide();
    m_pView->setShowGrid(false);
    m_pView->setColumnWidth(0, 200);
    m_pView->setColumnWidth(1, 60);
    m_pView->setColumnWidth(2, 200);
    m_pView->setColumnWidth(3, 200);
    m_pView->setColumnWidth(4, 200);
    m_pView->setColumnWidth(5, 200);
    m_pView->setColumnWidth(6, 200);
    m_pView->setColumnWidth(7, 200);

    QList<ProjectExplorer::PbTpInfoSet*> lstTpInfoSets = pTpBay->GetChildInfoSets();
    qSort(lstTpInfoSets.begin(), lstTpInfoSets.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    foreach(ProjectExplorer::PbTpInfoSet *pTpInfoSet, lstTpInfoSets)
        SlotBaseObjectCreated(pTpInfoSet);

    m_pActionNew = new QAction(ProjectExplorer::PbBaseObject::GetObjectIcon(ProjectExplorer::PbBaseObject::botTpInfoSet), tr("New..."), this);
    m_pActionNew->setEnabled(true);
    connect(m_pActionNew, SIGNAL(triggered()), this, SLOT(SlotActionNew()));
    m_pView->addAction(m_pActionNew);

    m_pActionDelete = new QAction(QIcon(":/baytemplate/images/oper_remove.png"), tr("&Delete..."), this);
    m_pActionDelete->setEnabled(false);
    connect(m_pActionDelete, SIGNAL(triggered()), this, SLOT(SlotActionDelete()));
    m_pView->addAction(m_pActionDelete);

    QAction *pSeparator = new QAction(this);
    pSeparator->setSeparator(true);
    m_pView->addAction(pSeparator);

    m_pActionProperty = new QAction(QIcon(":/devexplorer/images/oper_edit.png"), tr("&Property..."), this);
    m_pActionProperty->setEnabled(false);
    connect(m_pActionProperty, SIGNAL(triggered()), this, SLOT(SlotActionProperty()));
    m_pView->addAction(m_pActionProperty);

    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(m_pView);

    connect(ProjectExplorer::BaseManager::Instance(), SIGNAL(sigObjectCreated(ProjectExplorer::PbBaseObject*)),
            this, SLOT(SlotBaseObjectCreated(ProjectExplorer::PbBaseObject*)));
    connect(ProjectExplorer::BaseManager::Instance(), SIGNAL(sigObjectPropertyChanged(ProjectExplorer::PbBaseObject*)),
            this, SLOT(SlotBaseObjectPropertyChanged(ProjectExplorer::PbBaseObject*)));
    connect(ProjectExplorer::BaseManager::Instance(), SIGNAL(sigObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject*)),
            this, SLOT(SlotBaseObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject*)));
    connect(m_pView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(m_pView, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewDoubleClicked(const QModelIndex&)));
}

ConfigWidgetTpInfoSet::~ConfigWidgetTpInfoSet()
{
}

void ConfigWidgetTpInfoSet::UpdateCompleteStatus(ProjectExplorer::PbTpInfoSet *pTpInfoSet)
{
    if(QStandardItem *pItem = m_mapTpInfoSetToItem.value(pTpInfoSet, 0))
    {
        for(int i = 0; i < m_pModel->columnCount(); i++)
            m_pModel->item(pItem->row(), i)->setForeground(pTpInfoSet->IsComplete() ? QColor(Qt::black) : QColor(Qt::red));
    }
}

//bool ConfigWidgetTpInfoSet::CreateTpDevice(ProjectExplorer::PbTpDevice *pTpDeviceTemp)
//{
//    Utils::WaitCursor cursor;
//    Q_UNUSED(cursor)

//    ProjectExplorer::BaseManager *pBaseManager = ProjectExplorer::BaseManager::Instance();
//    if(!pBaseManager)
//        return false;

//    ///////////////////////////////////////////////////////////////////////
//    // Handle Database
//    ///////////////////////////////////////////////////////////////////////

//    // Create new device
//    if(!pBaseManager->DbCreateObject(*pTpDeviceTemp))
//        return false;

//    foreach(ProjectExplorer::PbTpPort *pTpPort, pTpDeviceTemp->GetChildPorts())
//    {
//        if(!pBaseManager->DbCreateObject(*pTpPort))
//            return false;
//    }

//    ///////////////////////////////////////////////////////////////////////
//    // Handle Object
//    ///////////////////////////////////////////////////////////////////////
//    ProjectExplorer::PbTpDevice *pTpDeviceCreated = qobject_cast<ProjectExplorer::PbTpDevice*>(pBaseManager->CreateObject(*pTpDeviceTemp));
//    if(!pTpDeviceCreated)
//        return false;

//    foreach(ProjectExplorer::PbTpPort *pTpPort, pTpDeviceTemp->GetChildPorts())
//    {
//        pTpPort->SetParentDevice(pTpDeviceCreated);

//        ProjectExplorer::PbTpPort *pTpPortCreated = qobject_cast<ProjectExplorer::PbTpPort*>(pBaseManager->CreateObject(*pTpPort, false));
//        if(!pTpPortCreated)
//            return false;
//    }

//    return true;
//}

//bool ConfigWidgetTpInfoSet::UpdateTpDevice(ProjectExplorer::PbTpDevice *pTpDeviceTemp)
//{
//    Utils::WaitCursor cursor;
//    Q_UNUSED(cursor)

//    ProjectExplorer::BaseManager *pBaseManager = ProjectExplorer::BaseManager::Instance();
//    if(!pBaseManager)
//        return false;

//    ProjectExplorer::PbTpDevice *pTpDevice = pBaseManager->FindTpDeviceById(pTpDeviceTemp->GetId());
//    if(!pTpDevice)
//        return false;

//    QList<ProjectExplorer::PbTpPort*> lstCreatedTpPorts, lstDeletedTpPorts, lstUpdatedTpPorts;

//    QList<ProjectExplorer::PbTpPort*> lstOldTpPorts = pTpDevice->GetChildPorts();
//    QList<ProjectExplorer::PbTpPort*> lstNewTpPorts = pTpDeviceTemp->GetChildPorts();
//    foreach(ProjectExplorer::PbTpPort *pNewTpPort, lstNewTpPorts)
//    {
//        if(pNewTpPort->GetId() == ProjectExplorer::PbBaseObject::m_iInvalidObjectId)
//        {
//            lstNewTpPorts.removeOne(pNewTpPort);
//            lstCreatedTpPorts.append(pNewTpPort);
//        }
//    }

//    foreach(ProjectExplorer::PbTpPort *pNewTpPort, lstNewTpPorts)
//    {
//        ProjectExplorer::PbTpPort *pOldTpPort = pBaseManager->FindTpPortById(pNewTpPort->GetId());
//        if(!pOldTpPort)
//            return false;

//        if(!lstOldTpPorts.removeOne(pOldTpPort))
//            return false;
//    }

//    lstUpdatedTpPorts = lstNewTpPorts;
//    lstDeletedTpPorts = lstOldTpPorts;

//    ///////////////////////////////////////////////////////////////////////
//    // Handle Database
//    ///////////////////////////////////////////////////////////////////////

//    // Update device
//    if(!pBaseManager->DbUpdateObject(*pTpDeviceTemp))
//        return false;

//    // Create port
//    foreach(ProjectExplorer::PbTpPort *pTpPort, lstCreatedTpPorts)
//    {
//        if(!pBaseManager->DbCreateObject(*pTpPort))
//            return false;
//    }

//    // Update port
//    foreach(ProjectExplorer::PbTpPort *pTpPort, lstUpdatedTpPorts)
//    {
//        if(!pBaseManager->DbUpdateObject(*pTpPort))
//            return false;
//    }

//    // Delete port
//    foreach(ProjectExplorer::PbTpPort *pTpPort, lstDeletedTpPorts)
//    {
//        if(!pBaseManager->DbDeleteObject(pTpPort->GetObjectType(), pTpPort->GetId(), false))
//            return false;
//    }

//    ///////////////////////////////////////////////////////////////////////
//    // Handle Object
//    ///////////////////////////////////////////////////////////////////////

//    // Update device
//    if(!pBaseManager->UpdateObject(*pTpDeviceTemp))
//        return false;

//    // Create port
//    foreach(ProjectExplorer::PbTpPort *pTpPort, lstCreatedTpPorts)
//    {
//        pTpPort->SetParentDevice(pTpDevice);

//        ProjectExplorer::PbTpPort *pTpPortCreated = qobject_cast<ProjectExplorer::PbTpPort*>(pBaseManager->CreateObject(*pTpPort, false));
//        if(!pTpPortCreated)
//            return false;
//    }

//    // Update port
//    foreach(ProjectExplorer::PbTpPort *pTpPort, lstUpdatedTpPorts)
//    {
//        pTpPort->SetParentDevice(pTpDevice);

//        ProjectExplorer::PbTpPort *pTpPortUpdated = qobject_cast<ProjectExplorer::PbTpPort*>(pBaseManager->UpdateObject(*pTpPort, false));
//        if(!pTpPortUpdated)
//            return false;
//    }

//    // Delete port
//    foreach(ProjectExplorer::PbTpPort *pTpPort, lstDeletedTpPorts)
//    {
//        if(!pBaseManager->DeleteObject(pTpPort, false))
//            return false;
//    }

//    return true;
//}

//bool ConfigWidgetTpInfoSet::DeleteTpDevice(const QList<ProjectExplorer::PbTpDevice*> &lstTpDevices)
//{
//    if(lstTpDevices.isEmpty())
//        return true;

//    QString strItems;
//    foreach(ProjectExplorer::PbTpDevice *pTpDevice, lstTpDevices)
//        strItems = QString("%1\n%2: %3").arg(strItems).arg(pTpDevice->GetObjectTypeName()).arg(pTpDevice->GetDisplayName());

//    if(QMessageBox::question(this,
//                             tr("Delete Confirmation"),
//                             tr("Are you sure you want to delete following items?") + "\n" + strItems,
//                             QMessageBox::Yes | QMessageBox::No,
//                             QMessageBox::No) == QMessageBox::Yes)
//    {
//        Utils::WaitCursor cursor;
//        Q_UNUSED(cursor)

//        ///////////////////////////////////////////////////////////////////////
//        // Handle Database
//        ///////////////////////////////////////////////////////////////////////

//        foreach(ProjectExplorer::PbTpDevice *pTpDevice, lstTpDevices)
//        {
//            if(!ProjectExplorer::BaseManager::Instance()->DbDeleteObject(pTpDevice->GetObjectType(), pTpDevice->GetId(), false))
//                return false;
//        }

//        ///////////////////////////////////////////////////////////////////////
//        // Handle Object
//        ///////////////////////////////////////////////////////////////////////

//        foreach(ProjectExplorer::PbTpDevice *pTpDevice, lstTpDevices)
//            ProjectExplorer::BaseManager::Instance()->DeleteObject(pTpDevice);
//    }

//    return true;
//}

void ConfigWidgetTpInfoSet::SlotBaseObjectCreated(ProjectExplorer::PbBaseObject *pBaseObject)
{
    ProjectExplorer::PbTpInfoSet *pTpInfoSet = qobject_cast<ProjectExplorer::PbTpInfoSet*>(pBaseObject);
    if(!pTpInfoSet)
        return;

    ProjectExplorer::PbTpDevice *pTxIED = pTpInfoSet->GetTxIED();
    ProjectExplorer::PbTpDevice *pRxIED = pTpInfoSet->GetRxIED();
    ProjectExplorer::PbTpDevice *pSwitch1 = pTpInfoSet->GetSwitch1();
    ProjectExplorer::PbTpDevice *pSwitch2 = pTpInfoSet->GetSwitch2();
    ProjectExplorer::PbTpDevice *pSwitch3 = pTpInfoSet->GetSwitch3();
    ProjectExplorer::PbTpDevice *pSwitch4 = pTpInfoSet->GetSwitch4();

    QList<QStandardItem*> lstItems;

    QStandardItem *pItem = new QStandardItem(pTpInfoSet->GetDisplayIcon(), pTpInfoSet->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pTpInfoSet));
    lstItems.append(pItem);

    pItem = new QStandardItem(ProjectExplorer::PbTpInfoSet::GetInfoSetTypeName(pTpInfoSet->GetInfoSetType()));
    pItem->setData(reinterpret_cast<int>(pTpInfoSet));
    lstItems.append(pItem);

    QString strTxIED = "";
    if(pTxIED)
    {
        strTxIED = pTxIED->GetDisplayName();
        if(ProjectExplorer::PbTpPort *pTxPort = pTpInfoSet->GetTxPort())
            strTxIED = QString("%1<%2>").arg(strTxIED).arg(pTxPort->GetDisplayName());
    }
    pItem = new QStandardItem(strTxIED);
    pItem->setData(reinterpret_cast<int>(pTpInfoSet));
    lstItems.append(pItem);

    QString strSwitch1 = "";
    if(pSwitch1)
    {
        strSwitch1 = pSwitch1->GetDisplayName();
        if(ProjectExplorer::PbTpPort *pSwitch1TxPort = pTpInfoSet->GetSwitch1TxPort())
            strSwitch1 = QString("%1<%2>").arg(strSwitch1).arg(pSwitch1TxPort->GetDisplayName());

        if(ProjectExplorer::PbTpPort *pSwitch1RxPort = pTpInfoSet->GetSwitch1RxPort())
            strSwitch1 = QString("<%1>%2").arg(pSwitch1RxPort->GetDisplayName()).arg(strSwitch1);
    }
    pItem = new QStandardItem(strSwitch1);
    pItem->setData(reinterpret_cast<int>(pTpInfoSet));
    lstItems.append(pItem);

    QString strSwitch2 = "";
    if(pSwitch2)
    {
        strSwitch2 = pSwitch2->GetDisplayName();
        if(ProjectExplorer::PbTpPort *pSwitch2TxPort = pTpInfoSet->GetSwitch2TxPort())
            strSwitch2 = QString("%1<%2>").arg(strSwitch2).arg(pSwitch2TxPort->GetDisplayName());

        if(ProjectExplorer::PbTpPort *pSwitch2RxPort = pTpInfoSet->GetSwitch2RxPort())
            strSwitch2 = QString("<%1>%2").arg(pSwitch2RxPort->GetDisplayName()).arg(strSwitch2);
    }
    pItem = new QStandardItem(strSwitch2);
    pItem->setData(reinterpret_cast<int>(pTpInfoSet));
    lstItems.append(pItem);

    QString strSwitch3 = "";
    if(pSwitch3)
    {
        strSwitch3 = pSwitch3->GetDisplayName();
        if(ProjectExplorer::PbTpPort *pSwitch3TxPort = pTpInfoSet->GetSwitch3TxPort())
            strSwitch3 = QString("%1<%2>").arg(strSwitch3).arg(pSwitch3TxPort->GetDisplayName());

        if(ProjectExplorer::PbTpPort *pSwitch3RxPort = pTpInfoSet->GetSwitch3RxPort())
            strSwitch3 = QString("<%1>%2").arg(pSwitch3RxPort->GetDisplayName()).arg(strSwitch3);
    }
    pItem = new QStandardItem(strSwitch3);
    pItem->setData(reinterpret_cast<int>(pTpInfoSet));
    lstItems.append(pItem);

    QString strSwitch4 = "";
    if(pSwitch4)
    {
        strSwitch4 = pSwitch4->GetDisplayName();
        if(ProjectExplorer::PbTpPort *pSwitch4TxPort = pTpInfoSet->GetSwitch4TxPort())
            strSwitch4 = QString("%1<%2>").arg(strSwitch4).arg(pSwitch4TxPort->GetDisplayName());

        if(ProjectExplorer::PbTpPort *pSwitch4RxPort = pTpInfoSet->GetSwitch4RxPort())
            strSwitch4 = QString("<%1>%2").arg(pSwitch4RxPort->GetDisplayName()).arg(strSwitch4);
    }
    pItem = new QStandardItem(strSwitch4);
    pItem->setData(reinterpret_cast<int>(pTpInfoSet));
    lstItems.append(pItem);

    QString strRxIED = "";
    if(pRxIED)
    {
        strRxIED = pRxIED->GetDisplayName();
        if(ProjectExplorer::PbTpPort *pRxPort = pTpInfoSet->GetRxPort())
            strRxIED = QString("<%1>%3").arg(pRxPort->GetDisplayName()).arg(strRxIED);
    }
    pItem = new QStandardItem(strRxIED);
    pItem->setData(reinterpret_cast<int>(pTpInfoSet));
    lstItems.append(pItem);

    m_pModel->appendRow(lstItems);
    m_mapTpInfoSetToItem.insert(pTpInfoSet, lstItems.first());

    UpdateCompleteStatus(pTpInfoSet);
}

void ConfigWidgetTpInfoSet::SlotBaseObjectPropertyChanged(ProjectExplorer::PbBaseObject *pBaseObject)
{
    ProjectExplorer::PbTpInfoSet *pTpInfoSet = qobject_cast<ProjectExplorer::PbTpInfoSet*>(pBaseObject);
    if(!pTpInfoSet)
        return;

    QStandardItem *pItem = m_mapTpInfoSetToItem.value(pTpInfoSet, 0);
    if(!pItem)
        return;
    const int iRow = pItem->row();

    ProjectExplorer::PbTpDevice *pTxIED = pTpInfoSet->GetTxIED();
    ProjectExplorer::PbTpDevice *pRxIED = pTpInfoSet->GetRxIED();
    ProjectExplorer::PbTpDevice *pSwitch1 = pTpInfoSet->GetSwitch1();
    ProjectExplorer::PbTpDevice *pSwitch2 = pTpInfoSet->GetSwitch2();
    ProjectExplorer::PbTpDevice *pSwitch3 = pTpInfoSet->GetSwitch3();
    ProjectExplorer::PbTpDevice *pSwitch4 = pTpInfoSet->GetSwitch4();

    pItem = m_pModel->item(iRow, 0);
    pItem->setText(pTpInfoSet->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pTpInfoSet));

    pItem = m_pModel->item(iRow, 1);
    pItem->setText(ProjectExplorer::PbTpInfoSet::GetInfoSetTypeName(pTpInfoSet->GetInfoSetType()));
    pItem->setData(reinterpret_cast<int>(pTpInfoSet));

    QString strTxIED = "";
    if(pTxIED)
    {
        strTxIED = pTxIED->GetDisplayName();
        if(ProjectExplorer::PbTpPort *pTxPort = pTpInfoSet->GetTxPort())
            strTxIED = QString("%1<%2>").arg(strTxIED).arg(pTxPort->GetDisplayName());
    }
    pItem = m_pModel->item(iRow, 2);
    pItem->setText(strTxIED);
    pItem->setData(reinterpret_cast<int>(pTpInfoSet));

    QString strSwitch1 = "";
    if(pSwitch1)
    {
        strSwitch1 = pSwitch1->GetDisplayName();
        if(ProjectExplorer::PbTpPort *pSwitch1TxPort = pTpInfoSet->GetSwitch1TxPort())
            strSwitch1 = QString("%1<%2>").arg(strSwitch1).arg(pSwitch1TxPort->GetDisplayName());

        if(ProjectExplorer::PbTpPort *pSwitch1RxPort = pTpInfoSet->GetSwitch1RxPort())
            strSwitch1 = QString("<%1>%2").arg(pSwitch1RxPort->GetDisplayName()).arg(strSwitch1);
    }
    pItem = m_pModel->item(iRow, 3);
    pItem->setText(strSwitch1);

    QString strSwitch2 = "";
    if(pSwitch2)
    {
        strSwitch2 = pSwitch2->GetDisplayName();
        if(ProjectExplorer::PbTpPort *pSwitch2TxPort = pTpInfoSet->GetSwitch2TxPort())
            strSwitch2 = QString("%1<%2>").arg(strSwitch2).arg(pSwitch2TxPort->GetDisplayName());

        if(ProjectExplorer::PbTpPort *pSwitch2RxPort = pTpInfoSet->GetSwitch2RxPort())
            strSwitch2 = QString("<%1>%2").arg(pSwitch2RxPort->GetDisplayName()).arg(strSwitch2);
    }
    pItem = m_pModel->item(iRow, 4);
    pItem->setText(strSwitch2);

    QString strSwitch3 = "";
    if(pSwitch3)
    {
        strSwitch3 = pSwitch3->GetDisplayName();
        if(ProjectExplorer::PbTpPort *pSwitch3TxPort = pTpInfoSet->GetSwitch3TxPort())
            strSwitch3 = QString("%1<%2>").arg(strSwitch3).arg(pSwitch3TxPort->GetDisplayName());

        if(ProjectExplorer::PbTpPort *pSwitch3RxPort = pTpInfoSet->GetSwitch3RxPort())
            strSwitch3 = QString("<%1>%2").arg(pSwitch3RxPort->GetDisplayName()).arg(strSwitch3);
    }
    pItem = m_pModel->item(iRow, 5);
    pItem->setText(strSwitch3);

    QString strSwitch4 = "";
    if(pSwitch4)
    {
        strSwitch4 = pSwitch4->GetDisplayName();
        if(ProjectExplorer::PbTpPort *pSwitch4TxPort = pTpInfoSet->GetSwitch4TxPort())
            strSwitch4 = QString("%1<%2>").arg(strSwitch4).arg(pSwitch4TxPort->GetDisplayName());

        if(ProjectExplorer::PbTpPort *pSwitch4RxPort = pTpInfoSet->GetSwitch4RxPort())
            strSwitch4 = QString("<%1>%2").arg(pSwitch4RxPort->GetDisplayName()).arg(strSwitch4);
    }
    pItem = m_pModel->item(iRow, 6);
    pItem->setText(strSwitch4);

    QString strRxIED = "";
    if(pRxIED)
    {
        strRxIED = pRxIED->GetDisplayName();
        if(ProjectExplorer::PbTpPort *pRxPort = pTpInfoSet->GetRxPort())
            strRxIED = QString("<%1>%3").arg(pRxPort->GetDisplayName()).arg(strRxIED);
    }
    pItem = m_pModel->item(iRow, 7);
    pItem->setText(strRxIED);

    UpdateCompleteStatus(pTpInfoSet);
}

void ConfigWidgetTpInfoSet::SlotBaseObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject *pBaseObject)
{
    ProjectExplorer::PbTpInfoSet *pTpInfoSet = qobject_cast<ProjectExplorer::PbTpInfoSet*>(pBaseObject);
    if(!pTpInfoSet)
        return;

    if(QStandardItem *pItem = m_mapTpInfoSetToItem.value(pTpInfoSet, 0))
        m_pModel->removeRow(pItem->row());
}

void ConfigWidgetTpInfoSet::SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    QModelIndexList lstSelectedIndex = m_pView->selectionModel()->selectedRows();

    m_pActionNew->setEnabled(true);
    m_pActionDelete->setEnabled(!lstSelectedIndex.isEmpty());
    m_pActionProperty->setEnabled(lstSelectedIndex.size() == 1);
}

void ConfigWidgetTpInfoSet::SlotViewDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    SlotActionProperty();
}

void ConfigWidgetTpInfoSet::SlotActionNew()
{
    ProjectExplorer::PbTpInfoSet TpInfoSet;
    TpInfoSet.SetParentBay(m_pTpBay);

    ProjectExplorer::PbTpInfoSet TpInfoSetPair;
    TpInfoSetPair.SetParentBay(m_pTpBay);

    PropertyDlgTpInfoSet dlg(&TpInfoSet, &TpInfoSetPair, this);
    dlg.exec();
}

void ConfigWidgetTpInfoSet::SlotActionDelete()
{
    QModelIndexList lstSelectedIndex = m_pView->selectionModel()->selectedRows();
    qSort(lstSelectedIndex);

    QList<ProjectExplorer::PbTpInfoSet*> lstDeleteInfoSets;
    foreach(const QModelIndex &index, lstSelectedIndex)
    {
        if(QStandardItem *pItem = m_pModel->itemFromIndex(index))
        {
            if(ProjectExplorer::PbTpInfoSet *pTpInfoSet = reinterpret_cast<ProjectExplorer::PbTpInfoSet*>(pItem->data().toInt()))
            {
                if(!lstDeleteInfoSets.contains(pTpInfoSet))
                    lstDeleteInfoSets.append(pTpInfoSet);

                if(ProjectExplorer::PbTpInfoSet *pTpInfoSetPair = pTpInfoSet->GetPairInfoSet())
                {
                    if(!lstDeleteInfoSets.contains(pTpInfoSetPair))
                        lstDeleteInfoSets.append(pTpInfoSetPair);
                }
            }
        }
    }

    QString strItems;
    foreach(ProjectExplorer::PbTpInfoSet *pTpInfoSet, lstDeleteInfoSets)
    {
        if(pTpInfoSet->GetInfoSetType() == ProjectExplorer::PbTpInfoSet::itNone)
            continue;

        strItems = QString("%1\n%2: %3").arg(strItems).arg(pTpInfoSet->GetObjectTypeName()).arg(pTpInfoSet->GetDisplayName());
    }

    if(QMessageBox::question(this,
                             tr("Delete Confirmation"),
                             tr("Are you sure you want to delete following items?") + "\n" + strItems,
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No) == QMessageBox::Yes)
    {
        Utils::WaitCursor cursor;
        Q_UNUSED(cursor)

        foreach(ProjectExplorer::PbTpInfoSet *pTpInfoSet, lstDeleteInfoSets)
        {
            if(!ProjectExplorer::BaseManager::Instance()->DbDeleteObject(pTpInfoSet->GetObjectType(), pTpInfoSet->GetId(), false))
                return;
        }

        foreach(ProjectExplorer::PbTpInfoSet *pTpInfoSet, lstDeleteInfoSets)
            ProjectExplorer::BaseManager::Instance()->DeleteObject(pTpInfoSet);
    }
}

void ConfigWidgetTpInfoSet::SlotActionProperty()
{
    QItemSelectionModel *pSelectionModel = m_pView->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
    if(lstSelectedIndex.size() != 1)
        return;

    QStandardItem *pItem = m_pModel->itemFromIndex(lstSelectedIndex.first());
    if(!pItem)
        return;

    ProjectExplorer::PbTpInfoSet *pTpInfoSet = reinterpret_cast<ProjectExplorer::PbTpInfoSet*>(pItem->data().toInt());
    if(!pTpInfoSet)
        return;

    ProjectExplorer::PbTpInfoSet *pTpInfoSetPair = pTpInfoSet->GetPairInfoSet();
    if(!pTpInfoSetPair)
        return;

    ProjectExplorer::PbTpInfoSet TpInfoSet(*pTpInfoSet);
    ProjectExplorer::PbTpInfoSet TpInfoSetPair(*pTpInfoSetPair);
    PropertyDlgTpInfoSet dlg(&TpInfoSet, &TpInfoSetPair, this);
    dlg.exec();
}
