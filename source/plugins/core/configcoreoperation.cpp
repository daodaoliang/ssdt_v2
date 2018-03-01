#include <QApplication>
#include <QMap>
#include <QFileInfo>
#include <QDateTime>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "sclparser/scldoccontrol.h"
#include "sclparser/scldocument.h"
#include "sclparser/sclelement.h"
#include "sclparser/sclattribute.h"
#include "sclparser/sclhelp.h"
#include "sclparser/sclparser_constants.h"
#include "projectexplorer/peproject.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/peport.h"
#include "projectexplorer/peinfoset.h"
#include "projectexplorer/pecubicleconn.h"
#include "projectexplorer/pevterminal.h"
#include "projectexplorer/pestrap.h"
#include "projectexplorer/pevterminalconn.h"
#include "projectexplorer/pecable.h"
#include "projectexplorer/pefiber.h"
#include "projectexplorer/basemanager.h"
#include "projectexplorer/pblibdevice.h"
#include "projectexplorer/pblibboard.h"
#include "projectexplorer/pblibport.h"

#include "configcoreoperation.h"
#include "vterminaldefine.h"
#include "mainwindow.h"

static bool CompareSCLElement(SclParser::SCLElement *pSCLElementIcd, SclParser::SCLElement *pSCLElementScd)
{
    Q_ASSERT(pSCLElementIcd && pSCLElementScd);

    // Compare the tag name
    if(pSCLElementIcd->GetTag() != pSCLElementScd->GetTag())
        return false;

    // Compare the attribute
    QMap<QString, QString> mapAttributesIcd, mapAttributesScd;
    foreach(SclParser::SCLAttribute *pSCLAttribute, pSCLElementIcd->GetAttributeList())
    {
        if(!pSCLAttribute->GetName().isEmpty() && pSCLAttribute->GetName() != "desc")
            mapAttributesIcd.insert(pSCLAttribute->GetName(), pSCLAttribute->GetValue());
    }
    foreach(SclParser::SCLAttribute *pSCLAttribute, pSCLElementScd->GetAttributeList())
    {
        if(!pSCLAttribute->GetName().isEmpty() && pSCLAttribute->GetName() != "desc")
            mapAttributesScd.insert(pSCLAttribute->GetName(), pSCLAttribute->GetValue());
    }

    if(pSCLElementIcd->GetTag() == "DO")
    {
        if(!mapAttributesIcd.contains("transient"))
            mapAttributesIcd.insert("transient", "false");

        if(!mapAttributesScd.contains("transient"))
            mapAttributesScd.insert("transient", "false");
    }
    else if(pSCLElementIcd->GetTag() == "DA")
    {
        if(!mapAttributesIcd.contains("dchg"))
            mapAttributesIcd.insert("dchg", "false");
        if(!mapAttributesIcd.contains("qchg"))
            mapAttributesIcd.insert("qchg", "false");
        if(!mapAttributesIcd.contains("dupd"))
            mapAttributesIcd.insert("dupd", "false");
        if(!mapAttributesIcd.contains("valKind"))
            mapAttributesIcd.insert("valKind", "Set");
        if(!mapAttributesIcd.contains("count"))
            mapAttributesIcd.insert("count", "0");

        if(!mapAttributesScd.contains("dchg"))
            mapAttributesScd.insert("dchg", "false");
        if(!mapAttributesScd.contains("qchg"))
            mapAttributesScd.insert("qchg", "false");
        if(!mapAttributesScd.contains("dupd"))
            mapAttributesScd.insert("dupd", "false");
        if(!mapAttributesScd.contains("valKind"))
            mapAttributesScd.insert("valKind", "Set");
        if(!mapAttributesScd.contains("count"))
            mapAttributesScd.insert("count", "0");
    }
    else if(pSCLElementIcd->GetTag() == "BDA")
    {
        if(!mapAttributesIcd.contains("valKind"))
            mapAttributesIcd.insert("valKind", "Set");
        if(!mapAttributesIcd.contains("count"))
            mapAttributesIcd.insert("count", "0");

        if(!mapAttributesScd.contains("valKind"))
            mapAttributesScd.insert("valKind", "Set");
        if(!mapAttributesScd.contains("count"))
            mapAttributesScd.insert("count", "0");
    }

    if(mapAttributesIcd.size() != mapAttributesScd.size())
        return false;

    foreach(QString strAttribute, mapAttributesIcd.keys())
    {
        if(!mapAttributesScd.contains(strAttribute))
            return false;

        if(mapAttributesIcd.value(strAttribute) != mapAttributesScd.value(strAttribute))
            return false;
    }

    // Compare the child number
    if(pSCLElementIcd->GetChildList().size() != pSCLElementScd->GetChildList().size())
        return false;

    // Compare the child element
    foreach(SclParser::SCLElement *pSCLElementIcdChild, pSCLElementIcd->GetChildList())
    {
        QMap<QString, QString> mapAttValue;

        if(pSCLElementIcdChild->FindAttribute("name"))
            mapAttValue.insert("name", pSCLElementIcdChild->GetAttribute("name"));
        else if(pSCLElementIcdChild->FindAttribute("id"))
            mapAttValue.insert("id", pSCLElementIcdChild->GetAttribute("id"));
        else if(pSCLElementIcdChild->FindAttribute("ord"))
            mapAttValue.insert("ord", pSCLElementIcdChild->GetAttribute("ord"));

        SclParser::SCLElement* pSCLElementScdChild = 0;
        if(mapAttValue.isEmpty() && pSCLElementIcdChild->GetTag() == "Val")
            pSCLElementScdChild = pSCLElementScd->GetFirstChild("Val");
        else
            pSCLElementScdChild = pSCLElementScd->FindChild(mapAttValue);

        if(pSCLElementScdChild)
        {
            if(!CompareSCLElement(pSCLElementIcdChild, pSCLElementScdChild))
                return false;
        }
        else
        {
            return false;
        }
    }

    return true;
}

static void HandleEnumTypeConflict(SclParser::SCLElement *pSCLElementDataTypeTemplatesIcd, SclParser::SCLElement *pSCLElementDataTypeTemplatesScd)
{
    ///////////////////////////////////////////////////////
    // Check Conflict EnumType
    ///////////////////////////////////////////////////////
    QMap<QString, QString> mapConfilictIdToNewID;
    QList<SclParser::SCLElement*> lstNewTypeIntoScd;
    foreach(SclParser::SCLElement *pSCLElementEnumTypeIcd, pSCLElementDataTypeTemplatesIcd->FindChildByType("EnumType"))
    {
        const QString strID = pSCLElementEnumTypeIcd->GetAttribute("id");
        if(strID.isEmpty())
            continue;

        if(SclParser::SCLElement *pSCLElementEnumTypeScd = pSCLElementDataTypeTemplatesScd->FindChild("EnumType", "id", strID, false))
        {
            if(CompareSCLElement(pSCLElementEnumTypeIcd, pSCLElementEnumTypeScd))
                continue;

            const QString strNewID = QString("AutoPrefix%1_%2").arg(QDateTime::currentDateTime().toTime_t()).arg(strID);
            pSCLElementEnumTypeIcd->SetAttribute("id", strNewID, false);
            mapConfilictIdToNewID.insert(strID, strNewID);
        }

        lstNewTypeIntoScd.append(pSCLElementEnumTypeIcd);
    }

    //////////////////////////////////////////////////////////////
    // Update BDA refering to the EnumType which will add prefix
    //////////////////////////////////////////////////////////////
    foreach(SclParser::SCLElement *pSCLElementDAType, pSCLElementDataTypeTemplatesIcd->FindChildByType("DAType"))
    {
        foreach(SclParser::SCLElement *pSCLElementBDA, pSCLElementDAType->FindChildByType("BDA"))
        {
            if(pSCLElementBDA->GetAttribute("bType") != "Enum")
                continue;

            QString strType = pSCLElementBDA->GetAttribute("type");
            if(mapConfilictIdToNewID.contains(strType))
                pSCLElementBDA->SetAttribute("type", mapConfilictIdToNewID.value(strType), false);
        }
    }

    //////////////////////////////////////////////////////////////
    // Update DA refering to the EnumType which will add prefix
    //////////////////////////////////////////////////////////////
    foreach(SclParser::SCLElement *pSCLElementDOType, pSCLElementDataTypeTemplatesIcd->FindChildByType("DOType"))
    {
        foreach(SclParser::SCLElement *pSCLElementDA, pSCLElementDOType->FindChildByType("DA"))
        {
            if(pSCLElementDA->GetAttribute("bType") != "Enum")
                continue;

            QString strType = pSCLElementDA->GetAttribute("type");
            if(mapConfilictIdToNewID.contains(strType))
                pSCLElementDA->SetAttribute("type", mapConfilictIdToNewID.value(strType), false);
        }
    }

    //////////////////////////////////////////////////////////////
    // Move the EnumType from Icd Template to Scd Template
    //////////////////////////////////////////////////////////////
    foreach(SclParser::SCLElement *pSCLElementEnumType, lstNewTypeIntoScd)
        pSCLElementDataTypeTemplatesScd->AttachChild(pSCLElementEnumType->Clone(true), -1, false);
}

static void HandleDATypeConflict(SclParser::SCLElement *pSCLElementDataTypeTemplatesIcd, SclParser::SCLElement *pSCLElementDataTypeTemplatesScd)
{
    ///////////////////////////////////////////////////////
    // Check Conflict DAType
    ///////////////////////////////////////////////////////
    QMap<QString, QString> mapConfilictIdToNewID;
    QList<SclParser::SCLElement*> lstNewTypeIntoScd;
    foreach(SclParser::SCLElement *pSCLElementDATypeIcd, pSCLElementDataTypeTemplatesIcd->FindChildByType("DAType"))
    {
        const QString strID = pSCLElementDATypeIcd->GetAttribute("id");
        if(strID.isEmpty())
            continue;

        if(SclParser::SCLElement *pSCLElementDATypeScd = pSCLElementDataTypeTemplatesScd->FindChild("DAType", "id", strID, false))
        {
            if(CompareSCLElement(pSCLElementDATypeIcd, pSCLElementDATypeScd))
                continue;

            const QString strNewID = QString("AutoPrefix%1_%2").arg(QDateTime::currentDateTime().toTime_t()).arg(strID);
            pSCLElementDATypeIcd->SetAttribute("id", strNewID, false);
            mapConfilictIdToNewID.insert(strID, strNewID);
        }

        lstNewTypeIntoScd.append(pSCLElementDATypeIcd);
    }

    //////////////////////////////////////////////////////////////
    // Update BDA refering to the DAType which will add prefix
    //////////////////////////////////////////////////////////////
    foreach(SclParser::SCLElement *pSCLElementDAType, pSCLElementDataTypeTemplatesIcd->FindChildByType("DAType"))
    {
        foreach(SclParser::SCLElement *pSCLElementBDA, pSCLElementDAType->FindChildByType("BDA"))
        {
            if(pSCLElementBDA->GetAttribute("bType") != "Struct")
                continue;

            QString strType = pSCLElementBDA->GetAttribute("type");
            if(mapConfilictIdToNewID.contains(strType))
                pSCLElementBDA->SetAttribute("type", mapConfilictIdToNewID.value(strType), false);
        }
    }

    //////////////////////////////////////////////////////////////
    // Update DA refering to the DAType which will add prefix
    //////////////////////////////////////////////////////////////
    foreach(SclParser::SCLElement *pSCLElementDOType, pSCLElementDataTypeTemplatesIcd->FindChildByType("DOType"))
    {
        foreach(SclParser::SCLElement *pSCLElementDA, pSCLElementDOType->FindChildByType("DA"))
        {
            if(pSCLElementDA->GetAttribute("bType") != "Struct")
                continue;

            QString strType = pSCLElementDA->GetAttribute("type");
            if(mapConfilictIdToNewID.contains(strType))
                pSCLElementDA->SetAttribute("type", mapConfilictIdToNewID.value(strType), false);
        }
    }

    //////////////////////////////////////////////////////////////
    // Move the DAType from Icd Template to Scd Template
    //////////////////////////////////////////////////////////////
    foreach(SclParser::SCLElement *pSCLElementDAType, lstNewTypeIntoScd)
        pSCLElementDataTypeTemplatesScd->AttachChild(pSCLElementDAType->Clone(true), -1, false);
}

static void HandleDOTypeConflict(SclParser::SCLElement *pSCLElementDataTypeTemplatesIcd, SclParser::SCLElement *pSCLElementDataTypeTemplatesScd)
{
    ///////////////////////////////////////////////////////
    // Check Conflict DOType
    ///////////////////////////////////////////////////////
    QMap<QString, QString> mapConfilictIdToNewID;
    QList<SclParser::SCLElement*> lstNewTypeIntoScd;
    foreach(SclParser::SCLElement *pSCLElementDOTypeIcd, pSCLElementDataTypeTemplatesIcd->FindChildByType("DOType"))
    {
        const QString strID = pSCLElementDOTypeIcd->GetAttribute("id");
        if(strID.isEmpty())
            continue;

        if(SclParser::SCLElement *pSCLElementDOTypeScd = pSCLElementDataTypeTemplatesScd->FindChild("DOType", "id", strID, false))
        {
            if(CompareSCLElement(pSCLElementDOTypeIcd, pSCLElementDOTypeScd))
                continue;

            const QString strNewID = QString("AutoPrefix%1_%2").arg(QDateTime::currentDateTime().toTime_t()).arg(strID);
            pSCLElementDOTypeIcd->SetAttribute("id", strNewID, false);
            mapConfilictIdToNewID.insert(strID, strNewID);
        }

        lstNewTypeIntoScd.append(pSCLElementDOTypeIcd);
    }

    //////////////////////////////////////////////////////////////
    // Update SDO refering to the DOType which will add prefix
    //////////////////////////////////////////////////////////////
    foreach(SclParser::SCLElement *pSCLElementDOType, pSCLElementDataTypeTemplatesIcd->FindChildByType("DOType"))
    {
        foreach(SclParser::SCLElement *pSCLElementSDO, pSCLElementDOType->FindChildByType("SDO"))
        {
            QString strType = pSCLElementSDO->GetAttribute("type");
            if(mapConfilictIdToNewID.contains(strType))
                pSCLElementSDO->SetAttribute("type", mapConfilictIdToNewID.value(strType), false);
        }
    }

    //////////////////////////////////////////////////////////////
    // Update DO refering to the DOType which will add prefix
    //////////////////////////////////////////////////////////////
    foreach(SclParser::SCLElement *pSCLElementLNodeType, pSCLElementDataTypeTemplatesIcd->FindChildByType("LNodeType"))
    {
        foreach(SclParser::SCLElement *pSCLElementDO, pSCLElementLNodeType->FindChildByType("DO"))
        {
            QString strType = pSCLElementDO->GetAttribute("type");
            if(mapConfilictIdToNewID.contains(strType))
                pSCLElementDO->SetAttribute("type", mapConfilictIdToNewID.value(strType), false);
        }
    }

    //////////////////////////////////////////////////////////////
    // Move the DOType from Icd Template to Scd Template
    //////////////////////////////////////////////////////////////
    foreach(SclParser::SCLElement *pSCLElementDOType, lstNewTypeIntoScd)
        pSCLElementDataTypeTemplatesScd->AttachChild(pSCLElementDOType->Clone(true), -1, false);
}


static void HandleLNodeTypeConflict(SclParser::SCLElement *pSCLElementIedIcd, SclParser::SCLElement *pSCLElementDataTypeTemplatesIcd, SclParser::SCLElement *pSCLElementDataTypeTemplatesScd)
{
    ///////////////////////////////////////////////////////
    // Check Conflict LNodeType
    ///////////////////////////////////////////////////////
    QMap<QString, QString> mapConfilictIdToNewID;
    QList<SclParser::SCLElement*> lstNewTypeIntoScd;
    foreach(SclParser::SCLElement *pSCLElementLNodeTypeIcd, pSCLElementDataTypeTemplatesIcd->FindChildByType("LNodeType"))
    {
        const QString strID = pSCLElementLNodeTypeIcd->GetAttribute("id");
        if(strID.isEmpty())
            continue;

        if(SclParser::SCLElement *pSCLElementLNodeTypeScd = pSCLElementDataTypeTemplatesScd->FindChild("LNodeType", "id", strID, false))
        {
            if(CompareSCLElement(pSCLElementLNodeTypeIcd, pSCLElementLNodeTypeScd))
                continue;

            const QString strNewID = QString("AutoPrefix%1_%2").arg(QDateTime::currentDateTime().toTime_t()).arg(strID);
            pSCLElementLNodeTypeIcd->SetAttribute("id", strNewID, false);
            mapConfilictIdToNewID.insert(strID, strNewID);
        }

        lstNewTypeIntoScd.append(pSCLElementLNodeTypeIcd);
    }

    //////////////////////////////////////////////////////////////
    // Update LN/LN0 refering to the LNodeType which will add prefix
    //////////////////////////////////////////////////////////////
    foreach(SclParser::SCLElement *pSCLElementAccessPoint, pSCLElementIedIcd->FindChildByType("AccessPoint"))
    {
        SclParser::SCLElement *pSCLElementServer = pSCLElementAccessPoint->GetFirstChild("Server");
        if(!pSCLElementServer)
            continue;

        foreach(SclParser::SCLElement *PSCLElementLDevice, pSCLElementServer->FindChildByType("LDevice"))
        {
            QList<SclParser::SCLElement*> lstLNs = PSCLElementLDevice->FindChildByType("LN");
            if(SclParser::SCLElement *pSCLElementLN0 = PSCLElementLDevice->GetFirstChild("LN0"))
                lstLNs.prepend(pSCLElementLN0);

            foreach(SclParser::SCLElement *pSCLElementLN, lstLNs)
            {
                QString strLNType = pSCLElementLN->GetAttribute("lnType");
                if(mapConfilictIdToNewID.contains(strLNType))
                    pSCLElementLN->SetAttribute("lnType", mapConfilictIdToNewID.value(strLNType));
            }
        }
    }


    //////////////////////////////////////////////////////////////
    // Move the LNodeType from Icd Template to Scd Template
    //////////////////////////////////////////////////////////////
    foreach(SclParser::SCLElement *pSCLElementLNodeType, lstNewTypeIntoScd)
        pSCLElementDataTypeTemplatesScd->AttachChild(pSCLElementLNodeType->Clone(true), -1, false);
}

