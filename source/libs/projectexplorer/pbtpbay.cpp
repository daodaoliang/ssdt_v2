#include <QMap>

#include "pbtpbay.h"

namespace ProjectExplorer {

class PbTpBayPrivate
{
public:
    PbTpBayPrivate() : m_strName(""), m_iVLevel(0) { }

    QString             m_strName;
    int                 m_iVLevel;
    QList<PbTpDevice*>  m_lstDevices;
    QList<PbTpInfoSet*> m_lstInfoSets;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbTpBay::PbTpBay() : PbBaseObject(PbBaseObject::botTpBay), m_d(new PbTpBayPrivate)
{
}

PbTpBay::PbTpBay(const PbTpBay &Bay) : PbBaseObject(Bay), m_d(new PbTpBayPrivate)
{
    *m_d = *Bay.m_d;
}

PbTpBay::PbTpBay(int iId) : PbBaseObject(PbBaseObject::botTpBay, iId), m_d(new PbTpBayPrivate)
{
}

PbTpBay::~PbTpBay()
{
    delete m_d;
}

QString PbTpBay::GetDisplayName() const
{
    return GetName();
}

PbTpBay& PbTpBay::operator=(const PbTpBay &Bay)
{
    PbBaseObject::operator =(Bay);
    *m_d = *Bay.m_d;

    return *this;
}

QString PbTpBay::GetName() const
{
    return m_d->m_strName;
}

void PbTpBay::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

int PbTpBay::GetVLevel() const
{
    return m_d->m_iVLevel;
}

void PbTpBay::SetVLevel(int iVLevel)
{
    m_d->m_iVLevel = iVLevel;
}

void PbTpBay::AddChildDevice(PbTpDevice *pDevice)
{
    if(!m_d->m_lstDevices.contains(pDevice))
        m_d->m_lstDevices.append(pDevice);
}

void PbTpBay::RemoveChildDevice(PbTpDevice *pDevice)
{
    m_d->m_lstDevices.removeOne(pDevice);
}

QList<PbTpDevice*> PbTpBay::GetChildDevices() const
{
    return m_d->m_lstDevices;
}

void PbTpBay::AddChildInfoSet(PbTpInfoSet *pInfoSet)
{
    if(!m_d->m_lstInfoSets.contains(pInfoSet))
        m_d->m_lstInfoSets.append(pInfoSet);
}

void PbTpBay::RemoveChildInfoSet(PbTpInfoSet *pInfoSet)
{
    m_d->m_lstInfoSets.removeOne(pInfoSet);
}

QList<PbTpInfoSet*> PbTpBay::GetChildInfoSets() const
{
    return m_d->m_lstInfoSets;
}
