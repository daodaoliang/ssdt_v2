#include <QMap>

#include "pbtpinfoset.h"
#include "basemanager.h"
#include "pbtpdevice.h"
#include "pbtpbay.h"
#include "pbtpport.h"

namespace ProjectExplorer {

class PbTpInfoSetPrivate
{
public:
    PbTpInfoSetPrivate()
    {
        m_strName = "";
        m_eInfoSetType = PbTpInfoSet::itGoose;
        m_iGroup = 0;
        m_pTxIED = 0;
        m_pTxPort = 0;
        m_pRxIED = 0;
        m_pRxPort = 0;
        m_pSwitch1 = 0;
        m_pSwitch1TxPort = 0;
        m_pSwitch1RxPort = 0;
        m_pSwitch2 = 0;
        m_pSwitch2TxPort = 0;
        m_pSwitch2RxPort = 0;
        m_pSwitch3 = 0;
        m_pSwitch3TxPort = 0;
        m_pSwitch3RxPort = 0;
        m_pSwitch4 = 0;
        m_pSwitch4TxPort = 0;
        m_pSwitch4RxPort = 0;
        m_pTpBay = 0;
    }

    QString                     m_strName;
    PbTpInfoSet::InfoSetType    m_eInfoSetType;
    int                         m_iGroup;
    PbTpDevice                  *m_pTxIED;
    PbTpPort                    *m_pTxPort;
    PbTpDevice                  *m_pRxIED;
    PbTpPort                    *m_pRxPort;
    PbTpDevice                  *m_pSwitch1;
    PbTpPort                    *m_pSwitch1TxPort;
    PbTpPort                    *m_pSwitch1RxPort;
    PbTpDevice                  *m_pSwitch2;
    PbTpPort                    *m_pSwitch2TxPort;
    PbTpPort                    *m_pSwitch2RxPort;
    PbTpDevice                  *m_pSwitch3;
    PbTpPort                    *m_pSwitch3TxPort;
    PbTpPort                    *m_pSwitch3RxPort;
    PbTpDevice                  *m_pSwitch4;
    PbTpPort                    *m_pSwitch4TxPort;
    PbTpPort                    *m_pSwitch4RxPort;

