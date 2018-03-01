#include "peroom.h"

namespace ProjectExplorer {

class PeRoomPrivate
{
public:
    PeRoomPrivate() : m_strName(""), m_strNumber(""), m_bYard(false) {}

    QString             m_strName;
    QString             m_strNumber;
    bool                m_bYard;
    QList<PeCubicle*>   m_lstCubicles;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeRoom::PeRoom() : PeProjectObject(PeProjectObject::otRoom), m_d(new PeRoomPrivate)
{
}

PeRoom::PeRoom(const PeRoom &Room) : PeProjectObject(Room), m_d(new PeRoomPrivate)
{
    *m_d = *Room.m_d;
}

PeRoom::PeRoom(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otRoom, pProjectVersion, iId), m_d(new PeRoomPrivate)
{
}

PeRoom::~PeRoom()
{
    delete m_d;
}

QString PeRoom::GetDisplayName() const
{
    if(!m_d->m_strName.isEmpty())
        return GetNumber() + ":" + m_d->m_strName;

    return GetNumber();
}

PeRoom& PeRoom::operator=(const PeRoom &Room)
{
    PeProjectObject::operator =(Room);
    *m_d = *Room.m_d;

    return *this;
}

QString PeRoom::GetName() const
{
    return m_d->m_strName;
}

void PeRoom::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

QString PeRoom::GetNumber() const
{
    return m_d->m_strNumber;
}

void PeRoom::SetNumber(const QString &strNumber)
{
    m_d->m_strNumber = strNumber;
}

bool PeRoom::GetYard() const
{
    return m_d->m_bYard;
}

void PeRoom::SetYard(bool bYard)
{
    m_d->m_bYard = bYard;
}

void PeRoom::AddChildCubicle(PeCubicle *pCubicle)
{
    if(!m_d->m_lstCubicles.contains(pCubicle))
        m_d->m_lstCubicles.append(pCubicle);
}

void PeRoom::RemoveChildCubicle(PeCubicle *pCubicle)
{
    m_d->m_lstCubicles.removeOne(pCubicle);
}

QList<PeCubicle*> PeRoom::GetChildCubicles() const
{
    return m_d->m_lstCubicles;
}

