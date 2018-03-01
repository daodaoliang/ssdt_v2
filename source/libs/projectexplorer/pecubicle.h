#ifndef PECUBICLE_H
#define PECUBICLE_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeCubiclePrivate;
class PeRoom;
class PeDevice;
class PROJECTEXPLORER_EXPORT PeCubicle : public PeProjectObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PeCubicle();
    PeCubicle(const PeCubicle &Cubicle);
    PeCubicle(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeCubicle();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PeCubicle&          operator=(const PeCubicle &Cubicle);

    // Property operations
    QString             GetName() const;
    void                SetName(const QString &strName);
    QString             GetNumber() const;
    void                SetNumber(const QString &strNumber);
    QString             GetManufacture() const;
    void                SetManufacture(const QString &strManufacture);

    // Project object operation
    PeRoom*             GetParentRoom() const;
    void                SetParentRoom(PeRoom *pRoom);

    void                AddChildDevice(PeDevice *pDevice);
    void                RemoveChildDevice(PeDevice *pDevice);
    QList<PeDevice*>    GetChildDevices() const;

    int                 GetChildDevicePos(PeDevice *pDevice) const;
    void                SetChildDevicePos(PeDevice *pDevice, int iPos);

    QString             GetChildDeviceNumber(PeDevice *pDevice) const;
    void                SetChildDeviceNumber(PeDevice *pDevice, const QString &strNumber);

    bool                IsYardCubile() const;

// Properties
private:
    PeCubiclePrivate    *m_d;
};

} // namespace ProjectExplorer

#endif // PECUBICLE_H
