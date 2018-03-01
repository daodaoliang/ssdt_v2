#include <QApplication>
#include <QSettings>
#include <QVBoxLayout>
#include <QDir>
#include <QStackedWidget>
#include <QToolButton>
#include <QAction>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QSqlError>
#include <QFontDatabase>

#include "styledui/styledbutton.h"
#include "styledui/stylehelper.h"
#include "utils/waitcursor.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peproject.h"
#include "projectexplorer/peprojectversion.h"

#include "projectexplorer/pebay.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pecubicle.h"
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

#include "mainwindow.h"
#include "explorerwidget.h"
#include "cablewidget.h"
#include "openprojectdlg.h"

MainWindow* MainWindow::m_pInstance = 0;

MainWindow::MainWindow(QWidget *pParent) : StyledUi::StyledWindow(pParent)
{
    m_pInstance = this;

    qApp->setStyleSheet(StyledUi::StyleHelper::GetStyleSheet(StyledUi::StyleHelper::stBlue));
    SetWindowTitle(tr("Smart Label V1.0"));

    int iId = QFontDatabase::addApplicationFont(QCoreApplication::applicationDirPath() + QLatin1String("/../share/font/msyh.ttc"));
    if(iId != -1)
    {
        QStringList lstFamilies = QFontDatabase::applicationFontFamilies(iId);
        if(!lstFamilies.empty())
            qApp->setFont(QFont(lstFamilies.first(), 9));
    }

    m_pExplorerWidget = new ExplorerWidget(this);
    m_pCableWidget = new CableWidget(this);

    SetupToolBar();

    QHBoxLayout *pHBoxLayoutTop = new QHBoxLayout;
    pHBoxLayoutTop->setSpacing(0);
    pHBoxLayoutTop->setContentsMargins(0, 0, 0, 0);
    pHBoxLayoutTop->addWidget(m_pToolBar);
    pHBoxLayoutTop->addStretch(1);

    QHBoxLayout *pHBoxLayout = new QHBoxLayout;
    pHBoxLayout->setSpacing(0);
    pHBoxLayout->setContentsMargins(0, 0, 0, 0);
    pHBoxLayout->addWidget(m_pExplorerWidget, 0, Qt::AlignLeft);
    pHBoxLayout->addSpacing(1);
    pHBoxLayout->addWidget(m_pCableWidget);

    QVBoxLayout *pVBoxLayout = static_cast<QVBoxLayout*>(layout());
    pVBoxLayout->addLayout(pHBoxLayoutTop);
    pVBoxLayout->addLayout(pHBoxLayout);

    connect(m_pExplorerWidget, SIGNAL(sigCubicleActivated(ProjectExplorer::PeCubicle*)),
            m_pCableWidget, SLOT(SlotSetCubicle(ProjectExplorer::PeCubicle*)));

    foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, ProjectExplorer::ProjectManager::Instance()->GetAllProjectVersions())
    {
        if(pProjectVersion->IsOpend())
        {
            m_pExplorerWidget->SetProjectVersion(pProjectVersion);
            return;
        }
    }
}

MainWindow::~MainWindow()
{
}

MainWindow* MainWindow::Instance()
{
    return m_pInstance;
}

bool MainWindow::Initialize()
{
    ShowMaximized(true);
    return true;
}

