#ifndef PBDEVICECATEGORY_H
#define PBDEVICECATEGORY_H

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbDeviceCategoryPrivate;
class PROJECTEXPLORER_EXPORT PbDeviceCategory : public PbBaseObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PbDeviceCategory();
    PbDeviceCategory(const PbDeviceCategory &DeviceCategory);
    PbDeviceCategory(int iId);
    virtual ~PbDeviceCategory();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PbDeviceCategory&   operator=(const PbDeviceCategory &DeviceCategory);

    // Property operations
    QString             GetName() const;
    void                SetName(const QString &strName);
    QString             GetCode() const;
    void                SetCode(const QString &strCode);

// Properties
private:
    PbDeviceCategoryPrivate  *m_d;
};

} // namespace ProjectExplorer

#endif // PBDEVICECATEGORY_H
