#ifndef PBTPDEVICE_H
#define PBTPDEVICE_H

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbTpDevicePrivate;
class PbTpBay;
class PbTpPort;
class PROJECTEXPLORER_EXPORT PbTpDevice : public PbBaseObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum DeviceType { dtIED = 0, dtSwitch, dtInvalid};

// Construction and Destruction
public:
    PbTpDevice();
    PbTpDevice(const PbTpDevice &Device);
    PbTpDevice(int iId);
    virtual ~PbTpDevice();

// Operations
public:
    static QIcon        GetDeviceIcon(const DeviceType &eDeviceType);
    static QString      GetDeviceTypeName(const DeviceType &eDeviceType);

    // Derived from PeProjectObject
    virtual QIcon       GetDisplayIcon() const;
    virtual QString     GetDisplayName() const;
    virtual QString     GetObjectTypeName() const;

    // Operator overload
    PbTpDevice&         operator=(const PbTpDevice &Device);

    // Property operations
    QString             GetName() const;
    void                SetName(const QString &strName);
    DeviceType          GetDeviceType() const;
    void                SetDeviceType(const DeviceType &eDeviceType);

    // Project object operation
    PbTpBay*            GetParentBay() const;
    void                SetParentBay(PbTpBay *pTpBay);

    void                AddChildPort(PbTpPort *pPort);
    void                RemoveChildPort(PbTpPort *pPort);
    QList<PbTpPort*>    GetChildPorts() const;

// Properties
private:
    PbTpDevicePrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PBTPDEVICE_H
