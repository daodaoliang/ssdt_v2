#include <QSettings>

#include "sclparser/sclelement.h"
#include "sclparser/sclhelp.h"
#include "mainwindow.h"

#include "vterminaldefine.h"

static const char * const g_szSettings_GroupVirtualTerminal = "VirtualTerminal";
static const char * const g_szSettings_GooseTx              = "GooseTx";
static const char * const g_szSettings_GooseRx              = "GooseRx";
static const char * const g_szSettings_SvTx                 = "SvTx";
static const char * const g_szSettings_SvRx                 = "SvRx";

namespace Core {

class VTerminalDefinePrivate
{
    // Construction and Destruction
public:
    VTerminalDefinePrivate()
    {
    }

    // Properties
public:
    static VTerminalDefine *m_pInstance;

    QMap<QString, QString> m_mapGooseTx;
    QMap<QString, QString> m_mapGooseRx;
    QMap<QString, QString> m_mapSvTx;
    QMap<QString, QString> m_mapSvRx;
};

VTerminalDefine* VTerminalDefinePrivate::m_pInstance = 0;

} // namespace SclSelector

using namespace Core;

VTerminalDefine::VTerminalDefine(QObject *pParent) : QObject(pParent), m_d(new VTerminalDefinePrivate)
{
    VTerminalDefinePrivate::m_pInstance = this;

    ReadSettings();
}

VTerminalDefine::~VTerminalDefine()
{
    SaveSettings();

    delete m_d;
}

VTerminalDefine* VTerminalDefine::Instance()
{
    return VTerminalDefinePrivate::m_pInstance;
}

QMap<QString, QString> VTerminalDefine::GetGooseTx() const
{
    return m_d->m_mapGooseTx;
}

void VTerminalDefine::SetGooseTx(const QMap<QString, QString> &mapGooseTx)
{
    m_d->m_mapGooseTx = mapGooseTx;
}

QMap<QString, QString> VTerminalDefine::GetGooseRx() const
{
    return m_d->m_mapGooseRx;
}

void VTerminalDefine::SetGooseRx(const QMap<QString, QString> &mapGooseRx)
{
    m_d->m_mapGooseRx = mapGooseRx;
}

QMap<QString, QString> VTerminalDefine::GetSvTx() const
{
    return m_d->m_mapSvTx;
}

void VTerminalDefine::SetSvTx(const QMap<QString, QString> &mapSvTx)
{
    m_d->m_mapSvTx = mapSvTx;
}

QMap<QString, QString> VTerminalDefine::GetSvRx() const
{
    return m_d->m_mapSvRx;
}

void VTerminalDefine::SetSvRx(const QMap<QString, QString> &mapSvRx)
{
    m_d->m_mapSvRx = mapSvRx;
}

QMap<QString, QString> VTerminalDefine::GetDefaultGooseTx() const
{
    static QMap<QString, QString> mapDefaultGooseTx;
    if(mapDefaultGooseTx.isEmpty())
    {
        mapDefaultGooseTx.insert("DataSet", "\\bdsGOOSE");
    }

    return mapDefaultGooseTx;
}

QMap<QString, QString> VTerminalDefine::GetDefaultGooseRx() const
{
    static QMap<QString, QString> mapDefaultGooseRx;
    if(mapDefaultGooseRx.isEmpty())
    {
        mapDefaultGooseRx.insert("LNode", "GOIN");
        mapDefaultGooseRx.insert("FC", "\\bST\\b|\\bMX\\b");
        mapDefaultGooseRx.insert("DO", "\\b((?!Mod|Beh|Health)\\w)+\\b");
        mapDefaultGooseRx.insert("DA", "\\bt\\b|\\bstVal\\b|\\bgeneral\\b|\\bmag\\b|\\bf\\b");
    }

    return mapDefaultGooseRx;
}

QMap<QString, QString> VTerminalDefine::GetDefaultSvTx() const
{
    static QMap<QString, QString> mapDefaultSvTx;
    if(mapDefaultSvTx.isEmpty())
    {
        mapDefaultSvTx.insert("DataSet", "\\bdsSV");
    }

    return mapDefaultSvTx;
}

QMap<QString, QString> VTerminalDefine::GetDefaultSvRx() const
{
    static QMap<QString, QString> mapDefaultSvRx;
    if(mapDefaultSvRx.isEmpty())
    {
        mapDefaultSvRx.insert("LNode", "SVIN");
        mapDefaultSvRx.insert("FC", "\\bMX\\b");
        mapDefaultSvRx.insert("DO", "\\b((?!Mod|Beh|Health)\\w)+\\b");
        mapDefaultSvRx.insert("DA", "\\bmag\\b|\\bf\\b|\\binstMag\\b|\\bi\\b|\\bq\\b");
    }

    return mapDefaultSvRx;
}

