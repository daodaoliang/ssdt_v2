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

#include "configwidgettpdevice.h"
#include "propertydlgtpdevice.h"

using namespace BayTemplate::Internal;

ConfigWidgetTpDevice::ConfigWidgetTpDevice(ProjectExplorer::PbTpBay *pTpBay, QWidget *pParent) :
    QWidget(pParent), m_pTpBay(pTpBay)
{
    setMinimumSize(QSize(1200, 500));

    // Create Model
    m_pModel = new QStandardItemModel(0, 2, this);
    m_pModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_pModel->setHeaderData(1, Qt::Horizontal, tr("Type"));

    QList<ProjectExplorer::PbTpDevice*> lstTpDevices = pTpBay->GetChildDevices();
    qSort(lstTpDevices.begin(), lstTpDevices.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    foreach(ProjectExplorer::PbTpDevice *pTpDevice, lstTpDevices)
        SlotBaseObjectCreated(pTpDevice);

    // Create View
    m_pView = new Utils::ReadOnlyTableView(this);
    m_pView->setShowGrid(false);
    m_pView->setAlternatingRowColors(true);
    m_pView->setSortingEnabled(true);
    m_pView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pView->horizontalHeader()->setHighlightSections(false);
    m_pView->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_pView->horizontalHeader()->setStretchLastSection(true);
    m_pView->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pView->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pView->setModel(m_pModel);
    m_pView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pView->setColumnWidth(0, 170);
    m_pView->setColumnWidth(1, 70);

    m_pActionNewIED = new QAction(ProjectExplorer::PbTpDevice::GetDeviceIcon(ProjectExplorer::PbTpDevice::dtIED), tr("New %1...").arg(ProjectExplorer::PbTpDevice::GetDeviceTypeName(ProjectExplorer::PbTpDevice::dtIED)), this);
    m_pActionNewIED->setEnabled(true);
    connect(m_pActionNewIED, SIGNAL(triggered()), this, SLOT(SlotActionNewIED()));

    m_pActionNewSwitch = new QAction(ProjectExplorer::PbTpDevice::GetDeviceIcon(ProjectExplorer::PbTpDevice::dtSwitch), tr("New %1...").arg(ProjectExplorer::PbTpDevice::GetDeviceTypeName(ProjectExplorer::PbTpDevice::dtSwitch)), this);
    m_pActionNewSwitch->setEnabled(true);
    connect(m_pActionNewSwitch, SIGNAL(triggered()), this, SLOT(SlotActionNewSwitch()));

    QMenu *pMenuAddNew = new QMenu(tr("Add &New"), this);
    pMenuAddNew->addAction(m_pActionNewIED);
    pMenuAddNew->addAction(m_pActionNewSwitch);
    m_pView->addAction(pMenuAddNew->menuAction());

    QAction *pSeparator = new QAction(this);
    pSeparator->setSeparator(true);
    m_pView->addAction(pSeparator);

    m_pActionDelete = new QAction(QIcon(":/baytemplate/images/oper_remove.png"), tr("&Delete..."), this);
    m_pActionDelete->setEnabled(false);
    connect(m_pActionDelete, SIGNAL(triggered()), this, SLOT(SlotActionDelete()));
    m_pView->addAction(m_pActionDelete);

    pSeparator = new QAction(this);
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

ConfigWidgetTpDevice::~ConfigWidgetTpDevice()
{
}

bool ConfigWidgetTpDevice::CreateTpDevice(ProjectExplorer::PbTpDevice *pTpDeviceTemp)
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    ProjectExplorer::BaseManager *pBaseManager = ProjectExplorer::BaseManager::Instance();
    if(!pBaseManager)
        return false;

    ///////////////////////////////////////////////////////////////////////
    // Handle Database
    ///////////////////////////////////////////////////////////////////////

    // Create new device
    if(!pBaseManager->DbCreateObject(*pTpDeviceTemp))
        return false;

    foreach(ProjectExplorer::PbTpPort *pTpPort, pTpDeviceTemp->GetChildPorts())
    {
        if(!pBaseManager->DbCreateObject(*pTpPort))
            return false;
    }

    ///////////////////////////////////////////////////////////////////////
    // Handle Object
    ///////////////////////////////////////////////////////////////////////
    ProjectExplorer::PbTpDevice *pTpDeviceCreated = qobject_cast<ProjectExplorer::PbTpDevice*>(pBaseManager->CreateObject(*pTpDeviceTemp));
    if(!pTpDeviceCreated)
        return false;

    foreach(ProjectExplorer::PbTpPort *pTpPort, pTpDeviceTemp->GetChildPorts())
    {
        pTpPort->SetParentDevice(pTpDeviceCreated);

        ProjectExplorer::PbTpPort *pTpPortCreated = qobject_cast<ProjectExplorer::PbTpPort*>(pBaseManager->CreateObject(*pTpPort, false));
        if(!pTpPortCreated)
            return false;
    }

    return true;
}

bool ConfigWidgetTpDevice::UpdateTpDevice(ProjectExplorer::PbTpDevice *pTpDeviceTemp)
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    ProjectExplorer::BaseManager *pBaseManager = ProjectExplorer::BaseManager::Instance();
    if(!pBaseManager)
        return false;

    ProjectExplorer::PbTpDevice *pTpDevice = pBaseManager->FindTpDeviceById(pTpDeviceTemp->GetId());
    if(!pTpDevice)
        return false;

    QList<ProjectExplorer::PbTpPort*> lstCreatedTpPorts, lstDeletedTpPorts, lstUpdatedTpPorts;

    QList<ProjectExplorer::PbTpPort*> lstOldTpPorts = pTpDevice->GetChildPorts();
    QList<ProjectExplorer::PbTpPort*> lstNewTpPorts = pTpDeviceTemp->GetChildPorts();
    foreach(ProjectExplorer::PbTpPort *pNewTpPort, lstNewTpPorts)
    {
        if(pNewTpPort->GetId() == ProjectExplorer::PbBaseObject::m_iInvalidObjectId)
        {
            lstNewTpPorts.removeOne(pNewTpPort);
            lstCreatedTpPorts.append(pNewTpPort);
        }
    }

    foreach(ProjectExplorer::PbTpPort *pNewTpPort, lstNewTpPorts)
    {
        ProjectExplorer::PbTpPort *pOldTpPort = pBaseManager->FindTpPortById(pNewTpPort->GetId());
        if(!pOldTpPort)
            return false;

        if(!lstOldTpPorts.removeOne(pOldTpPort))
            return false;
    }

    lstUpdatedTpPorts = lstNewTpPorts;
    lstDeletedTpPorts = lstOldTpPorts;

    ///////////////////////////////////////////////////////////////////////
    // Handle Database
    ///////////////////////////////////////////////////////////////////////

    // Update device
    if(!pBaseManager->DbUpdateObject(*pTpDeviceTemp))
        return false;

    // Create port
    foreach(ProjectExplorer::PbTpPort *pTpPort, lstCreatedTpPorts)
    {
        if(!pBaseManager->DbCreateObject(*pTpPort))
            return false;
    }

    // Update port
    foreach(ProjectExplorer::PbTpPort *pTpPort, lstUpdatedTpPorts)
    {
        if(!pBaseManager->DbUpdateObject(*pTpPort))
            return false;
    }

    // Delete port
    foreach(ProjectExplorer::PbTpPort *pTpPort, lstDeletedTpPorts)
    {
        if(!pBaseManager->DbDeleteObject(pTpPort->GetObjectType(), pTpPort->GetId(), false))
            return false;
    }

    ///////////////////////////////////////////////////////////////////////
    // Handle Object
    ///////////////////////////////////////////////////////////////////////

    // Update device
    if(!pBaseManager->UpdateObject(*pTpDeviceTemp))
        return false;

    // Create port
    foreach(ProjectExplorer::PbTpPort *pTpPort, lstCreatedTpPorts)
    {
        pTpPort->SetParentDevice(pTpDevice);

        ProjectExplorer::PbTpPort *pTpPortCreated = qobject_cast<ProjectExplorer::PbTpPort*>(pBaseManager->CreateObject(*pTpPort, false));
        if(!pTpPortCreated)
            return false;
    }

    // Update port
    foreach(ProjectExplorer::PbTpPort *pTpPort, lstUpdatedTpPorts)
    {
        pTpPort->SetParentDevice(pTpDevice);

        ProjectExplorer::PbTpPort *pTpPortUpdated = qobject_cast<ProjectExplorer::PbTpPort*>(pBaseManager->UpdateObject(*pTpPort, false));
        if(!pTpPortUpdated)
            return false;
    }

    // Delete port
    foreach(ProjectExplorer::PbTpPort *pTpPort, lstDeletedTpPorts)
    {
        if(!pBaseManager->DeleteObject(pTpPort, false))
            return false;
    }

    return true;
}

