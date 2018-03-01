#include <QApplication>
#include <QFormLayout>
#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTabBar>
#include <QStackedWidget>
#include <QHeaderView>
#include <QAction>
#include <QFile>
#include <QTime>

#include "utils/waitcursor.h"
#include "utils/pathchooser.h"
#include "utils/searchcombobox.h"
#include "sclparser/scldoccontrol.h"
#include "sclparser/scldocument.h"
#include "sclparser/sclelement.h"
#include "sclparser/sclparser_constants.h"
#include "projectexplorer/basemanager.h"
#include "projectexplorer/pbvlevel.h"
#include "projectexplorer/pbmanufacture.h"
#include "projectexplorer/pbdevicetype.h"
#include "projectexplorer/pbdevicecategory.h"
#include "projectexplorer/pbbaycategory.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peproject.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/pevterminal.h"
#include "projectexplorer/pestrap.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pevterminal.h"
#include "projectexplorer/pevterminalconn.h"
#include "projectexplorer/peinfoset.h"
#include "projectexplorer/peport.h"
#include "projectexplorer/peboard.h"
#include "core/configcoreoperation.h"
#include "core/mainwindow.h"

#include "ssdautoconndlg.h"

using namespace SsdAutoConn::Internal;

SsdAutoConnDlg::SsdAutoConnDlg(QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("SSD based auto connection"));

    m_pComboBoxProject = new QComboBox(this);
    m_pComboBoxProject->setEditable(false);
    m_pComboBoxProject->setMinimumWidth(300);

    m_pPathChooserScdFileName = new Utils::PathChooser(this);
    m_pPathChooserScdFileName->setExpectedKind(Utils::PathChooser::File);
    m_pPathChooserScdFileName->setPromptDialogFilter(tr("System Configuration Description File (*.scd)"));

    m_pTextEdit = new QTextEdit(this);

    foreach(ProjectExplorer::PeProject *pProject, ProjectExplorer::ProjectManager::Instance()->GetAllProjects())
    {
        foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, pProject->GetAllProjectVersions())
        {
            if(pProjectVersion->IsOpend())
            {
                m_pComboBoxProject->addItem(pProjectVersion->GetDisplayIcon(), pProjectVersion->GetDisplayName(), reinterpret_cast<int>(pProjectVersion));
                m_pComboBoxProject->addItem(pProjectVersion->GetDisplayIcon(), pProjectVersion->GetDisplayName(), reinterpret_cast<int>(pProjectVersion));
            }
        }
    }

    QGroupBox *pGroupBoxProject = new QGroupBox(this);
    QFormLayout *pFormLayoutProject = new QFormLayout(pGroupBoxProject);
    pFormLayoutProject->addRow(tr("Select Project:"), m_pComboBoxProject);
    pFormLayoutProject->addRow(tr("Select Scd File:"), m_pPathChooserScdFileName);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    QPushButton *pPushButtonExecute = pDialogButtonBox->addButton(tr("Execute"), QDialogButtonBox::ActionRole);
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(pPushButtonExecute, SIGNAL(clicked()), this, SLOT(SlotActionExecute()));

    QVBoxLayout *pVBoxLayout = GetClientLayout();
    pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    pVBoxLayout->addWidget(pGroupBoxProject);
    pVBoxLayout->addSpacing(10);
    pVBoxLayout->addWidget(m_pTextEdit);
    pVBoxLayout->addSpacing(15);
    pVBoxLayout->addWidget(pDialogButtonBox);

}

SsdAutoConnDlg::~SsdAutoConnDlg()
{
}

QSize SsdAutoConnDlg::sizeHint() const
{
    return QSize(1050, 600);
}

