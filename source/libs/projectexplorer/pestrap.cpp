#include <QMap>

#include "pestrap.h"
#include "pedevice.h"

namespace ProjectExplorer {

class PeStrapPrivate
{
public:
    PeStrapPrivate()
        : m_iNumber(0), m_strDescription(""), m_strLDInst(""), m_strLNPrefix(""), m_strLNClass(""), m_strLNInst(""), m_strDOName(""), m_strDAName(""), m_pDevice(0)
    {
    }

    int                             m_iNumber;
    QString                         m_strDescription;
    QString                         m_strLDInst;
    QString                         m_strLNPrefix;
    QString                         m_strLNClass;
    QString                         m_strLNInst;
    QString                         m_strDOName;
    QString                         m_strDAName;
    PeDevice                        *m_pDevice;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeStrap::PeStrap() : PeProjectObject(PeProjectObject::otStrap), m_d(new PeStrapPrivate)
{
}

PeStrap::PeStrap(const PeStrap &Strap) : PeProjectObject(Strap), m_d(new PeStrapPrivate)
{
    *m_d = *Strap.m_d;
}

PeStrap::PeStrap(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otStrap, pProjectVersion, iId), m_d(new PeStrapPrivate)
{
}

PeStrap::~PeStrap()
{
    delete m_d;
}

bool PeStrap::ComparNumber(PeStrap *pStrap1, PeStrap *pStrap2)
{
    return pStrap1->GetNumber() < pStrap2->GetNumber();
}

QString PeStrap::GetDisplayName() const
{
    return GetDescription();
}

PeStrap& PeStrap::operator=(const PeStrap &Strap)
{
    PeProjectObject::operator =(Strap);
    *m_d = *Strap.m_d;

    return *this;
}

int PeStrap::GetNumber() const
{
    return m_d->m_iNumber;
}

void PeStrap::SetNumber(int iNumber)
{
    m_d->m_iNumber = iNumber;
}

QString PeStrap::GetDescription() const
{
    return m_d->m_strDescription;
}

void PeStrap::SetDescription(QString strDescription)
{
    m_d->m_strDescription = strDescription;
}

QString PeStrap::GetLDInst() const
{
    return m_d->m_strLDInst;
}

void PeStrap::SetLDInst(QString strLDInst)
{
    m_d->m_strLDInst = strLDInst;
}

QString PeStrap::GetLNPrefix() const
{
    return m_d->m_strLNPrefix;
}

void PeStrap::SetLNPrefix(QString strLNPrefix)
{
    m_d->m_strLNPrefix = strLNPrefix;
}

QString PeStrap::GetLNClass() const
{
    return m_d->m_strLNClass;
}

void PeStrap::SetLNClass(QString strLNClass)
{
    m_d->m_strLNClass = strLNClass;
}

QString PeStrap::GetLNInst() const
{
    return m_d->m_strLNInst;
}

void PeStrap::SetLNInst(QString strLNInst)
{
    m_d->m_strLNInst = strLNInst;
}

QString PeStrap::GetDOName() const
{
    return m_d->m_strDOName;
}

void PeStrap::SetDOName(QString strDOName)
{
    m_d->m_strDOName = strDOName;
}

QString PeStrap::GetDAName() const
{
    return m_d->m_strDAName;
}

void PeStrap::SetDAName(QString strDAName)
{
    m_d->m_strDAName = strDAName;
}

QString PeStrap::GetRefName() const
{
    QString strRefName = QString("%1/%2%3%4.%5").arg(m_d->m_strLDInst)
                                                .arg(m_d->m_strLNPrefix)
                                                .arg(m_d->m_strLNClass)
                                                .arg(m_d->m_strLNInst)
                                                .arg(m_d->m_strDOName);
    if(!m_d->m_strDAName.isEmpty())
        strRefName = QString("%1.%2").arg(strRefName).arg(m_d->m_strDAName);

    return strRefName;
}

PeDevice* PeStrap::GetParentDevice() const
{
    return m_d->m_pDevice;
}

void PeStrap::SetParentDevice(PeDevice *pDevice)
{
    m_d->m_pDevice = pDevice;
}
