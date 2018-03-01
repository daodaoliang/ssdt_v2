#ifndef PECABLE_H
#define PECABLE_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeCablePrivate;
class PeCubicle;
class PeFiber;
class PROJECTEXPLORER_EXPORT PeCable : public PeProjectObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum CableType { ctOptical = 0, ctTail, ctJump};

// Construction and Destruction
public:
    PeCable();
    PeCable(const PeCable &Cable);
    PeCable(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeCable();

// Operations
public:
    static QString      GetCableTypeName(const CableType &eCableType);
    static bool         ComparCableName(PeCable *pCable1, PeCable *pCable2);

    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PeCable&            operator=(const PeCable &Cable);

    // Property operations
    CableType           GetCableType() const;
    void                SetCableType(const CableType &eCableType);
    QString             GetName() const;
    void                SetName(const QString &strName);
    QString             GetNameBay() const;
    void                SetNameBay(const QString &strNameBay);
    int                 GetNameNumber() const;
    void                SetNameNumber(int iNameNumber);
    QString             GetNameSet() const;
    void                SetNameSet(const QString &strNameSet);

    QString             CalculateNameByRule(const QString &strNameRule);

    // Project object operation
    PeCubicle*          GetCubicle1() const;
    void                SetCubicle1(PeCubicle *pCubicle1);
    PeCubicle*          GetCubicle2() const;
    void                SetCubicle2(PeCubicle *pCubicle2);

    void                AddChildFiber(PeFiber *pFiber);
    void                RemoveChildFiber(PeFiber *pFiber);
    QList<PeFiber*>     GetChildFibers() const;

// Properties
private:
    PeCablePrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PECABLE_H
