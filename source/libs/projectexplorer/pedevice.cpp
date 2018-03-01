#include <QMap>

#include "pedevice.h"

namespace ProjectExplorer {

class PeDevicePrivate
{
public:
    PeDevicePrivate()
        : m_eDeviceType(PeDevice::dtInvalid), m_strName(""), m_strDeviceCategory(""), m_strBayCategory(""), m_iVLevel(0), m_strCircuitIndex(""), m_strSet(""), m_strDescription(""), m_strManufacture(""), m_strType(""), m_strConfigVersion(""), m_strIcdFileName(""), m_strCubiclePos(""), m_strCubicleNum(""), m_bHasModel(false), m_pCubicle(0), m_pBay(0)
    {
    }

    PeDevice::DeviceType    m_eDeviceType;
    QString                 m_strName;
    QString                 m_strDeviceCategory;
    QString                 m_strBayCategory;
    int                     m_iVLevel;
    QString                 m_strCircuitIndex;
    QString                 m_strSet;
    QString                 m_strDescription;
    QString                 m_strManufacture;
    QString                 m_strType;    
    QString                 m_strConfigVersion;
    QString                 m_strIcdFileName;
    QString                 m_strCubiclePos;
    QString                 m_strCubicleNum;
    bool                    m_bHasModel;
    PeCubicle*              m_pCubicle;
    PeBay*                  m_pBay;
    QList<PeBoard*>         m_lstBoards;
    QList<PeVTerminal*>     m_lstVTerminals;
    QList<PeStrap*>         m_lstStraps;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeDevice::PeDevice() : PeProjectObject(PeProjectObject::otDevice), m_d(new PeDevicePrivate)
{
}

PeDevice::PeDevice(const PeDevice &Device) : PeProjectObject(Device), m_d(new PeDevicePrivate)
{
    *m_d = *Device.m_d;
}

PeDevice::PeDevice(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otDevice, pProjectVersion, iId), m_d(new PeDevicePrivate)
{
}

PeDevice::~PeDevice()
{
    delete m_d;
}

QIcon PeDevice::GetDeviceIcon(const DeviceType &eDeviceType)
{
    static QMap<DeviceType, QIcon> s_mapTypeToIcon;
    if(s_mapTypeToIcon.isEmpty())
    {
        s_mapTypeToIcon.insert(dtIED, QIcon(":/projectexplorer/images/pe_ied.png"));
        s_mapTypeToIcon.insert(dtSwitch, QIcon(":/projectexplorer/images/pe_switch.png"));
        s_mapTypeToIcon.insert(dtODF, QIcon(":/projectexplorer/images/pe_odf.png"));
    }

    return s_mapTypeToIcon.value(eDeviceType, QIcon());
}

QString PeDevice::GetDeviceTypeName(const DeviceType &eDeviceType)
{
    static QMap<DeviceType, QString> s_mapTypeToName;
    if(s_mapTypeToName.isEmpty())
    {
        s_mapTypeToName.insert(dtIED, tr("IED"));
        s_mapTypeToName.insert(dtSwitch, tr("Switch"));
        s_mapTypeToName.insert(dtODF, tr("ODF"));
    }

    return s_mapTypeToName.value(eDeviceType, tr("Unknown"));
}

QIcon PeDevice::GetDisplayIcon() const
{
    return GetDeviceIcon(m_d->m_eDeviceType);
}

QString PeDevice::GetDisplayName() const
{
    if((m_d->m_eDeviceType == dtIED || m_d->m_eDeviceType == dtSwitch) && !m_d->m_strDescription.isEmpty())
        return GetName() + ":" + m_d->m_strDescription;

    return GetName();
}

QString PeDevice::GetObjectTypeName() const
{
    return GetDeviceTypeName(m_d->m_eDeviceType);
}

PeDevice& PeDevice::operator=(const PeDevice &Device)
{
    PeProjectObject::operator =(Device);
    *m_d = *Device.m_d;

    return *this;
}

PeDevice::DeviceType PeDevice::GetDeviceType() const
{
    return m_d->m_eDeviceType;
}

void PeDevice::SetDeviceType(const DeviceType &eDeviceType)
{
    m_d->m_eDeviceType = eDeviceType;
}

QString PeDevice::GetName() const
{
    return m_d->m_strName;
}

void PeDevice::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

QString PeDevice::GetDeviceCategory() const
{
    if(m_d->m_strDeviceCategory.isEmpty())
    {
        const int iLength = m_d->m_strName.length();
        if(iLength >= 5 && iLength <= 7)
            return m_d->m_strName.mid(0, 1);
    }

    return m_d->m_strDeviceCategory;
}

void PeDevice::SetDeviceCategory(const QString &strDeviceCategory)
{
    m_d->m_strDeviceCategory = strDeviceCategory;
}

QString PeDevice::GetBayCategory() const
{
    if(m_d->m_strBayCategory.isEmpty())
    {
        const int iLength = m_d->m_strName.length();
        if(iLength >= 5 && iLength <= 7)
            return m_d->m_strName.mid(1, 1);
    }

    return m_d->m_strBayCategory;
}

void PeDevice::SetBayCategory(const QString &strBayCategory)
{
    m_d->m_strBayCategory = strBayCategory;
}

int PeDevice::GetVLevel() const
{
    return m_d->m_iVLevel;
}

void PeDevice::SetVLevel(int iVLevel)
{
    m_d->m_iVLevel = iVLevel;
}

QString PeDevice::GetCircuitIndex() const
{
    if(m_d->m_strCircuitIndex.isEmpty())
    {
        const int iLength = m_d->m_strName.length();
        if(iLength == 5)
        {
            return m_d->m_strName.mid(3, 2);
        }
        else if(iLength == 6)
        {
            const QString strSet = m_d->m_strName.mid(5, 1).toUpper();
            if(strSet >= "A" && strSet <= "Z")
                return m_d->m_strName.mid(3, 2);
            else
                return m_d->m_strName.mid(4, 2);
        }
        else if(iLength == 7)
        {
            return m_d->m_strName.mid(4, 2);
        }
    }

    return m_d->m_strCircuitIndex;
}

void PeDevice::SetCircuitIndex(const QString &strCircuitIndex)
{
    m_d->m_strCircuitIndex = strCircuitIndex;
}

QString PeDevice::GetSet() const
{
    if(m_d->m_strSet.isEmpty())
    {
        const int iLength = m_d->m_strName.length();
        if(iLength == 6 || iLength == 7)
        {
            const QString strSet = m_d->m_strName.mid(5, 1).toUpper();
            if(strSet >= "A" && strSet <= "Z")
                return strSet;
        }
    }

    return m_d->m_strSet;
}

void PeDevice::SetSet(const QString &strSet)
{
    m_d->m_strSet = strSet;
}

QString PeDevice::GetDescription() const
{
    return m_d->m_strDescription;
}

void PeDevice::SetDescription(const QString &strDescription)
{
    m_d->m_strDescription = strDescription;
}

QString PeDevice::GetManufacture() const
{
    return m_d->m_strManufacture;
}

void PeDevice::SetManufacture(const QString &strManufacture)
{
    m_d->m_strManufacture = strManufacture;
}

QString PeDevice::GetType() const
{
    return m_d->m_strType;
}

void PeDevice::SetType(const QString &strType)
{
    m_d->m_strType = strType;
}

QString PeDevice::GetConfigVersion() const
{
    return m_d->m_strConfigVersion;
}

void PeDevice::SetConfigVersion(const QString &strConfigVersion)
{
    m_d->m_strConfigVersion = strConfigVersion;
}

QString PeDevice::GetIcdFileName() const
{
    return m_d->m_strIcdFileName;
}

void PeDevice::SetIcdFileName(const QString &strIcdFileName)
{
    m_d->m_strIcdFileName = strIcdFileName;
}

PeCubicle* PeDevice::GetParentCubicle() const
{
    return m_d->m_pCubicle;
}

QString PeDevice::GetCubiclePos() const
{
    return m_d->m_strCubiclePos;
}

void PeDevice::SetCubiclePos(const QString &strCubiclePos)
{
    m_d->m_strCubiclePos = strCubiclePos;
}

QString PeDevice::GetCubicleNum() const
{
    return m_d->m_strCubicleNum;
}

void PeDevice::SetCubicleNum(const QString &strCubicleNum)
{
    m_d->m_strCubicleNum = strCubicleNum;
}

bool PeDevice::GetHasModel() const
{
    return m_d-> m_bHasModel;
}

void PeDevice::SetHasModel(bool bHasModel)
{
    m_d->m_bHasModel = bHasModel;
}

void PeDevice::SetParentCubicle(PeCubicle *pCubicle)
{
    m_d->m_pCubicle = pCubicle;
}

PeBay* PeDevice::GetParentBay() const
{
    return m_d->m_pBay;
}

void PeDevice::SetParentBay(PeBay *pBay)
{
    m_d->m_pBay = pBay;
}

void PeDevice::AddChildBoard(PeBoard *pBoard)
{
    if(!m_d->m_lstBoards.contains(pBoard))
        m_d->m_lstBoards.append(pBoard);
}

void PeDevice::RemoveChildBoard(PeBoard *pBoard)
{
    m_d->m_lstBoards.removeOne(pBoard);
}

QList<PeBoard*> PeDevice::GetChildBoards() const
{
    return m_d->m_lstBoards;
}

void PeDevice::AddVTerminal(PeVTerminal *pVTerminal)
{
    if(!m_d->m_lstVTerminals.contains(pVTerminal))
        m_d->m_lstVTerminals.append(pVTerminal);
}

void PeDevice::RemoveVTerminal(PeVTerminal *pVTerminal)
{
    m_d->m_lstVTerminals.removeOne(pVTerminal);
}

QList<PeVTerminal*> PeDevice::GetVTerminals() const
{
    return m_d->m_lstVTerminals;
}

void PeDevice::AddStrap(PeStrap *pStrap)
{
    if(!m_d->m_lstStraps.contains(pStrap))
        m_d->m_lstStraps.append(pStrap);
}

void PeDevice::RemoveStrap(PeStrap *pStrap)
{
    m_d->m_lstStraps.removeOne(pStrap);
}

QList<PeStrap*> PeDevice::GetStraps() const
{
    return m_d->m_lstStraps;
}
