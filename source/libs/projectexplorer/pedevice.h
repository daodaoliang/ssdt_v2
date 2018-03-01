#ifndef PEDEVICE_H
#define PEDEVICE_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeDevicePrivate;
class PeCubicle;
class PeBay;
class PeBoard;
class PeVTerminal;
class PeStrap;
class PROJECTEXPLORER_EXPORT PeDevice : public PeProjectObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum DeviceType { dtIED = 0, dtSwitch, dtODF, dtInvalid};

// Construction and Destruction
public:
    PeDevice();
    PeDevice(const PeDevice &Device);
    PeDevice(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeDevice();

// Operations
public:
    static QIcon        GetDeviceIcon(const DeviceType &eDeviceType);
    static QString      GetDeviceTypeName(const DeviceType &eDeviceType);

    // Derived from PeProjectObject
    virtual QIcon       GetDisplayIcon() const;
    virtual QString     GetDisplayName() const;
    virtual QString     GetObjectTypeName() const;

    // Operator overload
    PeDevice&           operator=(const PeDevice &Device);

    // Property operations
    DeviceType          GetDeviceType() const;
    void                SetDeviceType(const DeviceType &eDeviceType);
    QString             GetName() const;
    void                SetName(const QString &strName);
    QString             GetDeviceCategory() const;
    void                SetDeviceCategory(const QString &strDeviceCategory);
    QString             GetBayCategory() const;
    void                SetBayCategory(const QString &strBayCategory);
    int                 GetVLevel() const;
    void                SetVLevel(int iVLevel);
    QString             GetCircuitIndex() const;
    void                SetCircuitIndex(const QString &strCircuitIndex);
    QString             GetSet() const;
    void                SetSet(const QString &strName);
    QString             GetDescription() const;
    void                SetDescription(const QString &strDescription);
    QString             GetManufacture() const;
    void                SetManufacture(const QString &strManufacture);
    QString             GetType() const;
    void                SetType(const QString &strType);
    QString             GetConfigVersion() const;
    void                SetConfigVersion(const QString &strConfigVersion);
    QString             GetIcdFileName() const;
    void                SetIcdFileName(const QString &strIcdFileName);
    QString             GetCubiclePos() const;
    void                SetCubiclePos(const QString &strCubiclePos);
    QString             GetCubicleNum() const;
    void                SetCubicleNum(const QString &strCubicleNum);
    bool                GetHasModel() const;
    void                SetHasModel(bool bHasModel);

    // Project object operation
    PeCubicle*          GetParentCubicle() const;
    void                SetParentCubicle(PeCubicle *pCubicle);

    PeBay*              GetParentBay() const;
    void                SetParentBay(PeBay *pBay);

    void                AddChildBoard(PeBoard *pBoard);
    void                RemoveChildBoard(PeBoard *pBoard);
    QList<PeBoard*>     GetChildBoards() const;

    void                AddVTerminal(PeVTerminal *pVTerminal);
    void                RemoveVTerminal(PeVTerminal *pVTerminal);
    QList<PeVTerminal*> GetVTerminals() const;

    void                AddStrap(PeStrap *pStrap);
    void                RemoveStrap(PeStrap *pStrap);
    QList<PeStrap*>     GetStraps() const;

// Properties
private:
    PeDevicePrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PEDEVICE_H