QList<QStringList> VTerminalDefine::PickupGooseTx(SclParser::SCLElement *pSCLElement)
{
    return PickupTx(pSCLElement, m_d->m_mapGooseTx);
}

QList<QStringList> VTerminalDefine::PickupGooseRx(SclParser::SCLElement *pSCLElement)
{
    return PickupRx(pSCLElement, m_d->m_mapGooseRx);
}

QList<QStringList> VTerminalDefine::PickupSvTx(SclParser::SCLElement *pSCLElement)
{
    return PickupTx(pSCLElement, m_d->m_mapSvTx);
}

QList<QStringList> VTerminalDefine::PickupSvRx(SclParser::SCLElement *pSCLElement)
{
    return PickupRx(pSCLElement, m_d->m_mapSvRx);
}

QList<QStringList> VTerminalDefine::PickupTx(SclParser::SCLElement *pSCLElement, const QMap<QString, QString> &mapTx)
{
    QList<QStringList> lstTxData;

    const QString strTag = pSCLElement->GetTag();
    if(strTag == "IED")
    {
        foreach(SclParser::SCLElement *pSCLElementAccessPoint, pSCLElement->FindChildByType("AccessPoint"))
            lstTxData.append(PickupTx(pSCLElementAccessPoint, mapTx));
    }
    else if(strTag == "AccessPoint")
    {
        const QString strAccessPointName = pSCLElement->GetAttribute("name");
        if(!strAccessPointName.contains(QRegExp(mapTx.value("AccessPoint"))))
            return lstTxData;

        SclParser::SCLElement* pSCLElementServer = pSCLElement->GetFirstChild("Server");
        if(!pSCLElementServer)
            return lstTxData;

        foreach(SclParser::SCLElement *pSCLElementLDevice, pSCLElementServer->FindChildByType("LDevice"))
            lstTxData.append(PickupTx(pSCLElementLDevice, mapTx));
    }
    else if(strTag == "LDevice")
    {
        const QString strLDeviceInst = pSCLElement->GetAttribute("inst");
        if(!strLDeviceInst.contains(QRegExp(mapTx.value("LDevice"))))
            return lstTxData;

        QList<SclParser::SCLElement*> lstLN = pSCLElement->FindChildByType("LN");
        SclParser::SCLElement *pSCLElementLN0 = pSCLElement->GetFirstChild("LN0");
        if(pSCLElementLN0)
            lstLN.prepend(pSCLElementLN0);

        foreach(SclParser::SCLElement *pSCLElementLN, lstLN)
            lstTxData.append(PickupTx(pSCLElementLN, mapTx));
    }
    else if(strTag == "LN" || strTag == "LN0")
    {
        QString strLNPrefix = pSCLElement->GetAttribute("prefix");
        QString strLNClass = pSCLElement->GetAttribute("lnClass");
        QString strLNInst = pSCLElement->GetAttribute("inst");
        QString strLNInfo = strLNPrefix + strLNClass + strLNInst;
        if(!strLNInfo.contains(QRegExp(mapTx.value("LNode"))))
            return lstTxData;

        foreach(SclParser::SCLElement *pSCLElementDataSet, pSCLElement->FindChildByType("DataSet"))
            lstTxData.append(PickupTx(pSCLElementDataSet, mapTx));
    }
    else if(strTag == "DataSet")
    {
        const QString strDataSetName = pSCLElement->GetAttribute("name");
        if(!strDataSetName.contains(QRegExp(mapTx.value("DataSet"))))
            return lstTxData;

        SclParser::SCLElement *pSCLElementIED = pSCLElement->GetParent("IED");
        if(!pSCLElementIED)
            return lstTxData;

        foreach(SclParser::SCLElement *pSCLElementFCDA, pSCLElement->FindChildByType("FCDA"))
        {
            const QString strLDInst   = pSCLElementFCDA->GetAttribute("ldInst");
            const QString strPrefix   = pSCLElementFCDA->GetAttribute("prefix");
            const QString strLNClass  = pSCLElementFCDA->GetAttribute("lnClass");
            const QString strLNInst   = pSCLElementFCDA->GetAttribute("lnInst");
            const QString strLNInfo = strPrefix + strLNClass + strLNInst;
            const QString strFC       = pSCLElementFCDA->GetAttribute("fc");
            const QString strDOName   = pSCLElementFCDA->GetAttribute("doName");
            const QString strDAName   = pSCLElementFCDA->GetAttribute("daName");

            QString strFCDAName = QString("%1/%2$%3$%4").arg(strLDInst).arg(strLNInfo).arg(strFC).arg(strDOName);
            if(!strDAName.isEmpty())
                strFCDAName = QString("%1$%2").arg(strFCDAName).arg(strDAName);

            if(!strFCDAName.contains(QRegExp(mapTx.value("FCDA"))))
                continue;

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

            lstTxData.append(QStringList() << strLDInst << strPrefix << strLNClass << strLNInst << strFC << strDOName << strDAName << pSCLElementFCDADOI->GetAttribute("desc"));
        }
    }

    return lstTxData;
}

