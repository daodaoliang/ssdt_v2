#include <QMap>

#include "pblibdevice.h"

namespace ProjectExplorer {

class PbLibDevicePrivate
{
public:
    PbLibDevicePrivate()
        : m_eDeviceType(PbLibDevice::dtInvalid), m_strManufacture(""), m_strDeviceCategory(""), m_strBayCategory(""), m_strType(""), m_strIcdFileName("")
    {
    }

    PbLibDevice::DeviceType m_eDeviceType;
    QString                 m_strManufacture;
    QString                 m_strDeviceCategory;
    QString                 m_strBayCategory;
    QString                 m_strType;
    QString                 m_strIcdFileName;

    QList<PbLibBoard*>      m_lstBoards;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbLibDevice::PbLibDevice() : PbBaseObject(PbBaseObject::botLibDevice), m_d(new PbLibDevicePrivate)
{
}

PbLibDevice::PbLibDevice(const PbLibDevice &Device) : PbBaseObject(Device), m_d(new PbLibDevicePrivate)
{
    *m_d = *Device.m_d;
}

PbLibDevice::PbLibDevice(int iId) : PbBaseObject(PbBaseObject::botLibDevice, iId), m_d(new PbLibDevicePrivate)
{
}

PbLibDevice::~PbLibDevice()
{
    delete m_d;
}

QIcon PbLibDevice::GetDeviceIcon(const DeviceType &eDeviceType)
{
    static QMap<DeviceType, QIcon> s_mapTypeToIcon;
    if(s_mapTypeToIcon.isEmpty())
    {
        s_mapTypeToIcon.insert(dtIED, QIcon(":/projectexplorer/images/pe_ied.png"));
        s_mapTypeToIcon.insert(dtSwitch, QIcon(":/projectexplorer/images/pe_switch.png"));
    }

    return s_mapTypeToIcon.value(eDeviceType, QIcon());
}

QString PbLibDevice::GetDeviceTypeName(const DeviceType &eDeviceType)
{
    static QMap<DeviceType, QString> s_mapTypeToName;
    if(s_mapTypeToName.isEmpty())
    {
        s_mapTypeToName.insert(dtIED, tr("IED"));
        s_mapTypeToName.insert(dtSwitch, tr("Switch"));
    }

    return s_mapTypeToName.value(eDeviceType, tr("Unknown"));
}

QIcon PbLibDevice::GetDisplayIcon() const
{
    return GetDeviceIcon(m_d->m_eDeviceType);
}

QString PbLibDevice::GetDisplayName() const
{
    return GetType();
}

QString PbLibDevice::GetObjectTypeName() const
{
    return GetDeviceTypeName(m_d->m_eDeviceType);
}

PbLibDevice& PbLibDevice::operator=(const PbLibDevice &Device)
{
    PbBaseObject::operator =(Device);
    *m_d = *Device.m_d;

    return *this;
}

PbLibDevice::DeviceType PbLibDevice::GetDeviceType() const
{
    return m_d->m_eDeviceType;
}

void PbLibDevice::SetDeviceType(const DeviceType &eDeviceType)
{
    m_d->m_eDeviceType = eDeviceType;
}

QString PbLibDevice::GetManufacture() const
{
    return m_d->m_strManufacture;
}

void PbLibDevice::SetManufacture(const QString &strManufacture)
{
    m_d->m_strManufacture = strManufacture;
}

QString PbLibDevice::GetDeviceCategory() const
{
    return m_d->m_strDeviceCategory;
}

void PbLibDevice::SetDeviceCategory(const QString &strDeviceCategory)
{
    m_d->m_strDeviceCategory = strDeviceCategory;
}

QString PbLibDevice::GetBayCategory() const
{
    return m_d->m_strBayCategory;
}

void PbLibDevice::SetBayCategory(const QString &strBayCategory)
{
    m_d->m_strBayCategory = strBayCategory;
}

QString PbLibDevice::GetType() const
{
    return m_d->m_strType;
}

void PbLibDevice::SetType(const QString &strType)
{
    m_d->m_strType = strType;
}

QString PbLibDevice::GetIcdFileName() const
{
    return m_d->m_strIcdFileName;
}

void PbLibDevice::SetIcdFileName(const QString &strIcdFileName)
{
    m_d->m_strIcdFileName = strIcdFileName;
}

void PbLibDevice::AddChildBoard(PbLibBoard *pBoard)
{
    if(!m_d->m_lstBoards.contains(pBoard))
        m_d->m_lstBoards.append(pBoard);
}

void PbLibDevice::RemoveChildBoard(PbLibBoard *pBoard)
{
    m_d->m_lstBoards.removeOne(pBoard);
}

QList<PbLibBoard*> PbLibDevice::GetChildBoards() const
{
    return m_d->m_lstBoards;
}
