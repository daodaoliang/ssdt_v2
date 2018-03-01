#ifndef PBBAYCATEGORY_H
#define PBBAYCATEGORY_H

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbBayCategoryPrivate;
class PROJECTEXPLORER_EXPORT PbBayCategory : public PbBaseObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PbBayCategory();
    PbBayCategory(const PbBayCategory &BayCategory);
    PbBayCategory(int iId);
    virtual ~PbBayCategory();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PbBayCategory&      operator=(const PbBayCategory &BayCategory);

    // Property operations
    QString             GetName() const;
    void                SetName(const QString &strName);
    QString             GetCode() const;
    void                SetCode(const QString &strCode);

// Properties
private:
    PbBayCategoryPrivate  *m_d;
};

} // namespace ProjectExplorer

#endif // PBDEVICECATEGORY_H