void SsdAutoConnDlg::WaitMSec(int iMSec)
{
    QTime tm;
    tm.start();
    while(tm.elapsed() < iMSec)
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void SsdAutoConnDlg::SlotActionExecute()
{
    int iIndex = m_pComboBoxProject->currentIndex();
    if(iIndex < 0)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxProject->itemData(iIndex).toInt());
    if(!pProjectVersion)
        return;

    QString strScdFileName = m_pPathChooserScdFileName->path();

    m_pTextEdit->append(tr("Parsing SCD file '%1'...").arg(strScdFileName));
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    SclParser::SCLDocument *pSCLDocument = SclParser::SCLDocControl::Instance()->OpenDocument(Core::MainWindow::Instance()->GetResourcePath() + QLatin1String("/ssdautoconn/ssdautoconn.scd"), false);
    if(!pSCLDocument)
        return;

    SclParser::SCLElement *pSCLElementRoot = pSCLDocument->Root();
    if(!pSCLElementRoot)
    {
        SclParser::SCLDocControl::Instance()->CloseDocument(pSCLDocument, false);
        return;
    }

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    ////////////////////////////////////////////////////////
    /// Prepare
    ////////////////////////////////////////////////////////
    QList<ProjectExplorer::PeBay> lstCreatedBays;
    QList<ProjectExplorer::PeDevice> lstCreatedDevices;
    QList<ProjectExplorer::PeVTerminal> lstCreatedVTerminals;
    QList<ProjectExplorer::PeStrap> lstCreatedStraps;
    QList<ProjectExplorer::PeVTerminalConn> lstCreatedVTerminalConns;

    QMap<QString, ProjectExplorer::PeBay*> mapIedNameToBay;
    QMap<ProjectExplorer::PeDevice*, SclParser::SCLElement*> mapDeviceToIed;
    QMap<QString, ProjectExplorer::PeDevice*> mapNameToDevice;
    QMap<QString, ProjectExplorer::PeVTerminal*> mapRefNameToVTerninal;

    m_pTextEdit->append(tr("Reading SSD information..."));
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // Prepare bay
    if(SclParser::SCLElement *pSCLElementSubstation = pSCLElementRoot->GetFirstChild(SclParser::Constants::g_strSclEle_Substation))
    {
        foreach(SclParser::SCLElement *pSCLElementVoltageLevel, pSCLElementSubstation->FindChildByType("VoltageLevel"))
        {
            const QString strName = pSCLElementVoltageLevel->GetAttribute("name");
            int iVLevel = strName.toInt() / 1000;

            foreach(SclParser::SCLElement *pSCLElementBay, pSCLElementVoltageLevel->FindChildByType("Bay"))
            {
                ProjectExplorer::PeBay bay;
                bay.SetNumber(pSCLElementBay->GetAttribute("name"));
                bay.SetName(pSCLElementBay->GetAttribute("desc"));
                bay.SetVLevel(iVLevel);
                lstCreatedBays.append(bay);

                ProjectExplorer::PeBay &BayTemp = lstCreatedBays.last();
                ProjectExplorer::PeBay *pBay = &BayTemp;

                foreach(SclParser::SCLElement *pSCLElementConductingEquipment, pSCLElementBay->FindChildByType("ConductingEquipment"))
                {
                    foreach(SclParser::SCLElement *pSCLElementLNode, pSCLElementConductingEquipment->FindChildByType("LNode"))
                    {
                        const QString strIedName = pSCLElementLNode->GetAttribute("iedName");
                        if(!strIedName.isEmpty())
                            mapIedNameToBay[strIedName] = pBay;
                    }
                }
            }
        }
    }

    m_pTextEdit->append(tr("Reading IED information..."));
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    // Prepare device, vterminal, strap
    const QList<SclParser::SCLElement*> lstIEDs = pSCLElementRoot->FindChildByType("IED");
    foreach(SclParser::SCLElement *pSCLElementIED, lstIEDs)
    {
        const QString strIedName = pSCLElementIED->GetAttribute("name");

        ProjectExplorer::PeDevice Device;
        Device.SetDeviceType(ProjectExplorer::PeDevice::dtIED);
        Device.SetName(strIedName);
        Device.SetDescription(pSCLElementIED->GetAttribute("desc"));
        Device.SetDescription(pSCLElementIED->GetAttribute("desc"));
        Device.SetManufacture(pSCLElementIED->GetAttribute("manufacturer"));
        Device.SetType(pSCLElementIED->GetAttribute("type"));
        Device.SetConfigVersion(pSCLElementIED->GetAttribute("configVersion"));
        Device.SetParentBay(mapIedNameToBay.value(strIedName, 0));
        lstCreatedDevices.append(Device);

        ProjectExplorer::PeDevice &DeviceTemp = lstCreatedDevices.last();
        ProjectExplorer::PeDevice *pDevice = &DeviceTemp;
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
        return;
    }

    // Handle bay update
    for(int i = 0; i < lstCreatedBays.size(); i++)
    {
        ProjectExplorer::PeBay &bay = lstCreatedBays[i];
        if(!pProjectVersion->DbCreateObject(bay, false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    // Handle device update
    foreach(ProjectExplorer::PeDevice *pDevice, mapDeviceToContent.keys())
    {
        if(pDevice->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        {
            if(!pProjectVersion->DbCreateObject(*pDevice, false))
            {
                DbTrans.Rollback();
                return;
            }
        }

        if(!pProjectVersion->DbUpdateDeviceModel(pDevice->GetId(), mapDeviceToContent.value(pDevice), false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    // Handle vterminal update
    for(int i = 0; i < lstCreatedVTerminals.size(); i++)
    {
        ProjectExplorer::PeVTerminal &vterminal = lstCreatedVTerminals[i];
        if(!pProjectVersion->DbCreateObject(vterminal, false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    // Handle strap update
    for(int i = 0; i < lstCreatedStraps.size(); i++)
    {
        ProjectExplorer::PeStrap &strap = lstCreatedStraps[i];
        if(!pProjectVersion->DbCreateObject(strap, false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    // Handle vterminalconn update
    for(int i = 0; i < lstCreatedVTerminalConns.size(); i++)
    {
        ProjectExplorer::PeVTerminalConn &vterminalconn = lstCreatedVTerminalConns[i];
        if(!pProjectVersion->DbCreateObject(vterminalconn, false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    // Handle communication update
    if(!arCommunication.isNull() && !arCommunication.isEmpty())
    {
        if(!pProjectVersion->DbUpdateSclCommunication(arCommunication, false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    // Handle template update
    if(!arTemplate.isNull() && !arTemplate.isEmpty())
    {
        if(!pProjectVersion->DbUpdateSclTemplate(arTemplate, false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    m_pTextEdit->append(tr("Successfully create %1 bays").arg(lstCreatedBays.size()));
    m_pTextEdit->append(tr("Successfully create %1 IEDs").arg(mapDeviceToContent.size()));
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

    QFile file(Core::MainWindow::Instance()->GetResourcePath() + QLatin1String("/ssdautoconn/ssdautoconn.txt"));
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream TextStream(&file);
        while(!TextStream.atEnd())
        {
            QString strLine = TextStream.readLine();
            QStringList lstLines = strLine.split(QLatin1String(","));
            if(lstLines.size() == 1)
            {
                m_pTextEdit->append(lstLines.at(0));
            }
            else if(lstLines.size() == 2)
            {
                m_pTextEdit->append(lstLines.at(0));

                bool bOk;
                int iSleep = strLine.toInt(&bOk);
                if(bOk)
                    WaitMSec(iSleep);
            }
        }
    }

    if(!DbTrans.Commit())
        return;

    pProjectVersion->Reopen();
}