void MainWindow::SetupToolBar()
{
    m_pToolBar = new QWidget(this);

    m_pButtonOpen = new StyledUi::StyledButton(":/label/images/open.png", tr("Open Project..."), m_pToolBar);
    m_pButtonOpen->SetSize(QSize(28, 28));
    m_pButtonClose = new StyledUi::StyledButton(":/label/images/close.png", tr("Close Project"), m_pToolBar);
    m_pButtonClose->SetSize(QSize(28, 28));
    m_pButtonRefresh = new StyledUi::StyledButton(":/label/images/refresh.png", tr("Refresh Project"), m_pToolBar);
    m_pButtonRefresh->SetSize(QSize(28, 28));
    m_pButtonExportDb = new StyledUi::StyledButton(":/label/images/exportdb.png", tr("Generate Data File..."), m_pToolBar);
    m_pButtonExportDb->SetSize(QSize(28, 28));
    m_pButtonExportExcel = new StyledUi::StyledButton(":/label/images/exportexcel.png", tr("Generate Label File..."), m_pToolBar);
    m_pButtonExportExcel->SetSize(QSize(28, 28));

    QHBoxLayout *pHBoxLayout = new QHBoxLayout(m_pToolBar);
    pHBoxLayout->addWidget(m_pButtonOpen);
    pHBoxLayout->addSpacing(40);
    pHBoxLayout->addWidget(m_pButtonClose);
    pHBoxLayout->addSpacing(40);
    pHBoxLayout->addWidget(m_pButtonRefresh);
    pHBoxLayout->addSpacing(40);
    pHBoxLayout->addWidget(m_pButtonExportDb);
    pHBoxLayout->addSpacing(40);
    pHBoxLayout->addWidget(m_pButtonExportExcel);
    pHBoxLayout->addStretch(1);

    connect(m_pButtonOpen, SIGNAL(sigClicked()), this, SLOT(SlotActionOpen()));
    connect(m_pButtonClose, SIGNAL(sigClicked()), this, SLOT(SlotActionClose()));
    connect(m_pButtonRefresh, SIGNAL(sigClicked()), this, SLOT(SlotActionRefresh()));
    connect(m_pButtonExportDb, SIGNAL(sigClicked()), this, SLOT(SlotActionExportDb()));
    connect(m_pButtonExportExcel, SIGNAL(sigClicked()), this, SLOT(SlotActionExportExcel()));
}

