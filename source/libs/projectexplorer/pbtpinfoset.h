#ifndef PBTPINFOSET_H
#define PBTPINFOSET_H

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbTpInfoSetPrivate;
class PbTpDevice;
class PbTpPort;
class PbTpBay;
class PROJECTEXPLORER_EXPORT PbTpInfoSet : public PbBaseObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum InfoSetType { itNone = 0, itGoose, itSv, itTime, itMix};

// Construction and Destruction
public:
    PbTpInfoSet();
    PbTpInfoSet(const PbTpInfoSet &InfoSet);
    PbTpInfoSet(int iId);
    virtual ~PbTpInfoSet();

// Operations
public:
    static QString      GetInfoSetTypeName(const InfoSetType &eInfoSetType);

    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PbTpInfoSet&          operator=(const PbTpInfoSet &InfoSet);

    // Property operations
    QString             GetName() const;
    void                SetName(const QString &strName);
    InfoSetType         GetInfoSetType() const;
    void                SetInfoSetType(const InfoSetType &eInfoSetType);
    int                 GetGroup() const;
    void                SetGroup(int iGroup) const;

    // Project object operation
    PbTpDevice*         GetTxIED() const;
    void                SetTxIED(PbTpDevice *pTxIED);
    PbTpPort*           GetTxPort() const;
    void                SetTxPort(PbTpPort *pTxPort);
    PbTpDevice*         GetRxIED() const;
    void                SetRxIED(PbTpDevice *pRxIED);
    PbTpPort*           GetRxPort() const;
    void                SetRxPort(PbTpPort *pRxPort);
    PbTpDevice*         GetSwitch1() const;
    void                SetSwitch1(PbTpDevice *pSwitch1);
    PbTpPort*           GetSwitch1TxPort() const;
    void                SetSwitch1TxPort(PbTpPort *pSwitch1TxPort);
    PbTpPort*           GetSwitch1RxPort() const;
    void                SetSwitch1RxPort(PbTpPort *pSwitch1RxPort);
    PbTpDevice*         GetSwitch2() const;
    void                SetSwitch2(PbTpDevice *pSwitch2);
    PbTpPort*           GetSwitch2TxPort() const;
    void                SetSwitch2TxPort(PbTpPort *pSwitch2TxPort);
    PbTpPort*           GetSwitch2RxPort() const;
    void                SetSwitch2RxPort(PbTpPort *pSwitch2RxPort);
    PbTpDevice*         GetSwitch3() const;
    void                SetSwitch3(PbTpDevice *pSwitch3);
    PbTpPort*           GetSwitch3TxPort() const;
    void                SetSwitch3TxPort(PbTpPort *pSwitch3TxPort);
    PbTpPort*           GetSwitch3RxPort() const;
    void                SetSwitch3RxPort(PbTpPort *pSwitch3RxPort);
    PbTpDevice*         GetSwitch4() const;
    void                SetSwitch4(PbTpDevice *pSwitch4);
    PbTpPort*           GetSwitch4TxPort() const;
    void                SetSwitch4TxPort(PbTpPort *pSwitch4TxPort);
    PbTpPort*           GetSwitch4RxPort() const;
    void                SetSwitch4RxPort(PbTpPort *pSwitch4RxPort);

    PbTpInfoSet*        GetPairInfoSet() const;
    bool                IsContaintPort(ProjectExplorer::PbTpPort *pPort, ProjectExplorer::PbTpPort **ppPortConnected) const;
    bool                ReplacePort(ProjectExplorer::PbTpPort *pPort, ProjectExplorer::PbTpPort *pPortReplaced);
    bool                IsComplete() const;

    // Project object operation
    PbTpBay*            GetParentBay() const;
    void                SetParentBay(PbTpBay *pTpBay);

// Properties
private:
    PbTpInfoSetPrivate    *m_d;
};

} // namespace ProjectExplorer

#endif // PBTPINFOSET_H
