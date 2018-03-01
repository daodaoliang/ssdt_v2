#include "pevolume.h"
#include "peprojectversion.h"

namespace ProjectExplorer {

class PeVolumePrivate
{
public:
    PeVolumePrivate() : m_strName(""), m_strDescription("") {}

    QString             m_strName;
    QString             m_strDescription;
    QList<PeDwg*>       m_lstDwgs;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeVolume::PeVolume() : PeProjectObject(PeProjectObject::otVolume), m_d(new PeVolumePrivate)
{
}

PeVolume::PeVolume(const PeVolume &Volume) : PeProjectObject(Volume), m_d(new PeVolumePrivate)
{
    *m_d = *Volume.m_d;
}

PeVolume::PeVolume(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otVolume, pProjectVersion, iId), m_d(new PeVolumePrivate)
{
}

PeVolume::~PeVolume()
{
    delete m_d;
}

QString PeVolume::GetDisplayName() const
{
    return GetName();
}

PeVolume& PeVolume::operator=(const PeVolume &Volume)
{
    PeProjectObject::operator =(Volume);
    *m_d = *Volume.m_d;

    return *this;
}

QString PeVolume::GetName() const
{
    return m_d->m_strName;
}

void PeVolume::SetName(const QString strName)
{
    m_d->m_strName = strName;
}

QString PeVolume::GetDescription() const
{
    return m_d->m_strDescription;
}

void PeVolume::SetDescription(const QString &strDescription)
{
    m_d->m_strDescription = strDescription;
}

void PeVolume::AddChildDwg(PeDwg *pDwg)
{
    if(!m_d->m_lstDwgs.contains(pDwg))
        m_d->m_lstDwgs.append(pDwg);
}

void PeVolume::RemoveChildDwg(PeDwg *pDwg)
{
    m_d->m_lstDwgs.removeOne(pDwg);
}

QList<PeDwg*> PeVolume::GetChildDwgs() const
{
    return m_d->m_lstDwgs;
}
