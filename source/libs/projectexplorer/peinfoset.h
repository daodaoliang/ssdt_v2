#ifndef PEINFOSET_H
#define PEINFOSET_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeInfoSetPrivate;
class PeDevice;
class PePort;
class PROJECTEXPLORER_EXPORT PeInfoSet : public PeProjectObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum InfoSetType { itNone = 0, itGoose, itSv, itTime, itMix};

// Construction and Destruction
public:
    PeInfoSet();
    PeInfoSet(const PeInfoSet &InfoSet);
    PeInfoSet(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeInfoSet();

// Operations
public:
    static QString      GetInfoSetTypeName(const InfoSetType &eInfoSetType);

    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PeInfoSet&          operator=(const PeInfoSet &InfoSet);

    // Property operations
    QString             GetName() const;
    void                SetName(const QString &strName);
    QString             GetDescription() const;
    void                SetDescription(const QString &strDescription);
    InfoSetType         GetInfoSetType() const;
    void                SetInfoSetType(const InfoSetType &eInfoSetType);
    int                 GetGroup() const;
    void                SetGroup(int iGroup) const;

    // Project object operation
    PeDevice*           GetTxIED() const;
    void                SetTxIED(PeDevice *pTxIED);
    PePort*             GetTxPort() const;
    void                SetTxPort(PePort *pTxPort);
    PeDevice*           GetRxIED() const;
    void                SetRxIED(PeDevice *pRxIED);
    PePort*             GetRxPort() const;
    void                SetRxPort(PePort *pRxPort);
    PeDevice*           GetSwitch1() const;
    void                SetSwitch1(PeDevice *pSwitch1);
    PePort*             GetSwitch1TxPort() const;
    void                SetSwitch1TxPort(PePort *pSwitch1TxPort);
    PePort*             GetSwitch1RxPort() const;
    void                SetSwitch1RxPort(PePort *pSwitch1RxPort);
    PeDevice*           GetSwitch2() const;
    void                SetSwitch2(PeDevice *pSwitch2);
    PePort*             GetSwitch2TxPort() const;
    void                SetSwitch2TxPort(PePort *pSwitch2TxPort);
    PePort*             GetSwitch2RxPort() const;
    void                SetSwitch2RxPort(PePort *pSwitch2RxPort);
    PeDevice*           GetSwitch3() const;
    void                SetSwitch3(PeDevice *pSwitch3);
    PePort*             GetSwitch3TxPort() const;
    void                SetSwitch3TxPort(PePort *pSwitch3TxPort);
    PePort*             GetSwitch3RxPort() const;
    void                SetSwitch3RxPort(PePort *pSwitch3RxPort);
    PeDevice*           GetSwitch4() const;
    void                SetSwitch4(PeDevice *pSwitch4);
    PePort*             GetSwitch4TxPort() const;
    void                SetSwitch4TxPort(PePort *pSwitch4TxPort);
    PePort*             GetSwitch4RxPort() const;
    void                SetSwitch4RxPort(PePort *pSwitch4RxPort);

    void                UpdateName(const QStringList &lstExtraInfoSetNames = QStringList());
    PeInfoSet*          GetPairInfoSet() const;
    bool                GetSetIndexAndMainDevice(QString &strSetIndex, ProjectExplorer::PeDevice **ppMainDevice = 0) const;
    bool                IsContaintPort(ProjectExplorer::PePort *pPort, ProjectExplorer::PePort **ppPortConnected) const;
    bool                ReplacePort(ProjectExplorer::PePort *pPort, ProjectExplorer::PePort *pPortReplaced);
    bool                IsComplete() const;

// Properties
private:
    PeInfoSetPrivate    *m_d;
};

} // namespace ProjectExplorer

#endif // PEINFOSET_H
