#include <QMap>

#include "pecable.h"

namespace ProjectExplorer {

class PeCablePrivate
{
public:
    PeCablePrivate()
        : m_strName(""), m_strNameBay(""), m_iNameNumber(0), m_strNameSet(""), m_pCubicle1(0), m_pCubicle2(0)
    {
    }

    PeCable::CableType      m_eCableType;
    QString                 m_strName;
    QString                 m_strNameBay;
    int                     m_iNameNumber;
    QString                 m_strNameSet;
    PeCubicle               *m_pCubicle1;
    PeCubicle               *m_pCubicle2;
    QList<PeFiber*>         m_lstFibers;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeCable::PeCable() : PeProjectObject(PeProjectObject::otCable), m_d(new PeCablePrivate)
{
}

PeCable::PeCable(const PeCable &Cable) : PeProjectObject(Cable), m_d(new PeCablePrivate)
{
    *m_d = *Cable.m_d;
}

PeCable::PeCable(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otCable, pProjectVersion, iId), m_d(new PeCablePrivate)
{
}

PeCable::~PeCable()
{
    delete m_d;
}

QString PeCable::GetCableTypeName(const CableType &eCableType)
{
    static QMap<CableType, QString> s_mapTypeToName;
    if(s_mapTypeToName.isEmpty())
    {
        s_mapTypeToName.insert(ctOptical, tr("Optical Cable"));
        s_mapTypeToName.insert(ctTail, tr("Tail Cable"));
        s_mapTypeToName.insert(ctJump, tr("Jump Fiber"));
    }

    return s_mapTypeToName.value(eCableType, tr("Unknown"));
}

bool PeCable::ComparCableName(PeCable *pCable1, PeCable *pCable2)
{
    if(pCable1->GetNameSet() == pCable2->GetNameSet())
        return pCable1->GetDisplayName() < pCable2->GetDisplayName();

    return pCable1->GetNameSet() < pCable2->GetNameSet();
}

QString PeCable::GetDisplayName() const
{
    return GetName();
}

PeCable& PeCable::operator=(const PeCable &Cable)
{
    PeProjectObject::operator =(Cable);
    *m_d = *Cable.m_d;

    return *this;
}

PeCable::CableType PeCable::GetCableType() const
{
    return m_d->m_eCableType;
}

void PeCable::SetCableType(const CableType &eCableType)
{
    m_d->m_eCableType = eCableType;
}

QString PeCable::GetName() const
{
    return m_d->m_strName;
}

void PeCable::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

QString PeCable::GetNameBay() const
{
    return m_d->m_strNameBay;
}

void PeCable::SetNameBay(const QString &strNameBay)
{
    m_d->m_strNameBay = strNameBay;
}

int PeCable::GetNameNumber() const
{
    return m_d->m_iNameNumber;
}

void PeCable::SetNameNumber(int iNameNumber)
{
    m_d->m_iNameNumber = iNameNumber;
}

QString PeCable::GetNameSet() const
{
    return m_d->m_strNameSet;
}

void PeCable::SetNameSet(const QString &strNameSet)
{
    m_d->m_strNameSet = strNameSet;
}

QString PeCable::CalculateNameByRule(const QString &strNameRule)
{
    QString strName = strNameRule;

    strName.replace("%{BayNumber}", m_d->m_strNameBay, Qt::CaseInsensitive);
    strName.replace("%{Index}", QString("%1").arg(m_d->m_iNameNumber, 3, 10, QLatin1Char('0')), Qt::CaseInsensitive);
    strName.replace("%{Set}", m_d->m_strNameSet, Qt::CaseInsensitive);

    return strName;
}

PeCubicle* PeCable::GetCubicle1() const
{
    return m_d->m_pCubicle1;
}

void PeCable::SetCubicle1(PeCubicle *pCubicle1)
{
    m_d->m_pCubicle1 = pCubicle1;
}

PeCubicle* PeCable::GetCubicle2() const
{
    return m_d->m_pCubicle2;
}

void PeCable::SetCubicle2(PeCubicle *pCubicle2)
{
    m_d->m_pCubicle2 = pCubicle2;
}

void PeCable::AddChildFiber(PeFiber *pFiber)
{
    if(!m_d->m_lstFibers.contains(pFiber))
        m_d->m_lstFibers.append(pFiber);
}

void PeCable::RemoveChildFiber(PeFiber *pFiber)
{
    m_d->m_lstFibers.removeOne(pFiber);
}

QList<PeFiber*> PeCable::GetChildFibers() const
{
    return m_d->m_lstFibers;
}
