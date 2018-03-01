#ifndef PBTPBAY_H
#define PBTPBAY_H

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbTpBayPrivate;
class PbTpDevice;
class PbTpInfoSet;
class PROJECTEXPLORER_EXPORT PbTpBay : public PbBaseObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PbTpBay();
    PbTpBay(const PbTpBay &Bay);
    PbTpBay(int iId);
    virtual ~PbTpBay();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PbTpBay&            operator=(const PbTpBay &Bay);

    // Property operations
    QString             GetName() const;
    void                SetName(const QString &strName);
    int                 GetVLevel() const;
    void                SetVLevel(int iVLevel);

    // Project object operation
    void                AddChildDevice(PbTpDevice *pDevice);
    void                RemoveChildDevice(PbTpDevice *pDevice);
    QList<PbTpDevice*>  GetChildDevices() const;

    void                AddChildInfoSet(PbTpInfoSet *pInfoSet);
    void                RemoveChildInfoSet(PbTpInfoSet *pInfoSet);
    QList<PbTpInfoSet*> GetChildInfoSets() const;

// Properties
private:
    PbTpBayPrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PBTPBAY_H
