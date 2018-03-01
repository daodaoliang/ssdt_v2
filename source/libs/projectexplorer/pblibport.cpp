#include <QMap>

#include "pblibport.h"
#include "pblibboard.h"

namespace ProjectExplorer {

class PbLibPortPrivate
{
public:
    PbLibPortPrivate()
        : m_strName(""), m_iGroup(0), m_ePortType(PbLibPort::ptFiber), m_ePortDirection(PbLibPort::pdTx), m_eFiberPlug(PbLibPort::fpST), m_eFiberMode(PbLibPort::fmMultiple), m_bCascade(false), m_pBoard(0)
    {
    }

    QString                     m_strName;
    int                         m_iGroup;
    PbLibPort::PortType         m_ePortType;
    PbLibPort::PortDirection    m_ePortDirection;
    PbLibPort::FiberPlug        m_eFiberPlug;
    PbLibPort::FiberMode        m_eFiberMode;
    bool                        m_bCascade;
    PbLibBoard*                    m_pBoard;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbLibPort::PbLibPort() : PbBaseObject(PbBaseObject::botLibPort), m_d(new PbLibPortPrivate)
{
}

PbLibPort::PbLibPort(const PbLibPort &Port) : PbBaseObject(Port), m_d(new PbLibPortPrivate)
{
    *m_d = *Port.m_d;
}

PbLibPort::PbLibPort(int iId) : PbBaseObject(PbBaseObject::botLibPort, iId), m_d(new PbLibPortPrivate)
{
}

PbLibPort::~PbLibPort()
{
    delete m_d;
}

QString PbLibPort::GetPortTypeName(const PortType &ePortType)
{
    static QMap<PortType, QString> s_mapPortTypeToName;
    if(s_mapPortTypeToName.isEmpty())
    {
        s_mapPortTypeToName.insert(ptFiber, tr("Fiber"));
        s_mapPortTypeToName.insert(ptRJ45, tr("RJ45"));
        s_mapPortTypeToName.insert(ptOdf, tr("ODF"));
    }

    return s_mapPortTypeToName.value(ePortType, tr("Unknown"));
}

QString PbLibPort::GetPortDirectionName(const PortDirection &ePortDirection)
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

QString PbLibPort::GetFiberPlugName(const FiberPlug &eFiberPlug)
{
    static QMap<FiberPlug, QString> s_mapFiberPlugToName;
    if(s_mapFiberPlugToName.isEmpty())
    {
        s_mapFiberPlugToName.insert(fpNone, "None");
        s_mapFiberPlugToName.insert(fpST, "ST");
        s_mapFiberPlugToName.insert(fpLC, "LC");
        s_mapFiberPlugToName.insert(fpSC, "SC");
        s_mapFiberPlugToName.insert(fpFC, "FC");
    }

    return s_mapFiberPlugToName.value(eFiberPlug, tr("None"));
}

QString PbLibPort::GetFiberModeName(const FiberMode &eFiberMode)
{
    static QMap<FiberMode, QString> s_mapFiberModeToName;
    if(s_mapFiberModeToName.isEmpty())
    {
        s_mapFiberModeToName.insert(fmNone, tr("None"));
        s_mapFiberModeToName.insert(fmSingle, tr("Single"));
        s_mapFiberModeToName.insert(fmMultiple, tr("Multiple"));
    }

    return s_mapFiberModeToName.value(eFiberMode, tr("None"));
}

bool PbLibPort::CompareGroup(PbLibPort *pPort1, PbLibPort *pPort2)
{
    return pPort1->GetGroup() < pPort2->GetGroup();
}

QString PbLibPort::GetDisplayName() const
{
    return GetName();
}

PbLibPort& PbLibPort::operator=(const PbLibPort &Port)
{
    PbBaseObject::operator =(Port);
    *m_d = *Port.m_d;

    return *this;
}

QString PbLibPort::GetName() const
{
    return m_d->m_strName;
}

void PbLibPort::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

int PbLibPort::GetGroup() const
{
    return m_d->m_iGroup;
}

void PbLibPort::SetGroup(int iGroup)
{
    m_d->m_iGroup = iGroup;
}

PbLibPort::PortType PbLibPort::GetPortType() const
{
    return m_d->m_ePortType;
}

void PbLibPort::SetPortType(const PortType &ePortType)
{
    m_d->m_ePortType = ePortType;
}

PbLibPort::PortDirection PbLibPort::GetPortDirection() const
{
    return m_d->m_ePortDirection;
}

void PbLibPort::SetPortDirection(const PortDirection &ePortDirection)
{
    m_d->m_ePortDirection = ePortDirection;
}

PbLibPort::FiberPlug PbLibPort::GetFiberPlug() const
{
    return m_d->m_eFiberPlug;
}

void PbLibPort::SetFiberPlug(const FiberPlug &eFiberPlug)
{
    m_d->m_eFiberPlug = eFiberPlug;
}

PbLibPort::FiberMode PbLibPort::GetFiberMode() const
{
    return m_d->m_eFiberMode;
}

void PbLibPort::SetFiberMode(const FiberMode &eFiberMode)
{
    m_d->m_eFiberMode = eFiberMode;
}

bool PbLibPort::GetCascade() const
{
    return m_d->m_bCascade;
}

void PbLibPort::SetCascade(bool bCascade)
{
    m_d->m_bCascade = bCascade;
}

PbLibBoard* PbLibPort::GetParentBoard() const
{
    return m_d->m_pBoard;
}

void PbLibPort::SetParentBoard(PbLibBoard *pBoard)
{
    m_d->m_pBoard = pBoard;
}

PbLibDevice* PbLibPort::GetParentDevice() const
{
    return (m_d->m_pBoard ? m_d->m_pBoard->GetParentDevice() : 0);
}

PbLibPort* PbLibPort::GetPairPort()
{
    if(!m_d->m_pBoard)
        return 0;

    if(m_d->m_ePortDirection == pdRT)
        return this;

    foreach(PbLibPort *pPort, m_d->m_pBoard->GetChildPorts())
    {
        if(pPort != this && pPort->GetGroup() == m_d->m_iGroup)
            return pPort;
    }

    return 0;
}
