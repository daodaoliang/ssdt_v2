#include <QMap>

#include "pbbaycategory.h"

namespace ProjectExplorer {

class PbBayCategoryPrivate
{
public:
    PbBayCategoryPrivate() : m_strName(""), m_strCode("") {}

    QString m_strName;
    QString m_strCode;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbBayCategory::PbBayCategory() :
    PbBaseObject(PbBaseObject::botBayCategory), m_d(new PbBayCategoryPrivate)
{
}

PbBayCategory::PbBayCategory(const PbBayCategory &BayCategory) :
    PbBaseObject(BayCategory), m_d(new PbBayCategoryPrivate)
{
    *m_d = *BayCategory.m_d;
}

PbBayCategory::PbBayCategory(int iId) :
    PbBaseObject(PbBaseObject::botBayCategory, iId), m_d(new PbBayCategoryPrivate)
{
}

PbBayCategory::~PbBayCategory()
{
    delete m_d;
}

QString PbBayCategory::GetDisplayName() const
{
    return m_d->m_strName;
}

PbBayCategory& PbBayCategory::operator=(const PbBayCategory &BayCategory)
{
    *m_d = *BayCategory.m_d;

    return *this;
}

QString PbBayCategory::GetName() const
{
    return m_d->m_strName;
}

void PbBayCategory::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}

QString PbBayCategory::GetCode() const
{
    return m_d->m_strCode;
}

void PbBayCategory::SetCode(const QString &strCode)
{
    m_d->m_strCode = strCode;
}
