#ifndef PBTPPORT_H
#define PBTPPORT_H

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbTpPortPrivate;
class PbTpDevice;
class PROJECTEXPLORER_EXPORT PbTpPort : public PbBaseObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum PortDirection { pdRx = 0, pdTx, pdRT };

// Construction and Destruction
public:
    PbTpPort();
    PbTpPort(const PbTpPort &Port);
    PbTpPort(int iId);
    virtual ~PbTpPort();

// Operations
public:
    static QString  GetPortDirectionName(const PortDirection &ePortDirection);

    static bool     CompareGroup(PbTpPort *pPort1, PbTpPort *pPort2);

    // Derived from PeProjectObject
    virtual QString GetDisplayName() const;

    // Operator overload
    PbTpPort&      operator=(const PbTpPort &Port);

    // Property operations
    QString         GetName() const;
    void            SetName(const QString &strName);
    int             GetGroup() const;
    void            SetGroup(int iGroup);
    PortDirection   GetPortDirection() const;
    void            SetPortDirection(const PortDirection &ePortDirection);

    // Project object operation
    PbTpDevice*     GetParentDevice() const;
    void            SetParentDevice(PbTpDevice *pTpDevice);

    PbTpPort*       GetPairPort();

// Properties
private:
    PbTpPortPrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PBTPPORT_H