QList<QStringList> VTerminalDefine::PickupRx(SclParser::SCLElement *pSCLElement, const QMap<QString, QString> &mapRx)
{
    QList<QStringList> lstRxData;

    const QString strTag = pSCLElement->GetTag();
    if(strTag == "IED")
    {
        foreach(SclParser::SCLElement *pSCLElementAccessPoint, pSCLElement->FindChildByType("AccessPoint"))
            lstRxData.append(PickupRx(pSCLElementAccessPoint, mapRx));
    }
    else if(strTag == "AccessPoint")
    {
        const QString strAccessPointName = pSCLElement->GetAttribute("name");
        if(!strAccessPointName.contains(QRegExp(mapRx.value("AccessPoint"))))
            return lstRxData;

        SclParser::SCLElement* pSCLElementServer = pSCLElement->GetFirstChild("Server");
        if(!pSCLElementServer)
            return lstRxData;

        foreach(SclParser::SCLElement *pSCLElementLDevice, pSCLElementServer->FindChildByType("LDevice"))
            lstRxData.append(PickupRx(pSCLElementLDevice, mapRx));
    }
    else if(strTag == "LDevice")
    {
        QList<SclParser::SCLElement*> lstLN = pSCLElement->FindChildByType("LN");
        SclParser::SCLElement *pSCLElementLN0 = pSCLElement->GetFirstChild("LN0");
        if(pSCLElementLN0)
            lstLN.prepend(pSCLElementLN0);

        foreach(SclParser::SCLElement *pSCLElementLN, lstLN)
            lstRxData.append(PickupRx(pSCLElementLN, mapRx));
    }
    else if(strTag == "LN" || strTag == "LN0")
    {
        SclParser::SCLElement *pSCLElementLDevice = pSCLElement->GetParent("LDevice");
        if(!pSCLElementLDevice)
            return lstRxData;

        const QString strLDInst = pSCLElementLDevice->GetAttribute("inst");
        const QString strLNPrefix = pSCLElement->GetAttribute("prefix");
        const QString strLNClass = pSCLElement->GetAttribute("lnClass");
        const QString strLNInst = pSCLElement->GetAttribute("inst");
        const QString strLNInfo = strLNPrefix + strLNClass + strLNInst;
        if(!strLNInfo.contains(QRegExp(mapRx.value("LNode"))))
            return lstRxData;

        // Get the SCL Element LNodeType for current LN
        SclParser::SCLElement* pSCLElementLNodeType = SclParser::SCLHelp::GetLNTypeFromLN(pSCLElement);
        if(!pSCLElementLNodeType)
            return lstRxData;

        QList<QStringList> lstDOData;

        // Loop for all SCL Element DOs under SCL Element LNodeType
        foreach(SclParser::SCLElement *pSCLElementDO, pSCLElementLNodeType->FindChildByType("DO"))
        {
            QString strDOName = pSCLElementDO->GetAttribute("name");
            if(!strDOName.contains(QRegExp(mapRx.value("DO"))))
                continue;

            SclParser::SCLElement *pSCLElementDOI = pSCLElement->FindChild("DOI", "name", strDOName, false);
            if(!pSCLElementDOI)
                continue;

            QString strDOIDesc = pSCLElementDOI->GetAttribute("desc");
            lstDOData.append(QStringList() << "" << strDOName << "" << strDOIDesc);

            // Get the SCL Element DOType for DO
            SclParser::SCLElement *pSCLElementDOType = SclParser::SCLHelp::GetDOTypeFromDO(pSCLElementDO);
            if(!pSCLElementDOType)
                continue;

            // Loop for all SCL Element DAs under SCL Element DOType
            foreach(SclParser::SCLElement *pSCLElementDA, pSCLElementDOType->FindChildByType("DA"))
            {
                QString strDAName = pSCLElementDA->GetAttribute("name");
                if(!strDAName.contains(QRegExp(mapRx.value("DA"))))
                    continue;

                QString strFC = pSCLElementDA->GetAttribute("fc");
                if(!strFC.contains(QRegExp(mapRx.value("FC"))))
                    continue;

                if(pSCLElementDA->GetAttribute("bType") == "Struct")
                {
                    QStringList lstBDAData = PickupBDA(pSCLElementDA, mapRx);
                    foreach(const QString &strBDAName, lstBDAData)
                        lstDOData.append(QStringList() << strFC << strDOName << QString("%1.%2").arg(strDAName).arg(strBDAName) << strDOIDesc);

                }
                else
                {
                    lstDOData.append(QStringList() << strFC << strDOName << strDAName << strDOIDesc );
                }
            }

            // Loop for all SCL Element SDO under SCL Element DOType
            foreach(SclParser::SCLElement *pSCLElementSDO, pSCLElementDOType->FindChildByType("SDO"))
            {
                QList<QStringList> lstSDOData = PickupDOTypeSDO(pSCLElementSDO, pSCLElementDOI, mapRx);

                foreach(const QStringList &lst, lstSDOData)
                    lstDOData.append(QStringList() << lst.at(0) << QString("%1.%2").arg(strDOName).arg(lst.at(1)) << lst.at(2) << strDOIDesc);
            }
        }

        foreach(QStringList lst, lstDOData)
            lstRxData.append(QStringList() << strLDInst << strLNPrefix << strLNClass << strLNInst << lst.at(0) << lst.at(1) << lst.at(2) << lst.at(3));
    }

    return lstRxData;
}