bool ConfigWidgetTpDevice::DeleteTpDevice(const QList<ProjectExplorer::PbTpDevice*> &lstTpDevices)
{
    if(lstTpDevices.isEmpty())
        return true;

    QString strItems;
    foreach(ProjectExplorer::PbTpDevice *pTpDevice, lstTpDevices)
        strItems = QString("%1\n%2: %3").arg(strItems).arg(pTpDevice->GetObjectTypeName()).arg(pTpDevice->GetDisplayName());

    if(QMessageBox::question(this,
                             tr("Delete Confirmation"),
                             tr("Are you sure you want to delete following items?") + "\n" + strItems,
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No) == QMessageBox::Yes)
    {
        Utils::WaitCursor cursor;
        Q_UNUSED(cursor)

        ///////////////////////////////////////////////////////////////////////
        // Handle Database
        ///////////////////////////////////////////////////////////////////////

        foreach(ProjectExplorer::PbTpDevice *pTpDevice, lstTpDevices)
        {
            if(!ProjectExplorer::BaseManager::Instance()->DbDeleteObject(pTpDevice->GetObjectType(), pTpDevice->GetId(), false))
                return false;
        }

        ///////////////////////////////////////////////////////////////////////
        // Handle Object
        ///////////////////////////////////////////////////////////////////////

        foreach(ProjectExplorer::PbTpDevice *pTpDevice, lstTpDevices)
            ProjectExplorer::BaseManager::Instance()->DeleteObject(pTpDevice);
    }

    return true;
}

