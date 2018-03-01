#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QMap>
#include "projectexplorer_global.h"

namespace MySqlWrapper {

class MySqlDatabase;

} // namespace ProjectExplorer

namespace ProjectExplorer {

class DatabaseManagerPrivate;
class PROJECTEXPLORER_EXPORT DatabaseManager : public QObject
{
    Q_OBJECT

// Construction and Destruction
public:
    DatabaseManager(QObject *pParent = 0);
    virtual ~DatabaseManager();

// Operations
public:
    static DatabaseManager*         Instance();

    bool                            Initialize(const QString &strHost,
                                               const QString &strUser,
                                               const QString &strPassword,
                                               const QString &strBaseDbName,
                                               const QString &strProjectDbName);

    QString                         GetHost() const;
    QString                         GetUser() const;
    QString                         GetPassword() const;
    QString                         GetBaseDbName() const;
    QString                         GetProjectDbName() const;

    MySqlWrapper::MySqlDatabase*    GetBaseDb();
    MySqlWrapper::MySqlDatabase*    GetProjectDb(int iProjectVersionId);

    bool                            CreateProjectDb(int iProjectVersionId);
    bool                            RemoveProjectDb(int iProjectVersionId);
    bool                            CopyProjectDb(int iSrcProjectVersionId, int iDstProjectVersionId);

// Properties
private:
    DatabaseManagerPrivate  *m_d;

};

} // namespace ProjectExplorer

#endif // DATABASEMANAGER_H
