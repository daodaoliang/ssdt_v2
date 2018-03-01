#ifndef PBMANUFACTURE_H
#define PBMANUFACTURE_H

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbManufacturePrivate;
class PROJECTEXPLORER_EXPORT PbManufacture : public PbBaseObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PbManufacture();
    PbManufacture(const PbManufacture &Manufacture);
    PbManufacture(int iId);
    virtual ~PbManufacture();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PbManufacture&      operator=(const PbManufacture &Manufacture);

    // Property operations
    QString             GetName() const;
    void                SetName(const QString &strName);

// Properties
private:
    PbManufacturePrivate  *m_d;
};

} // namespace ProjectExplorer

#endif // PBMANUFACTURE_H
