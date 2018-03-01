#ifndef PEBOARD_H
#define PEBOARD_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeBoardPrivate;
class PeDevice;
class PePort;
class PeFiber;
class PROJECTEXPLORER_EXPORT PeBoard : public PeProjectObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PeBoard();
    PeBoard(const PeBoard &Board);
    PeBoard(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeBoard();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString GetDisplayName() const;

    // Operator overload
    PeBoard&        operator=(const PeBoard &Board);

    // Property operations
    QString         GetPosition() const;
    void            SetPosition(const QString &strPosition);
    QString         GetDescription() const;
    void            SetDescription(const QString &strDescription);
    QString         GetType() const;
    void            SetType(const QString &strType);

    // Project object operation
    PeDevice*       GetParentDevice() const;
    void            SetParentDevice(PeDevice *pDevice);

    void            AddChildPort(PePort *pPort);
    void            RemoveChildPort(PePort *pPort);
    QList<PePort*>  GetChildPorts() const;

    bool            IsBoardUsed(const QList<PeFiber*> lstUnusedFibers = QList<PeFiber*>()) const;

// Properties
private:
    PeBoardPrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PEBOARD_H
