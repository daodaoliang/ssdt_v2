#ifndef PBBASEOBJECT_H
#define PBBASEOBJECT_H

#include <QObject>
#include <QIcon>
#include "projectexplorer_global.h"

namespace ProjectExplorer {

class PbBaseObjectPrivate;
class PROJECTEXPLORER_EXPORT PbBaseObject : public QObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum ObjectType { botUnknown = 0, botDeviceCategory, botBayCategory, botVLevel,
                      botManufacture, botDeviceType, botLibDevice, botLibBoard, botLibPort,
                      botTpBay, botTpDevice, botTpPort, botTpInfoSet };

    const static int m_iInvalidObjectId = 0;

// Construction and Destruction
public:
    PbBaseObject(const PbBaseObject &BaseObject);
    PbBaseObject(ObjectType eObjectType);
    PbBaseObject(ObjectType eObjectType, int iId);
    virtual ~PbBaseObject();

// Operations
public:
    static QIcon        GetObjectIcon(const ObjectType &eObjectType);
    static QString      GetObjectTypeName(const ObjectType &eObjectType);

    static bool         CompareId(PbBaseObject *pBaseObject1, PbBaseObject *pBaseObject2);
    static bool         CompareDisplayName(PbBaseObject *pBaseObject1, PbBaseObject *pBaseObject2);

    // For UI display purpose
    virtual QString     GetDisplayName() const = 0;
    virtual QIcon       GetDisplayIcon() const;
    virtual QString     GetObjectTypeName() const;

    // Operator overload
    PbBaseObject&       operator=(const PbBaseObject &BaseObject);

    // Property operations
    ObjectType          GetObjectType() const;
    void                SetObjectType(const ObjectType &eObjectType);

    int                 GetId() const;
    void                SetId(int iId);

// Properties
private:
    PbBaseObjectPrivate  *m_d;
};

} // namespace ProjectExplorer

#endif // PEPROJECTOBJECT_H
