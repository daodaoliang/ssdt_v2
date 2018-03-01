#include <QApplication>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QFileIconProvider>
#include <QAction>
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>

#include "utils/waitcursor.h"
#include "utils/readonlyview.h"
#include "projectexplorer/basemanager.h"
#include "projectexplorer/pblibdevice.h"
#include "projectexplorer/pblibboard.h"
#include "projectexplorer/pblibport.h"
#include "core/configcoreoperation.h"

#include "devlibraryexplorerwidget.h"
#include "propertydlglibdevice.h"
#include "propertydlglibswitch.h"

using namespace DevLibrary::Internal;

DevLibraryExplorerWidget::DevLibraryExplorerWidget(QWidget *pParent) : QWidget(pParent), m_pCurrentItem(0)
{
    m_pModel = new QStandardItemModel(0, 1, this);
    m_pView = new Utils::ReadOnlyTreeView(this);
    m_pView->setStyleSheet("QTreeView {background-color: rgba(255, 255, 255, 240); }");
    m_pView->setFrameStyle(QFrame::NoFrame);
    m_pView->setIndentation(m_pView->indentation() * 9/10);
    m_pView->setUniformRowHeights(true);
    m_pView->setTextElideMode(Qt::ElideNone);
    m_pView->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_pView->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pView->setDragEnabled(true);
    m_pView->setHeaderHidden(true);
#if QT_VERSION >= 0x050000
   m_pView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_pView->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    m_pView->header()->setStretchLastSection(false);
    m_pView->setModel(m_pModel);

    QAction *pActionSeparator1 = new QAction(this);
    pActionSeparator1->setSeparator(true);
    QAction *pActionSeparator2 = new QAction(this);
    pActionSeparator2->setSeparator(true);
    QAction *pActionExpandAll = new QAction(tr("E&xpand All"), this);
    QAction *pActionCollapseAll = new QAction(tr("Co&llapse All"), this);

    m_pActionNewDevice = new QAction(ProjectExplorer::PbLibDevice::GetDeviceIcon(ProjectExplorer::PbLibDevice::dtIED), tr("New %1...").arg(ProjectExplorer::PbLibDevice::GetDeviceTypeName(ProjectExplorer::PbLibDevice::dtIED)), this);
    m_pActionNewSwitch = new QAction(ProjectExplorer::PbLibDevice::GetDeviceIcon(ProjectExplorer::PbLibDevice::dtSwitch), tr("New %1...").arg(ProjectExplorer::PbLibDevice::GetDeviceTypeName(ProjectExplorer::PbLibDevice::dtSwitch)), this);
    m_pActionDelete = new QAction(QIcon(":/devlibrary/images/oper_remove.png"), tr("&Delete"), this);
    m_pActionProperty = new QAction(QIcon(":/devlibrary/images/oper_edit.png"), tr("&Property..."), this);
    m_pActionExportIpcd = new QAction(tr("&Export IPCD File..."), this);

    QMenu *pMenuAddNew = new QMenu(tr("Add &New"), this);
    pMenuAddNew->addAction(m_pActionNewDevice);
    pMenuAddNew->addAction(m_pActionNewSwitch);
    m_pView->addAction(pMenuAddNew->menuAction());
    m_pView->addAction(m_pActionDelete);
    m_pView->addAction(m_pActionProperty);
    m_pView->addAction(pActionSeparator1);
    m_pView->addAction(m_pActionExportIpcd);
    m_pView->addAction(pActionSeparator2);
    m_pView->addAction(pActionExpandAll);
    m_pView->addAction(pActionCollapseAll);

    QVBoxLayout *pVBoxLayout = new QVBoxLayout(this);
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->addWidget(m_pView);

    connect(ProjectExplorer::BaseManager::Instance(), SIGNAL(sigInitialized()),
            this, SLOT(SlotBaseManagerInitialized()));
    connect(ProjectExplorer::BaseManager::Instance(), SIGNAL(sigUninitialized()),
            this, SLOT(SlotBaseManagerUninitialized()));
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
    connect(m_pActionNewDevice, SIGNAL(triggered()),
            this, SLOT(SlotActionNewDevice()));
    connect(m_pActionNewSwitch, SIGNAL(triggered()),
            this, SLOT(SlotActionNewSwitch()));
    connect(m_pActionDelete, SIGNAL(triggered()),
            this, SLOT(SlotActionDelete()));
    connect(m_pActionProperty, SIGNAL(triggered()),
            this, SLOT(SlotActionProperty()));
    connect(m_pActionExportIpcd, SIGNAL(triggered()),
            this, SLOT(SlotActionExportIpcd()));
    connect(pActionExpandAll, SIGNAL(triggered()),
            m_pView, SLOT(expandAll()));
    connect(pActionCollapseAll, SIGNAL(triggered()),
            m_pView, SLOT(collapseAll()));

    SlotBaseManagerInitialized();
}

