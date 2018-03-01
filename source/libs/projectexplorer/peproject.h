#ifndef PEPROJECT_H
#define PEPROJECT_H

#include "peprojectobject.h"

QT_BEGIN_NAMESPACE
class QDateTime;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeProjectPrivate;
class PeProject;
class PROJECTEXPLORER_EXPORT PeProject : public PeProjectObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PeProject();
    PeProject(const PeProject &Project);
    PeProject(int iId);
    virtual ~PeProject();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString             GetDisplayName() const;

    // Operator overload
    PeProject&                  operator=(const PeProject &Project);

    // Property operations
    QString                     GetName() const;
    void                        SetName(const QString &strName);
    QString                     GetDescription() const;
    void                        SetDescription(const QString &strDescription);
    QString                     GetProvince() const;
    void                        SetProvince(const QString &strProvince);
    QString                     GetCity() const;
    void                        SetCity(const QString &strCity);
    int                         GetVLevel() const;
    void                        SetVLevel(int iVLevel);
    QString                     GetType() const;
    void                        SetType(const QString &strType);
    QString                     GetNumber() const;
    void                        SetNumber(const QString &strNumber);

    // Project version operations
    void                        AddProjectVersion(PeProjectVersion *pProjectVersion);
    void                        RemoveProjectVersion(PeProjectVersion *pProjectVersion);
    PeProjectVersion*           FindProjectVersion(int iProjectVersionId);
    QList<PeProjectVersion*>    GetAllProjectVersions() const;
    PeProjectVersion*           FindMaxProjectVersion() const;

// Properties
private:
    PeProjectPrivate    *m_d;
};

} // namespace ProjectExplorer

#endif // PEPROJECT_H
