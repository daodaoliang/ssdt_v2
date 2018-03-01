#include <QMap>

#include "pedwg.h"
#include "peprojectversion.h"

namespace ProjectExplorer {

class PeDwgPrivate
{
public:
    PeDwgPrivate() : m_strName(""), m_strDescription("") {}

    PeDwg::DwgType      m_eDwgType;
    QString             m_strName;
    QString             m_strDescription;
    PeVolume*           m_pVolume;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeDwg::PeDwg() : PeProjectObject(PeProjectObject::otDwg), m_d(new PeDwgPrivate)
{
}

PeDwg::PeDwg(const PeDwg &Volume) : PeProjectObject(Volume), m_d(new PeDwgPrivate)
{
    *m_d = *Volume.m_d;
}

PeDwg::PeDwg(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otDwg, pProjectVersion, iId), m_d(new PeDwgPrivate)
{
}

PeDwg::~PeDwg()
{
    delete m_d;
}

QString PeDwg::GetDwgTypeName(const DwgType &eDwgType)
{
    static QMap<DwgType, QString> s_mapTypeToName;
    if(s_mapTypeToName.isEmpty())
    {
        s_mapTypeToName.insert(dtDeviceConfig, tr("Device Configuration Diagram"));
        s_mapTypeToName.insert(dtInfoSetGoose, tr("GOOSE Information Logic Diagram"));
        s_mapTypeToName.insert(dtInfoSetSv, tr("SV Information Logic Diagram"));
        s_mapTypeToName.insert(dtVirtualGoose, tr("GOOSE Virtual Terminal Diagram"));
        s_mapTypeToName.insert(dtVirtualSv, tr("SV Virtual Terminal Diagram"));
        s_mapTypeToName.insert(dtCableConn, tr("Cable Connection Diagram"));
        s_mapTypeToName.insert(dtNetwork, tr("Switch Networking Diagram"));
        s_mapTypeToName.insert(dtNetworkStructure, tr("Network Structure Diagram"));
    }

    return s_mapTypeToName.value(eDwgType, tr("Unknown"));
}

QString PeDwg::GetDisplayName() const
{
    return GetName();
}

PeDwg& PeDwg::operator=(const PeDwg &Volume)
{
    PeProjectObject::operator =(Volume);
    *m_d = *Volume.m_d;

    return *this;
}

PeDwg::DwgType PeDwg::GetDwgType() const
{
    return m_d->m_eDwgType;
}

void PeDwg::SetDwgType(DwgType eDwgType)
{
    m_d->m_eDwgType = eDwgType;
}

QString PeDwg::GetName() const
{
    return m_d->m_strName;
}

void PeDwg::SetName(const QString strName)
{
    m_d->m_strName = strName;
}

QString PeDwg::GetDescription() const
{
    return m_d->m_strDescription;
}

void PeDwg::SetDescription(const QString &strDescription)
{
    m_d->m_strDescription = strDescription;
}

PeVolume* PeDwg::GetParentVolume() const
{
    return m_d->m_pVolume;
}

void PeDwg::SetParentVolume(PeVolume *pVolume)
{
    m_d->m_pVolume = pVolume;
}

