#ifndef PEBAY_H
#define PEBAY_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeBayPrivate;
class PeDevice;
class PROJECTEXPLORER_EXPORT PeBay : public PeProjectObject
{
    Q_OBJECT

// Construction and Destruction
public:
    PeBay();
    PeBay(const PeBay &Bay);
    PeBay(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeBay();

// Operations
public:
    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PeBay&              operator=(const PeBay &Bay);

    // Property operations
    QString             GetName() const;
    void                SetName(const QString strName);
    QString             GetNumber() const;
    void                SetNumber(const QString &strNumber);
    int                 GetVLevel() const;
    void                SetVLevel(int iVLevel);

    // Project object operation
    void                AddChildDevice(PeDevice *pDevice);
    void                RemoveChildDevice(PeDevice *pDevice);
    QList<PeDevice*>    GetChildDevices() const;

// Properties
private:
    PeBayPrivate    *m_d;
};

} // namespace ProjectExplorer

#endif // PEBAY_H
