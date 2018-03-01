#ifndef PBLIBPORT_H
#define PBLIBPORT_H

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbLibPortPrivate;
class PbLibDevice;
class PbLibBoard;
class PROJECTEXPLORER_EXPORT PbLibPort : public PbBaseObject
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
    PbLibPort();
    PbLibPort(const PbLibPort &Port);
    PbLibPort(int iId);
    virtual ~PbLibPort();

// Operations
public:
    static QString  GetPortTypeName(const PortType &ePortType);
    static QString  GetPortDirectionName(const PortDirection &ePortDirection);
    static QString  GetFiberPlugName(const FiberPlug &eFiberPlug);
    static QString  GetFiberModeName(const FiberMode &eFiberMode);

    static bool     CompareGroup(PbLibPort *pPort1, PbLibPort *pPort2);

    // Derived from PeProjectObject
    virtual QString GetDisplayName() const;

    // Operator overload
    PbLibPort&      operator=(const PbLibPort &Port);

    // Property operations
    QString         GetName() const;
    void            SetName(const QString &strName);
    int             GetGroup() const;
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
    PbLibBoard*     GetParentBoard() const;
    void            SetParentBoard(PbLibBoard *pBoard);

    PbLibDevice*    GetParentDevice() const;
    PbLibPort*      GetPairPort();

// Properties
private:
    PbLibPortPrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PBLIBPORT_H