namespace Core {

class ConfigCoreOperationPrivate
{
public:
    ConfigCoreOperationPrivate() {}

    static ConfigCoreOperation *m_pInstance;
};

ConfigCoreOperation* ConfigCoreOperationPrivate::m_pInstance = 0;

ConfigCoreOperation::ConfigCoreOperation(QObject *pParent) :
    QObject(pParent), m_d(new ConfigCoreOperationPrivate)
{
    ConfigCoreOperationPrivate::m_pInstance = this;
}

ConfigCoreOperation::~ConfigCoreOperation()
{
}

ConfigCoreOperation* ConfigCoreOperation::Instance()
{
    return ConfigCoreOperationPrivate::m_pInstance;
}

bool ConfigCoreOperation::CloneDevices(const QList<ProjectExplorer::PeDevice*> &lstOriDevices, QList<ProjectExplorer::PeDevice> lstDstDevices)
{
    if(lstOriDevices.isEmpty() || lstOriDevices.size() != lstDstDevices.size())
        return false;

    ///////////////////////////////////////////////////////////////////////
    // Prepare
    ///////////////////////////////////////////////////////////////////////
    ProjectExplorer::PeProjectVersion *pOriProjectVersion = lstOriDevices.first()->GetProjectVersion();
    if(!pOriProjectVersion)
        return false;
    foreach(ProjectExplorer::PeDevice *pDevice, lstOriDevices)
    {
        if(pOriProjectVersion != pDevice->GetProjectVersion())
            return false;
    }

    ProjectExplorer::PeProjectVersion *pProjectVersion = lstDstDevices.first().GetProjectVersion();
    if(!pProjectVersion)
        return false;
    foreach(const ProjectExplorer::PeDevice &device, lstDstDevices)
    {
        if(pProjectVersion != device.GetProjectVersion())
            return false;
    }

    QList<ProjectExplorer::PeBay*> lstNewBays;
    QList<ProjectExplorer::PeCubicle*> lstNewCubicles;
    foreach(const ProjectExplorer::PeDevice &device, lstDstDevices)
    {
        if(ProjectExplorer::PeBay *pBay = device.GetParentBay())
        {
            if(pBay->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId && !lstNewBays.contains(pBay))
                lstNewBays.append(pBay);
        }

        if(ProjectExplorer::PeCubicle *pCubicle = device.GetParentCubicle())
        {
            if(pCubicle->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId && !lstNewCubicles.contains(pCubicle))
                lstNewCubicles.append(pCubicle);
        }
    }

    QList<ProjectExplorer::PeBoard> lstNewBoards;
    QList<ProjectExplorer::PePort> lstNewPorts;
    QList<ProjectExplorer::PeVTerminal> lstNewVTerminals;
    QList<ProjectExplorer::PeStrap> lstNewStraps;
    QMap<ProjectExplorer::PePort*, ProjectExplorer::PePort*> mapOriToNewPort;
    QMap<ProjectExplorer::PeVTerminal*, ProjectExplorer::PeVTerminal*> mapOriToNewVTerminal;
    QMap<ProjectExplorer::PeStrap*, ProjectExplorer::PeStrap*> mapOriToNewStrap;

    for(int i = 0; i < lstOriDevices.size(); i++)
    {
        ProjectExplorer::PeDevice *pOriDevice = lstOriDevices.at(i);
        ProjectExplorer::PeDevice &NewDevice = lstDstDevices[i];

        foreach(ProjectExplorer::PeBoard *pOriBoard, pOriDevice->GetChildBoards())
        {
            ProjectExplorer::PeBoard BoardTemp(*pOriBoard);
            BoardTemp.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            BoardTemp.SetProjectVersion(pProjectVersion);
            BoardTemp.SetParentDevice(&NewDevice);
            lstNewBoards.append(BoardTemp);

            ProjectExplorer::PeBoard &BoardNew = lstNewBoards.last();

            foreach(ProjectExplorer::PePort *pOriPort, pOriBoard->GetChildPorts())
            {
                ProjectExplorer::PePort PortTemp(*pOriPort);
                PortTemp.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                PortTemp.SetProjectVersion(pProjectVersion);
                PortTemp.SetParentBoard(&BoardNew);
                lstNewPorts.append(PortTemp);

                ProjectExplorer::PePort &PortNew = lstNewPorts.last();
                mapOriToNewPort.insert(pOriPort, &PortNew);
            }
        }

        foreach(ProjectExplorer::PeVTerminal *pOriVTerminal, pOriDevice->GetVTerminals())
        {
            ProjectExplorer::PeVTerminal VTerminalTemp(*pOriVTerminal);
            VTerminalTemp.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            VTerminalTemp.SetProjectVersion(pProjectVersion);
            VTerminalTemp.SetParentDevice(&NewDevice);
            lstNewVTerminals.append(VTerminalTemp);

            ProjectExplorer::PeVTerminal &VTerminalNew = lstNewVTerminals.last();
            mapOriToNewVTerminal.insert(pOriVTerminal, &VTerminalNew);
        }

        foreach(ProjectExplorer::PeStrap *pOriStrap, pOriDevice->GetStraps())
        {
            ProjectExplorer::PeStrap StrapTemp(*pOriStrap);
            StrapTemp.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            StrapTemp.SetProjectVersion(pProjectVersion);
            StrapTemp.SetParentDevice(&NewDevice);
            lstNewStraps.append(StrapTemp);

            ProjectExplorer::PeStrap &StrapNew = lstNewStraps.last();
            mapOriToNewStrap.insert(pOriStrap, &StrapNew);
        }
    }

    int iGroup = pProjectVersion->DbGenerateNewInfoSetGroup();
    if(iGroup <= 0)
        return false;

    QMap<int, int> mapGroupOldToNew;
    QStringList lstExtraInfoSetNames;
    QList<ProjectExplorer::PeInfoSet> lstNewInfoSets;
    foreach(ProjectExplorer::PeInfoSet *pInfoSet, pOriProjectVersion->GetAllInfoSets())
    {
        ProjectExplorer::PeDevice *pTxIED = pInfoSet->GetTxIED();
        ProjectExplorer::PeDevice *pRxIED = pInfoSet->GetRxIED();
        int iIndexTxIED = lstOriDevices.indexOf(pTxIED);
        int iIndexRxIED = lstOriDevices.indexOf(pRxIED);
        if(iIndexTxIED == -1 || iIndexRxIED == -1)
            continue;

        ProjectExplorer::PeInfoSet InfoSetTemp(*pInfoSet);
        InfoSetTemp.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        InfoSetTemp.SetProjectVersion(pProjectVersion);

        InfoSetTemp.SetTxIED(&lstDstDevices[iIndexTxIED]);
        InfoSetTemp.SetRxIED(&lstDstDevices[iIndexRxIED]);
        if(ProjectExplorer::PePort *pTxPort = pInfoSet->GetTxPort())
            InfoSetTemp.SetTxPort(mapOriToNewPort.value(pTxPort, 0));
        if(ProjectExplorer::PePort *pRxPort = pInfoSet->GetRxPort())
            InfoSetTemp.SetRxPort(mapOriToNewPort.value(pRxPort, 0));

        ProjectExplorer::PeDevice *pSwitch1 = pInfoSet->GetSwitch1();
        int iIndexSwitch1 = lstOriDevices.indexOf(pSwitch1);
        if(iIndexSwitch1 != -1)
        {
            InfoSetTemp.SetSwitch1(&lstDstDevices[iIndexSwitch1]);
            if(ProjectExplorer::PePort *pSwitch1TxPort = pInfoSet->GetSwitch1TxPort())
                InfoSetTemp.SetSwitch1TxPort(mapOriToNewPort.value(pSwitch1TxPort, 0));
            if(ProjectExplorer::PePort *pSwitch1RxPort = pInfoSet->GetSwitch1RxPort())
                InfoSetTemp.SetSwitch1RxPort(mapOriToNewPort.value(pSwitch1RxPort, 0));
        }
        else
        {
            InfoSetTemp.SetSwitch1(0);
            InfoSetTemp.SetSwitch1TxPort(0);
            InfoSetTemp.SetSwitch1RxPort(0);
        }

        ProjectExplorer::PeDevice *pSwitch2 = pInfoSet->GetSwitch2();
        int iIndexSwitch2 = lstOriDevices.indexOf(pSwitch2);
        if(iIndexSwitch2 != -1)
        {
            InfoSetTemp.SetSwitch2(&lstDstDevices[iIndexSwitch2]);
            if(ProjectExplorer::PePort *pSwitch2TxPort = pInfoSet->GetSwitch2TxPort())
                InfoSetTemp.SetSwitch2TxPort(mapOriToNewPort.value(pSwitch2TxPort, 0));
            if(ProjectExplorer::PePort *pSwitch2RxPort = pInfoSet->GetSwitch2RxPort())
                InfoSetTemp.SetSwitch2RxPort(mapOriToNewPort.value(pSwitch2RxPort, 0));
        }
        else
        {
            InfoSetTemp.SetSwitch2(0);
            InfoSetTemp.SetSwitch2TxPort(0);
            InfoSetTemp.SetSwitch2RxPort(0);
        }

        ProjectExplorer::PeDevice *pSwitch3 = pInfoSet->GetSwitch3();
        int iIndexSwitch3 = lstOriDevices.indexOf(pSwitch3);
        if(iIndexSwitch3 != -1)
        {
            InfoSetTemp.SetSwitch3(&lstDstDevices[iIndexSwitch3]);
            if(ProjectExplorer::PePort *pSwitch3TxPort = pInfoSet->GetSwitch3TxPort())
                InfoSetTemp.SetSwitch3TxPort(mapOriToNewPort.value(pSwitch3TxPort, 0));
            if(ProjectExplorer::PePort *pSwitch3RxPort = pInfoSet->GetSwitch3RxPort())
                InfoSetTemp.SetSwitch3RxPort(mapOriToNewPort.value(pSwitch3RxPort, 0));
        }
        else
        {
            InfoSetTemp.SetSwitch3(0);
            InfoSetTemp.SetSwitch3TxPort(0);
            InfoSetTemp.SetSwitch3RxPort(0);
        }

        ProjectExplorer::PeDevice *pSwitch4 = pInfoSet->GetSwitch4();
        int iIndexSwitch4 = lstOriDevices.indexOf(pSwitch4);
        if(iIndexSwitch4 != -1)
        {
            InfoSetTemp.SetSwitch4(&lstDstDevices[iIndexSwitch4]);
            if(ProjectExplorer::PePort *pSwitch4TxPort = pInfoSet->GetSwitch4TxPort())
                InfoSetTemp.SetSwitch4TxPort(mapOriToNewPort.value(pSwitch4TxPort, 0));
            if(ProjectExplorer::PePort *pSwitch4RxPort = pInfoSet->GetSwitch4RxPort())
                InfoSetTemp.SetSwitch4RxPort(mapOriToNewPort.value(pSwitch4RxPort, 0));
        }
        else
        {
            InfoSetTemp.SetSwitch4(0);
            InfoSetTemp.SetSwitch4TxPort(0);
            InfoSetTemp.SetSwitch4RxPort(0);
        }

        int iOldGroup = InfoSetTemp.GetGroup();
        int iNewGroup = mapGroupOldToNew.value(iOldGroup, 0);
        if(iNewGroup <= 0)
        {
            iNewGroup = ++iGroup;
            mapGroupOldToNew.insert(iOldGroup, iNewGroup);
        }
        InfoSetTemp.SetGroup(iNewGroup);

        if(InfoSetTemp.GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
        {
            InfoSetTemp.UpdateName(lstExtraInfoSetNames);
            lstExtraInfoSetNames.append(InfoSetTemp.GetName());
        }

        lstNewInfoSets.append(InfoSetTemp);
    }

    QList<ProjectExplorer::PeVTerminalConn> lstNewVTerminalConns;
    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, pOriProjectVersion->GetAllVTerminalConns())
    {
        ProjectExplorer::PeVTerminal *pTxVTerminal = pVTerminalConn->GetTxVTerminal();
        ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();

        ProjectExplorer::PeVTerminal *pNewTxVTerminal = mapOriToNewVTerminal.value(pTxVTerminal, 0);
        ProjectExplorer::PeVTerminal *pNewRxVTerminal = mapOriToNewVTerminal.value(pRxVTerminal, 0);
        if(!pNewTxVTerminal || !pNewRxVTerminal)
            continue;

        ProjectExplorer::PeVTerminalConn VTerminalConnTemp(*pVTerminalConn);
        VTerminalConnTemp.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        VTerminalConnTemp.SetProjectVersion(pProjectVersion);

        VTerminalConnTemp.SetTxVTerminal(pNewTxVTerminal);
        VTerminalConnTemp.SetRxVTerminal(pNewRxVTerminal);
        if(ProjectExplorer::PeStrap *pTxStrap = pVTerminalConn->GetTxStrap())
            VTerminalConnTemp.SetTxStrap(mapOriToNewStrap.value(pTxStrap, 0));
        else
            VTerminalConnTemp.SetTxStrap(0);
        if(ProjectExplorer::PeStrap *pRxStrap = pVTerminalConn->GetRxStrap())
            VTerminalConnTemp.SetRxStrap(mapOriToNewStrap.value(pRxStrap, 0));
        else
            VTerminalConnTemp.SetRxStrap(0);

        lstNewVTerminalConns.append(VTerminalConnTemp);
    }

    ///////////////////////////////////////////////////////////////////////
    // Handle Database
    ///////////////////////////////////////////////////////////////////////
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

    // Create new bays
    foreach(ProjectExplorer::PeBay *pBay, lstNewBays)
    {
        if(!pProjectVersion->DbCreateObject(*pBay, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Create new cubicles
    foreach(ProjectExplorer::PeCubicle *pCubicle, lstNewCubicles)
    {
        if(!pProjectVersion->DbCreateObject(*pCubicle, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Create new devices
    for(int i = 0; i < lstDstDevices.size(); i++)
    {
        ProjectExplorer::PeDevice &NewDevice = lstDstDevices[i];
        ProjectExplorer::PeDevice *pOriDevice = lstOriDevices.at(i);

        if(!pProjectVersion->DbCreateObject(NewDevice, false))
        {
            DbTrans.Rollback();
            return false;
        }

        QByteArray baModelContent;
        if(!pOriProjectVersion->DbReadDeviceModel(pOriDevice->GetId(), baModelContent))
        {
            DbTrans.Rollback();
            return false;
        }

        if(!pProjectVersion->DbUpdateDeviceModel(NewDevice.GetId(), baModelContent, false))
        {
            DbTrans.Rollback();
            return false;
        }

        QByteArray baCommunicationContent;
        if(!pProjectVersion->DbReadSclCommunication(baCommunicationContent))
        {
            DbTrans.Rollback();
            return false;
        }

        if(!baCommunicationContent.isNull() && !baCommunicationContent.isEmpty())
        {
            SclParser::SCLElement SCLElementCommunication("Communication", 0, 0);
            SCLElementCommunication.ReadFromBinary(baCommunicationContent);

            bool bNeedUpdateCommunication = false;
            foreach(SclParser::SCLElement *pSCLElementSubNetwork, SCLElementCommunication.FindChildByType("SubNetwork"))
            {
                foreach(SclParser::SCLElement *pSCLElementConnectedAP, pSCLElementSubNetwork->FindChildByType("ConnectedAP"))
                {
                    if(pSCLElementConnectedAP->GetAttribute("iedName") == pOriDevice->GetName())
                    {
                        SclParser::SCLElement *pSCLElementConnectedAPClone = pSCLElementConnectedAP->Clone(true);
                        pSCLElementConnectedAPClone->SetAttribute("iedName", NewDevice.GetName());
                        pSCLElementSubNetwork->AttachChild(pSCLElementConnectedAPClone, -1, false);

                        bNeedUpdateCommunication = true;
                    }
                }
            }

            if(bNeedUpdateCommunication)
            {
                if(!pProjectVersion->DbUpdateSclCommunication(SCLElementCommunication.WriteToBinary(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }
        }
    }

    // Create new boards
    for(int i = 0; i < lstNewBoards.size(); i++)
    {
        ProjectExplorer::PeBoard &BoardNew = lstNewBoards[i];
        if(!pProjectVersion->DbCreateObject(BoardNew, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Create new ports
    for(int i = 0; i < lstNewPorts.size(); i++)
    {
        ProjectExplorer::PePort &PortNew = lstNewPorts[i];
        if(!pProjectVersion->DbCreateObject(PortNew, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Create new vterminals
    for(int i = 0; i < lstNewVTerminals.size(); i++)
    {
        ProjectExplorer::PeVTerminal &VTerminalNew = lstNewVTerminals[i];
        if(!pProjectVersion->DbCreateObject(VTerminalNew, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Create new straps
    for(int i = 0; i < lstNewStraps.size(); i++)
    {
        ProjectExplorer::PeStrap &StrapNew = lstNewStraps[i];
        if(!pProjectVersion->DbCreateObject(StrapNew, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Create new infoset
    for(int i = 0; i < lstNewInfoSets.size(); i++)
    {
        ProjectExplorer::PeInfoSet &InfoSetNew = lstNewInfoSets[i];
        if(!pProjectVersion->DbCreateObject(InfoSetNew, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Create new verminalconn
    for(int i = 0; i < lstNewVTerminalConns.size(); i++)
    {
        ProjectExplorer::PeVTerminalConn &VTerminalConn = lstNewVTerminalConns[i];
        if(!pProjectVersion->DbCreateObject(VTerminalConn, false))
            return false;
    }

    if(!DbTrans.Commit())
        return false;

    ///////////////////////////////////////////////////////////////////////
    // Handle Object
    ///////////////////////////////////////////////////////////////////////

    // Create new bays
    QMap<ProjectExplorer::PeBay*, ProjectExplorer::PeBay*> mapTempToCreateBay;
    foreach(ProjectExplorer::PeBay *pBay, lstNewBays)
    {
        ProjectExplorer::PeBay *pBayCreated = qobject_cast<ProjectExplorer::PeBay*>(pProjectVersion->CreateObject(*pBay));
        if(!pBayCreated)
            return false;

        mapTempToCreateBay.insert(pBay, pBayCreated);
    }

    // Create new cubicles
    QMap<ProjectExplorer::PeCubicle*, ProjectExplorer::PeCubicle*> mapTempToCreateCubicle;
    foreach(ProjectExplorer::PeCubicle *pCubicle, lstNewCubicles)
    {
        ProjectExplorer::PeCubicle *pCubicleCreated = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectVersion->CreateObject(*pCubicle));
        if(!pCubicleCreated)
            return false;

        mapTempToCreateCubicle.insert(pCubicle, pCubicleCreated);
    }

    // Create new devices
    QMap<ProjectExplorer::PeDevice*, ProjectExplorer::PeDevice*> mapTempToCreateDevice;
    for(int i = 0; i < lstDstDevices.size(); i++)
    {
        ProjectExplorer::PeDevice &NewDevice = lstDstDevices[i];
        ProjectExplorer::PeDevice *pOriDevice = lstOriDevices.at(i);

        if(ProjectExplorer::PeBay *pBayCreated = mapTempToCreateBay.value(NewDevice.GetParentBay(), 0))
            NewDevice.SetParentBay(pBayCreated);

        if(ProjectExplorer::PeCubicle *pCubicleCreated = mapTempToCreateCubicle.value(NewDevice.GetParentCubicle(), 0))
            NewDevice.SetParentCubicle(pCubicleCreated);

        ProjectExplorer::PeDevice *pDeviceCreated = qobject_cast<ProjectExplorer::PeDevice*>(pProjectVersion->CreateObject(NewDevice));
        if(!pDeviceCreated)
            return false;
        pProjectVersion->UpdateDeviceModelStatus(pDeviceCreated, pOriDevice->GetHasModel());

        mapTempToCreateDevice.insert(&NewDevice, pDeviceCreated);
    }

    // Create new boards
    QMap<ProjectExplorer::PeBoard*, ProjectExplorer::PeBoard*> mapTempToCreateBoard;
    for(int i = 0; i < lstNewBoards.size(); i++)
    {
        ProjectExplorer::PeBoard &BoardNew = lstNewBoards[i];

        if(ProjectExplorer::PeDevice *pDeviceCreated = mapTempToCreateDevice.value(BoardNew.GetParentDevice(), 0))
            BoardNew.SetParentDevice(pDeviceCreated);

        ProjectExplorer::PeBoard *pBoardCreated = qobject_cast<ProjectExplorer::PeBoard*>(pProjectVersion->CreateObject(BoardNew));
        if(!pBoardCreated)
            return false;

        mapTempToCreateBoard.insert(&BoardNew, pBoardCreated);
    }

    // Create new ports
    QMap<ProjectExplorer::PePort*, ProjectExplorer::PePort*> mapTempToCreatePort;
    for(int i = 0; i < lstNewPorts.size(); i++)
    {
        ProjectExplorer::PePort &PortNew = lstNewPorts[i];

        if(ProjectExplorer::PeBoard *pBoardCreated = mapTempToCreateBoard.value(PortNew.GetParentBoard(), 0))
            PortNew.SetParentBoard(pBoardCreated);

        ProjectExplorer::PePort *pPortCreated = qobject_cast<ProjectExplorer::PePort*>(pProjectVersion->CreateObject(PortNew));
        if(!pPortCreated)
            return false;

        mapTempToCreatePort.insert(&PortNew, pPortCreated);
    }

    // Create new vterminals
    QMap<ProjectExplorer::PeVTerminal*, ProjectExplorer::PeVTerminal*> mapTempToCreateVTerminal;
    for(int i = 0; i < lstNewVTerminals.size(); i++)
    {
        ProjectExplorer::PeVTerminal &VTerminalNew = lstNewVTerminals[i];

        if(ProjectExplorer::PeDevice *pDeviceCreated = mapTempToCreateDevice.value(VTerminalNew.GetParentDevice(), 0))
            VTerminalNew.SetParentDevice(pDeviceCreated);

        ProjectExplorer::PeVTerminal *pVTerminalCreated = qobject_cast<ProjectExplorer::PeVTerminal*>(pProjectVersion->CreateObject(VTerminalNew));
        if(!pVTerminalCreated)
            return false;

        mapTempToCreateVTerminal.insert(&VTerminalNew, pVTerminalCreated);
    }

    // Create new straps
    QMap<ProjectExplorer::PeStrap*, ProjectExplorer::PeStrap*> mapTempToCreateStrap;
    for(int i = 0; i < lstNewStraps.size(); i++)
    {
        ProjectExplorer::PeStrap &StrapNew = lstNewStraps[i];

        if(ProjectExplorer::PeDevice *pDeviceCreated = mapTempToCreateDevice.value(StrapNew.GetParentDevice(), 0))
            StrapNew.SetParentDevice(pDeviceCreated);

        ProjectExplorer::PeStrap *pStrapCreated = qobject_cast<ProjectExplorer::PeStrap*>(pProjectVersion->CreateObject(StrapNew));
        if(!pStrapCreated)
            return false;

        mapTempToCreateStrap.insert(&StrapNew, pStrapCreated);
    }

    // Create new infoset
    for(int i = 0; i < lstNewInfoSets.size(); i++)
    {
        ProjectExplorer::PeInfoSet &InfoSetNew = lstNewInfoSets[i];

        if(ProjectExplorer::PeDevice *pTxIED = mapTempToCreateDevice.value(InfoSetNew.GetTxIED(), 0))
            InfoSetNew.SetTxIED(pTxIED);
        else
            return false;

        if(InfoSetNew.GetTxPort())
        {
            if(ProjectExplorer::PePort *pTxPort = mapTempToCreatePort.value(InfoSetNew.GetTxPort()))
                InfoSetNew.SetTxPort(pTxPort);
            else
                return false;
        }

        if(ProjectExplorer::PeDevice *pRxIED = mapTempToCreateDevice.value(InfoSetNew.GetRxIED(), 0))
            InfoSetNew.SetRxIED(pRxIED);
        else
            return false;

        if(InfoSetNew.GetRxPort())
        {
            if(ProjectExplorer::PePort *pRxPort = mapTempToCreatePort.value(InfoSetNew.GetRxPort()))
                InfoSetNew.SetRxPort(pRxPort);
            else
                return false;
        }

        if(InfoSetNew.GetSwitch1())
        {
            if(ProjectExplorer::PeDevice *pSwitch1 = mapTempToCreateDevice.value(InfoSetNew.GetSwitch1(), 0))
                InfoSetNew.SetSwitch1(pSwitch1);
            else
                return false;
        }
        if(InfoSetNew.GetSwitch1TxPort())
        {
            if(ProjectExplorer::PePort *pSwitch1TxPort = mapTempToCreatePort.value(InfoSetNew.GetSwitch1TxPort()))
                InfoSetNew.SetSwitch1TxPort(pSwitch1TxPort);
            else
                return false;
        }
        if(InfoSetNew.GetSwitch1RxPort())
        {
            if(ProjectExplorer::PePort *pSwitch1RxPort = mapTempToCreatePort.value(InfoSetNew.GetSwitch1RxPort()))
                InfoSetNew.SetSwitch1RxPort(pSwitch1RxPort);
            else
                return false;
        }

        if(InfoSetNew.GetSwitch2())
        {
            if(ProjectExplorer::PeDevice *pSwitch2 = mapTempToCreateDevice.value(InfoSetNew.GetSwitch2(), 0))
                InfoSetNew.SetSwitch2(pSwitch2);
            else
                return false;
        }
        if(InfoSetNew.GetSwitch2TxPort())
        {
            if(ProjectExplorer::PePort *pSwitch2TxPort = mapTempToCreatePort.value(InfoSetNew.GetSwitch2TxPort()))
                InfoSetNew.SetSwitch2TxPort(pSwitch2TxPort);
            else
                return false;
        }
        if(InfoSetNew.GetSwitch2RxPort())
        {
            if(ProjectExplorer::PePort *pSwitch2RxPort = mapTempToCreatePort.value(InfoSetNew.GetSwitch2RxPort()))
                InfoSetNew.SetSwitch2RxPort(pSwitch2RxPort);
            else
                return false;
        }

        if(InfoSetNew.GetSwitch3())
        {
            if(ProjectExplorer::PeDevice *pSwitch3 = mapTempToCreateDevice.value(InfoSetNew.GetSwitch3(), 0))
                InfoSetNew.SetSwitch3(pSwitch3);
            else
                return false;
        }
        if(InfoSetNew.GetSwitch3TxPort())
        {
            if(ProjectExplorer::PePort *pSwitch3TxPort = mapTempToCreatePort.value(InfoSetNew.GetSwitch3TxPort()))
                InfoSetNew.SetSwitch3TxPort(pSwitch3TxPort);
            else
                return false;
        }
        if(InfoSetNew.GetSwitch3RxPort())
        {
            if(ProjectExplorer::PePort *pSwitch3RxPort = mapTempToCreatePort.value(InfoSetNew.GetSwitch3RxPort()))
                InfoSetNew.SetSwitch3RxPort(pSwitch3RxPort);
            else
                return false;
        }

        if(InfoSetNew.GetSwitch4())
        {
            if(ProjectExplorer::PeDevice *pSwitch4 = mapTempToCreateDevice.value(InfoSetNew.GetSwitch4(), 0))
                InfoSetNew.SetSwitch4(pSwitch4);
            else
                return false;
        }
        if(InfoSetNew.GetSwitch4TxPort())
        {
            if(ProjectExplorer::PePort *pSwitch4TxPort = mapTempToCreatePort.value(InfoSetNew.GetSwitch4TxPort()))
                InfoSetNew.SetSwitch4TxPort(pSwitch4TxPort);
            else
                return false;
        }
        if(InfoSetNew.GetSwitch4RxPort())
        {
            if(ProjectExplorer::PePort *pSwitch4RxPort = mapTempToCreatePort.value(InfoSetNew.GetSwitch4RxPort()))
                InfoSetNew.SetSwitch4RxPort(pSwitch4RxPort);
            else
                return false;
        }

        if(!pProjectVersion->CreateObject(InfoSetNew))
            return false;
    }

    // Create new vterminalconn
    for(int i = 0; i < lstNewVTerminalConns.size(); i++)
    {
        ProjectExplorer::PeVTerminalConn &VTerminalConnNew = lstNewVTerminalConns[i];

        if(ProjectExplorer::PeVTerminal *pTxVTerminal = mapTempToCreateVTerminal.value(VTerminalConnNew.GetTxVTerminal()))
            VTerminalConnNew.SetTxVTerminal(pTxVTerminal);
        else
            return false;

        if(ProjectExplorer::PeVTerminal *pRxVTerminal = mapTempToCreateVTerminal.value(VTerminalConnNew.GetRxVTerminal()))
            VTerminalConnNew.SetRxVTerminal(pRxVTerminal);
        else
            return false;

        if(VTerminalConnNew.GetTxStrap())
        {
            if(ProjectExplorer::PeStrap *pTxStrap = mapTempToCreateStrap.value(VTerminalConnNew.GetTxStrap()))
                VTerminalConnNew.SetTxStrap(pTxStrap);
            else
                return false;
        }

        if(VTerminalConnNew.GetRxStrap())
        {
            if(ProjectExplorer::PeStrap *pRxStrap = mapTempToCreateStrap.value(VTerminalConnNew.GetRxStrap()))
                VTerminalConnNew.SetRxStrap(pRxStrap);
            else
                return false;
        }

        if(!pProjectVersion->CreateObject(VTerminalConnNew))
            return false;
    }

    return true;
}

ProjectExplorer::PeDevice* ConfigCoreOperation::CreateDeviceFromLibDevice(ProjectExplorer::PeDevice device, ProjectExplorer::PbLibDevice *pLibDevice)
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = device.GetProjectVersion();
    if(!pProjectVersion)
        return 0;

    QList<ProjectExplorer::PeBoard> lstNewBoards;
    QList<ProjectExplorer::PePort> lstNewPorts;

    if(pLibDevice)
    {
        foreach(ProjectExplorer::PbLibBoard *pLibBoard, pLibDevice->GetChildBoards())
        {
            ProjectExplorer::PeBoard BoardTemp(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            BoardTemp.SetParentDevice(&device);
            BoardTemp.SetPosition(pLibBoard->GetPosition());
            BoardTemp.SetDescription(pLibBoard->GetDescription());
            BoardTemp.SetType(pLibBoard->GetType());
            lstNewBoards.append(BoardTemp);

            ProjectExplorer::PeBoard &BoardNew = lstNewBoards.last();

            foreach(ProjectExplorer::PbLibPort *pLibPort, pLibBoard->GetChildPorts())
            {
                ProjectExplorer::PePort PortTemp(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                PortTemp.SetProjectVersion(pProjectVersion);
                PortTemp.SetParentBoard(&BoardNew);
                PortTemp.SetName(pLibPort->GetName());
                PortTemp.SetGroup(pLibPort->GetGroup());
                PortTemp.SetPortType(ProjectExplorer::PePort::PortType(pLibPort->GetPortType()));
                PortTemp.SetPortDirection(ProjectExplorer::PePort::PortDirection(pLibPort->GetPortDirection()));
                PortTemp.SetFiberPlug(ProjectExplorer::PePort::FiberPlug(pLibPort->GetFiberPlug()));
                PortTemp.SetFiberMode(ProjectExplorer::PePort::FiberMode(pLibPort->GetFiberMode()));
                PortTemp.SetCascade(pLibPort->GetCascade());
                lstNewPorts.append(PortTemp);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////
    // Handle Database
    ///////////////////////////////////////////////////////////////////////
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

    // Create new device
    if(!pProjectVersion->DbCreateObject(device, false))
    {
        DbTrans.Rollback();
        return 0;
    }

    // Create new boards
    for(int i = 0; i < lstNewBoards.size(); i++)
    {
        ProjectExplorer::PeBoard &BoardNew = lstNewBoards[i];
        if(!pProjectVersion->DbCreateObject(BoardNew, false))
        {
            DbTrans.Rollback();
            return 0;
        }
    }

    // Create new ports
    for(int i = 0; i < lstNewPorts.size(); i++)
    {
        ProjectExplorer::PePort &PortNew = lstNewPorts[i];
        if(!pProjectVersion->DbCreateObject(PortNew, false))
        {
            DbTrans.Rollback();
            return 0;
        }
    }

    if(!DbTrans.Commit())
        return 0;

    ///////////////////////////////////////////////////////////////////////
    // Handle Database
    ///////////////////////////////////////////////////////////////////////

    // Create new devices
    ProjectExplorer::PeDevice *pDeviceCreated = qobject_cast<ProjectExplorer::PeDevice*>(pProjectVersion->CreateObject(device));
    if(!pDeviceCreated)
        return 0;

    // Create new boards
    QMap<ProjectExplorer::PeBoard*, ProjectExplorer::PeBoard*> mapTempToCreateBoard;
    for(int i = 0; i < lstNewBoards.size(); i++)
    {
        ProjectExplorer::PeBoard &BoardNew = lstNewBoards[i];
        BoardNew.SetParentDevice(pDeviceCreated);

        ProjectExplorer::PeBoard *pBoardCreated = qobject_cast<ProjectExplorer::PeBoard*>(pProjectVersion->CreateObject(BoardNew));
        if(!pBoardCreated)
            return 0;

        mapTempToCreateBoard.insert(&BoardNew, pBoardCreated);
    }

    // Create new ports
    for(int i = 0; i < lstNewPorts.size(); i++)
    {
        ProjectExplorer::PePort &PortNew = lstNewPorts[i];

        if(ProjectExplorer::PeBoard *pBoardCreated = mapTempToCreateBoard.value(PortNew.GetParentBoard(), 0))
            PortNew.SetParentBoard(pBoardCreated);

        ProjectExplorer::PePort *pPortCreated = qobject_cast<ProjectExplorer::PePort*>(pProjectVersion->CreateObject(PortNew));
        if(!pPortCreated)
            return 0;
    }

    ///////////////////////////////////////////////////////////////////////
    // Handle Model
    ///////////////////////////////////////////////////////////////////////

    if(pLibDevice)
    {
        QByteArray baIcdContent;
        if(ProjectExplorer::BaseManager::Instance()->DbReadLibDeviceModel(pLibDevice->GetId(), baIcdContent))
        {
            if(!baIcdContent.isNull() && !baIcdContent.isEmpty())
            {
                SclParser::SCLDocument document;
                document.ReadBinary(baIcdContent);

                UpdateDeviceModel(&document, pLibDevice->GetIcdFileName(), QList<ProjectExplorer::PeDevice*>() << pDeviceCreated);
            }
        }
    }

    return pDeviceCreated;
}

QList<ProjectExplorer::PeVTerminal> ConfigCoreOperation::PickupVTerminals(SclParser::SCLElement *pSCLElementIED)
{
    QList<ProjectExplorer::PeVTerminal> lstVTerminals;

    if(!pSCLElementIED)
        return lstVTerminals;

    int iNumber = 0;
    foreach(const QStringList lstTerminalData, Core::VTerminalDefine::Instance()->PickupGooseTx(pSCLElementIED))
    {
        ProjectExplorer::PeVTerminal vterminal;
        vterminal.SetNumber(++iNumber);
        vterminal.SetType(ProjectExplorer::PeVTerminal::ttGoose);
        vterminal.SetDirection(ProjectExplorer::PeVTerminal::tdOut);
        vterminal.SetIEDDesc(lstTerminalData.at(7));
        vterminal.SetProDesc(lstTerminalData.at(7));
        vterminal.SetLDInst(lstTerminalData.at(0));
        vterminal.SetLNPrefix(lstTerminalData.at(1));
        vterminal.SetLNClass(lstTerminalData.at(2));
        vterminal.SetLNInst(lstTerminalData.at(3));
        vterminal.SetDOName(lstTerminalData.at(5));
        vterminal.SetDAName(lstTerminalData.at(6));

        lstVTerminals.append(vterminal);
    }

    iNumber = 0;
    foreach(const QStringList lstTerminalData, Core::VTerminalDefine::Instance()->PickupGooseRx(pSCLElementIED))
    {
        ProjectExplorer::PeVTerminal vterminal;
        vterminal.SetNumber(++iNumber);
        vterminal.SetType(ProjectExplorer::PeVTerminal::ttGoose);
        vterminal.SetDirection(ProjectExplorer::PeVTerminal::tdIn);
        vterminal.SetIEDDesc(lstTerminalData.at(7));
        vterminal.SetProDesc(lstTerminalData.at(7));
        vterminal.SetLDInst(lstTerminalData.at(0));
        vterminal.SetLNPrefix(lstTerminalData.at(1));
        vterminal.SetLNClass(lstTerminalData.at(2));
        vterminal.SetLNInst(lstTerminalData.at(3));
        vterminal.SetDOName(lstTerminalData.at(5));
        vterminal.SetDAName(lstTerminalData.at(6));

        lstVTerminals.append(vterminal);
    }

    iNumber = 0;
    foreach(const QStringList lstTerminalData, Core::VTerminalDefine::Instance()->PickupSvTx(pSCLElementIED))
    {
        ProjectExplorer::PeVTerminal vterminal;
        vterminal.SetNumber(++iNumber);
        vterminal.SetType(ProjectExplorer::PeVTerminal::ttSv);
        vterminal.SetDirection(ProjectExplorer::PeVTerminal::tdOut);
        vterminal.SetIEDDesc(lstTerminalData.at(7));
        vterminal.SetProDesc(lstTerminalData.at(7));
        vterminal.SetLDInst(lstTerminalData.at(0));
        vterminal.SetLNPrefix(lstTerminalData.at(1));
        vterminal.SetLNClass(lstTerminalData.at(2));
        vterminal.SetLNInst(lstTerminalData.at(3));
        vterminal.SetDOName(lstTerminalData.at(5));
        vterminal.SetDAName(lstTerminalData.at(6));

        lstVTerminals.append(vterminal);
    }

    iNumber = 0;
    foreach(const QStringList lstTerminalData, Core::VTerminalDefine::Instance()->PickupSvRx(pSCLElementIED))
    {
        ProjectExplorer::PeVTerminal vterminal;
        vterminal.SetNumber(++iNumber);
        vterminal.SetType(ProjectExplorer::PeVTerminal::ttSv);
        vterminal.SetDirection(ProjectExplorer::PeVTerminal::tdIn);
        vterminal.SetIEDDesc(lstTerminalData.at(7));
        vterminal.SetProDesc(lstTerminalData.at(7));
        vterminal.SetLDInst(lstTerminalData.at(0));
        vterminal.SetLNPrefix(lstTerminalData.at(1));
        vterminal.SetLNClass(lstTerminalData.at(2));
        vterminal.SetLNInst(lstTerminalData.at(3));
        vterminal.SetDOName(lstTerminalData.at(5));
        vterminal.SetDAName(lstTerminalData.at(6));

        lstVTerminals.append(vterminal);
    }

    return lstVTerminals;
}

QList<ProjectExplorer::PeStrap> ConfigCoreOperation::PickupStraps(SclParser::SCLElement *pSCLElementIED)
{
    QList<ProjectExplorer::PeStrap> lstStraps;

    if(!pSCLElementIED)
        return lstStraps;

    int iNumber = 0;
    foreach(SclParser::SCLElement *pSCLElementAccessPoint, pSCLElementIED->FindChildByType("AccessPoint"))
    {
        SclParser::SCLElement *pSCLElementServer = pSCLElementAccessPoint->GetFirstChild("Server");
        if(!pSCLElementServer)
            continue;

        foreach(SclParser::SCLElement *pSCLElementLDevice, pSCLElementServer->FindChildByType("LDevice"))
        {
            QList<SclParser::SCLElement*> lstLNs = pSCLElementLDevice->FindChildByType("LN");
            if(SclParser::SCLElement *pSCLElementLN0 = pSCLElementLDevice->GetFirstChild("LN0"))
                lstLNs.prepend(pSCLElementLN0);

            foreach(SclParser::SCLElement *pSCLElementLN, lstLNs)
            {
                foreach(SclParser::SCLElement *pSCLElementDataSet, pSCLElementLN->FindChildByType("DataSet"))
                {
                    if(pSCLElementDataSet->GetAttribute("name") != QLatin1String("dsRelayEna"))
                        continue;

                    foreach(SclParser::SCLElement *pSCLElementFCDA, pSCLElementDataSet->FindChildByType("FCDA"))
                    {
                        const QString strLDInst   = pSCLElementFCDA->GetAttribute("ldInst");
                        const QString strPrefix   = pSCLElementFCDA->GetAttribute("prefix");
                        const QString strLNClass  = pSCLElementFCDA->GetAttribute("lnClass");
                        const QString strLNInst   = pSCLElementFCDA->GetAttribute("lnInst");
                        const QString strDOName   = pSCLElementFCDA->GetAttribute("doName");
                        const QString strDAName   = pSCLElementFCDA->GetAttribute("daName");

                        SclParser::SCLElement *pSCLElementFCDALDevice = SclParser::SCLHelp::GetLDeviceFromIEDAndInst(pSCLElementIED, strLDInst);
                        if(!pSCLElementFCDALDevice)
                            continue;

                        QMap<QString, QString> mapAttValue;
                        mapAttValue["prefix"] = strPrefix;
                        mapAttValue["lnClass"] = strLNClass;
                        mapAttValue["inst"] = strLNInst;
                        SclParser::SCLElement *pSCLElementFCDALN = pSCLElementFCDALDevice->FindChild("LN0", mapAttValue, false);
                        if(!pSCLElementFCDALN)
                            pSCLElementFCDALN = pSCLElementFCDALDevice->FindChild("LN", mapAttValue, false);
                        if(!pSCLElementFCDALN)
                            continue;

                        SclParser::SCLElement *pSCLElementFCDADOI = SclParser::SCLHelp::GetDOIFromLNAndName(pSCLElementFCDALN, strDOName);
                        if(!pSCLElementFCDADOI)
                            continue;

                        ProjectExplorer::PeStrap strap;
                        strap.SetNumber(++iNumber);
                        strap.SetDescription(pSCLElementFCDADOI->GetAttribute("desc"));
                        strap.SetLDInst(strLDInst);
                        strap.SetLNPrefix(strPrefix);
                        strap.SetLNClass(strLNClass);
                        strap.SetLNInst(strLNInst);
                        strap.SetDOName(strDOName);
                        strap.SetDAName(strDAName);

                        lstStraps.append(strap);
                    }
                }
            }
        }
    }

    return lstStraps;
}

bool ConfigCoreOperation::UpdateDeviceModel(SclParser::SCLDocument *pSCLDocumentIcd, QString strIcdFileName, const QList<ProjectExplorer::PeDevice*> &lstDevices, QList<ProjectExplorer::PeDevice*> *plstFailedDevices)
{
    ////////////////////////////////////////////////////////
    /// Validate Parameters
    ////////////////////////////////////////////////////////
    if(lstDevices.isEmpty() || !pSCLDocumentIcd)
    {
        if(plstFailedDevices)
            *plstFailedDevices = lstDevices;

        return false;
    }

    ProjectExplorer::PeProjectVersion *pProjectVersion = lstDevices.first()->GetProjectVersion();
    if(!pProjectVersion)
    {
        if(plstFailedDevices)
            *plstFailedDevices = lstDevices;

        return false;
    }

    for(int i = 1; i < lstDevices.size(); i++)
    {
        if(lstDevices.at(i)->GetProjectVersion() != pProjectVersion)
        {
            if(plstFailedDevices)
                *plstFailedDevices = lstDevices;

            return false;
        }
    }

    SclParser::SCLElement *pSCLElementRootIcd = pSCLDocumentIcd->Root();
    if(!pSCLElementRootIcd)
    {
        if(plstFailedDevices)
            *plstFailedDevices = lstDevices;

        return false;
    }

    SclParser::SCLElement *pSCLElementIedIcd = pSCLElementRootIcd->GetFirstChild("IED");
    if(!pSCLElementIedIcd)
    {
        if(plstFailedDevices)
            *plstFailedDevices = lstDevices;

        return false;
    }

    ////////////////////////////////////////////////////////
    /// Handle DataTypeTemplates Conflic
    ////////////////////////////////////////////////////////
    QByteArray baTemplateContent;
    if(SclParser::SCLElement *pSCLElementDataTypeTemplatesIcd = pSCLElementRootIcd->GetFirstChild("DataTypeTemplates"))
    {
        QByteArray baContent;
        if(!pProjectVersion->DbReadSclTemplate(baContent))
        {
            if(plstFailedDevices)
                *plstFailedDevices = lstDevices;

            return false;
        }

        if(baContent.isNull() || baContent.isEmpty())
        {
            baTemplateContent = pSCLElementDataTypeTemplatesIcd->WriteToBinary();
        }
        else
        {
            SclParser::SCLElement SCLElementDataTypeTemplatesScd("DataTypeTemplates", 0, 0);
            SCLElementDataTypeTemplatesScd.ReadFromBinary(baContent);

            HandleEnumTypeConflict(pSCLElementDataTypeTemplatesIcd, &SCLElementDataTypeTemplatesScd);
            HandleDATypeConflict(pSCLElementDataTypeTemplatesIcd, &SCLElementDataTypeTemplatesScd);
            HandleDOTypeConflict(pSCLElementDataTypeTemplatesIcd, &SCLElementDataTypeTemplatesScd);
            HandleLNodeTypeConflict(pSCLElementIedIcd, pSCLElementDataTypeTemplatesIcd, &SCLElementDataTypeTemplatesScd);

            baTemplateContent = SCLElementDataTypeTemplatesScd.WriteToBinary();
        }
    }

    QByteArray baModelContent = pSCLElementIedIcd->WriteToBinary();
    const QString strConfigVersion = pSCLElementIedIcd->GetAttribute("configVersion");
    if(strIcdFileName.isEmpty())
        strIcdFileName = QFileInfo(pSCLDocumentIcd->GetFileName()).fileName();

    const QList<ProjectExplorer::PeVTerminal> lstVTerminals = PickupVTerminals(pSCLElementIedIcd);
    const QList<ProjectExplorer::PeStrap> lstStraps = PickupStraps(pSCLElementIedIcd);

    bool bResult = true;
    bool bUpdateTemplate = false;
    foreach(ProjectExplorer::PeDevice *pDevice, lstDevices)
    {
        ////////////////////////////////////////////////////////
        /// Prepare Communication Update
        ////////////////////////////////////////////////////////
        QMap<QString, SclParser::SCLElement*> mapApNameToConnectedAPElement;
        if(SclParser::SCLElement *pSCLElementCommunicationIcd = pSCLElementRootIcd->GetFirstChild("Communication"))
        {
            foreach(SclParser::SCLElement *pSCLElementSubNetworkIcd, pSCLElementCommunicationIcd->FindChildByType("SubNetwork"))
            {
                foreach(SclParser::SCLElement *pSCLElementConnectedAPIcd, pSCLElementSubNetworkIcd->FindChildByType("ConnectedAP"))
                    mapApNameToConnectedAPElement.insert(pSCLElementConnectedAPIcd->GetAttribute("apName"), pSCLElementConnectedAPIcd);
            }
        }

        QByteArray baCommunicationContent;
        if(!mapApNameToConnectedAPElement.isEmpty())
        {
            QByteArray baContent;
            if(!pProjectVersion->DbReadSclCommunication(baContent))
            {
                if(plstFailedDevices)
                    plstFailedDevices->append(pDevice);

                continue;
            }

            SclParser::SCLElement SCLElementCommunicationScd("Communication", 0, 0);
            if(!baContent.isNull() && !baContent.isEmpty())
                SCLElementCommunicationScd.ReadFromBinary(baContent);

            foreach(SclParser::SCLElement *pSCLElementSubNetworkScd, SCLElementCommunicationScd.FindChildByType("SubNetwork"))
            {
                foreach(SclParser::SCLElement *pSCLElementConnectedAPScd, pSCLElementSubNetworkScd->FindChildByType("ConnectedAP"))
                {
                    const QString strIedName = pSCLElementConnectedAPScd->GetAttribute("iedName");
                    const QString strApName = pSCLElementConnectedAPScd->GetAttribute("apName");

                    if(strIedName == pDevice->GetName())
                    {
                        pSCLElementSubNetworkScd->RemoveChild(pSCLElementConnectedAPScd, false);

                        if(SclParser::SCLElement *pSCLElementConnectedAPIcd = mapApNameToConnectedAPElement.take(strApName))
                        {
                            if(SclParser::SCLElement *pSCLElementConnectedAPIcdClone = pSCLElementConnectedAPIcd->Clone(true))
                            {
                                pSCLElementConnectedAPIcdClone->SetAttribute("iedName", pDevice->GetName());
                                pSCLElementSubNetworkScd->AttachChild(pSCLElementConnectedAPIcdClone);
                            }
                        }
                    }
                }
            }

            SclParser::SCLElement *pSCLElementSubnetworkStationbusScd = SCLElementCommunicationScd.FindChild("SubNetwork", "name", "Subnetwork_Stationbus", false);
            SclParser::SCLElement *pSCLElementSubnetworkProcessbusScd = SCLElementCommunicationScd.FindChild("SubNetwork", "name", "Subnetwork_Processbus", false);
            foreach(SclParser::SCLElement *pSCLElementConnectedAPIcd, mapApNameToConnectedAPElement.values())
            {
                if(pSCLElementConnectedAPIcd->GetFirstChild("Address"))
                {
                    if(!pSCLElementSubnetworkStationbusScd)
                    {
                        QMap<QString, QString> mapAttValue;
                        mapAttValue.insert("name", "Subnetwork_Stationbus");
                        pSCLElementSubnetworkStationbusScd = SCLElementCommunicationScd.CreateChild("SubNetwork", mapAttValue, -1, false);
                    }

                    if(SclParser::SCLElement *pSCLElementConnectedAPIcdClone = pSCLElementConnectedAPIcd->Clone(true))
                    {
                        pSCLElementConnectedAPIcdClone->SetAttribute("iedName", pDevice->GetName());
                        pSCLElementSubnetworkStationbusScd->AttachChild(pSCLElementConnectedAPIcdClone);
                    }
                }
                else
                {
                    if(!pSCLElementSubnetworkProcessbusScd)
                    {
                        QMap<QString, QString> mapAttValue;
                        mapAttValue.insert("name", "Subnetwork_Processbus");
                        pSCLElementSubnetworkProcessbusScd = SCLElementCommunicationScd.CreateChild("SubNetwork", mapAttValue, -1, false);
                    }

                    if(SclParser::SCLElement *pSCLElementConnectedAPIcdClone = pSCLElementConnectedAPIcd->Clone(true))
                    {
                        pSCLElementConnectedAPIcdClone->SetAttribute("iedName", pDevice->GetName());
                        pSCLElementSubnetworkProcessbusScd->AttachChild(pSCLElementConnectedAPIcdClone);
                    }
                }
            }

            baCommunicationContent = SCLElementCommunicationScd.WriteToBinary();
        }

        ////////////////////////////////////////////////////////
        /// Prepare VTerminals Update
        ////////////////////////////////////////////////////////
        const QList<ProjectExplorer::PeVTerminal*> lstOldVTerminals = pDevice->GetVTerminals();

        // Set the parent device for terminals
        QList<ProjectExplorer::PeVTerminal> lstTempVTerminals = lstVTerminals;
        for(int i = 0; i < lstTempVTerminals.size(); i++)
        {
            ProjectExplorer::PeVTerminal &vterminal = lstTempVTerminals[i];
            vterminal.SetParentDevice(pDevice);
        }

        // Get all terminals need to be created and updated
        QList<ProjectExplorer::PeVTerminal> lstCreateVTerminals, lstUpdateVTerminals;
        foreach(ProjectExplorer::PeVTerminal vterminal, lstTempVTerminals)
        {
            ProjectExplorer::PeVTerminal *pVTerminalFound = 0;
            foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstOldVTerminals)
            {
                if(vterminal.GetRefName() == pVTerminal->GetRefName())
                {
                    pVTerminalFound = pVTerminal;
                    break;
                }
            }

            if(pVTerminalFound)
            {
                vterminal.SetId(pVTerminalFound->GetId());
                lstUpdateVTerminals.append(vterminal);
            }
            else
            {
                vterminal.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                lstCreateVTerminals.append(vterminal);
            }
        }

        // Get all terminals need to be deleted
        QList<ProjectExplorer::PeVTerminal*> lstDeleteVTerminals;
        foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstOldVTerminals)
        {
            bool bHasVTerminal = false;
            foreach(const ProjectExplorer::PeVTerminal &vterminal, lstTempVTerminals)
            {
                if(pVTerminal->GetRefName() == vterminal.GetRefName())
                {
                    bHasVTerminal = true;
                    break;
                }
            }

            if(!bHasVTerminal)
                lstDeleteVTerminals.append(pVTerminal);
        }

        ////////////////////////////////////////////////////////
        /// Prepare Strap Update
        ////////////////////////////////////////////////////////
        const QList<ProjectExplorer::PeStrap*> lstOldStraps = pDevice->GetStraps();

        // Set the parent device for straps
        QList<ProjectExplorer::PeStrap> lstTempStraps = lstStraps;
        for(int i = 0; i < lstTempStraps.size(); i++)
        {
            ProjectExplorer::PeStrap &strap = lstTempStraps[i];
            strap.SetParentDevice(pDevice);
        }

        // Get all straps need to be created and updated
        QList<ProjectExplorer::PeStrap> lstCreateStraps, lstUpdateStraps;
        foreach(ProjectExplorer::PeStrap strap, lstTempStraps)
        {
            ProjectExplorer::PeStrap *pStrapFound = 0;
            foreach(ProjectExplorer::PeStrap *pStrap, lstOldStraps)
            {
                if(strap.GetRefName() == pStrap->GetRefName())
                {
                    pStrapFound = pStrap;
                    break;
                }
            }

            if(pStrapFound)
            {
                strap.SetId(pStrapFound->GetId());
                lstUpdateStraps.append(strap);
            }
            else
            {
                strap.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                lstCreateStraps.append(strap);
            }
        }

        // Get all terminals need to be deleted
        QList<ProjectExplorer::PeStrap*> lstDeleteStraps;
        foreach(ProjectExplorer::PeStrap *pStrap, lstOldStraps)
        {
            bool bHasStrap = false;
            foreach(const ProjectExplorer::PeStrap &vterminal, lstTempStraps)
            {
                if(pStrap->GetRefName() == vterminal.GetRefName())
                {
                    bHasStrap = true;
                    break;
                }
            }

            if(!bHasStrap)
                lstDeleteStraps.append(pStrap);
        }

        ////////////////////////////////////////////////////////
        /// Start Transaction
        ////////////////////////////////////////////////////////
        bool bFailed = false;
        ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

        ////////////////////////////////////////////////////////
        /// Handle Communication Update
        ////////////////////////////////////////////////////////

        if(!baCommunicationContent.isNull() && !baCommunicationContent.isEmpty())
        {
            if(!pProjectVersion->DbUpdateSclCommunication(baCommunicationContent, false))
            {
                DbTrans.Rollback();

                bResult = false;

                if(plstFailedDevices)
                    plstFailedDevices->append(pDevice);

                continue;
            }
        }

        ////////////////////////////////////////////////////////
        /// Handle VTerminals Update
        ////////////////////////////////////////////////////////

        // Create vterminals into database
        for(int i = 0; i < lstCreateVTerminals.size(); i++)
        {
            ProjectExplorer::PeVTerminal &vterminal = lstCreateVTerminals[i];
            if(!pProjectVersion->DbCreateObject(vterminal, false))
            {
                DbTrans.Rollback();

                bFailed = true;
                break;
            }
        }
        if(bFailed)
        {
            bResult = false;

            if(plstFailedDevices)
                plstFailedDevices->append(pDevice);

            continue;
        }

        // Delete vterminals from database
        foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstDeleteVTerminals)
        {
            if(!pProjectVersion->DbDeleteObject(pVTerminal->GetObjectType(), pVTerminal->GetId(), false))
            {
                DbTrans.Rollback();

                bFailed = true;
                break;
            }
        }
        if(bFailed)
        {
            bResult = false;

            if(plstFailedDevices)
                plstFailedDevices->append(pDevice);

            continue;
        }

        // Update vterminals into database
        foreach(const ProjectExplorer::PeVTerminal &vterminal, lstUpdateVTerminals)
        {
            if(!pProjectVersion->DbUpdateObject(vterminal, false))
            {
                DbTrans.Rollback();

                bFailed = true;
                break;
            }
        }
        if(bFailed)
        {
            bResult = false;

            if(plstFailedDevices)
                plstFailedDevices->append(pDevice);

            continue;
        }

        ////////////////////////////////////////////////////////
        /// Handle Straps Update
        ////////////////////////////////////////////////////////

        // Create straps into database
        for(int i = 0; i < lstCreateStraps.size(); i++)
        {
            ProjectExplorer::PeStrap &strap = lstCreateStraps[i];
            if(!pProjectVersion->DbCreateObject(strap, false))
            {
                DbTrans.Rollback();

                bFailed = true;
                break;
            }
        }
        if(bFailed)
        {
            bResult = false;

            if(plstFailedDevices)
                plstFailedDevices->append(pDevice);

            continue;
        }

        // Delete straps from database
        foreach(ProjectExplorer::PeStrap *pStrap, lstDeleteStraps)
        {
            if(!pProjectVersion->DbDeleteObject(pStrap->GetObjectType(), pStrap->GetId(), false))
            {
                DbTrans.Rollback();

                bFailed = true;
                break;
            }
        }
        if(bFailed)
        {
            bResult = false;

            if(plstFailedDevices)
                plstFailedDevices->append(pDevice);

            continue;
        }

        // Update straps into database
        foreach(const ProjectExplorer::PeStrap &strap, lstUpdateStraps)
        {
            if(!pProjectVersion->DbUpdateObject(strap, false))
            {
                DbTrans.Rollback();

                bFailed = true;
                break;
            }
        }
        if(bFailed)
        {
            bResult = false;

            if(plstFailedDevices)
                plstFailedDevices->append(pDevice);

            continue;
        }

        ////////////////////////////////////////////////////////
        /// Handle Device Update
        ////////////////////////////////////////////////////////

        ProjectExplorer::PeDevice device(*pDevice);
        device.SetConfigVersion(strConfigVersion);
        device.SetIcdFileName(strIcdFileName);
        if(!pProjectVersion->DbUpdateObject(device, false))
        {
            DbTrans.Rollback();

            bResult = false;

            if(plstFailedDevices)
                plstFailedDevices->append(pDevice);

            continue;
        }

        if(!pProjectVersion->DbUpdateDeviceModel(pDevice->GetId(), baModelContent, false))
        {
            DbTrans.Rollback();

            bResult = false;

            if(plstFailedDevices)
                plstFailedDevices->append(pDevice);

            continue;
        }

        if(!baTemplateContent.isNull() && !baTemplateContent.isEmpty())
        {
            if(!bUpdateTemplate && !pProjectVersion->DbUpdateSclTemplate(baTemplateContent, false))
            {
                DbTrans.Rollback();

                bResult = false;

                if(plstFailedDevices)
                    plstFailedDevices->append(pDevice);

                continue;
            }
            bUpdateTemplate = true;
        }

        ////////////////////////////////////////////////////////
        /// Commit Transaction
        ////////////////////////////////////////////////////////

        if(!DbTrans.Commit())
        {
            bResult = false;

            if(plstFailedDevices)
                plstFailedDevices->append(pDevice);

            continue;
        }


        ////////////////////////////////////////////////////////
        /// Handle Object Update
        ////////////////////////////////////////////////////////

        foreach(const ProjectExplorer::PeVTerminal &vterminal, lstCreateVTerminals)
            pProjectVersion->CreateObject(vterminal);

        foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstDeleteVTerminals)
            pProjectVersion->DeleteObject(pVTerminal);

        foreach(const ProjectExplorer::PeVTerminal &vterminal, lstUpdateVTerminals)
            pProjectVersion->UpdateObject(vterminal);

        foreach(const ProjectExplorer::PeStrap &strap, lstCreateStraps)
            pProjectVersion->CreateObject(strap);

        foreach(ProjectExplorer::PeStrap *pStrap, lstDeleteStraps)
            pProjectVersion->DeleteObject(pStrap);

        foreach(const ProjectExplorer::PeStrap &strap, lstUpdateStraps)
            pProjectVersion->UpdateObject(strap);

        pProjectVersion->UpdateObject(device, true);
        pProjectVersion->UpdateDeviceModelStatus(pDevice, true);
    }

    return bResult;
}

bool ConfigCoreOperation::ClearDeviceModel(const QList<ProjectExplorer::PeDevice*> &lstDevices, QList<ProjectExplorer::PeDevice*> *plstFailedDevices)
{
    bool bResult = true;
    foreach(ProjectExplorer::PeDevice *pDevice, lstDevices)
    {
        ProjectExplorer::PeProjectVersion *pProjectVersion = pDevice->GetProjectVersion();
        if(!pProjectVersion)
        {
            if(plstFailedDevices)
                plstFailedDevices->append(pDevice);

            bResult = false;
            continue;
        }

        if(!pProjectVersion->DbClearDeviceModel(pDevice->GetId(), true) || !pProjectVersion->ClearDeviceModel(pDevice))
        {
            if(plstFailedDevices)
                plstFailedDevices->append(pDevice);

            bResult = false;
            continue;
        }
    }

    return bResult;
}

bool ConfigCoreOperation::ExportToScd(ProjectExplorer::PeProjectVersion *pProjectVersion, const QString &strScdFileName, bool bExportPortConnection, bool bExportConnection)
{
    if(!pProjectVersion)
        return false;

    QList<ProjectExplorer::PeDevice*> lstDevices;
    foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
    {
        if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
            lstDevices.append(pDevice);
    }
    qSort(lstDevices.begin(), lstDevices.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    SclParser::SCLDocument *pSCLDocument = SclParser::SCLDocControl::Instance()->CreateDocument(strScdFileName, false);
    if(!pSCLDocument)
        return false;

    SclParser::SCLElement *pSCLElementRoot = pSCLDocument->Root();
    if(!pSCLElementRoot)
    {
        SclParser::SCLDocControl::Instance()->CloseDocument(pSCLDocument);
        return false;
    }

    QFutureInterface<void> *pFutureInterface = new QFutureInterface<void>;
    Core::MainWindow::Instance()->AddProgressTask(pFutureInterface->future(), tr("Exporting Scd file"));

    pFutureInterface->setProgressRange(0, lstDevices.size() + 3);
    pFutureInterface->reportStarted();
    pFutureInterface->setProgressValue(0);

    int iSteps = 0;
    foreach(ProjectExplorer::PeDevice *pDevice, lstDevices)
    {
        pFutureInterface->setProgressValue(++iSteps);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        QByteArray baModelContent;
        if(!pProjectVersion->DbReadDeviceModel(pDevice->GetId(), baModelContent))
            continue;

        if(baModelContent.isEmpty() || baModelContent.isNull())
            continue;

        SclParser::SCLElement *pSCLElementIED = pSCLElementRoot->CreateChild("IED", -1, false);
        pSCLElementIED->ReadFromBinary(baModelContent);

        // Write attributes
        pSCLElementIED->SetAttribute("name", pDevice->GetName(), false);
        pSCLElementIED->SetAttribute("desc", pDevice->GetDescription(), false);

        // Write vterminal description
        QMap<QString, SclParser::SCLElement*> mapLDevice, mapLN, mapDOI;
        foreach(ProjectExplorer::PeVTerminal *pVTerminal, pDevice->GetVTerminals())
        {
            const QString strLDevice = pVTerminal->GetLDInst();
            SclParser::SCLElement *pSCLElementLDevice = mapLDevice.value(strLDevice);
            if(!pSCLElementLDevice)
            {
                if(pSCLElementLDevice = pSCLElementIED->FindChild(SclParser::Constants::g_strSclEle_LDevice, QLatin1String("inst"), strLDevice, true))
                    mapLDevice.insert(strLDevice, pSCLElementLDevice);
                else
                    continue;

            }

            const QString strLN = pVTerminal->GetLNPrefix() + pVTerminal->GetLNClass() + pVTerminal->GetLNInst();
            SclParser::SCLElement *pSCLElementLN = mapLN.value(strLDevice + strLN);
            if(!pSCLElementLN)
            {
                if(pSCLElementLN = pSCLElementLDevice->FindChild(strLN))
                    mapLN.insert(strLDevice + strLN, pSCLElementLN);
                else
                    continue;
            }

            const QString strDOI = pVTerminal->GetDOName();
            SclParser::SCLElement *pSCLElementDOI = mapDOI.value(strLDevice + strLN + strDOI);
            if(!pSCLElementDOI)
            {
                if(pSCLElementDOI = pSCLElementLN->FindChild(SclParser::Constants::g_strSclEle_DOI, QLatin1String("name"), strDOI, false))
                    mapDOI.insert(strLDevice + strLN + strDOI, pSCLElementDOI);
                else
                    continue;
            }

            pSCLElementDOI->SetAttribute(QLatin1String("desc"), pVTerminal->GetProDesc());

            SclParser::SCLElement *pSCLElementDU = pSCLElementDOI->FindChild(SclParser::Constants::g_strSclEle_DAI, QLatin1String("name"), QLatin1String("dU"), false);
            if(!pSCLElementDU)
            {
                pSCLElementDU = pSCLElementDOI->CreateChild(SclParser::Constants::g_strSclEle_DAI);
                pSCLElementDU->SetAttribute(QLatin1String("name"), QLatin1String("dU"));
            }

            SclParser::SCLElement *pSCLElementVal = pSCLElementDU->GetFirstChild(SclParser::Constants::g_strSclEle_Val);
            if(!pSCLElementVal)
                pSCLElementVal = pSCLElementDU->CreateChild(SclParser::Constants::g_strSclEle_Val);

            pSCLElementVal->SetValue(pVTerminal->GetProDesc());
        }

        if(bExportConnection)
        {
            // Write inputs
            QList<ProjectExplorer::PeVTerminalConn*> lstVTerminalConns;
            foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, pProjectVersion->GetAllVTerminalConns())
            {
                ProjectExplorer::PeVTerminal *pVTerminalRx = pVTerminalConn->GetRxVTerminal();
                if(pVTerminalRx && pVTerminalRx->GetParentDevice() == pDevice)
                    lstVTerminalConns.append(pVTerminalConn);
            }
            qSort(lstVTerminalConns.begin(), lstVTerminalConns.end(), ProjectExplorer::PeProjectObject::CompareId);

            ///////////////////Rx Port///////////////////////////
            QMap<ProjectExplorer::PeDevice*, QList<ProjectExplorer::PeInfoSet*> > mapTxDeviceToInfoSets;
            foreach(ProjectExplorer::PeInfoSet *pInfoSet, pProjectVersion->GetAllInfoSets())
            {
                if(!pInfoSet->GetTxIED() || !pInfoSet->GetRxIED() || pInfoSet->GetRxIED() != pDevice)
                    continue;

                mapTxDeviceToInfoSets[pInfoSet->GetTxIED()].append(pInfoSet);
            }
            ///////////////////Rx Port///////////////////////////

            foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, lstVTerminalConns)
            {
                ProjectExplorer::PeVTerminal *pVTerminalTx = pVTerminalConn->GetTxVTerminal();
                if(!pVTerminalTx)
                    continue;

                ProjectExplorer::PeDevice *pDeviceTx = pVTerminalTx->GetParentDevice();
                if(!pDeviceTx)
                    continue;

                ProjectExplorer::PeVTerminal *pVTerminalRx = pVTerminalConn->GetRxVTerminal();
                if(!pVTerminalRx)
                    continue;

                ProjectExplorer::PeDevice *pDeviceRx = pVTerminalRx->GetParentDevice();
                if(!pDeviceRx)
                    continue;

                ///////////////////Rx Port///////////////////////////
                ProjectExplorer::PePort *pRxPort = 0;
                QList<ProjectExplorer::PeInfoSet*> lstInfoSets = mapTxDeviceToInfoSets.value(pDeviceTx);
                if(!lstInfoSets.isEmpty())
                {
                    pRxPort = lstInfoSets.first()->GetRxPort();
                    if(lstInfoSets.size() > 1)
                    {
                        // Check if InfoSet is straight
                        QList<ProjectExplorer::PeInfoSet*> lstInfoSetsForStraight;
                        if(pVTerminalConn->GetStraight())
                        {
                            foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstInfoSets)
                            {
                                if(!pInfoSet->GetSwitch1() && !pInfoSet->GetSwitch2() && !pInfoSet->GetSwitch3() && !pInfoSet->GetSwitch4())
                                    lstInfoSetsForStraight.append(pInfoSet);
                            }
                        }
                        else
                        {
                            foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstInfoSets)
                            {
                                if(pInfoSet->GetSwitch1() || pInfoSet->GetSwitch2() || pInfoSet->GetSwitch3() || pInfoSet->GetSwitch4())
                                    lstInfoSetsForStraight.append(pInfoSet);
                            }
                        }

                        if(!lstInfoSetsForStraight.isEmpty())
                        {
                            pRxPort = lstInfoSetsForStraight.first()->GetRxPort();
                            if(lstInfoSetsForStraight.size() > 1)
                            {
                                // Check InfoSet Type
                                QList<ProjectExplorer::PeInfoSet*> lstInfoSetsForType;
                                if(pVTerminalConn->GetRxVTerminal()->GetType() == ProjectExplorer::PeVTerminal::ttGoose)
                                {
                                    foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstInfoSetsForStraight)
                                    {
                                        const ProjectExplorer::PeInfoSet::InfoSetType eInfoSetType = pInfoSet->GetInfoSetType();
                                        if(eInfoSetType == ProjectExplorer::PeInfoSet::itGoose)
                                            lstInfoSetsForType.prepend(pInfoSet);
                                        else if(eInfoSetType == ProjectExplorer::PeInfoSet::itMix)
                                            lstInfoSetsForType.append(pInfoSet);
                                    }
                                }
                                else if(pVTerminalConn->GetRxVTerminal()->GetType() == ProjectExplorer::PeVTerminal::ttSv)
                                {
                                    foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstInfoSetsForStraight)
                                    {
                                        const ProjectExplorer::PeInfoSet::InfoSetType eInfoSetType = pInfoSet->GetInfoSetType();
                                        if(eInfoSetType == ProjectExplorer::PeInfoSet::itSv)
                                            lstInfoSetsForType.prepend(pInfoSet);
                                        else if(eInfoSetType == ProjectExplorer::PeInfoSet::itMix)
                                            lstInfoSetsForType.append(pInfoSet);
                                    }
                                }

                                if(!lstInfoSetsForType.isEmpty())
                                    pRxPort = lstInfoSetsForType.first()->GetRxPort();

                            }
                        }
                    }
                }
                ///////////////////Rx Port///////////////////////////

                SclParser::SCLElement *pSCLElementLDevice = pSCLElementIED->FindChild("LDevice", "inst", pVTerminalRx->GetLDInst(), true);
                if(!pSCLElementLDevice)
                    continue;

                SclParser::SCLElement *pSCLElementLN0 = pSCLElementLDevice->GetFirstChild("LN0");
                if(!pSCLElementLN0)
                    continue;

                SclParser::SCLElement *pSCLElementInputs = pSCLElementLN0->GetFirstChild("Inputs");
                if(!pSCLElementInputs)
                    pSCLElementInputs = pSCLElementLN0->CreateChild("Inputs", -1, false);
                if(!pSCLElementInputs)
                    continue;

                QString strIntAddr = QString("%1/%2%3%4.%5").arg(pVTerminalRx->GetLDInst())
                                                            .arg(pVTerminalRx->GetLNPrefix())
                                                            .arg(pVTerminalRx->GetLNClass())
                                                            .arg(pVTerminalRx->GetLNInst())
                                                            .arg(pVTerminalRx->GetDOName());
                if(!pVTerminalRx->GetDAName().isEmpty())
                    strIntAddr = QString("%1.%2").arg(strIntAddr).arg(pVTerminalRx->GetDAName());

                ///////////////////Rx Port///////////////////////////
                if(pRxPort && bExportPortConnection)
                {
                    strIntAddr = QString("%1-%2:%3").arg(pRxPort->GetParentBoard()->GetPosition())
                                                    .arg(QString('A' + pRxPort->GetGroup() - 1))
                                                    .arg(strIntAddr);
                }
                ///////////////////Rx Port///////////////////////////

                QMap<QString, QString> mapAttValue;
                mapAttValue.insert("iedName", pDeviceTx->GetName());
                mapAttValue.insert("ldInst", pVTerminalTx->GetLDInst());
                mapAttValue.insert("prefix", pVTerminalTx->GetLNPrefix());
                mapAttValue.insert("lnClass", pVTerminalTx->GetLNClass());
                mapAttValue.insert("lnInst", pVTerminalTx->GetLNInst());
                mapAttValue.insert("doName", pVTerminalTx->GetDOName());
                mapAttValue.insert("daName", pVTerminalTx->GetDAName());
                mapAttValue.insert("intAddr", strIntAddr);

                pSCLElementInputs->CreateChild("ExtRef", mapAttValue, -1, false);
            }
        }
    }

    // Write Communication
    pFutureInterface->setProgressValue(++iSteps);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    QByteArray baCommunicationContent;
    if(pProjectVersion->DbReadSclCommunication(baCommunicationContent))
    {
        if(!baCommunicationContent.isEmpty() && !baCommunicationContent.isNull())
        {
            SclParser::SCLElement *pSCLElementCommunication = pSCLElementRoot->CreateChild("Communication", -1 ,false);
            pSCLElementCommunication->ReadFromBinary(baCommunicationContent);
        }
    }

    // Write DataTypeTemplates
    pFutureInterface->setProgressValue(++iSteps);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    QByteArray baTemplateContent;
    if(pProjectVersion->DbReadSclTemplate(baTemplateContent))
    {
        if(!baTemplateContent.isEmpty() && !baTemplateContent.isNull())
        {
            SclParser::SCLElement *pSCLElementDataTypeTemplates = pSCLElementRoot->CreateChild("DataTypeTemplates", -1 ,false);
            pSCLElementDataTypeTemplates->ReadFromBinary(baTemplateContent);
        }
    }

    // Write to file
    pFutureInterface->setProgressValue(++iSteps);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    pSCLDocument->SaveAs(strScdFileName);
    SclParser::SCLDocControl::Instance()->CloseDocument(pSCLDocument);

    pFutureInterface->reportFinished();
    delete pFutureInterface;

    return true;
}

bool ConfigCoreOperation::ImportFromScd(ProjectExplorer::PeProjectVersion *pProjectVersion, const QString &strScdFileName)
{
    if(!pProjectVersion)
        return false;

    SclParser::SCLDocument *pSCLDocument = SclParser::SCLDocControl::Instance()->OpenDocument(strScdFileName, false);
    if(!pSCLDocument)
        return false;

    SclParser::SCLElement *pSCLElementRoot = pSCLDocument->Root();
    if(!pSCLElementRoot)
    {
        SclParser::SCLDocControl::Instance()->CloseDocument(pSCLDocument, false);
        return false;
    }

    ////////////////////////////////////////////////////////
    /// Prepare
    ////////////////////////////////////////////////////////
    QMap<QString, ProjectExplorer::PeDevice*> mapNameToOldDevice;
    foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
    {
        if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
            mapNameToOldDevice.insert(pDevice->GetName(), pDevice);
    }

    QList<ProjectExplorer::PeDevice> lstCreatedDevices;
    QList<ProjectExplorer::PeVTerminal> lstCreatedVTerminals;
    QList<ProjectExplorer::PeStrap> lstCreatedStraps;
    QList<ProjectExplorer::PeVTerminalConn> lstCreatedVTerminalConns;

    QMap<ProjectExplorer::PeDevice*, SclParser::SCLElement*> mapDeviceToIed;
    QMap<QString, ProjectExplorer::PeDevice*> mapNameToDevice;
    QMap<QString, ProjectExplorer::PeVTerminal*> mapRefNameToVTerninal;

    // Prepare device, vterminal, strap
    const QList<SclParser::SCLElement*> lstIEDs = pSCLElementRoot->FindChildByType("IED");
    foreach(SclParser::SCLElement *pSCLElementIED, lstIEDs)
    {
        const QString strIedName = pSCLElementIED->GetAttribute("name");

        ProjectExplorer::PeDevice *pDevice = mapNameToOldDevice.take(strIedName);
        if(!pDevice)
        {
            ProjectExplorer::PeDevice Device;
            Device.SetDeviceType(ProjectExplorer::PeDevice::dtIED);
            Device.SetName(strIedName);
            Device.SetDescription(pSCLElementIED->GetAttribute("desc"));
            Device.SetDescription(pSCLElementIED->GetAttribute("desc"));
            Device.SetManufacture(pSCLElementIED->GetAttribute("manufacturer"));
            Device.SetType(pSCLElementIED->GetAttribute("type"));
            Device.SetConfigVersion(pSCLElementIED->GetAttribute("configVersion"));
            lstCreatedDevices.append(Device);

            ProjectExplorer::PeDevice &DeviceTemp = lstCreatedDevices.last();
            pDevice = &DeviceTemp;
        }
        mapDeviceToIed.insert(pDevice, pSCLElementIED);
        mapNameToDevice.insert(strIedName, pDevice);

        foreach(ProjectExplorer::PeVTerminal VTerminal, Core::ConfigCoreOperation::Instance()->PickupVTerminals(pSCLElementIED))
        {
            VTerminal.SetParentDevice(pDevice);
            lstCreatedVTerminals.append(VTerminal);

            ProjectExplorer::PeVTerminal &VTerminalTemp = lstCreatedVTerminals.last();
            mapRefNameToVTerninal.insert(QString("%1%2").arg(pDevice->GetName()).arg(VTerminalTemp.GetRefName()), &VTerminalTemp);
        }

        foreach(ProjectExplorer::PeStrap strap, Core::ConfigCoreOperation::Instance()->PickupStraps(pSCLElementIED))
        {
            strap.SetParentDevice(pDevice);
            lstCreatedStraps.append(strap);
        }
    }

    // Prepare vterminalconn
    foreach(SclParser::SCLElement *pSCLElementIED, lstIEDs)
    {
        foreach(SclParser::SCLElement *pSCLElementAccessPoint, pSCLElementIED->FindChildByType("AccessPoint"))
        {
            SclParser::SCLElement *pSCLElementServer = pSCLElementAccessPoint->GetFirstChild("Server");
            if(!pSCLElementServer)
                continue;

            foreach(SclParser::SCLElement *pSCLElementLDevice, pSCLElementServer->FindChildByType("LDevice"))
            {
                SclParser::SCLElement *pSCLElementLN0 = pSCLElementLDevice->GetFirstChild("LN0");
                if(!pSCLElementLN0)
                    continue;

                SclParser::SCLElement *pSCLElementInputs = pSCLElementLN0->GetFirstChild("Inputs");
                if(!pSCLElementInputs)
                    continue;

                foreach(SclParser::SCLElement *pSCLElementExtRef ,pSCLElementInputs->FindChildByType("ExtRef"))
                {
                    QString strExtRefName = QString("%1%2/%3%4%5.%6").arg(pSCLElementExtRef->GetAttribute("iedName"))
                                                                     .arg(pSCLElementExtRef->GetAttribute("ldInst"))
                                                                     .arg(pSCLElementExtRef->GetAttribute("prefix"))
                                                                     .arg(pSCLElementExtRef->GetAttribute("lnClass"))
                                                                     .arg(pSCLElementExtRef->GetAttribute("lnInst"))
                                                                     .arg(pSCLElementExtRef->GetAttribute("doName"));
                    const QString strDAName = pSCLElementExtRef->GetAttribute("daName");
                    if(!strDAName.isEmpty())
                        strExtRefName = QString("%1.%2").arg(strExtRefName).arg(strDAName);

                    QString strIntAddr;
                    QStringList lstPortAndIntAddr = pSCLElementExtRef->GetAttribute("intAddr").split(QLatin1Char(':'));
                    if(lstPortAndIntAddr.size() == 1)
                        strIntAddr = lstPortAndIntAddr.first();
                    else if(lstPortAndIntAddr.size() == 2)
                        strIntAddr = lstPortAndIntAddr.at(1);

                    const QString strIntRefName = QString("%1%2").arg(pSCLElementIED->GetAttribute("name"))
                                                                 .arg(strIntAddr);

                    ProjectExplorer::PeVTerminal *pTxVTerminal = mapRefNameToVTerninal.value(strExtRefName, 0);
                    ProjectExplorer::PeVTerminal *pRxVTerminal = mapRefNameToVTerninal.value(strIntRefName, 0);
                    if(pTxVTerminal && pRxVTerminal)
                    {
                        ProjectExplorer::PeVTerminalConn vterminalconn;
                        vterminalconn.SetTxVTerminal(pTxVTerminal);
                        vterminalconn.SetRxVTerminal(pRxVTerminal);
                        lstCreatedVTerminalConns.append(vterminalconn);
                    }
                }

                pSCLElementLN0->RemoveChild(pSCLElementInputs, false);
            }
        }
    }

    // Prepare binary content
    QMap<ProjectExplorer::PeDevice*, QByteArray> mapDeviceToContent;
    foreach(ProjectExplorer::PeDevice *pDevice, mapDeviceToIed.keys())
    {
        SclParser::SCLElement *pSCLElementIED = mapDeviceToIed.value(pDevice);
        QByteArray baContent = pSCLElementIED->WriteToBinary();

        mapDeviceToContent.insert(pDevice, baContent);
    }

    QByteArray arCommunication;
    if(SclParser::SCLElement *pSCLElementCommunication = pSCLElementRoot->GetFirstChild("Communication"))
        arCommunication = pSCLElementCommunication->WriteToBinary();

    QByteArray arTemplate;
    if(SclParser::SCLElement *pSCLElementDataTypeTemplates = pSCLElementRoot->GetFirstChild("DataTypeTemplates"))
        arTemplate = pSCLElementDataTypeTemplates->WriteToBinary();

    // Delete scl document
    SclParser::SCLDocControl::Instance()->CloseDocument(pSCLDocument, false);

    ////////////////////////////////////////////////////////
    /// Handle Database
    ////////////////////////////////////////////////////////
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

    // Clear all device model
    if(!pProjectVersion->DbClearAllModel(false))
    {
        DbTrans.Rollback();
        return false;
    }

    // Handle device update
    foreach(ProjectExplorer::PeDevice *pDevice, mapDeviceToContent.keys())
    {
        if(pDevice->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        {
            if(!pProjectVersion->DbCreateObject(*pDevice, false))
            {
                DbTrans.Rollback();
                return false;
            }
        }

        if(!pProjectVersion->DbUpdateDeviceModel(pDevice->GetId(), mapDeviceToContent.value(pDevice), false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Handle vterminal update
    for(int i = 0; i < lstCreatedVTerminals.size(); i++)
    {
        ProjectExplorer::PeVTerminal &vterminal = lstCreatedVTerminals[i];
        if(!pProjectVersion->DbCreateObject(vterminal, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Handle strap update
    for(int i = 0; i < lstCreatedStraps.size(); i++)
    {
        ProjectExplorer::PeStrap &strap = lstCreatedStraps[i];
        if(!pProjectVersion->DbCreateObject(strap, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Handle vterminalconn update
    for(int i = 0; i < lstCreatedVTerminalConns.size(); i++)
    {
        ProjectExplorer::PeVTerminalConn &vterminalconn = lstCreatedVTerminalConns[i];
        if(!pProjectVersion->DbCreateObject(vterminalconn, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Handle communication update
    if(!arCommunication.isNull() && !arCommunication.isEmpty())
    {
        if(!pProjectVersion->DbUpdateSclCommunication(arCommunication, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Handle template update
    if(!arTemplate.isNull() && !arTemplate.isEmpty())
    {
        if(!pProjectVersion->DbUpdateSclTemplate(arTemplate, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    if(!DbTrans.Commit())
        return false;

    pProjectVersion->Reopen();
    return true;
}

bool ConfigCoreOperation::ExportSpcd(ProjectExplorer::PeProjectVersion *pProjectVersion, const QString &strSpcdFileName)
{
    if(!pProjectVersion)
        return false;

    QFile file(strSpcdFileName);
    if(!file.open(QIODevice::WriteOnly))
        return false;

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(2);
    xmlWriter.setCodec("UTF-8");

    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("SPCL");
    xmlWriter.writeStartElement("Substation");
    xmlWriter.writeAttribute("name", pProjectVersion->GetProject()->GetName());
    xmlWriter.writeAttribute("desc", pProjectVersion->GetProject()->GetDescription());

    QList<ProjectExplorer::PeRoom*> lstAllRooms = pProjectVersion->GetAllRooms();
    qSort(lstAllRooms.begin(), lstAllRooms.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    QList<ProjectExplorer::PeCubicle*> lstAllCubicles = pProjectVersion->GetAllCubicles();
    qSort(lstAllCubicles.begin(), lstAllCubicles.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    QList<ProjectExplorer::PeCable*> lstOpticalCables, lstTailCables;
    foreach(ProjectExplorer::PeCable *pCable, pProjectVersion->GetAllCables())
    {
        if(pCable->GetCableType() == ProjectExplorer::PeCable::ctOptical)
            lstOpticalCables.append(pCable);
        else if(pCable->GetCableType() == ProjectExplorer::PeCable::ctTail)
            lstTailCables.append(pCable);
    }
    qSort(lstOpticalCables.begin(), lstOpticalCables.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    qSort(lstTailCables.begin(), lstTailCables.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    QFutureInterface<void> *pFutureInterface = new QFutureInterface<void>;
    Core::MainWindow::Instance()->AddProgressTask(pFutureInterface->future(), tr("Exporting Spcd file"));

    pFutureInterface->setProgressRange(0, lstAllCubicles.size() + lstOpticalCables.size() + lstTailCables.size() + 1);
    pFutureInterface->reportStarted();
    pFutureInterface->setProgressValue(0);
    int iStpes = 0;

    foreach(ProjectExplorer::PeRoom *pRoom, lstAllRooms)
    {
        xmlWriter.writeStartElement("Region");
        xmlWriter.writeAttribute("name", pRoom->GetNumber());
        xmlWriter.writeAttribute("desc", pRoom->GetName());
        xmlWriter.writeAttribute("area", pRoom->GetYard() ? QLatin1String("true") : QLatin1String("false"));

        QList<ProjectExplorer::PeCubicle*> lstCubicles = pRoom->GetChildCubicles();
        qSort(lstCubicles.begin(), lstCubicles.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
        foreach(ProjectExplorer::PeCubicle *pCubicle, lstCubicles)
        {
            pFutureInterface->setProgressValue(++iStpes);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

            lstAllCubicles.removeAll(pCubicle);

            xmlWriter.writeStartElement("Cubicle");
            xmlWriter.writeAttribute("name", pCubicle->GetNumber());
            xmlWriter.writeAttribute("desc", pCubicle->GetName());

            QList<ProjectExplorer::PeDevice*> lstIEDs, lstSwitches, lstOdfs;
            foreach(ProjectExplorer::PeDevice *pDevice, pCubicle->GetChildDevices())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    lstIEDs.append(pDevice);
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                    lstSwitches.append(pDevice);
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                    lstOdfs.append(pDevice);
            }
            qSort(lstIEDs.begin(), lstIEDs.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
            qSort(lstSwitches.begin(), lstSwitches.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
            qSort(lstOdfs.begin(), lstOdfs.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

            foreach(ProjectExplorer::PeDevice *pDevice, lstIEDs)
            {
                xmlWriter.writeStartElement("Unit");
                xmlWriter.writeAttribute("name", pDevice->GetCubicleNum());
                xmlWriter.writeAttribute("iedName", pDevice->GetName());
                xmlWriter.writeAttribute("manufacturer", pDevice->GetManufacture());
                xmlWriter.writeAttribute("type", pDevice->GetType());
                xmlWriter.writeAttribute("class", "IED");
                xmlWriter.writeAttribute("desc", pDevice->GetDescription());

                QList<ProjectExplorer::PeBoard*> lstBoards = pDevice->GetChildBoards();
                qSort(lstBoards.begin(), lstBoards.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
                foreach(ProjectExplorer::PeBoard *pBoard, lstBoards)
                {
                    xmlWriter.writeStartElement("Board");
                    xmlWriter.writeAttribute("slot", pBoard->GetPosition());
                    xmlWriter.writeAttribute("type", pBoard->GetType());
                    xmlWriter.writeAttribute("desc", pBoard->GetDescription());

                    QList<ProjectExplorer::PePort*> lstPorts = pBoard->GetChildPorts();
                    qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
                    foreach(ProjectExplorer::PePort *pPort, lstPorts)
                    {
                        xmlWriter.writeStartElement("Port");
                        xmlWriter.writeAttribute("no", pPort->GetGroupCh());
                        xmlWriter.writeAttribute("direction", ProjectExplorer::PePort::GetPortDirectionName(pPort->GetPortDirection()));
                        xmlWriter.writeAttribute("plug", ProjectExplorer::PePort::GetFiberPlugName(pPort->GetFiberPlug()));
                        xmlWriter.writeAttribute("desc", pPort->GetName());
                        xmlWriter.writeEndElement();
                    }

                    xmlWriter.writeEndElement();
                }

                xmlWriter.writeEndElement();
            }

            foreach(ProjectExplorer::PeDevice *pDevice, lstSwitches)
            {
                xmlWriter.writeStartElement("Unit");
                xmlWriter.writeAttribute("name", pDevice->GetCubicleNum());
                xmlWriter.writeAttribute("iedName", pDevice->GetName());
                xmlWriter.writeAttribute("manufacturer", pDevice->GetManufacture());
                xmlWriter.writeAttribute("type", pDevice->GetType());
                xmlWriter.writeAttribute("class", "SWITCH");
                xmlWriter.writeAttribute("desc", pDevice->GetDescription());

                QList<ProjectExplorer::PeBoard*> lstBoards = pDevice->GetChildBoards();
                qSort(lstBoards.begin(), lstBoards.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
                foreach(ProjectExplorer::PeBoard *pBoard, lstBoards)
                {
                    xmlWriter.writeStartElement("Board");
                    xmlWriter.writeAttribute("slot", pBoard->GetPosition());
                    xmlWriter.writeAttribute("type", pBoard->GetType());
                    xmlWriter.writeAttribute("desc", pBoard->GetDescription());

                    QList<ProjectExplorer::PePort*> lstPorts = pBoard->GetChildPorts();
                    qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
                    foreach(ProjectExplorer::PePort *pPort, lstPorts)
                    {
                        xmlWriter.writeStartElement("Port");
                        xmlWriter.writeAttribute("no", pPort->GetGroupCh());
                        xmlWriter.writeAttribute("direction", ProjectExplorer::PePort::GetPortDirectionName(pPort->GetPortDirection()));
                        xmlWriter.writeAttribute("plug", ProjectExplorer::PePort::GetFiberPlugName(pPort->GetFiberPlug()));
                        xmlWriter.writeAttribute("desc", "");
                        xmlWriter.writeEndElement();
                    }

                    xmlWriter.writeEndElement();
                }

                xmlWriter.writeEndElement();
            }

            foreach(ProjectExplorer::PeDevice *pDevice, lstOdfs)
            {
                xmlWriter.writeStartElement("Unit");
                xmlWriter.writeAttribute("name", pDevice->GetName());
                xmlWriter.writeAttribute("iedName", "");
                xmlWriter.writeAttribute("manufacturer", pDevice->GetManufacture());
                xmlWriter.writeAttribute("type", pDevice->GetType());
                xmlWriter.writeAttribute("class", "ODF");
                xmlWriter.writeAttribute("desc", pDevice->GetDescription().isEmpty() ? pDevice->GetName() : pDevice->GetDescription());

                QList<ProjectExplorer::PeBoard*> lstBoards = pDevice->GetChildBoards();
                qSort(lstBoards.begin(), lstBoards.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
                foreach(ProjectExplorer::PeBoard *pBoard, lstBoards)
                {
                    xmlWriter.writeStartElement("Board");
                    xmlWriter.writeAttribute("slot", pBoard->GetPosition());
                    xmlWriter.writeAttribute("type", pBoard->GetType());
                    xmlWriter.writeAttribute("desc", pBoard->GetDescription());

                    QList<ProjectExplorer::PePort*> lstPorts = pBoard->GetChildPorts();
                    qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
                    foreach(ProjectExplorer::PePort *pPort, lstPorts)
                    {
                        xmlWriter.writeStartElement("Port");
                        xmlWriter.writeAttribute("no", pPort->GetGroupCh());
                        xmlWriter.writeAttribute("direction", ProjectExplorer::PePort::GetPortDirectionName(pPort->GetPortDirection()));
                        xmlWriter.writeAttribute("plug", ProjectExplorer::PePort::GetFiberPlugName(pPort->GetFiberPlug()));
                        xmlWriter.writeAttribute("desc", "");
                        xmlWriter.writeEndElement();
                    }

                    xmlWriter.writeEndElement();
                }

                xmlWriter.writeEndElement();
            }

            QList<ProjectExplorer::PeCable*> lstCables = pProjectVersion->GetAllCables();
            qSort(lstCables.begin(), lstCables.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
            foreach(ProjectExplorer::PeCable *pCable, lstCables)
            {
                if(pCable->GetCableType() != ProjectExplorer::PeCable::ctJump)
                    continue;

                if(pCable->GetCubicle1() != pCubicle || pCable->GetCubicle2() != pCubicle)
                    continue;

                if(pCable->GetChildFibers().size() != 1)
                    continue;

                QString strPortA;
                if(ProjectExplorer::PePort *pPortA = pCable->GetChildFibers().first()->GetPort1())
                {
                    if(ProjectExplorer::PeBoard *pBoard = pPortA->GetParentBoard())
                    {
                        if(ProjectExplorer::PeDevice *pDevice = pBoard->GetParentDevice())
                        {
                            strPortA = QString("%1.%2.%3-%4").arg(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF ? pDevice->GetName() : pDevice->GetCubicleNum())
                                                             .arg(pBoard->GetPosition())
                                                             .arg(pPortA->GetGroupCh())
                                                             .arg(ProjectExplorer::PePort::GetPortDirectionName(pPortA->GetPortDirection()));
                        }
                    }
                }

                QString strPortB;
                if(ProjectExplorer::PePort *pPortB = pCable->GetChildFibers().first()->GetPort2())
                {
                    if(ProjectExplorer::PeBoard *pBoard = pPortB->GetParentBoard())
                    {
                        if(ProjectExplorer::PeDevice *pDevice = pBoard->GetParentDevice())
                        {
                            strPortB = QString("%1.%2.%3-%4").arg(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF ? pDevice->GetName() : pDevice->GetCubicleNum())
                                                             .arg(pBoard->GetPosition())
                                                             .arg(pPortB->GetGroupCh())
                                                             .arg(ProjectExplorer::PePort::GetPortDirectionName(pPortB->GetPortDirection()));
                        }
                    }
                }

                xmlWriter.writeStartElement("IntCore");
                xmlWriter.writeAttribute("name", pCable->GetDisplayName());
                xmlWriter.writeAttribute("type", "TX");
                xmlWriter.writeAttribute("portA", strPortA);
                xmlWriter.writeAttribute("portB", strPortB);
                xmlWriter.writeEndElement();
            }

            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
    }

    if(!lstAllCubicles.isEmpty())
    {
        xmlWriter.writeStartElement("Region");
        xmlWriter.writeAttribute("name", "yard");
        xmlWriter.writeAttribute("desc", "yard");

        foreach(ProjectExplorer::PeCubicle *pCubicle, lstAllCubicles)
        {
            pFutureInterface->setProgressValue(++iStpes);
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

            xmlWriter.writeStartElement("Cubicle");
            xmlWriter.writeAttribute("name", pCubicle->GetNumber());
            xmlWriter.writeAttribute("desc", pCubicle->GetName());

            QList<ProjectExplorer::PeDevice*> lstIEDs, lstSwitches, lstOdfs;
            foreach(ProjectExplorer::PeDevice *pDevice, pCubicle->GetChildDevices())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    lstIEDs.append(pDevice);
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                    lstSwitches.append(pDevice);
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                    lstOdfs.append(pDevice);
            }
            qSort(lstIEDs.begin(), lstIEDs.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
            qSort(lstSwitches.begin(), lstSwitches.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
            qSort(lstOdfs.begin(), lstOdfs.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

            foreach(ProjectExplorer::PeDevice *pDevice, lstIEDs)
            {
                xmlWriter.writeStartElement("Unit");
                xmlWriter.writeAttribute("name", pDevice->GetCubicleNum());
                xmlWriter.writeAttribute("iedName", pDevice->GetName());
                xmlWriter.writeAttribute("manufacturer", pDevice->GetManufacture());
                xmlWriter.writeAttribute("type", pDevice->GetType());
                xmlWriter.writeAttribute("class", "IED");
                xmlWriter.writeAttribute("desc", pDevice->GetDescription());

                QList<ProjectExplorer::PeBoard*> lstBoards = pDevice->GetChildBoards();
                qSort(lstBoards.begin(), lstBoards.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
                foreach(ProjectExplorer::PeBoard *pBoard, lstBoards)
                {
                    xmlWriter.writeStartElement("Board");
                    xmlWriter.writeAttribute("slot", pBoard->GetPosition());
                    xmlWriter.writeAttribute("type", pBoard->GetType());
                    xmlWriter.writeAttribute("desc", pBoard->GetDescription());

                    QList<ProjectExplorer::PePort*> lstPorts = pBoard->GetChildPorts();
                    qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
                    foreach(ProjectExplorer::PePort *pPort, lstPorts)
                    {
                        xmlWriter.writeStartElement("Port");
                        xmlWriter.writeAttribute("no", pPort->GetGroupCh());
                        xmlWriter.writeAttribute("direction", ProjectExplorer::PePort::GetPortDirectionName(pPort->GetPortDirection()));
                        xmlWriter.writeAttribute("plug", ProjectExplorer::PePort::GetFiberPlugName(pPort->GetFiberPlug()));
                        xmlWriter.writeAttribute("desc", "");
                        xmlWriter.writeEndElement();
                    }

                    xmlWriter.writeEndElement();
                }

                xmlWriter.writeEndElement();
            }

            foreach(ProjectExplorer::PeDevice *pDevice, lstSwitches)
            {
                xmlWriter.writeStartElement("Unit");
                xmlWriter.writeAttribute("name", pDevice->GetCubicleNum());
                xmlWriter.writeAttribute("iedName", pDevice->GetName());
                xmlWriter.writeAttribute("manufacturer", pDevice->GetManufacture());
                xmlWriter.writeAttribute("type", pDevice->GetType());
                xmlWriter.writeAttribute("class", "SWITCH");
                xmlWriter.writeAttribute("desc", pDevice->GetDescription());

                QList<ProjectExplorer::PeBoard*> lstBoards = pDevice->GetChildBoards();
                qSort(lstBoards.begin(), lstBoards.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
                foreach(ProjectExplorer::PeBoard *pBoard, lstBoards)
                {
                    xmlWriter.writeStartElement("Board");
                    xmlWriter.writeAttribute("slot", pBoard->GetPosition());
                    xmlWriter.writeAttribute("type", pBoard->GetType());
                    xmlWriter.writeAttribute("desc", pBoard->GetDescription());

                    QList<ProjectExplorer::PePort*> lstPorts = pBoard->GetChildPorts();
                    qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
                    foreach(ProjectExplorer::PePort *pPort, lstPorts)
                    {
                        xmlWriter.writeStartElement("Port");
                        xmlWriter.writeAttribute("no", pPort->GetGroupCh());
                        xmlWriter.writeAttribute("direction", ProjectExplorer::PePort::GetPortDirectionName(pPort->GetPortDirection()));
                        xmlWriter.writeAttribute("plug", ProjectExplorer::PePort::GetFiberPlugName(pPort->GetFiberPlug()));
                        xmlWriter.writeAttribute("desc", "");
                        xmlWriter.writeEndElement();
                    }

                    xmlWriter.writeEndElement();
                }

                xmlWriter.writeEndElement();
            }

            foreach(ProjectExplorer::PeDevice *pDevice, lstOdfs)
            {
                xmlWriter.writeStartElement("Unit");
                xmlWriter.writeAttribute("name", pDevice->GetName());
                xmlWriter.writeAttribute("iedName", "");
                xmlWriter.writeAttribute("manufacturer", pDevice->GetManufacture());
                xmlWriter.writeAttribute("type", pDevice->GetType());
                xmlWriter.writeAttribute("class", "ODF");
                xmlWriter.writeAttribute("desc", pDevice->GetDescription().isEmpty() ? pDevice->GetName() : pDevice->GetDescription());

                QList<ProjectExplorer::PeBoard*> lstBoards = pDevice->GetChildBoards();
                qSort(lstBoards.begin(), lstBoards.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
                foreach(ProjectExplorer::PeBoard *pBoard, lstBoards)
                {
                    xmlWriter.writeStartElement("Board");
                    xmlWriter.writeAttribute("slot", pBoard->GetPosition());
                    xmlWriter.writeAttribute("type", pBoard->GetType());
                    xmlWriter.writeAttribute("desc", pBoard->GetDescription());

                    QList<ProjectExplorer::PePort*> lstPorts = pBoard->GetChildPorts();
                    qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
                    foreach(ProjectExplorer::PePort *pPort, lstPorts)
                    {
                        xmlWriter.writeStartElement("Port");
                        xmlWriter.writeAttribute("no", pPort->GetGroupCh());
                        xmlWriter.writeAttribute("direction", ProjectExplorer::PePort::GetPortDirectionName(pPort->GetPortDirection()));
                        xmlWriter.writeAttribute("plug", ProjectExplorer::PePort::GetFiberPlugName(pPort->GetFiberPlug()));
                        xmlWriter.writeAttribute("desc", "");
                        xmlWriter.writeEndElement();
                    }

                    xmlWriter.writeEndElement();
                }

                xmlWriter.writeEndElement();
            }

            QList<ProjectExplorer::PeCable*> lstCables = pProjectVersion->GetAllCables();
            qSort(lstCables.begin(), lstCables.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
            foreach(ProjectExplorer::PeCable *pCable, lstCables)
            {
                if(pCable->GetCableType() != ProjectExplorer::PeCable::ctJump)
                    continue;

                if(pCable->GetCubicle1() != pCubicle || pCable->GetCubicle2() != pCubicle)
                    continue;

                if(pCable->GetChildFibers().size() != 1)
                    continue;

                QString strPortA;
                if(ProjectExplorer::PePort *pPortA = pCable->GetChildFibers().first()->GetPort1())
                {
                    if(ProjectExplorer::PeBoard *pBoard = pPortA->GetParentBoard())
                    {
                        if(ProjectExplorer::PeDevice *pDevice = pBoard->GetParentDevice())
                        {
                            strPortA = QString("%1.%2.%3-%4").arg(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF ? pDevice->GetName() : pDevice->GetCubicleNum())
                                                             .arg(pBoard->GetPosition())
                                                             .arg(pPortA->GetGroupCh())
                                                             .arg(ProjectExplorer::PePort::GetPortDirectionName(pPortA->GetPortDirection()));
                        }
                    }
                }

                QString strPortB;
                if(ProjectExplorer::PePort *pPortB = pCable->GetChildFibers().first()->GetPort2())
                {
                    if(ProjectExplorer::PeBoard *pBoard = pPortB->GetParentBoard())
                    {
                        if(ProjectExplorer::PeDevice *pDevice = pBoard->GetParentDevice())
                        {
                            strPortB = QString("%1.%2.%3-%4").arg(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF ? pDevice->GetName() : pDevice->GetCubicleNum())
                                                             .arg(pBoard->GetPosition())
                                                             .arg(pPortB->GetGroupCh())
                                                             .arg(ProjectExplorer::PePort::GetPortDirectionName(pPortB->GetPortDirection()));
                        }
                    }
                }

                xmlWriter.writeStartElement("IntCore");
                xmlWriter.writeAttribute("name", pCable->GetDisplayName());
                xmlWriter.writeAttribute("type", "TX");
                xmlWriter.writeAttribute("portA", strPortA);
                xmlWriter.writeAttribute("portB", strPortB);
                xmlWriter.writeEndElement();
            }

            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
    }

    foreach(ProjectExplorer::PeCable *pCable, lstOpticalCables + lstTailCables)
    {
        pFutureInterface->setProgressValue(++iStpes);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        QString strType;
        if(pCable->GetCableType() == ProjectExplorer::PeCable::ctOptical)
            strType = "GL";
        else if(pCable->GetCableType() == ProjectExplorer::PeCable::ctTail)
            strType = "WL";

        QString strCubicleA;
        if(ProjectExplorer::PeCubicle *pCubicleA = pCable->GetCubicle1())
        {
            if(ProjectExplorer::PeRoom *pRoom = pCubicleA->GetParentRoom())
                strCubicleA = QString("%1.%2").arg(pRoom->GetNumber()).arg(pCubicleA->GetNumber());
            else
                strCubicleA = QString("yard.%1").arg(pCubicleA->GetNumber());
        }

        QString strCubicleB;
        if(ProjectExplorer::PeCubicle *pCubicleB = pCable->GetCubicle2())
        {
            if(ProjectExplorer::PeRoom *pRoom = pCubicleB->GetParentRoom())
                strCubicleB = QString("%1.%2").arg(pRoom->GetNumber()).arg(pCubicleB->GetNumber());
            else
                strCubicleB = QString("yard.%1").arg(pCubicleB->GetNumber());
        }

        xmlWriter.writeStartElement("Cable");
        xmlWriter.writeAttribute("name", pCable->GetDisplayName());
        xmlWriter.writeAttribute("coresNum", QString::number(pCable->GetChildFibers().size()));
        xmlWriter.writeAttribute("type", strType);
        xmlWriter.writeAttribute("cubicleA", strCubicleA);
        xmlWriter.writeAttribute("cubicleB", strCubicleB);

        QList<ProjectExplorer::PeFiber*> lstFibers = pCable->GetChildFibers();
        qSort(lstFibers.begin(), lstFibers.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
        foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
        {
            QString strPortA;
            if(ProjectExplorer::PePort *pPortA = pFiber->GetPort1())
            {
                if(ProjectExplorer::PeBoard *pBoard = pPortA->GetParentBoard())
                {
                    if(ProjectExplorer::PeDevice *pDevice = pBoard->GetParentDevice())
                    {
                        strPortA = QString("%1.%2.%3-%4").arg(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF ? pDevice->GetName() : pDevice->GetCubicleNum())
                                                         .arg(pBoard->GetPosition())
                                                         .arg(pPortA->GetGroupCh())
                                                         .arg(ProjectExplorer::PePort::GetPortDirectionName(pPortA->GetPortDirection()));
                    }
                }
            }

            QString strPortB;
            if(ProjectExplorer::PePort *pPortB = pFiber->GetPort2())
            {
                if(ProjectExplorer::PeBoard *pBoard = pPortB->GetParentBoard())
                {
                    if(ProjectExplorer::PeDevice *pDevice = pBoard->GetParentDevice())
                    {
                        strPortB = QString("%1.%2.%3-%4").arg(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF ? pDevice->GetName() : pDevice->GetCubicleNum())
                                                         .arg(pBoard->GetPosition())
                                                         .arg(pPortB->GetGroupCh())
                                                         .arg(ProjectExplorer::PePort::GetPortDirectionName(pPortB->GetPortDirection()));
                    }
                }
            }

            xmlWriter.writeStartElement("Core");
            xmlWriter.writeAttribute("no", QString::number(pFiber->GetIndex()));
            xmlWriter.writeAttribute("portA", strPortA);
            xmlWriter.writeAttribute("portB", strPortB);
            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
    }

    pFutureInterface->setProgressValue(++iStpes);
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    xmlWriter.writeEndElement();
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();

    file.close();

    pFutureInterface->reportFinished();
    delete pFutureInterface;

    return true;
}

bool ConfigCoreOperation::ExportIpcd(ProjectExplorer::PeDevice *pDevice, const QString &strIpcdFileName)
{
    if(!pDevice)
        return false;

    QFile file(strIpcdFileName);
    if(!file.open(QIODevice::WriteOnly))
        return false;

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(2);
    xmlWriter.setCodec("UTF-8");

    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("SPCL");

    const ProjectExplorer::PeDevice::DeviceType eDeviceType = pDevice->GetDeviceType();
    if(eDeviceType == ProjectExplorer::PeDevice::dtIED)
    {
        xmlWriter.writeStartElement("Unit");
        xmlWriter.writeAttribute("name", pDevice->GetCubicleNum());
        xmlWriter.writeAttribute("iedName", pDevice->GetName());
        xmlWriter.writeAttribute("class", "IED");
        xmlWriter.writeAttribute("type", pDevice->GetType());
        xmlWriter.writeAttribute("manufacturer", pDevice->GetManufacture());
        xmlWriter.writeAttribute("desc", pDevice->GetDescription());

        QList<ProjectExplorer::PeBoard*> lstBoards = pDevice->GetChildBoards();
        qSort(lstBoards.begin(), lstBoards.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
        foreach(ProjectExplorer::PeBoard *pBoard, lstBoards)
        {
            xmlWriter.writeStartElement("Board");
            xmlWriter.writeAttribute("slot", pBoard->GetPosition());
            xmlWriter.writeAttribute("type", pBoard->GetType());
            xmlWriter.writeAttribute("desc", pBoard->GetDescription());

            QList<ProjectExplorer::PePort*> lstPorts = pBoard->GetChildPorts();
            qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
            foreach(ProjectExplorer::PePort *pPort, lstPorts)
            {
                xmlWriter.writeStartElement("Port");
                xmlWriter.writeAttribute("no", QString('A' + pPort->GetGroup() - 1));
                xmlWriter.writeAttribute("direction", ProjectExplorer::PePort::GetPortDirectionName(pPort->GetPortDirection()));
                xmlWriter.writeAttribute("plug", ProjectExplorer::PePort::GetFiberPlugName(pPort->GetFiberPlug()));
                xmlWriter.writeAttribute("desc", pPort->GetName());
                xmlWriter.writeEndElement();
            }

            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
    }
    else if(eDeviceType == ProjectExplorer::PeDevice::dtSwitch)
    {
        xmlWriter.writeStartElement("Unit");
        xmlWriter.writeAttribute("name", pDevice->GetCubicleNum());
        xmlWriter.writeAttribute("iedName", pDevice->GetName());
        xmlWriter.writeAttribute("class", "SWITCH");
        xmlWriter.writeAttribute("type", pDevice->GetType());
        xmlWriter.writeAttribute("manufacturer", pDevice->GetManufacture());
        xmlWriter.writeAttribute("desc", pDevice->GetDescription());

        QList<ProjectExplorer::PeBoard*> lstBoards = pDevice->GetChildBoards();
        qSort(lstBoards.begin(), lstBoards.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
        foreach(ProjectExplorer::PeBoard *pBoard, lstBoards)
        {
            xmlWriter.writeStartElement("Board");
            xmlWriter.writeAttribute("slot", pBoard->GetPosition());
            xmlWriter.writeAttribute("type", pBoard->GetType());
            xmlWriter.writeAttribute("desc", pBoard->GetDescription());

            QList<ProjectExplorer::PePort*> lstPorts = pBoard->GetChildPorts();
            qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
            foreach(ProjectExplorer::PePort *pPort, lstPorts)
            {
                xmlWriter.writeStartElement("Port");
                xmlWriter.writeAttribute("no", QString('A' + pPort->GetGroup() - 1));
                xmlWriter.writeAttribute("direction", ProjectExplorer::PePort::GetPortDirectionName(pPort->GetPortDirection()));
                xmlWriter.writeAttribute("plug", ProjectExplorer::PePort::GetFiberPlugName(pPort->GetFiberPlug()));
                xmlWriter.writeAttribute("desc", pPort->GetName());
                xmlWriter.writeEndElement();
            }

            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
    }
    else if(eDeviceType == ProjectExplorer::PeDevice::dtODF)
    {
        xmlWriter.writeStartElement("Unit");
        xmlWriter.writeAttribute("name", pDevice->GetName());
        xmlWriter.writeAttribute("iedName", "");
        xmlWriter.writeAttribute("class", "ODF");
        xmlWriter.writeAttribute("type", pDevice->GetType());
        xmlWriter.writeAttribute("manufacturer", pDevice->GetManufacture());
        xmlWriter.writeAttribute("desc", pDevice->GetDescription());

        QList<ProjectExplorer::PeBoard*> lstBoards = pDevice->GetChildBoards();
        qSort(lstBoards.begin(), lstBoards.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
        foreach(ProjectExplorer::PeBoard *pBoard, lstBoards)
        {
            xmlWriter.writeStartElement("Board");
            xmlWriter.writeAttribute("slot", pBoard->GetPosition());
            xmlWriter.writeAttribute("type", pBoard->GetType());
            xmlWriter.writeAttribute("desc", pBoard->GetDescription());

            QList<ProjectExplorer::PePort*> lstPorts = pBoard->GetChildPorts();
            qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
            foreach(ProjectExplorer::PePort *pPort, lstPorts)
            {
                xmlWriter.writeStartElement("Port");
                xmlWriter.writeAttribute("no", QString('A' + pPort->GetGroup() - 1));
                xmlWriter.writeAttribute("direction", ProjectExplorer::PePort::GetPortDirectionName(pPort->GetPortDirection()));
                xmlWriter.writeAttribute("plug", ProjectExplorer::PePort::GetFiberPlugName(pPort->GetFiberPlug()));
                xmlWriter.writeAttribute("desc", pPort->GetName());
                xmlWriter.writeEndElement();
            }

            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
    }

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();

    file.close();

    return true;
}

bool ConfigCoreOperation::ExportIpcd(ProjectExplorer::PbLibDevice *pLibDevice, const QString &strIpcdFileName)
{
    if(!pLibDevice)
        return false;

    QFile file(strIpcdFileName);
    if(!file.open(QIODevice::WriteOnly))
        return false;

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(2);
    xmlWriter.setCodec("UTF-8");

    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("SPCL");

    const ProjectExplorer::PbLibDevice::DeviceType eDeviceType = pLibDevice->GetDeviceType();
    if(eDeviceType == ProjectExplorer::PbLibDevice::dtIED)
    {
        xmlWriter.writeStartElement("Unit");
        xmlWriter.writeAttribute("name", QLatin1String("TEMPLATE"));
        xmlWriter.writeAttribute("iedName", "");
        xmlWriter.writeAttribute("class", QLatin1String("IED"));
        xmlWriter.writeAttribute("type", pLibDevice->GetType());
        xmlWriter.writeAttribute("manufacturer", pLibDevice->GetManufacture());
        xmlWriter.writeAttribute("desc", "");

        QList<ProjectExplorer::PbLibBoard*> lstLibBoards = pLibDevice->GetChildBoards();
        qSort(lstLibBoards.begin(), lstLibBoards.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
        foreach(ProjectExplorer::PbLibBoard *pLibBoard, lstLibBoards)
        {
            xmlWriter.writeStartElement("Board");
            xmlWriter.writeAttribute("slot", pLibBoard->GetPosition());
            xmlWriter.writeAttribute("type", pLibBoard->GetType());
            xmlWriter.writeAttribute("desc", pLibBoard->GetDescription());

            QList<ProjectExplorer::PbLibPort*> lstLibPorts = pLibBoard->GetChildPorts();
            qSort(lstLibPorts.begin(), lstLibPorts.end(), ProjectExplorer::PbLibPort::CompareGroup);
            foreach(ProjectExplorer::PbLibPort *pLibPort, lstLibPorts)
            {
                xmlWriter.writeStartElement("Port");
                xmlWriter.writeAttribute("no", QString('A' + pLibPort->GetGroup() - 1));
                xmlWriter.writeAttribute("direction", ProjectExplorer::PbLibPort::GetPortDirectionName(pLibPort->GetPortDirection()));
                xmlWriter.writeAttribute("plug", ProjectExplorer::PbLibPort::GetFiberPlugName(pLibPort->GetFiberPlug()));
                xmlWriter.writeAttribute("desc", pLibPort->GetName());
                xmlWriter.writeEndElement();
            }

            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
    }
    else if(eDeviceType == ProjectExplorer::PbLibDevice::dtSwitch)
    {
        xmlWriter.writeStartElement("Unit");
        xmlWriter.writeAttribute("name", QLatin1String("TEMPLATE"));
        xmlWriter.writeAttribute("iedName", "");
        xmlWriter.writeAttribute("class", QLatin1String("SWITCH"));
        xmlWriter.writeAttribute("type", pLibDevice->GetType());
        xmlWriter.writeAttribute("manufacturer", pLibDevice->GetManufacture());
        xmlWriter.writeAttribute("desc", "");

        QList<ProjectExplorer::PbLibBoard*> lstLibBoards = pLibDevice->GetChildBoards();
        qSort(lstLibBoards.begin(), lstLibBoards.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
        foreach(ProjectExplorer::PbLibBoard *pLibBoard, lstLibBoards)
        {
            xmlWriter.writeStartElement("Board");
            xmlWriter.writeAttribute("slot", pLibBoard->GetPosition());
            xmlWriter.writeAttribute("type", pLibBoard->GetType());
            xmlWriter.writeAttribute("desc", pLibBoard->GetDescription());

            QList<ProjectExplorer::PbLibPort*> lstLibPorts = pLibBoard->GetChildPorts();
            qSort(lstLibPorts.begin(), lstLibPorts.end(), ProjectExplorer::PbLibPort::CompareGroup);
            foreach(ProjectExplorer::PbLibPort *pLibPort, lstLibPorts)
            {
                xmlWriter.writeStartElement("Port");
                xmlWriter.writeAttribute("no", QString('A' + pLibPort->GetGroup() - 1));
                xmlWriter.writeAttribute("direction", ProjectExplorer::PbLibPort::GetPortDirectionName(pLibPort->GetPortDirection()));
                xmlWriter.writeAttribute("plug", ProjectExplorer::PbLibPort::GetFiberPlugName(pLibPort->GetFiberPlug()));
                xmlWriter.writeAttribute("desc", pLibPort->GetName());
                xmlWriter.writeEndElement();
            }

            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
    }

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();

    file.close();

    return true;
}

bool ConfigCoreOperation::ImportIpcd(const QList<ProjectExplorer::PeDevice*> &lstDevices, const QString &strIpcdFileName)
{
    if(lstDevices.isEmpty())
        return false;

    QFile file(strIpcdFileName);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QList<ProjectExplorer::PeBoard> lstBoards;
    QList<ProjectExplorer::PePort> lstPorts;

    QXmlStreamReader xmlReader(&file);
    if(xmlReader.readNextStartElement())
    {
        if(xmlReader.name() == "SPCL")
        {
            if(xmlReader.readNextStartElement())
            {
                if(xmlReader.name() == "Unit")
                {
                    while(xmlReader.readNextStartElement())
                    {
                        if(xmlReader.name() == "Board")
                        {
                            lstBoards.append(ProjectExplorer::PeBoard());
                            ProjectExplorer::PeBoard &board = lstBoards.last();
                            board.SetPosition(xmlReader.attributes().value("slot").toString());
                            board.SetType(xmlReader.attributes().value("type").toString());
                            board.SetDescription(xmlReader.attributes().value("desc").toString());

                            while(xmlReader.readNextStartElement())
                            {
                                if(xmlReader.name() == "Port")
                                {
                                    int iGroup = xmlReader.attributes().value("no").toString().at(0).toLatin1() - 'A' + 1;

                                    ProjectExplorer::PePort::PortDirection ePortDirection = ProjectExplorer::PePort::pdRT;
                                    const QString strPortDirectionName = xmlReader.attributes().value("direction").toString();
                                    if(strPortDirectionName == "Rx")
                                        ePortDirection = ProjectExplorer::PePort::pdRx;
                                    else if(strPortDirectionName == "Tx")
                                        ePortDirection = ProjectExplorer::PePort::pdTx;
                                    else
                                        ePortDirection = ProjectExplorer::PePort::pdRT;

                                    ProjectExplorer::PePort::FiberPlug eFiberPlug = ProjectExplorer::PePort::fpNone;
                                    const QString strFiberPlugName = xmlReader.attributes().value("plug").toString();
                                    if(strFiberPlugName == "ST")
                                        eFiberPlug = ProjectExplorer::PePort::fpST;
                                    else if(strFiberPlugName == "LC")
                                        eFiberPlug = ProjectExplorer::PePort::fpLC;
                                    else if(strFiberPlugName == "SC")
                                        eFiberPlug = ProjectExplorer::PePort::fpSC;
                                    else if(strFiberPlugName == "FC")
                                        eFiberPlug = ProjectExplorer::PePort::fpFC;

                                    lstPorts.append(ProjectExplorer::PePort());
                                    ProjectExplorer::PePort &port = lstPorts.last();
                                    board.AddChildPort(&port);
                                    port.SetParentBoard(&board);

                                    port.SetName(xmlReader.attributes().value("desc").toString());
                                    port.SetGroup(iGroup);
                                    port.SetPortType(ProjectExplorer::PePort::ptFiber);
                                    port.SetPortDirection(ePortDirection);
                                    port.SetFiberPlug(eFiberPlug);
                                    port.SetFiberMode(ProjectExplorer::PePort::fmMultiple);
                                    port.SetCascade(false);
                                }

                                xmlReader.skipCurrentElement();
                            }
                        }
                        else
                        {
                            xmlReader.skipCurrentElement();
                        }
                    }
                }
                else
                {
                    xmlReader.skipCurrentElement();
                }
            }
        }
    }

    foreach(ProjectExplorer::PeDevice *pDevice, lstDevices)
    {
        ProjectExplorer::PeProjectVersion *pProjectVersion = pDevice->GetProjectVersion();
        if(!pProjectVersion)
            continue;

        bool bFailed = false;
        ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

        foreach(ProjectExplorer::PeBoard *pBoard, pDevice->GetChildBoards())
        {
            if(!pProjectVersion->DbDeleteObject(pBoard->GetObjectType(), pBoard->GetId(), false))
            {
                DbTrans.Rollback();

                bFailed = true;
                break;
            }
        }
        if(bFailed)
            continue;

        for(int i = 0; i < lstBoards.size(); i++)
        {
            ProjectExplorer::PeBoard &board = lstBoards[i];
            board.SetProjectVersion(pProjectVersion);
            board.SetParentDevice(pDevice);

            if(!pProjectVersion->DbCreateObject(board, false))
            {
                DbTrans.Rollback();

                bFailed = true;
                break;
            }
        }
        if(bFailed)
            continue;

        for(int i = 0; i < lstPorts.size(); i++)
        {
            ProjectExplorer::PePort &port = lstPorts[i];
            port.SetProjectVersion(pProjectVersion);

            if(!pProjectVersion->DbCreateObject(port, false))
            {
                DbTrans.Rollback();

                bFailed = true;
                break;
            }
        }
        if(bFailed)
            continue;

        if(!DbTrans.Commit())
            continue;

        foreach(ProjectExplorer::PeBoard *pBoard, pDevice->GetChildBoards())
            pProjectVersion->DeleteObject(pBoard);

        foreach(ProjectExplorer::PeBoard board, lstBoards)
        {
            if(ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pProjectVersion->CreateObject(board)))
            {
                foreach(ProjectExplorer::PePort *pPortTemp, board.GetChildPorts())
                {
                    pPortTemp->SetParentBoard(pBoard);
                    pProjectVersion->CreateObject(*pPortTemp);
                }
            }
        }
    }

    return true;
}

} // namespace Config