    PbTpBay                     *m_pTpBay;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbTpInfoSet::PbTpInfoSet() : PbBaseObject(PbBaseObject::botTpInfoSet), m_d(new PbTpInfoSetPrivate)
{
}

PbTpInfoSet::PbTpInfoSet(const PbTpInfoSet &InfoSet) : PbBaseObject(InfoSet), m_d(new PbTpInfoSetPrivate)
{
    *m_d = *InfoSet.m_d;
}

PbTpInfoSet::PbTpInfoSet(int iId) :
    PbBaseObject(PbBaseObject::botTpInfoSet, iId), m_d(new PbTpInfoSetPrivate)
{
}

PbTpInfoSet::~PbTpInfoSet()
{
    delete m_d;
}

QString PbTpInfoSet::GetInfoSetTypeName(const InfoSetType &eInfoSetType)
{
    static QMap<InfoSetType, QString> s_mapInfoSetTypeToName;
    if(s_mapInfoSetTypeToName.isEmpty())
    {
        s_mapInfoSetTypeToName.insert(itNone, "None");
        s_mapInfoSetTypeToName.insert(itGoose, "GOOSE");
        s_mapInfoSetTypeToName.insert(itSv, "SV");
        s_mapInfoSetTypeToName.insert(itTime, "Time");
        s_mapInfoSetTypeToName.insert(itMix, "Mix");
    }

    return s_mapInfoSetTypeToName.value(eInfoSetType, tr("None"));
}

QString PbTpInfoSet::GetDisplayName() const
{
    return GetName();
}

PbTpInfoSet& PbTpInfoSet::operator=(const PbTpInfoSet &InfoSet)
{
    PbBaseObject::operator =(InfoSet);
    *m_d = *InfoSet.m_d;

    return *this;
}

QString PbTpInfoSet::GetName() const
{
    return m_d->m_strName;
}

void PbTpInfoSet::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

PbTpInfoSet::InfoSetType PbTpInfoSet::GetInfoSetType() const
{
    return m_d->m_eInfoSetType;
}

void PbTpInfoSet::SetInfoSetType(const InfoSetType &eInfoSetType)
{
    m_d->m_eInfoSetType = eInfoSetType;
}

int PbTpInfoSet::GetGroup() const
{
    return m_d->m_iGroup;
}

void PbTpInfoSet::SetGroup(int iGroup) const
{
    m_d->m_iGroup = iGroup;
}

PbTpDevice* PbTpInfoSet::GetTxIED() const
{
    return m_d->m_pTxIED;
}

void PbTpInfoSet::SetTxIED(PbTpDevice *pTxIED)
{
    m_d->m_pTxIED = pTxIED;
}

PbTpPort* PbTpInfoSet::GetTxPort() const
{
    return m_d->m_pTxPort;
}

void PbTpInfoSet::SetTxPort(PbTpPort *pTxPort)
{
    m_d->m_pTxPort = pTxPort;
}

PbTpDevice* PbTpInfoSet::GetRxIED() const
{
    return m_d->m_pRxIED;
}

void PbTpInfoSet::SetRxIED(PbTpDevice *pRxIED)
{
    m_d->m_pRxIED = pRxIED;
}

PbTpPort* PbTpInfoSet::GetRxPort() const
{
    return m_d->m_pRxPort;
}

void PbTpInfoSet::SetRxPort(PbTpPort *pRxPort)
{
    m_d->m_pRxPort = pRxPort;
}

PbTpDevice* PbTpInfoSet::GetSwitch1() const
{
    return m_d->m_pSwitch1;
}

void PbTpInfoSet::SetSwitch1(PbTpDevice *pSwitch1)
{
    m_d->m_pSwitch1 = pSwitch1;
}

PbTpPort* PbTpInfoSet::GetSwitch1TxPort() const
{
    return m_d->m_pSwitch1TxPort;
}

void PbTpInfoSet::SetSwitch1TxPort(PbTpPort *pSwitch1TxPort)
{
     m_d->m_pSwitch1TxPort = pSwitch1TxPort;
}

PbTpPort* PbTpInfoSet::GetSwitch1RxPort() const
{
    return m_d->m_pSwitch1RxPort;
}

void PbTpInfoSet::SetSwitch1RxPort(PbTpPort *pSwitch1RxPort)
{
    m_d->m_pSwitch1RxPort = pSwitch1RxPort;
}

PbTpDevice* PbTpInfoSet::GetSwitch2() const
{
    return m_d->m_pSwitch2;
}

void PbTpInfoSet::SetSwitch2(PbTpDevice *pSwitch2)
{
    m_d->m_pSwitch2 = pSwitch2;
}

PbTpPort* PbTpInfoSet::GetSwitch2TxPort() const
{
    return m_d->m_pSwitch2TxPort;
}

void PbTpInfoSet::SetSwitch2TxPort(PbTpPort *pSwitch2TxPort)
{
     m_d->m_pSwitch2TxPort = pSwitch2TxPort;
}

PbTpPort* PbTpInfoSet::GetSwitch2RxPort() const
{
    return m_d->m_pSwitch2RxPort;
}

void PbTpInfoSet::SetSwitch2RxPort(PbTpPort *pSwitch2RxPort)
{
    m_d->m_pSwitch2RxPort = pSwitch2RxPort;
}

PbTpDevice* PbTpInfoSet::GetSwitch3() const
{
    return m_d->m_pSwitch3;
}

void PbTpInfoSet::SetSwitch3(PbTpDevice *pSwitch3)
{
    m_d->m_pSwitch3 = pSwitch3;
}

PbTpPort* PbTpInfoSet::GetSwitch3TxPort() const
{
    return m_d->m_pSwitch3TxPort;
}

void PbTpInfoSet::SetSwitch3TxPort(PbTpPort *pSwitch3TxPort)
{
     m_d->m_pSwitch3TxPort = pSwitch3TxPort;
}

PbTpPort* PbTpInfoSet::GetSwitch3RxPort() const
{
    return m_d->m_pSwitch3RxPort;
}

void PbTpInfoSet::SetSwitch3RxPort(PbTpPort *pSwitch3RxPort)
{
    m_d->m_pSwitch3RxPort = pSwitch3RxPort;
}

PbTpDevice* PbTpInfoSet::GetSwitch4() const
{
    return m_d->m_pSwitch4;
}

void PbTpInfoSet::SetSwitch4(PbTpDevice *pSwitch4)
{
    m_d->m_pSwitch4 = pSwitch4;
}

PbTpPort* PbTpInfoSet::GetSwitch4TxPort() const
{
    return m_d->m_pSwitch4TxPort;
}

void PbTpInfoSet::SetSwitch4TxPort(PbTpPort *pSwitch4TxPort)
{
     m_d->m_pSwitch4TxPort = pSwitch4TxPort;
}

PbTpPort* PbTpInfoSet::GetSwitch4RxPort() const
{
    return m_d->m_pSwitch4RxPort;
}

void PbTpInfoSet::SetSwitch4RxPort(PbTpPort *pSwitch4RxPort)
{
    m_d->m_pSwitch4RxPort = pSwitch4RxPort;
}

PbTpInfoSet* PbTpInfoSet::GetPairInfoSet() const
{
    foreach(PbTpInfoSet *pInfoSet, BaseManager::Instance()->GetAllTpInfoSets())
    {
        if(pInfoSet != this && pInfoSet->GetGroup() == m_d->m_iGroup)
            return pInfoSet;
    }

    return 0;
}

bool PbTpInfoSet::IsContaintPort(ProjectExplorer::PbTpPort *pPort, ProjectExplorer::PbTpPort **ppPortConnected) const
{
    if(!pPort)
        return false;

    if(pPort == m_d->m_pTxPort)
    {
        if(m_d->m_pSwitch1)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pSwitch1RxPort;
        }
        else if(m_d->m_pRxIED)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pRxPort;
        }

