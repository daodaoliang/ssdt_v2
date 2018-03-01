#include <QIcon>
#include <QFont>
#include <QDebug>
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peproject.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"

#include "projectexplorermodel.h"

namespace Core {

static bool CompareItem(ProjectExplorerItem *pItem1, ProjectExplorerItem *pItem2)
{
    ProjectExplorer::PeProjectObject *pProjectObject1 = pItem1->GetProjectObject();
    ProjectExplorer::PeProjectObject *pProjectObject2 = pItem2->GetProjectObject();

    if(pProjectObject1->GetObjectType() != pProjectObject2->GetObjectType())
    {
        return pProjectObject1->GetObjectType() < pProjectObject2->GetObjectType();
    }
    else
    {
        if(pProjectObject1->GetObjectType() == ProjectExplorer::PeProjectObject::otDevice)
        {
            ProjectExplorer::PeDevice *pDevice1 = reinterpret_cast<ProjectExplorer::PeDevice*>(pProjectObject1);
            ProjectExplorer::PeDevice *pDevice2 = reinterpret_cast<ProjectExplorer::PeDevice*>(pProjectObject2);
            if(pDevice1->GetDeviceType() != pDevice2->GetDeviceType())
            {
                return pDevice1->GetDeviceType() < pDevice2->GetDeviceType();
            }
        }
    }

    return pProjectObject1->GetDisplayName() <= pProjectObject2->GetDisplayName();
}

static ProjectExplorer::PeProjectObject* GetProjectObjectParent(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!pProjectObject)
        return 0;

    ProjectExplorer::PeProjectObject *pProjectObjectParent = 0;
    switch(pProjectObject->GetObjectType())
    {
        case ProjectExplorer::PeProjectObject::otRoom:
        {
            if(ProjectExplorer::PeRoom *pRoom = qobject_cast<ProjectExplorer::PeRoom*>(pProjectObject))
                pProjectObjectParent = pRoom->GetProjectVersion();
        }
        break;

        case ProjectExplorer::PeProjectObject::otCubicle:
        {
            if(ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectObject))
            {
                pProjectObjectParent = pCubicle->GetProjectVersion();
                if(pCubicle->GetParentRoom())
                    pProjectObjectParent = pCubicle->GetParentRoom();
            }
        }
        break;

        case ProjectExplorer::PeProjectObject::otDevice:
        {
            if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
            {
                if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
                {
                    pProjectObjectParent = pDevice->GetProjectVersion();
                    if(pDevice->GetParentCubicle())
                        pProjectObjectParent = pDevice->GetParentCubicle();
                }
            }
        }
        break;

        case ProjectExplorer::PeProjectObject::otBoard:
        {
            if(ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pProjectObject))
            {
                if(ProjectExplorer::PeDevice *pDevice = pBoard->GetParentDevice())
                {
                    if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF && pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
                        pProjectObjectParent = pDevice;
                }
            }
        }
        break;
    }

    return pProjectObjectParent;
}

static ProjectExplorer::PeProjectObject* GetProjectObjectBay(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!pProjectObject)
        return 0;

    ProjectExplorer::PeProjectObject *pProjectObjectBay = 0;
    switch(pProjectObject->GetObjectType())
    {
        case ProjectExplorer::PeProjectObject::otBay:
        {
            if(ProjectExplorer::PeBay *pBay = qobject_cast<ProjectExplorer::PeBay*>(pProjectObject))
                pProjectObjectBay = pBay->GetProjectVersion();
        }
        break;

        case ProjectExplorer::PeProjectObject::otDevice:
        {
            if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
            {
                if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
                {
                    pProjectObjectBay = pDevice->GetProjectVersion();
                    if(pDevice->GetParentBay())
                        pProjectObjectBay = pDevice->GetParentBay();
                }
            }
        }
        break;

        case ProjectExplorer::PeProjectObject::otBoard:
        {
            if(ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pProjectObject))
            {
                if(ProjectExplorer::PeDevice *pDevice = pBoard->GetParentDevice())
                {
                    if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF && pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
                        pProjectObjectBay = pDevice;
                }
            }
        }
        break;
    }

    return pProjectObjectBay;
}

///////////////////////////////////////////////////////////////////////
// ProjectExplorerItem member functions
///////////////////////////////////////////////////////////////////////

