#ifndef CABLEGENERATOR_H
#define CABLEGENERATOR_H

#include <QObject>
#include <QMap>
#include <QString>

namespace ProjectExplorer {

class PeProjectVersion;
class PeCubicle;
class PeInfoSet;
class PePort;
class PeDevice;
class PeCable;
class PeFiber;
class PeBoard;

}

namespace PhyNetwork {
namespace Internal {

class CableGenerator : public QObject
{
// Structures and Enumerations
public:
    class _Cable;
    class _Fiber
    {
    public:
        bool bReserve;

        QMap<ProjectExplorer::PeCubicle*, ProjectExplorer::PePort*> mapCubicleToPort;
        QMap<ProjectExplorer::PeCubicle*, _Fiber*> mapCubicleToConnectedFiber;

        ProjectExplorer::PePort* pJumpPort1;
        ProjectExplorer::PePort* pJumpPort2;
        _Fiber* pJumpConnectedFiber1;
        _Fiber* pJumpConnectedFiber2;

        _Cable *pCable;
        QList<ProjectExplorer::PeInfoSet*> lstInfoSets;
    };

    class _Cable
    {
    public:
        bool bPass;
        QString strSet;
        ProjectExplorer::PeCubicle *pCubicle1;
        ProjectExplorer::PeCubicle *pCubicle2;
        QList<_Fiber*> lstFibers;
    };

    class _InfoSetConn
    {
    public:
        QString strSet;
        QMap<ProjectExplorer::PeCubicle*, ProjectExplorer::PePort*> mapCubicleToPort;
        ProjectExplorer::PePort* pJumpPort1;
        ProjectExplorer::PePort* pJumpPort2;
        QList<ProjectExplorer::PeInfoSet*> lstInfoSets;
    };

    class _CubicleInfoSet
    {
    public:
        ProjectExplorer::PeCubicle *pCubicle1;
        ProjectExplorer::PeCubicle *pCubicle2;
        QList<_InfoSetConn*> lstInfoSetConns;
    };

    class _ValidPortPath
    {
        ProjectExplorer::PePort     *pPortTx;
        ProjectExplorer::PePort     *pPortRx;

        ProjectExplorer::PeFiber    *pFiberJumpTail1;
        ProjectExplorer::PeFiber    *pFiberOptical;
        ProjectExplorer::PeFiber    *pFiberJumpTail2;
    };

    enum GroupType { gtCustom = 0, gtPort };
    enum ReserveType { rtAll = 0, rtSingle };

// Construction and Destruction
public:
    CableGenerator(QObject *pParent = 0);
    ~CableGenerator();

// Operations
public:
    static                              CableGenerator* Instance();

    GroupType                           GetOpticalGroupType () const;
    void                                SetOpticalGroupType(GroupType eGroupType);
    QList<int>                          GetOpticalFiberNumbers() const;
    void                                SetOpticalFiberNumbers(const QList<int> &lstFiberNumbers);
    ReserveType                         GetOpticalReserveType() const;
    void                                SetOpticalReserveType(ReserveType eReserveType);
    double                              GetOpticalReserveRate() const;
    void                                SetOpticalReserveRate(double dReserveRate);

    GroupType                           GetTailGroupType () const;
    void                                SetTailGroupType(GroupType eGroupType);
    QList<int>                          GetTailFiberNumbers() const;
    void                                SetTailFiberNumbers(const QList<int> &lstFiberNumbers);
    ReserveType                         GetTailReserveType() const;
    void                                SetTailReserveType(ReserveType eReserveType);
    double                              GetTailReserveRate() const;
    void                                SetTailReserveRate(double dReserveRate);

    QString                             GetOpticalNameRule() const;
    void                                SetOpticalNameRule(const QString &strNameRule);
    QString                             GetTailNameRule() const;
    void                                SetTailNameRule(const QString &strNameRule);
    QString                             GetJumpNameRule() const;
    void                                SetJumpNameRule(const QString &strNameRule);

    QString                             GetOdfNameRule() const;
    void                                SetOdfNameRule(const QString &strNameRule);
    int                                 GetOdfLayerPortNumber() const;
    void                                SetOdfLayerPortNumber(int iOdfLayerPortNumber);

    bool                                ReadSettings(ProjectExplorer::PeProjectVersion *pProjectVersion);
    bool                                SaveSettings(ProjectExplorer::PeProjectVersion *pProjectVersion);

    bool                                Clear(ProjectExplorer::PeProjectVersion *pProjectVersion);
    bool                                Generator(ProjectExplorer::PeProjectVersion *pProjectVersion);

    bool                                AdjustOpticalCableSize(ProjectExplorer::PeCable *pCable, int iNewFiberNumber);
    bool                                AdjustTailCableSize(ProjectExplorer::PeCable *pCable, int iNewFiberNumber);
    bool                                MergeOpticalCables(const QList<ProjectExplorer::PeCable*> &lstCables);
    bool                                MergeTailCables(const QList<ProjectExplorer::PeCable*> &lstCables);
    bool                                SplitOpticalCable(const QList<ProjectExplorer::PeFiber*> &lstFibers);
    bool                                SplitTailCable(const QList<ProjectExplorer::PeFiber*> &lstFibers);
    bool                                AdjustFiberIndex(ProjectExplorer::PeFiber *pFiber, int iIndex);
    bool                                ExchangeFiber(ProjectExplorer::PeFiber *pFiber1, ProjectExplorer::PeFiber *pFiber2);
    bool                                AdjustOdfPortIndex(ProjectExplorer::PePort *pPort, int iIndex);
    bool                                ExchangeOdfPort(ProjectExplorer::PePort *pPort1, ProjectExplorer::PePort *pPort2);