QList<QStringList> VTerminalDefine::PickupDOTypeSDO(SclParser::SCLElement *pSCLElementSDO, SclParser::SCLElement *pSCLElementDOISDI, const QMap<QString, QString> &mapRx)
{
    Q_ASSERT(pSCLElementSDO && pSCLElementDOISDI);

    QList<QStringList> lstSDOData;

    // Get the name of SCL Element SDO
    QString strSDOName = pSCLElementSDO->GetAttribute("name");
    SclParser::SCLElement *pSCLElementSDI = pSCLElementDOISDI->FindChild("SDI", "name", strSDOName, false);
    if(!pSCLElementSDI)
        return lstSDOData;

    // Get the SCL Element DOType for current SDO
    SclParser::SCLElement *pSCLElementDOType = SclParser::SCLHelp::GetDOTypeFromDO(pSCLElementSDO);
    if(!pSCLElementDOType)
        return lstSDOData;

    // Loop for all SCL Element DAs under SCL Element DOType
    foreach(SclParser::SCLElement *pSCLElementDA, pSCLElementDOType->FindChildByType("DA"))
    {
        QString strDAName = pSCLElementDA->GetAttribute("name");
        if(!strDAName.contains(QRegExp(mapRx.value("DA"))))
            continue;

        QString strFC = pSCLElementDA->GetAttribute("fc");
        if(!strFC.contains(QRegExp(mapRx.value("FC"))))
            continue;

        if(pSCLElementDA->GetAttribute("bType") == "Struct")
        {
            QStringList lstDAData = PickupBDA(pSCLElementDA, mapRx);
            foreach(const QString &strDAName, lstDAData)
                lstSDOData.append(QStringList() << strFC << strSDOName << strDAName);

        }
        else
        {
            lstSDOData.append(QStringList() << strFC << strSDOName << strDAName);
        }
    }

    // Loop for all SCL Element SDO under SCL Element DOType
    foreach(SclParser::SCLElement *pSCLElementSDO, pSCLElementDOType->FindChildByType("SDO"))
    {
        QList<QStringList> lstData = PickupDOTypeSDO(pSCLElementSDO, pSCLElementSDI, mapRx);
        foreach(const QStringList &lst, lstData)
            lstSDOData.append(QStringList() << lst.at(0) << QString("%1.%2").arg(strSDOName).arg(lst.at(1)) << lst.at(2));
    }

    return lstSDOData;
}

