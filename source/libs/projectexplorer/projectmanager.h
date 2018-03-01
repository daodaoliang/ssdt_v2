#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <QMap>
#include "projectexplorer_global.h"

namespace MySqlWrapper {

class MySqlDatabase;

} // namespace ProjectExplorer

namespace ProjectExplorer {

class ProjectManagerPrivate;
class PeProject;
class PeProjectVersion;
class PROJECTEXPLORER_EXPORT ProjectManager : public QObject
{
    Q_OBJECT

// Construction and Destruction
public:
    ProjectManager(QObject *pParent = 0);
    virtual ~ProjectManager();

// Operations
public:
    static                      ProjectManager* Instance();

    bool                        Initialize();
    void                        Uninitialize();
    bool                        Reinitialize();

    // Object operation
    PeProject*                  CreateProject(const PeProject &Project, bool bNotify = true);
    PeProject*                  UpdateProject(const PeProject &Project, bool bNotify = true);
    bool                        DeleteProject(PeProject *pProject, bool bNotify = true);
    PeProjectVersion*           CreateProjectVersion(const PeProjectVersion &ProjectVersion, bool bNotify = true);
    PeProjectVersion*           UpdateProjectVersion(const PeProjectVersion &ProjectVersion, bool bNotify = true);
    bool                        DeleteProjectVersion(PeProjectVersion *pProjectVersion, bool bNotify = true);

    QList<PeProject*>           GetAllProjects() const;
    QList<PeProjectVersion*>    GetAllProjectVersions() const;

    PeProject*                  FindProjectById(int iProjectId) const;
    PeProjectVersion*           FindProjectVersionById(int iProjectVersionId) const;

    // DB Operation
    bool                        DbCreateProject(PeProject &Project);
    bool                        DbUpdateProject(const PeProject &Project);
    bool                        DbDeleteProject(int iId, bool bUseTransaction = true);
    bool                        DbCreateProjectVersion(PeProjectVersion &ProjectVersion, PeProjectVersion *pProjectVersionBase);
    bool                        DbUpdateProjectVersion(const PeProjectVersion &ProjectVersion);
    bool                        DbDeleteProjectVersion(int iId, bool bUseTransaction = true);

// Properties
private:
    ProjectManagerPrivate *m_d;

signals:
    void    sigInitialized();
    void    sigUninitialized();
    void    sigProjectCreated(ProjectExplorer::PeProject *pProject);
    void    sigProjectPropertyChanged(ProjectExplorer::PeProject *pProject);
    void    sigProjectAboutToBeDeleted(ProjectExplorer::PeProject *pProject);
    void    sigProjectDeleted();
    void    sigProjectVersionCreated(ProjectExplorer::PeProjectVersion *ProjectVersion);
    void    sigProjectVersionPropertyChanged(ProjectExplorer::PeProjectVersion *ProjectVersion);
    void    sigProjectVersionAboutToBeDeleted(ProjectExplorer::PeProjectVersion *ProjectVersion);
    void    sigProjectVersionDeleted();
    void    sigProjectVersionOpened(ProjectExplorer::PeProjectVersion *ProjectVersion);
    void    sigProjectVersionAboutToBeClosed(ProjectExplorer::PeProjectVersion *ProjectVersion);
    void    sigProjectVersionClosed(ProjectExplorer::PeProjectVersion *ProjectVersion);

    friend class PeProject;
    friend class PeProjectVersion;
};

} // namespace ProjectExplorer

#endif // PROJECTMANAGER_H