class ProjectExplorerItemPrivate
{
public:
    ProjectExplorerItemPrivate(ProjectExplorer::PeProjectObject *pProjectObject, ProjectExplorerItem *pItemParent)
        : m_pProjectObject(pProjectObject), m_pItemParent(pItemParent), m_bBold(false)
    {
    }

    ProjectExplorer::PeProjectObject    *m_pProjectObject;
    ProjectExplorerItem                 *m_pItemParent;
    QList<ProjectExplorerItem*>         m_lstChildItems;
    bool                                m_bBold;
};

ProjectExplorerItem::ProjectExplorerItem(ProjectExplorer::PeProjectObject *pProjectObject, ProjectExplorerItem *pItemParent)
    : m_d(new ProjectExplorerItemPrivate(pProjectObject, pItemParent))
{
}

ProjectExplorerItem::~ProjectExplorerItem()
{

}

ProjectExplorer::PeProjectObject* ProjectExplorerItem::GetProjectObject() const
{
    return m_d->m_pProjectObject;
}

ProjectExplorerItem* ProjectExplorerItem::GetParentItem() const
{
    return m_d->m_pItemParent;
}

QList<ProjectExplorerItem*> ProjectExplorerItem::GetChildItems() const
{
    return m_d->m_lstChildItems;
}

QString ProjectExplorerItem::GetItemName() const
{
    if(m_d->m_pProjectObject)
        return m_d->m_pProjectObject->GetDisplayName();

    return "";
}

QIcon ProjectExplorerItem::GetItemIcon() const
{
    if(m_d->m_pProjectObject)
        return m_d->m_pProjectObject->GetDisplayIcon();

    return QIcon();
}

void ProjectExplorerItem::SetBold(bool bBold)
{
    m_d->m_bBold = bBold;
}

bool ProjectExplorerItem::GetBold() const
{
    return m_d->m_bBold;
}

void ProjectExplorerItem::SetParentItem(ProjectExplorerItem *pItemParent)
{
    m_d->m_pItemParent = pItemParent;
}

void ProjectExplorerItem::InsertChildItem(ProjectExplorerItem *pChildItem)
{
    // Find the correct place to insert file
    if(m_d->m_lstChildItems.size() == 0 || CompareItem(m_d->m_lstChildItems.last(), pChildItem))
    {
        m_d->m_lstChildItems.append(pChildItem);
    }
    else
    {
        // Binary Search for insertion point
        int iLeft = 0;
        int iRight = m_d->m_lstChildItems.count();
        while(iLeft != iRight)
        {
            int i = (iLeft + iRight) / 2;
            if(CompareItem(pChildItem, m_d->m_lstChildItems.at(i)))
                iRight = i;
            else
                iLeft = i + 1;
        }

        m_d->m_lstChildItems.insert(iLeft, pChildItem);
    }
}

int ProjectExplorerItem::InsertChildItem(ProjectExplorerItem *pChildItem, int iPos)
{
    if(iPos < 0 || iPos > m_d->m_lstChildItems.size())
    {
        // Find the correct place to insert file
        if(m_d->m_lstChildItems.size() == 0 || CompareItem(m_d->m_lstChildItems.last(), pChildItem))
        {
            iPos = m_d->m_lstChildItems.size();
        }
        else
        {
            // Binary Search for insertion point
            int iLeft = 0;
            int iRight = m_d->m_lstChildItems.count();
            while(iLeft != iRight)
            {
                int i = (iLeft + iRight) / 2;
                if(CompareItem(pChildItem, m_d->m_lstChildItems.at(i)))
                    iRight = i;
                else
                    iLeft = i + 1;
            }

            iPos = iLeft;
        }
    }
    else
    {
        m_d->m_lstChildItems.insert(iPos, pChildItem);
    }

    return iPos;
}

ProjectExplorerItem* ProjectExplorerItem::TakeChildItem(ProjectExplorerItem *pChildItem)
{
    int iIndex = m_d->m_lstChildItems.indexOf(pChildItem);
    if(iIndex == -1)
        return 0;

    return m_d->m_lstChildItems.takeAt(iIndex);
}

void ProjectExplorerItem::RemoveChildItem(ProjectExplorerItem *pChildItem)
{
    if(m_d->m_lstChildItems.removeOne(pChildItem))
        delete pChildItem;
}

