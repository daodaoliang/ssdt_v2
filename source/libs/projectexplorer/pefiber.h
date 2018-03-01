#ifndef PEFIBER_H
#define PEFIBER_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeFiberPrivate;
class PePort;
class PeCable;
class PROJECTEXPLORER_EXPORT PeFiber : public PeProjectObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum FiberColor { fcBlue = 0, fcOrange, fcGreen, fcBrown, fcGray, fcOrigin, fcRed,
                      rcBlack, fcYellow, fcViolet, fcPink, fcTurquoise, fcInvalid};

// Construction and Destruction
public:
    PeFiber();
    PeFiber(const PeFiber &Fiber);
    PeFiber(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeFiber();

// Operations
public:
    static QString      GetFiberColorName(const FiberColor &eFiberColor);
    static QColor       GetFiberColor(const FiberColor &eFiberColor);

    static bool         CompareIndex(PeFiber *pFiber1, PeFiber *pFiber2);

    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PeFiber&            operator=(const PeFiber &Fiber);

    // Property operations
    int                 GetIndex() const;
    void                SetIndex(int iIndex);
    FiberColor          GetFiberColor() const;
    void                SetFiberColor(const FiberColor &eFiberColor);
    FiberColor          GetPipeColor() const;
    void                SetPipeColor(const FiberColor &ePipeColor);
    bool                GetReserve() const;
    void                SetReserve(bool bReserve);
    QStringList         GetInfoSetIds() const;
    void                SetInfoSetIds(const QStringList &lstInfoSetIds);

    // Project object operation
    PePort*             GetPort1() const;
    void                SetPort1(PePort *pPort1);
    PePort*             GetPort2() const;
    void                SetPort2(PePort *pPort2);
    PeCable*            GetParentCable() const;
    void                SetParentCable(PeCable *pCable);

    void                AddInfoSetIds(const QStringList &lstInfoSetIds);
    void                RemoveInfoSetIds(const QStringList &lstInfoSetIds);

// Properties
private:
    PeFiberPrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PEFIBER_H
