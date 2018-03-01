#include <QApplication>
#include <QDateTime>
#include <QIcon>
#include <QProcess>
#include <QDir>

#include "mysqlwrapper/mysqldatabase.h"
#include "mysqlwrapper/mysqlquery.h"
#include "mysqlwrapper/mysqltransaction.h"

#include "projectmanager.h"
#include "databasemanager.h"
#include "peproject.h"
#include "peprojectversion.h"

namespace ProjectExplorer {

class ProjectManagerPrivate
{
public:
    ProjectManagerPrivate()
    {
    }

    static ProjectManager           *m_pInstance;

    QMap<int, PeProject*>           m_mapIdToProjects;
    QMap<int, PeProjectVersion*>    m_mapIdToProjectVersions;
};

ProjectManager *ProjectManagerPrivate::m_pInstance = 0;

} // namespace ProjectExplorer

using namespace ProjectExplorer;

ProjectManager::ProjectManager(QObject *pParent) : QObject(pParent), m_d(new ProjectManagerPrivate)
{
    ProjectManagerPrivate::m_pInstance = this;
}

ProjectManager::~ProjectManager()
{
    delete m_d;
    ProjectManagerPrivate::m_pInstance = 0;
}

ProjectManager* ProjectManager::Instance()
{
    return ProjectManagerPrivate::m_pInstance;
}

