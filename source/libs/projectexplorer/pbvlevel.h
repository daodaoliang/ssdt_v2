#ifndef PBVLEVEL_H
#define PBVLEVEL_H

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbVLevelPrivate;
class PROJECTEXPLORER_EXPORT PbVLevel : public PbBaseObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PbVLevel();
    PbVLevel(const PbVLevel &VLevel);
    PbVLevel(int iId);
    virtual ~PbVLevel();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PbVLevel&           operator=(const PbVLevel &VLevel);

    // Property operations
    int                 GetVLevel() const;
    void                SetVLevel(int iVLevel);
    QString             GetCode() const;
    void                SetCode(const QString &strCode);

// Properties
private:
    PbVLevelPrivate  *m_d;
};

} // namespace ProjectExplorer

#endif // PBVLEVEL_H
