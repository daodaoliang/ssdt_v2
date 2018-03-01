#ifndef PBLIBBOARD_H
#define PBLIBBOARD_H

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbLibBoardPrivate;
class PbLibDevice;
class PbLibPort;
class PROJECTEXPLORER_EXPORT PbLibBoard : public PbBaseObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PbLibBoard();
    PbLibBoard(const PbLibBoard &Board);
    PbLibBoard(int iId);
    virtual ~PbLibBoard();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PbLibBoard&         operator=(const PbLibBoard &Board);

    // Property operations
    QString             GetPosition() const;
    void                SetPosition(const QString &strPosition);
    QString             GetDescription() const;
    void                SetDescription(const QString &strDescription);
    QString             GetType() const;
    void                SetType(const QString &strType);

    // Project object operation
    PbLibDevice*        GetParentDevice() const;
    void                SetParentDevice(PbLibDevice *pDevice);

    void                AddChildPort(PbLibPort *pPort);
    void                RemoveChildPort(PbLibPort *pPort);
    QList<PbLibPort*>   GetChildPorts() const;

// Properties
private:
    PbLibBoardPrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PBLIBBOARD_H
