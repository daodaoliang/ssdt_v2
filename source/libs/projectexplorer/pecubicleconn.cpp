#include <QMap>

#include "pecubicleconn.h"
#include "peprojectversion.h"
#include "pecubicle.h"

namespace ProjectExplorer {

class PeCubicleConnPrivate
{
public:
    PeCubicleConnPrivate() : m_pCubicle1(0), m_bUseOdf1(false), m_pPassCubicle1(0), m_pCubicle2(0), m_bUseOdf2(false), m_pPassCubicle2(0)
    {
    }

    PeCubicle   *m_pCubicle1;
    bool        m_bUseOdf1;
    PeCubicle   *m_pPassCubicle1;
    PeCubicle   *m_pCubicle2;
    bool        m_bUseOdf2;
    PeCubicle   *m_pPassCubicle2;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeCubicleConn::PeCubicleConn() : PeProjectObject(PeProjectObject::otCubicleConn), m_d(new PeCubicleConnPrivate)
{
}

PeCubicleConn::PeCubicleConn(const PeCubicleConn &CubicleConn) : PeProjectObject(CubicleConn), m_d(new PeCubicleConnPrivate)
{
    *m_d = *CubicleConn.m_d;
}

PeCubicleConn::PeCubicleConn(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otCubicleConn, pProjectVersion, iId), m_d(new PeCubicleConnPrivate)
{
}

PeCubicleConn::~PeCubicleConn()
{
    delete m_d;
}

QString PeCubicleConn::GetDisplayName() const
{
    return "";
}

PeCubicleConn& PeCubicleConn::operator=(const PeCubicleConn &CubicleConn)
{
    PeProjectObject::operator =(CubicleConn);
    *m_d = *CubicleConn.m_d;

    return *this;
}

PeCubicle* PeCubicleConn::GetCubicle1() const
{
    return m_d->m_pCubicle1;
}

void PeCubicleConn::SetCubicle1(PeCubicle *pCubicle)
{
    m_d->m_pCubicle1 = pCubicle;
}

bool PeCubicleConn::GetUseOdf1() const
{
    return m_d->m_bUseOdf1;
}

void PeCubicleConn::SetUseOdf1(bool bUseOdf)
{
    m_d->m_bUseOdf1 = bUseOdf;
}

PeCubicle* PeCubicleConn::GetPassCubicle1() const
{
    return m_d->m_pPassCubicle1;
}

void PeCubicleConn::SetPassCubicle1(PeCubicle *pCubicle)
{
    m_d->m_pPassCubicle1 = pCubicle;
}

PeCubicle* PeCubicleConn::GetCubicle2() const
{
    return m_d->m_pCubicle2;
}

void PeCubicleConn::SetCubicle2(PeCubicle *pCubicle)
{
    m_d->m_pCubicle2 = pCubicle;
}

bool PeCubicleConn::GetUseOdf2() const
{
    return m_d->m_bUseOdf2;
}

void PeCubicleConn::SetUseOdf2(bool bUseOdf)
{
    m_d->m_bUseOdf2 = bUseOdf;
}

PeCubicle* PeCubicleConn::GetPassCubicle2() const
{
    return m_d->m_pPassCubicle2;
}

void PeCubicleConn::SetPassCubicle2(PeCubicle *pCubicle)
{
    m_d->m_pPassCubicle2 = pCubicle;
}
