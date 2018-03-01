#include "peproject.h"
#include "peprojectversion.h"

namespace ProjectExplorer {

class PeProjectPrivate
{
public:
    PeProjectPrivate() : m_strName(""), m_strDescription("")
    {
    }

    QString                     m_strName;
    QString                     m_strDescription;
    QString                     m_strProvince;
    QString                     m_strCity;
    int                         m_iVLevel;
    QString                     m_strType;
    QString                     m_strNumber;
    QList<PeProjectVersion*>    m_lstProjectVersions;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeProject::PeProject() :
    PeProjectObject(PeProjectObject::otProject, 0, PeProjectObject::m_iInvalidObjectId), m_d(new PeProjectPrivate)
{
}

PeProject::PeProject(const PeProject &ProjectVersion) :
    PeProjectObject(ProjectVersion), m_d(new PeProjectPrivate)
{
    m_d->m_strName = ProjectVersion.m_d->m_strName;
    m_d->m_strDescription = ProjectVersion.m_d->m_strDescription;
    m_d->m_strProvince = ProjectVersion.m_d->m_strProvince;
    m_d->m_strCity = ProjectVersion.m_d->m_strCity;
    m_d->m_iVLevel = ProjectVersion.m_d->m_iVLevel;
    m_d->m_strType = ProjectVersion.m_d->m_strType;
    m_d->m_strNumber = ProjectVersion.m_d->m_strNumber;
}

PeProject::PeProject(int iId) :
    PeProjectObject(PeProjectObject::otProject, 0, iId), m_d(new PeProjectPrivate)
{
}

PeProject::~PeProject()
{
    delete m_d;
}

QString PeProject::GetDisplayName() const
{
    if(!m_d->m_strDescription.isEmpty())
        return m_d->m_strName + " : " + m_d->m_strDescription;

    return m_d->m_strName;
}

PeProject& PeProject::operator=(const PeProject &ProjectVersion)
{
    PeProjectObject::operator =(ProjectVersion);
    *m_d = *ProjectVersion.m_d;

    return *this;
}

QString PeProject::GetName() const
{
    return m_d->m_strName;
}

void PeProject::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

QString PeProject::GetDescription() const
{
    return m_d->m_strDescription;
}

void PeProject::SetDescription(const QString &strDescription)
{
    m_d->m_strDescription = strDescription;
}

QString PeProject::GetProvince() const
{
    return m_d->m_strProvince;
}

void PeProject::SetProvince(const QString &strProvince)
{
    m_d->m_strProvince = strProvince;
}

QString PeProject::GetCity() const
{
    return m_d->m_strCity;
}

void PeProject::SetCity(const QString &strCity)
{
    m_d->m_strCity = strCity;
}

int PeProject::GetVLevel() const
{
    return m_d->m_iVLevel;
}

void PeProject::SetVLevel(int iVLevel)
{
    m_d->m_iVLevel = iVLevel;
}

QString PeProject::GetType() const
{
    return m_d->m_strType;
}

void PeProject::SetType(const QString &strType)
{
    m_d->m_strType = strType;
}

QString PeProject::GetNumber() const
{
    return m_d->m_strNumber;
}

void PeProject::SetNumber(const QString &strNumber)
{
    m_d->m_strNumber = strNumber;
}

void PeProject::AddProjectVersion(PeProjectVersion *pProjectVersion)
{
    if(!m_d->m_lstProjectVersions.contains(pProjectVersion))
        m_d->m_lstProjectVersions.append(pProjectVersion);
}

void PeProject::RemoveProjectVersion(PeProjectVersion *pProjectVersion)
{
    m_d->m_lstProjectVersions.removeOne(pProjectVersion);
}

PeProjectVersion* PeProject::FindProjectVersion(int iProjectVersionId)
{
    foreach(PeProjectVersion *pProjectVersion, m_d->m_lstProjectVersions)
    {
        if(pProjectVersion->GetId() == iProjectVersionId)
            return pProjectVersion;
    }

    return 0;
}

QList<PeProjectVersion*> PeProject::GetAllProjectVersions() const
{
    return m_d->m_lstProjectVersions;
}

PeProjectVersion* PeProject::FindMaxProjectVersion() const
{
    PeProjectVersion *pMaxProjectVersion = 0;

    int iMaxVersionNumber = 0;
    foreach(PeProjectVersion *pProjectVersion, m_d->m_lstProjectVersions)
    {
        if(pProjectVersion->GetVersionNumber() > iMaxVersionNumber)
        {
            iMaxVersionNumber = pProjectVersion->GetVersionNumber();
            pMaxProjectVersion = pProjectVersion;

        }
    }

    return pMaxProjectVersion;
}