///////////////////////////////////////////////////////////////////////
// ProjectExplorerModel member functions
///////////////////////////////////////////////////////////////////////
class ProjectExplorerModelPrivate
{
public:
    ProjectExplorerModelPrivate(const ProjectExplorerModel::ModelType &eModelType) : m_eModelType(eModelType)
    {
    }

    QList<ProjectExplorerItem*> m_lstRootItems;
    ProjectExplorerModel::ModelType m_eModelType;
    QMap<ProjectExplorer::PeProjectObject*, ProjectExplorerItem*> m_mapObjectToItem;
};

ProjectExplorerModel::ProjectExplorerModel(const ModelType &eModelType, QObject *pParent) :
    QAbstractItemModel(pParent), m_d(new ProjectExplorerModelPrivate(eModelType))
{
    Refresh();

    connect(ProjectExplorer::ProjectManager::Instance(), SIGNAL(sigProjectVersionOpened(ProjectExplorer::PeProjectVersion*)),
            this, SLOT(SlotProjectVersionOpened(ProjectExplorer::PeProjectVersion*)));
    connect(ProjectExplorer::ProjectManager::Instance(), SIGNAL(sigProjectVersionAboutToBeClosed(ProjectExplorer::PeProjectVersion*)),
            this, SLOT(SlotProjectVersionAboutToBeClosed(ProjectExplorer::PeProjectVersion*)));
    connect(ProjectExplorer::ProjectManager::Instance(), SIGNAL(sigProjectVersionPropertyChanged(ProjectExplorer::PeProjectVersion*)),
            this, SLOT(SlotProjectVersionPropertyChanged(ProjectExplorer::PeProjectVersion*)));
}

ProjectExplorerModel::~ProjectExplorerModel()
{
    qDeleteAll(m_d->m_lstRootItems);
    m_d->m_lstRootItems.clear();
}

QModelIndex ProjectExplorerModel::index(int nRow, int nColumn, const QModelIndex &Parent) const
{
    if(!hasIndex(nRow, nColumn, Parent))
        return QModelIndex();

    if(ProjectExplorerItem *pItemParent = ItemFromIndex(Parent))
        return createIndex(nRow, nColumn, pItemParent->GetChildItems().at(nRow));

    return createIndex(nRow, nColumn, m_d->m_lstRootItems.at(nRow));
}

QModelIndex ProjectExplorerModel::parent(const QModelIndex &child) const
{
    if(ProjectExplorerItem *pItem = ItemFromIndex(child))
    {
        if(ProjectExplorerItem *pItemParent = pItem->GetParentItem())
        {
            if(m_d->m_lstRootItems.contains(pItemParent))
                return createIndex(m_d->m_lstRootItems.indexOf(pItemParent), 0, pItemParent);

            if(ProjectExplorerItem *pItemGrandParent = pItemParent->GetParentItem())
            {
                const int iRow = pItemGrandParent->GetChildItems().indexOf(pItemParent);
                Q_ASSERT(iRow >= 0);

                return createIndex(iRow, 0, pItemParent);
            }
        }
    }

    return QModelIndex();
}

int ProjectExplorerModel::rowCount(const QModelIndex &parent) const
{
    if(parent.column() > 0)
        return 0;

    if(ProjectExplorerItem *pItemParent = ItemFromIndex(parent))
        return pItemParent->GetChildItems().size();

    return m_d->m_lstRootItems.size();
}

int ProjectExplorerModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

QVariant ProjectExplorerModel::data(const QModelIndex &index, int role) const
{
    if(ProjectExplorerItem *pItem = ItemFromIndex(index))
    {
        if(role == Qt::DisplayRole || role == Qt::EditRole)
        {
            return pItem->GetItemName();
        }
        else if(role == Qt::DecorationRole)
        {
            return pItem->GetItemIcon();
        }
        else if(role == Qt::FontRole)
        {
            QFont font;
            font.setBold(pItem->GetBold());
            return font;
        }
    }

    return QVariant();
}

Qt::ItemFlags ProjectExplorerModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;
}