DevLibraryExplorerWidget::~DevLibraryExplorerWidget()
{
}

QStandardItem* DevLibraryExplorerWidget::FindChildItem(QStandardItem *pItemParent, const QString &strText)
{
    if(pItemParent)
    {
        for(int i = 0; i < pItemParent->rowCount(); i++)
        {
            if(pItemParent->child(i)->text() == strText)
                return pItemParent->child(i);
        }
    }
    else
    {
        for(int i = 0; i < m_pModel->rowCount(); i++)
        {
            if(m_pModel->item(i)->text() == strText)
                return m_pModel->item(i);
        }
    }

    return 0;
}

void DevLibraryExplorerWidget::InsertChildItem(QStandardItem *pItemParent, QStandardItem *pItemChild)
{
    if(!pItemChild)
        return;

    if(pItemParent)
    {
        int iPos = pItemParent->rowCount();
        for(int i = 0; i < pItemParent->rowCount(); i++)
        {
            if(pItemChild->text() < pItemParent->child(i)->text())
            {
                iPos = i;
                break;
            }
        }

        pItemParent->insertRow(iPos, pItemChild);
    }
    else
    {
        int iPos = m_pModel->rowCount();
        for(int i = 0; i < m_pModel->rowCount(); i++)
        {
            if(pItemChild->text() < m_pModel->item(i)->text())
            {
                iPos = i;
                break;
            }
        }

        m_pModel->insertRow(iPos, pItemChild);
    }
}

bool DevLibraryExplorerWidget::CreateLibDevice(ProjectExplorer::PbLibDevice *pLibDeviceTemp, const QByteArray &baIcdContent)
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
    if(!pBaseManager->DbCreateObject(*pLibDeviceTemp))
        return false;

    if(!pBaseManager->DbUpdateLibDeviceModel(pLibDeviceTemp->GetId(), baIcdContent))
        return false;

    foreach(ProjectExplorer::PbLibBoard *pLibBoard, pLibDeviceTemp->GetChildBoards())
    {
        if(!pBaseManager->DbCreateObject(*pLibBoard))
            return false;

        foreach(ProjectExplorer::PbLibPort *pLibPort, pLibBoard->GetChildPorts())
        {
            if(!pBaseManager->DbCreateObject(*pLibPort))
                return false;
        }
    }

    ///////////////////////////////////////////////////////////////////////
    // Handle Object
    ///////////////////////////////////////////////////////////////////////
    ProjectExplorer::PbLibDevice *pLibDeviceCreated = qobject_cast<ProjectExplorer::PbLibDevice*>(pBaseManager->CreateObject(*pLibDeviceTemp));
    if(!pLibDeviceCreated)
        return false;

    foreach(ProjectExplorer::PbLibBoard *pLibBoard, pLibDeviceTemp->GetChildBoards())
    {
        pLibBoard->SetParentDevice(pLibDeviceCreated);

        ProjectExplorer::PbLibBoard *pLibBoardCreated = qobject_cast<ProjectExplorer::PbLibBoard*>(pBaseManager->CreateObject(*pLibBoard, false));
        if(!pLibBoardCreated)
            return false;

        foreach(ProjectExplorer::PbLibPort *pLibPort, pLibBoard->GetChildPorts())
        {
            pLibPort->SetParentBoard(pLibBoardCreated);

            ProjectExplorer::PbLibPort *pLibPortCreated = qobject_cast<ProjectExplorer::PbLibPort*>(pBaseManager->CreateObject(*pLibPort, false));
            if(!pLibPortCreated)
                return false;
        }
    }

    return true;
}

