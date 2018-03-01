#include <QPair>
#include <QApplication>
#include <QSettings>
#include <QDebug>

#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pecubicleconn.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/peinfoset.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/peport.h"
#include "projectexplorer/pecable.h"
#include "projectexplorer/pefiber.h"
#include "core/mainwindow.h"

#include "cablegenerator.h"

static const char * const g_szSettings_OpticalGroupType     = "CableRule/OpticalGroupType";
static const char * const g_szSettings_OpticalFiberNumbers  = "CableRule/OpticalFiberNumbers";
static const char * const g_szSettings_OpticalReserveRate   = "CableRule/OpticalReserveRate";
static const char * const g_szSettings_OpticalReserveType   = "CableRule/OpticalReserveType";
static const char * const g_szSettings_TailGroupType        = "CableRule/TailGroupType";
static const char * const g_szSettings_TailFiberNumbers     = "CableRule/TailFiberNumbers";
static const char * const g_szSettings_TailReserveRate      = "CableRule/TailReserveRate";
static const char * const g_szSettings_TailReserveType      = "CableRule/TailReserveType";
static const char * const g_szSettings_OpticalNameRule      = "CableRule/OpticalNameRule";
static const char * const g_szSettings_TailNameRule         = "CableRule/TailNameRule";
static const char * const g_szSettings_JumpNameRule         = "CableRule/JumpNameRule";
static const char * const g_szSettings_OdfNameRule          = "CableRule/OdfNameRule";
static const char * const g_szSettings_OdfLayerPortNumber   = "CableRule/OdfLayerPortNumber";

static int g_iOdfLayerPortNum = 12;
static QList<int> g_lstPipeNumber = QList<int>() << 6 << 4;

using namespace PhyNetwork::Internal;

CableGenerator* CableGenerator::m_pInstance = 0;
CableGenerator::CableGenerator(QObject *pParent) : QObject(pParent), m_pProjectVersion(0)
{
    CableGenerator::m_pInstance = this;
}

CableGenerator::~CableGenerator()
{
    ClearBuild();
}

CableGenerator* CableGenerator::Instance()
{
    return m_pInstance;
}

CableGenerator::GroupType CableGenerator::GetOpticalGroupType () const
{
    return m_eOpticalGroupType;
}

void CableGenerator::SetOpticalGroupType(GroupType eGroupType)
{
    m_eOpticalGroupType = eGroupType;
}

QList<int> CableGenerator::GetOpticalFiberNumbers() const
{
    return m_lstOpticalFiberNumbers;
}

void CableGenerator::SetOpticalFiberNumbers(const QList<int> &lstFiberNumbers)
{
    m_lstOpticalFiberNumbers = lstFiberNumbers;
    qSort(m_lstOpticalFiberNumbers);
}

CableGenerator::ReserveType CableGenerator::GetOpticalReserveType() const
{
    return m_eOpticalReserveType;
}

void CableGenerator::SetOpticalReserveType(ReserveType eReserveType)
{
    m_eOpticalReserveType = eReserveType;
}

double CableGenerator::GetOpticalReserveRate() const
{
    return m_dOpticalReserveRate;
}

void CableGenerator::SetOpticalReserveRate(double dReserveRate)
{
    m_dOpticalReserveRate = dReserveRate;
}

CableGenerator::GroupType CableGenerator::GetTailGroupType () const
{
    return m_eTailGroupType;
}

void CableGenerator::SetTailGroupType(GroupType eGroupType)
{
    m_eTailGroupType = eGroupType;
}

QList<int> CableGenerator::GetTailFiberNumbers() const
{
    return m_lstTailFiberNumbers;
}

void CableGenerator::SetTailFiberNumbers(const QList<int> &lstFiberNumbers)
{
    m_lstTailFiberNumbers = lstFiberNumbers;
    qSort(m_lstTailFiberNumbers);
}

CableGenerator::ReserveType CableGenerator::GetTailReserveType() const
{
    return m_eTailReserveType;
}

void CableGenerator::SetTailReserveType(ReserveType eReserveType)
{
    m_eTailReserveType = eReserveType;
}

double CableGenerator::GetTailReserveRate() const
{
    return m_dTailReserveRate;
}

void CableGenerator::SetTailReserveRate(double dReserveRate)
{
    m_dTailReserveRate = dReserveRate;
}

QString CableGenerator::GetOpticalNameRule() const
{
    return m_strOpticalNameRule;
}

void CableGenerator::SetOpticalNameRule(const QString &strNameRule)
{
    m_strOpticalNameRule = strNameRule;
}

QString CableGenerator::GetTailNameRule() const
{
    return m_strTailNameRule;
}

void CableGenerator::SetTailNameRule(const QString &strNameRule)
{
    m_strTailNameRule = strNameRule;
}

QString CableGenerator::GetJumpNameRule() const
{
    return m_strJumpNameRule;
}

void CableGenerator::SetJumpNameRule(const QString &strNameRule)
{
    m_strJumpNameRule = strNameRule;
}

QString CableGenerator::GetOdfNameRule() const
{
    return m_strOdfNameRule;
}

void CableGenerator::SetOdfNameRule(const QString &strNameRule)
{
    m_strOdfNameRule = strNameRule;
}

int CableGenerator::GetOdfLayerPortNumber() const
{
    return m_iOdfLayerPortNumber;
}

void CableGenerator::SetOdfLayerPortNumber(int iOdfLayerPortNumber)
{
    m_iOdfLayerPortNumber = iOdfLayerPortNumber;
}

bool CableGenerator::ReadSettings(ProjectExplorer::PeProjectVersion *pProjectVersion)
{
    QMap<QString, QString> mapSettings;
    if(!pProjectVersion->DbReadSettings(mapSettings))
        return false;

    // OpticalGroupType
    m_eOpticalGroupType = GroupType(mapSettings.value(QLatin1String(g_szSettings_OpticalGroupType), "0").toInt());

    // OpticalFiberNumbers
    QString strOpticalFiberNumbers = mapSettings.value(QLatin1String(g_szSettings_OpticalFiberNumbers));
    if(strOpticalFiberNumbers.isEmpty())
    {
        m_lstOpticalFiberNumbers = QList<int>() << 4 << 8 << 12 << 24;
    }
    else
    {
        m_lstOpticalFiberNumbers.clear();
        foreach(const QString &strValue, strOpticalFiberNumbers.split(QLatin1Char(',')))
            m_lstOpticalFiberNumbers.append(strValue.toInt());

        qSort(m_lstOpticalFiberNumbers);
    }

    // OpticalReserveType
    m_eOpticalReserveType = ReserveType(mapSettings.value(QLatin1String(g_szSettings_OpticalReserveType), "0").toInt());

    // OpticalReserveRate
    m_dOpticalReserveRate = mapSettings.value(QLatin1String(g_szSettings_OpticalReserveRate), "0.2").toDouble();

    // TailGroupType
    m_eTailGroupType = GroupType(mapSettings.value(QLatin1String(g_szSettings_TailGroupType), "0").toInt());

    // TailFiberNumbers
    QString strTailFiberNumbers = mapSettings.value(QLatin1String(g_szSettings_TailFiberNumbers));
    if(strTailFiberNumbers.isEmpty())
    {
        m_lstTailFiberNumbers = QList<int>() << 4 << 8 << 12 << 24;
    }
    else
    {
        m_lstTailFiberNumbers.clear();
        foreach(const QString &strValue, strTailFiberNumbers.split(QLatin1Char(',')))
            m_lstTailFiberNumbers.append(strValue.toInt());

        qSort(m_lstTailFiberNumbers);
    }

    // TailReserveType
    m_eTailReserveType = ReserveType(mapSettings.value(QLatin1String(g_szSettings_TailReserveType), "0").toInt());

    // TailReserveRate
    m_dTailReserveRate = mapSettings.value(QLatin1String(g_szSettings_TailReserveRate), "0.2").toDouble();

    // OpticalNameRule
    m_strOpticalNameRule = mapSettings.value(QLatin1String(g_szSettings_OpticalNameRule), "%{BayNumber}-GL%{Index}%{Set}");

    // TailNameRule
    m_strTailNameRule = mapSettings.value(QLatin1String(g_szSettings_TailNameRule), "%{BayNumber}-WL%{Index}%{Set}");

    // JumpNameRule
    m_strJumpNameRule = mapSettings.value(QLatin1String(g_szSettings_JumpNameRule), "TX%{Index}");

    // OdfNameRule
    m_strOdfNameRule = mapSettings.value(QLatin1String(g_szSettings_OdfNameRule), "ODF-%{Set}");

    // OdfLayerPortNumber
    m_iOdfLayerPortNumber = mapSettings.value(QLatin1String(g_szSettings_OdfLayerPortNumber), "12").toInt();

    return true;
}

bool CableGenerator::SaveSettings(ProjectExplorer::PeProjectVersion *pProjectVersion)
{
    QMap<QString, QString> mapSettings;
    if(!pProjectVersion->DbReadSettings(mapSettings))
        return false;

    // OpticalGroupType
    mapSettings[g_szSettings_OpticalGroupType] = QString::number(m_eOpticalGroupType);

    // OpticalFiberNumbers
    QStringList lstValues;
    foreach(int iValue, m_lstOpticalFiberNumbers)
        lstValues.append(QString::number(iValue));
    mapSettings[g_szSettings_OpticalFiberNumbers] = lstValues.join(QLatin1String(","));

    // OpticalReserveType
    mapSettings[g_szSettings_OpticalReserveType] = QString::number(m_eOpticalReserveType);

    // OpticalReserveRate
    mapSettings[g_szSettings_OpticalReserveRate] = QString::number(m_dOpticalReserveRate);

    // TailGroupType
    mapSettings[g_szSettings_TailGroupType] = QString::number(m_eTailGroupType);

    // TailFiberNumbers
    lstValues.clear();
    foreach(int iValue, m_lstTailFiberNumbers)
        lstValues.append(QString::number(iValue));
    mapSettings[g_szSettings_TailFiberNumbers] = lstValues.join(QLatin1String(","));

    // TailReserveType
    mapSettings[g_szSettings_TailReserveType] = QString::number(m_eTailReserveType);

    // TailReserveRate
    mapSettings[g_szSettings_TailReserveRate] = QString::number(m_dTailReserveRate);

    // OpticalNameRule
    mapSettings[g_szSettings_OpticalNameRule] = m_strOpticalNameRule;

    // TailNameRule
    mapSettings[g_szSettings_TailNameRule] = m_strTailNameRule;

    // JumpNameRule
    mapSettings[g_szSettings_JumpNameRule] = m_strJumpNameRule;

    // OdfNameRule
    mapSettings[g_szSettings_OdfNameRule] = m_strOdfNameRule;

    // OdfLayerPortNumber
    mapSettings[g_szSettings_OdfLayerPortNumber] = QString::number(m_iOdfLayerPortNumber);

    return pProjectVersion->DbUpdateSettings(mapSettings, true);
}