bool MainWindow::ExportSqliteDb(const QString &strSqliteFileName)
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pExplorerWidget->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(strSqliteFileName);
    if(!db.open())
        return false;

    if(!db.transaction())
        return false;

    // Write Bay
    foreach(ProjectExplorer::PeBay *pBay, pProjectVersion->GetAllBays())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO bay ('bay_id', 'name', 'number', 'vlevel') VALUES (?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pBay->GetId());
        SqlQuery.bindValue(1, pBay->GetName());
        SqlQuery.bindValue(2, pBay->GetNumber());
        SqlQuery.bindValue(3, pBay->GetVLevel());

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    // Write Room
    foreach(ProjectExplorer::PeRoom *pRoom, pProjectVersion->GetAllRooms())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO room ('room_id', 'name', 'number', 'yard') VALUES (?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pRoom->GetId());
        SqlQuery.bindValue(1, pRoom->GetName());
        SqlQuery.bindValue(2, pRoom->GetNumber());
        SqlQuery.bindValue(3, pRoom->GetYard());

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    // Write Cubicle
    foreach(ProjectExplorer::PeCubicle *pCubicle, pProjectVersion->GetAllCubicles())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO cubicle ('cubicle_id', 'room_id', 'name', 'number', 'manufacture') VALUES (?, ?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pCubicle->GetId());
        SqlQuery.bindValue(1, pCubicle->GetParentRoom() ?  pCubicle->GetParentRoom()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(2, pCubicle->GetName());
        SqlQuery.bindValue(3, pCubicle->GetNumber());
        SqlQuery.bindValue(4, pCubicle->GetManufacture());

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    // Write Device
    foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO device ('device_id', 'cubicle_id', 'cubicle_pos', 'cubicle_num', 'bay_id', 'device_type', 'name', 'device_category', 'bay_category', 'vlevel', 'circuit_index', 'set', 'description', 'manufacture', 'type', 'config_version') VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pDevice->GetId());
        SqlQuery.bindValue(1, pDevice->GetParentCubicle() ?  pDevice->GetParentCubicle()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(2, pDevice->GetCubiclePos());
        SqlQuery.bindValue(3, pDevice->GetCubicleNum());
        SqlQuery.bindValue(4, pDevice->GetParentBay() ? pDevice->GetParentBay()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(5, pDevice->GetDeviceType());
        SqlQuery.bindValue(6, pDevice->GetName());
        SqlQuery.bindValue(7, pDevice->GetDeviceCategory());
        SqlQuery.bindValue(8, pDevice->GetBayCategory());
        SqlQuery.bindValue(9, pDevice->GetVLevel());
        SqlQuery.bindValue(10, pDevice->GetCircuitIndex());
        SqlQuery.bindValue(11, pDevice->GetSet());
        SqlQuery.bindValue(12, pDevice->GetDescription());
        SqlQuery.bindValue(13, pDevice->GetManufacture());
        SqlQuery.bindValue(14, pDevice->GetType());
        SqlQuery.bindValue(15, pDevice->GetConfigVersion());

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    // Write Board
    foreach(ProjectExplorer::PeBoard *pBoard, pProjectVersion->GetAllBoards())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO board ('board_id', 'device_id', 'position', 'description', 'type') VALUES (?, ?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pBoard->GetId());
        SqlQuery.bindValue(1, pBoard->GetParentDevice() ?  pBoard->GetParentDevice()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(2, pBoard->GetPosition());
        SqlQuery.bindValue(3, pBoard->GetDescription());
        SqlQuery.bindValue(4, pBoard->GetType());

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    // Write Port
    foreach(ProjectExplorer::PePort *pPort, pProjectVersion->GetAllPorts())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO port ('port_id', 'board_id', 'name', 'group', 'type', 'direction', 'fiber_plug', 'fiber_mode', 'cascade') VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pPort->GetId());
        SqlQuery.bindValue(1, pPort->GetParentBoard() ?  pPort->GetParentBoard()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(2, pPort->GetName());
        SqlQuery.bindValue(3, pPort->GetGroup());
        SqlQuery.bindValue(4, pPort->GetPortType());
        SqlQuery.bindValue(5, pPort->GetPortDirection());
        SqlQuery.bindValue(6, pPort->GetFiberPlug());
        SqlQuery.bindValue(7, pPort->GetFiberMode());
        SqlQuery.bindValue(8, int(pPort->GetCascade()));

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    // Write InfoSet
    foreach(ProjectExplorer::PeInfoSet *pInfoSet, pProjectVersion->GetAllInfoSets())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO infoset ('infoset_id', 'name', 'description', 'type', 'group', 'txied_id', 'txiedport_id', 'rxied_id', 'rxiedport_id', 'switch1_id', 'switch1_txport_id', 'switch1_rxport_id', 'switch2_id', 'switch2_txport_id', 'switch2_rxport_id', 'switch3_id', 'switch3_txport_id', 'switch3_rxport_id', 'switch4_id', 'switch4_txport_id', 'switch4_rxport_id') VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pInfoSet->GetId());
        SqlQuery.bindValue(1, pInfoSet->GetName());
        SqlQuery.bindValue(2, pInfoSet->GetDescription());
        SqlQuery.bindValue(3, pInfoSet->GetInfoSetType());
        SqlQuery.bindValue(4, pInfoSet->GetGroup());
        SqlQuery.bindValue(5, pInfoSet->GetTxIED() ? pInfoSet->GetTxIED()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(6, pInfoSet->GetTxPort() ? pInfoSet->GetTxPort()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(7, pInfoSet->GetRxIED() ? pInfoSet->GetRxIED()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(8, pInfoSet->GetRxPort() ? pInfoSet->GetRxPort()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(9, pInfoSet->GetSwitch1() ? pInfoSet->GetSwitch1()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(10, pInfoSet->GetSwitch1TxPort() ? pInfoSet->GetSwitch1TxPort()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(11, pInfoSet->GetSwitch1RxPort() ? pInfoSet->GetSwitch1RxPort()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(12, pInfoSet->GetSwitch2() ? pInfoSet->GetSwitch2()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(13, pInfoSet->GetSwitch2TxPort() ? pInfoSet->GetSwitch2TxPort()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(14, pInfoSet->GetSwitch2RxPort() ? pInfoSet->GetSwitch2RxPort()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(15, pInfoSet->GetSwitch3() ? pInfoSet->GetSwitch3()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(16, pInfoSet->GetSwitch3TxPort() ? pInfoSet->GetSwitch3TxPort()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(17, pInfoSet->GetSwitch3RxPort() ? pInfoSet->GetSwitch3RxPort()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(18, pInfoSet->GetSwitch4() ? pInfoSet->GetSwitch4()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(19, pInfoSet->GetSwitch4TxPort() ? pInfoSet->GetSwitch4TxPort()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(20, pInfoSet->GetSwitch4RxPort() ? pInfoSet->GetSwitch4RxPort()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    // Write Cubicle Connection
    foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, pProjectVersion->GetAllCubicleConns())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO cubicle_connection ('connection_id', 'cubicle1_id', 'use_odf1', 'passcubicle1_id', 'cubicle2_id', 'use_odf2', 'passcubicle2_id') VALUES (?, ?, ?, ?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pCubicleConn->GetId());
        SqlQuery.bindValue(1, pCubicleConn->GetCubicle1() ? pCubicleConn->GetCubicle1()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(2, int(pCubicleConn->GetUseOdf1()));
        SqlQuery.bindValue(3, pCubicleConn->GetPassCubicle1() ? pCubicleConn->GetPassCubicle1()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(4, pCubicleConn->GetCubicle2() ? pCubicleConn->GetCubicle2()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(5, int(pCubicleConn->GetUseOdf2()));
        SqlQuery.bindValue(6, pCubicleConn->GetPassCubicle2() ? pCubicleConn->GetPassCubicle2()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    // Write VTerminal
    foreach(ProjectExplorer::PeVTerminal *pVTerminal, pProjectVersion->GetAllVTerminals())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO vterminal ('vterminal_id', 'device_id', 'type', 'direction', 'vterminal_no', 'ied_desc', 'pro_desc', 'ld_inst', 'ln_prefix', 'ln_class', 'ln_inst', 'do_name', 'da_name') VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pVTerminal->GetId());
        SqlQuery.bindValue(1, pVTerminal->GetParentDevice() ? pVTerminal->GetParentDevice()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(2, pVTerminal->GetType());
        SqlQuery.bindValue(3, pVTerminal->GetDirection());
        SqlQuery.bindValue(4, pVTerminal->GetNumber());
        SqlQuery.bindValue(5, pVTerminal->GetIEDDesc());
        SqlQuery.bindValue(6, pVTerminal->GetProDesc());
        SqlQuery.bindValue(7, pVTerminal->GetLDInst());
        SqlQuery.bindValue(8, pVTerminal->GetLNPrefix());
        SqlQuery.bindValue(9, pVTerminal->GetLNClass());
        SqlQuery.bindValue(10, pVTerminal->GetLNInst());
        SqlQuery.bindValue(11, pVTerminal->GetDOName());
        SqlQuery.bindValue(12, pVTerminal->GetDAName());

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    // Write Strap
    foreach(ProjectExplorer::PeStrap *pStrap, pProjectVersion->GetAllStraps())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO strap ('strap_id', 'device_id', 'strap_no', 'description', 'ld_inst', 'ln_prefix', 'ln_class', 'ln_inst', 'do_name', 'da_name') VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pStrap->GetId());
        SqlQuery.bindValue(1, pStrap->GetParentDevice() ? pStrap->GetParentDevice()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(2, pStrap->GetNumber());
        SqlQuery.bindValue(3, pStrap->GetDescription());
        SqlQuery.bindValue(4, pStrap->GetLDInst());
        SqlQuery.bindValue(5, pStrap->GetLNPrefix());
        SqlQuery.bindValue(6, pStrap->GetLNClass());
        SqlQuery.bindValue(7, pStrap->GetLNInst());
        SqlQuery.bindValue(8, pStrap->GetDOName());
        SqlQuery.bindValue(9, pStrap->GetDAName());

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    // Write VTerminalConnection
    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, pProjectVersion->GetAllVTerminalConns())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO vterminal_connection ('connection_id', 'txvterminal_id', 'txstrap_id', 'rxvterminal_id', 'rxstrap_id', 'straight') VALUES (?, ?, ?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pVTerminalConn->GetId());
        SqlQuery.bindValue(1, pVTerminalConn->GetTxVTerminal() ? pVTerminalConn->GetTxVTerminal()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(2, pVTerminalConn->GetTxStrap() ? pVTerminalConn->GetTxStrap()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(3, pVTerminalConn->GetRxVTerminal() ? pVTerminalConn->GetRxVTerminal()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(4, pVTerminalConn->GetRxStrap() ? pVTerminalConn->GetRxStrap()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(5, int(pVTerminalConn->GetStraight()));

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    // Write Cable
    foreach(ProjectExplorer::PeCable *pCable, pProjectVersion->GetAllCables())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO cable ('cable_id', 'cubicle1_id', 'cubicle2_id', 'cable_type', 'name', 'name_bay', 'name_number', 'name_set') VALUES (?, ?, ?, ?, ?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pCable->GetId());
        SqlQuery.bindValue(1, pCable->GetCubicle1() ? pCable->GetCubicle1()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(2, pCable->GetCubicle2() ? pCable->GetCubicle2()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(3, pCable->GetCableType());
        SqlQuery.bindValue(4, pCable->GetName());
        SqlQuery.bindValue(5, pCable->GetNameBay());
        SqlQuery.bindValue(6, pCable->GetNameNumber());
        SqlQuery.bindValue(7, pCable->GetNameSet());

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    // Write Fiber
    foreach(ProjectExplorer::PeFiber *pFiber, pProjectVersion->GetAllFibers())
    {
        QSqlQuery SqlQuery(db);
        if(!SqlQuery.prepare("INSERT INTO fiber ('fiber_id', 'cable_id', 'port1_id', 'port2_id', 'index', 'fiber_color', 'pipe_color', 'reserve', 'infoset_ids') VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"))
        {
            db.rollback();
            return false;
        }

        SqlQuery.bindValue(0, pFiber->GetId());
        SqlQuery.bindValue(1, pFiber->GetParentCable() ? pFiber->GetParentCable()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(2, pFiber->GetPort1() ? pFiber->GetPort1()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(3, pFiber->GetPort2() ? pFiber->GetPort2()->GetId() : ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        SqlQuery.bindValue(4, pFiber->GetIndex());
        SqlQuery.bindValue(5, pFiber->GetFiberColor());
        SqlQuery.bindValue(6, pFiber->GetPipeColor());
        SqlQuery.bindValue(7, int(pFiber->GetReserve()));
        SqlQuery.bindValue(8, pFiber->GetInfoSetIds().join(QLatin1String(",")).isNull() ? "" : pFiber->GetInfoSetIds().join(QLatin1String(",")));

        if(!SqlQuery.exec())
        {
            db.rollback();
            return false;
        }
    }

    if(!db.commit())
        return false;

    db.close();
    return true;
}

bool MainWindow::ExportCsv(const QString &strDir)
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pExplorerWidget->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    QDir dir(strDir);
    if(!dir.exists())
        return false;

    QList<ProjectExplorer::PeCubicle*> lstSortedCubicles = pProjectVersion->GetAllCubicles();
    qSort(lstSortedCubicles.begin(), lstSortedCubicles.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    QMap<ProjectExplorer::PeCubicle*, QList<ProjectExplorer::PeCable*> > mapCubicleToCables;
    foreach(ProjectExplorer::PeCubicle *pCubicle, lstSortedCubicles)
    {
        QList<ProjectExplorer::PeCable*> lstCubicleCables;
        foreach(ProjectExplorer::PeCable *pCable, pProjectVersion->GetAllCables())
        {
            if(pCable->GetCubicle1() == pCubicle || pCable->GetCubicle2() == pCubicle)
                lstCubicleCables.append(pCable);
        }

        mapCubicleToCables[pCubicle] = lstCubicleCables;
    }

    // Cable
    int iCableIndex = 1;
    QStringList lstCableRows;
    foreach(ProjectExplorer::PeCubicle *pCubicle, lstSortedCubicles)
    {
        QList<ProjectExplorer::PeCable*> lstCubicleCables = mapCubicleToCables.value(pCubicle);
        qSort(lstCubicleCables.begin(), lstCubicleCables.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

        foreach(ProjectExplorer::PeCable *pCable, lstCubicleCables)
        {
            if(pCable->GetCableType() == ProjectExplorer::PeCable::ctJump)
                continue;

            const QList<ProjectExplorer::PeFiber*> lstFibers = pCable->GetChildFibers();
            int iFiberCount = lstFibers.size();
            int iRealFiberCount = 0;
            foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
            {
                if(!pFiber->GetReserve())
                    iRealFiberCount++;
            }

            QString strRow = QString("%1,%2,%3(%4),%5,%6,C:%7.%8").arg(iCableIndex++)
                                                                  .arg(pCable->GetDisplayName())
                                                                  .arg(iFiberCount)
                                                                  .arg(iRealFiberCount)
                                                                  .arg(pCubicle->GetName())
                                                                  .arg(pCable->GetCubicle1() == pCubicle ? pCable->GetCubicle2()->GetName() : pCable->GetCubicle1()->GetName())
                                                                  .arg(pCubicle->GetNumber())
                                                                  .arg(pCable->GetDisplayName());
            lstCableRows.append(strRow);
        }
    }

    // Fiber for Optical
    int iOpticalFiberIndex = 1, iOtherFiberIndex = 1;
    QStringList lstOpticalFiberRows, lstOtherFiberRows;
    foreach(ProjectExplorer::PeCubicle *pCubicle, lstSortedCubicles)
    {
        QList<ProjectExplorer::PeCable*> lstCubicleCables = mapCubicleToCables.value(pCubicle);
        qSort(lstCubicleCables.begin(), lstCubicleCables.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

        foreach(ProjectExplorer::PeCable *pCable, lstCubicleCables)
        {
            int &iFiberIndex = pCable->GetCableType() == ProjectExplorer::PeCable::ctOptical ? iOpticalFiberIndex : iOtherFiberIndex;
            QStringList &lstRows = pCable->GetCableType() == ProjectExplorer::PeCable::ctOptical ? lstOpticalFiberRows : lstOtherFiberRows;

            QList<ProjectExplorer::PeFiber*> lstFibers = pCable->GetChildFibers();
            qSort(lstFibers.begin(), lstFibers.end(), ProjectExplorer::PeProjectObject::CompareId);

            foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
            {
                QString strStartDevice = tr("Reserve");
                QString strStartPort = tr("Reserve");
                QString strEndDevice = tr("Reserve");
                QString strEndPort = tr("Reserve");
                QString strStartPortId, strEndPortId;

                ProjectExplorer::PePort *pPort1 = pFiber->GetPort1();
                ProjectExplorer::PePort *pPort2 = pFiber->GetPort2();
                if(pPort1 && pPort2)
                {
                    ProjectExplorer::PeBoard *pBoard1 = pPort1->GetParentBoard();
                    ProjectExplorer::PeBoard *pBoard2 = pPort2->GetParentBoard();
                    if(pBoard1 && pBoard2)
                    {
                        ProjectExplorer::PeDevice *pDevice1 = pBoard1->GetParentDevice();
                        ProjectExplorer::PeDevice *pDevice2 = pBoard2->GetParentDevice();
                        if(pDevice1 && pDevice2)
                        {
                            if(pDevice1->GetParentCubicle() == pCubicle)
                            {
                                strStartDevice = pDevice1->GetDescription();
                                if(strStartDevice.isEmpty())
                                    strStartDevice = pDevice1->GetName();
                                strStartPort = pDevice1->GetDeviceType() == ProjectExplorer::PeDevice::dtIED ? QString("%1/%2").arg(pBoard1->GetDisplayName()).arg(pPort1->GetDisplayName()) : pPort1->GetDisplayName();

                                strEndDevice = pDevice2->GetDescription();
                                if(strEndDevice.isEmpty())
                                    strEndDevice = pDevice2->GetName();
                                strEndPort = pDevice2->GetDeviceType() == ProjectExplorer::PeDevice::dtIED ? QString("%1/%2").arg(pBoard2->GetDisplayName()).arg(pPort2->GetDisplayName()) : pPort2->GetDisplayName();

                                strStartPortId = QString("%1.%2.%3.%4").arg(pCubicle->GetNumber())
                                                                       .arg(pDevice1->GetName())
                                                                       .arg(pBoard1->GetPosition())
                                                                       .arg(pPort1->GetName());
                                strEndPortId = QString("%1.%2.%3.%4").arg(pCubicle->GetNumber())
                                                                       .arg(pDevice2->GetName())
                                                                       .arg(pBoard2->GetPosition())
                                                                       .arg(pPort2->GetName());
                            }
                            else
                            {
                                strStartDevice = pDevice2->GetDescription();
                                if(strStartDevice.isEmpty())
                                    strStartDevice = pDevice2->GetName();
                                strStartPort = pDevice2->GetDeviceType() == ProjectExplorer::PeDevice::dtIED ? QString("%1/%2").arg(pBoard2->GetDisplayName()).arg(pPort2->GetDisplayName()) : pPort2->GetDisplayName();

                                strEndDevice = pDevice1->GetDescription();
                                if(strEndDevice.isEmpty())
                                    strEndDevice = pDevice1->GetName();
                                strEndPort = pDevice1->GetDeviceType() == ProjectExplorer::PeDevice::dtIED ? QString("%1/%2").arg(pBoard1->GetDisplayName()).arg(pPort1->GetDisplayName()) : pPort1->GetDisplayName();

                                strStartPortId = QString("%1.%2.%3.%4").arg(pCubicle->GetNumber())
                                                                       .arg(pDevice2->GetName())
                                                                       .arg(pBoard2->GetPosition())
                                                                       .arg(pPort2->GetName());
                                strEndPortId = QString("%1.%2.%3.%4").arg(pCubicle->GetNumber())
                                                                       .arg(pDevice1->GetName())
                                                                       .arg(pBoard1->GetPosition())
                                                                       .arg(pPort1->GetName());
                            }
                        }
                    }
                }

                QString strRow = QString("%1,%2,%3,%4_%5,%6,%7,%8,%9,F:%10").arg(iFiberIndex++)
                                                                            .arg(pCubicle->GetName())
                                                                            .arg(pCable->GetDisplayName())
                                                                            .arg(pCable->GetChildFibers().size())
                                                                            .arg(pCable->GetChildFibers().size() == 1 ? 1 : pFiber->GetIndex())
                                                                            .arg(strStartDevice)
                                                                            .arg(strStartPort)
                                                                            .arg(strEndDevice)
                                                                            .arg(strEndPort)
                                                                            .arg(strStartPortId);

                lstRows.append(strRow);

                if(pCable->GetCubicle1() == pCable->GetCubicle2())
                {
                    QString strRow = QString("%1,%2,%3,%4_%5,%6,%7,%8,%9,F:%10").arg(iFiberIndex++)
                                                                                .arg(pCubicle->GetName())
                                                                                .arg(pCable->GetDisplayName())
                                                                                .arg(pCable->GetChildFibers().size())
                                                                                .arg(pCable->GetChildFibers().size() == 1 ? 1 : pFiber->GetIndex())
                                                                                .arg(strEndDevice)
                                                                                .arg(strEndPort)
                                                                                .arg(strStartDevice)
                                                                                .arg(strStartPort)
                                                                                .arg(strEndPortId);

                    lstRows.append(strRow);
                }
            }
        }
    }

    // Write Cable
    QFile fCable(dir.absoluteFilePath("label_cable.csv"));
    if(!fCable.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    QTextStream tsCable(&fCable);
    tsCable << QString("%1,%2,%3,%4,%5,%6\n").arg(tr("Cable Index"))
                                             .arg(tr("Cable Number"))
                                             .arg(tr("Cable Information"))
                                             .arg(tr("Start Cubicle"))
                                             .arg(tr("End Cubicle"))
                                             .arg(tr("Cable Code"));
    foreach(const QString &strCableRow, lstCableRows)
        tsCable << strCableRow << "\n";

    tsCable.flush();
    fCable.close();

    // Write Optical Fiber
    QFile fOpticalFiber(dir.absoluteFilePath("label_opticalfiber.csv"));
    if(!fOpticalFiber.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    QTextStream tsOpticalFiber(&fOpticalFiber);
    tsOpticalFiber << QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\n").arg(tr("Fiber Index"))
                                                             .arg(tr("Parent Cubicle"))
                                                             .arg(tr("Cable Number"))
                                                             .arg(tr("Fiber Number"))
                                                             .arg(tr("Start Device"))
                                                             .arg(tr("Start Port"))
                                                             .arg(tr("End Device"))
                                                             .arg(tr("End Port"))
                                                             .arg(tr("Fiber Code"));
    foreach(const QString &strOpticalFiberRow, lstOpticalFiberRows)
        tsOpticalFiber << strOpticalFiberRow << "\n";

    tsOpticalFiber.flush();
    fOpticalFiber.close();

    // Write Other Fiber
    QFile fOtherFiber(dir.absoluteFilePath("label_otherfiber.csv"));
    if(!fOtherFiber.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    QTextStream tsOtherFiber(&fOtherFiber);
    tsOtherFiber << QString("%1,%2,%3,%4,%5,%6,%7,%8,%9\n").arg(tr("Fiber Index"))
                                                           .arg(tr("Parent Cubicle"))
                                                           .arg(tr("Cable Number"))
                                                           .arg(tr("Fiber Number"))
                                                           .arg(tr("Start Device"))
                                                           .arg(tr("Start Port"))
                                                           .arg(tr("End Device"))
                                                           .arg(tr("End Port"))
                                                           .arg(tr("Fiber Code"));
    foreach(const QString &strOtherFiberRow, lstOtherFiberRows)
        tsOtherFiber << strOtherFiberRow << "\n";

    tsOtherFiber.flush();
    fOtherFiber.close();

    return true;
}

void MainWindow::SlotActionOpen()
{
    OpenProjectDlg dlg(this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(ProjectExplorer::PeProjectVersion *pProjectVersion = dlg.GetSelectedProjectVersion())
    {
        SlotActionClose();

        pProjectVersion->Open();
        m_pExplorerWidget->SetProjectVersion(pProjectVersion);
    }
}

void MainWindow::SlotActionClose()
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(ProjectExplorer::PeProjectVersion *pProjectVersion = m_pExplorerWidget->GetProjectVersion())
    {
        m_pExplorerWidget->SetProjectVersion(0);

        pProjectVersion->Close();
    }
}

void MainWindow::SlotActionRefresh()
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(ProjectExplorer::PeProjectVersion *pProjectVersion = m_pExplorerWidget->GetProjectVersion())
    {
        m_pExplorerWidget->SetProjectVersion(0);
        pProjectVersion->Reopen();
        m_pExplorerWidget->SetProjectVersion(pProjectVersion);
    }
}

void MainWindow::SlotActionExportDb()
{
    QFile FileTemplate(QCoreApplication::applicationDirPath() + QLatin1String("/../share/templates/db_sqlite/template.db"));
    if(!FileTemplate.exists())
        return;

    const QString strSqliteFileName = QFileDialog::getSaveFileName(this,
                                                                   tr("Generate Data File"),
                                                                   "",
                                                                   tr("Sqlite File (*.sqlite)"));

    if(!strSqliteFileName.isEmpty())
    {
        QFile FileNew(strSqliteFileName);
        if(FileNew.exists() && !FileNew.remove())
        {
            QMessageBox::critical(this, tr("Error"), tr("Failed to generate data file"));
            return;
        }

        if(!FileTemplate.copy(strSqliteFileName))
        {
            QMessageBox::critical(this, tr("Error"), tr("Failed to generate data file"));
            return;
        }

        qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
        bool bRet = ExportSqliteDb(strSqliteFileName);
        qApp->restoreOverrideCursor();

        if(!bRet)
            QMessageBox::critical(this, tr("Failure"), tr("Failed to generate data file"));
        else
            QMessageBox::information(this, tr("Success"), tr("Succeed to generate data file"));
    }
}

void MainWindow::SlotActionExportExcel()
{
    const QString strDir = QFileDialog::getExistingDirectory(this, tr("Generate Label File"));
    if(!strDir.isEmpty())
    {
        qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
        bool bRet = ExportCsv(strDir);
        qApp->restoreOverrideCursor();

        if(!bRet)
            QMessageBox::critical(this, tr("Error"), tr("Failed to generate label file"));
        else
            QMessageBox::information(this, tr("Success"), tr("Succeed to generate label file"));
    }
}