void ProjectExplorerModel::Refresh()
{
#if QT_VERSION >= 0x050000
    beginResetModel();
#endif

    qDeleteAll(m_d->m_lstRootItems);
    m_d->m_lstRootItems.clear();

#if QT_VERSION >= 0x050000
    endResetModel();
#else
    reset();
#endif

    foreach(ProjectExplorer::PeProject *pProject, ProjectExplorer::ProjectManager::Instance()->GetAllProjects())
    {
        foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, pProject->GetAllProjectVersions())
        {
            if(pProjectVersion->IsOpend())
            {
                disconnect(pProjectVersion, 0, this, 0);
                SlotProjectVersionOpened(pProjectVersion);
            }
        }
    }
}

void ProjectExplorerModel::SetModelType(const ModelType &eModelType)
{
    if(m_d->m_eModelType == eModelType)
        return;

    m_d->m_eModelType = eModelType;
    Refresh();
}

ProjectExplorerModel::ModelType ProjectExplorerModel::GetModelType() const
{
    return m_d->m_eModelType;
}

ProjectExplorerItem* ProjectExplorerModel::ItemFromIndex(const QModelIndex &index) const
{
    if(index.isValid())
        return static_cast<ProjectExplorerItem*>(index.internalPointer());

    return 0;
}

ProjectExplorerItem* ProjectExplorerModel::ItemFromObject(ProjectExplorer::PeProjectObject *pProjectObject) const
{
    return m_d->m_mapObjectToItem.value(pProjectObject, 0);
}

QModelIndex ProjectExplorerModel::IndexForItem(ProjectExplorerItem *pItem) const
{
    if(m_d->m_lstRootItems.contains(pItem))
        return createIndex(m_d->m_lstRootItems.indexOf(pItem), 0, pItem);

    if(ProjectExplorerItem *pItemParent = pItem->GetParentItem())
        return createIndex(pItemParent->GetChildItems().indexOf(pItem), 0, pItem);

    return QModelIndex();
}

QModelIndex ProjectExplorerModel::IndexForObject(ProjectExplorer::PeProjectObject *pProjectObject) const
{
    if(ProjectExplorerItem *pItem = ItemFromObject(pProjectObject))
        return IndexForItem(pItem);

    return QModelIndex();
}

void ProjectExplorerModel::InsertItem(ProjectExplorerItem *pItem, ProjectExplorerItem *pItemParent)
{
    if(!pItem || !pItem->GetProjectObject())
        return;

    if(pItem->GetParentItem() != pItemParent)
        pItem->SetParentItem(pItemParent);

    if(pItemParent)
    {
        int iPos = pItemParent->InsertChildItem(pItem, -1);
        beginInsertRows(IndexForItem(pItemParent), iPos, iPos);
        pItemParent->InsertChildItem(pItem, iPos);
        m_d->m_mapObjectToItem.insert(pItem->GetProjectObject(), pItem);
        endInsertRows();
    }
    else
    {
        int iRowInsert = m_d->m_lstRootItems.size();
        for(int i = 0; i < m_d->m_lstRootItems.size(); i++)
        {
            if(pItem->GetItemName() <= m_d->m_lstRootItems.at(i)->GetItemName())
            {
                iRowInsert = i;
                break;
            }
        }

        beginInsertRows(QModelIndex(), iRowInsert, iRowInsert);
        m_d->m_lstRootItems.insert(iRowInsert, pItem);
        m_d->m_mapObjectToItem.insert(pItem->GetProjectObject(), pItem);
        endInsertRows();
    }
}

void ProjectExplorerModel::RemoveItem(ProjectExplorerItem *pItem)
{
    if(!pItem || !pItem->GetProjectObject())
        return;

    QModelIndex index = IndexForItem(pItem);
    if(!index.isValid())
        return;

    if(m_d->m_lstRootItems.contains(pItem))
    {
        beginRemoveRows(QModelIndex(), index.row(), index.row());
        m_d->m_mapObjectToItem.remove(pItem->GetProjectObject());
        m_d->m_lstRootItems.removeOne(pItem);
        endRemoveRows();
    }
    else
    {
        ProjectExplorerItem *pItemParent = pItem->GetParentItem();
        if(!pItemParent)
            return;

        QModelIndex indexParent = IndexForItem(pItemParent);
        if(!index.isValid())
            return;

        beginRemoveRows(indexParent, index.row(), index.row());
        m_d->m_mapObjectToItem.remove(pItem->GetProjectObject());
        pItemParent->RemoveChildItem(pItem);
        endRemoveRows();
    }
}

