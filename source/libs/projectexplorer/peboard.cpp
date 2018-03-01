#include <QMap>

#include "peboard.h"
#include "pedevice.h"
#include "peport.h"

namespace ProjectExplorer {

class PeBoardPrivate
{
public:
    PeBoardPrivate()
        : m_strPosition(""), m_strDescription(""), m_strType(""), m_pDevice(0)
    {
    }

    QString         m_strPosition;
    QString         m_strDescription;
    QString         m_strType;
    PeDevice*       m_pDevice;
    QList<PePort*>  m_lstPorts;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeBoard::PeBoard() : PeProjectObject(PeProjectObject::otBoard), m_d(new PeBoardPrivate)
{
}

PeBoard::PeBoard(const PeBoard &Board) : PeProjectObject(Board), m_d(new PeBoardPrivate)
{
    *m_d = *Board.m_d;
}

PeBoard::PeBoard(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otBoard, pProjectVersion, iId), m_d(new PeBoardPrivate)
{
}

PeBoard::~PeBoard()
{
    delete m_d;
}

QString PeBoard::GetDisplayName() const
{
    return GetPosition();
}

PeBoard& PeBoard::operator=(const PeBoard &Board)
{
    PeProjectObject::operator =(Board);
    *m_d = *Board.m_d;

    return *this;
}

QString PeBoard::GetPosition() const
{
    return m_d->m_strPosition;
}

void PeBoard::SetPosition(const QString &strPosition)
{
    m_d->m_strPosition = strPosition;
}

QString PeBoard::GetDescription() const
{
    return m_d->m_strDescription;
}

void PeBoard::SetDescription(const QString &strDescription)
{
    m_d->m_strDescription = strDescription;
}

QString PeBoard::GetType() const
{
    return m_d->m_strType;
}

void PeBoard::SetType(const QString &strType)
{
    m_d->m_strType = strType;
}

PeDevice* PeBoard::GetParentDevice() const
{
    return m_d->m_pDevice;
}

void PeBoard::SetParentDevice(PeDevice *pDevice)
{
    m_d->m_pDevice = pDevice;
}

void PeBoard::AddChildPort(PePort *pPort)
{
    if(!m_d->m_lstPorts.contains(pPort))
        m_d->m_lstPorts.append(pPort);
}

void PeBoard::RemoveChildPort(PePort *pPort)
{
    m_d->m_lstPorts.removeOne(pPort);
}

QList<PePort*> PeBoard::GetChildPorts() const
{
    return m_d->m_lstPorts;
}

bool PeBoard::IsBoardUsed(const QList<PeFiber*> lstUnusedFibers) const
{
    bool bUsed = false;

    foreach(PePort *pPort, m_d->m_lstPorts)
    {
        if(!pPort->GetUsedFibers(lstUnusedFibers).isEmpty())
        {
            bUsed = true;
            break;
        }
    }

    return bUsed;
}
