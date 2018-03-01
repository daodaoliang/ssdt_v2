#include <QMap>

#include "peport.h"
#include "peprojectversion.h"
#include "pedevice.h"
#include "peboard.h"
#include "pefiber.h"

namespace ProjectExplorer {

class PePortPrivate
{
public:
    PePortPrivate()
        : m_strName(""), m_iGroup(0), m_ePortType(PePort::ptFiber), m_ePortDirection(PePort::pdTx), m_eFiberPlug(PePort::fpST), m_eFiberMode(PePort::fmMultiple), m_bCascade(false), m_pBoard(0)
    {
    }

    QString                 m_strName;
    int                     m_iGroup;
    PePort::PortType        m_ePortType;
    PePort::PortDirection   m_ePortDirection;
    PePort::FiberPlug       m_eFiberPlug;
    PePort::FiberMode       m_eFiberMode;
    bool                    m_bCascade;
    PeBoard*                m_pBoard;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PePort::PePort() : PeProjectObject(PeProjectObject::otPort), m_d(new PePortPrivate)
{
}

PePort::PePort(const PePort &Port) : PeProjectObject(Port), m_d(new PePortPrivate)
{
    *m_d = *Port.m_d;
}

PePort::PePort(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otPort, pProjectVersion, iId), m_d(new PePortPrivate)
{
}

PePort::~PePort()
{
    delete m_d;
}

QString PePort::GetPortTypeName(const PortType &ePortType)
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

QString PePort::GetPortDirectionName(const PortDirection &ePortDirection)
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

QString PePort::GetFiberPlugName(const FiberPlug &eFiberPlug)
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

QString PePort::GetFiberModeName(const FiberMode &eFiberMode)
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

bool PePort::CompareGroup(PePort *pPort1, PePort *pPort2)
{
    return pPort1->GetGroup() < pPort2->GetGroup();
}

QString PePort::GetDisplayName() const
{
    return GetName();
}

PePort& PePort::operator=(const PePort &Port)
{
    PeProjectObject::operator =(Port);
    *m_d = *Port.m_d;

    return *this;
}

QString PePort::GetName() const
{
    return m_d->m_strName;
}

void PePort::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

QString PePort::GetFullDisplayName() const
{
    if(ProjectExplorer::PeDevice *pDevice = GetParentDevice())
    {
        if(pDevice->GetDeviceType() == PeDevice::dtIED)
        {
            if(ProjectExplorer::PeBoard *pBoard = GetParentBoard())
                return QString("%1:%2").arg(pBoard->GetDisplayName()).arg(GetDisplayName());
        }
    }

    return GetDisplayName();
}

int PePort::GetGroup() const
{
    return m_d->m_iGroup;
}

QString PePort::GetGroupCh() const
{
    return QString('A' + m_d->m_iGroup - 1);
}

void PePort::SetGroup(int iGroup)
{
    m_d->m_iGroup = iGroup;
}

PePort::PortType PePort::GetPortType() const
{
    return m_d->m_ePortType;
}

void PePort::SetPortType(const PortType &ePortType)
{
    m_d->m_ePortType = ePortType;
}

PePort::PortDirection PePort::GetPortDirection() const
{
    return m_d->m_ePortDirection;
}

void PePort::SetPortDirection(const PortDirection &ePortDirection)
{
    m_d->m_ePortDirection = ePortDirection;
}

PePort::FiberPlug PePort::GetFiberPlug() const
{
    return m_d->m_eFiberPlug;
}

void PePort::SetFiberPlug(const FiberPlug &eFiberPlug)
{
    m_d->m_eFiberPlug = eFiberPlug;
}

PePort::FiberMode PePort::GetFiberMode() const
{
    return m_d->m_eFiberMode;
}

void PePort::SetFiberMode(const FiberMode &eFiberMode)
{
    m_d->m_eFiberMode = eFiberMode;
}

bool PePort::GetCascade() const
{
    return m_d->m_bCascade;
}

void PePort::SetCascade(bool bCascade)
{
    m_d->m_bCascade = bCascade;
}

PeBoard* PePort::GetParentBoard() const
{
    return m_d->m_pBoard;
}

void PePort::SetParentBoard(PeBoard *pBoard)
{
    m_d->m_pBoard = pBoard;
}

PeDevice* PePort::GetParentDevice() const
{
    return (m_d->m_pBoard ? m_d->m_pBoard->GetParentDevice() : 0);
}

PePort* PePort::GetPairPort()
{
    if(!m_d->m_pBoard)
        return 0;

    if(m_d->m_ePortDirection == pdRT)
        return this;

    foreach(PePort *pPort, m_d->m_pBoard->GetChildPorts())
    {
        if(pPort != this && pPort->GetGroup() == m_d->m_iGroup)
            return pPort;
    }

    return 0;
}

QList<PeFiber*> PePort::GetUsedFibers(const QList<PeFiber*> lstUnusedFibers) const
{
    QList<PeFiber*> lstUsedFibers;

    if(ProjectExplorer::PeProjectVersion *pProjectVersion = GetProjectVersion())
    {
        foreach(ProjectExplorer::PeFiber *pFiber, pProjectVersion->GetAllFibers())
        {
            if(lstUnusedFibers.contains(pFiber))
                continue;

            if(pFiber->GetPort1() == this || pFiber->GetPort2() == this)
                lstUsedFibers.append(pFiber);
        }
    }

    return lstUsedFibers;
}