ProjectExplorerItem* ProjectExplorerModel::TakeItem(ProjectExplorerItem *pItem)
{
    if(!pItem || !pItem->GetProjectObject())
        return 0;

    QModelIndex index = IndexForItem(pItem);
    if(!index.isValid())
        return 0;

    ProjectExplorerItem *pItemTaken = 0;
    if(m_d->m_lstRootItems.contains(pItem))
    {
        beginRemoveRows(QModelIndex(), index.row(), index.row());
        m_d->m_mapObjectToItem.remove(pItem->GetProjectObject());
        pItemTaken = m_d->m_lstRootItems.takeAt(index.row());
        endRemoveRows();
    }
    else
    {
        ProjectExplorerItem *pItemParent = pItem->GetParentItem();
        if(!pItemParent)
            return 0;

        QModelIndex indexParent = IndexForItem(pItemParent);
        if(!index.isValid())
            return 0;

        beginRemoveRows(indexParent, index.row(), index.row());
        m_d->m_mapObjectToItem.remove(pItem->GetProjectObject());
        pItemTaken = pItemParent->TakeChildItem(pItem);
        endRemoveRows();
    }

    return pItemTaken;
}

void ProjectExplorerModel::SlotProjectVersionOpened(ProjectExplorer::PeProjectVersion *pProjectVersion)
{
    if(!pProjectVersion)
        return;

    ProjectExplorerItem *pItemProject = new ProjectExplorerItem(pProjectVersion, 0);
    if(m_d->m_eModelType == mtPhysical)
    {
        QList<ProjectExplorer::PeCubicle*> lstCubicles = pProjectVersion->GetAllCubicles();

        QList<ProjectExplorer::PeDevice*> lstDevices;
        foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
        {
            if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
                lstDevices.append(pDevice);
        }

        foreach(ProjectExplorer::PeRoom *pRoom, pProjectVersion->GetAllRooms())
        {
            ProjectExplorerItem *pItemRoom = new ProjectExplorerItem(pRoom, pItemProject);
            pItemProject->InsertChildItem(pItemRoom);
            m_d->m_mapObjectToItem.insert(pRoom, pItemRoom);

            foreach(ProjectExplorer::PeCubicle *pCubicle, pRoom->GetChildCubicles())
            {
                ProjectExplorerItem *pItemCubicle = new ProjectExplorerItem(pCubicle, pItemRoom);
                pItemRoom->InsertChildItem(pItemCubicle);
                m_d->m_mapObjectToItem.insert(pCubicle, pItemCubicle);

                foreach(ProjectExplorer::PeDevice *pDevice, pCubicle->GetChildDevices())
                {
                    if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                        continue;

                    ProjectExplorerItem *pItemDevice = new ProjectExplorerItem(pDevice, pItemCubicle);
                    pItemCubicle->InsertChildItem(pItemDevice);
                    m_d->m_mapObjectToItem.insert(pDevice, pItemDevice);
                    lstDevices.removeOne(pDevice);

                    if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
                    {
                        foreach(ProjectExplorer::PeBoard *pBoard, pDevice->GetChildBoards())
                        {
                            ProjectExplorerItem *pItemBoard = new ProjectExplorerItem(pBoard, pItemDevice);
                            pItemDevice->InsertChildItem(pItemBoard);
                            m_d->m_mapObjectToItem.insert(pBoard, pItemBoard);
                        }
                    }
                }

                lstCubicles.removeOne(pCubicle);
            }
        }

        foreach(ProjectExplorer::PeCubicle *pCubicle, lstCubicles)
        {
            ProjectExplorerItem *pItemCubicle = new ProjectExplorerItem(pCubicle, pItemProject);
            pItemProject->InsertChildItem(pItemCubicle);
            m_d->m_mapObjectToItem.insert(pCubicle, pItemCubicle);

            foreach(ProjectExplorer::PeDevice *pDevice, pCubicle->GetChildDevices())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                    continue;

                ProjectExplorerItem *pItemDevice = new ProjectExplorerItem(pDevice, pItemCubicle);
                pItemCubicle->InsertChildItem(pItemDevice);
                m_d->m_mapObjectToItem.insert(pDevice, pItemDevice);
                lstDevices.removeOne(pDevice);

                if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
                {
                    foreach(ProjectExplorer::PeBoard *pBoard, pDevice->GetChildBoards())
                    {
                        ProjectExplorerItem *pItemBoard = new ProjectExplorerItem(pBoard, pItemDevice);
                        pItemDevice->InsertChildItem(pItemBoard);
                        m_d->m_mapObjectToItem.insert(pBoard, pItemBoard);
                    }
                }
            }
        }

        foreach(ProjectExplorer::PeDevice *pDevice, lstDevices)
        {
            ProjectExplorerItem *pItemDevice = new ProjectExplorerItem(pDevice, pItemProject);
            pItemProject->InsertChildItem(pItemDevice);
            m_d->m_mapObjectToItem.insert(pDevice, pItemDevice);

            if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
            {
                foreach(ProjectExplorer::PeBoard *pBoard, pDevice->GetChildBoards())
                {
                    ProjectExplorerItem *pItemBoard = new ProjectExplorerItem(pBoard, pItemDevice);
                    pItemDevice->InsertChildItem(pItemBoard);
                    m_d->m_mapObjectToItem.insert(pBoard, pItemBoard);
                }
            }
        }
    }
    else if(m_d->m_eModelType == mtElectrical)
    {
        QList<ProjectExplorer::PeDevice*> lstDevices;
        foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
        {
            if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
                lstDevices.append(pDevice);
        }

        foreach(ProjectExplorer::PeBay *pBay, pProjectVersion->GetAllBays())
        {
            ProjectExplorerItem *pItemBay = new ProjectExplorerItem(pBay, pItemProject);
            pItemProject->InsertChildItem(pItemBay);
            m_d->m_mapObjectToItem.insert(pBay, pItemBay);

            foreach(ProjectExplorer::PeDevice *pDevice, pBay->GetChildDevices())
            {
                ProjectExplorerItem *pItemDevice = new ProjectExplorerItem(pDevice, pItemBay);
                pItemBay->InsertChildItem(pItemDevice);
                m_d->m_mapObjectToItem.insert(pDevice, pItemDevice);
                lstDevices.removeOne(pDevice);

                if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
                {
                    foreach(ProjectExplorer::PeBoard *pBoard, pDevice->GetChildBoards())
                    {
                        ProjectExplorerItem *pItemBoard = new ProjectExplorerItem(pBoard, pItemDevice);
                        pItemDevice->InsertChildItem(pItemBoard);
                        m_d->m_mapObjectToItem.insert(pBoard, pItemBoard);
                    }
                }
            }
        }

        foreach(ProjectExplorer::PeDevice *pDevice, lstDevices)
        {
            ProjectExplorerItem *pItemDevice = new ProjectExplorerItem(pDevice, pItemProject);
            pItemProject->InsertChildItem(pItemDevice);
            m_d->m_mapObjectToItem.insert(pDevice, pItemDevice);

            if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
            {
                foreach(ProjectExplorer::PeBoard *pBoard, pDevice->GetChildBoards())
                {
                    ProjectExplorerItem *pItemBoard = new ProjectExplorerItem(pBoard, pItemDevice);
                    pItemDevice->InsertChildItem(pItemBoard);
                    m_d->m_mapObjectToItem.insert(pBoard, pItemBoard);
                }
            }
        }
    }

    InsertItem(pItemProject, 0);

    connect(pProjectVersion, SIGNAL(sigObjectCreated(ProjectExplorer::PeProjectObject*)),
            this, SLOT(SlotProjectObjectCreated(ProjectExplorer::PeProjectObject*)));
    connect(pProjectVersion, SIGNAL(sigObjectPropertyChanged(ProjectExplorer::PeProjectObject*)),
            this, SLOT(SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject*)));
    connect(pProjectVersion, SIGNAL(sigObjectParentChanged(ProjectExplorer::PeProjectObject*,ProjectExplorer::PeProjectObject*)),
            this, SLOT(SlotProjectObjectParentChanged(ProjectExplorer::PeProjectObject*,ProjectExplorer::PeProjectObject*)));
    connect(pProjectVersion, SIGNAL(sigDeviceBayChanged(ProjectExplorer::PeDevice*,ProjectExplorer::PeBay*)),
            this, SLOT(SlotDeviceBayChanged(ProjectExplorer::PeDevice*,ProjectExplorer::PeBay*)));
    connect(pProjectVersion, SIGNAL(sigObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject*)),
            this, SLOT(SlotProjectObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject*)));
}

