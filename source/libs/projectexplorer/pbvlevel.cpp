#include <QMap>

#include "pbvlevel.h"

namespace ProjectExplorer {

class PbVLevelPrivate
{
public:
    PbVLevelPrivate() : m_iVLevel(0), m_strCode("") {}

    int     m_iVLevel;
    QString m_strCode;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbVLevel::PbVLevel() :
    PbBaseObject(PbBaseObject::botVLevel), m_d(new PbVLevelPrivate)
{
}

PbVLevel::PbVLevel(const PbVLevel &VLevel) :
    PbBaseObject(VLevel), m_d(new PbVLevelPrivate)
{
    *m_d = *VLevel.m_d;
}

PbVLevel::PbVLevel(int iId) :
    PbBaseObject(PbBaseObject::botVLevel, iId), m_d(new PbVLevelPrivate)
{
}

PbVLevel::~PbVLevel()
{
    delete m_d;
}

QString PbVLevel::GetDisplayName() const
{
    return QString("%1 kV").arg(m_d->m_iVLevel);
}

PbVLevel& PbVLevel::operator=(const PbVLevel &VLevel)
{
    *m_d = *VLevel.m_d;

    return *this;
}

int PbVLevel::GetVLevel() const
{
    return m_d->m_iVLevel;
}

void PbVLevel::SetVLevel(int iVLevel)
{
    m_d->m_iVLevel = iVLevel;
}

QString PbVLevel::GetCode() const
{
    return m_d->m_strCode;
}

void PbVLevel::SetCode(const QString &strCode)
{
    m_d->m_strCode = strCode;
}
