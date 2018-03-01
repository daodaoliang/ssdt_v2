#ifndef PEPROJECTVERSION_H
#define PEPROJECTVERSION_H

#include "peprojectobject.h"

QT_BEGIN_NAMESPACE
class QDateTime;
class QSettings;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeProjectVersionPrivate;
class PeProject;
class PeRoom;
class PeCubicle;
class PeBay;
class PeDevice;
class PeBoard;
class PePort;
class PeInfoSet;
class PeCubicleConn;
class PeVTerminal;
class PeStrap;
class PeVTerminalConn;
class PeCable;
class PeFiber;
class PeVolume;
class PeDwg;
class PROJECTEXPLORER_EXPORT PeProjectVersion : public PeProjectObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PeProjectVersion();
    PeProjectVersion(const PeProjectVersion &ProjectVersion);
    PeProjectVersion(PeProject *pProject, int iId);
    virtual ~PeProjectVersion();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString         GetDisplayName() const;

    // Operator overload
    PeProjectVersion&       operator=(const PeProjectVersion &ProjectVersion);

    // Open/Close operations
    bool                    Open();
    void                    Close();
    bool                    Reopen();
    bool                    IsOpend() const;

    // Property operations
    PeProject*              GetProject() const;
    void                    SetProject(PeProject *pProject);
    int                     GetVersionNumber() const;
    void                    SetVersionNumber(int iVersionNumber);
    QString                 GetStage() const;
    void                    SetStage(const QString &strStage);
    QString                 GetModifier() const;
    void                    SetModifier(const QString &strModifier);
    QDateTime               GetModificationTime() const;
    void                    SetModificationTime(const QDateTime &dtModification);
    QString                 GetWhat() const;
    void                    SetWhat(const QString &strWhat);
    QString                 GetWhy() const;
    void                    SetWhy(const QString &strWhy);
    QString                 GetVersionPwd() const;
    void                    SetVersionPwd(const QString &strVersionPwd);

    bool                    ValidateModifyTime();
    bool                    UpdateModifyTime();
    bool                    ValidateVersionPwd(QWidget *pParent);

    // Project object operation
    PeProjectObject*        CreateObject(const PeProjectObject &ProjectObject, bool bNotify = true);
    PeProjectObject*        UpdateObject(const PeProjectObject &ProjectObject, bool bNotify = true);
    bool                    DeleteObject(PeProjectObject *pProjectObject, bool bNotify = true);
    PeProjectObject*        CloneObject(const PeProjectObject &ProjectObject, bool bNotify = true);
    bool                    ClearDeviceModel(PeDevice *pDevice, bool bNotify = true);
    void                    UpdateDeviceModelStatus(PeDevice *pDevice, bool bHasModel, bool bNotify = true);

    QList<PeBay*>           GetAllBays() const;
    QList<PeRoom*>          GetAllRooms() const;
    QList<PeCubicle*>       GetAllCubicles() const;
    QList<PeDevice*>        GetAllDevices() const;
    QList<PeBoard*>         GetAllBoards() const;
    QList<PePort*>          GetAllPorts() const;
    QList<PeInfoSet*>       GetAllInfoSets() const;
    QList<PeCubicleConn*>   GetAllCubicleConns() const;
    QList<PeVTerminal*>     GetAllVTerminals() const;
    QList<PeStrap*>         GetAllStraps() const;
    QList<PeVTerminalConn*> GetAllVTerminalConns() const;
    QList<PeCable*>         GetAllCables() const;
    QList<PeFiber*>         GetAllFibers() const;
    QList<PeVolume*>        GetAllVolumes() const;
    QList<PeDwg*>           GetAllDwgs() const;

    PeBay*                  FindBayById(int iId);
    PeRoom*                 FindRoomById(int iId);
    PeCubicle*              FindCubicleById(int iId);
    PeDevice*               FindDeviceById(int iId);
    PeBoard*                FindBoardById(int iId);
    PePort*                 FindPortById(int iId);
    PeInfoSet*              FindInfoSetById(int iId);
    PeCubicleConn*          FindCubicleConnById(int iId);
    PeVTerminal*            FindVTerminalById(int iId);
    PeStrap*                FindStrapById(int iId);
    PeVTerminalConn*        FindVTerminalConnById(int iId);
    PeCable*                FindCableById(int iId);
    PeFiber*                FindFiberById(int iId);
    PeVolume*               FindVolumeById(int iId);
    PeDwg*                  FindDwgById(int iId);

    // DB Operation
    bool                    DbCreateObject(PeProjectObject &ProjectObject, bool bUseTransaction);
    bool                    DbUpdateObject(const PeProjectObject &ProjectObject, bool bUseTransaction);
    bool                    DbDeleteObject(const PeProjectObject::ObjectType &eObjectType, int iId, bool bUseTransaction);
    bool                    DbReadDeviceModel(int iDeviceId, QByteArray &baModelContent);
    bool                    DbUpdateDeviceModel(int iDeviceId, const QByteArray &baModelContent, bool bUseTransaction);
    bool                    DbClearDeviceModel(int iDeviceId, bool bUseTransaction);
    bool                    DbUpdateSclCommunication(const QByteArray &baCommunicationContent, bool bUseTransaction);
    bool                    DbReadSclCommunication(QByteArray &baCommunicationContent);
    bool                    DbUpdateSclTemplate(const QByteArray &baTemplateContent, bool bUseTransaction);
    bool                    DbReadSclTemplate(QByteArray &baTemplateContent);
    bool                    DbClearAllModel(bool bUseTransaction);
    bool                    DbReadSettings(QMap<QString, QString> &mapSettings);
    bool                    DbUpdateSettings(const QMap<QString, QString> &mapSettings, bool bUseTransaction);
    bool                    DbReadDwgFile(int iDwgId, QByteArray &baFileContent);
    bool                    DbUpdateDwgFile(int iDwgId, const QByteArray &baFileContent, bool bUseTransaction);
    int                     DbGenerateNewInfoSetGroup();
    bool                    DbReadDeviceInfomation(QList<PeDevice*> &lstDevice);
    bool                    DbReadGeneralInformation(int &iBayNumber, int &iRoomNumber,
                                                     int &iCubicleNumber, int &iIedNumber,
                                                     int &iSwitchNumber, int &iCableNumber);

// Properties
private:
    PeProjectVersionPrivate *m_d;

signals:
    void                    sigObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject);
    void                    sigObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject);
    void                    sigObjectParentChanged(ProjectExplorer::PeProjectObject *pProjectObject, ProjectExplorer::PeProjectObject *pOldParentProjectObject);
    void                    sigDeviceBayChanged(ProjectExplorer::PeDevice *pDevice, ProjectExplorer::PeBay *pOldBay);
    void                    sigDeviceModelChanged(ProjectExplorer::PeDevice *pDevice);
    void                    sigObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject *pProjectObject);
    void                    sigObjectDeleted();
};

class DbTransactionPrivate;
class PROJECTEXPLORER_EXPORT DbTransaction : public QObject
{
    Q_OBJECT

// Construction and Destruction
public:
    DbTransaction(PeProjectVersion *pProjectVersion, bool bEnable);
    ~DbTransaction();

// Operations
public:
    void    Rollback();
    bool    Commit();

// Properties
private:
    DbTransactionPrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PEPROJECTVERSION_H