void ProjectExplorerModel::SlotProjectVersionAboutToBeClosed(ProjectExplorer::PeProjectVersion *pProjectVersion)
{
    if(!pProjectVersion)
        return;

    if(ProjectExplorerItem *pItem = m_d->m_mapObjectToItem.value(pProjectVersion, 0))
    {
        foreach(ProjectExplorer::PeProjectObject *pProjectObject, m_d->m_mapObjectToItem.keys())
        {
            if(pProjectObject->GetProjectVersion() == pProjectVersion)
                m_d->m_mapObjectToItem.remove(pProjectObject);
        }

        RemoveItem(pItem);
    }
}

void ProjectExplorerModel::SlotProjectVersionPropertyChanged(ProjectExplorer::PeProjectVersion *pProjectVersion)
{
    if(!pProjectVersion)
        return;

    if(ProjectExplorerItem *pItem = m_d->m_mapObjectToItem.value(pProjectVersion, 0))
    {
        if(TakeItem(pItem) == pItem)
            InsertItem(pItem, 0);
    }
}

void ProjectExplorerModel::SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!pProjectObject || m_d->m_mapObjectToItem.contains(pProjectObject))
        return;

    if(m_d->m_eModelType == mtPhysical)
    {
        if(ProjectExplorer::PeProjectObject *pProjectObjectParent = GetProjectObjectParent(pProjectObject))
        {
            if(ProjectExplorerItem *pItemParent = m_d->m_mapObjectToItem.value(pProjectObjectParent, 0))
                InsertItem(new ProjectExplorerItem(pProjectObject, pItemParent), pItemParent);
        }
    }
    else if(m_d->m_eModelType == mtElectrical)
    {
        if(ProjectExplorer::PeProjectObject *pProjectObjectParent = GetProjectObjectBay(pProjectObject))
        {
            if(ProjectExplorerItem *pItemParent = m_d->m_mapObjectToItem.value(pProjectObjectParent, 0))
                InsertItem(new ProjectExplorerItem(pProjectObject, pItemParent), pItemParent);
        }
    }
}