bool DevLibraryExplorerWidget::UpdateLibDevice(ProjectExplorer::PbLibDevice *pLibDeviceTemp, const QByteArray &baIcdContent)
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    ProjectExplorer::BaseManager *pBaseManager = ProjectExplorer::BaseManager::Instance();
    if(!pBaseManager)
        return false;

    ProjectExplorer::PbLibDevice *pLibDevice = pBaseManager->FindLibDeviceById(pLibDeviceTemp->GetId());
    if(!pLibDevice)
        return false;

    QList<ProjectExplorer::PbLibBoard*> lstCreatedLibBoards, lstDeletedLibBoards, lstUpdatedLibBoards;
    QList<ProjectExplorer::PbLibPort*> lstCreatedLibPorts, lstDeletedLibPorts, lstUpdatedLibPorts;

    QList<ProjectExplorer::PbLibBoard*> lstOldLibBoards = pLibDevice->GetChildBoards();
    QList<ProjectExplorer::PbLibBoard*> lstNewLibBoards = pLibDeviceTemp->GetChildBoards();
    foreach(ProjectExplorer::PbLibBoard *pNewLibBoard, lstNewLibBoards)
    {
        if(pNewLibBoard->GetId() == ProjectExplorer::PbBaseObject::m_iInvalidObjectId)
        {
            lstNewLibBoards.removeOne(pNewLibBoard);
            lstCreatedLibBoards.append(pNewLibBoard);

            lstCreatedLibPorts.append(pNewLibBoard->GetChildPorts());
        }
    }

    foreach(ProjectExplorer::PbLibBoard *pNewLibBoard, lstNewLibBoards)
    {
        ProjectExplorer::PbLibBoard *pOldLibBoard = pBaseManager->FindLibBoardById(pNewLibBoard->GetId());
        if(!pOldLibBoard)
            return false;

        if(!lstOldLibBoards.removeOne(pOldLibBoard))
            return false;

        QList<ProjectExplorer::PbLibPort*> lstOldLibPorts = pOldLibBoard->GetChildPorts();
        QList<ProjectExplorer::PbLibPort*> lstNewLibPorts = pNewLibBoard->GetChildPorts();
        foreach(ProjectExplorer::PbLibPort *pNewLibPort, lstNewLibPorts)
        {
            if(pNewLibPort->GetId() == ProjectExplorer::PbBaseObject::m_iInvalidObjectId)
            {
                lstNewLibPorts.removeOne(pNewLibPort);
                lstCreatedLibPorts.append(pNewLibPort);
            }
        }

        foreach(ProjectExplorer::PbLibPort *pNewLibPort, lstNewLibPorts)
        {
            ProjectExplorer::PbLibPort *pOldLibPort = pBaseManager->FindLibPortById(pNewLibPort->GetId());
            if(!pOldLibPort)
                return false;

            if(!lstOldLibPorts.removeOne(pOldLibPort))
                return false;
        }

        lstUpdatedLibPorts.append(lstNewLibPorts);
        lstDeletedLibPorts.append(lstOldLibPorts);
    }

    lstUpdatedLibBoards = lstNewLibBoards;
    lstDeletedLibBoards = lstOldLibBoards;

    ///////////////////////////////////////////////////////////////////////
    // Handle Database
    ///////////////////////////////////////////////////////////////////////

    // Update device
    if(!pBaseManager->DbUpdateObject(*pLibDeviceTemp))
        return false;

    if(!pBaseManager->DbUpdateLibDeviceModel(pLibDeviceTemp->GetId(), baIcdContent))
        return false;

    // Create board
    foreach(ProjectExplorer::PbLibBoard *pLibBoard, lstCreatedLibBoards)
    {
        if(!pBaseManager->DbCreateObject(*pLibBoard))
            return false;
    }

    // Update board
    foreach(ProjectExplorer::PbLibBoard *pLibBoard, lstUpdatedLibBoards)
    {
        if(!pBaseManager->DbUpdateObject(*pLibBoard))
            return false;
    }

    // Delete board
    foreach(ProjectExplorer::PbLibBoard *pLibBoard, lstDeletedLibBoards)
    {
        if(!pBaseManager->DbDeleteObject(pLibBoard->GetObjectType(), pLibBoard->GetId(), false))
            return false;
    }

    // Create port
    foreach(ProjectExplorer::PbLibPort *pLibPort, lstCreatedLibPorts)
    {
        if(!pBaseManager->DbCreateObject(*pLibPort))
            return false;
    }

    // Update port
    foreach(ProjectExplorer::PbLibPort *pLibPort, lstUpdatedLibPorts)
    {
        if(!pBaseManager->DbUpdateObject(*pLibPort))
            return false;
    }

    // Delete port
    foreach(ProjectExplorer::PbLibPort *pLibPort, lstDeletedLibPorts)
    {
        if(!pBaseManager->DbDeleteObject(pLibPort->GetObjectType(), pLibPort->GetId(), false))
            return false;
    }

    ///////////////////////////////////////////////////////////////////////
    // Handle Object
    ///////////////////////////////////////////////////////////////////////

    // Update device
    if(!pBaseManager->UpdateObject(*pLibDeviceTemp))
        return false;

    // Create board
    QMap<ProjectExplorer::PbLibBoard*, ProjectExplorer::PbLibBoard*> mapLibBoardOldToNew;
    foreach(ProjectExplorer::PbLibBoard *pLibBoard, lstCreatedLibBoards)
    {
        pLibBoard->SetParentDevice(pLibDevice);

        ProjectExplorer::PbLibBoard *pLibBoardCreated = qobject_cast<ProjectExplorer::PbLibBoard*>(pBaseManager->CreateObject(*pLibBoard, false));
        if(!pLibBoardCreated)
            return false;

        mapLibBoardOldToNew.insert(pLibBoard, pLibBoardCreated);
    }

    // Update board
    foreach(ProjectExplorer::PbLibBoard *pLibBoard, lstUpdatedLibBoards)
    {
        pLibBoard->SetParentDevice(pLibDevice);

        ProjectExplorer::PbLibBoard *pLibBoardUpdated = qobject_cast<ProjectExplorer::PbLibBoard*>(pBaseManager->UpdateObject(*pLibBoard, false));
        if(!pLibBoardUpdated)
            return false;

        mapLibBoardOldToNew.insert(pLibBoard, pLibBoardUpdated);
    }

    // Delete board
    foreach(ProjectExplorer::PbLibBoard *pLibBoard, lstDeletedLibBoards)
    {
        if(!pBaseManager->DeleteObject(pLibBoard, false))
            return false;
    }

    // Create port
    foreach(ProjectExplorer::PbLibPort *pLibPort, lstCreatedLibPorts)
    {
        ProjectExplorer::PbLibBoard *pLibBoardCreated = mapLibBoardOldToNew.value(pLibPort->GetParentBoard(), 0);
        if(!pLibBoardCreated)
            return false;

        pLibPort->SetParentBoard(pLibBoardCreated);

        ProjectExplorer::PbLibPort *pLibPortCreated = qobject_cast<ProjectExplorer::PbLibPort*>(pBaseManager->CreateObject(*pLibPort, false));
        if(!pLibPortCreated)
            return false;
    }

    // Update port
    foreach(ProjectExplorer::PbLibPort *pLibPort, lstUpdatedLibPorts)
    {
        ProjectExplorer::PbLibBoard *pLibBoardUpdated = mapLibBoardOldToNew.value(pLibPort->GetParentBoard(), 0);
        if(!pLibBoardUpdated)
            return false;

        pLibPort->SetParentBoard(pLibBoardUpdated);

        ProjectExplorer::PbLibPort *pLibPortUpdated = qobject_cast<ProjectExplorer::PbLibPort*>(pBaseManager->UpdateObject(*pLibPort, false));
        if(!pLibPortUpdated)
            return false;
    }

    // Delete port
    foreach(ProjectExplorer::PbLibPort *pLibPort, lstDeletedLibPorts)
    {
        if(!pBaseManager->DeleteObject(pLibPort, false))
            return false;
    }

    return true;
}