void ConfigWidgetTpDevice::SlotBaseObjectCreated(ProjectExplorer::PbBaseObject *pBaseObject)
{
    ProjectExplorer::PbTpDevice *pTpDevice = qobject_cast<ProjectExplorer::PbTpDevice*>(pBaseObject);
    if(!pTpDevice)
        return;

    QList<QStandardItem*> lstItems;

    QStandardItem *pItemName = new QStandardItem(pTpDevice->GetDisplayIcon(), pTpDevice->GetDisplayName());
    pItemName->setData(reinterpret_cast<int>(pTpDevice));
    lstItems.append(pItemName);

    QStandardItem *pItemType = new QStandardItem(pTpDevice->GetDeviceTypeName(pTpDevice->GetDeviceType()));
    pItemType->setData(reinterpret_cast<int>(pTpDevice));
    lstItems.append(pItemType);

    m_pModel->appendRow(lstItems);
}

void ConfigWidgetTpDevice::SlotBaseObjectPropertyChanged(ProjectExplorer::PbBaseObject *pBaseObject)
{
    ProjectExplorer::PbTpDevice *pTpDevice = qobject_cast<ProjectExplorer::PbTpDevice*>(pBaseObject);
    if(!pTpDevice)
        return;

    for(int i = 0; i < m_pModel->rowCount(); i++)
    {
        QStandardItem *pItemName = m_pModel->item(i, 0);
        if(!pItemName)
            continue;

        QStandardItem *pItemType = m_pModel->item(i, 1);
        if(!pItemType)
            continue;

        if(pTpDevice == reinterpret_cast<ProjectExplorer::PbTpDevice*>(pItemName->data().toInt()))
        {
            pItemName->setText(pTpDevice->GetDisplayName());
            pItemType->setText(pTpDevice->GetDeviceTypeName(pTpDevice->GetDeviceType()));

            break;
        }
    }
}

