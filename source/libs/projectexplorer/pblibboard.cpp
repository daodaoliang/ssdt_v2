#include <QMap>

#include "pblibboard.h"
#include "pblibdevice.h"

namespace ProjectExplorer {

class PbLibBoardPrivate
{
public:
    PbLibBoardPrivate()
        : m_strPosition(""), m_strDescription(""), m_strType(""), m_pDevice(0)
    {
    }

    QString             m_strPosition;
    QString             m_strDescription;
    QString             m_strType;
    PbLibDevice*        m_pDevice;
    QList<PbLibPort*>   m_lstPorts;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbLibBoard::PbLibBoard() : PbBaseObject(PbBaseObject::botLibBoard), m_d(new PbLibBoardPrivate)
{
}

PbLibBoard::PbLibBoard(const PbLibBoard &Board) : PbBaseObject(Board), m_d(new PbLibBoardPrivate)
{
    *m_d = *Board.m_d;
}

PbLibBoard::PbLibBoard(int iId) : PbBaseObject(PbBaseObject::botLibBoard, iId), m_d(new PbLibBoardPrivate)
{
}

PbLibBoard::~PbLibBoard()
{
    delete m_d;
}

QString PbLibBoard::GetDisplayName() const
{
    return GetPosition();
}

PbLibBoard& PbLibBoard::operator=(const PbLibBoard &Board)
{
    PbBaseObject::operator =(Board);
    *m_d = *Board.m_d;

    return *this;
}

QString PbLibBoard::GetPosition() const
{
    return m_d->m_strPosition;
}

void PbLibBoard::SetPosition(const QString &strPosition)
{
    m_d->m_strPosition = strPosition;
}

QString PbLibBoard::GetDescription() const
{
    return m_d->m_strDescription;
}

void PbLibBoard::SetDescription(const QString &strDescription)
{
    m_d->m_strDescription = strDescription;
}

QString PbLibBoard::GetType() const
{
    return m_d->m_strType;
}

void PbLibBoard::SetType(const QString &strType)
{
    m_d->m_strType = strType;
}

PbLibDevice* PbLibBoard::GetParentDevice() const
{
    return m_d->m_pDevice;
}

void PbLibBoard::SetParentDevice(PbLibDevice *pDevice)
{
    m_d->m_pDevice = pDevice;
}

void PbLibBoard::AddChildPort(PbLibPort *pPort)
{
    if(!m_d->m_lstPorts.contains(pPort))
        m_d->m_lstPorts.append(pPort);
}

void PbLibBoard::RemoveChildPort(PbLibPort *pPort)
{
    m_d->m_lstPorts.removeOne(pPort);
}

QList<PbLibPort*> PbLibBoard::GetChildPorts() const
{
    return m_d->m_lstPorts;
}