    bool                                CreateOdfLayer(const ProjectExplorer::PeBoard &layer, int iLayerPortNumber, int iFiberPlug);
    bool                                UpdateOdfLayer(const ProjectExplorer::PeBoard &layer, int iFiberPlug);
    bool                                AdjustOdfLayerPortNumber(ProjectExplorer::PeDevice *pOdf, int iLayerPortNumber);

private:
    void                                PreparCopyValidation();
    void                                PrepareCubicleInfoSets();
    void                                PrepareCableRaw();
    void                                PrepareCableRawReuse();
    void                                PrepareCableReserve();
    void                                PrepareOdf();
    void                                PrepareAllCables();
    bool                                BuildAll();
    void                                ClearBuild();

    bool                                ValidateInfoSetPath(QList<ProjectExplorer::PeFiber*> &lstFibers, ProjectExplorer::PeCubicle *pInfoSetCubicle1, ProjectExplorer::PeCubicle *pInfoSetCubicle2, ProjectExplorer::PePort *pInfoSetPort1, ProjectExplorer::PePort *pInfoSetPort2);
    bool                                FindValidInfoSetPath(QList<ProjectExplorer::PeFiber*> &lstFibers, ProjectExplorer::PePort *pInfoSetPort1, ProjectExplorer::PePort *pInfoSetPort2);
    QList<ProjectExplorer::PeFiber*>    FindValidFibersFromPort(ProjectExplorer::PePort *pPort, const QList<ProjectExplorer::PeFiber*> &lstAllFibers);
    bool                                CheckFiberReuse(_Fiber *pFiber, ProjectExplorer::PeFiber *pFiberValid, bool bCheckSet);

    void                                AddCubicleInfoSet(ProjectExplorer::PeCubicle *pCubicle1, ProjectExplorer::PeCubicle *pCubicle2, ProjectExplorer::PeInfoSet *pInfoSet, ProjectExplorer::PePort *pPort1, ProjectExplorer::PePort *pPort2);
    _CubicleInfoSet*                    FindCubicleInfoSet(ProjectExplorer::PeCubicle *pCubicle1, ProjectExplorer::PeCubicle *pCubicle2);

    _Cable*                             FindCableOptical(ProjectExplorer::PeCubicle *pCubicle1, ProjectExplorer::PeCubicle *pCubicle2, const QString &strSet);
    _Cable*                             FindCableTail(ProjectExplorer::PeCubicle *pCubicle1, ProjectExplorer::PeCubicle *pCubicle2, const QString &strSet);
    _Cable*                             FindCableJump(ProjectExplorer::PeCubicle *pCubicle);
    _Fiber*                             FindPairFiber(_Fiber *pFiber);

    int                                 CalculateClosestFiberNumber(int iFiberNumber, bool bOptical) const;
    int                                 CalculatePipeNumber(int iFiberNumber) const;

    ProjectExplorer::PeDevice*          FindValidOdf(ProjectExplorer::PeCubicle *pCubicle, const QString &strSet, int &iOdfLayerPortNumber, int iUnusedOdfPortNumber = 0,  bool bCreate = true);

    void                                CalculateOpticalCableName(_Cable *pCableTemp, ProjectExplorer::PeCable *pCable);
    void                                CalculateTailCableName(_Cable *pCableTemp, ProjectExplorer::PeCable *pCable);
    void                                CalculateJumpCableName(_Cable *pCableTemp, ProjectExplorer::PeCable *pCable);

    bool                                GenerateReserveFiberAndOdf(int iAddedFiberNumber,
                                                                   ProjectExplorer::PeCable *pCable,
                                                                   QList<ProjectExplorer::PeFiber> &lstAddedFibers,
                                                                   QList<ProjectExplorer::PeBoard> &lstAddedOdfLayers1,
                                                                   QList<ProjectExplorer::PePort> &lstAddedOdfPorts1,
                                                                   QList<ProjectExplorer::PeBoard> &lstAddedOdfLayers2,
                                                                   QList<ProjectExplorer::PePort> &lstAddedOdfPorts2);

// Properties
private:
    static CableGenerator               *m_pInstance;
    GroupType                           m_eOpticalGroupType;
    QList<int>                          m_lstOpticalFiberNumbers;
    ReserveType                         m_eOpticalReserveType;
    double                              m_dOpticalReserveRate;

    GroupType                           m_eTailGroupType;
    QList<int>                          m_lstTailFiberNumbers;
    ReserveType                         m_eTailReserveType;
    double                              m_dTailReserveRate;

    QString                             m_strOpticalNameRule;
    QString                             m_strTailNameRule;
    QString                             m_strJumpNameRule;

    QString                             m_strOdfNameRule;
    int                                 m_iOdfLayerPortNumber;

    ProjectExplorer::PeProjectVersion   *m_pProjectVersion;

    QList<ProjectExplorer::PeCable*>    m_lstValidCables;
    QList<ProjectExplorer::PeInfoSet*>  m_lstValidInfoSets;
    QList<ProjectExplorer::PeDevice*>   m_lstValidOdfs;

    QList<_CubicleInfoSet*>             m_lstCubicleInfoSets;
    QList<_Cable*>                      m_lstOpticals;
    QList<_Cable*>                      m_lstTails;
    QList<_Cable*>                      m_lstJumps;
};

} // namespace Internal
} // namespace PhyNetwork

#endif // CABLEGENERATOR_H
