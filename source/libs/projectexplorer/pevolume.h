#ifndef PEVOLUME_H
#define PEVOLUME_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeVolumePrivate;
class PeDwg;
class PROJECTEXPLORER_EXPORT PeVolume : public PeProjectObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PeVolume();
    PeVolume(const PeVolume &Volume);
    PeVolume(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeVolume();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PeVolume&           operator=(const PeVolume &Volume);

    // Property operations
    QString             GetName() const;
    void                SetName(const QString strName);
    QString             GetDescription() const;
    void                SetDescription(const QString &strDescription);

    // Project object operation
    void                AddChildDwg(PeDwg *pDwg);
    void                RemoveChildDwg(PeDwg *pDwg);
    QList<PeDwg*>       GetChildDwgs() const;

// Properties
private:
    PeVolumePrivate    *m_d;
};

} // namespace ProjectExplorer

#endif // PEVOLUME_H
