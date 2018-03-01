#ifndef PECUBICLECONN_H
#define PECUBICLECONN_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeCubicleConnPrivate;
class PeCubicle;
class PROJECTEXPLORER_EXPORT PeCubicleConn : public PeProjectObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PeCubicleConn();
    PeCubicleConn(const PeCubicleConn &CubicleConn);
    PeCubicleConn(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeCubicleConn();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PeCubicleConn&      operator=(const PeCubicleConn &CubicleConn);

    // Project object operation
    PeCubicle*          GetCubicle1() const;
    void                SetCubicle1(PeCubicle *pCubicle);
    bool                GetUseOdf1() const;
    void                SetUseOdf1(bool bUseOdf);
    PeCubicle*          GetPassCubicle1() const;
    void                SetPassCubicle1(PeCubicle *pCubicle);
    PeCubicle*          GetCubicle2() const;
    void                SetCubicle2(PeCubicle *pCubicle);
    bool                GetUseOdf2() const;
    void                SetUseOdf2(bool bUseOdf);
    PeCubicle*          GetPassCubicle2() const;
    void                SetPassCubicle2(PeCubicle *pCubicle);

// Properties
private:
    PeCubicleConnPrivate    *m_d;
};

} // namespace ProjectExplorer

#endif // PECUBICLECONN_H
