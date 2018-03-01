#include <QProcess>
#include <QDir>

#include "mysqlwrapper/mysqldatabase.h"
#include "mysqlwrapper/mysqlquery.h"
#include "mysqlwrapper/mysqltransaction.h"

#include "databasemanager.h"
#include "projectmanager.h"
#include "basemanager.h"

namespace ProjectExplorer {

class DatabaseManagerPrivate
{
public:
    DatabaseManagerPrivate()
    {
    }

    static DatabaseManager  *m_pInstance;

    QString                  m_strHost;
    QString                  m_strUser;
    QString                  m_strPassword;
    QString                  m_strBaseDbName;
    QString                  m_strProjectDbName;
    QString                  m_strProjectSqlFileName;

    BaseManager             *m_pBaseManager;
    ProjectManager          *m_pProjectManager;
};

DatabaseManager *DatabaseManagerPrivate::m_pInstance = 0;

} // namespace ProjectExplorer

using namespace ProjectExplorer;

DatabaseManager::DatabaseManager(QObject *pParent) : QObject(pParent), m_d(new DatabaseManagerPrivate)
{
    DatabaseManagerPrivate::m_pInstance = this;

    m_d->m_pProjectManager = new ProjectManager(this);
    m_d->m_pBaseManager = new BaseManager(this);
}

DatabaseManager::~DatabaseManager()
{
    m_d->m_pBaseManager->Uninitialize();
    m_d->m_pProjectManager->Uninitialize();

    delete m_d->m_pProjectManager;
    m_d->m_pProjectManager = 0;

    delete m_d->m_pBaseManager;
    m_d->m_pBaseManager = 0;

    delete m_d;
    m_d = 0;

    DatabaseManagerPrivate::m_pInstance = 0;
    MySqlWrapper::MySqlDatabase::ClearDatabase();
}

DatabaseManager* DatabaseManager::Instance()
{
    return DatabaseManagerPrivate::m_pInstance;
}

bool DatabaseManager::Initialize(const QString &strHost,
                                 const QString &strUser,
                                 const QString &strPassword,
                                 const QString &strBaseDbName,
                                 const QString &strProjectDbName)
{
    if(strBaseDbName.isEmpty() || strBaseDbName.isEmpty())
        return false;

    m_d->m_strHost = strHost;
    m_d->m_strUser = strUser;
    m_d->m_strPassword = strPassword;
    m_d->m_strBaseDbName = strBaseDbName;
    m_d->m_strProjectDbName = strProjectDbName;

    return (m_d->m_pBaseManager->Initialize() && m_d->m_pProjectManager->Initialize());
}

QString DatabaseManager::GetHost() const
{
    return m_d->m_strHost;
}

QString DatabaseManager::GetUser() const
{
    return m_d->m_strUser;
}

QString DatabaseManager::GetPassword() const
{
    return m_d->m_strPassword;
}

QString DatabaseManager::GetBaseDbName() const
{
    return m_d->m_strBaseDbName;
}

QString DatabaseManager::GetProjectDbName() const
{
    return m_d->m_strProjectDbName;
}

MySqlWrapper::MySqlDatabase* DatabaseManager::GetBaseDb()
{
    MySqlWrapper::MySqlDatabase *pDatabase = MySqlWrapper::MySqlDatabase::AddDatabase(m_d->m_strHost,
                                                                                      m_d->m_strUser,
                                                                                      m_d->m_strPassword,
                                                                                      m_d->m_strBaseDbName);
    if(pDatabase->IsOpen())
        return pDatabase;

    if(pDatabase->Open())
        return pDatabase;

    return 0;
}

MySqlWrapper::MySqlDatabase* DatabaseManager::GetProjectDb(int iProjectVersionId)
{
    const QString strProjectDbName = QString("%1%2").arg(m_d->m_strProjectDbName).arg(iProjectVersionId);
    MySqlWrapper::MySqlDatabase *pDatabase = MySqlWrapper::MySqlDatabase::AddDatabase(m_d->m_strHost,
                                                                                      m_d->m_strUser,
                                                                                      m_d->m_strPassword,
                                                                                      strProjectDbName);
    if(pDatabase->IsOpen())
        return pDatabase;

    if(pDatabase->Open())
        return pDatabase;

    return 0;
}

bool DatabaseManager::CreateProjectDb(int iProjectVersionId)
{
    MySqlWrapper::MySqlDatabase *pDatabase = GetBaseDb();
    if(!pDatabase)
        return false;

    const QString &strProjectDbNmae = QString("%1%2").arg(m_d->m_strProjectDbName).arg(iProjectVersionId);

    QFile file(":/projectexplorer/sql/create_project.sql");
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QString strContent = file.readAll();
    strContent.replace("$$DatabaseName$$", strProjectDbNmae);

    return pDatabase->ExecSql(strContent);
}

bool DatabaseManager::RemoveProjectDb(int iProjectVersionId)
{
    MySqlWrapper::MySqlDatabase *pDatabase = GetBaseDb();
    if(!pDatabase)
        return false;

    const QString &strProjectDbNmae = QString("%1%2").arg(m_d->m_strProjectDbName).arg(iProjectVersionId);
    return pDatabase->DropDatabase(strProjectDbNmae);
}

bool DatabaseManager::CopyProjectDb(int iSrcProjectVersionId, int iDstProjectVersionId)
{
    MySqlWrapper::MySqlDatabase *pDatabase = GetBaseDb();
    if(!pDatabase)
        return false;

    const QString &strSrcProjectDbNmae = QString("%1%2").arg(m_d->m_strProjectDbName).arg(iSrcProjectVersionId);
    const QString &strDstProjectDbNmae = QString("%1%2").arg(m_d->m_strProjectDbName).arg(iDstProjectVersionId);

    QFile file(":/projectexplorer/sql/copy_project.sql");
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QString strContent = file.readAll();
    strContent.replace("$$SrcDatabaseName$$", strSrcProjectDbNmae);
    strContent.replace("$$DstDatabaseName$$", strDstProjectDbNmae);

    return pDatabase->ExecSql(strContent);
}
