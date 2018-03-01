#include <QMap>

#include "peinfoset.h"
#include "peprojectversion.h"
#include "pedevice.h"
#include "pebay.h"
#include "peport.h"

namespace ProjectExplorer {

class PeInfoSetPrivate
{
public:
    PeInfoSetPrivate()
    {
        m_strName = "";
        m_strDescription = "";
        m_eInfoSetType = PeInfoSet::itGoose;
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
    }

    QString                 m_strName;
    QString                 m_strDescription;
    PeInfoSet::InfoSetType  m_eInfoSetType;
    int                     m_iGroup;
    PeDevice                *m_pTxIED;
    PePort                  *m_pTxPort;
    PeDevice                *m_pRxIED;
    PePort                  *m_pRxPort;
    PeDevice                *m_pSwitch1;
    PePort                  *m_pSwitch1TxPort;
    PePort                  *m_pSwitch1RxPort;
    PeDevice                *m_pSwitch2;
    PePort                  *m_pSwitch2TxPort;
    PePort                  *m_pSwitch2RxPort;
    PeDevice                *m_pSwitch3;
    PePort                  *m_pSwitch3TxPort;
    PePort                  *m_pSwitch3RxPort;
    PeDevice                *m_pSwitch4;
    PePort                  *m_pSwitch4TxPort;
    PePort                  *m_pSwitch4RxPort;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeInfoSet::PeInfoSet() : PeProjectObject(PeProjectObject::otInfoSet), m_d(new PeInfoSetPrivate)
{
}

PeInfoSet::PeInfoSet(const PeInfoSet &InfoSet) : PeProjectObject(InfoSet), m_d(new PeInfoSetPrivate)
{
    *m_d = *InfoSet.m_d;
}

PeInfoSet::PeInfoSet(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otInfoSet, pProjectVersion, iId), m_d(new PeInfoSetPrivate)
{
}

PeInfoSet::~PeInfoSet()
{
    delete m_d;
}

QString PeInfoSet::GetInfoSetTypeName(const InfoSetType &eInfoSetType)
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

QString PeInfoSet::GetDisplayName() const
{
    if(!m_d->m_strDescription.isEmpty())
        return m_d->m_strName + " : " + m_d->m_strDescription;

    return m_d->m_strName;
}

PeInfoSet& PeInfoSet::operator=(const PeInfoSet &InfoSet)
{
    PeProjectObject::operator =(InfoSet);
    *m_d = *InfoSet.m_d;

    return *this;
}

QString PeInfoSet::GetName() const
{
    return m_d->m_strName;
}

void PeInfoSet::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

QString PeInfoSet::GetDescription() const
{
    return m_d->m_strDescription;
}

void PeInfoSet::SetDescription(const QString &strDescription)
{
    m_d->m_strDescription = strDescription;
}

PeInfoSet::InfoSetType PeInfoSet::GetInfoSetType() const
{
    return m_d->m_eInfoSetType;
}

void PeInfoSet::SetInfoSetType(const InfoSetType &eInfoSetType)
{
    m_d->m_eInfoSetType = eInfoSetType;
}

int PeInfoSet::GetGroup() const
{
    return m_d->m_iGroup;
}

void PeInfoSet::SetGroup(int iGroup) const
{
    m_d->m_iGroup = iGroup;
}

PeDevice* PeInfoSet::GetTxIED() const
{
    return m_d->m_pTxIED;
}

void PeInfoSet::SetTxIED(PeDevice *pTxIED)
{
    m_d->m_pTxIED = pTxIED;
}

PePort* PeInfoSet::GetTxPort() const
{
    return m_d->m_pTxPort;
}

void PeInfoSet::SetTxPort(PePort *pTxPort)
{
    m_d->m_pTxPort = pTxPort;
}

PeDevice* PeInfoSet::GetRxIED() const
{
    return m_d->m_pRxIED;
}

void PeInfoSet::SetRxIED(PeDevice *pRxIED)
{
    m_d->m_pRxIED = pRxIED;
}

PePort* PeInfoSet::GetRxPort() const
{
    return m_d->m_pRxPort;
}

void PeInfoSet::SetRxPort(PePort *pRxPort)
{
    m_d->m_pRxPort = pRxPort;
}

PeDevice* PeInfoSet::GetSwitch1() const
{
    return m_d->m_pSwitch1;
}

void PeInfoSet::SetSwitch1(PeDevice *pSwitch1)
{
    m_d->m_pSwitch1 = pSwitch1;
}

PePort* PeInfoSet::GetSwitch1TxPort() const
{
    return m_d->m_pSwitch1TxPort;
}

void PeInfoSet::SetSwitch1TxPort(PePort *pSwitch1TxPort)
{
     m_d->m_pSwitch1TxPort = pSwitch1TxPort;
}

PePort* PeInfoSet::GetSwitch1RxPort() const
{
    return m_d->m_pSwitch1RxPort;
}

void PeInfoSet::SetSwitch1RxPort(PePort *pSwitch1RxPort)
{
    m_d->m_pSwitch1RxPort = pSwitch1RxPort;
}

PeDevice* PeInfoSet::GetSwitch2() const
{
    return m_d->m_pSwitch2;
}

void PeInfoSet::SetSwitch2(PeDevice *pSwitch2)
{
    m_d->m_pSwitch2 = pSwitch2;
}

PePort* PeInfoSet::GetSwitch2TxPort() const
{
    return m_d->m_pSwitch2TxPort;
}

void PeInfoSet::SetSwitch2TxPort(PePort *pSwitch2TxPort)
{
     m_d->m_pSwitch2TxPort = pSwitch2TxPort;
}

PePort* PeInfoSet::GetSwitch2RxPort() const
{
    return m_d->m_pSwitch2RxPort;
}

void PeInfoSet::SetSwitch2RxPort(PePort *pSwitch2RxPort)
{
    m_d->m_pSwitch2RxPort = pSwitch2RxPort;
}

PeDevice* PeInfoSet::GetSwitch3() const
{
    return m_d->m_pSwitch3;
}

void PeInfoSet::SetSwitch3(PeDevice *pSwitch3)
{
    m_d->m_pSwitch3 = pSwitch3;
}

PePort* PeInfoSet::GetSwitch3TxPort() const
{
    return m_d->m_pSwitch3TxPort;
}

void PeInfoSet::SetSwitch3TxPort(PePort *pSwitch3TxPort)
{
     m_d->m_pSwitch3TxPort = pSwitch3TxPort;
}

PePort* PeInfoSet::GetSwitch3RxPort() const
{
    return m_d->m_pSwitch3RxPort;
}

void PeInfoSet::SetSwitch3RxPort(PePort *pSwitch3RxPort)
{
    m_d->m_pSwitch3RxPort = pSwitch3RxPort;
}

PeDevice* PeInfoSet::GetSwitch4() const
{
    return m_d->m_pSwitch4;
}

void PeInfoSet::SetSwitch4(PeDevice *pSwitch4)
{
    m_d->m_pSwitch4 = pSwitch4;
}

PePort* PeInfoSet::GetSwitch4TxPort() const
{
    return m_d->m_pSwitch4TxPort;
}

void PeInfoSet::SetSwitch4TxPort(PePort *pSwitch4TxPort)
{
     m_d->m_pSwitch4TxPort = pSwitch4TxPort;
}

PePort* PeInfoSet::GetSwitch4RxPort() const
{
    return m_d->m_pSwitch4RxPort;
}

void PeInfoSet::SetSwitch4RxPort(PePort *pSwitch4RxPort)
{
    m_d->m_pSwitch4RxPort = pSwitch4RxPort;
}

void PeInfoSet::UpdateName(const QStringList &lstExtraInfoSetNames)
{
    static QMap<InfoSetType, QString> s_mapInfoSetTypeToNameType;
    if(s_mapInfoSetTypeToNameType.isEmpty())
    {
        s_mapInfoSetTypeToNameType.insert(itGoose, "GS");
        s_mapInfoSetTypeToNameType.insert(itSv, "SV");
        s_mapInfoSetTypeToNameType.insert(itTime, "TM");
        s_mapInfoSetTypeToNameType.insert(itMix, "GV");
    }

    if(m_d->m_eInfoSetType == itNone)
    {
        m_d->m_strName = "";
        return;
    }

    QString strSetIndex;
    ProjectExplorer::PeDevice *pDevice = 0;
    if(!GetSetIndexAndMainDevice(strSetIndex, &pDevice))
    {
        m_d->m_strName = "";
        return;
    }

    ProjectExplorer::PeBay *pBay = pDevice->GetParentBay();
    if(!pBay)
    {
        m_d->m_strName = "";
        return;
    }

    QString strInfoSetName = "";
    QString strInfoSetNameWithoutIndex = QString("%1-%2-%3").arg(pBay->GetNumber())
                                                            .arg(s_mapInfoSetTypeToNameType.value(m_d->m_eInfoSetType))
                                                            .arg(strSetIndex);

    QStringList lstInfoSetNames;
    foreach(ProjectExplorer::PeInfoSet *pInfoSet, GetProjectVersion()->GetAllInfoSets())
    {
        if(GetId() == PeProjectObject::m_iInvalidObjectId || pInfoSet->GetName() != m_d->m_strName)
            lstInfoSetNames.append(pInfoSet->GetName());
    }
    foreach(const QString &strExtraInfoSetName, lstExtraInfoSetNames)
    {
        if(!strExtraInfoSetName.isEmpty() && !lstInfoSetNames.contains(strExtraInfoSetName))
            lstInfoSetNames.append(strExtraInfoSetName);
    }

    QString strOldIndex = m_d->m_strName.length() >= 2 && GetId() != PeProjectObject::m_iInvalidObjectId ? m_d->m_strName.right(2) : "";
    if(!strOldIndex.isEmpty())
    {
        strInfoSetName = strInfoSetNameWithoutIndex + strOldIndex;
        if(lstInfoSetNames.contains(strInfoSetName))
            strInfoSetName = "";
    }

    if(strInfoSetName.isEmpty())
    {
        for(int i = 1; i <= 99; i++)
        {
            strInfoSetName = strInfoSetNameWithoutIndex + QString("%1").arg(i, 2, 10, QLatin1Char('0'));
            if(!lstInfoSetNames.contains(strInfoSetName))
                break;
        }
    }

    m_d->m_strName = strInfoSetName;
}

PeInfoSet* PeInfoSet::GetPairInfoSet() const
{
    PeProjectVersion *pProjectVersion = GetProjectVersion();
    if(!pProjectVersion)
        return 0;

    foreach(PeInfoSet *pInfoSet, pProjectVersion->GetAllInfoSets())
    {
        if(pInfoSet != this && pInfoSet->GetGroup() == m_d->m_iGroup)
            return pInfoSet;
    }

    return 0;
}

bool PeInfoSet::GetSetIndexAndMainDevice(QString &strSetIndex, ProjectExplorer::PeDevice **ppMainDevice) const
{
    if(!m_d->m_pTxIED || !m_d->m_pRxIED)
        return false;

    PeDevice *pDevice = m_d->m_pTxIED;
    if(m_d->m_pTxIED->GetName().startsWith(QLatin1String("R")) ||
            m_d->m_pTxIED->GetName().startsWith(QLatin1String("C")) ||
            m_d->m_pTxIED->GetName().startsWith(QLatin1String("PMU")) ||
            m_d->m_pTxIED->GetName().startsWith(QLatin1String("PV")) ||
            m_d->m_pTxIED->GetName().startsWith(QLatin1String("PZ")))
    {
        pDevice = m_d->m_pRxIED;
    }
    else if(m_d->m_pTxIED->GetName().startsWith(QLatin1String("PM")) ||
            m_d->m_pTxIED->GetName().startsWith(QLatin1String("IM")) ||
            m_d->m_pTxIED->GetName().startsWith(QLatin1String("MM")))
    {
        if(m_d->m_pRxIED && !m_d->m_pRxIED->GetName().startsWith(QLatin1String("R")))
            pDevice = m_d->m_pRxIED;
    }
    else if(m_d->m_pTxIED->GetName().startsWith(QLatin1String("PT")) ||
            m_d->m_pTxIED->GetName().startsWith(QLatin1String("MT")) ||
            m_d->m_pTxIED->GetName().startsWith(QLatin1String("IT")))
    {
        if(m_d->m_pRxIED && !m_d->m_pRxIED->GetName().startsWith(QLatin1String("R")) && !m_d->m_pRxIED->GetName().startsWith(QLatin1String("PM")))
            pDevice = m_d->m_pRxIED;
    }

    QList<ProjectExplorer::PeDevice*> lstDevicesForSetIndex = QList<ProjectExplorer::PeDevice*>() << pDevice
                                                                                                  << (pDevice == m_d->m_pTxIED ? m_d->m_pRxIED : m_d->m_pTxIED)
                                                                                                  << m_d->m_pSwitch1
                                                                                                  << m_d->m_pSwitch2
                                                                                                  << m_d->m_pSwitch3
                                                                                                  << m_d->m_pSwitch4;
    strSetIndex.clear();
    foreach(ProjectExplorer::PeDevice *pDeviceForSetIndex, lstDevicesForSetIndex)
    {
        if(pDeviceForSetIndex && !pDeviceForSetIndex->GetName().isEmpty())
        {
            QString strTemp = pDeviceForSetIndex->GetName().right(1);
            if(strTemp >= "A" && strTemp <= "Z")
            {
                strSetIndex = strTemp;
                break;
            }
        }
    }

    if(ppMainDevice)
        *ppMainDevice = pDevice;

    return true;
}

bool PeInfoSet::IsContaintPort(ProjectExplorer::PePort *pPort, ProjectExplorer::PePort **ppPortConnected) const
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

bool PeInfoSet::ReplacePort(ProjectExplorer::PePort *pPort, ProjectExplorer::PePort *pPortReplaced)
{
    if(!pPort)
        return false;

    ProjectExplorer::PeDevice *pDeviceReplaced = pPortReplaced ? pPortReplaced->GetParentDevice() : 0;
    if(pPort == m_d->m_pTxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PeDevice::dtIED)
            return false;

        m_d->m_pTxPort = pPortReplaced;
        if(pDeviceReplaced && m_d->m_pTxIED != pDeviceReplaced)
            m_d->m_pTxIED = pDeviceReplaced;
    }
    else if(pPort == m_d->m_pRxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PeDevice::dtIED)
            return false;

        m_d->m_pRxPort = pPortReplaced;
        if(pDeviceReplaced && m_d->m_pRxIED != pDeviceReplaced)
            m_d->m_pRxIED = pDeviceReplaced;
    }
    else if(pPort == m_d->m_pSwitch1TxPort)
    {
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
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
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
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
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
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
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
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
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
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
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
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
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
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
        if(pDeviceReplaced && pDeviceReplaced->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
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

bool PeInfoSet::IsComplete() const
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
