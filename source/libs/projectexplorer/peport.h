#ifndef PEPORT_H
#define PEPORT_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PePortPrivate;
class PeDevice;
class PeBoard;
class PeFiber;
class PROJECTEXPLORER_EXPORT PePort : public PeProjectObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum PortType { ptFiber = 0, ptRJ45, ptOdf };
    enum PortDirection { pdRx = 0, pdTx, pdRT };
    enum FiberPlug { fpNone = 0, fpST, fpLC, fpSC, fpFC };
    enum FiberMode { fmNone = 0, fmSingle, fmMultiple };

// Construction and Destruction
public:
    PePort();
    PePort(const PePort &Port);
    PePort(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PePort();

// Operations
public:
    static QString  GetPortTypeName(const PortType &ePortType);
    static QString  GetPortDirectionName(const PortDirection &ePortDirection);
    static QString  GetFiberPlugName(const FiberPlug &eFiberPlug);
    static QString  GetFiberModeName(const FiberMode &eFiberMode);

    static bool     CompareGroup(PePort *pPort1, PePort *pPort2);

    // Derived from PeProjectObject
    virtual QString GetDisplayName() const;

    // Operator overload
    PePort&         operator=(const PePort &Port);

    // Property operations
    QString         GetName() const;
    void            SetName(const QString &strName);
    QString         GetFullDisplayName() const;
    int             GetGroup() const;
    QString         GetGroupCh() const;
    void            SetGroup(int iGroup);
    PortType        GetPortType() const;
    void            SetPortType(const PortType &ePortType);
    PortDirection   GetPortDirection() const;
    void            SetPortDirection(const PortDirection &ePortDirection);
    FiberPlug       GetFiberPlug() const;
    void            SetFiberPlug(const FiberPlug &eFiberPlug);
    FiberMode       GetFiberMode() const;
    void            SetFiberMode(const FiberMode &eFiberMode);
    bool            GetCascade() const;
    void            SetCascade(bool bCascade);

    // Project object operation
    PeBoard*        GetParentBoard() const;
    void            SetParentBoard(PeBoard *pBoard);

    PeDevice*       GetParentDevice() const;
    PePort*         GetPairPort();

    QList<PeFiber*> GetUsedFibers(const QList<PeFiber*> lstUnusedFibers = QList<PeFiber*>()) const;

// Properties
private:
    PePortPrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PEPORT_H