        return true;
    }
    else if(pPort == m_d->m_pRxPort)
    {
        if(m_d->m_pSwitch4)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pSwitch4TxPort;
        }
        else if(m_d->m_pSwitch3)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pSwitch3TxPort;
        }
        else if(m_d->m_pSwitch2)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pSwitch2TxPort;
        }
        else if(m_d->m_pSwitch1)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pSwitch1TxPort;
        }
        else if(m_d->m_pTxIED)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pTxPort;
        }

        return true;
    }
    else if(pPort == m_d->m_pSwitch1TxPort)
    {
        if(m_d->m_pSwitch2)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pSwitch2RxPort;
        }
        else if(m_d->m_pRxIED)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pRxPort;
        }

        return true;
    }
    else if(pPort == m_d->m_pSwitch1RxPort)
    {
        if(m_d->m_pTxIED)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pTxPort;
        }

        return true;
    }
    else if(pPort == m_d->m_pSwitch2TxPort)
    {
        if(m_d->m_pSwitch3)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pSwitch3RxPort;
        }
        else if(m_d->m_pRxIED)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pRxPort;
        }

        return true;
    }
    else if(pPort == m_d->m_pSwitch2RxPort)
    {
        if(m_d->m_pSwitch1)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pSwitch1TxPort;
        }

        return true;
    }
    else if(pPort == m_d->m_pSwitch3TxPort)
    {
        if(m_d->m_pSwitch4)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pSwitch4RxPort;
        }
        else if(m_d->m_pRxIED)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pRxPort;
        }

        return true;
    }
    else if(pPort == m_d->m_pSwitch3RxPort)
    {
        if(m_d->m_pSwitch2)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pSwitch2TxPort;
        }

        return true;
    }
    else if(pPort == m_d->m_pSwitch4TxPort)
    {
        if(m_d->m_pRxIED)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pRxPort;
        }

        return true;
    }
    else if(pPort == m_d->m_pSwitch4RxPort)
    {
        if(m_d->m_pSwitch3)
        {
            if(ppPortConnected)
                *ppPortConnected = m_d->m_pSwitch3TxPort;
        }

        return true;
    }

    return false;
}

