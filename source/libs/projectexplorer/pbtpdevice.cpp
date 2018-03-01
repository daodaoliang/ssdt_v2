#include <QMap>

#include "pbtpdevice.h"

namespace ProjectExplorer {

class PbTpDevicePrivate
{
public:
    PbTpDevicePrivate()
        : m_strName(""), m_eDeviceType(PbTpDevice::dtInvalid), m_pTpBay(0)
    {
    }

    QString                 m_strName;
    PbTpDevice::DeviceType  m_eDeviceType;

    PbTpBay                 *m_pTpBay;
    QList<PbTpPort*>        m_lstPorts;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbTpDevice::PbTpDevice() : PbBaseObject(PbBaseObject::botTpDevice), m_d(new PbTpDevicePrivate)
{
}

PbTpDevice::PbTpDevice(const PbTpDevice &Device) : PbBaseObject(Device), m_d(new PbTpDevicePrivate)
{
    *m_d = *Device.m_d;
}

PbTpDevice::PbTpDevice(int iId) : PbBaseObject(PbBaseObject::botTpDevice, iId), m_d(new PbTpDevicePrivate)
{
}

PbTpDevice::~PbTpDevice()
{
    delete m_d;
}

QIcon PbTpDevice::GetDeviceIcon(const DeviceType &eDeviceType)
{
    static QMap<DeviceType, QIcon> s_mapTypeToIcon;
    if(s_mapTypeToIcon.isEmpty())
    {
        s_mapTypeToIcon.insert(dtIED, QIcon(":/projectexplorer/images/pe_ied.png"));
        s_mapTypeToIcon.insert(dtSwitch, QIcon(":/projectexplorer/images/pe_switch.png"));
    }

    return s_mapTypeToIcon.value(eDeviceType, QIcon());
}

QString PbTpDevice::GetDeviceTypeName(const DeviceType &eDeviceType)
{
    static QMap<DeviceType, QString> s_mapTypeToName;
    if(s_mapTypeToName.isEmpty())
    {
        s_mapTypeToName.insert(dtIED, tr("IED"));
        s_mapTypeToName.insert(dtSwitch, tr("Switch"));
    }

    return s_mapTypeToName.value(eDeviceType, tr("Unknown"));
}

QIcon PbTpDevice::GetDisplayIcon() const
{
    return GetDeviceIcon(m_d->m_eDeviceType);
}

QString PbTpDevice::GetDisplayName() const
{
    return GetName();
}

QString PbTpDevice::GetObjectTypeName() const
{
    return GetDeviceTypeName(m_d->m_eDeviceType);
}

PbTpDevice& PbTpDevice::operator=(const PbTpDevice &Device)
{
    PbBaseObject::operator =(Device);
    *m_d = *Device.m_d;

    return *this;
}

QString PbTpDevice::GetName() const
{
    return m_d->m_strName;
}

void PbTpDevice::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

PbTpDevice::DeviceType PbTpDevice::GetDeviceType() const
{
    return m_d->m_eDeviceType;
}

void PbTpDevice::SetDeviceType(const DeviceType &eDeviceType)
{
    m_d->m_eDeviceType = eDeviceType;
}

PbTpBay* PbTpDevice::GetParentBay() const
{
    return m_d->m_pTpBay;
}

void PbTpDevice::SetParentBay(PbTpBay *pTpBay)
{
    m_d->m_pTpBay = pTpBay;
}

void PbTpDevice::AddChildPort(PbTpPort *pPort)
{
    if(!m_d->m_lstPorts.contains(pPort))
        m_d->m_lstPorts.append(pPort);
}

void PbTpDevice::RemoveChildPort(PbTpPort *pPort)
{
    m_d->m_lstPorts.removeOne(pPort);
}

QList<PbTpPort*> PbTpDevice::GetChildPorts() const
{
    return m_d->m_lstPorts;
}
