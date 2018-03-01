#ifndef PEROOM_H
#define PEROOM_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeRoomPrivate;
class PeCubicle;
class PROJECTEXPLORER_EXPORT PeRoom : public PeProjectObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PeRoom();
    PeRoom(const PeRoom &Room);
    PeRoom(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeRoom();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PeRoom&             operator=(const PeRoom &Room);

    // Property operations
    QString             GetName() const;
    void                SetName(const QString &strName);
    QString             GetNumber() const;
    void                SetNumber(const QString &strNumber);
    bool                GetYard() const;
    void                SetYard(bool bYard);

    // Project object operation
    void                AddChildCubicle(PeCubicle *pCubicle);
    void                RemoveChildCubicle(PeCubicle *pCubicle);
    QList<PeCubicle*>   GetChildCubicles() const;

// Properties
private:
    PeRoomPrivate   *m_d;
};

} // namespace ProjectExplorer

#endif // PEROOM_H