void ProjectExplorerModel::SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!pProjectObject)
        return;

    ProjectExplorerItem *pItem = m_d->m_mapObjectToItem.value(pProjectObject);
    if(!pItem)
        return;

    ProjectExplorerItem *pItemParent = pItem->GetParentItem();
    if(!pItemParent)
        return;

    if(TakeItem(pItem) == pItem)
        InsertItem(pItem, pItemParent);
}

void ProjectExplorerModel::SlotProjectObjectParentChanged(ProjectExplorer::PeProjectObject *pProjectObject, ProjectExplorer::PeProjectObject *pOldParentProjectObject)
{
    Q_UNUSED(pOldParentProjectObject);

    if(m_d->m_eModelType != mtPhysical)
        return;

    if(!pProjectObject)
        return;

    ProjectExplorerItem *pItem = m_d->m_mapObjectToItem.value(pProjectObject);
    if(!pItem)
        return;

    ProjectExplorer::PeProjectObject *pProjectObjectParent = GetProjectObjectParent(pProjectObject);
    if(!pProjectObjectParent)
        return;

    ProjectExplorerItem *pItemParent = m_d->m_mapObjectToItem.value(pProjectObjectParent, 0);
    if(!pItemParent)
        return;

    if(TakeItem(pItem) == pItem)
        InsertItem(pItem, pItemParent);
}

void ProjectExplorerModel::SlotDeviceBayChanged(ProjectExplorer::PeDevice *pDevice, ProjectExplorer::PeBay *pOldBay)
{
    Q_UNUSED(pOldBay);

    if(m_d->m_eModelType != mtElectrical)
        return;

    if(!pDevice)
        return;

    ProjectExplorerItem *pItem = m_d->m_mapObjectToItem.value(pDevice);
    if(!pItem)
        return;

    ProjectExplorer::PeProjectObject *pProjectObjectParent = GetProjectObjectBay(pDevice);
    if(!pProjectObjectParent)
        return;

    ProjectExplorerItem *pItemParent = m_d->m_mapObjectToItem.value(pProjectObjectParent, 0);
    if(!pItemParent)
        return;

    if(TakeItem(pItem) == pItem)
        InsertItem(pItem, pItemParent);
}