void DevLibraryExplorerWidget::SlotBaseManagerInitialized()
{
    m_pModel->clear();

    foreach(ProjectExplorer::PbLibDevice *pLibDevice, ProjectExplorer::BaseManager::Instance()->GetAllLibDevices())
        SlotBaseObjectCreated(pLibDevice);

    m_pView->expandAll();
}

void DevLibraryExplorerWidget::SlotBaseManagerUninitialized()
{
    m_pModel->clear();
}

void DevLibraryExplorerWidget::SlotBaseObjectCreated(ProjectExplorer::PbBaseObject *pBaseObject)
{
    ProjectExplorer::PbLibDevice *pLibDevice = qobject_cast<ProjectExplorer::PbLibDevice*>(pBaseObject);
    if(!pLibDevice)
        return;

    QFileIconProvider FileIconProvider;

    const QString strManufacture = pLibDevice->GetManufacture();
    QStandardItem *pItemManufacture = FindChildItem(0, strManufacture);
    if(!pItemManufacture)
    {
        pItemManufacture = new QStandardItem(FileIconProvider.icon(QFileIconProvider::Folder), strManufacture);
        InsertChildItem(0, pItemManufacture);
    }

    const QString strCategory = (pLibDevice->GetDeviceType() == ProjectExplorer::PbLibDevice::dtSwitch ? ProjectExplorer::PbLibDevice::GetDeviceTypeName(ProjectExplorer::PbLibDevice::dtSwitch) : pLibDevice->GetBayCategory() + pLibDevice->GetDeviceCategory());
    QStandardItem *pItemCategory = FindChildItem(pItemManufacture, strCategory);
    if(!pItemCategory)
    {
        pItemCategory = new QStandardItem(QIcon(":/devlibrary/images/category.png"), strCategory);
        InsertChildItem(pItemManufacture, pItemCategory);
    }

    QStandardItem *pItemType = new QStandardItem(pLibDevice->GetDisplayIcon(), pLibDevice->GetDisplayName());
    pItemType->setData(reinterpret_cast<int>(pLibDevice));
    pItemType->setDragEnabled(true);
    InsertChildItem(pItemCategory, pItemType);

    m_mapDeviceToItem.insert(pLibDevice, pItemType);

    m_pView->expand(pItemManufacture->index());
    m_pView->expand(pItemCategory->index());
}

