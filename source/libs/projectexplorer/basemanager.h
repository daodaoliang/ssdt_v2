#ifndef BASEMANAGER_H
#define BASEMANAGER_H

#include "pbbaseobject.h"

namespace MySqlWrapper {

class MySqlDatabase;

} // namespace ProjectExplorer

namespace ProjectExplorer {

class BaseManagerPrivate;
class PbBaseObject;
class PbDeviceCategory;
class PbBayCategory;
class PbVLevel;
class PbManufacture;
class PbDeviceType;
class PbLibDevice;
class PbLibBoard;
class PbLibPort;
class PbTpBay;
class PbTpDevice;
class PbTpPort;
class PbTpInfoSet;
class PROJECTEXPLORER_EXPORT BaseManager : public QObject
{
    Q_OBJECT

// Construction and Destruction
public:
    BaseManager(QObject *pParent = 0);
    virtual ~BaseManager();

// Operations
public:
    static  BaseManager*        Instance();

    bool                        Initialize();
    void                        Uninitialize();
    bool                        Reinitialize();

    // Base object operation
    PbBaseObject*               CreateObject(const PbBaseObject &BaseObject, bool bNotify = true);
    PbBaseObject*               UpdateObject(const PbBaseObject &BaseObject, bool bNotify = true);
    bool                        DeleteObject(PbBaseObject *pBaseObject, bool bNotify = true);

    QList<PbDeviceCategory*>    GetAllDeviceCategories() const;
    QList<PbBayCategory*>       GetAllBayCategories() const;
    QList<PbVLevel*>            GetAllVLevels() const;
    QList<PbManufacture*>       GetAllManufactures() const;
    QList<PbDeviceType*>        GetAllDeviceTypes() const;
    QList<PbLibDevice*>         GetAllLibDevices() const;
    QList<PbLibBoard*>          GetAllLibBoards() const;
    QList<PbLibPort*>           GetAllLibPorts() const;
    QList<PbTpBay*>             GetAllTpBays() const;
    QList<PbTpDevice*>          GetAllTpDevices() const;
    QList<PbTpPort*>            GetAllTpPorts() const;
    QList<PbTpInfoSet*>         GetAllTpInfoSets() const;

    PbDeviceCategory*           FindDeviceCategoryById(int iId) const;
    PbBayCategory*              FindBayCategoryById(int iId) const;
    PbVLevel*                   FindVLevelById(int iId) const;
    PbManufacture*              FindManufactureById(int iId) const;
    PbDeviceType*               FindDeviceTypeById(int iId) const;
    PbLibDevice*                FindLibDeviceById(int iId) const;
    PbLibBoard*                 FindLibBoardById(int iId) const;
    PbLibPort*                  FindLibPortById(int iId) const;
    PbTpBay*                    FindTpBayById(int iId) const;
    PbTpDevice*                 FindTpDeviceById(int iId) const;
    PbTpPort*                   FindTpPortById(int iId) const;
    PbTpInfoSet*                FindTpInfoSetById(int iId) const;

    // DB Operation
    bool                        DbCreateObject(PbBaseObject &BaseObject);
    bool                        DbUpdateObject(const PbBaseObject &BaseObject);
    bool                        DbDeleteObject(const PbBaseObject::ObjectType &eObjectType, int iId, bool bUseTransaction = true);
    bool                        DbReadLibDeviceModel(int iDeviceId, QByteArray &baModelContent);
    bool                        DbUpdateLibDeviceModel(int iDeviceId, const QByteArray &baModelContent);
    int                         DbGenerateNewTpInfoSetGroup();

// Properties
private:
    BaseManagerPrivate *m_d;

signals:
    void    sigInitialized();
    void    sigUninitialized();
    void    sigObjectCreated(ProjectExplorer::PbBaseObject *pBaseObject);
    void    sigObjectPropertyChanged(ProjectExplorer::PbBaseObject *pBaseObject);
    void    sigObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject *pBaseObject);
    void    sigObjectDeleted();
};

} // namespace ProjectExplorer

#endif // BASEMANAGER_H
