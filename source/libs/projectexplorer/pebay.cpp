#include "pebay.h"
#include "peprojectversion.h"

namespace ProjectExplorer {

class PeBayPrivate
{
public:
    PeBayPrivate() : m_strName(""), m_strNumber(""), m_iVLevel(0) {}

    QString             m_strName;
    QString             m_strNumber;
    int                 m_iVLevel;
    QList<PeDevice*>    m_lstDevices;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeBay::PeBay() : PeProjectObject(PeProjectObject::otBay), m_d(new PeBayPrivate)
{
}

PeBay::PeBay(const PeBay &Bay) : PeProjectObject(Bay), m_d(new PeBayPrivate)
{
    *m_d = *Bay.m_d;
}

PeBay::PeBay(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otBay, pProjectVersion, iId), m_d(new PeBayPrivate)
{
}

PeBay::~PeBay()
{
    delete m_d;
}

QString PeBay::GetDisplayName() const
{
    if(!m_d->m_strName.isEmpty())
        return GetNumber() + ":" + m_d->m_strName;

    return GetNumber();
}

PeBay& PeBay::operator=(const PeBay &Bay)
{
    PeProjectObject::operator =(Bay);
    *m_d = *Bay.m_d;

    return *this;
}

QString PeBay::GetName() const
{
    return m_d->m_strName;
}

void PeBay::SetName(const QString strName)
{
    m_d->m_strName = strName;
}

QString PeBay::GetNumber() const
{
    return m_d->m_strNumber;
}

void PeBay::SetNumber(const QString &strNumber)
{
    m_d->m_strNumber = strNumber;
}

int PeBay::GetVLevel() const
{
    return m_d->m_iVLevel;
}

void PeBay::SetVLevel(int iVLevel)
{
    m_d->m_iVLevel = iVLevel;
}

void PeBay::AddChildDevice(PeDevice *pDevice)
{
    if(!m_d->m_lstDevices.contains(pDevice))
        m_d->m_lstDevices.append(pDevice);
}

void PeBay::RemoveChildDevice(PeDevice *pDevice)
{
    m_d->m_lstDevices.removeOne(pDevice);
}

QList<PeDevice*> PeBay::GetChildDevices() const
{
    return m_d->m_lstDevices;
}
