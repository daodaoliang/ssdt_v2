#ifndef PEDWG_H
#define PEDWG_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeDwgPrivate;
class PeVolume;
class PROJECTEXPLORER_EXPORT PeDwg : public PeProjectObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum DwgType { dtDeviceConfig = 0, dtInfoSetGoose, dtInfoSetSv, dtVirtualGoose, dtVirtualSv, dtCableConn, dtNetwork, dtNetworkStructure};

// Construction and Destruction
public:
    PeDwg();
    PeDwg(const PeDwg &Dwg);
    PeDwg(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeDwg();

// Operations
public:
    static QString      GetDwgTypeName(const DwgType &eDwgType);

    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PeDwg&              operator=(const PeDwg &Dwg);

    // Property operations
    DwgType             GetDwgType() const;
    void                SetDwgType(DwgType eDwgType);
    QString             GetName() const;
    void                SetName(const QString strName);
    QString             GetDescription() const;
    void                SetDescription(const QString &strDescription);

    // Project object operation
    PeVolume*           GetParentVolume() const;
    void                SetParentVolume(PeVolume *pVolume);

// Properties
private:
    PeDwgPrivate    *m_d;
};

} // namespace ProjectExplorer

#endif // PEDWG_H
