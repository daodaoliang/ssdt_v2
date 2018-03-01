#include <QMap>

#include "pecubicle.h"
#include "peroom.h"

namespace ProjectExplorer {

class PeCubiclePrivate
{
public:
    struct ConfigInfo
    {
        ConfigInfo() : iPos(-1), strNumber("") {}

        int         iPos;
        QString     strNumber;
    };

public:
    PeCubiclePrivate() : m_strName(""), m_strNumber(""), m_strManufacture(""), m_pRoom(0) {}

    QString                     m_strName;
    QString                     m_strNumber;
    QString                     m_strManufacture;
    PeRoom                      *m_pRoom;
    QMap<PeDevice*, ConfigInfo> m_mapDeviceToConfigInfo;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeCubicle::PeCubicle() : PeProjectObject(PeProjectObject::otCubicle), m_d(new PeCubiclePrivate)
{
}

PeCubicle::PeCubicle(const PeCubicle &Cubicle) : PeProjectObject(Cubicle), m_d(new PeCubiclePrivate)
{
    *m_d = *Cubicle.m_d;
}

PeCubicle::PeCubicle(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otCubicle, pProjectVersion, iId), m_d(new PeCubiclePrivate)
{
}

PeCubicle::~PeCubicle()
{
    delete m_d;
}

QString PeCubicle::GetDisplayName() const
{
    if(!m_d->m_strName.isEmpty())
        return GetNumber() + ":" + m_d->m_strName;

    return GetNumber();
}

PeCubicle& PeCubicle::operator=(const PeCubicle &Cubicle)
{
    PeProjectObject::operator =(Cubicle);
    *m_d = *Cubicle.m_d;

    return *this;
}

QString PeCubicle::GetName() const
{
    return m_d->m_strName;
}

void PeCubicle::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

QString PeCubicle::GetNumber() const
{
    return m_d->m_strNumber;
}

void PeCubicle::SetNumber(const QString &strNumber)
{
    m_d->m_strNumber = strNumber;
}

QString PeCubicle::GetManufacture() const
{
    return m_d->m_strManufacture;
}

void PeCubicle::SetManufacture(const QString &strManufacture)
{
    m_d->m_strManufacture = strManufacture;
}

PeRoom* PeCubicle::GetParentRoom() const
{
    return m_d->m_pRoom;
}

void PeCubicle::SetParentRoom(PeRoom *pRoom)
{
    m_d->m_pRoom = pRoom;
}

void PeCubicle::AddChildDevice(PeDevice *pDevice)
{
    if(m_d->m_mapDeviceToConfigInfo.contains(pDevice))
        return;

    m_d->m_mapDeviceToConfigInfo.insert(pDevice, PeCubiclePrivate::ConfigInfo());
}

void PeCubicle::RemoveChildDevice(PeDevice *pDevice)
{
    m_d->m_mapDeviceToConfigInfo.remove(pDevice);
}

QList<PeDevice*> PeCubicle::GetChildDevices() const
{
    return m_d->m_mapDeviceToConfigInfo.keys();
}

int PeCubicle::GetChildDevicePos(PeDevice *pDevice) const
{
    PeCubiclePrivate::ConfigInfo info = m_d->m_mapDeviceToConfigInfo.value(pDevice);
    return info.iPos;
}

void PeCubicle::SetChildDevicePos(PeDevice *pDevice, int iPos)
{
    if(!m_d->m_mapDeviceToConfigInfo.contains(pDevice))
        return;

    PeCubiclePrivate::ConfigInfo &info = m_d->m_mapDeviceToConfigInfo[pDevice];
    info.iPos = iPos;
}

QString PeCubicle::GetChildDeviceNumber(PeDevice *pDevice) const
{
    PeCubiclePrivate::ConfigInfo info = m_d->m_mapDeviceToConfigInfo.value(pDevice);
    return info.strNumber;
}

void PeCubicle::SetChildDeviceNumber(PeDevice *pDevice, const QString &strNumber)
{
    if(!m_d->m_mapDeviceToConfigInfo.contains(pDevice))
        return;

    PeCubiclePrivate::ConfigInfo &info = m_d->m_mapDeviceToConfigInfo[pDevice];
    info.strNumber = strNumber;
}

bool PeCubicle::IsYardCubile() const
{
    return (!m_d->m_pRoom || m_d->m_pRoom->GetYard());
}