bool PbTpInfoSet::ReplacePort(ProjectExplorer::PbTpPort *pPort, ProjectExplorer::PbTpPort *pPortReplaced)
{
    if(!pPort)
        return false;

    ProjectExplorer::PbTpDevice *pDeviceReplaced = pPortReplaced ? pPortReplaced->GetParentDevice() : 0;
    if(pPort == m_d->m_pTxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PbTpDevice::dtIED)
            return false;

        m_d->m_pTxPort = pPortReplaced;
        if(pDeviceReplaced && m_d->m_pTxIED != pDeviceReplaced)
            m_d->m_pTxIED = pDeviceReplaced;
    }
    else if(pPort == m_d->m_pRxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PbTpDevice::dtIED)
            return false;

        m_d->m_pRxPort = pPortReplaced;
        if(pDeviceReplaced && m_d->m_pRxIED != pDeviceReplaced)
            m_d->m_pRxIED = pDeviceReplaced;
    }
    else if(pPort == m_d->m_pSwitch1TxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PbTpDevice::dtSwitch)
            return false;

        m_d->m_pSwitch1TxPort = pPortReplaced;
        if(pDeviceReplaced && m_d->m_pSwitch1 != pDeviceReplaced)
        {
            m_d->m_pSwitch1 = pDeviceReplaced;
            m_d->m_pSwitch1RxPort = 0;
        }
    }
    else if(pPort == m_d->m_pSwitch1RxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PbTpDevice::dtSwitch)
            return false;

        m_d->m_pSwitch1RxPort = pPortReplaced;
        if(pDeviceReplaced && m_d->m_pSwitch1 != pDeviceReplaced)
        {
            m_d->m_pSwitch1 = pDeviceReplaced;
            m_d->m_pSwitch1TxPort = 0;
        }
    }
    else if(pPort == m_d->m_pSwitch2TxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PbTpDevice::dtSwitch)
            return false;

        m_d->m_pSwitch2TxPort = pPortReplaced;
        if(pDeviceReplaced && m_d->m_pSwitch2 != pDeviceReplaced)
        {
            m_d->m_pSwitch2 = pDeviceReplaced;
            m_d->m_pSwitch2RxPort = 0;
        }
    }
    else if(pPort == m_d->m_pSwitch2RxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PbTpDevice::dtSwitch)
            return false;

        m_d->m_pSwitch2RxPort = pPortReplaced;
        if(pDeviceReplaced && m_d->m_pSwitch2 != pDeviceReplaced)
        {
            m_d->m_pSwitch2 = pDeviceReplaced;
            m_d->m_pSwitch2TxPort = 0;
        }
    }
    else if(pPort == m_d->m_pSwitch3TxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PbTpDevice::dtSwitch)
            return false;

        m_d->m_pSwitch3TxPort = pPortReplaced;
        if(pDeviceReplaced && m_d->m_pSwitch3 != pDeviceReplaced)
        {
            m_d->m_pSwitch3 = pDeviceReplaced;
            m_d->m_pSwitch3RxPort = 0;
        }
    }
    else if(pPort == m_d->m_pSwitch3RxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PbTpDevice::dtSwitch)
            return false;

        m_d->m_pSwitch3RxPort = pPortReplaced;
        if(pDeviceReplaced && m_d->m_pSwitch3 != pDeviceReplaced)
        {
            m_d->m_pSwitch3 = pDeviceReplaced;
            m_d->m_pSwitch3TxPort = 0;
        }
    }
    else if(pPort == m_d->m_pSwitch4TxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PbTpDevice::dtSwitch)
            return false;

        m_d->m_pSwitch4TxPort = pPortReplaced;
        if(pDeviceReplaced && m_d->m_pSwitch4 != pDeviceReplaced)
        {
            m_d->m_pSwitch4 = pDeviceReplaced;
            m_d->m_pSwitch4RxPort = 0;
        }
    }
    else if(pPort == m_d->m_pSwitch4RxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PbTpDevice::dtSwitch)
            return false;

        m_d->m_pSwitch4RxPort = pPortReplaced;
        if(pDeviceReplaced && m_d->m_pSwitch4 != pDeviceReplaced)
        {
            m_d->m_pSwitch4 = pDeviceReplaced;
            m_d->m_pSwitch4TxPort = 0;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool PbTpInfoSet::IsComplete() const
{
    if(!m_d->m_pTxIED || !m_d->m_pTxPort || !m_d->m_pRxIED || !m_d->m_pRxPort)
        return false;

    if(m_d->m_pSwitch1 && (!m_d->m_pSwitch1TxPort || !m_d->m_pSwitch1RxPort))
        return false;

    if(m_d->m_pSwitch2 && (!m_d->m_pSwitch2TxPort || !m_d->m_pSwitch2RxPort))
        return false;

    if(m_d->m_pSwitch3 && (!m_d->m_pSwitch3TxPort || !m_d->m_pSwitch3RxPort))
        return false;

    if(m_d->m_pSwitch4 && (!m_d->m_pSwitch4TxPort || !m_d->m_pSwitch4RxPort))
        return false;

    return true;
}

PbTpBay* PbTpInfoSet::GetParentBay() const
{
    return m_d->m_pTpBay;
}

void PbTpInfoSet::SetParentBay(PbTpBay *pTpBay)
{
    m_d->m_pTpBay = pTpBay;
}
