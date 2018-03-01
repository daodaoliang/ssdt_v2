#include <QMap>

#include "pevterminalconn.h"
#include "peprojectversion.h"
#include "pedevice.h"
#include "pevterminal.h"
#include "pestrap.h"

namespace ProjectExplorer {

class PeVTerminalConnPrivate
{
public:
    PeVTerminalConnPrivate() : m_pTxVTerminal(0), m_pRxVTerminal(0), m_pTxStrap(0), m_pRxStrap(0), m_bStraight(false)
    {
    }

    PeVTerminal *m_pTxVTerminal;
    PeVTerminal *m_pRxVTerminal;
    PeStrap     *m_pTxStrap;
    PeStrap     *m_pRxStrap;
    bool        m_bStraight;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeVTerminalConn::PeVTerminalConn() : PeProjectObject(PeProjectObject::otVTerminalConn), m_d(new PeVTerminalConnPrivate)
{
}

PeVTerminalConn::PeVTerminalConn(const PeVTerminalConn &VTerminalConn) : PeProjectObject(VTerminalConn), m_d(new PeVTerminalConnPrivate)
{
    *m_d = *VTerminalConn.m_d;
}

PeVTerminalConn::PeVTerminalConn(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otVTerminalConn, pProjectVersion, iId), m_d(new PeVTerminalConnPrivate)
{
}

PeVTerminalConn::~PeVTerminalConn()
{
    delete m_d;
}

QString PeVTerminalConn::GetDisplayName() const
{
    return "";
}

PeVTerminalConn& PeVTerminalConn::operator=(const PeVTerminalConn &VTerminalConn)
{
    PeProjectObject::operator =(VTerminalConn);
    *m_d = *VTerminalConn.m_d;

    return *this;
}

PeVTerminal* PeVTerminalConn::GetTxVTerminal() const
{
    return m_d->m_pTxVTerminal;
}

void PeVTerminalConn::SetTxVTerminal(PeVTerminal *pTxVTerminal)
{
    m_d->m_pTxVTerminal = pTxVTerminal;
}

PeVTerminal* PeVTerminalConn::GetRxVTerminal() const
{
    return m_d->m_pRxVTerminal;
}

void PeVTerminalConn::SetRxVTerminal(PeVTerminal *pRxVTerminal)
{
    m_d->m_pRxVTerminal = pRxVTerminal;
}

PeStrap* PeVTerminalConn::GetTxStrap() const
{
    return m_d->m_pTxStrap;
}

void PeVTerminalConn::SetTxStrap(PeStrap *pTxStrap)
{
    m_d->m_pTxStrap = pTxStrap;
}

PeStrap* PeVTerminalConn::GetRxStrap() const
{
    return m_d->m_pRxStrap;
}

void PeVTerminalConn::SetRxStrap(PeStrap *pRxStrap)
{
    m_d->m_pRxStrap = pRxStrap;
}

bool PeVTerminalConn::GetStraight() const
{
    return m_d->m_bStraight;
}

void PeVTerminalConn::SetStraight(bool bStraight)
{
    m_d->m_bStraight = bStraight;
}
