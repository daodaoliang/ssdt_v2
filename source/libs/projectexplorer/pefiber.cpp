#include <QMap>

#include "pefiber.h"

namespace ProjectExplorer {

class PeFiberPrivate
{
public:
    PeFiberPrivate()
        : m_iIndex(0), m_eFiberColor(PeFiber::fcInvalid), m_ePipeColor(PeFiber::fcInvalid), m_bReserve(false), m_pPort1(0), m_pPort2(0), m_pCable(0)
    {
    }

    int                 m_iIndex;
    PeFiber::FiberColor m_eFiberColor;
    PeFiber::FiberColor m_ePipeColor;
    bool                m_bReserve;
    QStringList         m_lstInfoSetIds;
    PePort              *m_pPort1;
    PePort              *m_pPort2;
    PeCable             *m_pCable;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeFiber::PeFiber() : PeProjectObject(PeProjectObject::otFiber), m_d(new PeFiberPrivate)
{
}

PeFiber::PeFiber(const PeFiber &Fiber) : PeProjectObject(Fiber), m_d(new PeFiberPrivate)
{
    *m_d = *Fiber.m_d;
}

PeFiber::PeFiber(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otFiber, pProjectVersion, iId), m_d(new PeFiberPrivate)
{
}

PeFiber::~PeFiber()
{
    delete m_d;
}

QString PeFiber::GetFiberColorName(const FiberColor &eFiberColor)
{
    static QMap<FiberColor, QString> s_mapTypeToName;
    if(s_mapTypeToName.isEmpty())
    {
        s_mapTypeToName.insert(fcBlue, tr("Blue"));
        s_mapTypeToName.insert(fcOrange, tr("Orange"));
        s_mapTypeToName.insert(fcGreen, tr("Green"));
        s_mapTypeToName.insert(fcBrown, tr("Brown"));
        s_mapTypeToName.insert(fcGray, tr("Gray"));
        s_mapTypeToName.insert(fcOrigin, tr("Origin"));
        s_mapTypeToName.insert(fcRed, tr("Red"));
        s_mapTypeToName.insert(rcBlack, tr("Black"));
        s_mapTypeToName.insert(fcYellow, tr("Yellow"));
        s_mapTypeToName.insert(fcViolet, tr("Voilet"));
        s_mapTypeToName.insert(fcPink, tr("Pink"));
        s_mapTypeToName.insert(fcTurquoise, tr("Turquoise"));
        s_mapTypeToName.insert(fcInvalid, tr("Invalid"));
    }

    return s_mapTypeToName.value(eFiberColor, tr("Invalid"));
}

QColor PeFiber::GetFiberColor(const FiberColor &eFiberColor)
{
    static QMap<FiberColor, QColor> s_mapTypeToColor;
    if(s_mapTypeToColor.isEmpty())
    {
        s_mapTypeToColor.insert(fcBlue, QColor(0, 0, 255));
        s_mapTypeToColor.insert(fcOrange, QColor(255, 165, 0));
        s_mapTypeToColor.insert(fcGreen, QColor(0, 128, 0));
        s_mapTypeToColor.insert(fcBrown, QColor(165, 42, 42));
        s_mapTypeToColor.insert(fcGray, QColor(128, 128, 128));
        s_mapTypeToColor.insert(fcOrigin, QColor(211, 211, 211));
        s_mapTypeToColor.insert(fcRed, QColor(255, 0, 0));
        s_mapTypeToColor.insert(rcBlack, QColor(0, 0, 0));
        s_mapTypeToColor.insert(fcYellow, QColor(255, 255, 0));
        s_mapTypeToColor.insert(fcViolet, QColor(238, 130, 238));
        s_mapTypeToColor.insert(fcPink, QColor(255, 192, 203));
        s_mapTypeToColor.insert(fcTurquoise, QColor(64, 224, 208));
        s_mapTypeToColor.insert(fcInvalid, QColor(0, 0, 0));
    }

    return s_mapTypeToColor.value(eFiberColor, QColor(0, 0, 0));
}

bool PeFiber::CompareIndex(PeFiber *pFiber1, PeFiber *pFiber2)
{
    return pFiber1->GetIndex() < pFiber2->GetIndex();
}

QString PeFiber::GetDisplayName() const
{
    QString strDisplayName = QString("%1").arg(GetIndex(), 2, 10, QLatin1Char('0'));
    if(m_d->m_bReserve)
        strDisplayName = strDisplayName + " " + tr("Reserve");

    return strDisplayName;
}

PeFiber& PeFiber::operator=(const PeFiber &Fiber)
{
    PeProjectObject::operator =(Fiber);
    *m_d = *Fiber.m_d;

    return *this;
}

int PeFiber::GetIndex() const
{
    return m_d->m_iIndex;
}

void PeFiber::SetIndex(int iIndex)
{
    m_d->m_iIndex = iIndex;
}

PeFiber::FiberColor PeFiber::GetFiberColor() const
{
    return m_d->m_eFiberColor;
}

void PeFiber::SetFiberColor(const FiberColor &eFiberColor)
{
    m_d->m_eFiberColor = eFiberColor;
}

PeFiber::FiberColor PeFiber::GetPipeColor() const
{
    return m_d->m_ePipeColor;
}

void PeFiber::SetPipeColor(const FiberColor &ePipeColor)
{
    m_d->m_ePipeColor = ePipeColor;
}

bool PeFiber::GetReserve() const
{
    return m_d->m_bReserve;
}

void PeFiber::SetReserve(bool bReserve)
{
    m_d->m_bReserve = bReserve;
}

QStringList PeFiber::GetInfoSetIds() const
{
    return m_d->m_lstInfoSetIds;
}

void PeFiber::SetInfoSetIds(const QStringList &lstInfoSetIds)
{
    m_d->m_lstInfoSetIds = lstInfoSetIds;
}

PePort* PeFiber::GetPort1() const
{
    return m_d->m_pPort1;
}

void PeFiber::SetPort1(PePort *pPort1)
{
    m_d->m_pPort1 = pPort1;
}

PePort* PeFiber::GetPort2() const
{
    return m_d->m_pPort2;
}

void PeFiber::SetPort2(PePort *pPort2)
{
    m_d->m_pPort2 = pPort2;
}

PeCable* PeFiber::GetParentCable() const
{
    return m_d->m_pCable;
}

void PeFiber::SetParentCable(PeCable *pCable)
{
    m_d->m_pCable = pCable;
}

void PeFiber::AddInfoSetIds(const QStringList &lstInfoSetIds)
{
    foreach(const QString &strInfoSetId, lstInfoSetIds)
    {
        if(!m_d->m_lstInfoSetIds.contains(strInfoSetId))
            m_d->m_lstInfoSetIds.append(strInfoSetId);
    }
}

void PeFiber::RemoveInfoSetIds(const QStringList &lstInfoSetIds)
{
    foreach(const QString &strInfoSetId, lstInfoSetIds)
        m_d->m_lstInfoSetIds.removeAll(strInfoSetId);
}