bool ProjectManager::Initialize()
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    MySqlWrapper::MySqlQuery query(pDatabase);

    // Read Project
    query.Clear();
    if(!query.Exec("SELECT * FROM project"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PeProject *pProject = new PeProject(record.value("project_id").toInt());
        pProject->SetName(record.value("name").toString());
        pProject->SetDescription(record.value("description").toString());
        pProject->SetProvince(record.value("province").toString());
        pProject->SetCity(record.value("city").toString());
        pProject->SetVLevel(record.value("vlevel").toInt());
        pProject->SetType(record.value("type").toString());
        pProject->SetNumber(record.value("number").toString());
        m_d->m_mapIdToProjects.insert(pProject->GetId(), pProject);
    }

    // Read Projec Version
    query.Clear();
    if(!query.Exec("SELECT * FROM project_version"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        if(PeProject *pProject = FindProjectById(record.value("project_id").toInt()))
        {
            PeProjectVersion *pProjectVersion = new PeProjectVersion(pProject, record.value("version_id").toInt());
            pProjectVersion->SetVersionNumber(record.value("version_number").toInt());
            pProjectVersion->SetStage(record.value("stage").toString());
            pProjectVersion->SetModifier(record.value("modifier").toString());
            pProjectVersion->SetModificationTime(record.value("modification_time").toDateTime());
            pProjectVersion->SetWhat(record.value("what").toString());
            pProjectVersion->SetWhy(record.value("why").toString());
            pProjectVersion->SetVersionPwd(record.value("version_pwd").toString());

            pProject->AddProjectVersion(pProjectVersion);
            m_d->m_mapIdToProjectVersions.insert(pProjectVersion->GetId(), pProjectVersion);
        }
    }

    emit sigInitialized();

    return true;
}

void ProjectManager::Uninitialize()
{
    foreach(PeProjectVersion *pProjectVersion, m_d->m_mapIdToProjectVersions)
        pProjectVersion->Close();

    qDeleteAll(m_d->m_mapIdToProjectVersions);
    m_d->m_mapIdToProjectVersions.clear();

    qDeleteAll(m_d->m_mapIdToProjects);
    m_d->m_mapIdToProjects.clear();

    emit sigUninitialized();
}

bool ProjectManager::Reinitialize()
{
    Uninitialize();
    return Initialize();
}

PeProject* ProjectManager::CreateProject(const PeProject &Project, bool bNotify)
{
    PeProject *pProject = new PeProject(Project);
    m_d->m_mapIdToProjects.insert(pProject->GetId(), pProject);

    if(bNotify)
        emit sigProjectCreated(pProject);

    return pProject;
}

PeProject* ProjectManager::UpdateProject(const PeProject &Project, bool bNotify)
{
    PeProject *pProject = FindProjectById(Project.GetId());
    if(!pProject)
        return 0;

    if(pProject->GetName() != Project.GetName() ||
       pProject->GetDescription() != Project.GetDescription() ||
       pProject->GetProvince() != Project.GetProvince() ||
       pProject->GetCity() != Project.GetCity() ||
       pProject->GetVLevel() != Project.GetVLevel() ||
       pProject->GetType() != Project.GetType() ||
       pProject->GetNumber() != Project.GetNumber())
    {
        pProject->SetName(Project.GetName());
        pProject->SetDescription(Project.GetDescription());
        pProject->SetProvince(Project.GetProvince());
        pProject->SetCity(Project.GetCity());
        pProject->SetVLevel(Project.GetVLevel());
        pProject->SetType(Project.GetType());
        pProject->SetNumber(Project.GetNumber());

        if(bNotify)
            emit sigProjectPropertyChanged(pProject);
    }

    return pProject;
}

bool ProjectManager::DeleteProject(PeProject *pProject, bool bNotify)
{
    if(!pProject)
        return false;

    /////////////////////////////////////////////////////////////////
    // Delete child project versions
    /////////////////////////////////////////////////////////////////
    foreach(PeProjectVersion *pProjectVersion, pProject->GetAllProjectVersions())
        DeleteProjectVersion(pProjectVersion, bNotify);

    /////////////////////////////////////////////////////////////////
    // Delete current project
    /////////////////////////////////////////////////////////////////
    if(bNotify)
        emit sigProjectAboutToBeDeleted(pProject);

    m_d->m_mapIdToProjects.remove(pProject->GetId());
    delete pProject;

    if(bNotify)
        emit sigProjectDeleted();

    return true;
}

PeProjectVersion* ProjectManager::CreateProjectVersion(const PeProjectVersion &ProjectVersion, bool bNotify)
{
    PeProject *pProject = ProjectVersion.GetProject();
    if(!pProject)
        return 0;

    PeProjectVersion *pProjectVersion = new PeProjectVersion(ProjectVersion);
    pProject->AddProjectVersion(pProjectVersion);
    m_d->m_mapIdToProjectVersions.insert(pProjectVersion->GetId(), pProjectVersion);

    if(bNotify)
        emit sigProjectVersionCreated(pProjectVersion);

    return pProjectVersion;
}

PeProjectVersion* ProjectManager::UpdateProjectVersion(const PeProjectVersion &ProjectVersion, bool bNotify)
{
    PeProjectVersion *pProjectVersion = FindProjectVersionById(ProjectVersion.GetId());
    if(!pProjectVersion)
        return 0;

    if(pProjectVersion->GetVersionNumber() != ProjectVersion.GetVersionNumber() ||
       pProjectVersion->GetStage() != ProjectVersion.GetStage() ||
       pProjectVersion->GetModifier() != ProjectVersion.GetModifier() ||
       pProjectVersion->GetModificationTime() != ProjectVersion.GetModificationTime() ||
       pProjectVersion->GetWhat() != ProjectVersion.GetWhat() ||
       pProjectVersion->GetWhy() != ProjectVersion.GetWhy())
    {
        pProjectVersion->SetVersionNumber(ProjectVersion.GetVersionNumber());
        pProjectVersion->SetStage(ProjectVersion.GetStage());
        pProjectVersion->SetModifier(ProjectVersion.GetModifier());
        pProjectVersion->SetModificationTime(ProjectVersion.GetModificationTime());
        pProjectVersion->SetWhat(ProjectVersion.GetWhat());
        pProjectVersion->SetWhy(ProjectVersion.GetWhy());
        pProjectVersion->SetVersionPwd(ProjectVersion.GetVersionPwd());

        if(bNotify)
            emit sigProjectVersionPropertyChanged(pProjectVersion);
    }

    return pProjectVersion;
}

bool ProjectManager::DeleteProjectVersion(PeProjectVersion *pProjectVersion, bool bNotify)
{
    if(!pProjectVersion)
        return false;

    pProjectVersion->Close();

    if(bNotify)
        emit sigProjectVersionAboutToBeDeleted(pProjectVersion);

    if(PeProject *pProject = pProjectVersion->GetProject())
        pProject->RemoveProjectVersion(pProjectVersion);

    m_d->m_mapIdToProjectVersions.remove(pProjectVersion->GetId());
    delete pProjectVersion;

    if(bNotify)
        emit sigProjectVersionDeleted();

    return true;
}

QList<PeProject*> ProjectManager::GetAllProjects() const
{
    return m_d->m_mapIdToProjects.values();
}

QList<PeProjectVersion*> ProjectManager::GetAllProjectVersions() const
{
    return m_d->m_mapIdToProjectVersions.values();
}

PeProject* ProjectManager::FindProjectById(int iId) const
{
    return m_d->m_mapIdToProjects.value(iId, 0);
}

PeProjectVersion* ProjectManager::FindProjectVersionById(int iId) const
{
    return m_d->m_mapIdToProjectVersions.value(iId, 0);
}

bool ProjectManager::DbCreateProject(PeProject &Project)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    if(!pDatabase->ExecSql(QString("INSERT INTO project(`name`,`description`,`province`,`city`,`vlevel`,`type`,`number`) "
                                   "VALUES('%1','%2','%3','%4','%5','%6','%7')").arg(Project.GetName())
                                                                                .arg(Project.GetDescription())
                                                                                .arg(Project.GetProvince())
                                                                                .arg(Project.GetCity())
                                                                                .arg(Project.GetVLevel())
                                                                                .arg(Project.GetType())
                                                                                .arg(Project.GetNumber())))
    {
        return false;
    }

    int iReturnId = pDatabase->GetInsertId();
    if(iReturnId <= PeProjectObject::m_iInvalidObjectId)
        return false;

    Project.SetId(iReturnId);
    return true;
}

