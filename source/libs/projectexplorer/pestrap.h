#ifndef PESTRAP_H
#define PESTRAP_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeStrapPrivate;
class PeDevice;
class PROJECTEXPLORER_EXPORT PeStrap : public PeProjectObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PeStrap();
    PeStrap(const PeStrap &Strap);
    PeStrap(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeStrap();

// Operations
public:
    static bool         ComparNumber(PeStrap *pStrap1, PeStrap *pStrap2);

    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PeStrap&            operator=(const PeStrap &Strap);

    // Property operations
    int                 GetNumber() const;
    void                SetNumber(int iNumber);
    QString             GetDescription() const;
    void                SetDescription(QString strescription);
    QString             GetLDInst() const;
    void                SetLDInst(QString strLDInst);
    QString             GetLNPrefix() const;
    void                SetLNPrefix(QString strLNPrefix);
    QString             GetLNClass() const;
    void                SetLNClass(QString strLNClass);
    QString             GetLNInst() const;
    void                SetLNInst(QString strLNInst);
    QString             GetDOName() const;
    void                SetDOName(QString strDOName);
    QString             GetDAName() const;
    void                SetDAName(QString strDAName);

    QString             GetRefName() const;

    // Project object operation
    PeDevice*           GetParentDevice() const;
    void                SetParentDevice(PeDevice *pDevice);

// Properties
private:
    PeStrapPrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PESTRAP_H
