#ifndef PBDEVICETYPE_H
#define PBDEVICETYPE_H

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbDeviceTypePrivate;
class PROJECTEXPLORER_EXPORT PbDeviceType : public PbBaseObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PbDeviceType();
    PbDeviceType(const PbDeviceType &DeviceType);
    PbDeviceType(int iId);
    virtual ~PbDeviceType();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PbDeviceType&      operator=(const PbDeviceType &DeviceType);

    // Property operations
    QString             GetName() const;
    void                SetName(const QString &strName);

// Properties
private:
    PbDeviceTypePrivate  *m_d;
};

} // namespace ProjectExplorer

#endif // PBDEVICETYPE_H