bool ProjectManager::DbUpdateProject(const PeProject &Project)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    return pDatabase->ExecSql(QString("UPDATE project SET `name`='%1',`description`='%2',`province`='%3',`city`='%4',`vlevel`='%5',`type`='%6',`number`='%7' WHERE `project_id`='%8'")
                                                                                .arg(Project.GetName())
                                                                                .arg(Project.GetDescription())
                                                                                .arg(Project.GetProvince())
                                                                                .arg(Project.GetCity())
                                                                                .arg(Project.GetVLevel())
                                                                                .arg(Project.GetType())
                                                                                .arg(Project.GetNumber())
                                                                                .arg(Project.GetId()));
}

bool ProjectManager::DbDeleteProject(int iId, bool bUseTransaction)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    MySqlWrapper::MySqlTransaction transaction(pDatabase, bUseTransaction);
    MySqlWrapper::MySqlQuery query(pDatabase);

    /////////////////////////////////////////////////////////////////
    // Delete child project versions
    /////////////////////////////////////////////////////////////////
    query.Clear();
    if(!query.Exec(QString("SELECT `version_id` FROM `project_version` WHERE `project_id`='%1'").arg(iId)))
    {
        transaction.Rollback();
        return false;
    }

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        if(!DbDeleteProjectVersion(record.value("version_id").toInt(), false))
        {
            transaction.Rollback();
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////
    // Delete current project
    /////////////////////////////////////////////////////////////////
    query.Clear();
    if(!query.Exec(QString("DELETE FROM `project` WHERE `project_id`='%1'").arg(iId)))
    {
        transaction.Rollback();
        return false;
    }

    transaction.Commit();
    return true;
}

bool ProjectManager::DbCreateProjectVersion(PeProjectVersion &ProjectVersion, PeProjectVersion *pProjectVersionBase)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    MySqlWrapper::MySqlTransaction transaction(pDatabase, true);

    if(!pDatabase->ExecSql(QString("INSERT INTO project_version(`project_id`,`version_number`,`stage`,`modifier`,`modification_time`,`what`,`why`) "
                                   "VALUES('%1','%2','%3','%4',NOW(),'%5','%6')").arg(ProjectVersion.GetProject()->GetId())
                                                                                 .arg(ProjectVersion.GetVersionNumber())
                                                                                 .arg(ProjectVersion.GetStage())
                                                                                 .arg(ProjectVersion.GetModifier())
                                                                                 .arg(ProjectVersion.GetWhat())
                                                                                 .arg(ProjectVersion.GetWhy())))
    {
        transaction.Rollback();
        return false;
    }

    int iReturnId = pDatabase->GetInsertId();
    if(iReturnId <= PeProjectObject::m_iInvalidObjectId)
    {
        transaction.Rollback();
        return false;
    }

    if(!DatabaseManager::Instance()->CreateProjectDb(iReturnId))
    {
        transaction.Rollback();
        return false;
    }

    if(!DatabaseManager::Instance()->CopyProjectDb(pProjectVersionBase->GetId(), iReturnId))
    {
        transaction.Rollback();
        return false;
    }

    transaction.Commit();

    ProjectVersion.SetId(iReturnId);
    return true;
}

bool ProjectManager::DbUpdateProjectVersion(const PeProjectVersion &ProjectVersion)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    return pDatabase->ExecSql(QString("UPDATE project_version SET `project_id`='%1',`version_number`='%2',`stage`='%3',`modifier`='%4',`what`='%5',`why`='%6' WHERE `version_id`='%7'")
                                                                                    .arg(ProjectVersion.GetProject()->GetId())
                                                                                    .arg(ProjectVersion.GetVersionNumber())
                                                                                    .arg(ProjectVersion.GetStage())
                                                                                    .arg(ProjectVersion.GetModifier())
                                                                                    .arg(ProjectVersion.GetWhat())
                                                                                    .arg(ProjectVersion.GetWhy())
                                                                                    .arg(ProjectVersion.GetId()));
}

bool ProjectManager::DbDeleteProjectVersion(int iId, bool bUseTransaction)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    MySqlWrapper::MySqlTransaction transaction(pDatabase, bUseTransaction);

    if(!pDatabase->ExecSql(QString("DELETE FROM `project_version` WHERE `version_id`='%1'").arg(iId)))
    {
        transaction.Rollback();
        return false;
    }

    if(!DatabaseManager::Instance()->RemoveProjectDb(iId))
    {
        transaction.Rollback();
        return false;
    }

    transaction.Commit();
    return true;
}