void ConfigWidgetTpDevice::SlotBaseObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject *pBaseObject)
{
    ProjectExplorer::PbTpDevice *pTpDevice = qobject_cast<ProjectExplorer::PbTpDevice*>(pBaseObject);
    if(!pTpDevice)
        return;

    for(int i = 0; i < m_pModel->rowCount(); i++)
    {
        QStandardItem *pItemName = m_pModel->item(i, 0);
        if(!pItemName)
            continue;

        if(pTpDevice == reinterpret_cast<ProjectExplorer::PbTpDevice*>(pItemName->data().toInt()))
        {
            m_pModel->removeRow(i);
            break;
        }
    }
}

void ConfigWidgetTpDevice::SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    QModelIndexList lstSelectedIndex = m_pView->selectionModel()->selectedRows();

    m_pActionNewIED->setEnabled(true);
    m_pActionNewSwitch->setEnabled(true);
    m_pActionDelete->setEnabled(!lstSelectedIndex.isEmpty());
    m_pActionProperty->setEnabled(lstSelectedIndex.size() == 1);
}

void ConfigWidgetTpDevice::SlotViewDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    SlotActionProperty();
}

void ConfigWidgetTpDevice::SlotActionNewIED()
{
    ProjectExplorer::PbTpDevice *pTpDeviceTemp = new ProjectExplorer::PbTpDevice;
    pTpDeviceTemp->SetParentBay(m_pTpBay);
    pTpDeviceTemp->SetDeviceType(ProjectExplorer::PbTpDevice::dtIED);

    PropertyDlgTpDevice dlg(pTpDeviceTemp, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    CreateTpDevice(pTpDeviceTemp);
}

void ConfigWidgetTpDevice::SlotActionNewSwitch()
{
    ProjectExplorer::PbTpDevice *pTpDeviceTemp = new ProjectExplorer::PbTpDevice;
    pTpDeviceTemp->SetParentBay(m_pTpBay);
    pTpDeviceTemp->SetDeviceType(ProjectExplorer::PbTpDevice::dtSwitch);

    PropertyDlgTpDevice dlg(pTpDeviceTemp, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    CreateTpDevice(pTpDeviceTemp);
}

void ConfigWidgetTpDevice::SlotActionDelete()
{
    QModelIndexList lstSelectedIndex = m_pView->selectionModel()->selectedRows();
    qSort(lstSelectedIndex);

    QList<ProjectExplorer::PbTpDevice*> lstDeleteTpDevices;
    foreach(const QModelIndex &index, lstSelectedIndex)
    {
        if(QStandardItem *pItem = m_pModel->itemFromIndex(index))
        {
            if(ProjectExplorer::PbTpDevice *pTpDevice = reinterpret_cast<ProjectExplorer::PbTpDevice*>(pItem->data().toInt()))
                lstDeleteTpDevices.append(pTpDevice);
        }
    }

    DeleteTpDevice(lstDeleteTpDevices);
}

void ConfigWidgetTpDevice::SlotActionProperty()
{
    QItemSelectionModel *pSelectionModel = m_pView->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
    if(lstSelectedIndex.size() != 1)
        return;

    QStandardItem *pItem = m_pModel->itemFromIndex(lstSelectedIndex.first());
    if(!pItem)
        return;

    ProjectExplorer::PbTpDevice *pTpDevice = reinterpret_cast<ProjectExplorer::PbTpDevice*>(pItem->data().toInt());
    if(!pTpDevice)
        return;

    ProjectExplorer::PbTpDevice *pTpDeviceTemp = new ProjectExplorer::PbTpDevice(*pTpDevice);
    foreach(ProjectExplorer::PbTpPort *pTpPort, pTpDevice->GetChildPorts())
    {
        ProjectExplorer::PbTpPort *pTpPortTemp = new ProjectExplorer::PbTpPort(*pTpPort);
        pTpPortTemp->SetParentDevice(pTpDeviceTemp);

        pTpDeviceTemp->AddChildPort(pTpPortTemp);
        pTpDeviceTemp->RemoveChildPort(pTpPort);
    }

    PropertyDlgTpDevice dlg(pTpDeviceTemp, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    UpdateTpDevice(pTpDeviceTemp);
}
