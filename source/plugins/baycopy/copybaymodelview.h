#ifndef COPYBAYMODELVIEW_H
#define COPYBAYMODELVIEW_H

#include <QAbstractItemModel>
#include "utils/readonlyview.h"

namespace ProjectExplorer {

class PeProjectVersion;
class PeBay;
class PeCubicle;
class PeDevice;

} // namespace ProjectExplorer

namespace BayCopy {
namespace Internal {

class CopyBayItemDevice;
class CopyBayItem : public QObject
{
    Q_OBJECT

// Construction and Destruction
public:
    CopyBayItem();
    virtual ~CopyBayItem();

// Operations
public:
    virtual QString GetText(int iColumn) const = 0;
    virtual QIcon   GetIcon(int iColumn) const = 0;
    virtual bool    GetValid(int iColumn) const = 0;

    bool            GetChecked() const;
    void            SetChecked(bool bChecked);

// Properties
protected:
    bool            m_bChecked;
};

class CopyBayItemBay : public CopyBayItem
{
    Q_OBJECT

// Construction and Destruction
public:
    CopyBayItemBay(ProjectExplorer::PeBay *pBaySrc);
    ~CopyBayItemBay();

// Operations
public:
    virtual QString             GetText(int iColumn) const;
    virtual QIcon               GetIcon(int iColumn) const;
    virtual bool                GetValid(int iColumn) const;

    ProjectExplorer::PeBay*     GetBaySrc() const;
    ProjectExplorer::PeBay*     GetBayDst() const;

    void                        AppendChild(CopyBayItemDevice *pChildItem);
    QList<CopyBayItemDevice*>   GetChildItems() const;

// Properties
private:
    ProjectExplorer::PeBay      *m_pBaySrc;
    ProjectExplorer::PeBay      *m_pBayDst;
    QList<CopyBayItemDevice*>   m_lstChildItems;
};

class CopyBayItemDevice : public CopyBayItem
{
    Q_OBJECT

// Construction and Destruction
public:
    CopyBayItemDevice(ProjectExplorer::PeDevice *pDeviceSrc, CopyBayItemBay *pParentItem);
    ~CopyBayItemDevice();

// Operations
public:
    virtual QString             GetText(int iColumn) const;
    virtual QIcon               GetIcon(int iColumn) const;
    virtual bool                GetValid(int iColumn) const;

    ProjectExplorer::PeDevice*  GetDeviceSrc() const;
    ProjectExplorer::PeDevice*  GetDeviceDst() const;
    CopyBayItemBay*             GetParentItem() const;

// Properties
private:
    CopyBayItemBay              *m_pParentItem;
    ProjectExplorer::PeDevice   *m_pDeviceSrc;
    ProjectExplorer::PeDevice   *m_pDeviceDst;
};

class CopyBayModel : public QAbstractItemModel
{
    Q_OBJECT

// Construction and Destruction
public:
    CopyBayModel(QObject *pParent = 0);
    ~CopyBayModel();

// Operations
public:
    virtual QModelIndex                 index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex                 parent (const QModelIndex &index) const ;
    virtual int                         rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int                         columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant                    data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags               flags(const QModelIndex &index) const;
    virtual QVariant                    headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool                        setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    ProjectExplorer::PeProjectVersion*  GetSrcProject() const;
    void                                SetSrcProject(ProjectExplorer::PeProjectVersion *pProjectVersionSrc);
    ProjectExplorer::PeProjectVersion*  GetDstProject() const;
    void                                SetDstProject(ProjectExplorer::PeProjectVersion *pProjectVersionDst);

    CopyBayItem*                        ItemFromIndex(const QModelIndex &index) const;
    void                                UpdateItem(CopyBayItem *pItem);

    QList<CopyBayItem*>                 GetCheckedItems() const;
    bool                                GetValid() const;

// Properties
protected:
    ProjectExplorer::PeProjectVersion   *m_pProjectVersionSrc;
    ProjectExplorer::PeProjectVersion   *m_pProjectVersionDst;

    QList<CopyBayItemBay*>              m_lstBayItems;
    QMap<ProjectExplorer::PeCubicle*, ProjectExplorer::PeCubicle*>    m_mapCubicleSrcToDst;

signals:
    void    sigItemCheckStateChanged();
};

class CopyBayView : public Utils::ReadOnlyTreeView
{
    Q_OBJECT

// Construction and Destruction
public:
    CopyBayView(QWidget *pParent = 0);
    ~CopyBayView();

// Operations
public:
    void SetModel(CopyBayModel *pModel);

// Properties
private:
    CopyBayModel *m_pModel;
};

} // namespace Internal
} // namespace BayCopy

#endif // COPYBAYMODELVIEW_H