void ProjectExplorerModel::SlotProjectObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!pProjectObject)
        return;

    if(ProjectExplorerItem *pItem = m_d->m_mapObjectToItem.value(pProjectObject))
    {
        if(ProjectExplorerItem *pItemParent = m_d->m_mapObjectToItem.value(pProjectObject->GetProjectVersion()))
        {
            while(!pItem->GetChildItems().isEmpty())
            {
                ProjectExplorerItem *pItemChild = pItem->GetChildItems().first();

                if(TakeItem(pItemChild) == pItemChild)
                    InsertItem(pItemChild, pItemParent);
            }

            RemoveItem(pItem);
        }
    }
}

///////////////////////////////////////////////////////////////////////
// ProjectExplorerModel member functions
///////////////////////////////////////////////////////////////////////
class ProjectExplorerFilterModelPrivate
{
public:
    ProjectExplorerFilterModelPrivate()
    {
    }

    ProjectExplorer::PeProjectObject::ObjectType    m_eObjectType;
    QString                                         m_strFilter;
    QMap<ProjectExplorerItem*, bool>                m_mapItemToFilter;
    QList<ProjectExplorerItem*>                     m_lstFilterItems;

};

ProjectExplorerFilterModel::ProjectExplorerFilterModel(QObject *pParent) :
    QSortFilterProxyModel(pParent), m_d(new ProjectExplorerFilterModelPrivate)
{
}

QList<ProjectExplorerItem*> ProjectExplorerFilterModel::GetFilterItems() const
{
    return m_d->m_lstFilterItems;
}

bool ProjectExplorerFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(m_d->m_strFilter.isEmpty())
        return true;

    ProjectExplorerModel *pProjectExplorerModel = qobject_cast<ProjectExplorerModel*>(sourceModel());
    if(!pProjectExplorerModel)
        return true;

    QModelIndex index = pProjectExplorerModel->index(sourceRow, 0, sourceParent);
    ProjectExplorerItem *pProjectExplorerItem = pProjectExplorerModel->ItemFromIndex(index);
    if(!pProjectExplorerItem)
        return true;

    if(m_d->m_mapItemToFilter.contains(pProjectExplorerItem))
        return m_d->m_mapItemToFilter.value(pProjectExplorerItem);

    if(!sourceParent.isValid())
    {
        m_d->m_mapItemToFilter.insert(pProjectExplorerItem, true);
        return true;
    }

    ProjectExplorerItem *pProjectExplorerItemParent = pProjectExplorerItem->GetParentItem();
    while(pProjectExplorerItemParent)
    {
        if(m_d->m_lstFilterItems.contains(pProjectExplorerItemParent))
        {
            m_d->m_mapItemToFilter.insert(pProjectExplorerItem, true);
            return true;
        }

        pProjectExplorerItemParent = pProjectExplorerItemParent->GetParentItem();
    }

    bool bFilter = true;
    if(ProjectExplorer::PeProjectObject *pProjectObject = pProjectExplorerItem->GetProjectObject())
    {
        if(pProjectObject->GetObjectType() == m_d->m_eObjectType)
        {
            bFilter = pProjectObject->GetDisplayName().toLower().contains(QRegExp(m_d->m_strFilter.toLower()));
            if(bFilter && !m_d->m_lstFilterItems.contains(pProjectExplorerItem))
                m_d->m_lstFilterItems.append(pProjectExplorerItem);
        }
        else
        {
            bFilter = false;
            for(int i = 0; i < pProjectExplorerItem->GetChildItems().size(); i++)
                bFilter |= filterAcceptsRow(i, index);
        }
    }

    m_d->m_mapItemToFilter.insert(pProjectExplorerItem, bFilter);
    return bFilter;
}

void ProjectExplorerFilterModel::SlotSetFilter(int iType, const QString &strFilter)
{
    m_d->m_mapItemToFilter.clear();
    m_d->m_lstFilterItems.clear();

    m_d->m_eObjectType = ProjectExplorer::PeProjectObject::ObjectType(iType);
    m_d->m_strFilter = strFilter.trimmed();

    invalidateFilter();
}

} // namespace Core
