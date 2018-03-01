#ifndef PEVTERMINALCONN_H
#define PEVTERMINALCONN_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeVTerminalConnPrivate;
class PeDevice;
class PeVTerminal;
class PeStrap;
class PROJECTEXPLORER_EXPORT PeVTerminalConn : public PeProjectObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PeVTerminalConn();
    PeVTerminalConn(const PeVTerminalConn &VTerminalConn);
    PeVTerminalConn(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeVTerminalConn();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PeVTerminalConn&    operator=(const PeVTerminalConn &VTerminalConn);

    // Project object operation
    PeVTerminal*        GetTxVTerminal() const;
    void                SetTxVTerminal(PeVTerminal *pTxVTerminal);
    PeVTerminal*        GetRxVTerminal() const;
    void                SetRxVTerminal(PeVTerminal *pRxVTerminal);
    PeStrap*            GetTxStrap() const;
    void                SetTxStrap(PeStrap *pTxStrap);
    PeStrap*            GetRxStrap() const;
    void                SetRxStrap(PeStrap *pRxStrap);
    bool                GetStraight() const;
    void                SetStraight(bool bStraight);

// Properties
private:
    PeVTerminalConnPrivate    *m_d;
};

} // namespace ProjectExplorer

#endif // PEVTERMINALCONN_H