QStringList VTerminalDefine::PickupBDA(SclParser::SCLElement *pSCLElementDA, const QMap<QString, QString> &mapRx)
{
    Q_ASSERT(pSCLElementDA);

    if(pSCLElementDA->GetAttribute("bType") != "Struct")
        return QStringList();

    // Get the SCL Element DAType for current DA
    SclParser::SCLElement* pSCLElementDAType = SclParser::SCLHelp::GetDATypeFromDA(pSCLElementDA);
    if(!pSCLElementDAType)
        return QStringList();

    QStringList lstDAData;

    // Loop for all SCL Element BDAs under SCL Element DAType
    foreach(SclParser::SCLElement *pSCLElementBDA, pSCLElementDAType->FindChildByType("BDA"))
    {
        const QString strBDAName = pSCLElementBDA->GetAttribute("name");
        if(!strBDAName.contains(QRegExp(mapRx.value("DA"))))
            continue;

        if(pSCLElementBDA->GetAttribute("bType") == "Struct")
        {
            QStringList lstBDAData = PickupBDA(pSCLElementBDA, mapRx);
            foreach(const QString &str, lstBDAData)
                lstDAData << QString("%1.%2").arg(strBDAName).arg(str);
        }
        else
        {
            lstDAData << strBDAName;
        }
    }

    return lstDAData;
}

void VTerminalDefine::ReadSettings()
{
    QSettings *pSettings = Core::MainWindow::Instance()->GetSettings();
    QMap<QString, QVariant> mapValue;

    pSettings->beginGroup(g_szSettings_GroupVirtualTerminal);

    // GooseTx
    mapValue = pSettings->value(QLatin1String(g_szSettings_GooseTx)).toMap();
    if(mapValue.isEmpty())
    {
        m_d->m_mapGooseTx = GetDefaultGooseTx();
    }
    else
    {
        foreach(const QString &strKey, mapValue.keys())
            m_d->m_mapGooseTx.insert(strKey, mapValue.value(strKey).toString());
    }

    // GooseRx
    mapValue = pSettings->value(QLatin1String(g_szSettings_GooseRx)).toMap();
    if(mapValue.isEmpty())
    {
        m_d->m_mapGooseRx = GetDefaultGooseRx();
    }
    else
    {
        foreach(const QString &strKey, mapValue.keys())
            m_d->m_mapGooseRx.insert(strKey, mapValue.value(strKey).toString());
    }

    // SvTx
    mapValue = pSettings->value(QLatin1String(g_szSettings_SvTx)).toMap();
    if(mapValue.isEmpty())
    {
        m_d->m_mapSvTx = GetDefaultSvTx();
    }
    else
    {
        foreach(const QString &strKey, mapValue.keys())
            m_d->m_mapSvTx.insert(strKey, mapValue.value(strKey).toString());
    }

    // SvRx
    mapValue = pSettings->value(QLatin1String(g_szSettings_SvRx)).toMap();
    if(mapValue.isEmpty())
    {
        m_d->m_mapSvRx = GetDefaultSvRx();
    }
    else
    {
        foreach(const QString &strKey, mapValue.keys())
            m_d->m_mapSvRx.insert(strKey, mapValue.value(strKey).toString());
    }

    pSettings->endGroup();
}

void VTerminalDefine::SaveSettings()
{
    QSettings *pSettings = Core::MainWindow::Instance()->GetSettings();
    QMap<QString, QVariant> mapValue;

    pSettings->beginGroup(g_szSettings_GroupVirtualTerminal);

    // GooseTx
    mapValue.clear();
    foreach(const QString &strKey, m_d->m_mapGooseTx.keys())
        mapValue.insert(strKey, m_d->m_mapGooseTx.value(strKey));
    pSettings->setValue(g_szSettings_GooseTx, mapValue);

    // GooseRx
    mapValue.clear();
    foreach(const QString &strKey, m_d->m_mapGooseRx.keys())
        mapValue.insert(strKey, m_d->m_mapGooseRx.value(strKey));
    pSettings->setValue(g_szSettings_GooseRx, mapValue);

    // SvTx
    mapValue.clear();
    foreach(const QString &strKey, m_d->m_mapSvTx.keys())
        mapValue.insert(strKey, m_d->m_mapSvTx.value(strKey));
    pSettings->setValue(g_szSettings_SvTx, mapValue);

    // SvRx
    mapValue.clear();
    foreach(const QString &strKey, m_d->m_mapSvRx.keys())
        mapValue.insert(strKey, m_d->m_mapSvRx.value(strKey));
    pSettings->setValue(g_szSettings_SvRx, mapValue);

    pSettings->endGroup();
}

