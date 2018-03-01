#ifndef PEPROJECTOBJECT_H
#define PEPROJECTOBJECT_H

#include <QObject>
#include <QIcon>
#include "projectexplorer_global.h"

namespace ProjectExplorer {

class PeProjectObjectPrivate;
class PeProjectVersion;
class PROJECTEXPLORER_EXPORT PeProjectObject : public QObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum ObjectType { otUnknown = 0, otProject, otProjectVersion, otRoom, otCubicle, otBay, otDevice,
                      otBoard, otPort, otInfoSet, otCubicleConn, otVTerminal, otStrap, otVTerminalConn,
                      otCable, otFiber, otVolume, otDwg };

    const static int m_iInvalidObjectId = 0;

// Construction and Destruction
public:
    PeProjectObject(const PeProjectObject &ProjectObject);
    PeProjectObject(ObjectType eObjectType);
    PeProjectObject(ObjectType eObjectType, PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeProjectObject();

// Operations
public:
    static QIcon        GetObjectIcon(const ObjectType &eObjectType);
    static QString      GetObjectTypeName(const ObjectType &eObjectType);

    static bool         CompareId(PeProjectObject *pProjectObject1, PeProjectObject *pProjectObject2);
    static bool         CompareDisplayName(PeProjectObject *pProjectObject1, PeProjectObject *pProjectObject2);

    // For UI display purpose
    virtual QString     GetDisplayName() const = 0;
    virtual QIcon       GetDisplayIcon() const;
    virtual QString     GetObjectTypeName() const;

    // Operator overload
    PeProjectObject&    operator=(const PeProjectObject &ProjectObject);

    // Property operations
    ObjectType          GetObjectType() const;
    void                SetObjectType(const ObjectType &eObjectType);

    PeProjectVersion*   GetProjectVersion() const;
    void                SetProjectVersion(PeProjectVersion *pProjectVersion);

    int                 GetId() const;
    void                SetId(int iId);

// Properties
private:
    PeProjectObjectPrivate  *m_d;
};

} // namespace ProjectExplorer

#endif // PEPROJECTOBJECT_H
