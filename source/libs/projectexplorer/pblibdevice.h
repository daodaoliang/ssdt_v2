#ifndef PBLIBDEVICE_H
#define PBLIBDEVICE_H

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbLibDevicePrivate;
class PbLibBoard;
class PROJECTEXPLORER_EXPORT PbLibDevice : public PbBaseObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum DeviceType { dtIED = 0, dtSwitch, dtInvalid};

// Construction and Destruction
public:
    PbLibDevice();
    PbLibDevice(const PbLibDevice &Device);
    PbLibDevice(int iId);
    virtual ~PbLibDevice();

// Operations
public:
    static QIcon        GetDeviceIcon(const DeviceType &eDeviceType);
    static QString      GetDeviceTypeName(const DeviceType &eDeviceType);

    // Derived from PeProjectObject
    virtual QIcon       GetDisplayIcon() const;
    virtual QString     GetDisplayName() const;
    virtual QString     GetObjectTypeName() const;

    // Operator overload
    PbLibDevice&        operator=(const PbLibDevice &Device);

    // Property operations
    DeviceType          GetDeviceType() const;
    void                SetDeviceType(const DeviceType &eDeviceType);
    QString             GetManufacture() const;
    void                SetManufacture(const QString &strManufacture);
    QString             GetDeviceCategory() const;
    void                SetDeviceCategory(const QString &strDeviceCategory);
    QString             GetBayCategory() const;
    void                SetBayCategory(const QString &strBayCategory);
    QString             GetType() const;
    void                SetType(const QString &strType);
    QString             GetIcdFileName() const;
    void                SetIcdFileName(const QString &strIcdFileName);

    // Project object operation
    void                AddChildBoard(PbLibBoard *pBoard);
    void                RemoveChildBoard(PbLibBoard *pBoard);
    QList<PbLibBoard*>  GetChildBoards() const;

// Properties
private:
    PbLibDevicePrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PBLIBDEVICE_H
