#include <QMap>

#include "pbtpport.h"
#include "pbtpdevice.h"

namespace ProjectExplorer {

class PbTpPortPrivate
{
public:
    PbTpPortPrivate()
        : m_strName(""), m_iGroup(0), m_ePortDirection(PbTpPort::pdTx), m_pTpDevice(0)
    {
    }

    QString                 m_strName;
    int                     m_iGroup;
    PbTpPort::PortDirection m_ePortDirection;
    PbTpDevice*             m_pTpDevice;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbTpPort::PbTpPort() : PbBaseObject(PbBaseObject::botTpPort), m_d(new PbTpPortPrivate)
{
}

PbTpPort::PbTpPort(const PbTpPort &Port) : PbBaseObject(Port), m_d(new PbTpPortPrivate)
{
    *m_d = *Port.m_d;
}

PbTpPort::PbTpPort(int iId) : PbBaseObject(PbBaseObject::botTpPort, iId), m_d(new PbTpPortPrivate)
{
}

PbTpPort::~PbTpPort()
{
    delete m_d;
}

QString PbTpPort::GetPortDirectionName(const PortDirection &ePortDirection)
{
    static QMap<PortDirection, QString> s_mapPortDirectionToName;
    if(s_mapPortDirectionToName.isEmpty())
    {
        s_mapPortDirectionToName.insert(pdRx, "Rx");
        s_mapPortDirectionToName.insert(pdTx, "Tx");
        s_mapPortDirectionToName.insert(pdRT, "RT");
    }

    return s_mapPortDirectionToName.value(ePortDirection, tr("Unknown"));
}

bool PbTpPort::CompareGroup(PbTpPort *pPort1, PbTpPort *pPort2)
{
    return pPort1->GetGroup() < pPort2->GetGroup();
}

QString PbTpPort::GetDisplayName() const
{
    return GetName();
}

PbTpPort& PbTpPort::operator=(const PbTpPort &Port)
{
    PbBaseObject::operator =(Port);
    *m_d = *Port.m_d;

    return *this;
}

QString PbTpPort::GetName() const
{
    return m_d->m_strName;
}

void PbTpPort::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

int PbTpPort::GetGroup() const
{
    return m_d->m_iGroup;
}

void PbTpPort::SetGroup(int iGroup)
{
    m_d->m_iGroup = iGroup;
}

PbTpPort::PortDirection PbTpPort::GetPortDirection() const
{
    return m_d->m_ePortDirection;
}

void PbTpPort::SetPortDirection(const PortDirection &ePortDirection)
{
    m_d->m_ePortDirection = ePortDirection;
}

PbTpDevice* PbTpPort::GetParentDevice() const
{
    return m_d->m_pTpDevice;
}

void PbTpPort::SetParentDevice(PbTpDevice *pTpDevice)
{
    m_d->m_pTpDevice = pTpDevice;
}

PbTpPort* PbTpPort::GetPairPort()
{
    if(!m_d->m_pTpDevice)
        return 0;

    if(m_d->m_ePortDirection == pdRT)
        return this;

    foreach(PbTpPort *pPort, m_d->m_pTpDevice->GetChildPorts())
    {
        if(pPort != this && pPort->GetGroup() == m_d->m_iGroup)
            return pPort;
    }

    return 0;
}
