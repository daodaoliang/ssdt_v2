#include <QMap>

#include "pbdevicetype.h"

namespace ProjectExplorer {

class PbDeviceTypePrivate
{
public:
    PbDeviceTypePrivate() : m_strName("") {}

    QString m_strName;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbDeviceType::PbDeviceType() :
    PbBaseObject(PbBaseObject::botDeviceType), m_d(new PbDeviceTypePrivate)
{
}

PbDeviceType::PbDeviceType(const PbDeviceType &DeviceType) :
    PbBaseObject(DeviceType), m_d(new PbDeviceTypePrivate)
{
    *m_d = *DeviceType.m_d;
}

PbDeviceType::PbDeviceType(int iId) :
    PbBaseObject(PbBaseObject::botDeviceType, iId), m_d(new PbDeviceTypePrivate)
{
}

PbDeviceType::~PbDeviceType()
{
    delete m_d;
}

QString PbDeviceType::GetDisplayName() const
{
    return m_d->m_strName;
}

PbDeviceType& PbDeviceType::operator=(const PbDeviceType &DeviceType)
{
    *m_d = *DeviceType.m_d;

    return *this;
}

QString PbDeviceType::GetName() const
{
    return m_d->m_strName;
}

void PbDeviceType::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}
