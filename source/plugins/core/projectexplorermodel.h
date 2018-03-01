#ifndef PROJECTEXPLORERMODEL_H
#define PROJECTEXPLORERMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include "core_global.h"

namespace ProjectExplorer {

class PeProjectObject;
class PeProjectVersion;
class PeBay;
class PeDevice;

} // namespace ProjectExplorer

namespace Core {

class ProjectExplorerItemPrivate;
class CORE_EXPORT ProjectExplorerItem
{
// Construction and Destruction
protected:
    ProjectExplorerItem(ProjectExplorer::PeProjectObject *pProjectObject, ProjectExplorerItem *pItemParent);

public:
    ~ProjectExplorerItem();

// Operations
public:
    ProjectExplorer::PeProjectObject*   GetProjectObject() const;
    ProjectExplorerItem*                GetParentItem() const;
    QList<ProjectExplorerItem*>         GetChildItems() const;

private:
    QString                             GetItemName() const;
    QIcon                               GetItemIcon() const;
    void                                SetBold(bool bBold);
    bool                                GetBold() const;
    void                                SetParentItem(ProjectExplorerItem *pItemParent);
    void                                InsertChildItem(ProjectExplorerItem *pChildItem);
    int                                 InsertChildItem(ProjectExplorerItem *pChildItem, int iPos);
    ProjectExplorerItem*                TakeChildItem(ProjectExplorerItem *pChildItem);
    void                                RemoveChildItem(ProjectExplorerItem *pChildItem);

// Properties
private:
    ProjectExplorerItemPrivate *m_d;
    friend class ProjectExplorerModel;
};

class ProjectExplorerModelPrivate;
class CORE_EXPORT ProjectExplorerModel : public QAbstractItemModel
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum ModelType { mtPhysical = 0, mtElectrical, mtNone };

// Construction and Destruction
public:
    ProjectExplorerModel(const ModelType &eModelType = mtPhysical, QObject *pParent = 0);
    ~ProjectExplorerModel();

// Operations
public:
    virtual QModelIndex index(int nRow, int nColumn, const QModelIndex &Parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    void                    Refresh();
    void                    SetModelType(const ModelType &eModelType);
    ModelType               GetModelType() const;

    ProjectExplorerItem*    ItemFromIndex(const QModelIndex &index) const;
    ProjectExplorerItem*    ItemFromObject(ProjectExplorer::PeProjectObject *pProjectObject) const;
    QModelIndex             IndexForItem(ProjectExplorerItem *pItem) const;
    QModelIndex             IndexForObject(ProjectExplorer::PeProjectObject *pProjectObject) const;

private:
    void                    InsertItem(ProjectExplorerItem *pItem, ProjectExplorerItem *pItemParent);
    void                    RemoveItem(ProjectExplorerItem *pItem);
    ProjectExplorerItem*    TakeItem(ProjectExplorerItem *pItem);

// Properties
protected:
    ProjectExplorerModelPrivate *m_d;

private slots:
    void SlotProjectVersionOpened(ProjectExplorer::PeProjectVersion *pProjectVersion);
    void SlotProjectVersionAboutToBeClosed(ProjectExplorer::PeProjectVersion *pProjectVersion);
    void SlotProjectVersionPropertyChanged(ProjectExplorer::PeProjectVersion *pProjectVersion);
    void SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject);
    void SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject);
    void SlotProjectObjectParentChanged(ProjectExplorer::PeProjectObject *pProjectObject, ProjectExplorer::PeProjectObject *pOldParentProjectObject);
    void SlotDeviceBayChanged(ProjectExplorer::PeDevice *pDevice, ProjectExplorer::PeBay *pOldBay);
    void SlotProjectObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject *pProjectObject);
};

class ProjectExplorerFilterModelPrivate;
class CORE_EXPORT ProjectExplorerFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

// Construction
public:
    ProjectExplorerFilterModel(QObject *pParent = 0);

// Operations
public:
    QList<ProjectExplorerItem*> GetFilterItems() const;

protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

// Properties
private:
    ProjectExplorerFilterModelPrivate *m_d;

public slots:
    void SlotSetFilter(int iType, const QString &strFilter);
};

} // namespace Core

#endif // PROJECTEXPLORERMODEL_H