void DevLibraryExplorerWidget::SlotBaseObjectPropertyChanged(ProjectExplorer::PbBaseObject *pBaseObject)
{
    ProjectExplorer::PbLibDevice *pLibDevice = qobject_cast<ProjectExplorer::PbLibDevice*>(pBaseObject);
    if(!pLibDevice)
        return;

    SlotBaseObjectAboutToBeDeleted(pLibDevice);
    SlotBaseObjectCreated(pLibDevice);
}

void DevLibraryExplorerWidget::SlotBaseObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject *pBaseObject)
{
    ProjectExplorer::PbLibDevice *pLibDevice = qobject_cast<ProjectExplorer::PbLibDevice*>(pBaseObject);
    if(!pLibDevice)
        return;

    if(QStandardItem *pItemType = m_mapDeviceToItem.value(pLibDevice, 0))
    {
        if(QStandardItem *pItemCategory = pItemType->parent())
        {
            pItemCategory->removeRow(pItemType->row());
            m_mapDeviceToItem.remove(pLibDevice);

            if(QStandardItem *pItemManufacture = pItemCategory->parent())
            {
                if(!pItemCategory->hasChildren())
                    pItemManufacture->removeRow(pItemCategory->row());

                if(!pItemManufacture->hasChildren())
                    m_pModel->removeRow(pItemManufacture->row());
            }
        }
    }
}