bool CableGenerator::Clear(ProjectExplorer::PeProjectVersion *pProjectVersion)
{
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

    foreach(ProjectExplorer::PeCable *pCable, pProjectVersion->GetAllCables())
    {
        if(!pProjectVersion->DbDeleteObject(pCable->GetObjectType(), pCable->GetId(), false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
    {
        if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
            continue;

        if(!pProjectVersion->DbDeleteObject(pDevice->GetObjectType(), pDevice->GetId(), false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    if(!DbTrans.Commit())
        return false;

    foreach(ProjectExplorer::PeCable *pCable, pProjectVersion->GetAllCables())
    {
        pProjectVersion->DeleteObject(pCable);
    }

    foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
    {
        if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
            pProjectVersion->DeleteObject(pDevice);
    }

    return true;
}

bool CableGenerator::Generator(ProjectExplorer::PeProjectVersion *pProjectVersion)
{
    ClearBuild();

    m_pProjectVersion = pProjectVersion;

    PreparCopyValidation();
    PrepareCubicleInfoSets();
    PrepareCableRaw();
    PrepareCableRawReuse();
    PrepareCableReserve();
    PrepareOdf();
    PrepareAllCables();
    bool bRet = BuildAll();

    ClearBuild();
    return bRet;
}

bool CableGenerator::AdjustOpticalCableSize(ProjectExplorer::PeCable *pCable, int iNewFiberNumber)
{
    if(!pCable || iNewFiberNumber <= 0)
        return false;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pCable->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    QList<ProjectExplorer::PeFiber*> lstOldFibers = pCable->GetChildFibers();
    qSort(lstOldFibers.begin(), lstOldFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

    QList<ProjectExplorer::PeFiber*> lstOldReserveFibers;
    foreach(ProjectExplorer::PeFiber *pFiber, lstOldFibers)
    {
        if(pFiber->GetReserve())
            lstOldReserveFibers.append(pFiber);
    }
    qSort(lstOldReserveFibers.begin(), lstOldReserveFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

    const int iOldFiberNumber = lstOldFibers.size();
    const int iOldRealFiberNumber = iOldFiberNumber - lstOldReserveFibers.size();

    if(iOldFiberNumber == iNewFiberNumber)
        return true;

    if(iOldRealFiberNumber > iNewFiberNumber)
        return false;

    const int iPipeNumber = CalculatePipeNumber(iNewFiberNumber);
    if(iNewFiberNumber < iOldFiberNumber )
    {
        // Prepare updated fibers
        int iIndex = 0;
        QList<ProjectExplorer::PeFiber> lstUpdatedFibers;
        for(int i = 0; i < iNewFiberNumber; i++)
        {
            ProjectExplorer::PeFiber fiber(*lstOldFibers.at(i));
            fiber.SetIndex(iIndex + 1);
            fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iIndex % iPipeNumber));
            fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iIndex / iPipeNumber));
            lstUpdatedFibers.append(fiber);
            iIndex++;
        }

        // Prepare unused fibers
        const int iUnusedFiberNumber = iOldFiberNumber - iNewFiberNumber;
        QList<ProjectExplorer::PeFiber*> lstUnusedFibers;
        QList<ProjectExplorer::PeBoard*> lstUpdatedOdfLayers;
        for(int i = 0; i < iUnusedFiberNumber; i++)
        {
            ProjectExplorer::PeFiber *pFiber = lstOldReserveFibers.takeLast();
            ProjectExplorer::PeBoard *pOdfLayer1 = pFiber->GetPort1() ? pFiber->GetPort1()->GetParentBoard() : 0;
            ProjectExplorer::PeBoard *pOdfLayer2 = pFiber->GetPort2() ? pFiber->GetPort2()->GetParentBoard() : 0;

            lstUnusedFibers.append(pFiber);

            if(pOdfLayer1 && !lstUpdatedOdfLayers.contains(pOdfLayer1))
                lstUpdatedOdfLayers.append(pOdfLayer1);
            if(pOdfLayer2 && !lstUpdatedOdfLayers.contains(pOdfLayer2))
                lstUpdatedOdfLayers.append(pOdfLayer2);
        }

        // Handle Database
        ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

        foreach(ProjectExplorer::PeFiber *pFiber, lstUnusedFibers)
        {
            if(!pProjectVersion->DbDeleteObject(pFiber->GetObjectType(), pFiber->GetId(), false))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->DbUpdateObject(fiber, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        if(!DbTrans.Commit())
            return false;

        foreach(ProjectExplorer::PeFiber *pFiber, lstUnusedFibers)
        {
            if(!pProjectVersion->DeleteObject(pFiber))
                return false;
        }
        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->UpdateObject(fiber))
                return false;
        }
    }
    else
    {
        // Prepare updated fibers
        int iIndex = 0;
        QList<ProjectExplorer::PeFiber> lstUpdatedFibers;
        foreach(ProjectExplorer::PeFiber *pFiber, lstOldFibers)
        {
            ProjectExplorer::PeFiber fiber(*pFiber);
            fiber.SetIndex(iIndex + 1);
            fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iIndex % iPipeNumber));
            fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iIndex / iPipeNumber));
            lstUpdatedFibers.append(fiber);
            iIndex++;
        }

        // Prepare added fibers
        const int iAddedFiberNumber = iNewFiberNumber - iOldFiberNumber;
        QList<ProjectExplorer::PeFiber> lstAddedFibers;
        QList<ProjectExplorer::PeBoard> lstAddedOdfLayers1, lstAddedOdfLayers2;
        QList<ProjectExplorer::PePort> lstAddedOdfPorts1, lstAddedOdfPorts2;

        if(iAddedFiberNumber > 0)
        {
            if(!GenerateReserveFiberAndOdf(iAddedFiberNumber,
                                           pCable,
                                           lstAddedFibers,
                                           lstAddedOdfLayers1,
                                           lstAddedOdfPorts1,
                                           lstAddedOdfLayers2,
                                           lstAddedOdfPorts2))
            {
                return false;
            }

            for(int i = 0; i < lstAddedFibers.size(); i++)
            {
                ProjectExplorer::PeFiber &fiber = lstAddedFibers[i];
                fiber.SetIndex(iIndex + i + 1);
                fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor((iIndex + i) % iPipeNumber));
                fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor((iIndex + i) / iPipeNumber));
                fiber.SetParentCable(pCable);
            }
        }

        // Handle Database
        ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

        for(int i = 0; i < lstAddedOdfLayers1.size(); i++)
        {
            ProjectExplorer::PeBoard &layer = lstAddedOdfLayers1[i];
            if(!pProjectVersion->DbCreateObject(layer, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        for(int i = 0; i < lstAddedOdfPorts1.size(); i++)
        {
            ProjectExplorer::PePort &port = lstAddedOdfPorts1[i];
            if(!pProjectVersion->DbCreateObject(port, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        for(int i = 0; i < lstAddedOdfLayers2.size(); i++)
        {
            ProjectExplorer::PeBoard &layer = lstAddedOdfLayers2[i];
            if(!pProjectVersion->DbCreateObject(layer, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        for(int i = 0; i < lstAddedOdfPorts2.size(); i++)
        {
            ProjectExplorer::PePort &port = lstAddedOdfPorts2[i];
            if(!pProjectVersion->DbCreateObject(port, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->DbUpdateObject(fiber, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        for(int i = 0; i < lstAddedFibers.size(); i++)
        {
            ProjectExplorer::PeFiber &fiber = lstAddedFibers[i];
            if(!pProjectVersion->DbCreateObject(fiber, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        if(!DbTrans.Commit())
            return false;

        // Handle Object
        foreach(const ProjectExplorer::PeBoard &layer, lstAddedOdfLayers1)
        {
            if(!pProjectVersion->CreateObject(layer))
                return false;
        }

        foreach(ProjectExplorer::PePort port, lstAddedOdfPorts1)
        {
            ProjectExplorer::PeBoard *pLayer = pProjectVersion->FindBoardById(port.GetParentBoard()->GetId());
            if(!pLayer)
                return false;

            port.SetParentBoard(pLayer);
            if(!pProjectVersion->CreateObject(port))
                return false;
        }

        foreach(const ProjectExplorer::PeBoard &layer, lstAddedOdfLayers2)
        {
            if(!pProjectVersion->CreateObject(layer))
                return false;
        }

        foreach(ProjectExplorer::PePort port, lstAddedOdfPorts2)
        {
            ProjectExplorer::PeBoard *pLayer = pProjectVersion->FindBoardById(port.GetParentBoard()->GetId());
            if(!pLayer)
                return false;

            port.SetParentBoard(pLayer);
            if(!pProjectVersion->CreateObject(port))
                return false;
        }

        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->UpdateObject(fiber))
                return false;
        }

        foreach(ProjectExplorer::PeFiber fiber, lstAddedFibers)
        {
            ProjectExplorer::PePort *pPort1 = pProjectVersion->FindPortById(fiber.GetPort1()->GetId());
            ProjectExplorer::PePort *pPort2 = pProjectVersion->FindPortById(fiber.GetPort2()->GetId());
            if(!pPort1 || !pPort2)
                return false;

            fiber.SetPort1(pPort1);
            fiber.SetPort2(pPort2);
            if(!pProjectVersion->CreateObject(fiber))
                return false;
        }
    }

    return true;
}

bool CableGenerator::AdjustTailCableSize(ProjectExplorer::PeCable *pCable, int iNewFiberNumber)
{
    if(!pCable || iNewFiberNumber <= 0)
        return false;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pCable->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    QList<ProjectExplorer::PeFiber*> lstOldFibers = pCable->GetChildFibers();
    qSort(lstOldFibers.begin(), lstOldFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

    QList<ProjectExplorer::PeFiber*> lstOldReserveFibers;
    foreach(ProjectExplorer::PeFiber *pFiber, lstOldFibers)
    {
        if(pFiber->GetReserve())
            lstOldReserveFibers.append(pFiber);
    }
    qSort(lstOldReserveFibers.begin(), lstOldReserveFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

    const int iOldFiberNumber = lstOldFibers.size();
    const int iOldRealFiberNumber = iOldFiberNumber - lstOldReserveFibers.size();

    if(iOldFiberNumber == iNewFiberNumber)
        return true;

    if(iOldRealFiberNumber > iNewFiberNumber)
        return false;

    const int iPipeNumber = CalculatePipeNumber(iNewFiberNumber);
    if(iNewFiberNumber < iOldFiberNumber )
    {
        // Prepare updated fibers
        int iIndex = 0;
        QList<ProjectExplorer::PeFiber> lstUpdatedFibers;
        for(int i = 0; i < iNewFiberNumber; i++)
        {
            ProjectExplorer::PeFiber fiber(*lstOldFibers.at(i));
            fiber.SetIndex(iIndex + 1);
            fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iIndex % iPipeNumber));
            fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iIndex / iPipeNumber));
            lstUpdatedFibers.append(fiber);
            iIndex++;
        }

        // Prepare unused fibers
        const int iUnusedFiberNumber = iOldFiberNumber - iNewFiberNumber;
        QList<ProjectExplorer::PeFiber*> lstUnusedFibers;
        for(int i = 0; i < iUnusedFiberNumber; i++)
            lstUnusedFibers.append(lstOldReserveFibers.takeLast());

        // Handle Database
        ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
        foreach(ProjectExplorer::PeFiber *pFiber, lstUnusedFibers)
        {
            if(!pProjectVersion->DbDeleteObject(pFiber->GetObjectType(), pFiber->GetId(), false))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->DbUpdateObject(fiber, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        if(!DbTrans.Commit())
            return false;

        foreach(ProjectExplorer::PeFiber *pFiber, lstUnusedFibers)
        {
            if(!pProjectVersion->DeleteObject(pFiber))
                return false;
        }
        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->UpdateObject(fiber))
                return false;
        }
    }
    else
    {
        // Prepare updated fibers
        int iIndex = 0;
        QList<ProjectExplorer::PeFiber> lstUpdatedFibers;
        foreach(ProjectExplorer::PeFiber *pFiber, lstOldFibers)
        {
            ProjectExplorer::PeFiber fiber(*pFiber);
            fiber.SetIndex(iIndex + 1);
            fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iIndex % iPipeNumber));
            fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iIndex / iPipeNumber));
            lstUpdatedFibers.append(fiber);
            iIndex++;
        }

        // Prepare added fibers
        const int iAddedFiberNumber = iNewFiberNumber - iOldFiberNumber;
        QList<ProjectExplorer::PeFiber> lstAddedFibers;
        for(int i = 0; i < iAddedFiberNumber; i++)
        {
            ProjectExplorer::PeFiber fiber(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            fiber.SetIndex(iIndex + i + 1);
            fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor((iIndex + i) % iPipeNumber));
            fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor((iIndex + i) / iPipeNumber));
            fiber.SetReserve(true);
            fiber.SetPort1(0);
            fiber.SetPort2(0);
            fiber.SetParentCable(pCable);
            lstAddedFibers.append(fiber);
        }

        // Handle Database
        ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->DbUpdateObject(fiber, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        for(int i = 0; i < lstAddedFibers.size(); i++)
        {
            ProjectExplorer::PeFiber &fiber = lstAddedFibers[i];
            if(!pProjectVersion->DbCreateObject(fiber, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        if(!DbTrans.Commit())
            return false;

        // Handle Object
        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->UpdateObject(fiber))
                return false;
        }
        foreach(ProjectExplorer::PeFiber fiber, lstAddedFibers)
        {
            if(!pProjectVersion->CreateObject(fiber))
                return false;
        }
    }

    return true;
}

bool CableGenerator::MergeOpticalCables(const QList<ProjectExplorer::PeCable*> &lstCables)
{
    if(lstCables.size() < 2)
        return false;

    ProjectExplorer::PeProjectVersion *pProjectVersion = lstCables.first()->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    QString strSetOfCable = lstCables.first()->GetNameSet();
    ProjectExplorer::PeCubicle *pCubicle1 = lstCables.first()->GetCubicle1();
    ProjectExplorer::PeCubicle *pCubicle2 = lstCables.first()->GetCubicle2();
    for(int i = 1; i < lstCables.size(); i++)
    {
        ProjectExplorer::PeCable *pCable = lstCables.at(i);
        if(strSetOfCable != pCable->GetNameSet())
        {
            return false;
        }

        if(!((pCable->GetCubicle1() == pCubicle1 && pCable->GetCubicle2() == pCubicle2) ||
             (pCable->GetCubicle1() == pCubicle2 && pCable->GetCubicle2() == pCubicle1)))
        {
            return false;
        }
    }

    int iTotalFiberNumber = 0, iTotalRealFiberNumber = 0;
    foreach(ProjectExplorer::PeCable *pCable, lstCables)
    {
        foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
        {
            iTotalFiberNumber++;
            if(!pFiber->GetReserve())
                iTotalRealFiberNumber++;
        }
    }

    const int iMaxFiberNumber = m_lstOpticalFiberNumbers.last();
    if(iTotalRealFiberNumber > iMaxFiberNumber)
        return false;

    QList<ProjectExplorer::PeCable*> lstMergeCables = lstCables;
    ProjectExplorer::PeCable *pTargetCable = lstMergeCables.takeFirst();

    if(iTotalFiberNumber <= iMaxFiberNumber)
    {
        const int iTargetFiberNumber = CalculateClosestFiberNumber(iTotalFiberNumber, true);
        const int iAddedFiberNumber = iTargetFiberNumber - iTotalFiberNumber;
        const int iPipeNumber = CalculatePipeNumber(iTargetFiberNumber);

        // Prepare updated fibers
        QList<ProjectExplorer::PeFiber*> lstAllRealFibers, lstAllReserveFibers;
        foreach(ProjectExplorer::PeCable *pCable, lstCables)
        {
            foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
            {
                if(pFiber->GetReserve())
                    lstAllReserveFibers.append(pFiber);
                else
                    lstAllRealFibers.append(pFiber);
            }
        }

        int iIndex = 0;
        QList<ProjectExplorer::PeFiber> lstUpdatedFibers;
        foreach(ProjectExplorer::PeFiber *pFiber, lstAllRealFibers + lstAllReserveFibers)
        {
            ProjectExplorer::PeFiber fiber(*pFiber);
            fiber.SetParentCable(pTargetCable);
            fiber.SetIndex(iIndex + 1);
            fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iIndex % iPipeNumber));
            fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iIndex / iPipeNumber));
            lstUpdatedFibers.append(fiber);
            iIndex++;
        }

        // Prepare added fibers
        QList<ProjectExplorer::PeFiber> lstAddedFibers;
        QList<ProjectExplorer::PeBoard> lstAddedOdfLayers1, lstAddedOdfLayers2;
        QList<ProjectExplorer::PePort> lstAddedOdfPorts1, lstAddedOdfPorts2;
        if(iAddedFiberNumber > 0)
        {
            if(!GenerateReserveFiberAndOdf(iAddedFiberNumber,
                                           pTargetCable,
                                           lstAddedFibers,
                                           lstAddedOdfLayers1,
                                           lstAddedOdfPorts1,
                                           lstAddedOdfLayers2,
                                           lstAddedOdfPorts2))
            {
                return false;
            }

            for(int i = 0; i < lstAddedFibers.size(); i++)
            {
                ProjectExplorer::PeFiber &fiber = lstAddedFibers[i];
                fiber.SetIndex(iIndex + i + 1);
                fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor((iIndex + i) % iPipeNumber));
                fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor((iIndex + i) / iPipeNumber));
                fiber.SetParentCable(pTargetCable);
            }
        }

        // Handle Database
        ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

        for(int i = 0; i < lstAddedOdfLayers1.size(); i++)
        {
            ProjectExplorer::PeBoard &layer = lstAddedOdfLayers1[i];
            if(!pProjectVersion->DbCreateObject(layer, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        for(int i = 0; i < lstAddedOdfPorts1.size(); i++)
        {
            ProjectExplorer::PePort &port = lstAddedOdfPorts1[i];
            if(!pProjectVersion->DbCreateObject(port, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        for(int i = 0; i < lstAddedOdfLayers2.size(); i++)
        {
            ProjectExplorer::PeBoard &layer = lstAddedOdfLayers2[i];
            if(!pProjectVersion->DbCreateObject(layer, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        for(int i = 0; i < lstAddedOdfPorts2.size(); i++)
        {
            ProjectExplorer::PePort &port = lstAddedOdfPorts2[i];
            if(!pProjectVersion->DbCreateObject(port, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->DbUpdateObject(fiber, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        foreach(ProjectExplorer::PeCable *pCable, lstMergeCables)
        {
            if(!pProjectVersion->DbDeleteObject(pCable->GetObjectType(), pCable->GetId(), false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        for(int i = 0; i < lstAddedFibers.size(); i++)
        {
            ProjectExplorer::PeFiber &fiber = lstAddedFibers[i];
            if(!pProjectVersion->DbCreateObject(fiber, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        if(!DbTrans.Commit())
            return false;

        // Handle Object
        foreach(const ProjectExplorer::PeBoard &layer, lstAddedOdfLayers1)
        {
            if(!pProjectVersion->CreateObject(layer))
                return false;
        }

        foreach(ProjectExplorer::PePort port, lstAddedOdfPorts1)
        {
            ProjectExplorer::PeBoard *pLayer = pProjectVersion->FindBoardById(port.GetParentBoard()->GetId());
            if(!pLayer)
                return false;

            port.SetParentBoard(pLayer);
            if(!pProjectVersion->CreateObject(port))
                return false;
        }

        foreach(const ProjectExplorer::PeBoard &layer, lstAddedOdfLayers2)
        {
            if(!pProjectVersion->CreateObject(layer))
                return false;
        }

        foreach(ProjectExplorer::PePort port, lstAddedOdfPorts2)
        {
            ProjectExplorer::PeBoard *pLayer = pProjectVersion->FindBoardById(port.GetParentBoard()->GetId());
            if(!pLayer)
                return false;

            port.SetParentBoard(pLayer);
            if(!pProjectVersion->CreateObject(port))
                return false;
        }

        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->UpdateObject(fiber))
                return false;
        }

        foreach(ProjectExplorer::PeCable *pCable, lstMergeCables)
        {
            if(!pProjectVersion->DeleteObject(pCable))
                return false;
        }

        foreach(ProjectExplorer::PeFiber fiber, lstAddedFibers)
        {
            ProjectExplorer::PePort *pPort1 = pProjectVersion->FindPortById(fiber.GetPort1()->GetId());
            ProjectExplorer::PePort *pPort2 = pProjectVersion->FindPortById(fiber.GetPort2()->GetId());
            if(!pPort1 || !pPort2)
                return false;

            fiber.SetPort1(pPort1);
            fiber.SetPort2(pPort2);
            if(!pProjectVersion->CreateObject(fiber))
                return false;
        }
    }
    else
    {
        const int iTargetFiberNumber = iMaxFiberNumber;
        const int iUnusedFiberNumber = iTotalFiberNumber - iTargetFiberNumber;
        const int iPipeNumber = CalculatePipeNumber(iTargetFiberNumber);

        // Prepare unused fibers
        QList<ProjectExplorer::PeFiber*> lstAllRealFibers, lstAllReserveFibers;
        foreach(ProjectExplorer::PeCable *pCable, lstCables)
        {
            foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
            {
                if(pFiber->GetReserve())
                    lstAllReserveFibers.append(pFiber);
                else
                    lstAllRealFibers.append(pFiber);
            }
        }

        QList<ProjectExplorer::PeFiber*> lstUnusedFibers;
        for(int i = 0; i < iUnusedFiberNumber; i++)
            lstUnusedFibers.append(lstAllReserveFibers.takeLast());

        // Prepare updated fibers
        int iIndex = 0;
        QList<ProjectExplorer::PeFiber> lstUpdatedFibers;
        foreach(ProjectExplorer::PeFiber *pFiber, lstAllRealFibers + lstAllReserveFibers)
        {
            ProjectExplorer::PeFiber fiber(*pFiber);
            fiber.SetParentCable(pTargetCable);
            fiber.SetIndex(iIndex + 1);
            fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iIndex % iPipeNumber));
            fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iIndex / iPipeNumber));
            lstUpdatedFibers.append(fiber);
            iIndex++;
        }

        // Handle Database
        ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->DbUpdateObject(fiber, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        foreach(ProjectExplorer::PeFiber *pFiber, lstUnusedFibers)
        {
            if(!pProjectVersion->DbDeleteObject(pFiber->GetObjectType(), pFiber->GetId(), false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        foreach(ProjectExplorer::PeCable *pCable, lstMergeCables)
        {
            if(!pProjectVersion->DbDeleteObject(pCable->GetObjectType(), pCable->GetId(), false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        if(!DbTrans.Commit())
            return false;

        // Handle Object
        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->UpdateObject(fiber))
                return false;
        }

        foreach(ProjectExplorer::PeFiber *pFiber, lstUnusedFibers)
        {
            if(!pProjectVersion->DeleteObject(pFiber))
                return false;
        }

        foreach(ProjectExplorer::PeCable *pCable, lstMergeCables)
        {
            if(!pProjectVersion->DeleteObject(pCable))
                return false;
        }
    }

    return true;
}

bool CableGenerator::MergeTailCables(const QList<ProjectExplorer::PeCable*> &lstCables)
{
    if(lstCables.size() < 2)
        return false;

    ProjectExplorer::PeProjectVersion *pProjectVersion = lstCables.first()->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    QString strSetOfCable = lstCables.first()->GetNameSet();
    ProjectExplorer::PeCubicle *pCubicle1 = lstCables.first()->GetCubicle1();
    ProjectExplorer::PeCubicle *pCubicle2 = lstCables.first()->GetCubicle2();
    for(int i = 1; i < lstCables.size(); i++)
    {
        ProjectExplorer::PeCable *pCable = lstCables.at(i);
        if(strSetOfCable != pCable->GetNameSet())
        {
            return false;
        }

        if(!((pCable->GetCubicle1() == pCubicle1 && pCable->GetCubicle2() == pCubicle2) ||
             (pCable->GetCubicle1() == pCubicle2 && pCable->GetCubicle2() == pCubicle1)))
        {
            return false;
        }
    }

    int iTotalFiberNumber = 0, iTotalRealFiberNumber = 0;
    foreach(ProjectExplorer::PeCable *pCable, lstCables)
    {
        foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
        {
            iTotalFiberNumber++;
            if(!pFiber->GetReserve())
                iTotalRealFiberNumber++;
        }
    }

    const int iMaxFiberNumber = m_lstTailFiberNumbers.last();
    if(iTotalRealFiberNumber > iMaxFiberNumber)
        return false;

    QList<ProjectExplorer::PeCable*> lstMergeCables = lstCables;
    ProjectExplorer::PeCable *pTargetCable = lstMergeCables.takeFirst();

    if(iTotalFiberNumber <= iMaxFiberNumber)
    {
        const int iTargetFiberNumber = CalculateClosestFiberNumber(iTotalFiberNumber, false);
        const int iAddedFiberNumber = iTargetFiberNumber - iTotalFiberNumber;
        const int iPipeNumber = CalculatePipeNumber(iTargetFiberNumber);

        // Prepare updated fibers
        QList<ProjectExplorer::PeFiber*> lstAllRealFibers, lstAllReserveFibers;
        foreach(ProjectExplorer::PeCable *pCable, lstCables)
        {
            foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
            {
                if(pFiber->GetReserve())
                    lstAllReserveFibers.append(pFiber);
                else
                    lstAllRealFibers.append(pFiber);
            }
        }

        int iIndex = 0;
        QList<ProjectExplorer::PeFiber> lstUpdatedFibers;
        foreach(ProjectExplorer::PeFiber *pFiber, lstAllRealFibers + lstAllReserveFibers)
        {
            ProjectExplorer::PeFiber fiber(*pFiber);
            fiber.SetParentCable(pTargetCable);
            fiber.SetIndex(iIndex + 1);
            fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iIndex % iPipeNumber));
            fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iIndex / iPipeNumber));
            lstUpdatedFibers.append(fiber);
            iIndex++;
        }

        // Prepare added fibers
        QList<ProjectExplorer::PeFiber> lstAddedFibers;
        for(int i = 0; i < iAddedFiberNumber; i++)
        {
            ProjectExplorer::PeFiber fiber(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            fiber.SetIndex(iIndex + i + 1);
            fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor((iIndex + i) % iPipeNumber));
            fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor((iIndex + i) / iPipeNumber));
            fiber.SetReserve(true);
            fiber.SetPort1(0);
            fiber.SetPort2(0);
            fiber.SetParentCable(pTargetCable);
            lstAddedFibers.append(fiber);
        }

        // Handle Database
        ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->DbUpdateObject(fiber, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        foreach(ProjectExplorer::PeCable *pCable, lstMergeCables)
        {
            if(!pProjectVersion->DbDeleteObject(pCable->GetObjectType(), pCable->GetId(), false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        for(int i = 0; i < lstAddedFibers.size(); i++)
        {
            ProjectExplorer::PeFiber &fiber = lstAddedFibers[i];
            if(!pProjectVersion->DbCreateObject(fiber, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        if(!DbTrans.Commit())
            return false;

        // Handle Object
        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->UpdateObject(fiber))
                return false;
        }

        foreach(ProjectExplorer::PeCable *pCable, lstMergeCables)
        {
            if(!pProjectVersion->DeleteObject(pCable))
                return false;
        }

        foreach(const ProjectExplorer::PeFiber &fiber, lstAddedFibers)
        {
            if(!pProjectVersion->CreateObject(fiber))
                return false;
        }
    }
    else
    {
        const int iTargetFiberNumber = iMaxFiberNumber;
        const int iUnusedFiberNumber = iTotalFiberNumber - iTargetFiberNumber;
        const int iPipeNumber = CalculatePipeNumber(iTargetFiberNumber);

        // Prepare unused fibers
        QList<ProjectExplorer::PeFiber*> lstAllRealFibers, lstAllReserveFibers;
        foreach(ProjectExplorer::PeCable *pCable, lstCables)
        {
            foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
            {
                if(pFiber->GetReserve())
                    lstAllReserveFibers.append(pFiber);
                else
                    lstAllRealFibers.append(pFiber);
            }
        }

        QList<ProjectExplorer::PeFiber*> lstUnusedFibers;
        for(int i = 0; i < iUnusedFiberNumber; i++)
            lstUnusedFibers.append(lstAllReserveFibers.takeLast());

        // Prepare updated fibers
        int iIndex = 0;
        QList<ProjectExplorer::PeFiber> lstUpdatedFibers;
        foreach(ProjectExplorer::PeFiber *pFiber, lstAllRealFibers + lstAllReserveFibers)
        {
            ProjectExplorer::PeFiber fiber(*pFiber);
            fiber.SetParentCable(pTargetCable);
            fiber.SetIndex(iIndex + 1);
            fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iIndex % iPipeNumber));
            fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iIndex / iPipeNumber));
            lstUpdatedFibers.append(fiber);
            iIndex++;
        }

        // Handle Database
        ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->DbUpdateObject(fiber, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        foreach(ProjectExplorer::PeCable *pCable, lstMergeCables)
        {
            if(!pProjectVersion->DbDeleteObject(pCable->GetObjectType(), pCable->GetId(), false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        if(!DbTrans.Commit())
            return false;

        // Handle Object
        foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
        {
            if(!pProjectVersion->UpdateObject(fiber))
                return false;
        }

        foreach(ProjectExplorer::PeCable *pCable, lstMergeCables)
        {
            if(!pProjectVersion->DeleteObject(pCable))
                return false;
        }
    }

    return true;
}

bool CableGenerator::SplitOpticalCable(const QList<ProjectExplorer::PeFiber*> &lstFibers)
{
    if(lstFibers.isEmpty())
        return false;

    ProjectExplorer::PeProjectVersion *pProjectVersion = lstFibers.first()->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    ProjectExplorer::PeCable *pCable = lstFibers.first()->GetParentCable();
    foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
    {
        if(pFiber->GetReserve())
            return false;

        if(pCable != pFiber->GetParentCable())
            return false;
    }

    if(lstFibers.size() > m_lstOpticalFiberNumbers.last())
        return false;

    QList<ProjectExplorer::PeFiber*> lstNewFibers = lstFibers;
    qSort(lstNewFibers.begin(), lstNewFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

    QList<ProjectExplorer::PeFiber*> lstOldFibers = pCable->GetChildFibers();
    qSort(lstOldFibers.begin(), lstOldFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

    foreach(ProjectExplorer::PeFiber *pFiber, lstOldFibers)
    {
        if(lstNewFibers.contains(pFiber))
            lstOldFibers.removeAll(pFiber);
    }

    const int iNewRealFiberNumber = lstNewFibers.size();
    int iNewTargetFiberNumber = int(double(iNewRealFiberNumber) * (1.0 + m_dOpticalReserveRate) + 0.5);
    iNewTargetFiberNumber = iNewTargetFiberNumber > m_lstOpticalFiberNumbers.last() ? m_lstOpticalFiberNumbers.last() : CalculateClosestFiberNumber(iNewTargetFiberNumber, true);

    QList<ProjectExplorer::PeFiber> lstAddedFibers;
    QList<ProjectExplorer::PeBoard> lstAddedOdfLayers1, lstAddedOdfLayers2;
    QList<ProjectExplorer::PePort> lstAddedOdfPorts1, lstAddedOdfPorts2;
    if(!GenerateReserveFiberAndOdf(iNewTargetFiberNumber,
                                   pCable,
                                   lstAddedFibers,
                                   lstAddedOdfLayers1,
                                   lstAddedOdfPorts1,
                                   lstAddedOdfLayers2,
                                   lstAddedOdfPorts2))
    {
        return false;
    }

    const int iOldPipeNumber = CalculatePipeNumber(pCable->GetChildFibers().size());
    const int iNewPipeNumber = CalculatePipeNumber(iNewTargetFiberNumber);

    // Prepare updated fiber for old cable
    int iOldIndex = 0;
    QList<ProjectExplorer::PeFiber> lstOldUpdatedFibers;
    foreach(ProjectExplorer::PeFiber *pFiber, lstOldFibers)
    {
        ProjectExplorer::PeFiber fiber(*pFiber);
        fiber.SetIndex(iOldIndex + 1);
        fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iOldIndex % iOldPipeNumber));
        fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iOldIndex / iOldPipeNumber));
        lstOldUpdatedFibers.append(fiber);
        iOldIndex++;
    }

    // Prepare added fiber for old cable
    for(int i = 0; i < lstNewFibers.size(); i++)
    {
        ProjectExplorer::PeFiber &fiber = lstAddedFibers[i];
        fiber.SetParentCable(pCable);
        fiber.SetIndex(iOldIndex + i + 1);
        fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor((iOldIndex + i) % iOldPipeNumber));
        fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor((iOldIndex + i) / iOldPipeNumber));
    }

    // Prepare added cable
    ProjectExplorer::PeCable AddedCable(*pCable);
    QStringList lstAllCablesNames;
    foreach(ProjectExplorer::PeCable *pC, pProjectVersion->GetAllCables())
        lstAllCablesNames.append(pC->GetName());

    for(int i = 0; i < 999; i++)
    {
        AddedCable.SetNameNumber(AddedCable.GetNameNumber() + 1);
        AddedCable.SetName(AddedCable.CalculateNameByRule(m_strOpticalNameRule));

        if(!lstAllCablesNames.contains(AddedCable.GetName()))
            break;
    }

    // Prepare updated fiber for new cable
    int iNewIndex = 0;
    QList<ProjectExplorer::PeFiber> lstNewUpdatedFibers;
    foreach(ProjectExplorer::PeFiber *pFiber, lstNewFibers)
    {
        ProjectExplorer::PeFiber fiber(*pFiber);
        fiber.SetParentCable(&AddedCable);
        fiber.SetIndex(iNewIndex + 1);
        fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iNewIndex % iNewPipeNumber));
        fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iNewIndex / iNewPipeNumber));
        lstNewUpdatedFibers.append(fiber);
        iNewIndex++;
    }

    // Prepare added fiber for new cable
    for(int i = lstNewFibers.size(); i < lstAddedFibers.size(); i++)
    {
        ProjectExplorer::PeFiber &fiber = lstAddedFibers[i];
        fiber.SetParentCable(&AddedCable);
        fiber.SetIndex(iNewIndex + 1);
        fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iNewIndex % iNewPipeNumber));
        fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iNewIndex / iNewPipeNumber));
        iNewIndex++;
    }

    // Handle Database
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

    for(int i = 0; i < lstAddedOdfLayers1.size(); i++)
    {
        ProjectExplorer::PeBoard &layer = lstAddedOdfLayers1[i];
        if(!pProjectVersion->DbCreateObject(layer, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    for(int i = 0; i < lstAddedOdfPorts1.size(); i++)
    {
        ProjectExplorer::PePort &port = lstAddedOdfPorts1[i];
        if(!pProjectVersion->DbCreateObject(port, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    for(int i = 0; i < lstAddedOdfLayers2.size(); i++)
    {
        ProjectExplorer::PeBoard &layer = lstAddedOdfLayers2[i];
        if(!pProjectVersion->DbCreateObject(layer, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    for(int i = 0; i < lstAddedOdfPorts2.size(); i++)
    {
        ProjectExplorer::PePort &port = lstAddedOdfPorts2[i];
        if(!pProjectVersion->DbCreateObject(port, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    foreach(const ProjectExplorer::PeFiber &fiber, lstOldUpdatedFibers)
    {
        if(!pProjectVersion->DbUpdateObject(fiber, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    if(!pProjectVersion->DbCreateObject(AddedCable, false))
    {
        DbTrans.Rollback();
        return false;
    }

    foreach(const ProjectExplorer::PeFiber &fiber, lstNewUpdatedFibers)
    {
        if(!pProjectVersion->DbUpdateObject(fiber, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    for(int i = 0; i < lstAddedFibers.size(); i++)
    {
        ProjectExplorer::PeFiber &fiber = lstAddedFibers[i];
        if(!pProjectVersion->DbCreateObject(fiber, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    if(!DbTrans.Commit())
        return false;

    // Handle Object
    foreach(const ProjectExplorer::PeBoard &layer, lstAddedOdfLayers1)
    {
        if(!pProjectVersion->CreateObject(layer))
            return false;
    }

    foreach(ProjectExplorer::PePort port, lstAddedOdfPorts1)
    {
        ProjectExplorer::PeBoard *pLayer = pProjectVersion->FindBoardById(port.GetParentBoard()->GetId());
        if(!pLayer)
            return false;

        port.SetParentBoard(pLayer);
        if(!pProjectVersion->CreateObject(port))
            return false;
    }

    foreach(const ProjectExplorer::PeBoard &layer, lstAddedOdfLayers2)
    {
        if(!pProjectVersion->CreateObject(layer))
            return false;
    }

    foreach(ProjectExplorer::PePort port, lstAddedOdfPorts2)
    {
        ProjectExplorer::PeBoard *pLayer = pProjectVersion->FindBoardById(port.GetParentBoard()->GetId());
        if(!pLayer)
            return false;

        port.SetParentBoard(pLayer);
        if(!pProjectVersion->CreateObject(port))
            return false;
    }

    foreach(const ProjectExplorer::PeFiber &fiber, lstOldUpdatedFibers)
    {
        if(!pProjectVersion->UpdateObject(fiber))
            return false;
    }

    if(!pProjectVersion->CreateObject(AddedCable))
        return false;

    foreach(ProjectExplorer::PeFiber fiber, lstNewUpdatedFibers)
    {
        ProjectExplorer::PeCable *pCable = pProjectVersion->FindCableById(fiber.GetParentCable()->GetId());
        if(!pCable)
            return false;

        fiber.SetParentCable(pCable);
        if(!pProjectVersion->UpdateObject(fiber))
            return false;
    }

    foreach(ProjectExplorer::PeFiber fiber, lstAddedFibers)
    {
        ProjectExplorer::PeCable *pCable = pProjectVersion->FindCableById(fiber.GetParentCable()->GetId());
        ProjectExplorer::PePort *pPort1 = pProjectVersion->FindPortById(fiber.GetPort1()->GetId());
        ProjectExplorer::PePort *pPort2 = pProjectVersion->FindPortById(fiber.GetPort2()->GetId());
        if(!pCable || !pPort1 || !pPort2)
            return false;

        fiber.SetParentCable(pCable);
        fiber.SetPort1(pPort1);
        fiber.SetPort2(pPort2);
        if(!pProjectVersion->CreateObject(fiber))
            return false;
    }

    return true;
}

bool CableGenerator::SplitTailCable(const QList<ProjectExplorer::PeFiber*> &lstFibers)
{
    if(lstFibers.isEmpty())
        return false;

    ProjectExplorer::PeProjectVersion *pProjectVersion = lstFibers.first()->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    ProjectExplorer::PeCable *pCable = lstFibers.first()->GetParentCable();
    foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
    {
        if(pFiber->GetReserve())
            return false;

        if(pCable != pFiber->GetParentCable())
            return false;
    }

    if(lstFibers.size() > m_lstOpticalFiberNumbers.last())
        return false;

    QList<ProjectExplorer::PeFiber*> lstNewFibers = lstFibers;
    qSort(lstNewFibers.begin(), lstNewFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

    QList<ProjectExplorer::PeFiber*> lstOldFibers = pCable->GetChildFibers();
    qSort(lstOldFibers.begin(), lstOldFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

    foreach(ProjectExplorer::PeFiber *pFiber, lstOldFibers)
    {
        if(lstNewFibers.contains(pFiber))
            lstOldFibers.removeAll(pFiber);
    }

    const int iNewRealFiberNumber = lstNewFibers.size();
    int iNewTargetFiberNumber = int(double(iNewRealFiberNumber) * (1.0 + m_dTailReserveRate) + 0.5);
    iNewTargetFiberNumber = iNewTargetFiberNumber > m_lstTailFiberNumbers.last() ? m_lstTailFiberNumbers.last() : CalculateClosestFiberNumber(iNewTargetFiberNumber, false);

    QList<ProjectExplorer::PeFiber> lstAddedFibers;
    for(int i = 0; i < iNewTargetFiberNumber; i++)
    {
        ProjectExplorer::PeFiber fiber(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        fiber.SetReserve(true);
        fiber.SetPort1(0);
        fiber.SetPort2(0);
        lstAddedFibers.append(fiber);
    }

    const int iOldPipeNumber = CalculatePipeNumber(pCable->GetChildFibers().size());
    const int iNewPipeNumber = CalculatePipeNumber(iNewTargetFiberNumber);

    // Prepare updated fiber for old cable
    int iOldIndex = 0;
    QList<ProjectExplorer::PeFiber> lstOldUpdatedFibers;
    foreach(ProjectExplorer::PeFiber *pFiber, lstOldFibers)
    {
        ProjectExplorer::PeFiber fiber(*pFiber);
        fiber.SetIndex(iOldIndex + 1);
        fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iOldIndex % iOldPipeNumber));
        fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iOldIndex / iOldPipeNumber));
        lstOldUpdatedFibers.append(fiber);
        iOldIndex++;
    }

    // Prepare added fiber for old cable
    for(int i = 0; i < lstNewFibers.size(); i++)
    {
        ProjectExplorer::PeFiber &fiber = lstAddedFibers[i];
        fiber.SetParentCable(pCable);
        fiber.SetIndex(iOldIndex + i + 1);
        fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor((iOldIndex + i) % iOldPipeNumber));
        fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor((iOldIndex + i) / iOldPipeNumber));
    }

    // Prepare added cable
    ProjectExplorer::PeCable AddedCable(*pCable);
    QStringList lstAllCablesNames;
    foreach(ProjectExplorer::PeCable *pC, pProjectVersion->GetAllCables())
        lstAllCablesNames.append(pC->GetName());

    for(int i = 0; i < 999; i++)
    {
        AddedCable.SetNameNumber(AddedCable.GetNameNumber() + 1);
        AddedCable.SetName(AddedCable.CalculateNameByRule(m_strTailNameRule));

        if(!lstAllCablesNames.contains(AddedCable.GetName()))
            break;
    }

    // Prepare updated fiber for new cable
    int iNewIndex = 0;
    QList<ProjectExplorer::PeFiber> lstNewUpdatedFibers;
    foreach(ProjectExplorer::PeFiber *pFiber, lstNewFibers)
    {
        ProjectExplorer::PeFiber fiber(*pFiber);
        fiber.SetParentCable(&AddedCable);
        fiber.SetIndex(iNewIndex + 1);
        fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iNewIndex % iNewPipeNumber));
        fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iNewIndex / iNewPipeNumber));
        lstNewUpdatedFibers.append(fiber);
        iNewIndex++;
    }

    // Prepare added fiber for new cable
    for(int i = lstNewFibers.size(); i < lstAddedFibers.size(); i++)
    {
        ProjectExplorer::PeFiber &fiber = lstAddedFibers[i];
        fiber.SetParentCable(&AddedCable);
        fiber.SetIndex(iNewIndex + 1);
        fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(iNewIndex % iNewPipeNumber));
        fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(iNewIndex / iNewPipeNumber));
        iNewIndex++;
    }

    // Handle Database
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

    foreach(const ProjectExplorer::PeFiber &fiber, lstOldUpdatedFibers)
    {
        if(!pProjectVersion->DbUpdateObject(fiber, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    if(!pProjectVersion->DbCreateObject(AddedCable, false))
    {
        DbTrans.Rollback();
        return false;
    }

    foreach(const ProjectExplorer::PeFiber &fiber, lstNewUpdatedFibers)
    {
        if(!pProjectVersion->DbUpdateObject(fiber, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    for(int i = 0; i < lstAddedFibers.size(); i++)
    {
        ProjectExplorer::PeFiber &fiber = lstAddedFibers[i];
        if(!pProjectVersion->DbCreateObject(fiber, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    if(!DbTrans.Commit())
        return false;

    // Handle Object
    foreach(const ProjectExplorer::PeFiber &fiber, lstOldUpdatedFibers)
    {
        if(!pProjectVersion->UpdateObject(fiber))
            return false;
    }

    if(!pProjectVersion->CreateObject(AddedCable))
        return false;

    foreach(ProjectExplorer::PeFiber fiber, lstNewUpdatedFibers)
    {
        ProjectExplorer::PeCable *pCable = pProjectVersion->FindCableById(fiber.GetParentCable()->GetId());
        if(!pCable)
            return false;

        fiber.SetParentCable(pCable);
        if(!pProjectVersion->UpdateObject(fiber))
            return false;
    }

    foreach(ProjectExplorer::PeFiber fiber, lstAddedFibers)
    {
        ProjectExplorer::PeCable *pCable = pProjectVersion->FindCableById(fiber.GetParentCable()->GetId());
        if(!pCable)
            return false;

        fiber.SetParentCable(pCable);
        if(!pProjectVersion->CreateObject(fiber))
            return false;
    }

    return true;
}

bool CableGenerator::AdjustFiberIndex(ProjectExplorer::PeFiber *pFiber, int iIndex)
{
    if(!pFiber)
        return false;

    ProjectExplorer::PeCable *pCable = pFiber->GetParentCable();
    if(!pCable)
        return false;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pCable->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    QList<ProjectExplorer::PeFiber*> lstFibers = pCable->GetChildFibers();
    qSort(lstFibers.begin(), lstFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

    int iOldIndex = lstFibers.indexOf(pFiber);
    if(iIndex == iOldIndex || iIndex < 0 || iIndex >= lstFibers.size())
        return false;

    lstFibers.move(iOldIndex, iIndex);

    const int iPipeNumber = CalculatePipeNumber(lstFibers.size());
    QList<ProjectExplorer::PeFiber> lstUpdatedFibers;
    for(int i = 0; i < lstFibers.size(); i++)
    {
        ProjectExplorer::PeFiber fiber(*lstFibers.at(i));
        fiber.SetIndex(i + 1);
        fiber.SetFiberColor(ProjectExplorer::PeFiber::FiberColor(i % iPipeNumber));
        fiber.SetPipeColor(ProjectExplorer::PeFiber::FiberColor(i / iPipeNumber));
        lstUpdatedFibers.append(fiber);
    }

    // Handle Database
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
    foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
    {
        if(!pProjectVersion->DbUpdateObject(fiber, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    if(!DbTrans.Commit())
        return false;

    // Handle Object
    foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
    {
        if(!pProjectVersion->UpdateObject(fiber))
            return false;
    }

    return true;
}

bool CableGenerator::ExchangeFiber(ProjectExplorer::PeFiber *pFiber1, ProjectExplorer::PeFiber *pFiber2)
{
    if(!pFiber1 || !pFiber2 || pFiber1 == pFiber2)
        return false;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pFiber1->GetProjectVersion();
    if(!pProjectVersion || pProjectVersion != pFiber2->GetProjectVersion())
        return false;

    if(!pFiber1->GetParentCable() || !pFiber2->GetParentCable())
        return false;

    if(pFiber1->GetParentCable()->GetCableType() != pFiber2->GetParentCable()->GetCableType())
        return false;

    ProjectExplorer::PeFiber fiberUpdate1(*pFiber1);
    fiberUpdate1.SetReserve(pFiber2->GetReserve());
    fiberUpdate1.SetInfoSetIds(pFiber2->GetInfoSetIds());
    if(pFiber1->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctOptical)
    {
        fiberUpdate1.SetPort1(pFiber2->GetPort1());
        fiberUpdate1.SetPort2(pFiber2->GetPort2());
    }

    ProjectExplorer::PeFiber fiberUpdate2(*pFiber2);
    fiberUpdate2.SetReserve(pFiber1->GetReserve());
    fiberUpdate2.SetInfoSetIds(pFiber1->GetInfoSetIds());
    if(pFiber2->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctOptical)
    {
        fiberUpdate2.SetPort1(pFiber1->GetPort1());
        fiberUpdate2.SetPort2(pFiber1->GetPort2());
    }

    QList<ProjectExplorer::PeFiber> lstUpdatedFibers;
    if(ProjectExplorer::PePort *pPort1Fiber1 = pFiber1->GetPort1())
    {
        foreach(ProjectExplorer::PeFiber *pUsedFiber, pPort1Fiber1->GetUsedFibers(QList<ProjectExplorer::PeFiber*>() << pFiber1))
        {
            ProjectExplorer::PeFiber fiber(*pUsedFiber);
            if(pUsedFiber->GetPort1() == pPort1Fiber1)
                fiber.SetPort1(pFiber2->GetPort1());
            else
                fiber.SetPort2(pFiber2->GetPort1());
            lstUpdatedFibers.append(fiber);
        }
    }
    if(ProjectExplorer::PePort *pPort2Fiber1 = pFiber1->GetPort2())
    {
        foreach(ProjectExplorer::PeFiber *pUsedFiber, pPort2Fiber1->GetUsedFibers(QList<ProjectExplorer::PeFiber*>() << pFiber1))
        {
            ProjectExplorer::PeFiber fiber(*pUsedFiber);
            if(pUsedFiber->GetPort1() == pPort2Fiber1)
                fiber.SetPort1(pFiber2->GetPort2());
            else
                fiber.SetPort2(pFiber2->GetPort2());
            lstUpdatedFibers.append(fiber);
        }
    }
    if(ProjectExplorer::PePort *pPort1Fiber2 = pFiber2->GetPort1())
    {
        foreach(ProjectExplorer::PeFiber *pUsedFiber, pPort1Fiber2->GetUsedFibers(QList<ProjectExplorer::PeFiber*>() << pFiber2))
        {
            ProjectExplorer::PeFiber fiber(*pUsedFiber);
            if(pUsedFiber->GetPort1() == pPort1Fiber2)
                fiber.SetPort1(pFiber1->GetPort1());
            else
                fiber.SetPort2(pFiber1->GetPort1());
            lstUpdatedFibers.append(fiber);
        }
    }
    if(ProjectExplorer::PePort *pPort2Fiber2 = pFiber2->GetPort2())
    {
        foreach(ProjectExplorer::PeFiber *pUsedFiber, pPort2Fiber2->GetUsedFibers(QList<ProjectExplorer::PeFiber*>() << pFiber2))
        {
            ProjectExplorer::PeFiber fiber(*pUsedFiber);
            if(pUsedFiber->GetPort1() == pPort2Fiber2)
                fiber.SetPort1(pFiber1->GetPort2());
            else
                fiber.SetPort2(pFiber1->GetPort2());
            lstUpdatedFibers.append(fiber);
        }
    }

    // Handle Database
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
    if(!pProjectVersion->DbUpdateObject(fiberUpdate1, false))
    {
        DbTrans.Rollback();
        return false;
    }
    if(!pProjectVersion->DbUpdateObject(fiberUpdate2, false))
    {
        DbTrans.Rollback();
        return false;
    }
    foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
    {
        if(!pProjectVersion->DbUpdateObject(fiber, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    if(!DbTrans.Commit())
        return false;

    // Handle Object
    if(!pProjectVersion->UpdateObject(fiberUpdate1))
        return false;
    if(!pProjectVersion->UpdateObject(fiberUpdate2))
        return false;
    foreach(const ProjectExplorer::PeFiber &fiber, lstUpdatedFibers)
    {
        if(!pProjectVersion->UpdateObject(fiber))
            return false;
    }

    return true;
}

bool CableGenerator::AdjustOdfPortIndex(ProjectExplorer::PePort *pPort, int iIndex)
{
    if(!pPort)
        return false;

    ProjectExplorer::PeBoard *pLayer = pPort->GetParentBoard();
    if(!pLayer)
        return false;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pLayer->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    QList<ProjectExplorer::PePort*> lstPorts = pLayer->GetChildPorts();
    qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    int iOldIndex = lstPorts.indexOf(pPort);
    if(iIndex == iOldIndex || iIndex < 0 || iIndex >= lstPorts.size())
        return false;

    lstPorts.move(iOldIndex, iIndex);

    QList<ProjectExplorer::PePort> lstUpdatedPorts;
    for(int i = 0; i < lstPorts.size(); i++)
    {
        ProjectExplorer::PePort port(*lstPorts.at(i));
        port.SetName(QString("%1%2").arg(pLayer->GetPosition()).arg(i + 1, 2, 10, QLatin1Char('0')));
        port.SetGroup(i + 1);
        lstUpdatedPorts.append(port);
    }

    // Handle Database
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
    foreach(const ProjectExplorer::PePort &port, lstUpdatedPorts)
    {
        if(!pProjectVersion->DbUpdateObject(port, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }
    if(!DbTrans.Commit())
        return false;

    // Handle Object
    foreach(const ProjectExplorer::PePort &port, lstUpdatedPorts)
    {
        if(!pProjectVersion->UpdateObject(port))
            return false;
    }

    return true;
}

bool CableGenerator::ExchangeOdfPort(ProjectExplorer::PePort *pPort1, ProjectExplorer::PePort *pPort2)
{
    if(!pPort1 || !pPort2 || pPort1 == pPort2)
        return false;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pPort1->GetProjectVersion();
    if(!pProjectVersion || pProjectVersion != pPort2->GetProjectVersion())
        return false;

    ProjectExplorer::PeBoard *pLayer1 = pPort1->GetParentBoard();
    ProjectExplorer::PeBoard *pLayer2 = pPort2->GetParentBoard();
    if(!pLayer1 || !pLayer2)
        return false;

    ProjectExplorer::PePort portUpdate1(*pPort1);
    portUpdate1.SetParentBoard(pPort2->GetParentBoard());
    portUpdate1.SetName(pPort2->GetName());
    portUpdate1.SetGroup(pPort2->GetGroup());
    portUpdate1.SetFiberPlug(pPort2->GetFiberPlug());

    ProjectExplorer::PePort portUpdate2(*pPort2);
    portUpdate2.SetParentBoard(pPort1->GetParentBoard());
    portUpdate2.SetName(pPort1->GetName());
    portUpdate2.SetGroup(pPort1->GetGroup());
    portUpdate2.SetFiberPlug(pPort1->GetFiberPlug());

    // Handle Database
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
    if(!pProjectVersion->DbUpdateObject(portUpdate1, false))
    {
        DbTrans.Rollback();
        return false;
    }
    if(!pProjectVersion->DbUpdateObject(portUpdate2, false))
    {
        DbTrans.Rollback();
        return false;
    }
    if(!DbTrans.Commit())
        return false;

    // Handle Object
    if(!pProjectVersion->UpdateObject(portUpdate1))
        return false;
    if(!pProjectVersion->UpdateObject(portUpdate2))
        return false;

    return true;
}

bool CableGenerator::CreateOdfLayer(const ProjectExplorer::PeBoard &layer, int iLayerPortNumber, int iFiberPlug)
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = layer.GetProjectVersion();
    if(!pProjectVersion)
        return false;

    ProjectExplorer::PeBoard *pLayerTemp = qobject_cast<ProjectExplorer::PeBoard*>(const_cast<ProjectExplorer::PeBoard *>(&layer));
    if(!pLayerTemp)
        return false;

    QList<ProjectExplorer::PePort> lstCreatedPorts;
    for(int i = 0; i < iLayerPortNumber; i++)
    {
        ProjectExplorer::PePort port(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        port.SetName(QString("%1%2").arg(layer.GetPosition()).arg(i + 1, 2, 10, QLatin1Char('0')));
        port.SetGroup(i + 1);
        port.SetPortType(ProjectExplorer::PePort::ptOdf);
        port.SetPortDirection(ProjectExplorer::PePort::pdRT);
        port.SetFiberPlug(ProjectExplorer::PePort::FiberPlug(iFiberPlug));
        port.SetFiberMode(ProjectExplorer::PePort::fmNone);
        port.SetParentBoard(pLayerTemp);
        lstCreatedPorts.append(port);
    }

    // Handle Database
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
    if(!pProjectVersion->DbCreateObject(*pLayerTemp, false))
    {
        DbTrans.Rollback();
        return false;
    }

    for(int i = 0; i < lstCreatedPorts.size(); i++)
    {
        ProjectExplorer::PePort &port = lstCreatedPorts[i];
        if(!pProjectVersion->DbCreateObject(port, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }
    if(!DbTrans.Commit())
        return false;

    // Handle Object
    ProjectExplorer::PeBoard *pLayer = qobject_cast<ProjectExplorer::PeBoard*>(pProjectVersion->CreateObject(layer));
    if(!pLayer)
        return false;

    foreach(ProjectExplorer::PePort port, lstCreatedPorts)
    {
        port.SetParentBoard(pLayer);
        if(!pProjectVersion->CreateObject(port))
            return false;
    }

    return true;
}

bool CableGenerator::UpdateOdfLayer(const ProjectExplorer::PeBoard &layer, int iFiberPlug)
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = layer.GetProjectVersion();
    if(!pProjectVersion)
        return false;

    ProjectExplorer::PeBoard *pLayer = pProjectVersion->FindBoardById(layer.GetId());
    if(!pLayer)
        return false;

    QList<ProjectExplorer::PePort> lstUpdatedPorts;
    QList<ProjectExplorer::PePort*> lstPorts = pLayer->GetChildPorts();
    qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
    for(int i = 0; i < lstPorts.size(); i++)
    {
        ProjectExplorer::PePort port(*lstPorts.at(i));
        port.SetName(QString("%1%2").arg(layer.GetPosition()).arg(i + 1, 2, 10, QLatin1Char('0')));
        port.SetGroup(i + 1);
        port.SetFiberPlug(ProjectExplorer::PePort::FiberPlug(iFiberPlug));
        lstUpdatedPorts.append(port);
    }

    // Handle Database
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
    if(!pProjectVersion->DbUpdateObject(layer, false))
    {
        DbTrans.Rollback();
        return false;
    }

    foreach(const ProjectExplorer::PePort &port, lstUpdatedPorts)
    {
        if(!pProjectVersion->DbUpdateObject(port, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }
    if(!DbTrans.Commit())
        return false;

    // Handle Object
    if(!pProjectVersion->UpdateObject(layer))
        return false;

    foreach(const ProjectExplorer::PePort &port, lstUpdatedPorts)
    {
        if(!pProjectVersion->UpdateObject(port))
            return false;
    }

    return true;
}

bool CableGenerator::AdjustOdfLayerPortNumber(ProjectExplorer::PeDevice *pOdf, int iLayerPortNumber)
{
    if(!pOdf || iLayerPortNumber <= 0)
        return false;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pOdf->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    QList<ProjectExplorer::PePort> lstCreatedPorts;
    QList<ProjectExplorer::PePort*> lstDeletedPorts;
    foreach(ProjectExplorer::PeBoard *pLayer, pOdf->GetChildBoards())
    {
        QList<ProjectExplorer::PePort*> lstPorts = pLayer->GetChildPorts();
        qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
        const int iOldLayerPortNumber = lstPorts.size();

        ProjectExplorer::PePort::FiberPlug eFiberPlug = ProjectExplorer::PePort::fpST;
        if(!lstPorts.isEmpty())
            eFiberPlug = lstPorts.first()->GetFiberPlug();

        if(iOldLayerPortNumber < iLayerPortNumber)
        {
            for(int i = iOldLayerPortNumber; i < iLayerPortNumber; i++)
            {
                ProjectExplorer::PePort port(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                port.SetName(QString("%1%2").arg(pLayer->GetPosition()).arg(i + 1, 2, 10, QLatin1Char('0')));
                port.SetGroup(i + 1);
                port.SetPortType(ProjectExplorer::PePort::ptOdf);
                port.SetPortDirection(ProjectExplorer::PePort::pdRT);
                port.SetFiberPlug(ProjectExplorer::PePort::fpST);
                port.SetFiberMode(ProjectExplorer::PePort::fmNone);
                port.SetParentBoard(pLayer);
                lstCreatedPorts.append(port);
            }
        }
        else
        {
            for(int i = iLayerPortNumber; i < iOldLayerPortNumber; i++)
            {
                ProjectExplorer::PePort *pPort = lstPorts.at(i);
                if(!pPort->GetUsedFibers().isEmpty())
                    return false;

                lstDeletedPorts.append(lstPorts.at(i));
            }
        }
    }

    // Handle Database
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
    for(int i = 0; i < lstCreatedPorts.size(); i++)
    {
        ProjectExplorer::PePort &port = lstCreatedPorts[i];
        if(!pProjectVersion->DbCreateObject(port, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    foreach(ProjectExplorer::PePort *pPort, lstDeletedPorts)
    {
        if(!pProjectVersion->DbDeleteObject(pPort->GetObjectType(), pPort->GetId(), false))
        {
            DbTrans.Rollback();
            return false;
        }
    }
    if(!DbTrans.Commit())
        return false;

    // Handle Object
    foreach(const ProjectExplorer::PePort &port, lstCreatedPorts)
    {
        if(!pProjectVersion->CreateObject(port))
            return false;
    }

    foreach(ProjectExplorer::PePort *pPort, lstDeletedPorts)
    {
        if(!pProjectVersion->DeleteObject(pPort))
            return false;
    }

    return true;
}

void CableGenerator::PreparCopyValidation()
{
    // Copy Odf
    QMap<ProjectExplorer::PePort*, ProjectExplorer::PePort*> mapOdfPortOldToNew;
    foreach(ProjectExplorer::PeDevice *pOdf, m_pProjectVersion->GetAllDevices())
    {
        if(pOdf->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
            continue;

        ProjectExplorer::PeDevice *pOdfNew = new ProjectExplorer::PeDevice(*pOdf);
        m_lstValidOdfs.append(pOdfNew);

        foreach(ProjectExplorer::PeBoard *pLayer, pOdf->GetChildBoards())
        {
            pOdfNew->RemoveChildBoard(pLayer);

            ProjectExplorer::PeBoard *pLayerNew = new ProjectExplorer::PeBoard(*pLayer);
            pLayerNew->SetParentDevice(pOdfNew);
            pOdfNew->AddChildBoard(pLayerNew);

            foreach(ProjectExplorer::PePort *pPort, pLayer->GetChildPorts())
            {
                pLayerNew->RemoveChildPort(pPort);

                ProjectExplorer::PePort *pPortNew = new ProjectExplorer::PePort(*pPort);
                pPortNew->SetParentBoard(pLayerNew);
                pLayerNew->AddChildPort(pPortNew);

                mapOdfPortOldToNew.insert(pPort, pPortNew);
            }
        }
    }

    // Copy and validate Cable and Fiber
    QMap<ProjectExplorer::PeInfoSet*, QList<ProjectExplorer::PeFiber*> > mapInfoSetToFibers;
    foreach(ProjectExplorer::PeCable *pCable, m_pProjectVersion->GetAllCables())
    {
        ProjectExplorer::PeCable *pCableNew = new ProjectExplorer::PeCable(*pCable);
        m_lstValidCables.append(pCableNew);

        foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
        {
            pCableNew->RemoveChildFiber(pFiber);

            QList<ProjectExplorer::PeInfoSet*> lstValidInfoSets;
            QStringList lstValidInfoSetIds;
            foreach(const QString &strInfoSetId, pFiber->GetInfoSetIds())
            {
                if(ProjectExplorer::PeInfoSet *pInfoSet = m_pProjectVersion->FindInfoSetById(strInfoSetId.toInt()))
                {
                    if(pInfoSet->IsComplete())
                    {
                        if(!lstValidInfoSets.contains(pInfoSet))
                            lstValidInfoSets.append(pInfoSet);

                        if(!lstValidInfoSetIds.contains(strInfoSetId))
                            lstValidInfoSetIds.append(strInfoSetId);
                    }
                }
            }

            ProjectExplorer::PeFiber *pFiberNew = new ProjectExplorer::PeFiber(*pFiber);
            pFiberNew->SetInfoSetIds(lstValidInfoSetIds);
            if(mapOdfPortOldToNew.contains(pFiberNew->GetPort1()))
                pFiberNew->SetPort1(mapOdfPortOldToNew.value(pFiberNew->GetPort1()));
            if(mapOdfPortOldToNew.contains(pFiberNew->GetPort2()))
                pFiberNew->SetPort2(mapOdfPortOldToNew.value(pFiberNew->GetPort2()));
            pFiberNew->SetParentCable(pCableNew);
            pCableNew->AddChildFiber(pFiberNew);

            foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstValidInfoSets)
            {
                QList<ProjectExplorer::PeFiber*> &lstFibers = mapInfoSetToFibers[pInfoSet];
                if(!lstFibers.contains(pFiberNew))
                    lstFibers.append(pFiberNew);
            }
        }        
    }

    // Validate InfoSet of Fiber
    foreach(ProjectExplorer::PeInfoSet *pInfoSet, mapInfoSetToFibers.keys())
    {
        QList<ProjectExplorer::PeFiber*> lstFibers = mapInfoSetToFibers.value(pInfoSet);
        if(lstFibers.isEmpty())
            continue;

        bool bInfoSetValid = true;

        // Validate set
        if(bInfoSetValid)
        {
            QString strSetOfCable;
            foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
            {
                const QString strCableEnd = pFiber->GetParentCable()->GetNameSet();
                if(strCableEnd >= "A" && strCableEnd <= "Z")
                {
                    strSetOfCable = strCableEnd;
                    break;
                }
            }

            if(!strSetOfCable.isEmpty())
            {
                QString strSetOfInfoSet;
                ProjectExplorer::PeInfoSet *pInfoSetTemp = (pInfoSet->GetInfoSetType() == ProjectExplorer::PeInfoSet::itNone ? pInfoSet->GetPairInfoSet() : pInfoSet);
                pInfoSetTemp->GetSetIndexAndMainDevice(strSetOfInfoSet);
                if(strSetOfInfoSet.isEmpty())
                    strSetOfInfoSet = "A";

                if(strSetOfCable != strSetOfInfoSet)
                    bInfoSetValid = false;
            }
        }

        // Validate path
        if(bInfoSetValid)
        {
            ProjectExplorer::PeDevice   *pTxIED = pInfoSet->GetTxIED();
            ProjectExplorer::PePort     *pTxPort = pInfoSet->GetTxPort();
            ProjectExplorer::PeDevice   *pRxIED = pInfoSet->GetRxIED();
            ProjectExplorer::PePort     *pRxPort = pInfoSet->GetRxPort();
            ProjectExplorer::PeDevice   *pSwitch1 = pInfoSet->GetSwitch1();
            ProjectExplorer::PePort     *pSwitch1TxPort = pInfoSet->GetSwitch1TxPort();
            ProjectExplorer::PePort     *pSwitch1RxPort = pInfoSet->GetSwitch1RxPort();
            ProjectExplorer::PeDevice   *pSwitch2 = pInfoSet->GetSwitch2();
            ProjectExplorer::PePort     *pSwitch2TxPort = pInfoSet->GetSwitch2TxPort();
            ProjectExplorer::PePort     *pSwitch2RxPort = pInfoSet->GetSwitch2RxPort();
            ProjectExplorer::PeDevice   *pSwitch3 = pInfoSet->GetSwitch3();
            ProjectExplorer::PePort     *pSwitch3TxPort = pInfoSet->GetSwitch3TxPort();
            ProjectExplorer::PePort     *pSwitch3RxPort = pInfoSet->GetSwitch3RxPort();
            ProjectExplorer::PeDevice   *pSwitch4 = pInfoSet->GetSwitch4();
            ProjectExplorer::PePort     *pSwitch4TxPort = pInfoSet->GetSwitch4TxPort();
            ProjectExplorer::PePort     *pSwitch4RxPort = pInfoSet->GetSwitch4RxPort();

            ProjectExplorer::PeCubicle *pCubicleTx = pTxIED ? pTxIED->GetParentCubicle() : 0;
            ProjectExplorer::PeCubicle *pCubicleRx = pRxIED ? pRxIED->GetParentCubicle() : 0;
            ProjectExplorer::PeCubicle *pCubicleSwitch1 = pSwitch1 ? pSwitch1->GetParentCubicle() : 0;
            ProjectExplorer::PeCubicle *pCubicleSwitch2 = pSwitch2 ? pSwitch2->GetParentCubicle() : 0;
            ProjectExplorer::PeCubicle *pCubicleSwitch3 = pSwitch3 ? pSwitch3->GetParentCubicle() : 0;
            ProjectExplorer::PeCubicle *pCubicleSwitch4 = pSwitch4 ? pSwitch4->GetParentCubicle() : 0;

            bool bPathValid = false;
            QList<ProjectExplorer::PeFiber*> lstTempFibers = lstFibers;
            if(pCubicleTx && pCubicleRx && !pCubicleSwitch1 && !pCubicleSwitch2 && !pCubicleSwitch3 && !pCubicleSwitch4)
            {
                bPathValid = ValidateInfoSetPath(lstTempFibers, pCubicleTx, pCubicleRx, pTxPort, pRxPort);
            }
            else if(pCubicleTx && pCubicleRx && pCubicleSwitch1 && !pCubicleSwitch2 && !pCubicleSwitch3 && !pCubicleSwitch4)
            {
                bPathValid = ValidateInfoSetPath(lstTempFibers, pCubicleTx, pCubicleSwitch1, pTxPort, pSwitch1RxPort) &&
                             ValidateInfoSetPath(lstTempFibers, pCubicleSwitch1, pCubicleRx, pSwitch1TxPort, pRxPort);
            }
            else if(pCubicleTx && pCubicleRx && pCubicleSwitch1 && pCubicleSwitch2 && !pCubicleSwitch3 && !pCubicleSwitch4)
            {
                bPathValid = ValidateInfoSetPath(lstTempFibers, pCubicleTx, pCubicleSwitch1, pTxPort, pSwitch1RxPort) &&
                             ValidateInfoSetPath(lstTempFibers, pCubicleSwitch1, pCubicleSwitch2, pSwitch1TxPort, pSwitch2RxPort) &&
                             ValidateInfoSetPath(lstTempFibers, pCubicleSwitch2, pCubicleRx, pSwitch2TxPort, pRxPort);
            }
            else if(pCubicleTx && pCubicleRx && pCubicleSwitch1 && pCubicleSwitch2 && pCubicleSwitch3 && !pCubicleSwitch4)
            {
                bPathValid = ValidateInfoSetPath(lstTempFibers, pCubicleTx, pCubicleSwitch1, pTxPort, pSwitch1RxPort) &&
                             ValidateInfoSetPath(lstTempFibers, pCubicleSwitch1, pCubicleSwitch2, pSwitch1TxPort, pSwitch2RxPort) &&
                             ValidateInfoSetPath(lstTempFibers, pCubicleSwitch2, pCubicleSwitch3, pSwitch2TxPort, pSwitch3RxPort) &&
                             ValidateInfoSetPath(lstTempFibers, pCubicleSwitch3, pCubicleRx, pSwitch3TxPort, pRxPort);
            }
            else if(pCubicleTx && pCubicleRx && pCubicleSwitch1 && pCubicleSwitch2 && pCubicleSwitch3 && pCubicleSwitch4)
            {
                bPathValid = ValidateInfoSetPath(lstTempFibers, pCubicleTx, pCubicleSwitch1, pTxPort, pSwitch1RxPort) &&
                             ValidateInfoSetPath(lstTempFibers, pCubicleSwitch1, pCubicleSwitch2, pSwitch1TxPort, pSwitch2RxPort) &&
                             ValidateInfoSetPath(lstTempFibers, pCubicleSwitch2, pCubicleSwitch3, pSwitch2TxPort, pSwitch3RxPort) &&
                             ValidateInfoSetPath(lstTempFibers, pCubicleSwitch3, pCubicleSwitch4, pSwitch3TxPort, pSwitch4RxPort) &&
                             ValidateInfoSetPath(lstTempFibers, pCubicleSwitch4, pCubicleRx, pSwitch4TxPort, pRxPort);
            }

            bInfoSetValid = (bPathValid && lstTempFibers.isEmpty());
        }

        if(bInfoSetValid)
        {
            if(!m_lstValidInfoSets.contains(pInfoSet))
                m_lstValidInfoSets.append(pInfoSet);
        }
        else
        {
            foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
                pFiber->RemoveInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
        }
    }    
}

void CableGenerator::PrepareCubicleInfoSets()
{
    qDeleteAll(m_lstCubicleInfoSets);
    m_lstCubicleInfoSets.clear();

    foreach(ProjectExplorer::PeInfoSet *pInfoSet, m_pProjectVersion->GetAllInfoSets())
    {
        if(!pInfoSet->IsComplete() || m_lstValidInfoSets.contains(pInfoSet))
            continue;

        ProjectExplorer::PeDevice   *pTxIED = pInfoSet->GetTxIED();
        ProjectExplorer::PePort     *pTxPort = pInfoSet->GetTxPort();
        ProjectExplorer::PeDevice   *pRxIED = pInfoSet->GetRxIED();
        ProjectExplorer::PePort     *pRxPort = pInfoSet->GetRxPort();
        ProjectExplorer::PeDevice   *pSwitch1 = pInfoSet->GetSwitch1();
        ProjectExplorer::PePort     *pSwitch1TxPort = pInfoSet->GetSwitch1TxPort();
        ProjectExplorer::PePort     *pSwitch1RxPort = pInfoSet->GetSwitch1RxPort();
        ProjectExplorer::PeDevice   *pSwitch2 = pInfoSet->GetSwitch2();
        ProjectExplorer::PePort     *pSwitch2TxPort = pInfoSet->GetSwitch2TxPort();
        ProjectExplorer::PePort     *pSwitch2RxPort = pInfoSet->GetSwitch2RxPort();
        ProjectExplorer::PeDevice   *pSwitch3 = pInfoSet->GetSwitch3();
        ProjectExplorer::PePort     *pSwitch3TxPort = pInfoSet->GetSwitch3TxPort();
        ProjectExplorer::PePort     *pSwitch3RxPort = pInfoSet->GetSwitch3RxPort();
        ProjectExplorer::PeDevice   *pSwitch4 = pInfoSet->GetSwitch4();
        ProjectExplorer::PePort     *pSwitch4TxPort = pInfoSet->GetSwitch4TxPort();
        ProjectExplorer::PePort     *pSwitch4RxPort = pInfoSet->GetSwitch4RxPort();

        ProjectExplorer::PeCubicle *pCubicleTx = pTxIED ? pTxIED->GetParentCubicle() : 0;
        ProjectExplorer::PeCubicle *pCubicleRx = pRxIED ? pRxIED->GetParentCubicle() : 0;
        ProjectExplorer::PeCubicle *pCubicleSwitch1 = pSwitch1 ? pSwitch1->GetParentCubicle() : 0;
        ProjectExplorer::PeCubicle *pCubicleSwitch2 = pSwitch2 ? pSwitch2->GetParentCubicle() : 0;
        ProjectExplorer::PeCubicle *pCubicleSwitch3 = pSwitch3 ? pSwitch3->GetParentCubicle() : 0;
        ProjectExplorer::PeCubicle *pCubicleSwitch4 = pSwitch4 ? pSwitch4->GetParentCubicle() : 0;

        if(pCubicleTx && pCubicleRx && !pCubicleSwitch1 && !pCubicleSwitch2 && !pCubicleSwitch3 && !pCubicleSwitch4)
        {
//            QList<ProjectExplorer::PeFiber*> lstFibers;

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pTxPort, pRxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleTx, pCubicleRx, pInfoSet, pTxPort, pRxPort);
//            }
        }
        else if(pCubicleTx && pCubicleRx && pCubicleSwitch1 && !pCubicleSwitch2 && !pCubicleSwitch3 && !pCubicleSwitch4)
        {
//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pTxPort, pSwitch1RxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleTx, pCubicleSwitch1, pInfoSet, pTxPort, pSwitch1RxPort);
//            }

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pSwitch1TxPort, pRxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleSwitch1, pCubicleRx, pInfoSet, pSwitch1TxPort, pRxPort);
//            }
        }
        else if(pCubicleTx && pCubicleRx && pCubicleSwitch1 && pCubicleSwitch2 && !pCubicleSwitch3 && !pCubicleSwitch4)
        {
//            QList<ProjectExplorer::PeFiber*> lstFibers;

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pTxPort, pSwitch1RxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleTx, pCubicleSwitch1, pInfoSet, pTxPort, pSwitch1RxPort);
//            }

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pSwitch1TxPort, pSwitch2RxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleSwitch1, pCubicleSwitch2, pInfoSet, pSwitch1TxPort, pSwitch2RxPort);
//            }

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pSwitch2TxPort, pRxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleSwitch2, pCubicleRx, pInfoSet, pSwitch2TxPort, pRxPort);
//            }
        }
        else if(pCubicleTx && pCubicleRx && pCubicleSwitch1 && pCubicleSwitch2 && pCubicleSwitch3 && !pCubicleSwitch4)
        {
//            QList<ProjectExplorer::PeFiber*> lstFibers;

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pTxPort, pSwitch1RxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleTx, pCubicleSwitch1, pInfoSet, pTxPort, pSwitch1RxPort);
//            }

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pSwitch1TxPort, pSwitch2RxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleSwitch1, pCubicleSwitch2, pInfoSet, pSwitch1TxPort, pSwitch2RxPort);
//            }

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pSwitch2TxPort, pSwitch3RxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleSwitch2, pCubicleSwitch3, pInfoSet, pSwitch2TxPort, pSwitch3RxPort);
//            }

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pSwitch3TxPort, pRxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleSwitch3, pCubicleRx, pInfoSet, pSwitch3TxPort, pRxPort);
//            }
        }
        else if(pCubicleTx && pCubicleRx && pCubicleSwitch1 && pCubicleSwitch2 && pCubicleSwitch3 && pCubicleSwitch4)
        {
//            QList<ProjectExplorer::PeFiber*> lstFibers;

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pTxPort, pSwitch1RxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleTx, pCubicleSwitch1, pInfoSet, pTxPort, pSwitch1RxPort);
//            }

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pSwitch1TxPort, pSwitch2RxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleSwitch1, pCubicleSwitch2, pInfoSet, pSwitch1TxPort, pSwitch2RxPort);
//            }

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pSwitch2TxPort, pSwitch3RxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleSwitch2, pCubicleSwitch3, pInfoSet, pSwitch2TxPort, pSwitch3RxPort);
//            }

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pSwitch3TxPort, pSwitch4RxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleSwitch3, pCubicleSwitch4, pInfoSet, pSwitch3TxPort, pSwitch4RxPort);
//            }

//            lstFibers.clear();
//            if(FindValidInfoSetPath(lstFibers, pSwitch4TxPort, pRxPort))
//            {
//                foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
//                    pFiber->AddInfoSetIds(QStringList() << QString::number(pInfoSet->GetId()));
//            }
//            else
//            {
                AddCubicleInfoSet(pCubicleSwitch4, pCubicleRx, pInfoSet, pSwitch4TxPort, pRxPort);
//            }
        }
    }
}

void CableGenerator::PrepareCableRaw()
{
    // Build optical, tail, jump cable
    foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, m_pProjectVersion->GetAllCubicleConns())
    {
        ProjectExplorer::PeCubicle *pCubicle1 = pCubicleConn->GetCubicle1();
        ProjectExplorer::PeCubicle *pCubicle2 = pCubicleConn->GetCubicle2();
        if(!pCubicle1 || !pCubicle2)
            continue;

        _CubicleInfoSet *pCubicleInfoSet = FindCubicleInfoSet(pCubicle1, pCubicle2);
        if(!pCubicleInfoSet)
            continue;

        bool bUseOdf1 = pCubicleConn->GetUseOdf1();
        bool bUseOdf2 = pCubicleConn->GetUseOdf2();
        ProjectExplorer::PeCubicle *pPassCubicle1 = pCubicleConn->GetPassCubicle1();
        ProjectExplorer::PeCubicle *pPassCubicle2 = pCubicleConn->GetPassCubicle2();

        if(!pPassCubicle1 && !pPassCubicle2)
        {
            if(bUseOdf1 && bUseOdf2) // Jump + Optical + Jump
            {
                foreach(_InfoSetConn *pInfoSetConn, pCubicleInfoSet->lstInfoSetConns)
                {
                    // Get the optical cable
                    _Cable *pCableOptical = FindCableOptical(pCubicle1, pCubicle2, pInfoSetConn->strSet);
                    if(!pCableOptical)
                    {
                        pCableOptical = new _Cable;
                        pCableOptical->strSet = pInfoSetConn->strSet;
                        pCableOptical->pCubicle1 = pCubicle1;
                        pCableOptical->pCubicle2 = pCubicle2;
                        m_lstOpticals.append(pCableOptical);
                    }
                    pCableOptical->bPass = false;

                    // Get the first jump cable
                    _Cable *pCableJump1 = FindCableJump(pCubicle1);
                    if(!pCableJump1)
                    {
                        pCableJump1 = new _Cable;
                        pCableJump1->pCubicle1 = pCubicle1;
                        pCableJump1->pCubicle2 = pCubicle1;
                        m_lstJumps.append(pCableJump1);
                    }

                    // Get the second jump cable
                    _Cable *pCableJump2 = FindCableJump(pCubicle2);
                    if(!pCableJump2)
                    {
                        pCableJump2 = new _Cable;
                        pCableJump2->pCubicle1 = pCubicle2;
                        pCableJump2->pCubicle2 = pCubicle2;
                        m_lstJumps.append(pCableJump2);
                    }

                    // Create the optical fiber
                    _Fiber *pFiberOptical = new _Fiber;
                    pFiberOptical->bReserve = false;
                    pFiberOptical->pCable = pCableOptical;
                    pFiberOptical->mapCubicleToPort.insert(pCubicle1, 0);
                    pFiberOptical->mapCubicleToPort.insert(pCubicle2, 0);
                    pFiberOptical->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                    pCableOptical->lstFibers.append(pFiberOptical);

                    // Create the first jump fiber
                    _Fiber *pFiberJump1 = new _Fiber;
                    pFiberJump1->bReserve = false;
                    pFiberJump1->pCable = pCableJump1;
                    pFiberJump1->pJumpPort1 = pInfoSetConn->mapCubicleToPort.value(pCubicle1);
                    pFiberJump1->pJumpPort2 = 0;
                    pFiberJump1->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                    pCableJump1->lstFibers.append(pFiberJump1);

                    // Create the second jump fiber
                    _Fiber *pFiberJump2 = new _Fiber;
                    pFiberJump2->bReserve = false;
                    pFiberJump2->pCable = pCableJump2;
                    pFiberJump2->pJumpPort1 = pInfoSetConn->mapCubicleToPort.value(pCubicle2);
                    pFiberJump2->pJumpPort2 = 0;
                    pFiberJump2->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                    pCableJump2->lstFibers.append(pFiberJump2);

                    // Setup the connection between fibers
                    pFiberOptical->mapCubicleToConnectedFiber.insert(pCubicle1, pFiberJump1);
                    pFiberOptical->mapCubicleToConnectedFiber.insert(pCubicle2, pFiberJump2);
                    pFiberJump1->pJumpConnectedFiber1 = 0;
                    pFiberJump1->pJumpConnectedFiber2 = pFiberOptical;
                    pFiberJump2->pJumpConnectedFiber1 = 0;
                    pFiberJump2->pJumpConnectedFiber2 = pFiberOptical;
                }
            }
            else  // Tail
            {
                foreach(_InfoSetConn *pInfoSetConn, pCubicleInfoSet->lstInfoSetConns)
                {
                    // Get the tail cable
                    _Cable *pCableTail = FindCableTail(pCubicle1, pCubicle2, pInfoSetConn->strSet);
                    if(!pCableTail)
                    {
                        pCableTail = new _Cable;
                        pCableTail->strSet = pInfoSetConn->strSet;
                        pCableTail->pCubicle1 = pCubicle1;
                        pCableTail->pCubicle2 = pCubicle2;
                        m_lstTails.append(pCableTail);
                    }

                    // Create the tail fiber
                    _Fiber *pFiberTail = new _Fiber;
                    pFiberTail->bReserve = false;
                    pFiberTail->pCable = pCableTail;
                    pFiberTail->mapCubicleToPort = pInfoSetConn->mapCubicleToPort;
                    pFiberTail->mapCubicleToConnectedFiber.insert(pCubicle1, 0);
                    pFiberTail->mapCubicleToConnectedFiber.insert(pCubicle2, 0);
                    pFiberTail->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                    pCableTail->lstFibers.append(pFiberTail);
                }
            }
        }
        else if(pPassCubicle1 && !pPassCubicle2) // Tail + Optical + Jump
        {
            foreach(_InfoSetConn *pInfoSetConn, pCubicleInfoSet->lstInfoSetConns)
            {
                // Get the tail cable
                _Cable *pCableTail = FindCableTail(pCubicle1, pPassCubicle1, pInfoSetConn->strSet);
                if(!pCableTail)
                {
                    pCableTail = new _Cable;
                    pCableTail->strSet = pInfoSetConn->strSet;
                    pCableTail->pCubicle1 = pCubicle1;
                    pCableTail->pCubicle2 = pPassCubicle1;
                    m_lstTails.append(pCableTail);
                }

                // Get the optical cable
                _Cable *pCableOptical = FindCableOptical(pPassCubicle1, pCubicle2, pInfoSetConn->strSet);
                if(!pCableOptical)
                {
                    pCableOptical = new _Cable;
                    pCableOptical->bPass = true;
                    pCableOptical->strSet = pInfoSetConn->strSet;
                    pCableOptical->pCubicle1 = pPassCubicle1;
                    pCableOptical->pCubicle2 = pCubicle2;
                    m_lstOpticals.append(pCableOptical);
                }

                // Get the jump cable
                _Cable *pCableJump = FindCableJump(pCubicle2);
                if(!pCableJump)
                {
                    pCableJump = new _Cable;
                    pCableJump->pCubicle1 = pCubicle2;
                    pCableJump->pCubicle2 = pCubicle2;
                    m_lstJumps.append(pCableJump);
                }

                // Create the tail fiber
                _Fiber *pFiberTail = new _Fiber;
                pFiberTail->bReserve = false;
                pFiberTail->pCable = pCableTail;
                pFiberTail->mapCubicleToPort.insert(pCubicle1, pInfoSetConn->mapCubicleToPort.value(pCubicle1));
                pFiberTail->mapCubicleToPort.insert(pPassCubicle1, 0);
                pFiberTail->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                pCableTail->lstFibers.append(pFiberTail);

                // Create the optical fiber
                _Fiber *pFiberOptical = new _Fiber;
                pFiberOptical->bReserve = false;
                pFiberOptical->pCable = pCableOptical;
                pFiberOptical->mapCubicleToPort.insert(pPassCubicle1, 0);
                pFiberOptical->mapCubicleToPort.insert(pCubicle2, 0);
                pFiberOptical->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                pCableOptical->lstFibers.append(pFiberOptical);

                // Create the jump fiber
                _Fiber *pFiberJump = new _Fiber;
                pFiberJump->bReserve = false;
                pFiberJump->pCable = pCableJump;
                pFiberJump->pJumpPort1 = pInfoSetConn->mapCubicleToPort.value(pCubicle2);
                pFiberJump->pJumpPort2 = 0;
                pFiberJump->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                pCableJump->lstFibers.append(pFiberJump);

                // Setup the connection between fibers
                pFiberTail->mapCubicleToConnectedFiber.insert(pCubicle1, 0);
                pFiberTail->mapCubicleToConnectedFiber.insert(pPassCubicle1, pFiberOptical);
                pFiberOptical->mapCubicleToConnectedFiber.insert(pPassCubicle1, pFiberTail);
                pFiberOptical->mapCubicleToConnectedFiber.insert(pCubicle2, pFiberJump);
                pFiberJump->pJumpConnectedFiber1 = 0;
                pFiberJump->pJumpConnectedFiber2 = pFiberOptical;
            }
        }
        else if(!pPassCubicle1 && pPassCubicle2) // Jump + Optical + Tail
        {
            foreach(_InfoSetConn *pInfoSetConn, pCubicleInfoSet->lstInfoSetConns)
            {
                // Get the jump cable
                _Cable *pCableJump = FindCableJump(pCubicle1);
                if(!pCableJump)
                {
                    pCableJump = new _Cable;
                    pCableJump->pCubicle1 = pCubicle1;
                    pCableJump->pCubicle2 = pCubicle1;
                    m_lstJumps.append(pCableJump);
                }

                // Get the optical cable
                _Cable *pCableOptical = FindCableOptical(pCubicle1, pPassCubicle2, pInfoSetConn->strSet);
                if(!pCableOptical)
                {
                    pCableOptical = new _Cable;
                    pCableOptical->bPass = true;
                    pCableOptical->strSet = pInfoSetConn->strSet;
                    pCableOptical->pCubicle1 = pCubicle1;
                    pCableOptical->pCubicle2 = pPassCubicle2;
                    m_lstOpticals.append(pCableOptical);
                }

                // Get the tail cable
                _Cable *pCableTail = FindCableTail(pPassCubicle2, pCubicle2, pInfoSetConn->strSet);
                if(!pCableTail)
                {
                    pCableTail = new _Cable;
                    pCableTail->strSet = pInfoSetConn->strSet;
                    pCableTail->pCubicle1 = pPassCubicle2;
                    pCableTail->pCubicle2 = pCubicle2;
                    m_lstTails.append(pCableTail);
                }

                // Create the jump fiber
                _Fiber *pFiberJump = new _Fiber;
                pFiberJump->bReserve = false;
                pFiberJump->pCable = pCableJump;
                pFiberJump->pJumpPort1 = pInfoSetConn->mapCubicleToPort.value(pCubicle1);
                pFiberJump->pJumpPort2 = 0;
                pFiberJump->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                pCableJump->lstFibers.append(pFiberJump);

                // Create the optical fiber
                _Fiber *pFiberOptical = new _Fiber;
                pFiberOptical->bReserve = false;
                pFiberOptical->pCable = pCableOptical;
                pFiberOptical->mapCubicleToPort.insert(pCubicle1, 0);
                pFiberOptical->mapCubicleToPort.insert(pPassCubicle2, 0);
                pFiberOptical->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                pCableOptical->lstFibers.append(pFiberOptical);

                // Create the tail fiber
                _Fiber *pFiberTail = new _Fiber;
                pFiberTail->bReserve = false;
                pFiberTail->pCable = pCableTail;
                pFiberTail->mapCubicleToPort.insert(pPassCubicle2, 0);
                pFiberTail->mapCubicleToPort.insert(pCubicle2, pInfoSetConn->mapCubicleToPort.value(pCubicle2));
                pFiberTail->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                pCableTail->lstFibers.append(pFiberTail);

                // Setup the connection between fibers
                pFiberJump->pJumpConnectedFiber1 = 0;
                pFiberJump->pJumpConnectedFiber2 = pFiberOptical;
                pFiberOptical->mapCubicleToConnectedFiber.insert(pCubicle1, pFiberJump);
                pFiberOptical->mapCubicleToConnectedFiber.insert(pPassCubicle2, pFiberTail);
                pFiberTail->mapCubicleToConnectedFiber.insert(pPassCubicle2, pFiberOptical);
                pFiberTail->mapCubicleToConnectedFiber.insert(pCubicle2, 0);
            }
        }
        else if(pPassCubicle1 && pPassCubicle2) // Tail + Optical + Tail
        {
            foreach(_InfoSetConn *pInfoSetConn, pCubicleInfoSet->lstInfoSetConns)
            {
                // Get the first tail cable
                _Cable *pCableTail1 = FindCableTail(pCubicle1, pPassCubicle1, pInfoSetConn->strSet);
                if(!pCableTail1)
                {
                    pCableTail1 = new _Cable;
                    pCableTail1->strSet = pInfoSetConn->strSet;
                    pCableTail1->pCubicle1 = pCubicle1;
                    pCableTail1->pCubicle2 = pPassCubicle1;
                    m_lstTails.append(pCableTail1);
                }

                // Get the optical cable
                _Cable *pCableOptical = FindCableOptical(pPassCubicle1, pPassCubicle2, pInfoSetConn->strSet);
                if(!pCableOptical)
                {
                    pCableOptical = new _Cable;
                    pCableOptical->bPass = true;
                    pCableOptical->strSet = pInfoSetConn->strSet;
                    pCableOptical->pCubicle1 = pPassCubicle1;
                    pCableOptical->pCubicle2 = pPassCubicle2;
                    m_lstOpticals.append(pCableOptical);
                }

                // Get the second tail cable
                _Cable *pCableTail2 = FindCableTail(pPassCubicle2, pCubicle2, pInfoSetConn->strSet);
                if(!pCableTail2)
                {
                    pCableTail2 = new _Cable;
                    pCableTail2->strSet = pInfoSetConn->strSet;
                    pCableTail2->pCubicle1 = pPassCubicle2;
                    pCableTail2->pCubicle2 = pCubicle2;
                    m_lstTails.append(pCableTail2);
                }

                // Create the first tail fiber
                _Fiber *pFiberTail1 = new _Fiber;
                pFiberTail1->bReserve = false;
                pFiberTail1->pCable = pCableTail1;
                pFiberTail1->mapCubicleToPort.insert(pCubicle1, pInfoSetConn->mapCubicleToPort.value(pCubicle1));
                pFiberTail1->mapCubicleToPort.insert(pPassCubicle1, 0);
                pFiberTail1->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                pCableTail1->lstFibers.append(pFiberTail1);

                // Create the optical fiber
                _Fiber *pFiberOptical = new _Fiber;
                pFiberOptical->bReserve = false;
                pFiberOptical->pCable = pCableOptical;
                pFiberOptical->mapCubicleToPort.insert(pPassCubicle1, 0);
                pFiberOptical->mapCubicleToPort.insert(pPassCubicle2, 0);
                pFiberOptical->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                pCableOptical->lstFibers.append(pFiberOptical);

                // Create the second tail fiber
                _Fiber *pFiberTail2 = new _Fiber;
                pFiberTail2->bReserve = false;
                pFiberTail2->pCable = pCableTail2;
                pFiberTail2->mapCubicleToPort.insert(pPassCubicle2, 0);
                pFiberTail2->mapCubicleToPort.insert(pCubicle2, pInfoSetConn->mapCubicleToPort.value(pCubicle2));
                pFiberTail2->lstInfoSets.append(pInfoSetConn->lstInfoSets);
                pCableTail2->lstFibers.append(pFiberTail2);

                // Setup the connection between fibers
                pFiberTail1->mapCubicleToConnectedFiber.insert(pCubicle1, 0);
                pFiberTail1->mapCubicleToConnectedFiber.insert(pPassCubicle1, pFiberOptical);
                pFiberOptical->mapCubicleToConnectedFiber.insert(pPassCubicle1, pFiberTail1);
                pFiberOptical->mapCubicleToConnectedFiber.insert(pPassCubicle2, pFiberTail2);
                pFiberTail2->mapCubicleToConnectedFiber.insert(pPassCubicle2, pFiberOptical);
                pFiberTail2->mapCubicleToConnectedFiber.insert(pCubicle2, 0);
            }
        }
    }

    // Build rest jump cable
    foreach(_CubicleInfoSet *pCubicleInfoSet, m_lstCubicleInfoSets)
    {
        if(pCubicleInfoSet->pCubicle1 != pCubicleInfoSet->pCubicle2)
            continue;

        // Get the jump cable
        _Cable *pCableJump = FindCableJump(pCubicleInfoSet->pCubicle1);
        if(!pCableJump)
        {
            pCableJump = new _Cable;
            pCableJump->pCubicle1 = pCubicleInfoSet->pCubicle1;
            pCableJump->pCubicle2 = pCubicleInfoSet->pCubicle1;
            m_lstJumps.append(pCableJump);
        }

        foreach(_InfoSetConn *pInfoSetConn, pCubicleInfoSet->lstInfoSetConns)
        {
            // Create the jump fiber
            _Fiber *pFiberJump = new _Fiber;
            pFiberJump->bReserve = false;
            pFiberJump->pCable = pCableJump;
            pFiberJump->pJumpPort1 = pInfoSetConn->pJumpPort1;
            pFiberJump->pJumpPort2 = pInfoSetConn->pJumpPort2;
            pFiberJump->lstInfoSets.append(pInfoSetConn->lstInfoSets);
            pCableJump->lstFibers.append(pFiberJump);
        }
    }
}

void CableGenerator::PrepareCableRawReuse()
{
    typedef QPair<ProjectExplorer::PePort*, ProjectExplorer::PePort*> PortPair;

    // Build valid fiber path for port
    QMap<PortPair, QList<ProjectExplorer::PeFiber*> > mapValidPortToFiberPath;
    QList<ProjectExplorer::PeFiber*> lstValidJumpTailFibers, lstValidOpticalFibers;
    foreach(ProjectExplorer::PeCable *pCable, m_lstValidCables)
    {
        ProjectExplorer::PeCable::CableType eCableType = pCable->GetCableType();
        if(eCableType == ProjectExplorer::PeCable::ctJump || eCableType == ProjectExplorer::PeCable::ctTail)
        {
            foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
            {
                if(!pFiber->GetReserve())
                    lstValidJumpTailFibers.append(pFiber);
            }
        }
        else if(eCableType == ProjectExplorer::PeCable::ctOptical)
        {
            lstValidOpticalFibers.append(pCable->GetChildFibers());
        }
    }

    while(!lstValidJumpTailFibers.isEmpty())
    {
        ProjectExplorer::PeFiber *pFiber = lstValidJumpTailFibers.takeFirst();
        ProjectExplorer::PePort *pPort1 = pFiber->GetPort1();
        ProjectExplorer::PePort *pPort2 = pFiber->GetPort2();
        const bool bOdfPort1 = (pPort1->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF);
        const bool bOdfPort2 = (pPort2->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF);

        if(bOdfPort1 && bOdfPort2)
        {
            qDebug() << "error: " << __LINE__;
            continue;
        }
        if(!bOdfPort1 && !bOdfPort2)
        {
            mapValidPortToFiberPath.insert(PortPair(pPort1, pPort2),  QList<ProjectExplorer::PeFiber*>() << pFiber);
        }
        else
        {
            ProjectExplorer::PePort *pPortStart = bOdfPort1 ? pPort2 : pPort1;
            ProjectExplorer::PePort *pPortOdfStart = bOdfPort1 ? pPort1 : pPort2;
            QList<ProjectExplorer::PeFiber*> lstConnectedFibers = FindValidFibersFromPort(pPortOdfStart, lstValidOpticalFibers);
            if(lstConnectedFibers.size() != 1)
            {
                qDebug() << "error: " << __LINE__;
                continue;
            }

            ProjectExplorer::PeFiber *pFiberMiddle = lstConnectedFibers.first();
            lstValidOpticalFibers.removeAll(pFiberMiddle);

            ProjectExplorer::PePort *pPortOdfEnd = pFiberMiddle->GetPort1() == pPortOdfStart ? pFiberMiddle->GetPort2() : pFiberMiddle->GetPort1();
            lstConnectedFibers = FindValidFibersFromPort(pPortOdfEnd, lstValidJumpTailFibers);
            if(lstConnectedFibers.size() != 1)
            {
                qDebug() << "error: " << __LINE__;
                continue;
            }

            ProjectExplorer::PeFiber *pFiberEnd = lstConnectedFibers.first();
            lstValidJumpTailFibers.removeAll(pFiberEnd);

            ProjectExplorer::PePort *pPortEnd = pFiberEnd->GetPort1() == pPortOdfEnd ? pFiberEnd->GetPort2() : pFiberEnd->GetPort1();

            mapValidPortToFiberPath.insert(PortPair(pPortStart, pPortEnd),  QList<ProjectExplorer::PeFiber*>() << pFiber << pFiberMiddle << pFiberEnd);
        }
    }

    // Build fiber path for port
    QMap<PortPair, QList<_Fiber*> > mapPortToFiberPath;

    QList<_Fiber*> lstJumpTailFibers;
    foreach(_Cable *pCable, m_lstJumps)
        lstJumpTailFibers.append(pCable->lstFibers);
    foreach(_Cable *pCable, m_lstTails)
        lstJumpTailFibers.append(pCable->lstFibers);

    while(!lstJumpTailFibers.isEmpty())
    {
        _Fiber *pFiber = lstJumpTailFibers.takeFirst();

        ProjectExplorer::PePort *pPort1 = 0, *pPort2 = 0;
        _Fiber *pFiberConnected1 = 0, *pFiberConnected2 = 0;
        if(m_lstJumps.contains(pFiber->pCable))
        {
            pPort1 = pFiber->pJumpPort1;
            pPort2 = pFiber->pJumpPort2;
            pFiberConnected1 = pFiber->pJumpConnectedFiber1;
            pFiberConnected2 = pFiber->pJumpConnectedFiber2;
        }
        else if(m_lstTails.contains(pFiber->pCable))
        {
            if(pFiber->mapCubicleToPort.keys().size() != 2)
            {
                qDebug() << "error: " << __LINE__;
                continue;
            }

            pPort1 = pFiber->mapCubicleToPort.value(pFiber->mapCubicleToPort.keys().at(0), 0);
            pPort2 = pFiber->mapCubicleToPort.value(pFiber->mapCubicleToPort.keys().at(1), 0);
            pFiberConnected1 = pFiber->mapCubicleToConnectedFiber.value(pFiber->mapCubicleToPort.keys().at(0), 0);
            pFiberConnected2 = pFiber->mapCubicleToConnectedFiber.value(pFiber->mapCubicleToPort.keys().at(1), 0);
        }
        else
        {
            qDebug() << "error: " << __LINE__;
            continue;
        }

        if(!pPort1 && !pPort2)
        {
            qDebug() << "error: " << __LINE__;
            continue;
        }
        else if(pPort1 && pPort2)
        {
            mapPortToFiberPath.insert(PortPair(pPort1, pPort2),  QList<_Fiber*>() << pFiber);
        }
        else
        {
            ProjectExplorer::PePort *pPortStart = pPort1 ? pPort1 : pPort2;
            _Fiber *pFiberMiddle = pPort1 ? pFiberConnected2 : pFiberConnected1;
            if(!pFiberMiddle || !m_lstOpticals.contains(pFiberMiddle->pCable))
            {
                qDebug() << "error: " << __LINE__;
                continue;
            }

            QList<_Fiber*> lstConnectedFibers = pFiberMiddle->mapCubicleToConnectedFiber.values();
            if(lstConnectedFibers.size() != 2)
            {
                qDebug() << "error: " << __LINE__;
                continue;
            }

            _Fiber *pFiberEnd = lstConnectedFibers.at(0) == pFiber ? lstConnectedFibers.at(1) : lstConnectedFibers.at(0);
            if(lstJumpTailFibers.removeAll(pFiberEnd) <= 0)
            {
                qDebug() << "error: " << __LINE__;
                continue;
            }

            ProjectExplorer::PePort *pPortEnd = 0;
            if(m_lstJumps.contains(pFiberEnd->pCable))
            {
                pPortEnd = (pFiberEnd->pJumpPort1 ? pFiberEnd->pJumpPort1 : pFiberEnd->pJumpPort2);
            }
            else if(m_lstTails.contains(pFiberEnd->pCable))
            {
                QList<ProjectExplorer::PePort*> lstPots = pFiberEnd->mapCubicleToPort.values();
                if(lstPots.size() == 2)
                    pPortEnd = (lstPots.at(0) ? lstPots.at(0) : lstPots.at(1));
            }
            if(!pPortEnd)
            {
                qDebug() << "error: " << __LINE__;
                continue;
            }

            mapPortToFiberPath.insert(PortPair(pPortStart, pPortEnd),  QList<_Fiber*>() << pFiber << pFiberMiddle << pFiberEnd);
        }
    }

    // Check reuse
    foreach(const PortPair &pp, mapPortToFiberPath.keys())
    {
        ProjectExplorer::PePort *pPortStart = 0, *pPortEnd = 0;
        QList<_Fiber*> lstFibers;

        ProjectExplorer::PePort *pPortValidStart = 0, *pPortValidEnd = 0;
        QList<ProjectExplorer::PeFiber*> lstValidFibers;

        foreach(const PortPair &vpp, mapValidPortToFiberPath.keys())
        {
            if(pp.first == vpp.first)
            {
                pPortStart = pp.first;
                pPortEnd = pp.second;
                lstFibers = mapPortToFiberPath.value(pp);

                pPortValidStart = vpp.first;
                pPortValidEnd = vpp.second;
                lstValidFibers = mapValidPortToFiberPath.value(vpp);

                break;
            }
            else if(pp.first == vpp.second)
            {
                pPortStart = pp.first;
                pPortEnd = pp.second;
                lstFibers = mapPortToFiberPath.value(pp);

                pPortValidStart = vpp.second;
                pPortValidEnd = vpp.first;
                foreach(ProjectExplorer::PeFiber *pFiber, mapValidPortToFiberPath.value(vpp))
                    lstValidFibers.prepend(pFiber);

                break;
            }
            else if(pp.second == vpp.first)
            {
                pPortStart = pp.second;
                pPortEnd = pp.first;
                foreach(_Fiber *pFiber, mapPortToFiberPath.value(pp))
                    lstFibers.prepend(pFiber);

                pPortValidStart = vpp.first;
                pPortValidEnd = vpp.second;
                lstValidFibers = mapValidPortToFiberPath.value(vpp);

                break;
            }
            else if(pp.second == vpp.second)
            {
                pPortStart = pp.second;
                pPortEnd = pp.first;
                foreach(_Fiber *pFiber, mapPortToFiberPath.value(pp))
                    lstFibers.prepend(pFiber);

                pPortValidStart = vpp.second;
                pPortValidEnd = vpp.first;
                foreach(ProjectExplorer::PeFiber *pFiber, mapValidPortToFiberPath.value(vpp))
                    lstValidFibers.prepend(pFiber);

                break;
            }
        }

        if(!pPortStart || !pPortEnd || lstFibers.isEmpty() || !pPortValidStart || !pPortValidEnd || lstValidFibers.isEmpty())
            continue;

        if(lstFibers.size() != lstValidFibers.size())
            continue;

        const bool bPortIdentical = ((pPortStart == pPortValidStart) && (pPortEnd == pPortValidEnd));
        if(lstFibers.size() == 1)
        {
            _Fiber *pFiber = lstFibers.first();
            ProjectExplorer::PeFiber *pFiberValid = lstValidFibers.first();
            const bool bHasInfoSet = !pFiberValid->GetInfoSetIds().isEmpty();

            if(CheckFiberReuse(pFiber, pFiberValid, !(bPortIdentical && bHasInfoSet)))
            {
                QStringList lstInfoSetIds;
                foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber->lstInfoSets)
                    lstInfoSetIds.append(QString::number(pInfoSet->GetId()));
                pFiberValid->AddInfoSetIds(lstInfoSetIds);

                if(pFiberValid->GetPort1() == pPortValidEnd)
                    pFiberValid->SetPort1(pPortEnd);
                else if(pFiberValid->GetPort2() == pPortValidEnd)
                    pFiberValid->SetPort2(pPortEnd);

                pFiber->pCable->lstFibers.removeAll(pFiber);
                delete pFiber;
            }
        }
        else if(lstFibers.size() == 3)
        {
            _Fiber *pFiber1 = lstFibers.at(0);
            ProjectExplorer::PeFiber *pFiberValid1 = lstValidFibers.at(0);

            _Fiber *pFiber2 = lstFibers.at(1);
            ProjectExplorer::PeFiber *pFiberValid2 = lstValidFibers.at(1);

            _Fiber *pFiber3 = lstFibers.at(2);
            ProjectExplorer::PeFiber *pFiberValid3 = lstValidFibers.at(2);

            const bool bHasInfoSet = !(pFiberValid1->GetInfoSetIds().isEmpty() &&
                                       pFiberValid2->GetInfoSetIds().isEmpty() &&
                                       pFiberValid3->GetInfoSetIds().isEmpty());

            const bool bUser1 = CheckFiberReuse(pFiber1, pFiberValid1, !(bPortIdentical && bHasInfoSet));
            const bool bUser2 = CheckFiberReuse(pFiber2, pFiberValid2, !(bPortIdentical && bHasInfoSet));
            const bool bUser3 = CheckFiberReuse(pFiber3, pFiberValid3, !(bPortIdentical && bHasInfoSet));

            if(bUser1 && bUser2 && bUser3)
            {
                QStringList lstInfoSetIds;
                foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber1->lstInfoSets)
                    lstInfoSetIds.append(QString::number(pInfoSet->GetId()));
                pFiberValid1->AddInfoSetIds(lstInfoSetIds);

                pFiber1->pCable->lstFibers.removeAll(pFiber1);
                delete pFiber1;

                lstInfoSetIds.clear();
                foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber2->lstInfoSets)
                    lstInfoSetIds.append(QString::number(pInfoSet->GetId()));
                pFiberValid2->AddInfoSetIds(lstInfoSetIds);

                pFiber2->pCable->lstFibers.removeAll(pFiber2);
                delete pFiber2;

                lstInfoSetIds.clear();
                foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber3->lstInfoSets)
                    lstInfoSetIds.append(QString::number(pInfoSet->GetId()));
                pFiberValid3->AddInfoSetIds(lstInfoSetIds);

                if(pFiberValid3->GetPort1() == pPortValidEnd)
                    pFiberValid3->SetPort1(pPortEnd);
                else if(pFiberValid3->GetPort2() == pPortValidEnd)
                    pFiberValid3->SetPort2(pPortEnd);

                pFiber3->pCable->lstFibers.removeAll(pFiber3);
                delete pFiber3;
            }
            else if(bUser1 && bUser2)
            {
                ProjectExplorer::PePort *pOdfPortStart = (pFiberValid1->GetPort1() == pPortValidStart ? pFiberValid1->GetPort2() : pFiberValid1->GetPort1());
                if(!pOdfPortStart || pOdfPortStart->GetParentDevice()->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
                {
                    qDebug() << "error: " << __LINE__;
                    continue;
                }

                ProjectExplorer::PePort *pOdfPortEnd = (pFiberValid2->GetPort1() == pOdfPortStart ? pFiberValid2->GetPort2() : pFiberValid2->GetPort1());
                if(!pOdfPortEnd || pOdfPortEnd->GetParentDevice()->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
                {
                    qDebug() << "error: " << __LINE__;
                    continue;
                }

                if(m_lstJumps.contains(pFiber3->pCable))
                {
                    if(!pFiber3->pJumpPort1)
                    {
                        pFiber3->pJumpPort1 = pOdfPortEnd;
                        pFiber3->pJumpConnectedFiber1 = 0;
                    }
                    else if(!pFiber3->pJumpPort2)
                    {
                        pFiber3->pJumpPort2 = pOdfPortEnd;
                        pFiber3->pJumpConnectedFiber2 = 0;
                    }
                    else
                    {
                        qDebug() << "error: " << __LINE__;
                        continue;
                    }
                }
                else if(m_lstTails.contains(pFiber3->pCable))
                {
                    ProjectExplorer::PeCubicle *pCubicle = pOdfPortEnd->GetParentDevice()->GetParentCubicle();
                    if(!pCubicle)
                    {
                        qDebug() << "error: " << __LINE__;
                        continue;
                    }

                    if(!pFiber3->mapCubicleToPort.contains(pCubicle) || !pFiber3->mapCubicleToConnectedFiber.contains(pCubicle))
                    {
                        qDebug() << "error: " << __LINE__;
                        continue;
                    }

                    pFiber3->mapCubicleToPort[pCubicle] = pOdfPortEnd;
                    pFiber3->mapCubicleToConnectedFiber[pCubicle] = 0;
                }

                QStringList lstInfoSetIds;
                foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber1->lstInfoSets)
                    lstInfoSetIds.append(QString::number(pInfoSet->GetId()));
                pFiberValid1->AddInfoSetIds(lstInfoSetIds);

                pFiber1->pCable->lstFibers.removeAll(pFiber1);
                delete pFiber1;

                lstInfoSetIds.clear();
                foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber2->lstInfoSets)
                    lstInfoSetIds.append(QString::number(pInfoSet->GetId()));
                pFiberValid2->AddInfoSetIds(lstInfoSetIds);

                pFiber2->pCable->lstFibers.removeAll(pFiber2);
                delete pFiber2;
            }
            else if(bUser3 && bUser2)
            {
                ProjectExplorer::PePort *pOdfPortEnd = (pFiberValid3->GetPort1() == pPortValidEnd ? pFiberValid3->GetPort2() : pFiberValid3->GetPort1());
                if(!pOdfPortEnd || pOdfPortEnd->GetParentDevice()->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
                {
                    qDebug() << "error: " << __LINE__;
                    continue;
                }

                ProjectExplorer::PePort *pOdfPortStart = (pFiberValid2->GetPort1() == pOdfPortEnd ? pFiberValid2->GetPort2() : pFiberValid2->GetPort1());
                if(!pOdfPortStart || pOdfPortStart->GetParentDevice()->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
                {
                    qDebug() << "error: " << __LINE__;
                    continue;
                }

                if(m_lstJumps.contains(pFiber1->pCable))
                {
                    if(!pFiber1->pJumpPort1)
                    {
                        pFiber1->pJumpPort1 = pOdfPortStart;
                        pFiber1->pJumpConnectedFiber1 = 0;
                    }
                    else if(!pFiber1->pJumpPort2)
                    {
                        pFiber1->pJumpPort2 = pOdfPortStart;
                        pFiber1->pJumpConnectedFiber2 = 0;
                    }
                    else
                    {
                        qDebug() << "error: " << __LINE__;
                        continue;
                    }
                }
                else if(m_lstTails.contains(pFiber1->pCable))
                {
                    ProjectExplorer::PeCubicle *pCubicle = pOdfPortStart->GetParentDevice()->GetParentCubicle();
                    if(!pCubicle)
                    {
                        qDebug() << "error: " << __LINE__;
                        continue;
                    }

                    if(!pFiber1->mapCubicleToPort.contains(pCubicle) || !pFiber1->mapCubicleToConnectedFiber.contains(pCubicle))
                    {
                        qDebug() << "error: " << __LINE__;
                        continue;
                    }

                    pFiber1->mapCubicleToPort[pCubicle] = pOdfPortStart;
                    pFiber1->mapCubicleToConnectedFiber[pCubicle] = 0;
                }

                QStringList lstInfoSetIds;
                foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber3->lstInfoSets)
                    lstInfoSetIds.append(QString::number(pInfoSet->GetId()));
                pFiberValid3->AddInfoSetIds(lstInfoSetIds);

                pFiber3->pCable->lstFibers.removeAll(pFiber3);
                delete pFiber3;

                lstInfoSetIds.clear();
                foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber2->lstInfoSets)
                    lstInfoSetIds.append(QString::number(pInfoSet->GetId()));
                pFiberValid2->AddInfoSetIds(lstInfoSetIds);

                pFiber2->pCable->lstFibers.removeAll(pFiber2);
                delete pFiber2;
            }
            /*else if(bUser1)
            {
                ProjectExplorer::PePort *pOdfPortStart = (pFiberValid1->GetPort1() == pPortValidStart ? pFiberValid1->GetPort2() : pFiberValid1->GetPort1());
                if(!pOdfPortStart || pOdfPortStart->GetParentDevice()->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
                {
                    qDebug() << "error: " << __LINE__;
                    continue;
                }

                ProjectExplorer::PeCubicle *pCubicle = pOdfPortStart->GetParentDevice()->GetParentCubicle();
                if(!pCubicle)
                {
                    qDebug() << "error: " << __LINE__;
                    continue;
                }

                if(!pFiber2->mapCubicleToPort.contains(pCubicle) || !pFiber2->mapCubicleToConnectedFiber.contains(pCubicle))
                {
                    qDebug() << "error: " << __LINE__;
                    continue;
                }

                pFiber2->mapCubicleToPort[pCubicle] = pOdfPortStart;
                pFiber2->mapCubicleToConnectedFiber[pCubicle] = 0;

                QStringList lstInfoSetIds;
                foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber1->lstInfoSets)
                    lstInfoSetIds.append(QString::number(pInfoSet->GetId()));
                pFiberValid1->AddInfoSetIds(lstInfoSetIds);

                pFiber1->pCable->lstFibers.removeAll(pFiber1);
                delete pFiber1;
            }
            else if(bUser3)
            {
                ProjectExplorer::PePort *pOdfPortEnd = (pFiberValid3->GetPort1() == pPortValidEnd ? pFiberValid3->GetPort2() : pFiberValid3->GetPort1());
                if(!pOdfPortEnd || pOdfPortEnd->GetParentDevice()->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
                {
                    qDebug() << "error: " << __LINE__;
                    continue;
                }

                ProjectExplorer::PeCubicle *pCubicle = pOdfPortEnd->GetParentDevice()->GetParentCubicle();
                if(!pCubicle)
                {
                    qDebug() << "error: " << __LINE__;
                    continue;
                }

                if(!pFiber2->mapCubicleToPort.contains(pCubicle) || !pFiber2->mapCubicleToConnectedFiber.contains(pCubicle))
                {
                    qDebug() << "error: " << __LINE__;
                    continue;
                }

                pFiber2->mapCubicleToPort[pCubicle] = pOdfPortEnd;
                pFiber2->mapCubicleToConnectedFiber[pCubicle] = 0;

                QStringList lstInfoSetIds;
                foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber3->lstInfoSets)
                    lstInfoSetIds.append(QString::number(pInfoSet->GetId()));
                pFiberValid3->AddInfoSetIds(lstInfoSetIds);

                pFiber3->pCable->lstFibers.removeAll(pFiber3);
                delete pFiber3;
            }*/
        }
    }

    // Clean unused cable
    foreach(_Cable *pCable, m_lstOpticals)
    {
        if(pCable->lstFibers.isEmpty())
        {
            m_lstOpticals.removeAll(pCable);
            delete pCable;
        }
    }
    foreach(_Cable *pCable, m_lstTails)
    {
        if(pCable->lstFibers.isEmpty())
        {
            m_lstTails.removeAll(pCable);
            delete pCable;
        }
    }
    foreach(_Cable *pCable, m_lstJumps)
    {
        if(pCable->lstFibers.isEmpty())
        {
            m_lstJumps.removeAll(pCable);
            delete pCable;
        }
    }

    // Clean unused valid cable
    foreach(ProjectExplorer::PeCable *pCable, m_lstValidCables)
    {
        ProjectExplorer::PeCable::CableType eCableType = pCable->GetCableType();
        if(eCableType == ProjectExplorer::PeCable::ctOptical || eCableType == ProjectExplorer::PeCable::ctTail)
        {
            bool bUnUsed = true;
            foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
            {
                if(pFiber->GetInfoSetIds().isEmpty())
                {
                    pFiber->SetReserve(true);
                    if(eCableType == ProjectExplorer::PeCable::ctTail)
                    {
                        pFiber->SetPort1(0);
                        pFiber->SetPort2(0);
                    }
                }

                if(bUnUsed && !pFiber->GetReserve())
                    bUnUsed = false;
            }

            /*
            if(bUnUsed)
            {
                m_lstValidCables.removeAll(pCable);
                delete pCable;
            }*/
        }
        else
        {
            foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
            {
                if(pFiber->GetInfoSetIds().isEmpty())
                {
                    pCable->RemoveChildFiber(pFiber);
                    pFiber->SetParentCable(0);

                    delete pFiber;
                }
            }

            if(pCable->GetChildFibers().isEmpty())
            {
                m_lstValidCables.removeAll(pCable);
                delete pCable;
            }
        }
    }

    // Clean unused valid ODF layer
    /*
    QList<ProjectExplorer::PePort*> lstUsedPorts;
    foreach(ProjectExplorer::PeCable *pCable, m_lstValidCables)
    {
        foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
        {
            if(!lstUsedPorts.contains(pFiber->GetPort1()))
                lstUsedPorts.append(pFiber->GetPort1());
            if(!lstUsedPorts.contains(pFiber->GetPort2()))
                lstUsedPorts.append(pFiber->GetPort2());
        }
    }

    foreach(ProjectExplorer::PeDevice *pOdf, m_lstValidOdfs)
    {
        foreach(ProjectExplorer::PeBoard *pLayer, pOdf->GetChildBoards())
        {
            bool bUnUsed = true;
            foreach(ProjectExplorer::PePort *pPort, pLayer->GetChildPorts())
            {
                if(lstUsedPorts.contains(pPort))
                {
                    bUnUsed = false;
                    break;
                }
            }

            if(bUnUsed)
            {
                pOdf->RemoveChildBoard(pLayer);
                pLayer->SetParentDevice(0);

                delete pLayer;
            }
        }

        if(pOdf->GetChildBoards().isEmpty())
        {
            m_lstValidOdfs.removeAll(pOdf);
            delete pOdf;
        }
    }*/
}

void CableGenerator::PrepareCableReserve()
{
    QMap<_Cable*, QList<_Cable*> > mapOriToSplit;

    ///////////////////////////////////////////////////////////////////
    // Calculate optical cables
    ///////////////////////////////////////////////////////////////////
    if(m_eOpticalGroupType == gtPort)
    {
        mapOriToSplit.clear();

        foreach(_Cable *pCable, m_lstOpticals)
        {
            if(pCable->lstFibers.isEmpty())
                continue;

            QList<_Cable*> lstSpliteCables;
            while(!pCable->lstFibers.isEmpty())
            {
                _Fiber *pFiber = pCable->lstFibers.takeFirst();
                _Fiber *pFiberPair = FindPairFiber(pFiber);
                if(pFiberPair)
                    pCable->lstFibers.removeOne(pFiberPair);

                _Cable *pCableSplit = new _Cable;
                pCableSplit->strSet = pCable->strSet;
                pCableSplit->pCubicle1 = pCable->pCubicle1;
                pCableSplit->pCubicle2 = pCable->pCubicle2;
                lstSpliteCables.append(pCableSplit);

                int iReserveCount = 1;
                pCableSplit->lstFibers.append(pFiber);
                if(pFiberPair)
                {
                    pCableSplit->lstFibers.append(pFiberPair);
                    iReserveCount++;
                }

                for(int i = 0; i < iReserveCount; i++)
                {
                    _Fiber *pFiberReserve = new _Fiber;
                    pFiberReserve->bReserve = true;
                    pFiberReserve->pCable = pCableSplit;
                    pCableSplit->lstFibers.append(pFiberReserve);
                }
            }

            mapOriToSplit.insert(pCable, lstSpliteCables);
        }

        foreach(_Cable *pCable, mapOriToSplit.keys())
        {
            int iIndex = m_lstOpticals.indexOf(pCable);
            if(iIndex == -1)
                continue;

            if(m_lstOpticals.takeAt(iIndex) != pCable)
                continue;

            QList<_Cable*> lstSpliteCables = mapOriToSplit.value(pCable);
            while(!lstSpliteCables.isEmpty())
                m_lstOpticals.insert(iIndex, lstSpliteCables.takeLast());

            qDeleteAll(pCable->lstFibers);
            delete pCable;
        }
    }
    else if(m_eOpticalGroupType == gtCustom)
    {
        if(m_eOpticalReserveType == rtAll)
        {
            mapOriToSplit.clear();

            foreach(_Cable *pCable, m_lstOpticals)
            {
                const int iAllFiberCountAfterReserved = int(double(pCable->lstFibers.size()) * (1.0 + m_dOpticalReserveRate) + 0.5);
                const int iFullCableCount = iAllFiberCountAfterReserved / m_lstOpticalFiberNumbers.last();
                const int iRestFiberCount = iAllFiberCountAfterReserved % m_lstOpticalFiberNumbers.last();
                const int iFiberCountForRestCable = iRestFiberCount ? CalculateClosestFiberNumber(iRestFiberCount, true) : 0;

                QList<_Cable*> lstSpliteCables;
                for(int i = 0; i < iFullCableCount; i++)
                {
                    _Cable *pCableSplit = new _Cable;
                    pCableSplit->bPass = pCable->bPass;
                    pCableSplit->strSet = pCable->strSet;
                    pCableSplit->pCubicle1 = pCable->pCubicle1;
                    pCableSplit->pCubicle2 = pCable->pCubicle2;
                    lstSpliteCables.append(pCableSplit);

                    for(int j = 0; j < m_lstOpticalFiberNumbers.last(); j++)
                    {
                        if(!pCable->lstFibers.isEmpty())
                        {
                            pCableSplit->lstFibers.append(pCable->lstFibers.takeFirst());
                        }
                        else
                        {
                            _Fiber *pFiber = new _Fiber;
                            pFiber->bReserve = true;
                            pFiber->pCable = pCableSplit;
                            pCableSplit->lstFibers.append(pFiber);
                        }
                    }
                }

                if(iFiberCountForRestCable)
                {
                    _Cable *pCableSplit = new _Cable;
                    pCableSplit->bPass = pCable->bPass;
                    pCableSplit->strSet = pCable->strSet;
                    pCableSplit->pCubicle1 = pCable->pCubicle1;
                    pCableSplit->pCubicle2 = pCable->pCubicle2;
                    lstSpliteCables.append(pCableSplit);
                    for(int i = 0; i < iFiberCountForRestCable; i++)
                    {
                        if(!pCable->lstFibers.isEmpty())
                        {
                            pCableSplit->lstFibers.append(pCable->lstFibers.takeFirst());
                        }
                        else
                        {
                            _Fiber *pFiber = new _Fiber;
                            pFiber->bReserve = true;
                            pFiber->pCable = pCableSplit;
                            pCableSplit->lstFibers.append(pFiber);
                        }
                    }
                }

                mapOriToSplit.insert(pCable, lstSpliteCables);
            }

            foreach(_Cable *pCable, mapOriToSplit.keys())
            {
                int iIndex = m_lstOpticals.indexOf(pCable);
                if(iIndex == -1)
                    continue;

                if(m_lstOpticals.takeAt(iIndex) != pCable)
                    continue;

                QList<_Cable*> lstSpliteCables = mapOriToSplit.value(pCable);
                while(!lstSpliteCables.isEmpty())
                    m_lstOpticals.insert(iIndex, lstSpliteCables.takeLast());

                qDeleteAll(pCable->lstFibers);
                delete pCable;
            }
        }
        else if(m_eOpticalReserveType == rtSingle)
        {
            mapOriToSplit.clear();

            QList<int> lstRealFiberNumbers;
            foreach(int iFiberNumber, m_lstOpticalFiberNumbers)
                lstRealFiberNumbers.append(int(double(iFiberNumber) / (1.0 + m_dOpticalReserveRate)));

            foreach(_Cable *pCable, m_lstOpticals)
            {
                QList<_Cable*> lstSpliteCables;
                while(!pCable->lstFibers.isEmpty())
                {
                    int iIndex;
                    if(pCable->lstFibers.size() >= lstRealFiberNumbers.last())
                    {
                        iIndex = lstRealFiberNumbers.size() - 1;
                    }
                    else if(pCable->lstFibers.size() <= lstRealFiberNumbers.first())
                    {
                        iIndex = 0;
                    }
                    else
                    {
                        for(int i = lstRealFiberNumbers.size() - 2; i >= 0; i--)
                        {
                            if(pCable->lstFibers.size() > lstRealFiberNumbers.at(i))
                            {
                                iIndex = i + 1;
                                break;
                            }
                        }
                    }

                    const int iFiberNumber = m_lstOpticalFiberNumbers.at(iIndex);
                    const int iRealFiberNumber = lstRealFiberNumbers.at(iIndex);

                    _Cable *pCableSplit = new _Cable;
                    pCableSplit->bPass = pCable->bPass;
                    pCableSplit->strSet = pCable->strSet;
                    pCableSplit->pCubicle1 = pCable->pCubicle1;
                    pCableSplit->pCubicle2 = pCable->pCubicle2;
                    lstSpliteCables.append(pCableSplit);

                    for(int j = 0; j < iFiberNumber; j++)
                    {
                        if(j < iRealFiberNumber && !pCable->lstFibers.isEmpty())
                        {
                            pCableSplit->lstFibers.append(pCable->lstFibers.takeFirst());
                        }
                        else
                        {
                            _Fiber *pFiber = new _Fiber;
                            pFiber->bReserve = true;
                            pFiber->pCable = pCableSplit;
                            pCableSplit->lstFibers.append(pFiber);
                        }
                    }

                    mapOriToSplit.insert(pCable, lstSpliteCables);
                }
            }

            foreach(_Cable *pCable, mapOriToSplit.keys())
            {
                int iIndex = m_lstOpticals.indexOf(pCable);
                if(iIndex == -1)
                    continue;

                if(m_lstOpticals.takeAt(iIndex) != pCable)
                    continue;

                QList<_Cable*> lstSpliteCables = mapOriToSplit.value(pCable);
                while(!lstSpliteCables.isEmpty())
                    m_lstOpticals.insert(iIndex, lstSpliteCables.takeLast());

                qDeleteAll(pCable->lstFibers);
                delete pCable;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////
    // Calculate tail cables
    ///////////////////////////////////////////////////////////////////
    if(m_eTailGroupType == gtPort)
    {
        mapOriToSplit.clear();

        foreach(_Cable *pCable, m_lstTails)
        {
            if(pCable->lstFibers.isEmpty())
                continue;

            QList<_Cable*> lstSpliteCables;
            while(!pCable->lstFibers.isEmpty())
            {
                _Fiber *pFiber = pCable->lstFibers.takeFirst();
                _Fiber *pFiberPair = FindPairFiber(pFiber);
                if(pFiberPair)
                    pCable->lstFibers.removeOne(pFiberPair);

                _Cable *pCableSplit = new _Cable;
                pCableSplit->strSet = pCable->strSet;
                pCableSplit->pCubicle1 = pCable->pCubicle1;
                pCableSplit->pCubicle2 = pCable->pCubicle2;
                lstSpliteCables.append(pCableSplit);

                int iReserveCount = 1;
                pCableSplit->lstFibers.append(pFiber);
                if(pFiberPair)
                {
                    pCableSplit->lstFibers.append(pFiberPair);
                    iReserveCount++;
                }

                for(int i = 0; i < iReserveCount; i++)
                {
                    _Fiber *pFiberReserve = new _Fiber;
                    pFiberReserve->bReserve = true;
                    pFiberReserve->pCable = pCableSplit;
                    pCableSplit->lstFibers.append(pFiberReserve);
                }
            }

            mapOriToSplit.insert(pCable, lstSpliteCables);
        }

        foreach(_Cable *pCable, mapOriToSplit.keys())
        {
            int iIndex = m_lstTails.indexOf(pCable);
            if(iIndex == -1)
                continue;

            if(m_lstTails.takeAt(iIndex) != pCable)
                continue;

            QList<_Cable*> lstSpliteCables = mapOriToSplit.value(pCable);
            while(!lstSpliteCables.isEmpty())
                m_lstTails.insert(iIndex, lstSpliteCables.takeLast());

            qDeleteAll(pCable->lstFibers);
            delete pCable;
        }
    }
    else if(m_eTailGroupType == gtCustom)
    {
        if(m_eTailReserveType == rtAll)
        {
            mapOriToSplit.clear();

            foreach(_Cable *pCable, m_lstTails)
            {
                const int iAllFiberCountAfterReserved = int(double(pCable->lstFibers.size()) * (1.0 + m_dTailReserveRate) + 0.5);
                if(iAllFiberCountAfterReserved > m_lstTailFiberNumbers.last())
                {
                    const int iFullCableCount = iAllFiberCountAfterReserved / m_lstTailFiberNumbers.last();
                    const int iRestFiberCount = iAllFiberCountAfterReserved % m_lstTailFiberNumbers.last();
                    const int iFiberCountForRestCable = iRestFiberCount ? CalculateClosestFiberNumber(iRestFiberCount, false) : 0;

                    QList<_Cable*> lstSpliteCables;
                    for(int i = 0; i < iFullCableCount; i++)
                    {
                        _Cable *pCableSplit = new _Cable;
                        pCableSplit->strSet = pCable->strSet;
                        pCableSplit->pCubicle1 = pCable->pCubicle1;
                        pCableSplit->pCubicle2 = pCable->pCubicle2;
                        lstSpliteCables.append(pCableSplit);

                        for(int j = 0; j < m_lstTailFiberNumbers.last(); j++)
                        {
                            if(!pCable->lstFibers.isEmpty())
                            {
                                pCableSplit->lstFibers.append(pCable->lstFibers.takeFirst());
                            }
                            else
                            {
                                _Fiber *pFiber = new _Fiber;
                                pFiber->bReserve = true;
                                pFiber->pCable = pCableSplit;
                                pCableSplit->lstFibers.append(pFiber);
                            }
                        }
                    }

                    if(iFiberCountForRestCable)
                    {
                        _Cable *pCableSplit = new _Cable;
                        pCableSplit->strSet = pCable->strSet;
                        pCableSplit->pCubicle1 = pCable->pCubicle1;
                        pCableSplit->pCubicle2 = pCable->pCubicle2;
                        lstSpliteCables.append(pCableSplit);
                        for(int i = 0; i < iFiberCountForRestCable; i++)
                        {
                            if(!pCable->lstFibers.isEmpty())
                            {
                                pCableSplit->lstFibers.append(pCable->lstFibers.takeFirst());
                            }
                            else
                            {
                                _Fiber *pFiber = new _Fiber;
                                pFiber->bReserve = true;
                                pFiber->pCable = pCableSplit;
                                pCableSplit->lstFibers.append(pFiber);
                            }
                        }
                    }

                    mapOriToSplit.insert(pCable, lstSpliteCables);
                }
                else
                {
                    int iFiberCountReal = CalculateClosestFiberNumber(iAllFiberCountAfterReserved, false);
                    int iDiff = iFiberCountReal - pCable->lstFibers.size();
                    for(int i = 0; i < iDiff; i++)
                    {
                        _Fiber *pFiber = new _Fiber;
                        pFiber->bReserve = true;
                        pFiber->pCable = pCable;
                        pCable->lstFibers.append(pFiber);
                    }
                }
            }

            foreach(_Cable *pCable, mapOriToSplit.keys())
            {
                int iIndex = m_lstTails.indexOf(pCable);
                if(iIndex == -1)
                    continue;

                if(m_lstTails.takeAt(iIndex) != pCable)
                    continue;

                QList<_Cable*> lstSpliteCables = mapOriToSplit.value(pCable);
                while(!lstSpliteCables.isEmpty())
                    m_lstTails.insert(iIndex, lstSpliteCables.takeLast());

                qDeleteAll(pCable->lstFibers);
                delete pCable;
            }
        }
        else if(m_eTailReserveType == rtSingle)
        {
            mapOriToSplit.clear();

            QList<int> lstRealFiberNumbers;
            foreach(int iFiberNumber, m_lstTailFiberNumbers)
                lstRealFiberNumbers.append(int(double(iFiberNumber) / (1.0 + m_dTailReserveRate)));

            foreach(_Cable *pCable, m_lstTails)
            {
                QList<_Cable*> lstSpliteCables;
                while(!pCable->lstFibers.isEmpty())
                {
                    int iIndex;
                    if(pCable->lstFibers.size() >= lstRealFiberNumbers.last())
                    {
                        iIndex = lstRealFiberNumbers.size() - 1;
                    }
                    else if(pCable->lstFibers.size() <= lstRealFiberNumbers.first())
                    {
                        iIndex = 0;
                    }
                    else
                    {
                        for(int i = lstRealFiberNumbers.size() - 2; i >= 0; i--)
                        {
                            if(pCable->lstFibers.size() > lstRealFiberNumbers.at(i))
                            {
                                iIndex = i + 1;
                                break;
                            }
                        }
                    }

                    const int iFiberNumber = m_lstTailFiberNumbers.at(iIndex);
                    const int iRealFiberNumber = lstRealFiberNumbers.at(iIndex);

                    _Cable *pCableSplit = new _Cable;
                    pCableSplit->bPass = pCable->bPass;
                    pCableSplit->strSet = pCable->strSet;
                    pCableSplit->pCubicle1 = pCable->pCubicle1;
                    pCableSplit->pCubicle2 = pCable->pCubicle2;
                    lstSpliteCables.append(pCableSplit);

                    for(int j = 0; j < iFiberNumber; j++)
                    {
                        if(j < iRealFiberNumber && !pCable->lstFibers.isEmpty())
                        {
                            pCableSplit->lstFibers.append(pCable->lstFibers.takeFirst());
                        }
                        else
                        {
                            _Fiber *pFiber = new _Fiber;
                            pFiber->bReserve = true;
                            pFiber->pCable = pCableSplit;
                            pCableSplit->lstFibers.append(pFiber);
                        }
                    }

                    mapOriToSplit.insert(pCable, lstSpliteCables);
                }
            }

            foreach(_Cable *pCable, mapOriToSplit.keys())
            {
                int iIndex = m_lstTails.indexOf(pCable);
                if(iIndex == -1)
                    continue;

                if(m_lstTails.takeAt(iIndex) != pCable)
                    continue;

                QList<_Cable*> lstSpliteCables = mapOriToSplit.value(pCable);
                while(!lstSpliteCables.isEmpty())
                    m_lstTails.insert(iIndex, lstSpliteCables.takeLast());

                qDeleteAll(pCable->lstFibers);
                delete pCable;
            }
        }
    }
}

void CableGenerator::PrepareOdf()
{
    typedef QMap<QString, QList<_Cable> > SetToCableMap;
    QMap<ProjectExplorer::PeCubicle*, SetToCableMap> mapCubicleToSetCable;

    foreach(_Cable *pCable, m_lstOpticals)
    {
        ProjectExplorer::PeCubicle *pCubicle1 = pCable->pCubicle1;
        ProjectExplorer::PeCubicle *pCubicle2 = pCable->pCubicle2;

        _Cable cable = *pCable;
        cable.lstFibers.clear();
        foreach(_Fiber *pFiber, pCable->lstFibers)
        {
            if(!pFiber->mapCubicleToPort.value(pCubicle1) || !pFiber->mapCubicleToPort.value(pCubicle2))
                cable.lstFibers.append(pFiber);
        }
        if(cable.lstFibers.isEmpty())
            continue;

        SetToCableMap &mapSetToCableForCubicle1 = mapCubicleToSetCable[pCubicle1];
        QList<_Cable> &lstCables1 = mapSetToCableForCubicle1[pCable->strSet];
        lstCables1.append(cable);

        SetToCableMap &mapSetToCableForCubicle2 = mapCubicleToSetCable[pCubicle2];
        QList<_Cable> &lstCables2 = mapSetToCableForCubicle2[pCable->strSet];
        lstCables2.append(cable);
    }

    foreach(ProjectExplorer::PeCubicle *pCubicle, mapCubicleToSetCable.keys())
    {
        SetToCableMap mapSetToCable = mapCubicleToSetCable.value(pCubicle);
        foreach(const QString &strSet, mapSetToCable.keys())
        {
            QList<_Cable> lstCables;
            foreach(_Cable cable, mapSetToCable.value(strSet))
            {
                int iPos = lstCables.size();
                for(int i = 0; i < lstCables.size(); i++)
                {
                    if(cable.lstFibers.size() > lstCables.at(i).lstFibers.size())
                    {
                        iPos = i;
                        break;
                    }
                }

                lstCables.insert(iPos, cable);


            }

            int iOdfLayerPortNum = m_iOdfLayerPortNumber;
            ProjectExplorer::PeDevice *pOdf = 0;
            int iResidueOdfLayerPortNum = 0;
            ProjectExplorer::PeBoard *pLayer = 0;
            foreach(_Cable cable, lstCables)
            {
                const int iFiberCount = cable.lstFibers.size();

                if(!pOdf)
                    pOdf = FindValidOdf(pCubicle, strSet, iOdfLayerPortNum, iFiberCount, true);

                const int iResidueOdfPortNum = (26 - pOdf->GetChildBoards().size()) * iOdfLayerPortNum;

                if(iFiberCount > iResidueOdfPortNum) // Check to see if the cable will cross the odf
                {
                    pOdf = FindValidOdf(pCubicle, strSet, iOdfLayerPortNum, iFiberCount, true);
                }
                else if(iFiberCount <= iResidueOdfLayerPortNum && pLayer) // Check to see if the cable can be put into the rest of layer
                {
                    int iCurrentPortIndex = iOdfLayerPortNum - iResidueOdfLayerPortNum;
                    foreach(_Fiber *pFiber, cable.lstFibers)
                    {
                        ProjectExplorer::PePort *pPort = pLayer->GetChildPorts().at(iCurrentPortIndex++);

                        pFiber->mapCubicleToPort[pCubicle] = pPort;

                        if(_Fiber *pFiberConnected = pFiber->mapCubicleToConnectedFiber.value(pCubicle))
                        {
                            pFiberConnected->mapCubicleToPort[pCubicle] = pPort;
                            pFiberConnected->pJumpPort2 = pPort;
                        }
                    }

                    iResidueOdfLayerPortNum = iResidueOdfLayerPortNum - iFiberCount;
                    continue;
                }

                int iLayerCount = iFiberCount / iOdfLayerPortNum;
                if(iFiberCount % iOdfLayerPortNum)
                {
                    iLayerCount = iLayerCount + 1;
                    iResidueOdfLayerPortNum = (iLayerCount * iOdfLayerPortNum) - iFiberCount;
                }
                else
                {
                    iResidueOdfLayerPortNum = 0;
                }

                for(int i = 0; i < iLayerCount; i++)
                {
                    QStringList lstExistLayerNames;
                    foreach(ProjectExplorer::PeBoard *pL, pOdf->GetChildBoards())
                        lstExistLayerNames.append(pL->GetPosition());

                    char chLayout = 'A';
                    while(lstExistLayerNames.contains(QString(chLayout)))
                        chLayout++;

                    pLayer = new ProjectExplorer::PeBoard(m_pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                    pLayer->SetPosition(QString(chLayout));
                    pLayer->SetParentDevice(pOdf);
                    pOdf->AddChildBoard(pLayer);

                    for(int j = 0; j < iOdfLayerPortNum; j++)
                    {
                        ProjectExplorer::PePort *pPort = new ProjectExplorer::PePort(m_pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                        pPort->SetName(QString("%1%2").arg(pLayer->GetPosition()).arg(j + 1, 2, 10, QLatin1Char('0')));
                        pPort->SetGroup(j + 1);
                        pPort->SetPortType(ProjectExplorer::PePort::ptOdf);
                        pPort->SetPortDirection(ProjectExplorer::PePort::pdRT);
                        pPort->SetFiberPlug(ProjectExplorer::PePort::fpST);
                        pPort->SetFiberMode(ProjectExplorer::PePort::fmNone);
                        pPort->SetParentBoard(pLayer);
                        pLayer->AddChildPort(pPort);

                        int iCurrentIndex = i * iOdfLayerPortNum + j;
                        if(iCurrentIndex < iFiberCount)
                        {
                            _Fiber *pFiber = cable.lstFibers.at(iCurrentIndex);
                            pFiber->mapCubicleToPort[pCubicle] = pPort;

                            if(_Fiber *pFiberConnected = pFiber->mapCubicleToConnectedFiber.value(pCubicle))
                            {
                                pFiberConnected->mapCubicleToPort[pCubicle] = pPort;
                                pFiberConnected->pJumpPort2 = pPort;
                            }
                        }
                    }
                }
            }
        }
    }
}

void CableGenerator::PrepareAllCables()
{
    // Create Optical Cables
    foreach(_Cable *pCable, m_lstOpticals)
    {
        ProjectExplorer::PeCable *pCableNew = new ProjectExplorer::PeCable(m_pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        pCableNew->SetCableType(ProjectExplorer::PeCable::ctOptical);
        CalculateOpticalCableName(pCable, pCableNew);
        pCableNew->SetCubicle1(pCable->pCubicle1);
        pCableNew->SetCubicle2(pCable->pCubicle2);
        m_lstValidCables.append(pCableNew);

        int iPipeNumber = CalculatePipeNumber(pCable->lstFibers.size());
        for(int i = 0; i < pCable->lstFibers.size(); i++)
        {
            _Fiber *pFiber = pCable->lstFibers.at(i);

            QStringList lstInfoSetIds;
            foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber->lstInfoSets)
                lstInfoSetIds.append(QString::number(pInfoSet->GetId()));

            ProjectExplorer::PeFiber *pFiberNew = new ProjectExplorer::PeFiber(m_pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            pFiberNew->SetIndex(i + 1);
            pFiberNew->SetFiberColor(ProjectExplorer::PeFiber::FiberColor(i % iPipeNumber));
            pFiberNew->SetPipeColor(ProjectExplorer::PeFiber::FiberColor(i / iPipeNumber));
            pFiberNew->SetReserve(pFiber->bReserve);
            pFiberNew->SetInfoSetIds(lstInfoSetIds);
            pFiberNew->SetPort1(pFiber->mapCubicleToPort.value(pCable->pCubicle1));
            pFiberNew->SetPort2(pFiber->mapCubicleToPort.value(pCable->pCubicle2));
            pFiberNew->SetParentCable(pCableNew);
            pCableNew->AddChildFiber(pFiberNew);
        }
    }

    // Create Tail Cables
    foreach(_Cable *pCable, m_lstTails)
    {
        ProjectExplorer::PeCable *pCableNew = new ProjectExplorer::PeCable(m_pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        pCableNew->SetCableType(ProjectExplorer::PeCable::ctTail);
        CalculateTailCableName(pCable, pCableNew);
        pCableNew->SetCubicle1(pCable->pCubicle1);
        pCableNew->SetCubicle2(pCable->pCubicle2);
        m_lstValidCables.append(pCableNew);

        int iPipeNumber = CalculatePipeNumber(pCable->lstFibers.size());
        for(int i = 0; i < pCable->lstFibers.size(); i++)
        {
            _Fiber *pFiber = pCable->lstFibers.at(i);

            QStringList lstInfoSetIds;
            foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber->lstInfoSets)
                lstInfoSetIds.append(QString::number(pInfoSet->GetId()));

            ProjectExplorer::PeFiber *pFiberNew = new ProjectExplorer::PeFiber(m_pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            pFiberNew->SetIndex(i + 1);
            pFiberNew->SetFiberColor(ProjectExplorer::PeFiber::FiberColor(i % iPipeNumber));
            pFiberNew->SetPipeColor(ProjectExplorer::PeFiber::FiberColor(i / iPipeNumber));
            pFiberNew->SetReserve(pFiber->bReserve);
            pFiberNew->SetInfoSetIds(lstInfoSetIds);
            pFiberNew->SetPort1(pFiber->mapCubicleToPort.value(pCable->pCubicle1));
            pFiberNew->SetPort2(pFiber->mapCubicleToPort.value(pCable->pCubicle2));
            pFiberNew->SetParentCable(pCableNew);
            pCableNew->AddChildFiber(pFiberNew);
        }
    }

    // Create Jump Cables
    foreach(_Cable *pCable, m_lstJumps)
    {
        foreach(_Fiber *pFiber, pCable->lstFibers)
        {
            ProjectExplorer::PeCable *pCableNew = new ProjectExplorer::PeCable(m_pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            pCableNew->SetCableType(ProjectExplorer::PeCable::ctJump);
            CalculateJumpCableName(pCable, pCableNew);
            pCableNew->SetCubicle1(pCable->pCubicle1);
            pCableNew->SetCubicle2(pCable->pCubicle2);
            m_lstValidCables.append(pCableNew);

            QStringList lstInfoSetIds;
            foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber->lstInfoSets)
                lstInfoSetIds.append(QString::number(pInfoSet->GetId()));

            ProjectExplorer::PeFiber *pFiberNew = new ProjectExplorer::PeFiber(m_pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            pFiberNew->SetIndex(0);
            pFiberNew->SetInfoSetIds(lstInfoSetIds);
            pFiberNew->SetPort1(pFiber->pJumpPort1);
            pFiberNew->SetPort2(pFiber->pJumpPort2);
            pFiberNew->SetParentCable(pCableNew);
            pCableNew->AddChildFiber(pFiberNew);
        }
    }
}

bool CableGenerator::BuildAll()
{
    ///////////////////////////////////////////////////////////////////////
    // Handle Database
    ///////////////////////////////////////////////////////////////////////
    ProjectExplorer::DbTransaction DbTrans(m_pProjectVersion, true);

    // Handle Odf
    QList<int> lstValidOdfIds, lstValidLayerIds, lstValidPortIds;
    foreach(ProjectExplorer::PeDevice *pOdf, m_lstValidOdfs)
    {
        if(pOdf->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        {
            if(!m_pProjectVersion->DbCreateObject(*pOdf, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        else
        {
            if(!m_pProjectVersion->DbUpdateObject(*pOdf, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        lstValidOdfIds.append(pOdf->GetId());

        foreach(ProjectExplorer::PeBoard *pLayer, pOdf->GetChildBoards())
        {
            if(pLayer->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
            {
                if(!m_pProjectVersion->DbCreateObject(*pLayer, false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }
            else
            {
                if(!m_pProjectVersion->DbUpdateObject(*pLayer, false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }
            lstValidLayerIds.append(pLayer->GetId());

            foreach(ProjectExplorer::PePort *pPort, pLayer->GetChildPorts())
            {
                if(pPort->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
                {
                    if(!m_pProjectVersion->DbCreateObject(*pPort, false))
                    {
                        DbTrans.Rollback();
                        return false;
                    }
                }
                else
                {
                    if(!m_pProjectVersion->DbUpdateObject(*pPort, false))
                    {
                        DbTrans.Rollback();
                        return false;
                    }
                }
                lstValidPortIds.append(pPort->GetId());
            }
        }
    }

    foreach(ProjectExplorer::PeDevice *pOdf, m_pProjectVersion->GetAllDevices())
    {
        if(pOdf->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
            continue;

        if(!lstValidOdfIds.contains(pOdf->GetId()))
        {
            if(!m_pProjectVersion->DbDeleteObject(ProjectExplorer::PeProjectObject::otDevice, pOdf->GetId(), false))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        else
        {
            foreach(ProjectExplorer::PeBoard *pLayer, pOdf->GetChildBoards())
            {
                if(!lstValidLayerIds.contains(pLayer->GetId()))
                {
                    if(!m_pProjectVersion->DbDeleteObject(ProjectExplorer::PeProjectObject::otBoard, pLayer->GetId(), false))
                    {
                        DbTrans.Rollback();
                        return false;
                    }
                }
                else
                {
                    foreach(ProjectExplorer::PePort *pPort, pLayer->GetChildPorts())
                    {
                        if(!lstValidPortIds.contains(pPort->GetId()))
                        {
                            if(!m_pProjectVersion->DbDeleteObject(ProjectExplorer::PeProjectObject::otPort, pPort->GetId(), false))
                            {
                                DbTrans.Rollback();
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    // Handle Cables
    QList<int> lstValidCableIds, lstValidFiberIds;
    foreach(ProjectExplorer::PeCable *pCable, m_lstValidCables)
    {
        if(pCable->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        {
            if(!m_pProjectVersion->DbCreateObject(*pCable, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        else
        {
            if(!m_pProjectVersion->DbUpdateObject(*pCable, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        lstValidCableIds.append(pCable->GetId());

        foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
        {
            if(pFiber->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
            {
                if(!m_pProjectVersion->DbCreateObject(*pFiber, false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }
            else
            {
                if(!m_pProjectVersion->DbUpdateObject(*pFiber, false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }
            lstValidFiberIds.append(pFiber->GetId());
        }
    }

    foreach(ProjectExplorer::PeCable *pCable, m_pProjectVersion->GetAllCables())
    {
        if(!lstValidCableIds.contains(pCable->GetId()))
        {
            if(!m_pProjectVersion->DbDeleteObject(ProjectExplorer::PeProjectObject::otCable, pCable->GetId(), false))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        else
        {
            foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
            {
                if(!lstValidFiberIds.contains(pFiber->GetId()))
                {
                    if(!m_pProjectVersion->DbDeleteObject(ProjectExplorer::PeProjectObject::otFiber, pFiber->GetId(), false))
                    {
                        DbTrans.Rollback();
                        return false;
                    }
                }
            }
        }
    }

    if(!DbTrans.Commit())
        return false;

    ///////////////////////////////////////////////////////////////////////
    // Handle Object
    ///////////////////////////////////////////////////////////////////////

    // Handle Odf
    QMap<ProjectExplorer::PePort*, ProjectExplorer::PePort*> mapOdfPortOldToNew;
    foreach(ProjectExplorer::PeDevice *pOdf, m_lstValidOdfs)
    {
        ProjectExplorer::PeDevice *pOdfModify = 0;
        if(!m_pProjectVersion->FindDeviceById(pOdf->GetId()))
            pOdfModify = qobject_cast<ProjectExplorer::PeDevice*>(m_pProjectVersion->CreateObject(*pOdf));
        else
            pOdfModify = qobject_cast<ProjectExplorer::PeDevice*>(m_pProjectVersion->UpdateObject(*pOdf));

        if(!pOdfModify)
            return false;

        foreach(ProjectExplorer::PeBoard *pLayer, pOdf->GetChildBoards())
        {
            pLayer->SetParentDevice(pOdfModify);

            ProjectExplorer::PeBoard *pLayerModify = 0;
            if(!m_pProjectVersion->FindBoardById(pLayer->GetId()))
                pLayerModify = qobject_cast<ProjectExplorer::PeBoard*>(m_pProjectVersion->CreateObject(*pLayer));
            else
                pLayerModify = qobject_cast<ProjectExplorer::PeBoard*>(m_pProjectVersion->UpdateObject(*pLayer));

            if(!pLayerModify)
                return false;

            foreach(ProjectExplorer::PePort *pPort, pLayer->GetChildPorts())
            {
                pPort->SetParentBoard(pLayerModify);

                ProjectExplorer::PePort *pPortModify = 0;
                if(!m_pProjectVersion->FindPortById(pPort->GetId()))
                    pPortModify = qobject_cast<ProjectExplorer::PePort*>(m_pProjectVersion->CreateObject(*pPort));
                else
                    pPortModify = qobject_cast<ProjectExplorer::PePort*>(m_pProjectVersion->UpdateObject(*pPort));

                if(!pPortModify)
                    return false;

                mapOdfPortOldToNew.insert(pPort, pPortModify);
            }
        }
    }

    foreach(ProjectExplorer::PeDevice *pOdf, m_pProjectVersion->GetAllDevices())
    {
        if(pOdf->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
            continue;

        if(!lstValidOdfIds.contains(pOdf->GetId()))
        {
            if(!m_pProjectVersion->DeleteObject(pOdf))
                return false;
        }
        else
        {
            foreach(ProjectExplorer::PeBoard *pLayer, pOdf->GetChildBoards())
            {
                if(!lstValidLayerIds.contains(pLayer->GetId()))
                {
                    if(!m_pProjectVersion->DeleteObject(pLayer))
                        return false;
                }
                else
                {
                    foreach(ProjectExplorer::PePort *pPort, pLayer->GetChildPorts())
                    {
                        if(!lstValidPortIds.contains(pPort->GetId()))
                        {
                            if(!m_pProjectVersion->DeleteObject(pPort))
                                return false;
                        }
                    }
                }
            }
        }
    }

    // Handle Cable
    foreach(ProjectExplorer::PeCable *pCable, m_lstValidCables)
    {
        ProjectExplorer::PeCable *pCableModify = 0;
        if(!m_pProjectVersion->FindCableById(pCable->GetId()))
            pCableModify = qobject_cast<ProjectExplorer::PeCable*>(m_pProjectVersion->CreateObject(*pCable));
        else
            pCableModify = qobject_cast<ProjectExplorer::PeCable*>(m_pProjectVersion->UpdateObject(*pCable));

        if(!pCableModify)
            return false;

        foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
        {
            pFiber->SetParentCable(pCableModify);
            if(mapOdfPortOldToNew.contains(pFiber->GetPort1()))
                pFiber->SetPort1(mapOdfPortOldToNew.value(pFiber->GetPort1(), 0));
            if(mapOdfPortOldToNew.contains(pFiber->GetPort2()))
                pFiber->SetPort2(mapOdfPortOldToNew.value(pFiber->GetPort2(), 0));

            ProjectExplorer::PeFiber *pFiberModify = 0;
            if(!m_pProjectVersion->FindFiberById(pFiber->GetId()))
                pFiberModify = qobject_cast<ProjectExplorer::PeFiber*>(m_pProjectVersion->CreateObject(*pFiber));
            else
                pFiberModify = qobject_cast<ProjectExplorer::PeFiber*>(m_pProjectVersion->UpdateObject(*pFiber));

            if(!pFiberModify)
                return false;
        }
    }

    foreach(ProjectExplorer::PeCable *pCable, m_pProjectVersion->GetAllCables())
    {
        if(!lstValidCableIds.contains(pCable->GetId()))
        {
            if(!m_pProjectVersion->DeleteObject(pCable))
                return false;
        }
        else
        {
            foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
            {
                if(!lstValidFiberIds.contains(pFiber->GetId()))
                {
                    if(!m_pProjectVersion->DeleteObject(pFiber))
                        return false;
                }
            }
        }
    }

    return true;
}

void CableGenerator::ClearBuild()
{
    foreach(_CubicleInfoSet *pCubicleInfoSet, m_lstCubicleInfoSets)
    {
        qDeleteAll(pCubicleInfoSet->lstInfoSetConns);
        pCubicleInfoSet->lstInfoSetConns.clear();
    }
    qDeleteAll(m_lstCubicleInfoSets);
    m_lstCubicleInfoSets.clear();

    foreach(_Cable *pCable, m_lstOpticals)
    {
        qDeleteAll(pCable->lstFibers);
        pCable->lstFibers.clear();
    }
    qDeleteAll(m_lstOpticals);
    m_lstOpticals.clear();

    foreach(_Cable *pCable, m_lstTails)
    {
        qDeleteAll(pCable->lstFibers);
        pCable->lstFibers.clear();
    }
    qDeleteAll(m_lstTails);
    m_lstTails.clear();

    foreach(_Cable *pCable, m_lstJumps)
    {
        qDeleteAll(pCable->lstFibers);
        pCable->lstFibers.clear();
    }
    qDeleteAll(m_lstJumps);
    m_lstJumps.clear();

    foreach(ProjectExplorer::PeCable *pCable, m_lstValidCables)
        qDeleteAll(pCable->GetChildFibers());
    qDeleteAll(m_lstValidCables);
    m_lstValidCables.clear();

    m_lstValidInfoSets.clear();

    foreach(ProjectExplorer::PeDevice *pOdf, m_lstValidOdfs)
    {
        foreach(ProjectExplorer::PeBoard *pLayer, pOdf->GetChildBoards())
            qDeleteAll(pLayer->GetChildPorts());

        qDeleteAll(pOdf->GetChildBoards());
    }
    qDeleteAll(m_lstValidOdfs);
    m_lstValidOdfs.clear();
}

bool CableGenerator::ValidateInfoSetPath(QList<ProjectExplorer::PeFiber*> &lstFibers, ProjectExplorer::PeCubicle *pInfoSetCubicle1, ProjectExplorer::PeCubicle *pInfoSetCubicle2, ProjectExplorer::PePort *pInfoSetPort1, ProjectExplorer::PePort *pInfoSetPort2)
{
    if(lstFibers.isEmpty() || !pInfoSetCubicle1 || !pInfoSetCubicle2 || !pInfoSetPort1 || !pInfoSetPort2)
        return false;

    if(pInfoSetCubicle1 == pInfoSetCubicle2)
    {
        foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
        {
            if((pFiber->GetPort1() == pInfoSetPort1 && pFiber->GetPort2() == pInfoSetPort2) ||
               (pFiber->GetPort1() == pInfoSetPort2 && pFiber->GetPort2() == pInfoSetPort1))
            {
                if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctJump)
                    continue;

                lstFibers.removeAll(pFiber);
                return true;
            }
        }

        return false;
    }

    ProjectExplorer::PeCubicleConn *pCubicleConnFouned = 0;
    foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, m_pProjectVersion->GetAllCubicleConns())
    {
        if((pCubicleConn->GetCubicle1() == pInfoSetCubicle1 && pCubicleConn->GetCubicle2() == pInfoSetCubicle2) ||
           (pCubicleConn->GetCubicle1() == pInfoSetCubicle2 && pCubicleConn->GetCubicle2() == pInfoSetCubicle1))
        {
            pCubicleConnFouned = pCubicleConn;
            break;
        }
    }

    if(!pCubicleConnFouned)
        return false;

    ProjectExplorer::PeCubicle *pCubicle1 = pCubicleConnFouned->GetCubicle1();
    ProjectExplorer::PeCubicle *pCubicle2 = pCubicleConnFouned->GetCubicle2();
    ProjectExplorer::PeCubicle *pPassCubicle1 = pCubicleConnFouned->GetPassCubicle1();
    ProjectExplorer::PeCubicle *pPassCubicle2 = pCubicleConnFouned->GetPassCubicle2();
    ProjectExplorer::PePort *pPort1 = (pInfoSetCubicle1 == pCubicle1 ? pInfoSetPort1 : pInfoSetPort2);
    ProjectExplorer::PePort *pPort2 = (pInfoSetCubicle2 == pCubicle2 ? pInfoSetPort2 : pInfoSetPort1);
    bool bUseOdf1 = pCubicleConnFouned->GetUseOdf1();
    bool bUseOdf2 = pCubicleConnFouned->GetUseOdf2();

    if(!pPassCubicle1 && !pPassCubicle2)
    {
        if(bUseOdf1 && bUseOdf2) // Jump + Optical + Jump
        {
            // Validate Jump1
            ProjectExplorer::PeFiber *pFiberJump1 = 0;
            ProjectExplorer::PePort *pPortOdf1 = 0;
            foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
            {
                if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctJump)
                    continue;

                if(pFiber->GetPort1() == pPort1)
                {
                    if(pFiber->GetPort2()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                    {
                        pFiberJump1 = pFiber;
                        pPortOdf1 = pFiber->GetPort2();
                        break;
                    }
                }
                if(pFiber->GetPort2() == pPort1)
                {
                    if(pFiber->GetPort1()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                    {
                        pFiberJump1 = pFiber;
                        pPortOdf1 = pFiber->GetPort1();
                        break;
                    }
                }
            }
            if(!pFiberJump1 || !pPortOdf1)
                return false;
            lstFibers.removeAll(pFiberJump1);

            // Validate Jump2
            ProjectExplorer::PeFiber *pFiberJump2 = 0;
            ProjectExplorer::PePort *pPortOdf2 = 0;
            foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
            {
                if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctJump)
                    continue;

                if(pFiber->GetPort1() == pPort2)
                {
                    if(pFiber->GetPort2()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                    {
                        pFiberJump2 = pFiber;
                        pPortOdf2 = pFiber->GetPort2();
                        break;
                    }
                }
                if(pFiber->GetPort2() == pPort2)
                {
                    if(pFiber->GetPort1()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                    {
                        pFiberJump2 = pFiber;
                        pPortOdf2 = pFiber->GetPort1();
                        break;
                    }
                }
            }
            if(!pFiberJump2 || !pPortOdf2)
                return false;
            lstFibers.removeAll(pFiberJump2);

            // Validate Optical
            ProjectExplorer::PeFiber *pFiberOptical = 0;
            foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
            {
                if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctOptical)
                    continue;

                if((pFiber->GetPort1() == pPortOdf1 && pFiber->GetPort2() == pPortOdf2) ||
                   (pFiber->GetPort1() == pPortOdf2 && pFiber->GetPort2() == pPortOdf1))
                {
                    pFiberOptical = pFiber;
                    break;
                }
            }
            if(!pFiberOptical)
                return false;
            lstFibers.removeAll(pFiberOptical);
        }
        else  // Tail
        {
            ProjectExplorer::PeFiber *pFiberTail = 0;
            foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
            {
                if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctTail)
                    continue;

                if((pFiber->GetPort1() == pPort1 && pFiber->GetPort2() == pPort2) ||
                   (pFiber->GetPort1() == pPort2 && pFiber->GetPort2() == pPort1))
                {
                    pFiberTail = pFiber;
                    break;
                }
            }
            if(!pFiberTail)
                return false;
            lstFibers.removeAll(pFiberTail);
        }
    }
    else if(pPassCubicle1 && !pPassCubicle2) // Tail + Optical + Jump
    {
        // Validate Tail
        ProjectExplorer::PeFiber *pFiberTail = 0;
        ProjectExplorer::PePort *pPortOdf1 = 0;
        foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
        {
            if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctTail)
                continue;

            if(pFiber->GetPort1() == pPort1)
            {
                if(pFiber->GetPort2()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF &&
                   pFiber->GetPort2()->GetParentDevice()->GetParentCubicle() == pPassCubicle1)
                {
                    pFiberTail = pFiber;
                    pPortOdf1 = pFiber->GetPort2();
                    break;
                }
            }
            if(pFiber->GetPort2() == pPort1)
            {
                if(pFiber->GetPort1()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF &&
                   pFiber->GetPort1()->GetParentDevice()->GetParentCubicle() == pPassCubicle1)
                {
                    pFiberTail = pFiber;
                    pPortOdf1 = pFiber->GetPort1();
                    break;
                }
            }
        }
        if(!pFiberTail || !pPortOdf1)
            return false;
        lstFibers.removeAll(pFiberTail);

        // Validate Jump
        ProjectExplorer::PeFiber *pFiberJump = 0;
        ProjectExplorer::PePort *pPortOdf2 = 0;
        foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
        {
            if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctJump)
                continue;

            if(pFiber->GetPort1() == pPort2)
            {
                if(pFiber->GetPort2()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                {
                    pFiberJump = pFiber;
                    pPortOdf2 = pFiber->GetPort2();
                    break;
                }
            }
            if(pFiber->GetPort2() == pPort2)
            {
                if(pFiber->GetPort1()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                {
                    pFiberJump = pFiber;
                    pPortOdf2 = pFiber->GetPort1();
                    break;
                }
            }
        }
        if(!pFiberJump || !pPortOdf2)
            return false;
        lstFibers.removeAll(pFiberJump);

        // Validate Optical
        ProjectExplorer::PeFiber *pFiberOptical = 0;
        foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
        {
            if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctOptical)
                continue;

            if((pFiber->GetPort1() == pPortOdf1 && pFiber->GetPort2() == pPortOdf2) ||
               (pFiber->GetPort1() == pPortOdf2 && pFiber->GetPort2() == pPortOdf1))
            {
                pFiberOptical = pFiber;
                break;
            }
        }
        if(!pFiberOptical)
            return false;
        lstFibers.removeAll(pFiberOptical);
    }
    else if(!pPassCubicle1 && pPassCubicle2) // Jump + Optical + Tail
    {
        // Validate Jump
        ProjectExplorer::PeFiber *pFiberJump = 0;
        ProjectExplorer::PePort *pPortOdf1 = 0;
        foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
        {
            if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctJump)
                continue;

            if(pFiber->GetPort1() == pPort1)
            {
                if(pFiber->GetPort2()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                {
                    pFiberJump = pFiber;
                    pPortOdf1 = pFiber->GetPort2();
                    break;
                }
            }
            if(pFiber->GetPort2() == pPort1)
            {
                if(pFiber->GetPort1()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                {
                    pFiberJump = pFiber;
                    pPortOdf1 = pFiber->GetPort1();
                    break;
                }
            }
        }
        if(!pFiberJump || !pPortOdf1)
            return false;
        lstFibers.removeAll(pFiberJump);

        // Validate Tail
        ProjectExplorer::PeFiber *pFiberTail = 0;
        ProjectExplorer::PePort *pPortOdf2 = 0;
        foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
        {
            if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctTail)
                continue;

            if(pFiber->GetPort1() == pPort2)
            {
                if(pFiber->GetPort2()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF &&
                   pFiber->GetPort2()->GetParentDevice()->GetParentCubicle() == pPassCubicle2)
                {
                    pFiberTail = pFiber;
                    pPortOdf2 = pFiber->GetPort2();
                    break;
                }
            }
            if(pFiber->GetPort2() == pPort2)
            {
                if(pFiber->GetPort1()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF &&
                   pFiber->GetPort1()->GetParentDevice()->GetParentCubicle() == pPassCubicle2)
                {
                    pFiberTail = pFiber;
                    pPortOdf2 = pFiber->GetPort1();
                    break;
                }
            }
        }
        if(!pFiberTail || !pPortOdf2)
            return false;
        lstFibers.removeAll(pFiberTail);

        // Validate Optical
        ProjectExplorer::PeFiber *pFiberOptical = 0;
        foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
        {
            if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctOptical)
                continue;

            if((pFiber->GetPort1() == pPortOdf1 && pFiber->GetPort2() == pPortOdf2) ||
               (pFiber->GetPort1() == pPortOdf2 && pFiber->GetPort2() == pPortOdf1))
            {
                pFiberOptical = pFiber;
                break;
            }
        }
        if(!pFiberOptical)
            return false;
        lstFibers.removeAll(pFiberOptical);
    }
    else if(pPassCubicle1 && pPassCubicle2) // Tail + Optical + Tail
    {
        // Validate Tail1
        ProjectExplorer::PeFiber *pFiberTail1 = 0;
        ProjectExplorer::PePort *pPortOdf1 = 0;
        foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
        {
            if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctTail)
                continue;

            if(pFiber->GetPort1() == pPort1)
            {
                if(pFiber->GetPort2()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF &&
                   pFiber->GetPort2()->GetParentDevice()->GetParentCubicle() == pPassCubicle1)
                {
                    pFiberTail1 = pFiber;
                    pPortOdf1 = pFiber->GetPort2();
                    break;
                }
            }
            if(pFiber->GetPort2() == pPort1)
            {
                if(pFiber->GetPort1()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF &&
                   pFiber->GetPort1()->GetParentDevice()->GetParentCubicle() == pPassCubicle1)
                {
                    pFiberTail1 = pFiber;
                    pPortOdf1 = pFiber->GetPort1();
                    break;
                }
            }
        }
        if(!pFiberTail1 || !pPortOdf1)
            return false;
        lstFibers.removeAll(pFiberTail1);

        // Validate Tail2
        ProjectExplorer::PeFiber *pFiberTail2 = 0;
        ProjectExplorer::PePort *pPortOdf2 = 0;
        foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
        {
            if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctTail)
                continue;

            if(pFiber->GetPort1() == pPort2)
            {
                if(pFiber->GetPort2()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF &&
                   pFiber->GetPort2()->GetParentDevice()->GetParentCubicle() == pPassCubicle2)
                {
                    pFiberTail2 = pFiber;
                    pPortOdf2 = pFiber->GetPort2();
                    break;
                }
            }
            if(pFiber->GetPort2() == pPort2)
            {
                if(pFiber->GetPort1()->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtODF &&
                   pFiber->GetPort1()->GetParentDevice()->GetParentCubicle() == pPassCubicle2)
                {
                    pFiberTail2 = pFiber;
                    pPortOdf2 = pFiber->GetPort1();
                    break;
                }
            }
        }
        if(!pFiberTail2 || !pPortOdf2)
            return false;
        lstFibers.removeAll(pFiberTail2);

        // Validate Optical
        ProjectExplorer::PeFiber *pFiberOptical = 0;
        foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
        {
            if(pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctOptical)
                continue;

            if((pFiber->GetPort1() == pPortOdf1 && pFiber->GetPort2() == pPortOdf2) ||
               (pFiber->GetPort1() == pPortOdf2 && pFiber->GetPort2() == pPortOdf1))
            {
                pFiberOptical = pFiber;
                break;
            }
        }
        if(!pFiberOptical)
            return false;
        lstFibers.removeAll(pFiberOptical);
    }

    return true;
}

bool CableGenerator::FindValidInfoSetPath(QList<ProjectExplorer::PeFiber*> &lstFibers, ProjectExplorer::PePort *pInfoSetPort1, ProjectExplorer::PePort *pInfoSetPort2)
{
    if(!pInfoSetPort1 || !pInfoSetPort2)
        return false;

    QList<ProjectExplorer::PeFiber*> lstAllFibers;
    foreach(ProjectExplorer::PeCable *pCable, m_lstValidCables)
        lstAllFibers.append(pCable->GetChildFibers());

    lstFibers.clear();
    ProjectExplorer::PePort *pPortStart = pInfoSetPort1, *pPortEnd = pInfoSetPort2;
    bool bFound = false;
    while(!bFound)
    {
        bool bEnd = true;
        foreach(ProjectExplorer::PeFiber *pFiber, lstAllFibers)
        {
            if(lstFibers.contains(pFiber))
                continue;

            if((pFiber->GetPort1() == pPortStart && pFiber->GetPort2() == pPortEnd) ||
               (pFiber->GetPort2() == pPortStart && pFiber->GetPort1() == pPortEnd))
            {
                bFound = true;
                break;
            }

            if(pFiber->GetPort1() == pInfoSetPort1)
            {
                pPortStart = pFiber->GetPort2();
                lstFibers.append(pFiber);

                bEnd = false;
                break;
            }
            else if(pFiber->GetPort2() == pInfoSetPort1)
            {
                pPortStart = pFiber->GetPort1();
                lstFibers.append(pFiber);

                bEnd = false;
                break;
            }

            if(pFiber->GetPort1() == pInfoSetPort2)
            {
                pPortEnd = pFiber->GetPort2();
                lstFibers.append(pFiber);

                bEnd = false;
                break;
            }
            else if(pFiber->GetPort2() == pInfoSetPort2)
            {
                pPortEnd = pFiber->GetPort1();
                lstFibers.append(pFiber);

                bEnd = false;
                break;
            }
        }

        if(bEnd)
            break;
    }

    return bFound;
}

QList<ProjectExplorer::PeFiber*> CableGenerator::FindValidFibersFromPort(ProjectExplorer::PePort *pPort, const QList<ProjectExplorer::PeFiber*> &lstAllFibers)
{
    QList<ProjectExplorer::PeFiber*> lstFibers;
    foreach(ProjectExplorer::PeFiber *pFiber, lstAllFibers)
    {
        if(pFiber->GetPort1() == pPort || pFiber->GetPort2() == pPort)
            lstFibers.append(pFiber);
    }

    return lstFibers;
}

bool CableGenerator::CheckFiberReuse(_Fiber *pFiber, ProjectExplorer::PeFiber *pFiberValid, bool bCheckSet)
{
    if(!pFiber || !pFiberValid)
        return false;

    _Cable *pCable = pFiber->pCable;
    ProjectExplorer::PeCable *pCableValid = pFiberValid->GetParentCable();
    if(!pCable || !pCableValid)
        return false;

    ProjectExplorer::PeCable::CableType eCableType;
    ProjectExplorer::PePort *pPort1 = 0, *pPort2 = 0;
    if(m_lstJumps.contains(pCable))
    {
        eCableType = ProjectExplorer::PeCable::ctJump;
        pPort1 = pFiber->pJumpPort1;
        pPort2 = pFiber->pJumpPort2;
    }
    else if(m_lstTails.contains(pCable))
    {
        eCableType = ProjectExplorer::PeCable::ctTail;

        pPort1 = pFiber->mapCubicleToPort.value(pCable->pCubicle1);
        pPort2 = pFiber->mapCubicleToPort.value(pCable->pCubicle2);
    }
    else if(m_lstOpticals.contains(pCable))
    {
        eCableType = ProjectExplorer::PeCable::ctOptical;

        pPort1 = pFiber->mapCubicleToPort.value(pCable->pCubicle1);
        pPort2 = pFiber->mapCubicleToPort.value(pCable->pCubicle2);
    }
    else
    {
        return false;
    }

    if(eCableType != pCableValid->GetCableType())
        return false;

    if(bCheckSet && (eCableType != ProjectExplorer::PeCable::ctJump) && (pCable->strSet != pCableValid->GetNameSet()))
        return false;

    if((pCable->pCubicle1 != pCableValid->GetCubicle1() && pCable->pCubicle1 != pCableValid->GetCubicle2()) ||
       (pCable->pCubicle2 != pCableValid->GetCubicle1() && pCable->pCubicle2 != pCableValid->GetCubicle2()))
    {
        return false;
    }

    if((pPort1 && pPort1 != pFiberValid->GetPort1() && pPort1 != pFiberValid->GetPort2()) ||
       (pPort2 && pPort2 != pFiberValid->GetPort1() && pPort2 != pFiberValid->GetPort2()))
    {
        return false;
    }

    return true;
}

void CableGenerator::AddCubicleInfoSet(ProjectExplorer::PeCubicle *pCubicle1, ProjectExplorer::PeCubicle *pCubicle2, ProjectExplorer::PeInfoSet *pInfoSet, ProjectExplorer::PePort *pPort1, ProjectExplorer::PePort *pPort2)
{
    if(!pCubicle1 || !pCubicle2 || !pInfoSet || !pPort1 || !pPort2)
        return;

    _CubicleInfoSet *pCubicleInfoSetFound = FindCubicleInfoSet(pCubicle1, pCubicle2);
    if(!pCubicleInfoSetFound)
    {
        pCubicleInfoSetFound = new _CubicleInfoSet;
        pCubicleInfoSetFound->pCubicle1 = pCubicle1;
        pCubicleInfoSetFound->pCubicle2 = pCubicle2;
        m_lstCubicleInfoSets.append(pCubicleInfoSetFound);
    }

    _InfoSetConn *pInfoSetConnFound = 0;
    if(pCubicle1 != pCubicle2)
    {
        foreach(_InfoSetConn *pInfoSetConn, pCubicleInfoSetFound->lstInfoSetConns)
        {
            if(pInfoSetConn->mapCubicleToPort.value(pCubicle1) == pPort1 && pInfoSetConn->mapCubicleToPort.value(pCubicle2) == pPort2)
            {
                pInfoSetConnFound = pInfoSetConn;
                break;
            }
        }
    }
    else
    {
        foreach(_InfoSetConn *pInfoSetConn, pCubicleInfoSetFound->lstInfoSetConns)
        {
            if((pInfoSetConn->pJumpPort1 == pPort1 && pInfoSetConn->pJumpPort2 == pPort2) ||
               (pInfoSetConn->pJumpPort1 == pPort2 && pInfoSetConn->pJumpPort2 == pPort1))
            {
                pInfoSetConnFound = pInfoSetConn;
                break;
            }
        }
    }

    if(!pInfoSetConnFound)
    {
        pInfoSetConnFound = new _InfoSetConn;

        QString strSet;
        ProjectExplorer::PeInfoSet *pInfoSetTemp = (pInfoSet->GetInfoSetType() == ProjectExplorer::PeInfoSet::itNone ? pInfoSet->GetPairInfoSet() : pInfoSet);
        pInfoSetTemp->GetSetIndexAndMainDevice(strSet);
        if(strSet.isEmpty())
            strSet = "A";

        pInfoSetConnFound->strSet = strSet;
        if(pCubicle1 != pCubicle2)
        {
            pInfoSetConnFound->mapCubicleToPort.insert(pCubicle1, pPort1);
            pInfoSetConnFound->mapCubicleToPort.insert(pCubicle2, pPort2);
        }
        else
        {
            pInfoSetConnFound->pJumpPort1 = pPort1;
            pInfoSetConnFound->pJumpPort2 = pPort2;
        }
        pCubicleInfoSetFound->lstInfoSetConns.append(pInfoSetConnFound);
    }

    pInfoSetConnFound->lstInfoSets.append(pInfoSet);
}

CableGenerator::_CubicleInfoSet* CableGenerator::FindCubicleInfoSet(ProjectExplorer::PeCubicle *pCubicle1, ProjectExplorer::PeCubicle *pCubicle2)
{
    foreach(_CubicleInfoSet *pCubicleInfoSet, m_lstCubicleInfoSets)
    {
        if((pCubicleInfoSet->pCubicle1 == pCubicle1 && pCubicleInfoSet->pCubicle2 == pCubicle2) ||
           (pCubicleInfoSet->pCubicle1 == pCubicle2 && pCubicleInfoSet->pCubicle2 == pCubicle1))
        {
            return pCubicleInfoSet;
        }
    }

    return 0;
}

CableGenerator::_Cable* CableGenerator::FindCableOptical(ProjectExplorer::PeCubicle *pCubicle1, ProjectExplorer::PeCubicle *pCubicle2, const QString &strSet)
{
    foreach(_Cable *pCable, m_lstOpticals)
    {
        if(pCable->strSet != strSet)
            continue;

        if((pCable->pCubicle1 == pCubicle1 && pCable->pCubicle2 == pCubicle2) ||
           (pCable->pCubicle1 == pCubicle2 && pCable->pCubicle2 == pCubicle1))
        {
            return pCable;
        }
    }

    return 0;
}

CableGenerator::_Cable* CableGenerator::FindCableTail(ProjectExplorer::PeCubicle *pCubicle1, ProjectExplorer::PeCubicle *pCubicle2, const QString &strSet)
{
    foreach(_Cable *pCable, m_lstTails)
    {
        if(pCable->strSet != strSet)
            continue;

        if((pCable->pCubicle1 == pCubicle1 && pCable->pCubicle2 == pCubicle2) ||
           (pCable->pCubicle1 == pCubicle2 && pCable->pCubicle2 == pCubicle1))
        {
            return pCable;
        }
    }

    return 0;
}

CableGenerator::_Cable* CableGenerator::FindCableJump(ProjectExplorer::PeCubicle *pCubicle)
{
    foreach(_Cable *pCable, m_lstJumps)
    {
        if(pCable->pCubicle1 == pCubicle && pCable->pCubicle2 == pCubicle)
            return pCable;
    }

    return 0;
}

CableGenerator::_Fiber* CableGenerator::FindPairFiber(_Fiber *pFiber)
{
    if(!pFiber)
        return 0;

    _Cable *pCable = pFiber->pCable;
    if(!pCable)
        return 0;

    foreach(ProjectExplorer::PeInfoSet *pInfoSet, pFiber->lstInfoSets)
    {
        ProjectExplorer::PeInfoSet *pInfoSetPair = pInfoSet->GetPairInfoSet();
        if(!pInfoSetPair)
            return 0;

        foreach(_Fiber *pFiberPair, pCable->lstFibers)
        {
            if(pFiberPair->lstInfoSets.contains(pInfoSetPair))
                return pFiberPair;
        }
    }

    return 0;
}

int CableGenerator::CalculateClosestFiberNumber(int iFiberNumber, bool bOptical) const
{
    const QList<int> lstFiberNumbers = bOptical ? m_lstOpticalFiberNumbers : m_lstTailFiberNumbers;

    for(int i = 0; i < lstFiberNumbers.size(); i++)
    {
        if(iFiberNumber <= lstFiberNumbers.at(i))
            return lstFiberNumbers.at(i);
    }

    return 0;
}

int CableGenerator::CalculatePipeNumber(int iFiberNumber) const
{
    foreach(int iPipeNumber, g_lstPipeNumber)
    {
        if(iFiberNumber % iPipeNumber == 0)
            return iPipeNumber;
    }

    return g_lstPipeNumber.last();
}

ProjectExplorer::PeDevice* CableGenerator::FindValidOdf(ProjectExplorer::PeCubicle *pCubicle, const QString &strSet, int &iOdfLayerPortNumber, int iUnusedOdfPortNumber, bool bCreate)
{
    if(!pCubicle)
        return 0;

    ProjectExplorer::PeDevice *pOdfReturned = 0;

    foreach(ProjectExplorer::PeDevice *pOdf, m_lstValidOdfs)
    {
        const QList<ProjectExplorer::PeBoard*> lstOdfLayers = pOdf->GetChildBoards();
        const int iUsedLayerSize = lstOdfLayers.size();
        if(pOdf->GetParentCubicle() == pCubicle && pOdf->GetSet() == strSet && iUsedLayerSize < 26)
        {
            iOdfLayerPortNumber = m_iOdfLayerPortNumber;
            if(lstOdfLayers.size() > 0)
                iOdfLayerPortNumber = lstOdfLayers.first()->GetChildPorts().size();

            if(((26 - iUsedLayerSize) * iOdfLayerPortNumber) < iUnusedOdfPortNumber)
                continue;

            pOdfReturned = pOdf;
            break;
        }
    }

    if(!pOdfReturned && bCreate)
    {
        iOdfLayerPortNumber = m_iOdfLayerPortNumber;

        pOdfReturned = new ProjectExplorer::PeDevice(m_pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        pOdfReturned->SetDeviceType(ProjectExplorer::PeDevice::dtODF);
        pOdfReturned->SetSet(strSet);
        pOdfReturned->SetParentCubicle(pCubicle);

        QString strOdfName = m_strOdfNameRule;
        strOdfName.replace("%{Set}", strSet, Qt::CaseInsensitive);

        ProjectExplorer::PeDevice *pOdfFound = 0;
        foreach(ProjectExplorer::PeDevice *pOdf, m_lstValidOdfs)
        {
            if(pOdf->GetParentCubicle() == pOdfReturned->GetParentCubicle() && pOdf->GetName() == strOdfName)
            {
                pOdfFound = pOdf;
                break;
            }
        }

        int iIndex = 1;
        while(pOdfFound)
        {
            pOdfFound = 0;
            QString strNewOdfName = QString("%1%2").arg(strOdfName).arg(iIndex++);

            foreach(ProjectExplorer::PeDevice *pOdf, m_lstValidOdfs)
            {
                if(pOdf->GetParentCubicle() == pOdfReturned->GetParentCubicle() && pOdf->GetName() == strNewOdfName)
                {
                    pOdfFound = pOdf;
                    break;
                }
            }

            if(!pOdfFound)
                strOdfName = strNewOdfName;
        }

        pOdfReturned->SetName(strOdfName);
        m_lstValidOdfs.append(pOdfReturned);
    }

    return pOdfReturned;
}

void CableGenerator::CalculateOpticalCableName(_Cable *pCableTemp, ProjectExplorer::PeCable *pCable)
{
    static QString strOldBayName;

    QString strBayNumber;
    if(pCableTemp->bPass)
    {
        strBayNumber = "ZJ";
    }
    else
    {
        ProjectExplorer::PeInfoSet *pInfoSet = 0;
        foreach(_Fiber *pFiber, pCableTemp->lstFibers)
        {
            if(!pFiber->lstInfoSets.isEmpty())
            {
                pInfoSet = pFiber->lstInfoSets.first();
                break;
            }
        }

        if(pInfoSet && pInfoSet->GetInfoSetType() == ProjectExplorer::PeInfoSet::itNone)
            pInfoSet = pInfoSet->GetPairInfoSet();

        if(pInfoSet)
        {
            QStringList lstInfoSetSections = pInfoSet->GetName().split(QLatin1String("-"));
            if(!lstInfoSetSections.isEmpty())
                strBayNumber = lstInfoSetSections.first();
        }
    }

    if(strBayNumber.isEmpty())
        strBayNumber = strOldBayName;
    else
        strOldBayName = strBayNumber;

    int iOpticalIndex = 100;
    if(pCableTemp->pCubicle1->IsYardCubile() && pCableTemp->pCubicle2->IsYardCubile())
        iOpticalIndex = 180;

    QStringList lstAllOpticalCableNames;
    foreach(ProjectExplorer::PeCable *pValidCable, m_lstValidCables)
    {
        if(pValidCable->GetCableType() == ProjectExplorer::PeCable::ctOptical)
            lstAllOpticalCableNames.append(pValidCable->GetName());
    }

    pCable->SetNameBay(strBayNumber);
    pCable->SetNameSet(pCableTemp->strSet);
    for(int i = 0; i < 999; i++)
    {
        pCable->SetNameNumber(iOpticalIndex++);
        pCable->SetName(pCable->CalculateNameByRule(m_strOpticalNameRule));

        if(!lstAllOpticalCableNames.contains(pCable->GetName()))
            break;
    }
}

void CableGenerator::CalculateTailCableName(_Cable *pCableTemp, ProjectExplorer::PeCable *pCable)
{
    ProjectExplorer::PeInfoSet *pInfoSet = 0;
    foreach(_Fiber *pFiber, pCableTemp->lstFibers)
    {
        if(!pFiber->lstInfoSets.isEmpty())
        {
            pInfoSet = pFiber->lstInfoSets.first();
            break;
        }
    }

    if(pInfoSet && pInfoSet->GetInfoSetType() == ProjectExplorer::PeInfoSet::itNone)
        pInfoSet = pInfoSet->GetPairInfoSet();

    if(!pInfoSet)
        return;

    QStringList lstInfoSetSections = pInfoSet->GetName().split(QLatin1String("-"));
    if(lstInfoSetSections.isEmpty())
        return;

    const QString strBayNumber = lstInfoSetSections.first();

    QStringList lstAllTailCableNames;
    foreach(ProjectExplorer::PeCable *pValidCable, m_lstValidCables)
    {
        if(pValidCable->GetCableType() == ProjectExplorer::PeCable::ctTail)
            lstAllTailCableNames.append(pValidCable->GetName());
    }

    pCable->SetNameBay(strBayNumber);
    pCable->SetNameSet(pCableTemp->strSet);
    int iTailIndex = 130;
    for(int i = 0; i < 999; i++)
    {
        pCable->SetNameNumber(iTailIndex++);
        pCable->SetName(pCable->CalculateNameByRule(m_strTailNameRule));

        if(!lstAllTailCableNames.contains(pCable->GetName()))
            break;
    }
}

void CableGenerator::CalculateJumpCableName(_Cable *pCableTemp, ProjectExplorer::PeCable *pCable)
{
    QStringList lstAllJumpCableNames;
    foreach(ProjectExplorer::PeCable *pValidCable, m_lstValidCables)
    {
        if(pValidCable->GetCableType() == ProjectExplorer::PeCable::ctJump)
        {
            if((pValidCable->GetCubicle1() == pCableTemp->pCubicle1 && pValidCable->GetCubicle2() == pCableTemp->pCubicle2) ||
               (pValidCable->GetCubicle1() == pCableTemp->pCubicle2 && pValidCable->GetCubicle2() == pCableTemp->pCubicle1))
            {
                lstAllJumpCableNames.append(pValidCable->GetName());
            }
        }
    }

    int iJumpIndex = 1;
    for(int i = 0; i < 999; i++)
    {
        pCable->SetNameNumber(iJumpIndex++);
        pCable->SetName(pCable->CalculateNameByRule(m_strJumpNameRule));

        if(!lstAllJumpCableNames.contains(pCable->GetName()))
            break;
    }
}

bool CableGenerator::GenerateReserveFiberAndOdf(int iAddedFiberNumber,
                                                ProjectExplorer::PeCable *pCable,
                                                QList<ProjectExplorer::PeFiber> &lstAddedFibers,
                                                QList<ProjectExplorer::PeBoard> &lstAddedOdfLayers1,
                                                QList<ProjectExplorer::PePort> &lstAddedOdfPorts1,
                                                QList<ProjectExplorer::PeBoard> &lstAddedOdfLayers2,
                                                QList<ProjectExplorer::PePort> &lstAddedOdfPorts2)
{
    if(iAddedFiberNumber < 0)
        return false;

    if(!pCable)
        return false;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pCable->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    ProjectExplorer::PeCubicle *pCubicle1 = pCable->GetCubicle1();
    ProjectExplorer::PeCubicle *pCubicle2 = pCable->GetCubicle2();
    if(!pCubicle1 || !pCubicle2)
        return false;

    // Prepare odf port for Cubicle1
    ProjectExplorer::PeDevice *pOdf1 = 0;
    QList<ProjectExplorer::PeBoard*> lstReusedOdfLayers1;
    foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
    {
        ProjectExplorer::PePort *pPort = 0;
        if(pFiber->GetPort1()->GetParentDevice()->GetParentCubicle() == pCubicle1)
            pPort = pFiber->GetPort1();
        else if(pFiber->GetPort2()->GetParentDevice()->GetParentCubicle() == pCubicle1)
            pPort = pFiber->GetPort2();

        if(!pPort)
            continue;

        if(!pOdf1)
            pOdf1 = pPort->GetParentDevice();

        if(!lstReusedOdfLayers1.contains(pPort->GetParentBoard()))
            lstReusedOdfLayers1.append(pPort->GetParentBoard());
    }
    qSort(lstReusedOdfLayers1.begin(), lstReusedOdfLayers1.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    if(!pOdf1)
        return false;

    QList<ProjectExplorer::PeBoard*> lstAllOdfLayers1 = pOdf1->GetChildBoards();
    qSort(lstAllOdfLayers1.begin(), lstAllOdfLayers1.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    foreach(ProjectExplorer::PeBoard *pLayer, lstAllOdfLayers1)
    {
        if(!lstReusedOdfLayers1.contains(pLayer))
            lstReusedOdfLayers1.append(pLayer);
    }

    QList<ProjectExplorer::PePort*> lstPortsForAddFiber1;
    foreach(ProjectExplorer::PeBoard *pOldLayer, lstReusedOdfLayers1)
    {
        foreach(ProjectExplorer::PePort *pPort, pOldLayer->GetChildPorts())
        {
            if(pPort->GetUsedFibers().isEmpty())
                lstPortsForAddFiber1.append(pPort);

            if(lstPortsForAddFiber1.size() >= iAddedFiberNumber)
                break;
        }

        if(lstPortsForAddFiber1.size() >= iAddedFiberNumber)
            break;
    }

    const int iOdfPortSizeAdded1 = iAddedFiberNumber - lstPortsForAddFiber1.size();
    if(iOdfPortSizeAdded1 > 0)
    {
        QStringList lstExistLayerNames;
        foreach(ProjectExplorer::PeBoard *pLayer, pOdf1->GetChildBoards())
            lstExistLayerNames.append(pLayer->GetPosition());

        int iOdfLayerSizeAdded = (iOdfPortSizeAdded1 % g_iOdfLayerPortNum == 0) ? (iOdfPortSizeAdded1 / g_iOdfLayerPortNum) : (iOdfPortSizeAdded1 / g_iOdfLayerPortNum + 1);
        for(int i = 0; i < iOdfLayerSizeAdded; i++)
        {
            char chLayer = 'A';
            while(lstExistLayerNames.contains(QString(chLayer)))
                chLayer++;
            lstExistLayerNames.append(QString(chLayer));

            ProjectExplorer::PeBoard TempLayer(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            TempLayer.SetParentDevice(pOdf1);
            TempLayer.SetPosition(QString(chLayer));
            lstAddedOdfLayers1.append(TempLayer);

            ProjectExplorer::PeBoard &layer = lstAddedOdfLayers1.last();
            for(int j = 0; j < g_iOdfLayerPortNum; j++)
            {
                ProjectExplorer::PePort TempPort(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                TempPort.SetName(QString("%1%2").arg(layer.GetPosition()).arg(j + 1, 2, 10, QLatin1Char('0')));
                TempPort.SetGroup(j + 1);
                TempPort.SetPortType(ProjectExplorer::PePort::ptOdf);
                TempPort.SetPortDirection(ProjectExplorer::PePort::pdRT);
                TempPort.SetFiberPlug(ProjectExplorer::PePort::fpST);
                TempPort.SetFiberMode(ProjectExplorer::PePort::fmNone);
                TempPort.SetParentBoard(&layer);
                lstAddedOdfPorts1.append(TempPort);

                ProjectExplorer::PePort &port = lstAddedOdfPorts1.last();
                lstPortsForAddFiber1.append(&port);
            }
        }
    }

    // Prepare odf port for Cubicle2
    ProjectExplorer::PeDevice *pOdf2 = 0;
    QList<ProjectExplorer::PeBoard*> lstReusedOdfLayers2;
    foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
    {
        ProjectExplorer::PePort *pPort = 0;
        if(pFiber->GetPort1()->GetParentDevice()->GetParentCubicle() == pCubicle2)
            pPort = pFiber->GetPort1();
        else if(pFiber->GetPort2()->GetParentDevice()->GetParentCubicle() == pCubicle2)
            pPort = pFiber->GetPort2();

        if(!pPort)
            continue;

        if(!pOdf2)
            pOdf2 = pPort->GetParentDevice();

        if(!lstReusedOdfLayers2.contains(pPort->GetParentBoard()))
            lstReusedOdfLayers2.append(pPort->GetParentBoard());
    }
    qSort(lstReusedOdfLayers2.begin(), lstReusedOdfLayers2.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    if(!pOdf2)
        return false;

    QList<ProjectExplorer::PeBoard*> lstAllOdfLayers2 = pOdf2->GetChildBoards();
    qSort(lstAllOdfLayers2.begin(), lstAllOdfLayers2.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    foreach(ProjectExplorer::PeBoard *pLayer, lstAllOdfLayers2)
    {
        if(!lstReusedOdfLayers2.contains(pLayer))
            lstReusedOdfLayers2.append(pLayer);
    }

    QList<ProjectExplorer::PePort*> lstPortsForAddFiber2;
    foreach(ProjectExplorer::PeBoard *pOldLayer, lstReusedOdfLayers2)
    {
        foreach(ProjectExplorer::PePort *pPort, pOldLayer->GetChildPorts())
        {
            if(pPort->GetUsedFibers().isEmpty())
                lstPortsForAddFiber2.append(pPort);

            if(lstPortsForAddFiber2.size() >= iAddedFiberNumber)
                break;
        }

        if(lstPortsForAddFiber2.size() >= iAddedFiberNumber)
            break;
    }

    const int iOdfPortSizeAdded2 = iAddedFiberNumber - lstPortsForAddFiber2.size();
    if(iOdfPortSizeAdded2 > 0)
    {
        QStringList lstExistLayerNames;
        foreach(ProjectExplorer::PeBoard *pLayer, pOdf2->GetChildBoards())
            lstExistLayerNames.append(pLayer->GetPosition());

        int iOdfLayerSizeAdded = (iOdfPortSizeAdded2 % g_iOdfLayerPortNum == 0) ? (iOdfPortSizeAdded2 / g_iOdfLayerPortNum) : (iOdfPortSizeAdded2 / g_iOdfLayerPortNum + 1);
        for(int i = 0; i < iOdfLayerSizeAdded; i++)
        {
            char chLayer = 'A';
            while(lstExistLayerNames.contains(QString(chLayer)))
                chLayer++;
            lstExistLayerNames.append(QString(chLayer));

            ProjectExplorer::PeBoard TempLayer(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            TempLayer.SetParentDevice(pOdf2);
            TempLayer.SetPosition(QString(chLayer));
            lstAddedOdfLayers2.append(TempLayer);

            ProjectExplorer::PeBoard &layer = lstAddedOdfLayers2.last();
            for(int j = 0; j < g_iOdfLayerPortNum; j++)
            {
                ProjectExplorer::PePort TempPort(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                TempPort.SetName(QString("%1%2").arg(layer.GetPosition()).arg(j + 1, 2, 10, QLatin1Char('0')));
                TempPort.SetGroup(j + 1);
                TempPort.SetPortType(ProjectExplorer::PePort::ptOdf);
                TempPort.SetPortDirection(ProjectExplorer::PePort::pdRT);
                TempPort.SetFiberPlug(ProjectExplorer::PePort::fpST);
                TempPort.SetFiberMode(ProjectExplorer::PePort::fmNone);
                TempPort.SetParentBoard(&layer);
                lstAddedOdfPorts2.append(TempPort);

                ProjectExplorer::PePort &port = lstAddedOdfPorts2.last();
                lstPortsForAddFiber2.append(&port);
            }
        }
    }

    for(int i = 0; i < iAddedFiberNumber; i++)
    {
        ProjectExplorer::PeFiber fiber(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        fiber.SetReserve(true);
        fiber.SetPort1(lstPortsForAddFiber1.at(i));
        fiber.SetPort2(lstPortsForAddFiber2.at(i));
        lstAddedFibers.append(fiber);
    }

    return true;
}
