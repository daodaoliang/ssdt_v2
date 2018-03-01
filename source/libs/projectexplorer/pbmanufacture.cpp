#include <QMap>

#include "pbmanufacture.h"

namespace ProjectExplorer {

class PbManufacturePrivate
{
public:
    PbManufacturePrivate() : m_strName("") {}

    QString m_strName;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbManufacture::PbManufacture() :
    PbBaseObject(PbBaseObject::botManufacture), m_d(new PbManufacturePrivate)
{
}

PbManufacture::PbManufacture(const PbManufacture &Manufacture) :
    PbBaseObject(Manufacture), m_d(new PbManufacturePrivate)
{
    *m_d = *Manufacture.m_d;
}

PbManufacture::PbManufacture(int iId) :
    PbBaseObject(PbBaseObject::botManufacture, iId), m_d(new PbManufacturePrivate)
{
}

PbManufacture::~PbManufacture()
{
    delete m_d;
}

QString PbManufacture::GetDisplayName() const
{
    return m_d->m_strName;
}

PbManufacture& PbManufacture::operator=(const PbManufacture &Manufacture)
{
    *m_d = *Manufacture.m_d;

    return *this;
}

QString PbManufacture::GetName() const
{
    return m_d->m_strName;
}

void PbManufacture::SetName(const QString &strName)
{
    m_d->m_strName = strName;
}