void DevLibraryExplorerWidget::SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    m_pActionDelete->setEnabled(false);
    m_pActionProperty->setEnabled(false);
    m_pActionExportIpcd->setEnabled(false);
    m_pCurrentItem = 0;

    QItemSelectionModel *pSelectionModel = m_pView->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
    if(lstSelectedIndex.size() == 1)
    {
        m_pCurrentItem = m_pModel->itemFromIndex(lstSelectedIndex.first());
        if(!m_pCurrentItem)
            return;

        m_pActionDelete->setEnabled(!m_pCurrentItem->hasChildren());
        m_pActionProperty->setEnabled(!m_pCurrentItem->hasChildren());
        m_pActionExportIpcd->setEnabled(!m_pCurrentItem->hasChildren());
    }
}

void DevLibraryExplorerWidget::SlotViewDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    SlotActionProperty();
}

void DevLibraryExplorerWidget::SlotActionNewDevice()
{
    ProjectExplorer::PbLibDevice *pLibDeviceTemp = new ProjectExplorer::PbLibDevice;
    pLibDeviceTemp->SetDeviceType(ProjectExplorer::PbLibDevice::dtIED);

    if(m_pCurrentItem)
    {
        if(ProjectExplorer::PbLibDevice *pLibDevice = reinterpret_cast<ProjectExplorer::PbLibDevice*>(m_pCurrentItem->data().toInt()))
        {
            pLibDeviceTemp->SetManufacture(pLibDevice->GetManufacture());
            pLibDeviceTemp->SetDeviceCategory(pLibDevice->GetDeviceCategory());
            pLibDeviceTemp->SetBayCategory(pLibDevice->GetBayCategory());
        }
        else if(!m_pCurrentItem->parent())
        {
            pLibDeviceTemp->SetManufacture(m_pCurrentItem->text());
        }
        else if(m_pCurrentItem->hasChildren())
        {
            if(ProjectExplorer::PbLibDevice *pLibDevice = reinterpret_cast<ProjectExplorer::PbLibDevice*>(m_pCurrentItem->child(0)->data().toInt()))
            {
                pLibDeviceTemp->SetManufacture(pLibDevice->GetManufacture());
                pLibDeviceTemp->SetDeviceCategory(pLibDevice->GetDeviceCategory());
                pLibDeviceTemp->SetBayCategory(pLibDevice->GetBayCategory());
            }
        }
    }

    PropertyDlgLibDevice dlg(pLibDeviceTemp, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    CreateLibDevice(pLibDeviceTemp, dlg.GetIcdContent());
}

void DevLibraryExplorerWidget::SlotActionNewSwitch()
{
    ProjectExplorer::PbLibDevice *pLibDeviceTemp = new ProjectExplorer::PbLibDevice;
    pLibDeviceTemp->SetDeviceType(ProjectExplorer::PbLibDevice::dtSwitch);

    if(m_pCurrentItem)
    {
        if(ProjectExplorer::PbLibDevice *pLibDevice = reinterpret_cast<ProjectExplorer::PbLibDevice*>(m_pCurrentItem->data().toInt()))
        {
            pLibDeviceTemp->SetManufacture(pLibDevice->GetManufacture());
        }
        else if(!m_pCurrentItem->parent())
        {
            pLibDeviceTemp->SetManufacture(m_pCurrentItem->text());
        }
        else if(m_pCurrentItem->hasChildren())
        {
            if(ProjectExplorer::PbLibDevice *pLibDevice = reinterpret_cast<ProjectExplorer::PbLibDevice*>(m_pCurrentItem->child(0)->data().toInt()))
            {
                pLibDeviceTemp->SetManufacture(pLibDevice->GetManufacture());
            }
        }
    }

    PropertyDlgLibSwitch dlg(pLibDeviceTemp, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    CreateLibDevice(pLibDeviceTemp, QByteArray());
}

void DevLibraryExplorerWidget::SlotActionDelete()
{
    if(!m_pCurrentItem)
        return;

    ProjectExplorer::PbLibDevice *pLibDevice = reinterpret_cast<ProjectExplorer::PbLibDevice*>(m_pCurrentItem->data().toInt());
    if(!pLibDevice)
        return;

    if(QMessageBox::question(this,
                             tr("Delete Confirmation"),
                             tr("Are you sure you want to delete %1 '%2'?").arg(pLibDevice->GetDeviceTypeName(pLibDevice->GetDeviceType())).arg(pLibDevice->GetDisplayName()),
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No) == QMessageBox::Yes)
    {
        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        if(ProjectExplorer::BaseManager::Instance()->DbDeleteObject(pLibDevice->GetObjectType(), pLibDevice->GetId()))
            ProjectExplorer::BaseManager::Instance()->DeleteObject(pLibDevice);
    }
}

void DevLibraryExplorerWidget::SlotActionProperty()
{
    if(!m_pCurrentItem)
        return;

    ProjectExplorer::PbLibDevice *pLibDevice = reinterpret_cast<ProjectExplorer::PbLibDevice*>(m_pCurrentItem->data().toInt());
    if(!pLibDevice)
        return;

    ProjectExplorer::PbLibDevice *pLibDeviceTemp = new ProjectExplorer::PbLibDevice(*pLibDevice);
    foreach(ProjectExplorer::PbLibBoard *pLibBoard, pLibDevice->GetChildBoards())
    {
        ProjectExplorer::PbLibBoard *pLibBoardTemp = new ProjectExplorer::PbLibBoard(*pLibBoard);
        pLibBoardTemp->SetParentDevice(pLibDeviceTemp);

        pLibDeviceTemp->AddChildBoard(pLibBoardTemp);
        pLibDeviceTemp->RemoveChildBoard(pLibBoard);

        foreach(ProjectExplorer::PbLibPort *pLibPort, pLibBoard->GetChildPorts())
        {
            ProjectExplorer::PbLibPort *pLibPortTemp = new ProjectExplorer::PbLibPort(*pLibPort);
            pLibPortTemp->SetParentBoard(pLibBoardTemp);

            pLibBoardTemp->AddChildPort(pLibPortTemp);
            pLibBoardTemp->RemoveChildPort(pLibPort);
        }
    }

    if(pLibDevice->GetDeviceType() == ProjectExplorer::PbLibDevice::dtIED)
    {
        PropertyDlgLibDevice dlg(pLibDeviceTemp, this);
        if(dlg.exec() != QDialog::Accepted)
            return;

        UpdateLibDevice(pLibDeviceTemp, dlg.GetIcdContent());
    }
    else if(pLibDevice->GetDeviceType() == ProjectExplorer::PbLibDevice::dtSwitch)
    {
        PropertyDlgLibSwitch dlg(pLibDeviceTemp, this);
        if(dlg.exec() != QDialog::Accepted)
            return;

        UpdateLibDevice(pLibDeviceTemp, QByteArray());
    }
}

void DevLibraryExplorerWidget::SlotActionExportIpcd()
{
    if(!m_pCurrentItem)
        return;

    ProjectExplorer::PbLibDevice *pLibDevice = reinterpret_cast<ProjectExplorer::PbLibDevice*>(m_pCurrentItem->data().toInt());
    if(!pLibDevice)
        return;

    const QString strFileName = QFileDialog::getSaveFileName(this,
                                                             tr("Export IPCD File"),
                                                             "template.ipcd",
                                                             tr("IED Physical Configuration Description File (*.ipcd)"));

    if(!strFileName.isEmpty())
    {
        qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
        bool bResult = Core::ConfigCoreOperation::Instance()->ExportIpcd(pLibDevice, strFileName);
        qApp->restoreOverrideCursor();

        if(bResult)
            QMessageBox::information(this, tr("Export Success"), tr("Succeed to export IPCD file!"));
        else
            QMessageBox::critical(this, tr("Export Failure"), tr("Failed to export IPCD file!"));
    }
}
