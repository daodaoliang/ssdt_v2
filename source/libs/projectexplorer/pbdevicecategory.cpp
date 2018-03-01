#include <QMap>

#include "pbdevicecategory.h"

namespace ProjectExplorer {

class PbDeviceCategoryPrivate
{
public:
    PbDeviceCategoryPrivate() : m_strName(""), m_strCode("") {}

    QString m_strName;
    QString m_strCode;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbDeviceCategory::PbDeviceCategory() :
    PbBaseObject(PbBaseObject::botDeviceCategory), m_d(new PbDeviceCategoryPrivate)
{
}

PbDeviceCategory::PbDeviceCategory(const PbDeviceCategory &DeviceCategory) :
    PbBaseObject(DeviceCategory), m_d(new PbDeviceCategoryPrivate)
{
    *m_d = *DeviceCategory.m_d;
}

PbDeviceCategory::PbDeviceCategory(int iId) :
    PbBaseObject(PbBaseObject::botDeviceCategory, iId), m_d(new PbDeviceCategoryPrivate)
{
}

PbDeviceCategory::~PbDeviceCategory()
{
    delete m_d;
}

QString PbDeviceCategory::GetDisplayName() const
{
    return m_d->m_strName;
}

PbDeviceCategory& PbDeviceCategory::operator=(const PbDeviceCategory &DeviceCategory)
{
    *m_d = *DeviceCategory.m_d;

    return *this;
}

QString PbDeviceCategory::GetName() const
{
    return m_d->m_strName;
}

void PbDeviceCategory::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

QString PbDeviceCategory::GetCode() const
{
    return m_d->m_strCode;
}

void PbDeviceCategory::SetCode(const QString &strCode)
{
    m_d->m_strCode = strCode;
}
