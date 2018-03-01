#include <QDateTime>
#include <QMessageBox>
#include <QSettings>
#include <QCryptographicHash>
#include <QInputDialog>

#include "mysqlwrapper/mysqldatabase.h"
#include "mysqlwrapper/mysqlquery.h"
#include "mysqlwrapper/mysqltransaction.h"
#include "sclparser/sclelement.h"

#include "peprojectversion.h"
#include "databasemanager.h"
#include "projectmanager.h"
#include "peproject.h"
#include "peroom.h"
#include "pecubicle.h"
#include "pebay.h"
#include "pedevice.h"
#include "peboard.h"
#include "peport.h"
#include "peinfoset.h"
#include "pecubicleconn.h"
#include "pevterminal.h"
#include "pestrap.h"
#include "pevterminalconn.h"
#include "pecable.h"
#include "pefiber.h"
#include "pevolume.h"
#include "pedwg.h"

namespace ProjectExplorer {

class PeProjectVersionPrivate
{
public:
    PeProjectVersionPrivate(PeProject *pProject) : m_pProject(pProject), m_bOpend(false), m_iVersionNumber(-1), m_strModifier(""), m_strWhat(""), m_strWhy(""), m_strVersionPwd("")
    {
    }

    ~PeProjectVersionPrivate()
    {
        qDeleteAll(m_mapIdToRooms);
        m_mapIdToRooms.clear();

        qDeleteAll(m_mapIdToCubicles);
        m_mapIdToCubicles.clear();

        qDeleteAll(m_mapIdToBays);
        m_mapIdToBays.clear();

        qDeleteAll(m_mapIdToDevices);
        m_mapIdToDevices.clear();

        qDeleteAll(m_mapIdToBoards);
        m_mapIdToBoards.clear();

        qDeleteAll(m_mapIdToPorts);
        m_mapIdToPorts.clear();

        qDeleteAll(m_mapIdToInfoSets);
        m_mapIdToInfoSets.clear();

        qDeleteAll(m_mapIdToCubicleConns);
        m_mapIdToCubicleConns.clear();

        qDeleteAll(m_mapIdToVTerminals);
        m_mapIdToVTerminals.clear();

        qDeleteAll(m_mapIdToStraps);
        m_mapIdToStraps.clear();

        qDeleteAll(m_mapIdToVTerminalConns);
        m_mapIdToVTerminalConns.clear();

        qDeleteAll(m_mapIdToCables);
        m_mapIdToCables.clear();

        qDeleteAll(m_mapIdToFibers);
        m_mapIdToFibers.clear();

        qDeleteAll(m_mapIdToVolumes);
        m_mapIdToVolumes.clear();

        qDeleteAll(m_mapIdToDwgs);
        m_mapIdToDwgs.clear();
    }

    PeProject                   *m_pProject;
    bool                        m_bOpend;

    int                         m_iVersionNumber;
    QString                     m_strStage;
    QString                     m_strModifier;
    QDateTime                   m_dtModification;
    QString                     m_strWhat;
    QString                     m_strWhy;
    QString                     m_strVersionPwd;

    QMap<int, PeRoom*>          m_mapIdToRooms;
    QMap<int, PeCubicle*>       m_mapIdToCubicles;
    QMap<int, PeBay*>           m_mapIdToBays;
    QMap<int, PeDevice*>        m_mapIdToDevices;
    QMap<int, PeBoard*>         m_mapIdToBoards;
    QMap<int, PePort*>          m_mapIdToPorts;
    QMap<int, PeInfoSet*>       m_mapIdToInfoSets;
    QMap<int, PeCubicleConn*>   m_mapIdToCubicleConns;
    QMap<int, PeVTerminal*>     m_mapIdToVTerminals;
    QMap<int, PeStrap*>         m_mapIdToStraps;
    QMap<int, PeVTerminalConn*> m_mapIdToVTerminalConns;
    QMap<int, PeCable*>         m_mapIdToCables;
    QMap<int, PeFiber*>         m_mapIdToFibers;
    QMap<int, PeVolume*>        m_mapIdToVolumes;
    QMap<int, PeDwg*>           m_mapIdToDwgs;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeProjectVersion::PeProjectVersion() :
    PeProjectObject(PeProjectObject::otProjectVersion, this, PeProjectObject::m_iInvalidObjectId), m_d(new PeProjectVersionPrivate(0))
{
}

PeProjectVersion::PeProjectVersion(const PeProjectVersion &ProjectVersion) :
    PeProjectObject(ProjectVersion), m_d(new PeProjectVersionPrivate(0))
{
    SetProjectVersion(this);
    m_d->m_pProject = ProjectVersion.m_d->m_pProject;
    m_d->m_iVersionNumber = ProjectVersion.m_d->m_iVersionNumber;
    m_d->m_strStage = ProjectVersion.m_d->m_strStage;
    m_d->m_strModifier = ProjectVersion.m_d->m_strModifier;
    m_d->m_dtModification = ProjectVersion.m_d->m_dtModification;
    m_d->m_strWhat = ProjectVersion.m_d->m_strWhat;
    m_d->m_strWhy = ProjectVersion.m_d->m_strWhy;
    m_d->m_strVersionPwd = ProjectVersion.m_d->m_strVersionPwd;
}

PeProjectVersion::PeProjectVersion(PeProject *pProject, int iId) :
    PeProjectObject(PeProjectObject::otProjectVersion, this, iId), m_d(new PeProjectVersionPrivate(pProject))
{
}

PeProjectVersion::~PeProjectVersion()
{
    delete m_d;
}

QString PeProjectVersion::GetDisplayName() const
{
    return QString("%1 (V%2)").arg(m_d->m_pProject->GetDisplayName()).arg(m_d->m_iVersionNumber);
}

PeProjectVersion& PeProjectVersion::operator=(const PeProjectVersion &ProjectVersion)
{
    PeProjectObject::operator =(ProjectVersion);
    *m_d = *ProjectVersion.m_d;

    return *this;
}

bool PeProjectVersion::Open()
{
    if(m_d->m_bOpend)
        return true;

    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    MySqlWrapper::MySqlQuery query(pDatabase);

    // Read Bay
    query.Clear();
    if(!query.Exec("SELECT * FROM `bay`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PeBay *pBay = new PeBay(this, record.value("bay_id").toInt());
        pBay->SetName(record.value("name").toString());
        pBay->SetNumber(record.value("number").toString());
        pBay->SetVLevel(record.value("vlevel").toInt());

        m_d->m_mapIdToBays.insert(pBay->GetId(), pBay);
    }

    // Read Room
    query.Clear();
    if(!query.Exec("SELECT * FROM `room`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PeRoom *pRoom = new PeRoom(this, record.value("room_id").toInt());
        pRoom->SetName(record.value("name").toString());
        pRoom->SetNumber(record.value("number").toString());
        pRoom->SetYard(record.value("yard").toBool());

        m_d->m_mapIdToRooms.insert(pRoom->GetId(), pRoom);
    }

    // Read Cubicle
    query.Clear();
    if(!query.Exec("SELECT * FROM `cubicle`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PeCubicle *pCubicle = new PeCubicle(this, record.value("cubicle_id").toInt());
        pCubicle->SetName(record.value("name").toString());
        pCubicle->SetNumber(record.value("number").toString());
        pCubicle->SetManufacture(record.value("manufacture").toString());

        if(PeRoom *pRoom = FindRoomById(record.value("room_id").toInt()))
        {
            pCubicle->SetParentRoom(pRoom);
            pRoom->AddChildCubicle(pCubicle);
        }

        m_d->m_mapIdToCubicles.insert(pCubicle->GetId(), pCubicle);
    }

    // Read Device Content
    query.Clear();
    if(!query.Exec("SELECT `device_id` FROM `device` WHERE `icd_content` IS NOT NULL"))
        return false;

    QList<int> lstDeviceIdForNotNull;
    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
        lstDeviceIdForNotNull.append(record.value("device_id").toInt());

    // Read Device
    query.Clear();
    if(!query.Exec("SELECT * FROM `device`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PeDevice *pDevice = new PeDevice(this, record.value("device_id").toInt());
        pDevice->SetCubiclePos(record.value("cubicle_pos").toString());
        pDevice->SetCubicleNum(record.value("cubicle_num").toString());
        pDevice->SetDeviceType(PeDevice::DeviceType(record.value("device_type").toInt()));
        pDevice->SetName(record.value("name").toString());
        pDevice->SetDeviceCategory(record.value("device_category").toString());
        pDevice->SetBayCategory(record.value("bay_category").toString());
        pDevice->SetVLevel(record.value("vlevel").toInt());
        pDevice->SetCircuitIndex(record.value("circuit_index").toString());
        pDevice->SetSet(record.value("set").toString());
        pDevice->SetDescription(record.value("description").toString());
        pDevice->SetManufacture(record.value("manufacture").toString());
        pDevice->SetType(record.value("type").toString());
        pDevice->SetConfigVersion(record.value("config_version").toString());
        pDevice->SetIcdFileName(record.value("icd_filename").toString());

        if(PeCubicle *pCubicle = FindCubicleById(record.value("cubicle_id").toInt()))
        {
            pDevice->SetParentCubicle(pCubicle);
            pCubicle->AddChildDevice(pDevice);
        }

        if(PeBay *pBay = FindBayById(record.value("bay_id").toInt()))
        {
            pDevice->SetParentBay(pBay);
            pBay->AddChildDevice(pDevice);
        }

        pDevice->SetHasModel(lstDeviceIdForNotNull.contains(pDevice->GetId()));

        m_d->m_mapIdToDevices.insert(pDevice->GetId(), pDevice);
    }

    // Read Board
    query.Clear();
    if(!query.Exec("SELECT * FROM `board`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        const int iDeviceId = record.value("device_id").toInt();
        if(PeDevice *pDevice = FindDeviceById(iDeviceId))
        {
            PeBoard *pBoard = new PeBoard(this, record.value("board_id").toInt());
            pBoard->SetPosition(record.value("position").toString());
            pBoard->SetDescription(record.value("description").toString());
            pBoard->SetType(record.value("type").toString());

            pBoard->SetParentDevice(pDevice);
            pDevice->AddChildBoard(pBoard);

            m_d->m_mapIdToBoards.insert(pBoard->GetId(), pBoard);
        }
    }

    // Read Port
    query.Clear();
    if(!query.Exec("SELECT * FROM `port`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        const int iBoardId = record.value("board_id").toInt();
        if(PeBoard *pBoard = FindBoardById(iBoardId))
        {
            PePort *pPort = new PePort(this, record.value("port_id").toInt());
            pPort->SetName(record.value("name").toString());
            pPort->SetGroup(record.value("group").toInt());
            pPort->SetPortType(PePort::PortType(record.value("type").toInt()));
            pPort->SetPortDirection(PePort::PortDirection(record.value("direction").toInt()));
            pPort->SetFiberPlug(PePort::FiberPlug(record.value("fiber_plug").toInt()));
            pPort->SetFiberMode(PePort::FiberMode(record.value("fiber_mode").toInt()));
            pPort->SetCascade(record.value("cascade").toBool());

            pPort->SetParentBoard(pBoard);
            pBoard->AddChildPort(pPort);

            m_d->m_mapIdToPorts.insert(pPort->GetId(), pPort);
        }
    }

    // Read InfoSet
    query.Clear();
    if(!query.Exec("SELECT * FROM `infoset`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PeInfoSet *pInfoSet = new PeInfoSet(this, record.value("infoset_id").toInt());
        pInfoSet->SetName(record.value("name").toString());
        pInfoSet->SetDescription(record.value("description").toString());
        pInfoSet->SetInfoSetType(PeInfoSet::InfoSetType(record.value("type").toInt()));
        pInfoSet->SetGroup(record.value("group").toInt());
        pInfoSet->SetTxIED(FindDeviceById(record.value("txied_id").toInt()));
        pInfoSet->SetTxPort(FindPortById(record.value("txiedport_id").toInt()));
        pInfoSet->SetRxIED(FindDeviceById(record.value("rxied_id").toInt()));
        pInfoSet->SetRxPort(FindPortById(record.value("rxiedport_id").toInt()));
        pInfoSet->SetSwitch1(FindDeviceById(record.value("switch1_id").toInt()));
        pInfoSet->SetSwitch1TxPort(FindPortById(record.value("switch1_txport_id").toInt()));
        pInfoSet->SetSwitch1RxPort(FindPortById(record.value("switch1_rxport_id").toInt()));
        pInfoSet->SetSwitch2(FindDeviceById(record.value("switch2_id").toInt()));
        pInfoSet->SetSwitch2TxPort(FindPortById(record.value("switch2_txport_id").toInt()));
        pInfoSet->SetSwitch2RxPort(FindPortById(record.value("switch2_rxport_id").toInt()));
        pInfoSet->SetSwitch3(FindDeviceById(record.value("switch3_id").toInt()));
        pInfoSet->SetSwitch3TxPort(FindPortById(record.value("switch3_txport_id").toInt()));
        pInfoSet->SetSwitch3RxPort(FindPortById(record.value("switch3_rxport_id").toInt()));
        pInfoSet->SetSwitch4(FindDeviceById(record.value("switch4_id").toInt()));
        pInfoSet->SetSwitch4TxPort(FindPortById(record.value("switch4_txport_id").toInt()));
        pInfoSet->SetSwitch4RxPort(FindPortById(record.value("switch4_rxport_id").toInt()));

        m_d->m_mapIdToInfoSets.insert(pInfoSet->GetId(), pInfoSet);
    }

    // Read Cubicle Connection
    query.Clear();
    if(!query.Exec("SELECT * FROM `cubicle_connection`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PeCubicleConn *pCubicleConn = new PeCubicleConn(this, record.value("connection_id").toInt());
        pCubicleConn->SetCubicle1(FindCubicleById(record.value("cubicle1_id").toInt()));
        pCubicleConn->SetUseOdf1(record.value("use_odf1").toBool());
        pCubicleConn->SetPassCubicle1(FindCubicleById(record.value("passcubicle1_id").toInt()));
        pCubicleConn->SetCubicle2(FindCubicleById(record.value("cubicle2_id").toInt()));
        pCubicleConn->SetUseOdf2(record.value("use_odf2").toBool());
        pCubicleConn->SetPassCubicle2(FindCubicleById(record.value("passcubicle2_id").toInt()));

        m_d->m_mapIdToCubicleConns.insert(pCubicleConn->GetId(), pCubicleConn);
    }

    // Read VTerminal
    query.Clear();
    if(!query.Exec("SELECT * FROM `vterminal`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        const int iDeviceId = record.value("device_id").toInt();
        if(PeDevice *pDevice = FindDeviceById(iDeviceId))
        {
            PeVTerminal *pVTerminal = new PeVTerminal(this, record.value("vterminal_id").toInt());
            pVTerminal->SetType(PeVTerminal::TerminalType(record.value("type").toInt()));
            pVTerminal->SetDirection(PeVTerminal::TerminalDirection(record.value("direction").toInt()));
            pVTerminal->SetNumber(record.value("vterminal_no").toInt());
            pVTerminal->SetIEDDesc(record.value("ied_desc").toString());
            pVTerminal->SetProDesc(record.value("pro_desc").toString());
            pVTerminal->SetLDInst(record.value("ld_inst").toString());
            pVTerminal->SetLNPrefix(record.value("ln_prefix").toString());
            pVTerminal->SetLNClass(record.value("ln_class").toString());
            pVTerminal->SetLNInst(record.value("ln_inst").toString());
            pVTerminal->SetDOName(record.value("do_name").toString());
            pVTerminal->SetDAName(record.value("da_name").toString());

            pVTerminal->SetParentDevice(pDevice);
            pDevice->AddVTerminal(pVTerminal);

            m_d->m_mapIdToVTerminals.insert(pVTerminal->GetId(), pVTerminal);
        }
    }

    // Read Strap
    query.Clear();
    if(!query.Exec("SELECT * FROM `strap`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        const int iDeviceId = record.value("device_id").toInt();
        if(PeDevice *pDevice = FindDeviceById(iDeviceId))
        {
            PeStrap *pStrap = new PeStrap(this, record.value("strap_id").toInt());
            pStrap->SetNumber(record.value("strap_no").toInt());
            pStrap->SetDescription(record.value("description").toString());
            pStrap->SetLDInst(record.value("ld_inst").toString());
            pStrap->SetLNPrefix(record.value("ln_prefix").toString());
            pStrap->SetLNClass(record.value("ln_class").toString());
            pStrap->SetLNInst(record.value("ln_inst").toString());
            pStrap->SetDOName(record.value("do_name").toString());
            pStrap->SetDAName(record.value("da_name").toString());

            pStrap->SetParentDevice(pDevice);
            pDevice->AddStrap(pStrap);

            m_d->m_mapIdToStraps.insert(pStrap->GetId(), pStrap);
        }
    }

    // Read VTerminalConnection
    query.Clear();
    if(!query.Exec("SELECT * FROM `vterminal_connection`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PeVTerminal *pVTerminalTx = FindVTerminalById(record.value("txvterminal_id").toInt());
        PeVTerminal *pVTerminalRx = FindVTerminalById(record.value("rxvterminal_id").toInt());
        if(!pVTerminalTx || !pVTerminalRx)
            continue;

        const int iStrapTxId = record.value("txstrap_id").toInt();
        PeStrap *pStrapTx = 0;
        if(iStrapTxId != PeProjectObject::m_iInvalidObjectId)
            pStrapTx = FindStrapById(iStrapTxId);

        const int iStrapRxId = record.value("rxstrap_id").toInt();
        PeStrap *pStrapRx = 0;
        if(iStrapRxId != PeProjectObject::m_iInvalidObjectId)
            pStrapRx = FindStrapById(iStrapRxId);

        PeVTerminalConn *pVTerminalConn = new PeVTerminalConn(this, record.value("connection_id").toInt());
        pVTerminalConn->SetTxVTerminal(pVTerminalTx);
        pVTerminalConn->SetTxStrap(pStrapTx);
        pVTerminalConn->SetRxVTerminal(pVTerminalRx);
        pVTerminalConn->SetRxStrap(pStrapRx);
        pVTerminalConn->SetStraight(record.value("straight").toBool());

        m_d->m_mapIdToVTerminalConns.insert(pVTerminalConn->GetId(), pVTerminalConn);
    }

    // Read Cable
    query.Clear();
    if(!query.Exec("SELECT * FROM `cable`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PeCable *pCable = new PeCable(this, record.value("cable_id").toInt());
        pCable->SetCableType(PeCable::CableType(record.value("cable_type").toInt()));
        pCable->SetName(record.value("name").toString());
        pCable->SetNameBay(record.value("name_bay").toString());
        pCable->SetNameNumber(record.value("name_number").toInt());
        pCable->SetNameSet(record.value("name_set").toString());
        pCable->SetCubicle1(FindCubicleById(record.value("cubicle1_id").toInt()));
        pCable->SetCubicle2(FindCubicleById(record.value("cubicle2_id").toInt()));

        m_d->m_mapIdToCables.insert(pCable->GetId(), pCable);
    }

    // Read Fiber
    query.Clear();
    if(!query.Exec("SELECT * FROM `fiber`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        const int iCableId = record.value("cable_id").toInt();
        if(PeCable *pCable = FindCableById(iCableId))
        {
            PeFiber *pFiber = new PeFiber(this, record.value("fiber_id").toInt());
            pFiber->SetIndex(record.value("index").toInt());
            pFiber->SetFiberColor(PeFiber::FiberColor(record.value("fiber_color").toInt()));
            pFiber->SetPipeColor(PeFiber::FiberColor(record.value("pipe_color").toInt()));
            pFiber->SetReserve(record.value("reserve").toBool());
            pFiber->SetInfoSetIds(record.value("infoset_ids").toString().split(QLatin1String(",")));
            pFiber->SetPort1(FindPortById(record.value("port1_id").toInt()));
            pFiber->SetPort2(FindPortById(record.value("port2_id").toInt()));

            pFiber->SetParentCable(pCable);
            pCable->AddChildFiber(pFiber);

            m_d->m_mapIdToFibers.insert(pFiber->GetId(), pFiber);
        }
    }

    // Read Volume
    query.Clear();
    if(!query.Exec("SELECT * FROM `volume`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PeVolume *pVolume = new PeVolume(this, record.value("volume_id").toInt());
        pVolume->SetName(record.value("name").toString());
        pVolume->SetDescription(record.value("description").toString());

        m_d->m_mapIdToVolumes.insert(pVolume->GetId(), pVolume);
    }

    // Read Dwg
    query.Clear();
    if(!query.Exec("SELECT * FROM `dwg`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        const int iVolumeId = record.value("volume_id").toInt();
        if(PeVolume *pVolume = FindVolumeById(iVolumeId))
        {
            PeDwg *pDwg = new PeDwg(this, record.value("dwg_id").toInt());
            pDwg->SetDwgType(PeDwg::DwgType(record.value("type").toInt()));
            pDwg->SetName(record.value("name").toString());
            pDwg->SetDescription(record.value("description").toString());

            pDwg->SetParentVolume(pVolume);
            pVolume->AddChildDwg(pDwg);

            m_d->m_mapIdToDwgs.insert(pDwg->GetId(), pDwg);
        }
    }

    m_d->m_bOpend = true;
    emit ProjectManager::Instance()->sigProjectVersionOpened(this);

    return true;
}

void PeProjectVersion::Close()
{
    if(!m_d->m_bOpend)
        return;

    emit ProjectManager::Instance()->sigProjectVersionAboutToBeClosed(this);

    qDeleteAll(m_d->m_mapIdToRooms);
    m_d->m_mapIdToRooms.clear();

    qDeleteAll(m_d->m_mapIdToCubicles);
    m_d->m_mapIdToCubicles.clear();

    qDeleteAll(m_d->m_mapIdToBays);
    m_d->m_mapIdToBays.clear();

    qDeleteAll(m_d->m_mapIdToDevices);
    m_d->m_mapIdToDevices.clear();

    qDeleteAll(m_d->m_mapIdToBoards);
    m_d->m_mapIdToBoards.clear();

    qDeleteAll(m_d->m_mapIdToPorts);
    m_d->m_mapIdToPorts.clear();

    qDeleteAll(m_d->m_mapIdToInfoSets);
    m_d->m_mapIdToInfoSets.clear();

    qDeleteAll(m_d->m_mapIdToCubicleConns);
    m_d->m_mapIdToCubicleConns.clear();

    qDeleteAll(m_d->m_mapIdToVTerminals);
    m_d->m_mapIdToVTerminals.clear();

    qDeleteAll(m_d->m_mapIdToStraps);
    m_d->m_mapIdToStraps.clear();

    qDeleteAll(m_d->m_mapIdToVTerminalConns);
    m_d->m_mapIdToVTerminalConns.clear();

    qDeleteAll(m_d->m_mapIdToCables);
    m_d->m_mapIdToCables.clear();

    qDeleteAll(m_d->m_mapIdToFibers);
    m_d->m_mapIdToFibers.clear();

    qDeleteAll(m_d->m_mapIdToVolumes);
    m_d->m_mapIdToVolumes.clear();

    qDeleteAll(m_d->m_mapIdToDwgs);
    m_d->m_mapIdToDwgs.clear();

    m_d->m_bOpend = false;
    emit ProjectManager::Instance()->sigProjectVersionClosed(this);
}

bool PeProjectVersion::Reopen()
{
    if(!m_d->m_bOpend)
        return false;

    Close();
    return Open();
}

bool PeProjectVersion::IsOpend() const
{
    return m_d->m_bOpend;
}

PeProject* PeProjectVersion::GetProject() const
{
    return m_d->m_pProject;
}

void PeProjectVersion::SetProject(PeProject *pProject)
{
    m_d->m_pProject = pProject;
}

int PeProjectVersion::GetVersionNumber() const
{
    return m_d->m_iVersionNumber;
}

void PeProjectVersion::SetVersionNumber(int iVersionNumber)
{
    m_d->m_iVersionNumber = iVersionNumber;
}

QString PeProjectVersion::GetStage() const
{
    return m_d->m_strStage;
}

void PeProjectVersion::SetStage(const QString &strStage)
{
    m_d->m_strStage = strStage;
}

QString PeProjectVersion::GetModifier() const
{
    return m_d->m_strModifier;
}

void PeProjectVersion::SetModifier(const QString &strModifier)
{
    m_d->m_strModifier = strModifier;
}

QDateTime PeProjectVersion::GetModificationTime() const
{
    return m_d->m_dtModification;
}

void PeProjectVersion::SetModificationTime(const QDateTime &dtModification)
{
    m_d->m_dtModification = dtModification;
}

QString PeProjectVersion::GetWhat() const
{
    return m_d->m_strWhat;
}

void PeProjectVersion::SetWhat(const QString &strWhat)
{
    m_d->m_strWhat = strWhat;
}

QString PeProjectVersion::GetWhy() const
{
    return m_d->m_strWhy;
}

void PeProjectVersion::SetWhy(const QString &strWhy)
{
    m_d->m_strWhy = strWhy;
}

QString PeProjectVersion::GetVersionPwd() const
{
    return m_d->m_strVersionPwd;
}

void PeProjectVersion::SetVersionPwd(const QString &strVersionPwd)
{
    m_d->m_strVersionPwd = strVersionPwd;
}

bool PeProjectVersion::ValidateModifyTime()
{
//    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
//    if(!pDatabase)
//        return false;

//    MySqlWrapper::MySqlQuery query(pDatabase);
//    if(!query.Exec(QString("SELECT `modification_time` FROM `project_version` WHERE `version_id`='%1'").arg(GetId())))
//        return false;

//    if(query.GetResults().size() != 1)
//        return false;

//    return query.GetResults().first().value("modification_time").toDateTime() == m_d->m_dtModification;
    return true;
}

bool PeProjectVersion::UpdateModifyTime()
{
//    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
//    if(!pDatabase)
//        return false;

//    MySqlWrapper::MySqlQuery query(pDatabase);
//    if(!query.Exec(QString("UPDATE `project_version` SET `modification_time`=NOW() WHERE `version_id`='%1'").arg(GetId())))
//        return false;

//    query.Clear();
//    if(!query.Exec(QString("SELECT `modification_time` FROM `project_version` WHERE `version_id`='%1'").arg(GetId())))
//        return false;

//    if(query.GetResults().size() != 1)
//        return false;

//    m_d->m_dtModification = query.GetResults().first().value("modification_time").toDateTime();
    return true;
}

bool PeProjectVersion::ValidateVersionPwd(QWidget *pParent)
{
    if(m_d->m_strVersionPwd.isEmpty())
        return true;

    QString strVersionPwd = QInputDialog::getText(pParent, tr("Input Password"), tr("Password"), QLineEdit::Password);
    if(strVersionPwd.isEmpty())
        return false;

    QByteArray baPwdMd5 = QCryptographicHash::hash(strVersionPwd.toLatin1(), QCryptographicHash::Md5);
    QString strPwdMd5 = baPwdMd5.toHex().toUpper();

    if(strPwdMd5 != m_d->m_strVersionPwd)
    {
        QMessageBox::critical(pParent, tr("Error"), tr("Wrong password"));
        return false;
    }

    return true;
}

PeProjectObject* PeProjectVersion::CreateObject(const PeProjectObject &ProjectObject, bool bNotify)
{
    PeProjectObject *pProjectObjectCreated = 0;

    const PeProjectObject::ObjectType &eObjectType = ProjectObject.GetObjectType();
    switch(eObjectType)
    {
        case otBay:
        {
            PeBay *pBayTemp = qobject_cast<PeBay*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pBayTemp)
                return 0;

            PeBay *pBay = new PeBay(this, pBayTemp->GetId());
            pBay->SetName(pBayTemp->GetName());
            pBay->SetNumber(pBayTemp->GetNumber());
            pBay->SetVLevel(pBayTemp->GetVLevel());

            m_d->m_mapIdToBays.insert(pBay->GetId(), pBay);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pBay);
        }
        break;

        case otRoom:
        {
            PeRoom *pRoomTemp = qobject_cast<PeRoom*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pRoomTemp)
                return 0;

            PeRoom *pRoom = new PeRoom(this, pRoomTemp->GetId());
            pRoom->SetName(pRoomTemp->GetName());
            pRoom->SetNumber(pRoomTemp->GetNumber());
            pRoom->SetYard(pRoomTemp->GetYard());

            m_d->m_mapIdToRooms.insert(pRoom->GetId(), pRoom);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pRoom);
        }
        break;

        case otCubicle:
        {
            PeCubicle *pCubicleTemp = qobject_cast<PeCubicle*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pCubicleTemp)
                return 0;

            PeCubicle *pCubicle = new PeCubicle(this, pCubicleTemp->GetId());
            pCubicle->SetName(pCubicleTemp->GetName());
            pCubicle->SetNumber(pCubicleTemp->GetNumber());
            pCubicle->SetManufacture(pCubicleTemp->GetManufacture());

            if(PeRoom *pParentRoom = pCubicleTemp->GetParentRoom())
            {
                pCubicle->SetParentRoom(pParentRoom);
                pParentRoom->AddChildCubicle(pCubicle);
            }

            m_d->m_mapIdToCubicles.insert(pCubicle->GetId(), pCubicle);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pCubicle);
        }
        break;

        case otDevice:
        {
            PeDevice *pDeviceTemp = qobject_cast<PeDevice*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pDeviceTemp)
                return 0;

            PeDevice *pDevice = new PeDevice(this, pDeviceTemp->GetId());
            pDevice->SetCubiclePos(pDeviceTemp->GetCubiclePos());
            pDevice->SetCubicleNum(pDeviceTemp->GetCubicleNum());
            pDevice->SetDeviceType(pDeviceTemp->GetDeviceType());
            pDevice->SetName(pDeviceTemp->GetName());
            pDevice->SetDeviceCategory(pDeviceTemp->GetDeviceCategory());
            pDevice->SetBayCategory(pDeviceTemp->GetBayCategory());
            pDevice->SetVLevel(pDeviceTemp->GetVLevel());
            pDevice->SetCircuitIndex(pDeviceTemp->GetCircuitIndex());
            pDevice->SetSet(pDeviceTemp->GetSet());
            pDevice->SetDescription(pDeviceTemp->GetDescription());
            pDevice->SetManufacture(pDeviceTemp->GetManufacture());
            pDevice->SetType(pDeviceTemp->GetType());
            pDevice->SetConfigVersion(pDeviceTemp->GetConfigVersion());
            pDevice->SetIcdFileName(pDeviceTemp->GetIcdFileName());

            if(PeCubicle *pParentCubicle = pDeviceTemp->GetParentCubicle())
            {
                pDevice->SetParentCubicle(pParentCubicle);
                pParentCubicle->AddChildDevice(pDevice);
            }

            if(PeBay *pParentBay = pDeviceTemp->GetParentBay())
            {
                pDevice->SetParentBay(pParentBay);
                pParentBay->AddChildDevice(pDevice);
            }

            m_d->m_mapIdToDevices.insert(pDevice->GetId(), pDevice);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pDevice);
        }
        break;

        case otBoard:
        {
            PeBoard *pBoardTemp = qobject_cast<PeBoard*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pBoardTemp)
                return 0;

            PeDevice *pParentDevice = pBoardTemp->GetParentDevice();
            if(!pParentDevice)
                return 0;

            PeBoard *pBoard = new PeBoard(this, pBoardTemp->GetId());
            pBoard->SetPosition(pBoardTemp->GetPosition());
            pBoard->SetDescription(pBoardTemp->GetDescription());
            pBoard->SetType(pBoardTemp->GetType());
            pBoard->SetParentDevice(pParentDevice);
            pParentDevice->AddChildBoard(pBoard);

            m_d->m_mapIdToBoards.insert(pBoard->GetId(), pBoard);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pBoard);
        }
        break;

        case otPort:
        {
            PePort *pPortTemp = qobject_cast<PePort*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pPortTemp)
                return 0;

            PeBoard *pParentBoard = pPortTemp->GetParentBoard();
            if(!pParentBoard)
                return 0;

            PePort *pPort = new PePort(this, pPortTemp->GetId());
            pPort->SetName(pPortTemp->GetName());
            pPort->SetGroup(pPortTemp->GetGroup());
            pPort->SetPortType(pPortTemp->GetPortType());
            pPort->SetPortDirection(pPortTemp->GetPortDirection());
            pPort->SetFiberPlug(pPortTemp->GetFiberPlug());
            pPort->SetFiberMode(pPortTemp->GetFiberMode());
            pPort->SetCascade(pPortTemp->GetCascade());
            pPort->SetParentBoard(pParentBoard);
            pParentBoard->AddChildPort(pPort);

            m_d->m_mapIdToPorts.insert(pPort->GetId(), pPort);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pPort);
        }
        break;

        case otInfoSet:
        {
            PeInfoSet *pInfoSetTemp = qobject_cast<PeInfoSet*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pInfoSetTemp)
                return 0;

            PeInfoSet *pInfoSet = new PeInfoSet(this, pInfoSetTemp->GetId());
            pInfoSet->SetName(pInfoSetTemp->GetName());
            pInfoSet->SetDescription(pInfoSetTemp->GetDescription());
            pInfoSet->SetInfoSetType(pInfoSetTemp->GetInfoSetType());
            pInfoSet->SetGroup(pInfoSetTemp->GetGroup());
            pInfoSet->SetTxIED(pInfoSetTemp->GetTxIED());
            pInfoSet->SetTxPort(pInfoSetTemp->GetTxPort());
            pInfoSet->SetRxIED(pInfoSetTemp->GetRxIED());
            pInfoSet->SetRxPort(pInfoSetTemp->GetRxPort());
            pInfoSet->SetSwitch1(pInfoSetTemp->GetSwitch1());
            pInfoSet->SetSwitch1TxPort(pInfoSetTemp->GetSwitch1TxPort());
            pInfoSet->SetSwitch1RxPort(pInfoSetTemp->GetSwitch1RxPort());
            pInfoSet->SetSwitch2(pInfoSetTemp->GetSwitch2());
            pInfoSet->SetSwitch2TxPort(pInfoSetTemp->GetSwitch2TxPort());
            pInfoSet->SetSwitch2RxPort(pInfoSetTemp->GetSwitch2RxPort());
            pInfoSet->SetSwitch3(pInfoSetTemp->GetSwitch3());
            pInfoSet->SetSwitch3TxPort(pInfoSetTemp->GetSwitch3TxPort());
            pInfoSet->SetSwitch3RxPort(pInfoSetTemp->GetSwitch3RxPort());
            pInfoSet->SetSwitch4(pInfoSetTemp->GetSwitch4());
            pInfoSet->SetSwitch4TxPort(pInfoSetTemp->GetSwitch4TxPort());
            pInfoSet->SetSwitch4RxPort(pInfoSetTemp->GetSwitch4RxPort());

            m_d->m_mapIdToInfoSets.insert(pInfoSet->GetId(), pInfoSet);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pInfoSet);
        }
        break;

        case otCubicleConn:
        {
            PeCubicleConn *pCubicleConnTemp = qobject_cast<PeCubicleConn*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pCubicleConnTemp)
                return 0;

            PeCubicleConn *pCubicleConn = new PeCubicleConn(this, pCubicleConnTemp->GetId());
            pCubicleConn->SetCubicle1(pCubicleConnTemp->GetCubicle1());
            pCubicleConn->SetUseOdf1(pCubicleConnTemp->GetUseOdf1());
            pCubicleConn->SetPassCubicle1(pCubicleConnTemp->GetPassCubicle1());
            pCubicleConn->SetCubicle2(pCubicleConnTemp->GetCubicle2());
            pCubicleConn->SetUseOdf2(pCubicleConnTemp->GetUseOdf2());
            pCubicleConn->SetPassCubicle2(pCubicleConnTemp->GetPassCubicle2());

            m_d->m_mapIdToCubicleConns.insert(pCubicleConn->GetId(), pCubicleConn);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pCubicleConn);
        }
        break;

        case otVTerminal:
        {
            PeVTerminal *pVTerminalTemp = qobject_cast<PeVTerminal*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pVTerminalTemp)
                return 0;

            PeDevice *pParentDevice = pVTerminalTemp->GetParentDevice();
            if(!pParentDevice)
                return 0;

            PeVTerminal *pVTerminal = new PeVTerminal(this, pVTerminalTemp->GetId());
            pVTerminal->SetType(pVTerminalTemp->GetType());
            pVTerminal->SetDirection(pVTerminalTemp->GetDirection());
            pVTerminal->SetNumber(pVTerminalTemp->GetNumber());
            pVTerminal->SetIEDDesc(pVTerminalTemp->GetIEDDesc());
            pVTerminal->SetProDesc(pVTerminalTemp->GetProDesc());
            pVTerminal->SetLDInst(pVTerminalTemp->GetLDInst());
            pVTerminal->SetLNPrefix(pVTerminalTemp->GetLNPrefix());
            pVTerminal->SetLNClass(pVTerminalTemp->GetLNClass());
            pVTerminal->SetLNInst(pVTerminalTemp->GetLNInst());
            pVTerminal->SetDOName(pVTerminalTemp->GetDOName());
            pVTerminal->SetDAName(pVTerminalTemp->GetDAName());
            pVTerminal->SetParentDevice(pParentDevice);
            pParentDevice->AddVTerminal(pVTerminal);

            m_d->m_mapIdToVTerminals.insert(pVTerminal->GetId(), pVTerminal);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pVTerminal);
        }
        break;

        case otStrap:
        {
            PeStrap *pStrapTemp = qobject_cast<PeStrap*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pStrapTemp)
                return 0;

            PeDevice *pParentDevice = pStrapTemp->GetParentDevice();
            if(!pParentDevice)
                return 0;

            PeStrap *pStrap = new PeStrap(this, pStrapTemp->GetId());
            pStrap->SetNumber(pStrapTemp->GetNumber());
            pStrap->SetDescription(pStrapTemp->GetDescription());
            pStrap->SetLDInst(pStrapTemp->GetLDInst());
            pStrap->SetLNPrefix(pStrapTemp->GetLNPrefix());
            pStrap->SetLNClass(pStrapTemp->GetLNClass());
            pStrap->SetLNInst(pStrapTemp->GetLNInst());
            pStrap->SetDOName(pStrapTemp->GetDOName());
            pStrap->SetDAName(pStrapTemp->GetDAName());
            pStrap->SetParentDevice(pParentDevice);
            pParentDevice->AddStrap(pStrap);

            m_d->m_mapIdToStraps.insert(pStrap->GetId(), pStrap);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pStrap);
        }
        break;

        case otVTerminalConn:
        {
            PeVTerminalConn *pVTerminalConnTemp = qobject_cast<PeVTerminalConn*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pVTerminalConnTemp)
                return 0;

            PeVTerminalConn *pVTerminalConn = new PeVTerminalConn(this, pVTerminalConnTemp->GetId());
            pVTerminalConn->SetTxVTerminal(pVTerminalConnTemp->GetTxVTerminal());
            pVTerminalConn->SetRxVTerminal(pVTerminalConnTemp->GetRxVTerminal());
            pVTerminalConn->SetTxStrap(pVTerminalConnTemp->GetTxStrap());
            pVTerminalConn->SetRxStrap(pVTerminalConnTemp->GetRxStrap());
            pVTerminalConn->SetStraight(pVTerminalConnTemp->GetStraight());

            m_d->m_mapIdToVTerminalConns.insert(pVTerminalConn->GetId(), pVTerminalConn);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pVTerminalConn);
        }
        break;

        case otCable:
        {
            PeCable *pCableTemp = qobject_cast<PeCable*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pCableTemp)
                return 0;

            PeCable *pCable = new PeCable(this, pCableTemp->GetId());
            pCable->SetCableType(pCableTemp->GetCableType());
            pCable->SetName(pCableTemp->GetName());
            pCable->SetNameBay(pCableTemp->GetNameBay());
            pCable->SetNameNumber(pCableTemp->GetNameNumber());
            pCable->SetNameSet(pCableTemp->GetNameSet());
            pCable->SetCubicle1(pCableTemp->GetCubicle1());
            pCable->SetCubicle2(pCableTemp->GetCubicle2());

            m_d->m_mapIdToCables.insert(pCable->GetId(), pCable);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pCable);
        }
        break;

        case otFiber:
        {
            PeFiber *pFiberTemp = qobject_cast<PeFiber*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pFiberTemp)
                return 0;

            PeCable *pParentCable = pFiberTemp->GetParentCable();
            if(!pParentCable)
                return 0;

            PeFiber *pFiber = new PeFiber(this, pFiberTemp->GetId());
            pFiber->SetIndex(pFiberTemp->GetIndex());
            pFiber->SetFiberColor(pFiberTemp->GetFiberColor());
            pFiber->SetPipeColor(pFiberTemp->GetPipeColor());
            pFiber->SetReserve(pFiberTemp->GetReserve());
            pFiber->SetInfoSetIds(pFiberTemp->GetInfoSetIds());
            pFiber->SetPort1(pFiberTemp->GetPort1());
            pFiber->SetPort2(pFiberTemp->GetPort2());
            pFiber->SetParentCable(pParentCable);
            pParentCable->AddChildFiber(pFiber);

            m_d->m_mapIdToFibers.insert(pFiber->GetId(), pFiber);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pFiber);
        }
        break;

        case otVolume:
        {
            PeVolume *pVolumeTemp = qobject_cast<PeVolume*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pVolumeTemp)
                return 0;

            PeVolume *pVolume = new PeVolume(this, pVolumeTemp->GetId());
            pVolume->SetName(pVolumeTemp->GetName());
            pVolume->SetDescription(pVolumeTemp->GetDescription());

            m_d->m_mapIdToVolumes.insert(pVolume->GetId(), pVolume);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pVolume);
        }
        break;

        case otDwg:
        {
            PeDwg *pDwgTemp = qobject_cast<PeDwg*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pDwgTemp)
                return 0;

            PeVolume *pParentVolume = pDwgTemp->GetParentVolume();
            if(!pParentVolume)
                return 0;

            PeDwg *pDwg = new PeDwg(this, pDwgTemp->GetId());
            pDwg->SetDwgType(pDwgTemp->GetDwgType());
            pDwg->SetName(pDwgTemp->GetName());
            pDwg->SetDescription(pDwgTemp->GetDescription());
            pDwg->SetParentVolume(pParentVolume);
            pParentVolume->AddChildDwg(pDwg);

            m_d->m_mapIdToDwgs.insert(pDwg->GetId(), pDwg);
            pProjectObjectCreated = qobject_cast<PeProjectObject*>(pDwg);
        }
        break;
    }

    if(pProjectObjectCreated && bNotify)
        emit sigObjectCreated(pProjectObjectCreated);

    return pProjectObjectCreated;
}

PeProjectObject* PeProjectVersion::UpdateObject(const PeProjectObject &ProjectObject, bool bNotify)
{
    const PeProjectObject::ObjectType &eObjectType = ProjectObject.GetObjectType();
    switch(eObjectType)
    {
        case otBay:
        {
            PeBay *pBayTemp = qobject_cast<PeBay*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeBay *pBay = FindBayById(ProjectObject.GetId());
            if(!pBayTemp || !pBay)
                return 0;

            const QString strBayNumberNew = pBayTemp->GetNumber();
            const QString strBayNumberOld = pBay->GetNumber();

            // Update property
            if(pBay->GetName() != pBayTemp->GetName() ||
               pBay->GetNumber() != pBayTemp->GetNumber() ||
               pBay->GetVLevel() != pBayTemp->GetVLevel())
            {
                pBay->SetName(pBayTemp->GetName());
                pBay->SetNumber(pBayTemp->GetNumber());
                pBay->SetVLevel(pBayTemp->GetVLevel());

                // Update associated information sets and cables if the bay number changed
                if(strBayNumberNew != strBayNumberOld)
                {
                    // Update information sets
                    const QList<PeDevice*> lstChildDevices = pBay->GetChildDevices();
                    foreach(PeInfoSet *pInfoSet, m_d->m_mapIdToInfoSets.values())
                    {
                        if(lstChildDevices.contains(pInfoSet->GetTxIED()) || lstChildDevices.contains(pInfoSet->GetRxIED()))
                        {
                            PeInfoSet InfoSet(*pInfoSet);
                            InfoSet.UpdateName();
                            if(InfoSet.GetName() != pInfoSet->GetName())
                                UpdateObject(InfoSet, bNotify);
                        }
                    }

                    // Update cables
                    foreach(PeCable *pCable, m_d->m_mapIdToCables.values())
                    {
                        if(pCable->GetNameBay() == strBayNumberOld)
                            pCable->SetNameBay(strBayNumberNew);
                    }
                }

                if(bNotify)
                    emit sigObjectPropertyChanged(pBay);
            }

            return pBay;
        }
        break;

        case otRoom:
        {
            PeRoom *pRoomTemp = qobject_cast<PeRoom*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeRoom *pRoom = FindRoomById(ProjectObject.GetId());
            if(!pRoomTemp || !pRoom)
                return 0;

            // Update property
            if(pRoom->GetName() != pRoomTemp->GetName() ||
               pRoom->GetNumber() != pRoomTemp->GetNumber() ||
               pRoom->GetYard() != pRoomTemp->GetYard())
            {
                pRoom->SetName(pRoomTemp->GetName());
                pRoom->SetNumber(pRoomTemp->GetNumber());
                pRoom->SetYard(pRoomTemp->GetYard());

                if(bNotify)
                    emit sigObjectPropertyChanged(pRoom);
            }

            return pRoom;
        }
        break;

        case otCubicle:
        {
            PeCubicle *pCubicleTemp = qobject_cast<PeCubicle*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeCubicle *pCubicle = FindCubicleById(ProjectObject.GetId());
            if(!pCubicleTemp || !pCubicle)
                return 0;

            // Update property
            if(pCubicle->GetName() != pCubicleTemp->GetName() ||
               pCubicle->GetNumber() != pCubicleTemp->GetNumber() ||
               pCubicle->GetManufacture() != pCubicleTemp->GetManufacture())
            {
                pCubicle->SetName(pCubicleTemp->GetName());
                pCubicle->SetNumber(pCubicleTemp->GetNumber());
                pCubicle->SetManufacture(pCubicleTemp->GetManufacture());

                if(bNotify)
                    emit sigObjectPropertyChanged(pCubicle);
            }

            // Update parent room
            PeRoom *pParentRoomNew = pCubicleTemp->GetParentRoom();
            PeRoom *pParentRoomOld = pCubicle->GetParentRoom();
            if(pParentRoomNew != pParentRoomOld)
            {
                pCubicle->SetParentRoom(pParentRoomNew);
                if(pParentRoomOld)
                    pParentRoomOld->RemoveChildCubicle(pCubicle);
                if(pParentRoomNew)
                    pParentRoomNew->AddChildCubicle(pCubicle);

                if(bNotify)
                    emit sigObjectParentChanged(pCubicle, pParentRoomOld);
            }

            return pCubicle;
        }
        break;

        case otDevice:
        {
            PeDevice *pDeviceTemp = qobject_cast<PeDevice*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeDevice *pDevice = FindDeviceById(ProjectObject.GetId());
            if(!pDeviceTemp || !pDevice)
                return 0;

            // Update property
            if(pDevice->GetCubiclePos() != pDeviceTemp->GetCubiclePos() ||
               pDevice->GetCubicleNum() != pDeviceTemp->GetCubicleNum() ||
               pDevice->GetDeviceType() != pDeviceTemp->GetDeviceType() ||
               pDevice->GetName() != pDeviceTemp->GetName() ||
               pDevice->GetDeviceCategory() != pDeviceTemp->GetDeviceCategory() ||
               pDevice->GetBayCategory() != pDeviceTemp->GetBayCategory() ||
               pDevice->GetVLevel() != pDeviceTemp->GetVLevel() ||
               pDevice->GetCircuitIndex() != pDeviceTemp->GetCircuitIndex() ||
               pDevice->GetSet() != pDeviceTemp->GetSet() ||
               pDevice->GetDescription() != pDeviceTemp->GetDescription() ||
               pDevice->GetManufacture() != pDeviceTemp->GetManufacture() ||
               pDevice->GetType() != pDeviceTemp->GetType() ||
               pDevice->GetConfigVersion() != pDeviceTemp->GetConfigVersion() ||
               pDevice->GetIcdFileName() != pDeviceTemp->GetIcdFileName())
            {
                pDevice->SetCubiclePos(pDeviceTemp->GetCubiclePos());
                pDevice->SetCubicleNum(pDeviceTemp->GetCubicleNum());
                pDevice->SetDeviceType(pDeviceTemp->GetDeviceType());
                pDevice->SetName(pDeviceTemp->GetName());
                pDevice->SetDeviceCategory(pDeviceTemp->GetDeviceCategory());
                pDevice->SetBayCategory(pDeviceTemp->GetBayCategory());
                pDevice->SetVLevel(pDeviceTemp->GetVLevel());
                pDevice->SetCircuitIndex(pDeviceTemp->GetCircuitIndex());
                pDevice->SetSet(pDeviceTemp->GetSet());
                pDevice->SetDescription(pDeviceTemp->GetDescription());
                pDevice->SetManufacture(pDeviceTemp->GetManufacture());
                pDevice->SetType(pDeviceTemp->GetType());
                pDevice->SetConfigVersion(pDeviceTemp->GetConfigVersion());
                pDevice->SetIcdFileName(pDeviceTemp->GetIcdFileName());

                if(bNotify)
                    emit sigObjectPropertyChanged(pDevice);
            }

            // Update parent cubicle
            PeCubicle *pParentCubicleNew = pDeviceTemp->GetParentCubicle();
            PeCubicle *pParentCubicleOld = pDevice->GetParentCubicle();
            if(pParentCubicleNew !=  pParentCubicleOld)
            {
                pDevice->SetParentCubicle(pParentCubicleNew);
                if(pParentCubicleOld)
                    pParentCubicleOld->RemoveChildDevice(pDevice);
                if(pParentCubicleNew)
                    pParentCubicleNew->AddChildDevice(pDevice);

                if(bNotify)
                    emit sigObjectParentChanged(pDevice, pParentCubicleOld);
            }

            // Update parent bay
            PeBay *pParentBayNew = pDeviceTemp->GetParentBay();
            PeBay *pParentBayOld = pDevice->GetParentBay();
            if(pParentBayNew != pParentBayOld)
            {
                pDevice->SetParentBay(pParentBayNew);
                if(pParentBayOld)
                    pParentBayOld->RemoveChildDevice(pDevice);
                if(pParentBayNew)
                    pParentBayNew->AddChildDevice(pDevice);

                // Update associated information sets if the parent bay changed
                foreach(PeInfoSet *pInfoSet, m_d->m_mapIdToInfoSets.values())
                {
                    if(pInfoSet->GetTxIED() == pDevice || pInfoSet->GetRxIED() == pDevice)
                    {
                        if(!pParentBayNew)
                        {
                            DeleteObject(pInfoSet, bNotify);
                        }
                        else
                        {
                            PeInfoSet InfoSet(*pInfoSet);
                            InfoSet.UpdateName();
                            if(InfoSet.GetName() != pInfoSet->GetName())
                                UpdateObject(InfoSet, bNotify);
                        }
                    }
                }

                if(bNotify)
                    emit sigDeviceBayChanged(pDevice, pParentBayOld);
            }

            return pDevice;
        }
        break;

        case otBoard:
        {
            PeBoard *pBoardTemp = qobject_cast<PeBoard*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeBoard *pBoard = FindBoardById(ProjectObject.GetId());
            if(!pBoardTemp || !pBoard)
                return 0;

            // Update property
            if(pBoard->GetPosition() != pBoardTemp->GetPosition() ||
               pBoard->GetDescription() != pBoardTemp->GetDescription() ||
               pBoard->GetType() != pBoardTemp->GetType())
            {
                pBoard->SetPosition(pBoardTemp->GetPosition());
                pBoard->SetDescription(pBoardTemp->GetDescription());
                pBoard->SetType(pBoardTemp->GetType());

                if(bNotify)
                    emit sigObjectPropertyChanged(pBoard);
            }

            // Update parent device
            PeDevice *pParentDeviceNew = pBoardTemp->GetParentDevice();
            PeDevice *pParentDeviceOld = pBoard->GetParentDevice();
            if(pParentDeviceNew != pParentDeviceOld)
            {
                pBoard->SetParentDevice(pParentDeviceNew);
                if(pParentDeviceOld)
                    pParentDeviceOld->RemoveChildBoard(pBoard);
                if(pParentDeviceNew)
                    pParentDeviceNew->AddChildBoard(pBoard);

                if(bNotify)
                    emit sigObjectParentChanged(pBoard, pParentDeviceOld);
            }

            return pBoard;
        }
        break;

        case otPort:
        {
            PePort *pPortTemp = qobject_cast<PePort*>(const_cast<PeProjectObject*>(&ProjectObject));
            PePort *pPort = FindPortById(ProjectObject.GetId());
            if(!pPortTemp || !pPort)
                return 0;

            // Update property
            if(pPort->GetName() != pPortTemp->GetName() ||
               pPort->GetGroup() != pPortTemp->GetGroup() ||
               pPort->GetPortType() != pPortTemp->GetPortType() ||
               pPort->GetPortDirection() != pPortTemp->GetPortDirection() ||
               pPort->GetFiberPlug() != pPortTemp->GetFiberPlug() ||
               pPort->GetFiberMode() != pPortTemp->GetFiberMode() ||
               pPort->GetCascade() != pPortTemp->GetCascade())
            {

                pPort->SetName(pPortTemp->GetName());
                pPort->SetGroup(pPortTemp->GetGroup());
                pPort->SetPortType(pPortTemp->GetPortType());
                pPort->SetPortDirection(pPortTemp->GetPortDirection());
                pPort->SetFiberPlug(pPortTemp->GetFiberPlug());
                pPort->SetFiberMode(pPortTemp->GetFiberMode());
                pPort->SetCascade(pPortTemp->GetCascade());

                if(bNotify)
                    emit sigObjectPropertyChanged(pPort);
            }

            // Update parent board
            PeBoard *pParentBoardNew = pPortTemp->GetParentBoard();
            PeBoard *pParentBoardOld = pPort->GetParentBoard();
            if(pParentBoardNew != pParentBoardOld)
            {
                pPort->SetParentBoard(pParentBoardNew);
                if(pParentBoardOld)
                    pParentBoardOld->RemoveChildPort(pPort);
                if(pParentBoardNew)
                    pParentBoardNew->AddChildPort(pPort);

                if(bNotify)
                    emit sigObjectParentChanged(pPort, pParentBoardOld);
            }

            return pPort;
        }
        break;

        case otInfoSet:
        {
            PeInfoSet *pInfoSetTemp = qobject_cast<PeInfoSet*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeInfoSet *pInfoSet = FindInfoSetById(ProjectObject.GetId());
            if(!pInfoSetTemp || !pInfoSet)
                return 0;

            if(pInfoSet->GetName() != pInfoSetTemp->GetName() ||
               pInfoSet->GetDescription() != pInfoSetTemp->GetDescription() ||
               pInfoSet->GetInfoSetType() != pInfoSetTemp->GetInfoSetType() ||
               pInfoSet->GetTxIED() != pInfoSetTemp->GetTxIED() ||
               pInfoSet->GetTxPort() != pInfoSetTemp->GetTxPort() ||
               pInfoSet->GetRxIED() != pInfoSetTemp->GetRxIED() ||
               pInfoSet->GetRxPort() != pInfoSetTemp->GetRxPort() ||
               pInfoSet->GetSwitch1() != pInfoSetTemp->GetSwitch1() ||
               pInfoSet->GetSwitch1TxPort() != pInfoSetTemp->GetSwitch1TxPort() ||
               pInfoSet->GetSwitch1RxPort() != pInfoSetTemp->GetSwitch1RxPort() ||
               pInfoSet->GetSwitch2() != pInfoSetTemp->GetSwitch2() ||
               pInfoSet->GetSwitch2TxPort() != pInfoSetTemp->GetSwitch2TxPort() ||
               pInfoSet->GetSwitch2RxPort() != pInfoSetTemp->GetSwitch2RxPort() ||
               pInfoSet->GetSwitch3() != pInfoSetTemp->GetSwitch3() ||
               pInfoSet->GetSwitch3TxPort() != pInfoSetTemp->GetSwitch3TxPort() ||
               pInfoSet->GetSwitch3RxPort() != pInfoSetTemp->GetSwitch3RxPort() ||
               pInfoSet->GetSwitch4() != pInfoSetTemp->GetSwitch4() ||
               pInfoSet->GetSwitch4TxPort() != pInfoSetTemp->GetSwitch4TxPort() ||
               pInfoSet->GetSwitch4RxPort() != pInfoSetTemp->GetSwitch4RxPort())
            {
                pInfoSet->SetName(pInfoSetTemp->GetName());
                pInfoSet->SetDescription(pInfoSetTemp->GetDescription());
                pInfoSet->SetInfoSetType(pInfoSetTemp->GetInfoSetType());
                pInfoSet->SetGroup(pInfoSetTemp->GetGroup());
                pInfoSet->SetTxIED(pInfoSetTemp->GetTxIED());
                pInfoSet->SetTxPort(pInfoSetTemp->GetTxPort());
                pInfoSet->SetRxIED(pInfoSetTemp->GetRxIED());
                pInfoSet->SetRxPort(pInfoSetTemp->GetRxPort());
                pInfoSet->SetSwitch1(pInfoSetTemp->GetSwitch1());
                pInfoSet->SetSwitch1TxPort(pInfoSetTemp->GetSwitch1TxPort());
                pInfoSet->SetSwitch1RxPort(pInfoSetTemp->GetSwitch1RxPort());
                pInfoSet->SetSwitch2(pInfoSetTemp->GetSwitch2());
                pInfoSet->SetSwitch2TxPort(pInfoSetTemp->GetSwitch2TxPort());
                pInfoSet->SetSwitch2RxPort(pInfoSetTemp->GetSwitch2RxPort());
                pInfoSet->SetSwitch3(pInfoSetTemp->GetSwitch3());
                pInfoSet->SetSwitch3TxPort(pInfoSetTemp->GetSwitch3TxPort());
                pInfoSet->SetSwitch3RxPort(pInfoSetTemp->GetSwitch3RxPort());
                pInfoSet->SetSwitch4(pInfoSetTemp->GetSwitch4());
                pInfoSet->SetSwitch4TxPort(pInfoSetTemp->GetSwitch4TxPort());
                pInfoSet->SetSwitch4RxPort(pInfoSetTemp->GetSwitch4RxPort());

                if(bNotify)
                    emit sigObjectPropertyChanged(pInfoSet);
            }

            return pInfoSet;
        }
        break;

        case otCubicleConn:
        {
            PeCubicleConn *pCubicleConnTemp = qobject_cast<PeCubicleConn*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeCubicleConn *pCubicleConn = FindCubicleConnById(ProjectObject.GetId());
            if(!pCubicleConnTemp || !pCubicleConn)
                return 0;

            if(pCubicleConn->GetCubicle1() != pCubicleConnTemp->GetCubicle1() ||
               pCubicleConn->GetUseOdf1() != pCubicleConnTemp->GetUseOdf1() ||
               pCubicleConn->GetPassCubicle1() != pCubicleConnTemp->GetPassCubicle1() ||
               pCubicleConn->GetCubicle2() != pCubicleConnTemp->GetCubicle2() ||
               pCubicleConn->GetUseOdf2() != pCubicleConnTemp->GetUseOdf2() ||
               pCubicleConn->GetPassCubicle2() != pCubicleConnTemp->GetPassCubicle2())
            {
                pCubicleConn->SetCubicle1(pCubicleConnTemp->GetCubicle1());
                pCubicleConn->SetUseOdf1(pCubicleConnTemp->GetUseOdf1());
                pCubicleConn->SetPassCubicle1(pCubicleConnTemp->GetPassCubicle1());
                pCubicleConn->SetCubicle2(pCubicleConnTemp->GetCubicle2());
                pCubicleConn->SetUseOdf2(pCubicleConnTemp->GetUseOdf2());
                pCubicleConn->SetPassCubicle2(pCubicleConnTemp->GetPassCubicle2());

                if(bNotify)
                    emit sigObjectPropertyChanged(pCubicleConn);
            }

            return pCubicleConn;
        }
        break;

        case otVTerminal:
        {
            PeVTerminal *pVTerminalTemp = qobject_cast<PeVTerminal*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeVTerminal *pVTerminal = FindVTerminalById(ProjectObject.GetId());
            if(!pVTerminalTemp || !pVTerminal || pVTerminal->GetParentDevice() != pVTerminalTemp->GetParentDevice())
                return 0;

            PeDevice *pParentDevice = pVTerminalTemp->GetParentDevice();
            if(!pParentDevice)
                return 0;

            if(pVTerminal->GetType() != pVTerminalTemp->GetType() ||
               pVTerminal->GetDirection() != pVTerminalTemp->GetDirection() ||
               pVTerminal->GetNumber() != pVTerminalTemp->GetNumber() ||
               pVTerminal->GetIEDDesc() != pVTerminalTemp->GetIEDDesc() ||
               pVTerminal->GetProDesc() != pVTerminalTemp->GetProDesc() ||
               pVTerminal->GetLDInst() != pVTerminalTemp->GetLDInst() ||
               pVTerminal->GetLNPrefix() != pVTerminalTemp->GetLNPrefix() ||
               pVTerminal->GetLNClass() != pVTerminalTemp->GetLNClass() ||
               pVTerminal->GetLNInst() != pVTerminalTemp->GetLNInst() ||
               pVTerminal->GetDOName() != pVTerminalTemp->GetDOName() ||
               pVTerminal->GetDAName() != pVTerminalTemp->GetDAName())
            {
                pVTerminal->SetType(pVTerminalTemp->GetType());
                pVTerminal->SetDirection(pVTerminalTemp->GetDirection());
                pVTerminal->SetNumber(pVTerminalTemp->GetNumber());
                pVTerminal->SetIEDDesc(pVTerminalTemp->GetIEDDesc());
                pVTerminal->SetProDesc(pVTerminalTemp->GetProDesc());
                pVTerminal->SetLDInst(pVTerminalTemp->GetLDInst());
                pVTerminal->SetLNPrefix(pVTerminalTemp->GetLNPrefix());
                pVTerminal->SetLNClass(pVTerminalTemp->GetLNClass());
                pVTerminal->SetLNInst(pVTerminalTemp->GetLNInst());
                pVTerminal->SetDOName(pVTerminalTemp->GetDOName());
                pVTerminal->SetDAName(pVTerminalTemp->GetDAName());

                if(bNotify)
                    emit sigObjectPropertyChanged(pVTerminal);
            }

            return pVTerminal;
        }
        break;

        case otStrap:
        {
            PeStrap *pStrapTemp = qobject_cast<PeStrap*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeStrap *pStrap = FindStrapById(ProjectObject.GetId());
            if(!pStrapTemp || !pStrap || pStrap->GetParentDevice() != pStrapTemp->GetParentDevice())
                return 0;

            PeDevice *pParentDevice = pStrapTemp->GetParentDevice();
            if(!pParentDevice)
                return 0;

            if(pStrapTemp->GetNumber() != pStrapTemp->GetNumber() ||
               pStrapTemp->GetDescription() != pStrapTemp->GetDescription() ||
               pStrapTemp->GetLDInst() != pStrapTemp->GetLDInst() ||
               pStrapTemp->GetLNPrefix() != pStrapTemp->GetLNPrefix() ||
               pStrapTemp->GetLNClass() != pStrapTemp->GetLNClass() ||
               pStrapTemp->GetLNInst() != pStrapTemp->GetLNInst() ||
               pStrapTemp->GetDOName() != pStrapTemp->GetDOName() ||
               pStrapTemp->GetDAName() != pStrapTemp->GetDAName())
            {
                pStrap->SetNumber(pStrapTemp->GetNumber());
                pStrap->SetDescription(pStrapTemp->GetDescription());
                pStrap->SetLDInst(pStrapTemp->GetLDInst());
                pStrap->SetLNPrefix(pStrapTemp->GetLNPrefix());
                pStrap->SetLNClass(pStrapTemp->GetLNClass());
                pStrap->SetLNInst(pStrapTemp->GetLNInst());
                pStrap->SetDOName(pStrapTemp->GetDOName());
                pStrap->SetDAName(pStrapTemp->GetDAName());

                if(bNotify)
                    emit sigObjectPropertyChanged(pStrap);
            }

            return pStrap;
        }
        break;

        case otVTerminalConn:
        {
            PeVTerminalConn *pVTerminalConnTemp = qobject_cast<PeVTerminalConn*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeVTerminalConn *pVTerminalConn = FindVTerminalConnById(ProjectObject.GetId());
            if(!pVTerminalConnTemp || !pVTerminalConn)
                return 0;

            if(pVTerminalConnTemp->GetTxVTerminal() != pVTerminalConn->GetTxVTerminal() ||
               pVTerminalConnTemp->GetRxVTerminal() != pVTerminalConn->GetRxVTerminal() ||
               pVTerminalConnTemp->GetTxStrap() != pVTerminalConn->GetTxStrap() ||
               pVTerminalConnTemp->GetRxStrap() != pVTerminalConn->GetRxStrap() ||
               pVTerminalConnTemp->GetStraight() != pVTerminalConn->GetStraight())
            {
                pVTerminalConn->SetTxVTerminal(pVTerminalConnTemp->GetTxVTerminal());
                pVTerminalConn->SetRxVTerminal(pVTerminalConnTemp->GetRxVTerminal());
                pVTerminalConn->SetTxStrap(pVTerminalConnTemp->GetTxStrap());
                pVTerminalConn->SetRxStrap(pVTerminalConnTemp->GetRxStrap());
                pVTerminalConn->SetStraight(pVTerminalConnTemp->GetStraight());

                if(bNotify)
                    emit sigObjectPropertyChanged(pVTerminalConn);
            }

            return pVTerminalConn;
        }
        break;

        case otCable:
        {
            PeCable *pCableTemp = qobject_cast<PeCable*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeCable *pCable = FindCableById(ProjectObject.GetId());
            if(!pCableTemp || !pCable)
                return 0;

            if(pCableTemp->GetCableType() != pCable->GetCableType() ||
               pCableTemp->GetName() != pCable->GetName() ||
               pCableTemp->GetNameBay() != pCable->GetNameBay() ||
               pCableTemp->GetNameNumber() != pCable->GetNameNumber() ||
               pCableTemp->GetNameSet() != pCable->GetNameSet() ||
               pCableTemp->GetCubicle1() != pCable->GetCubicle1() ||
               pCableTemp->GetCubicle2() != pCable->GetCubicle2())
            {
                pCable->SetCableType(pCableTemp->GetCableType());
                pCable->SetName(pCableTemp->GetName());
                pCable->SetNameBay(pCableTemp->GetNameBay());
                pCable->SetNameNumber(pCableTemp->GetNameNumber());
                pCable->SetNameSet(pCableTemp->GetNameSet());
                pCable->SetCubicle1(pCableTemp->GetCubicle1());
                pCable->SetCubicle2(pCableTemp->GetCubicle2());

                if(bNotify)
                    emit sigObjectPropertyChanged(pCable);
            }

            return pCable;
        }
        break;

        case otFiber:
        {
            PeFiber *pFiberTemp = qobject_cast<PeFiber*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeFiber *pFiber = FindFiberById(ProjectObject.GetId());
            if(!pFiberTemp || !pFiber)
                return 0;

            // Update property
            if(pFiberTemp->GetIndex() != pFiber->GetIndex() ||
               pFiberTemp->GetFiberColor() != pFiber->GetFiberColor() ||
               pFiberTemp->GetPipeColor() != pFiber->GetPipeColor() ||
               pFiberTemp->GetReserve() != pFiber->GetReserve() ||
               pFiberTemp->GetInfoSetIds() != pFiber->GetInfoSetIds() ||
               pFiberTemp->GetPort1() != pFiber->GetPort1() ||
               pFiberTemp->GetPort2() != pFiber->GetPort2())
            {
                pFiber->SetIndex(pFiberTemp->GetIndex());
                pFiber->SetFiberColor(pFiberTemp->GetFiberColor());
                pFiber->SetPipeColor(pFiberTemp->GetPipeColor());
                pFiber->SetReserve(pFiberTemp->GetReserve());
                pFiber->SetInfoSetIds(pFiberTemp->GetInfoSetIds());
                pFiber->SetPort1(pFiberTemp->GetPort1());
                pFiber->SetPort2(pFiberTemp->GetPort2());

                if(bNotify)
                    emit sigObjectPropertyChanged(pFiber);
            }

            // Update parent cable
            PeCable *pParentCableNew = pFiberTemp->GetParentCable();
            PeCable *pParentCableOld = pFiber->GetParentCable();
            if(pParentCableNew != pParentCableOld)
            {
                pFiber->SetParentCable(pParentCableNew);
                if(pParentCableOld)
                    pParentCableOld->RemoveChildFiber(pFiber);
                if(pParentCableNew)
                    pParentCableNew->AddChildFiber(pFiber);

                if(bNotify)
                    emit sigObjectParentChanged(pFiber, pParentCableOld);
            }

            return pFiber;
        }
        break;

        case otVolume:
        {
            PeVolume *pVolumeTemp = qobject_cast<PeVolume*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeVolume *pVolume = FindVolumeById(ProjectObject.GetId());
            if(!pVolumeTemp || !pVolume)
                return 0;

            if(pVolumeTemp->GetName() != pVolume->GetName() ||
               pVolumeTemp->GetDescription() != pVolume->GetDescription())
            {
                pVolume->SetName(pVolumeTemp->GetName());
                pVolume->SetDescription(pVolumeTemp->GetDescription());

                if(bNotify)
                    emit sigObjectPropertyChanged(pVolume);
            }

            return pVolume;
        }
        break;

        case otDwg:
        {
            PeDwg *pDwgTemp = qobject_cast<PeDwg*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeDwg *pDwg = FindDwgById(ProjectObject.GetId());
            if(!pDwgTemp || !pDwg)
                return 0;

            // Update property
            if(pDwgTemp->GetDwgType() != pDwg->GetDwgType() ||
               pDwgTemp->GetName() != pDwg->GetName() ||
               pDwgTemp->GetDescription() != pDwg->GetDescription())
            {
                pDwg->SetDwgType(pDwgTemp->GetDwgType());
                pDwg->SetName(pDwgTemp->GetName());
                pDwg->SetDescription(pDwgTemp->GetDescription());

                if(bNotify)
                    emit sigObjectPropertyChanged(pDwg);
            }

            // Update parent volume
            PeVolume *pParentVolumeNew = pDwgTemp->GetParentVolume();
            PeVolume *pParentVolumeOld = pDwg->GetParentVolume();
            if(pParentVolumeNew !=  pParentVolumeOld)
            {
                pDwg->SetParentVolume(pParentVolumeNew);
                if(pParentVolumeOld)
                    pParentVolumeOld->RemoveChildDwg(pDwg);
                if(pParentVolumeNew)
                    pParentVolumeNew->AddChildDwg(pDwg);

                if(bNotify)
                    emit sigObjectParentChanged(pDwg, pParentVolumeOld);
            }

            return pDwg;
        }
        break;
    }

    return 0;
}

bool PeProjectVersion::DeleteObject(PeProjectObject *pProjectObject, bool bNotify)
{
    if(!pProjectObject)
        return false;

    const PeProjectObject::ObjectType &eObjectType = pProjectObject->GetObjectType();
    switch(eObjectType)
    {
        case otBay:
        {
            PeBay *pBay = qobject_cast<PeBay*>(pProjectObject);
            if(!pBay)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete child devices
            /////////////////////////////////////////////////////////////////
            foreach(PeDevice *pDevice, pBay->GetChildDevices())
                DeleteObject(pDevice, bNotify);

            /////////////////////////////////////////////////////////////////
            // Delete current bay
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pBay);

            m_d->m_mapIdToBays.remove(pBay->GetId());
            delete pBay;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otRoom:
        {
            PeRoom *pRoom = qobject_cast<PeRoom*>(pProjectObject);
            if(!pRoom)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete child cubicles
            /////////////////////////////////////////////////////////////////
            foreach(PeCubicle *pCubicle, pRoom->GetChildCubicles())
                DeleteObject(pCubicle, bNotify);

            /////////////////////////////////////////////////////////////////
            // Delete current room
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pRoom);

            m_d->m_mapIdToRooms.remove(pRoom->GetId());
            delete pRoom;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otCubicle:
        {
            PeCubicle *pCubicle = qobject_cast<PeCubicle*>(pProjectObject);
            if(!pCubicle)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete all cubicle connection associated with current cubicle
            /////////////////////////////////////////////////////////////////
            foreach(PeCubicleConn *pCubicleConn, m_d->m_mapIdToCubicleConns.values())
            {
                if(pCubicleConn->GetCubicle1() == pCubicle ||
                   pCubicleConn->GetCubicle2() == pCubicle ||
                   pCubicleConn->GetPassCubicle1() == pCubicle ||
                   pCubicleConn->GetPassCubicle2() == pCubicle)
                {
                    DeleteObject(pCubicleConn, bNotify);
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete all cables associated with current cubicle
            /////////////////////////////////////////////////////////////////
            foreach(PeCable *pCable, m_d->m_mapIdToCables.values())
            {
                if(pCable->GetCubicle1() == pCubicle || pCable->GetCubicle2() == pCubicle)
                    DeleteObject(pCable, bNotify);
            }

            /////////////////////////////////////////////////////////////////
            // Delete child devices
            /////////////////////////////////////////////////////////////////
            foreach(PeDevice *pDevice, pCubicle->GetChildDevices())
                DeleteObject(pDevice, bNotify);

            /////////////////////////////////////////////////////////////////
            // Delete current cubicle
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pCubicle);

            // Remove current device from parent cubicle
            if(PeRoom *pRoom = pCubicle->GetParentRoom())
                pRoom->RemoveChildCubicle(pCubicle);

            // Delete current device
            m_d->m_mapIdToCubicles.remove(pCubicle->GetId());
            delete pCubicle;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otDevice:
        {
            PeDevice *pDevice = qobject_cast<PeDevice*>(pProjectObject);
            if(!pDevice)
                return false;

            /////////////////////////////////////////////////////////////////
            // Update all information set associated with current device
            /////////////////////////////////////////////////////////////////
            foreach(PeInfoSet *pInfoSet, m_d->m_mapIdToInfoSets.values())
            {
                if(pInfoSet->GetTxIED() == pDevice || pInfoSet->GetRxIED() == pDevice)
                {
                    DeleteObject(pInfoSet, bNotify);
                }
                else if(pInfoSet->GetSwitch1() == pDevice)
                {
                    PeInfoSet InfoSet(*pInfoSet);
                    InfoSet.SetSwitch1(0);
                    InfoSet.SetSwitch1TxPort(0);
                    InfoSet.SetSwitch1RxPort(0);
                    InfoSet.SetSwitch2(0);
                    InfoSet.SetSwitch2TxPort(0);
                    InfoSet.SetSwitch2RxPort(0);
                    InfoSet.SetSwitch3(0);
                    InfoSet.SetSwitch3TxPort(0);
                    InfoSet.SetSwitch3RxPort(0);
                    InfoSet.SetSwitch4(0);
                    InfoSet.SetSwitch4TxPort(0);
                    InfoSet.SetSwitch4RxPort(0);

                    UpdateObject(InfoSet, bNotify);
                }
                else if(pInfoSet->GetSwitch2() == pDevice)
                {
                    PeInfoSet InfoSet(*pInfoSet);
                    InfoSet.SetSwitch2(0);
                    InfoSet.SetSwitch2TxPort(0);
                    InfoSet.SetSwitch2RxPort(0);
                    InfoSet.SetSwitch3(0);
                    InfoSet.SetSwitch3TxPort(0);
                    InfoSet.SetSwitch3RxPort(0);
                    InfoSet.SetSwitch4(0);
                    InfoSet.SetSwitch4TxPort(0);
                    InfoSet.SetSwitch4RxPort(0);

                    UpdateObject(InfoSet, bNotify);
                }
                else if(pInfoSet->GetSwitch3() == pDevice)
                {
                    PeInfoSet InfoSet(*pInfoSet);
                    InfoSet.SetSwitch3(0);
                    InfoSet.SetSwitch3TxPort(0);
                    InfoSet.SetSwitch3RxPort(0);
                    InfoSet.SetSwitch4(0);
                    InfoSet.SetSwitch4TxPort(0);
                    InfoSet.SetSwitch4RxPort(0);

                    UpdateObject(InfoSet, bNotify);
                }
                else if(pInfoSet->GetSwitch4() == pDevice)
                {
                    PeInfoSet InfoSet(*pInfoSet);
                    InfoSet.SetSwitch4(0);
                    InfoSet.SetSwitch4TxPort(0);
                    InfoSet.SetSwitch4RxPort(0);

                    UpdateObject(InfoSet, bNotify);
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete child boards
            /////////////////////////////////////////////////////////////////
            foreach(PeBoard *pBoard, pDevice->GetChildBoards())
                DeleteObject(pBoard, bNotify);

            /////////////////////////////////////////////////////////////////
            // Cleare device model
            /////////////////////////////////////////////////////////////////
            ClearDeviceModel(pDevice, bNotify);

            /////////////////////////////////////////////////////////////////
            // Delete current device
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pDevice);

            // Remove current device from parent cubicle
            if(PeCubicle *pCubicle = pDevice->GetParentCubicle())
                pCubicle->RemoveChildDevice(pDevice);

            // Remove current device from parent bay
            if(PeBay *pBay = pDevice->GetParentBay())
                pBay->RemoveChildDevice(pDevice);

            // Delete current device
            m_d->m_mapIdToDevices.remove(pDevice->GetId());
            delete pDevice;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otBoard:
        {
            PeBoard *pBoard = qobject_cast<PeBoard*>(pProjectObject);
            if(!pBoard)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete child ports
            /////////////////////////////////////////////////////////////////
            foreach(PePort *pPort, pBoard->GetChildPorts())
                DeleteObject(pPort, bNotify);

            /////////////////////////////////////////////////////////////////
            // Delete current board
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pBoard);

            // Remove current board from parent device
            if(PeDevice *pDevice = pBoard->GetParentDevice())
                pDevice->RemoveChildBoard(pBoard);

            // Delete current board
            m_d->m_mapIdToBoards.remove(pBoard->GetId());
            delete pBoard;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otPort:
        {
            PePort *pPort = qobject_cast<PePort*>(pProjectObject);
            if(!pPort)
                return false;

            /////////////////////////////////////////////////////////////////
            // Update all information sets associated with current port
            /////////////////////////////////////////////////////////////////
            foreach(PeInfoSet *pInfoSet, m_d->m_mapIdToInfoSets.values())
            {
                PeInfoSet InfoSet(*pInfoSet);
                if(InfoSet.ReplacePort(pPort, 0))
                    UpdateObject(InfoSet, bNotify);
            }

            /////////////////////////////////////////////////////////////////
            // Delete all fibers associated with current port
            /////////////////////////////////////////////////////////////////
            foreach(PeFiber *pFiber, m_d->m_mapIdToFibers.values())
            {
                if(pFiber->GetPort1() == pPort || pFiber->GetPort2() == pPort)
                    DeleteObject(pFiber, bNotify);
            }

            /////////////////////////////////////////////////////////////////
            // Delete current port
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pPort);

            // Remove current port from parent board
            if(PeBoard *pBoard = pPort->GetParentBoard())
                pBoard->RemoveChildPort(pPort);

            // Delete current port
            m_d->m_mapIdToPorts.remove(pPort->GetId());
            delete pPort;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otInfoSet:
        {
            PeInfoSet *pInfoSet = qobject_cast<PeInfoSet*>(pProjectObject);
            if(!pInfoSet)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete current information set
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pInfoSet);

            // Delete current infoset
            m_d->m_mapIdToInfoSets.remove(pInfoSet->GetId());
            delete pInfoSet;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otCubicleConn:
        {
            PeCubicleConn *pCubicleConn = qobject_cast<PeCubicleConn*>(pProjectObject);
            if(!pCubicleConn)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete current cubicle connection
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pCubicleConn);

            // Delete current cubicle connection
            m_d->m_mapIdToCubicleConns.remove(pCubicleConn->GetId());
            delete pCubicleConn;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otVTerminal:
        {
            PeVTerminal *pVTerminal = qobject_cast<PeVTerminal*>(pProjectObject);
            if(!pVTerminal)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete all vterminal connections associated with current vterminal
            /////////////////////////////////////////////////////////////////
            foreach(PeVTerminalConn *pVTerminalConn, m_d->m_mapIdToVTerminalConns.values())
            {
                if(pVTerminalConn->GetTxVTerminal() == pVTerminal || pVTerminalConn->GetRxVTerminal() == pVTerminal)
                    DeleteObject(pVTerminalConn, bNotify);
            }

            /////////////////////////////////////////////////////////////////
            // Delete current vterminal
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pVTerminal);

            // Remove current vterminal from parent device
            if(PeDevice *pDevice = pVTerminal->GetParentDevice())
                pDevice->RemoveVTerminal(pVTerminal);

            // Delete current vterminal
            m_d->m_mapIdToVTerminals.remove(pVTerminal->GetId());
            delete pVTerminal;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otStrap:
        {
            PeStrap *pStrap = qobject_cast<PeStrap*>(pProjectObject);
            if(!pStrap)
                return false;

            /////////////////////////////////////////////////////////////////
            // Update all vterminal connections associated with current strap
            /////////////////////////////////////////////////////////////////
            foreach(PeVTerminalConn *pVTerminalConn, m_d->m_mapIdToVTerminalConns.values())
            {
                PeVTerminalConn VTerminalConn(*pVTerminalConn);
                if(pVTerminalConn->GetTxStrap() == pStrap)
                    VTerminalConn.SetTxStrap(0);
                if(pVTerminalConn->GetRxStrap() == pStrap)
                    VTerminalConn.SetRxStrap(0);

                UpdateObject(VTerminalConn, bNotify);
            }

            /////////////////////////////////////////////////////////////////
            // Delete current strap
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pStrap);

            // Remove current strap from parent device
            if(PeDevice *pDevice = pStrap->GetParentDevice())
                pDevice->RemoveStrap(pStrap);

            // Delete current strap
            m_d->m_mapIdToStraps.remove(pStrap->GetId());
            delete pStrap;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otVTerminalConn:
        {
            PeVTerminalConn *pVTerminalConn = qobject_cast<PeVTerminalConn*>(pProjectObject);
            if(!pVTerminalConn)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete current vterminal connection
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pVTerminalConn);

            // Delete current vconnection
            m_d->m_mapIdToVTerminalConns.remove(pVTerminalConn->GetId());
            delete pVTerminalConn;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otCable:
        {
            PeCable *pCable = qobject_cast<PeCable*>(pProjectObject);
            if(!pCable)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete child fibers
            /////////////////////////////////////////////////////////////////
            foreach(PeFiber *pFiber, pCable->GetChildFibers())
                DeleteObject(pFiber, bNotify);

            /////////////////////////////////////////////////////////////////
            // Delete current cable
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pCable);

            // Delete current cable
            m_d->m_mapIdToCables.remove(pCable->GetId());
            delete pCable;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otFiber:
        {
            PeFiber *pFiber = qobject_cast<PeFiber*>(pProjectObject);
            if(!pFiber)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete current fiber
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pFiber);

            // Remove current fiber from parent cable
            if(PeCable *pCable = pFiber->GetParentCable())
                pCable->RemoveChildFiber(pFiber);

            // Delete current fiber
            m_d->m_mapIdToFibers.remove(pFiber->GetId());
            delete pFiber;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otVolume:
        {
            PeVolume *pVolume = qobject_cast<PeVolume*>(pProjectObject);
            if(!pVolume)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete child dwgs
            /////////////////////////////////////////////////////////////////
            foreach(PeDwg *pDwg, pVolume->GetChildDwgs())
                DeleteObject(pDwg, bNotify);

            /////////////////////////////////////////////////////////////////
            // Delete current volume
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pVolume);

            // Delete current cable
            m_d->m_mapIdToVolumes.remove(pVolume->GetId());
            delete pVolume;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case otDwg:
        {
            PeDwg *pDwg = qobject_cast<PeDwg*>(pProjectObject);
            if(!pDwg)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete current dwg
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pDwg);

            // Remove current dwg from parent volume
            if(PeVolume *pVolume = pDwg->GetParentVolume())
                pVolume->RemoveChildDwg(pDwg);

            // Delete current dwg
            m_d->m_mapIdToDwgs.remove(pDwg->GetId());
            delete pDwg;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        default:
            return false;
    }

    return true;
}

PeProjectObject* PeProjectVersion::CloneObject(const PeProjectObject &ProjectObject, bool bNotify)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    MySqlWrapper::MySqlQuery query(pDatabase);

    PeProjectObject *pProjectObjectCloned = 0;
    const PeProjectObject::ObjectType &eObjectType = ProjectObject.GetObjectType();
    switch(eObjectType)
    {
        case otBay:
        {
        }
        break;

        case otDevice:
        {
            PeDevice *pDeviceTemp = qobject_cast<PeDevice*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeDevice *pDevice = qobject_cast<PeDevice*>(CreateObject(ProjectObject, bNotify));
            if(!pDevice)
                return 0;

            query.Clear();
            if(!query.Exec(QString("SELECT * FROM board WHERE `device_id`='%1'").arg(pDevice->GetId())))
                return 0;

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                PeBoard Board(this, record.value("board_id").toInt());
                Board.SetPosition(record.value("position").toString());
                Board.SetDescription(record.value("description").toString());
                Board.SetType(record.value("type").toString());
                Board.SetParentDevice(pDevice);

                if(!CloneObject(Board, bNotify))
                    return 0;
            }

            UpdateDeviceModelStatus(pDevice, pDeviceTemp->GetHasModel());

            query.Clear();
            if(!query.Exec(QString("SELECT * FROM vterminal WHERE `device_id`='%1'").arg(pDevice->GetId())))
                return 0;

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                PeVTerminal VTerminal(this, record.value("vterminal_id").toInt());
                VTerminal.SetType(PeVTerminal::TerminalType(record.value("type").toInt()));
                VTerminal.SetDirection(PeVTerminal::TerminalDirection(record.value("direction").toInt()));
                VTerminal.SetNumber(record.value("vterminal_no").toInt());
                VTerminal.SetIEDDesc(record.value("ied_desc").toString());
                VTerminal.SetProDesc(record.value("pro_desc").toString());
                VTerminal.SetLDInst(record.value("ld_inst").toString());
                VTerminal.SetLNPrefix(record.value("ln_prefix").toString());
                VTerminal.SetLNClass(record.value("ln_class").toString());
                VTerminal.SetLNInst(record.value("ln_inst").toString());
                VTerminal.SetDOName(record.value("do_name").toString());
                VTerminal.SetDAName(record.value("da_name").toString());
                VTerminal.SetParentDevice(pDevice);

                if(!CloneObject(VTerminal, bNotify))
                    return 0;
            }

            query.Clear();
            if(!query.Exec(QString("SELECT * FROM strap WHERE `device_id`='%1'").arg(pDevice->GetId())))
                return 0;

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                PeStrap Strap(this, record.value("strap_id").toInt());
                Strap.SetNumber(record.value("strap_no").toInt());
                Strap.SetDescription(record.value("description").toString());
                Strap.SetLDInst(record.value("ld_inst").toString());
                Strap.SetLNPrefix(record.value("ln_prefix").toString());
                Strap.SetLNClass(record.value("ln_class").toString());
                Strap.SetLNInst(record.value("ln_inst").toString());
                Strap.SetDOName(record.value("do_name").toString());
                Strap.SetDAName(record.value("da_name").toString());
                Strap.SetParentDevice(pDevice);

                if(!CloneObject(Strap, bNotify))
                    return 0;
            }

            pProjectObjectCloned = pDevice;
        }
        break;

        case otBoard:
        {
            PeBoard *pBoard = qobject_cast<PeBoard*>(CreateObject(ProjectObject, bNotify));
            if(!pBoard)
                return 0;

            query.Clear();
            if(!query.Exec(QString("SELECT * FROM port WHERE `board_id`='%1'").arg(pBoard->GetId())))
                return 0;

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                PePort Port(this, record.value("port_id").toInt());
                Port.SetName(record.value("name").toString());
                Port.SetGroup(record.value("group").toInt());
                Port.SetPortType(PePort::PortType(record.value("type").toInt()));
                Port.SetPortDirection(PePort::PortDirection(record.value("direction").toInt()));
                Port.SetFiberPlug(PePort::FiberPlug(record.value("fiber_plug").toInt()));
                Port.SetFiberMode(PePort::FiberMode(record.value("fiber_mode").toInt()));
                Port.SetCascade(record.value("cascade").toBool());
                Port.SetParentBoard(pBoard);

                if(!CloneObject(Port, bNotify))
                    return 0;
            }

            pProjectObjectCloned = pBoard;
        }
        break;

        case otPort:
        case otVTerminal:
        case otStrap:
        {
            pProjectObjectCloned = CreateObject(ProjectObject, bNotify);
        }
        break;
    }

    return pProjectObjectCloned;
}

bool PeProjectVersion::ClearDeviceModel(PeDevice *pDevice, bool bNotify)
{
    if(!pDevice)
        return false;

    foreach(PeVTerminal *pVTerminal, pDevice->GetVTerminals())
        DeleteObject(pVTerminal, bNotify);

    foreach(PeStrap *pStrap, pDevice->GetStraps())
        DeleteObject(pStrap, bNotify);

    ProjectExplorer::PeDevice device(*pDevice);
    device.SetConfigVersion("");
    device.SetIcdFileName("");
    UpdateObject(device, bNotify);

    UpdateDeviceModelStatus(pDevice, false, bNotify);
    return true;
}

void PeProjectVersion::UpdateDeviceModelStatus(PeDevice *pDevice, bool bHasModel, bool bNotify)
{
    if(!pDevice)
        return;

    pDevice->SetHasModel(bHasModel);
    if(bNotify)
        emit sigDeviceModelChanged(pDevice);
}

QList<PeBay*> PeProjectVersion::GetAllBays() const
{
    return m_d->m_mapIdToBays.values();
}

QList<PeRoom*> PeProjectVersion::GetAllRooms() const
{
    return m_d->m_mapIdToRooms.values();
}

QList<PeCubicle*> PeProjectVersion::GetAllCubicles() const
{
    return m_d->m_mapIdToCubicles.values();
}

QList<PeDevice*> PeProjectVersion::GetAllDevices() const
{
    return m_d->m_mapIdToDevices.values();
}

QList<PeBoard*> PeProjectVersion::GetAllBoards() const
{
    return m_d->m_mapIdToBoards.values();
}

QList<PePort*> PeProjectVersion::GetAllPorts() const
{
    return m_d->m_mapIdToPorts.values();
}

QList<PeInfoSet*> PeProjectVersion::GetAllInfoSets() const
{
    return m_d->m_mapIdToInfoSets.values();
}

QList<PeCubicleConn*> PeProjectVersion::GetAllCubicleConns() const
{
    return m_d->m_mapIdToCubicleConns.values();
}

QList<PeVTerminal*> PeProjectVersion::GetAllVTerminals() const
{
    return m_d->m_mapIdToVTerminals.values();
}

QList<PeStrap*> PeProjectVersion::GetAllStraps() const
{
    return m_d->m_mapIdToStraps.values();
}

QList<PeVTerminalConn*> PeProjectVersion::GetAllVTerminalConns() const
{
    return m_d->m_mapIdToVTerminalConns.values();
}

QList<PeCable*> PeProjectVersion::GetAllCables() const
{
    return m_d->m_mapIdToCables.values();
}

QList<PeFiber*> PeProjectVersion::GetAllFibers() const
{
    return m_d->m_mapIdToFibers.values();
}

QList<PeVolume*> PeProjectVersion::GetAllVolumes() const
{
    return m_d->m_mapIdToVolumes.values();
}

QList<PeDwg*> PeProjectVersion::GetAllDwgs() const
{
    return m_d->m_mapIdToDwgs.values();
}

PeBay* PeProjectVersion::FindBayById(int iId)
{
    return m_d->m_mapIdToBays.value(iId, 0);
}

PeRoom* PeProjectVersion::FindRoomById(int iId)
{
    return m_d->m_mapIdToRooms.value(iId, 0);
}

PeCubicle* PeProjectVersion::FindCubicleById(int iId)
{
    return m_d->m_mapIdToCubicles.value(iId, 0);
}

PeDevice* PeProjectVersion::FindDeviceById(int iId)
{
    return m_d->m_mapIdToDevices.value(iId, 0);
}

PeBoard* PeProjectVersion::FindBoardById(int iId)
{
    return m_d->m_mapIdToBoards.value(iId, 0);
}

PePort* PeProjectVersion::FindPortById(int iId)
{
    return m_d->m_mapIdToPorts.value(iId, 0);
}

PeInfoSet* PeProjectVersion::FindInfoSetById(int iId)
{
    return m_d->m_mapIdToInfoSets.value(iId, 0);
}

PeCubicleConn* PeProjectVersion::FindCubicleConnById(int iId)
{
    return m_d->m_mapIdToCubicleConns.value(iId, 0);
}

PeVTerminal* PeProjectVersion::FindVTerminalById(int iId)
{
    return m_d->m_mapIdToVTerminals.value(iId, 0);
}

PeStrap* PeProjectVersion::FindStrapById(int iId)
{
    return m_d->m_mapIdToStraps.value(iId, 0);
}

PeVTerminalConn* PeProjectVersion::FindVTerminalConnById(int iId)
{
    return m_d->m_mapIdToVTerminalConns.value(iId, 0);
}

PeCable* PeProjectVersion::FindCableById(int iId)
{
    return m_d->m_mapIdToCables.value(iId, 0);
}

PeFiber* PeProjectVersion::FindFiberById(int iId)
{
    return m_d->m_mapIdToFibers.value(iId, 0);
}

PeVolume* PeProjectVersion::FindVolumeById(int iId)
{
    return m_d->m_mapIdToVolumes.value(iId, 0);
}

PeDwg* PeProjectVersion::FindDwgById(int iId)
{
    return m_d->m_mapIdToDwgs.value(iId, 0);
}

#include <QDebug>
bool PeProjectVersion::DbCreateObject(PeProjectObject &ProjectObject, bool bUseTransaction)
{
    QString strProcedure;
    const PeProjectObject::ObjectType &eObjectType = ProjectObject.GetObjectType();
    switch(eObjectType)
    {
        case otBay:
        {
            PeBay *pBay = qobject_cast<PeBay*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pBay)
                return false;

            strProcedure = QString("INSERT INTO bay(`name`,`number`,`vlevel`) "
                                   "VALUES('%1','%2','%3')").arg(pBay->GetName())
                                                            .arg(pBay->GetNumber())
                                                            .arg(pBay->GetVLevel());
        }
        break;

        case otRoom:
        {
            PeRoom *pRoom = qobject_cast<PeRoom*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pRoom)
                return false;

            strProcedure = QString("INSERT INTO room(`name`,`number`,`yard`) "
                                   "VALUES('%1','%2','%3')").arg(pRoom->GetName())
                                                            .arg(pRoom->GetNumber())
                                                            .arg(pRoom->GetYard());
        }
        break;

        case otCubicle:
        {
            PeCubicle *pCubicle = qobject_cast<PeCubicle*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pCubicle)
                return false;

            strProcedure = QString("INSERT INTO cubicle(`room_id`,`name`,`number`,`manufacture`) "
                                   "VALUES('%1','%2','%3','%4')").arg(pCubicle->GetParentRoom() ? pCubicle->GetParentRoom()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                 .arg(pCubicle->GetName())
                                                                 .arg(pCubicle->GetNumber())
                                                                 .arg(pCubicle->GetManufacture());
        }
        break;

        case otDevice:
        {
            PeDevice *pDevice = qobject_cast<PeDevice*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pDevice)
                return false;

            strProcedure = QString("INSERT INTO device(`cubicle_id`,`cubicle_pos`,`cubicle_num`,`bay_id`,`device_type`,`name`,`device_category`,`bay_category`,`vlevel`,`circuit_index`,`set`,`description`,`manufacture`,`type`,`config_version`,`icd_filename`) "
                                   "VALUES('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13','%14','%15','%16')").arg(pDevice->GetParentCubicle() ? pDevice->GetParentCubicle()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                                                                                    .arg(pDevice->GetCubiclePos())
                                                                                                                                    .arg(pDevice->GetCubicleNum())
                                                                                                                                    .arg(pDevice->GetParentBay() ? pDevice->GetParentBay()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                                                                                    .arg(pDevice->GetDeviceType())
                                                                                                                                    .arg(pDevice->GetName())
                                                                                                                                    .arg(pDevice->GetDeviceCategory())
                                                                                                                                    .arg(pDevice->GetBayCategory())
                                                                                                                                    .arg(pDevice->GetVLevel())
                                                                                                                                    .arg(pDevice->GetCircuitIndex())
                                                                                                                                    .arg(pDevice->GetSet())
                                                                                                                                    .arg(pDevice->GetDescription())
                                                                                                                                    .arg(pDevice->GetManufacture())
                                                                                                                                    .arg(pDevice->GetType())
                                                                                                                                    .arg(pDevice->GetConfigVersion())
                                                                                                                                    .arg(pDevice->GetIcdFileName());
        }
        break;

        case otBoard:
        {
            PeBoard *pBoard = qobject_cast<PeBoard*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pBoard)
                return false;

            PeDevice *pParentDevice = pBoard->GetParentDevice();
            if(!pParentDevice)
                return false;

            strProcedure = QString("INSERT INTO board(`device_id`,`position`,`description`,`type`) "
                                   "VALUES('%1','%2','%3','%4')").arg(pParentDevice->GetId())
                                                                 .arg(pBoard->GetPosition())
                                                                 .arg(pBoard->GetDescription())
                                                                 .arg(pBoard->GetType());
        }
        break;

        case otPort:
        {
            PePort *pPort = qobject_cast<PePort*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pPort)
                return false;

            PeBoard *pParentBoard = pPort->GetParentBoard();
            if(!pParentBoard)
                return false;

            strProcedure = QString("INSERT INTO port(`board_id`,`name`,`group`,`type`,`direction`,`fiber_plug`,`fiber_mode`,`cascade`) "
                                   "VALUES('%1','%2','%3','%4','%5','%6','%7','%8')").arg(pParentBoard->GetId())
                                                                                     .arg(pPort->GetName())
                                                                                     .arg(pPort->GetGroup())
                                                                                     .arg(int(pPort->GetPortType()))
                                                                                     .arg(int(pPort->GetPortDirection()))
                                                                                     .arg(int(pPort->GetFiberPlug()))
                                                                                     .arg(int(pPort->GetFiberMode()))
                                                                                     .arg(int(pPort->GetCascade()));
        }
        break;

        case otInfoSet:
        {
            PeInfoSet *pInfoSet = qobject_cast<PeInfoSet*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pInfoSet)
                return false;

            strProcedure = QString("INSERT INTO infoset(`name`,`description`,`type`,`group`,`txied_id`,`txiedport_id`,`rxied_id`,`rxiedport_id`,`switch1_id`,`switch1_txport_id`,`switch1_rxport_id`,`switch2_id`,`switch2_txport_id`,`switch2_rxport_id`,`switch3_id`,`switch3_txport_id`,`switch3_rxport_id`,`switch4_id`,`switch4_txport_id`,`switch4_rxport_id`) "
                                   "VALUES('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13','%14','%15','%16','%17','%18','%19','%20')").arg(pInfoSet->GetName())
                                                                                                                                                            .arg(pInfoSet->GetDescription())
                                                                                                                                                            .arg(pInfoSet->GetInfoSetType())
                                                                                                                                                            .arg(pInfoSet->GetGroup())
                                                                                                                                                            .arg(pInfoSet->GetTxIED() ? pInfoSet->GetTxIED()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetTxPort() ? pInfoSet->GetTxPort()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetRxIED() ? pInfoSet->GetRxIED()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetRxPort() ? pInfoSet->GetRxPort()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetSwitch1() ? pInfoSet->GetSwitch1()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetSwitch1TxPort() ? pInfoSet->GetSwitch1TxPort()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetSwitch1RxPort() ? pInfoSet->GetSwitch1RxPort()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetSwitch2() ? pInfoSet->GetSwitch2()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetSwitch2TxPort() ? pInfoSet->GetSwitch2TxPort()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetSwitch2RxPort() ? pInfoSet->GetSwitch2RxPort()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetSwitch3() ? pInfoSet->GetSwitch3()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetSwitch3TxPort() ? pInfoSet->GetSwitch3TxPort()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetSwitch3RxPort() ? pInfoSet->GetSwitch3RxPort()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetSwitch4() ? pInfoSet->GetSwitch4()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetSwitch4TxPort() ? pInfoSet->GetSwitch4TxPort()->GetId() : 0)
                                                                                                                                                            .arg(pInfoSet->GetSwitch4RxPort() ? pInfoSet->GetSwitch4RxPort()->GetId() : 0);
        }
        break;

        case otCubicleConn:
        {
            PeCubicleConn *pCubicleConn = qobject_cast<PeCubicleConn*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pCubicleConn)
                return false;

            strProcedure = QString("INSERT INTO cubicle_connection(`cubicle1_id`,`use_odf1`,`passcubicle1_id`,`cubicle2_id`,`use_odf2`,`passcubicle2_id`) "
                                   "VALUES('%1','%2','%3','%4','%5','%6')").arg(pCubicleConn->GetCubicle1() ? pCubicleConn->GetCubicle1()->GetId() : 0)
                                                                           .arg(pCubicleConn->GetUseOdf1())
                                                                           .arg(pCubicleConn->GetPassCubicle1() ? pCubicleConn->GetPassCubicle1()->GetId() : 0)
                                                                           .arg(pCubicleConn->GetCubicle2() ? pCubicleConn->GetCubicle2()->GetId() : 0)
                                                                           .arg(pCubicleConn->GetUseOdf2())
                                                                           .arg(pCubicleConn->GetPassCubicle2() ? pCubicleConn->GetPassCubicle2()->GetId() : 0);
        }
        break;

        case otVTerminal:
        {
            PeVTerminal *pVTerminal = qobject_cast<PeVTerminal*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pVTerminal)
                return false;

            PeDevice *pParentDevice = pVTerminal->GetParentDevice();
            if(!pParentDevice)
                return false;

            strProcedure = QString("INSERT INTO vterminal(`device_id`,`type`,`direction`,`vterminal_no`,`ied_desc`,`pro_desc`,`ld_inst`,`ln_prefix`,`ln_class`,`ln_inst`,`do_name`,`da_name`) "
                                   "VALUES('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12')").arg(pParentDevice->GetId())
                                                                                                            .arg(pVTerminal->GetType())
                                                                                                            .arg(pVTerminal->GetDirection())
                                                                                                            .arg(pVTerminal->GetNumber())
                                                                                                            .arg(pVTerminal->GetIEDDesc())
                                                                                                            .arg(pVTerminal->GetProDesc())
                                                                                                            .arg(pVTerminal->GetLDInst())
                                                                                                            .arg(pVTerminal->GetLNPrefix())
                                                                                                            .arg(pVTerminal->GetLNClass())
                                                                                                            .arg(pVTerminal->GetLNInst())
                                                                                                            .arg(pVTerminal->GetDOName())
                                                                                                            .arg(pVTerminal->GetDAName());
        }
        break;

        case otStrap:
        {
            PeStrap *pStrap = qobject_cast<PeStrap*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pStrap)
                return false;

            PeDevice *pParentDevice = pStrap->GetParentDevice();
            if(!pParentDevice)
                return false;

            strProcedure = QString("INSERT INTO strap(`device_id`,`strap_no`,`description`,`ld_inst`,`ln_prefix`,`ln_class`,`ln_inst`,`do_name`,`da_name`) "
                                   "VALUES('%1','%2','%3','%4','%5','%6','%7','%8','%9')").arg(pParentDevice->GetId())
                                                                                          .arg(pStrap->GetNumber())
                                                                                          .arg(pStrap->GetDescription())
                                                                                          .arg(pStrap->GetLDInst())
                                                                                          .arg(pStrap->GetLNPrefix())
                                                                                          .arg(pStrap->GetLNClass())
                                                                                          .arg(pStrap->GetLNInst())
                                                                                          .arg(pStrap->GetDOName())
                                                                                          .arg(pStrap->GetDAName());
        }
        break;

        case otVTerminalConn:
        {
            PeVTerminalConn *pVTerminalConn = qobject_cast<PeVTerminalConn*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pVTerminalConn)
                return false;

            strProcedure = QString("INSERT INTO vterminal_connection(`txvterminal_id`,`txstrap_id`,`rxvterminal_id`,`rxstrap_id`,`straight`) "
                                   "VALUES('%1','%2','%3','%4','%5')").arg(pVTerminalConn->GetTxVTerminal()->GetId())
                                                                      .arg(pVTerminalConn->GetTxStrap() ? pVTerminalConn->GetTxStrap()->GetId() : 0)
                                                                      .arg(pVTerminalConn->GetRxVTerminal()->GetId())
                                                                      .arg(pVTerminalConn->GetRxStrap() ? pVTerminalConn->GetRxStrap()->GetId() : 0)
                                                                      .arg(pVTerminalConn->GetStraight());
        }
        break;

        case otCable:
        {
            PeCable *pCable = qobject_cast<PeCable*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pCable)
                return false;

            strProcedure = QString("INSERT INTO cable(`cubicle1_id`,`cubicle2_id`,`cable_type`,`name`,`name_bay`,`name_number`,`name_set`) "
                                   "VALUES('%1','%2','%3','%4','%5','%6','%7')").arg(pCable->GetCubicle1() ? pCable->GetCubicle1()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                                .arg(pCable->GetCubicle2() ? pCable->GetCubicle2()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                                .arg(pCable->GetCableType())
                                                                                .arg(pCable->GetName())
                                                                                .arg(pCable->GetNameBay())
                                                                                .arg(pCable->GetNameNumber())
                                                                                .arg(pCable->GetNameSet());
        }
        break;

        case otFiber:
        {
            PeFiber *pFiber = qobject_cast<PeFiber*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pFiber)
                return false;

            PeCable *pParentCable = pFiber->GetParentCable();
            if(!pParentCable)
                return false;

            strProcedure = QString("INSERT INTO fiber(`cable_id`,`port1_id`,`port2_id`,`index`,`fiber_color`,`pipe_color`,`reserve`,`infoset_ids`) "
                                   "VALUES('%1','%2','%3','%4','%5','%6','%7','%8')").arg(pParentCable->GetId())
                                                                                     .arg(pFiber->GetPort1() ? pFiber->GetPort1()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                                     .arg(pFiber->GetPort2() ? pFiber->GetPort2()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                                     .arg(pFiber->GetIndex())
                                                                                     .arg(pFiber->GetFiberColor())
                                                                                     .arg(pFiber->GetPipeColor())
                                                                                     .arg(pFiber->GetReserve())
                                                                                     .arg(pFiber->GetInfoSetIds().join(QLatin1String(",")));
        }
        break;

        case otVolume:
        {
            PeVolume *pVolume = qobject_cast<PeVolume*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pVolume)
                return false;

            strProcedure = QString("INSERT INTO volume(`name`,`description`) "
                                   "VALUES('%1','%2')").arg(pVolume->GetName())
                                                       .arg(pVolume->GetDescription());
        }
        break;

        case otDwg:
        {
            PeDwg *pDwg = qobject_cast<PeDwg*>(const_cast<PeProjectObject*>(&ProjectObject));
            if(!pDwg)
                return false;

            PeVolume *pParentVolume = pDwg->GetParentVolume();
            if(!pParentVolume)
                return false;

            strProcedure = QString("INSERT INTO dwg(`volume_id`,`type`,`name`,`description`) "
                                   "VALUES('%1','%2','%3','%4')").arg(pParentVolume->GetId())
                                                                 .arg(pDwg->GetDwgType())
                                                                 .arg(pDwg->GetName())
                                                                 .arg(pDwg->GetDescription());
        }
        break;
    }

    if(strProcedure.isEmpty())
        return false;

    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    ProjectExplorer::DbTransaction DbTrans(this, bUseTransaction);

    if(!pDatabase->ExecSql(strProcedure))
    {
        DbTrans.Rollback();
        return false;
    }

    int iReturnId = pDatabase->GetInsertId();
    if(iReturnId <= PeProjectObject::m_iInvalidObjectId)
    {
        DbTrans.Rollback();
        return false;
    }

    if(!DbTrans.Commit())
        return false;

    ProjectObject.SetId(iReturnId);
    return true;
}

bool PeProjectVersion::DbUpdateObject(const PeProjectObject &ProjectObject, bool bUseTransaction)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    ProjectExplorer::DbTransaction DbTrans(this, bUseTransaction);
    MySqlWrapper::MySqlQuery query(pDatabase);

    QString strProcedure = "";
    const PeProjectObject::ObjectType &eObjectType = ProjectObject.GetObjectType();
    switch(eObjectType)
    {
        case otBay:
        {
            PeBay *pBayTemp = qobject_cast<PeBay*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeBay *pBay = m_d->m_mapIdToBays.value(ProjectObject.GetId());
            if(!pBayTemp || !pBay)
                return false;

            /////////////////////////////////////////////////////////////////
            // Update associated information sets and cables if the bay number changed
            /////////////////////////////////////////////////////////////////
            const QString strBayNumberNew = pBayTemp->GetNumber();
            const QString strBayNumberOld = pBay->GetNumber();
            if(strBayNumberNew != strBayNumberOld)
            {
                // Update information sets
                query.Clear();
                if(!query.Exec(QString("SELECT `infoset_id` FROM `infoset` WHERE `txied_id` IN (SELECT `device_id` FROM `device` WHERE `bay_id`='%1') OR `rxied_id` IN (SELECT `device_id` FROM `device` WHERE `bay_id`='%1')").arg(ProjectObject.GetId())))
                {
                    DbTrans.Rollback();
                    return false;
                }

                foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
                {
                    PeInfoSet *pInfoSet = m_d->m_mapIdToInfoSets.value(record.value("infoset_id").toInt());
                    if(!pInfoSet)
                    {
                        DbTrans.Rollback();
                        return false;
                    }

                    PeInfoSet InfoSet(*pInfoSet);
                    PeDevice DeviceTx(*InfoSet.GetTxIED());
                    PeDevice DeviceRx(*InfoSet.GetRxIED());
                    if(DeviceTx.GetParentBay() == pBay)
                    {
                        DeviceTx.SetParentBay(pBayTemp);
                        InfoSet.SetTxIED(&DeviceTx);
                    }
                    if(DeviceRx.GetParentBay() == pBay)
                    {
                        DeviceRx.SetParentBay(pBayTemp);
                        InfoSet.SetRxIED(&DeviceRx);
                    }

                    InfoSet.UpdateName();
                    if(InfoSet.GetName() != pInfoSet->GetName())
                    {
                        if(!DbUpdateObject(InfoSet, false))
                        {
                            DbTrans.Rollback();
                            return false;
                        }
                    }

                    // Update cables
                    query.Clear();
                    if(!query.Exec(QString("UPDATE cable SET `name_bay`='%1' WHERE `name_bay`='%2'").arg(strBayNumberNew).arg(strBayNumberOld)))
                    {
                        DbTrans.Rollback();
                        return false;
                    }
                }
            }

            strProcedure = QString("UPDATE bay SET `name`='%1',`number`='%2',`vlevel`='%3' WHERE `bay_id`='%4'")
                                                                .arg(pBayTemp->GetName())
                                                                .arg(pBayTemp->GetNumber())
                                                                .arg(pBayTemp->GetVLevel())
                                                                .arg(pBayTemp->GetId());
        }
        break;

        case otRoom:
        {
            PeRoom *pRoom = qobject_cast<PeRoom*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE room SET `name`='%1',`number`='%2',`yard`='%3' WHERE `room_id`='%4'")
                                                                .arg(pRoom->GetName())
                                                                .arg(pRoom->GetNumber())
                                                                .arg(pRoom->GetYard())
                                                                .arg(pRoom->GetId());
        }
        break;

        case otCubicle:
        {
            PeCubicle *pCubicle = qobject_cast<PeCubicle*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE cubicle SET `room_id`='%1',`name`='%2',`number`='%3',`manufacture`='%4' WHERE `cubicle_id`='%5'")
                                                                .arg(pCubicle->GetParentRoom() ? pCubicle->GetParentRoom()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                .arg(pCubicle->GetName())
                                                                .arg(pCubicle->GetNumber())
                                                                .arg(pCubicle->GetManufacture())
                                                                .arg(pCubicle->GetId());
        }
        break;

        case otDevice:
        {
            PeDevice *pDeviceTemp = qobject_cast<PeDevice*>(const_cast<PeProjectObject*>(&ProjectObject));
            PeDevice *pDevice = m_d->m_mapIdToDevices.value(ProjectObject.GetId());
            if(!pDeviceTemp || !pDevice)
                return false;

            /////////////////////////////////////////////////////////////////
            // Update associated information sets if the parent bay changed
            /////////////////////////////////////////////////////////////////
            PeBay *pParentBayNew = pDeviceTemp->GetParentBay();
            PeBay *pParentBayOld = pDevice->GetParentBay();
            if(pParentBayNew != pParentBayOld)
            {
                query.Clear();
                if(!query.Exec(QString("SELECT `infoset_id` FROM `infoset` WHERE `txied_id`='%1' OR `rxied_id`='%1'").arg(ProjectObject.GetId())))
                {
                    DbTrans.Rollback();
                    return false;
                }

                foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
                {
                    PeInfoSet *pInfoSet = m_d->m_mapIdToInfoSets.value(record.value("infoset_id").toInt());
                    if(!pInfoSet)
                    {
                        DbTrans.Rollback();
                        return false;
                    }

                    if(!pParentBayNew)
                    {
                        if(!pDatabase->ExecSql(QString("DELETE FROM `infoset` WHERE `infoset_id`='%1'").arg(pInfoSet->GetId())))
                        {
                            DbTrans.Rollback();
                            return false;
                        }
                    }
                    else
                    {
                        PeInfoSet InfoSet(*pInfoSet);
                        if(InfoSet.GetTxIED() == pDevice)
                            InfoSet.SetTxIED(pDeviceTemp);
                        if(InfoSet.GetRxIED() == pDevice)
                            InfoSet.SetRxIED(pDeviceTemp);
                        InfoSet.UpdateName();
                        if(InfoSet.GetName() != pInfoSet->GetName())
                        {
                            if(!DbUpdateObject(InfoSet, false))
                            {
                                DbTrans.Rollback();
                                return false;
                            }
                        }
                    }
                }
            }

            /////////////////////////////////////////////////////////////////
            // Update associated communication if the 'name' changed
            /////////////////////////////////////////////////////////////////
            if(pDeviceTemp->GetName() != pDevice->GetName())
            {
                QByteArray baCommunicationContent;
                if(!DbReadSclCommunication(baCommunicationContent))
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
                            if(pSCLElementConnectedAP->GetAttribute("iedName") == pDevice->GetName())
                            {
                                pSCLElementConnectedAP->SetAttribute("iedName", pDeviceTemp->GetName(), false);
                                bNeedUpdateCommunication = true;
                            }
                        }
                    }

                    if(bNeedUpdateCommunication)
                    {
                        if(!DbUpdateSclCommunication(SCLElementCommunication.WriteToBinary(), false))
                        {
                            DbTrans.Rollback();
                            return false;
                        }
                    }
                }
            }

            /////////////////////////////////////////////////////////////////
            // Update current device
            /////////////////////////////////////////////////////////////////
            strProcedure = QString("UPDATE device SET `cubicle_id`='%1',`cubicle_pos`='%2',`cubicle_num`='%3',`bay_id`='%4',`device_type`='%5',`name`='%6',`device_category`='%7',`bay_category`='%8',`vlevel`='%9',`circuit_index`='%10',`set`='%11',`description`='%12',`manufacture`='%13',`type`='%14',`config_version`='%15',`icd_filename`='%16' WHERE `device_id`='%17'")
                                                                .arg(pDeviceTemp->GetParentCubicle() ? pDeviceTemp->GetParentCubicle()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                .arg(pDeviceTemp->GetCubiclePos())
                                                                .arg(pDeviceTemp->GetCubicleNum())
                                                                .arg(pDeviceTemp->GetParentBay() ? pDeviceTemp->GetParentBay()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                .arg(pDeviceTemp->GetDeviceType())
                                                                .arg(pDeviceTemp->GetName())
                                                                .arg(pDeviceTemp->GetDeviceCategory())
                                                                .arg(pDeviceTemp->GetBayCategory())
                                                                .arg(pDeviceTemp->GetVLevel())
                                                                .arg(pDeviceTemp->GetCircuitIndex())
                                                                .arg(pDeviceTemp->GetSet())
                                                                .arg(pDeviceTemp->GetDescription())
                                                                .arg(pDeviceTemp->GetManufacture())
                                                                .arg(pDeviceTemp->GetType())
                                                                .arg(pDeviceTemp->GetConfigVersion())
                                                                .arg(pDeviceTemp->GetIcdFileName())
                                                                .arg(pDeviceTemp->GetId());
        }
        break;

        case otBoard:
        {
            PeBoard *pBoard = qobject_cast<PeBoard*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE board SET `device_id`='%1',`position`='%2',`description`='%3',`type`='%4' WHERE `board_id`='%5'")
                                                                .arg(pBoard->GetParentDevice()->GetId())
                                                                .arg(pBoard->GetPosition())
                                                                .arg(pBoard->GetDescription())
                                                                .arg(pBoard->GetType())
                                                                .arg(pBoard->GetId());
        }
        break;

        case otPort:
        {
            PePort *pPort = qobject_cast<PePort*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE port SET `board_id`='%1',`name`='%2',`group`='%3',`type`='%4',`direction`='%5',`fiber_plug`='%6',`fiber_mode`='%7',`cascade`='%8' WHERE `port_id`='%9'")
                                                                .arg(pPort->GetParentBoard()->GetId())
                                                                .arg(pPort->GetName())
                                                                .arg(pPort->GetGroup())
                                                                .arg(int(pPort->GetPortType()))
                                                                .arg(int(pPort->GetPortDirection()))
                                                                .arg(int(pPort->GetFiberPlug()))
                                                                .arg(int(pPort->GetFiberMode()))
                                                                .arg(int(pPort->GetCascade()))
                                                                .arg(pPort->GetId());
        }
        break;

        case otInfoSet:
        {
            PeInfoSet *pInfoSet = qobject_cast<PeInfoSet*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE infoset SET `name`='%1',`description`='%2',`type`='%3',`group`='%4',`txied_id`='%5',`txiedport_id`='%6',`rxied_id`='%7',`rxiedport_id`='%8',`switch1_id`='%9',`switch1_txport_id`='%10',`switch1_rxport_id`='%11',`switch2_id`='%12',`switch2_txport_id`='%13',`switch2_rxport_id`='%14',`switch3_id`='%15',`switch3_txport_id`='%16',`switch3_rxport_id`='%17',`switch4_id`='%18',`switch4_txport_id`='%19',`switch4_rxport_id`='%20' WHERE `infoset_id`='%21'")
                                                                .arg(pInfoSet->GetName())
                                                                .arg(pInfoSet->GetDescription())
                                                                .arg(pInfoSet->GetInfoSetType())
                                                                .arg(pInfoSet->GetGroup())
                                                                .arg(pInfoSet->GetTxIED() ? pInfoSet->GetTxIED()->GetId() : 0)
                                                                .arg(pInfoSet->GetTxPort() ? pInfoSet->GetTxPort()->GetId() : 0)
                                                                .arg(pInfoSet->GetRxIED() ? pInfoSet->GetRxIED()->GetId() : 0)
                                                                .arg(pInfoSet->GetRxPort() ? pInfoSet->GetRxPort()->GetId() : 0)
                                                                .arg(pInfoSet->GetSwitch1() ? pInfoSet->GetSwitch1()->GetId() : 0)
                                                                .arg(pInfoSet->GetSwitch1TxPort() ? pInfoSet->GetSwitch1TxPort()->GetId() : 0)
                                                                .arg(pInfoSet->GetSwitch1RxPort() ? pInfoSet->GetSwitch1RxPort()->GetId() : 0)
                                                                .arg(pInfoSet->GetSwitch2() ? pInfoSet->GetSwitch2()->GetId() : 0)
                                                                .arg(pInfoSet->GetSwitch2TxPort() ? pInfoSet->GetSwitch2TxPort()->GetId() : 0)
                                                                .arg(pInfoSet->GetSwitch2RxPort() ? pInfoSet->GetSwitch2RxPort()->GetId() : 0)
                                                                .arg(pInfoSet->GetSwitch3() ? pInfoSet->GetSwitch3()->GetId() : 0)
                                                                .arg(pInfoSet->GetSwitch3TxPort() ? pInfoSet->GetSwitch3TxPort()->GetId() : 0)
                                                                .arg(pInfoSet->GetSwitch3RxPort() ? pInfoSet->GetSwitch3RxPort()->GetId() : 0)
                                                                .arg(pInfoSet->GetSwitch4() ? pInfoSet->GetSwitch4()->GetId() : 0)
                                                                .arg(pInfoSet->GetSwitch4TxPort() ? pInfoSet->GetSwitch4TxPort()->GetId() : 0)
                                                                .arg(pInfoSet->GetSwitch4RxPort() ? pInfoSet->GetSwitch4RxPort()->GetId() : 0)
                                                                .arg(pInfoSet->GetId());
        }
        break;

        case otCubicleConn:
        {
            PeCubicleConn *pCubicleConn = qobject_cast<PeCubicleConn*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE cubicle_connection SET `cubicle1_id`='%1',`use_odf1`='%2',`passcubicle1_id`='%3',`cubicle2_id`='%4',`use_odf2`='%5',`passcubicle2_id`='%6' WHERE `connection_id`='%7'")
                                                                .arg(pCubicleConn->GetCubicle1() ? pCubicleConn->GetCubicle1()->GetId() : 0)
                                                                .arg(pCubicleConn->GetUseOdf1())
                                                                .arg(pCubicleConn->GetPassCubicle1() ? pCubicleConn->GetPassCubicle1()->GetId() : 0)
                                                                .arg(pCubicleConn->GetCubicle2() ? pCubicleConn->GetCubicle2()->GetId() : 0)
                                                                .arg(pCubicleConn->GetUseOdf2())
                                                                .arg(pCubicleConn->GetPassCubicle2() ? pCubicleConn->GetPassCubicle2()->GetId() : 0)
                                                                .arg(pCubicleConn->GetId());
        }
        break;

        case otVTerminal:
        {
            PeVTerminal *pVTerminal = qobject_cast<PeVTerminal*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE vterminal SET `device_id`='%1',`type`='%2',`direction`='%3',`vterminal_no`='%4',`ied_desc`='%5',`pro_desc`='%6',`ld_inst`='%7',`ln_prefix`='%8',`ln_class`='%9',`ln_inst`='%10',`do_name`='%11',`da_name`='%12' WHERE `vterminal_id`='%13'")
                                                                .arg(pVTerminal->GetParentDevice()->GetId())
                                                                .arg(pVTerminal->GetType())
                                                                .arg(pVTerminal->GetDirection())
                                                                .arg(pVTerminal->GetNumber())
                                                                .arg(pVTerminal->GetIEDDesc())
                                                                .arg(pVTerminal->GetProDesc())
                                                                .arg(pVTerminal->GetLDInst())
                                                                .arg(pVTerminal->GetLNPrefix())
                                                                .arg(pVTerminal->GetLNClass())
                                                                .arg(pVTerminal->GetLNInst())
                                                                .arg(pVTerminal->GetDOName())
                                                                .arg(pVTerminal->GetDAName())
                                                                .arg(pVTerminal->GetId());
        }
        break;

        case otStrap:
        {
            PeStrap *pStrap = qobject_cast<PeStrap*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE strap SET `device_id`='%1',`strap_no`='%2',`description`='%3',`ld_inst`='%4',`ln_prefix`='%5',`ln_class`='%6',`ln_inst`='%7',`do_name`='%8',`da_name`='%9' WHERE `strap_id`='%10'")
                                                                .arg(pStrap->GetParentDevice()->GetId())
                                                                .arg(pStrap->GetNumber())
                                                                .arg(pStrap->GetDescription())
                                                                .arg(pStrap->GetLDInst())
                                                                .arg(pStrap->GetLNPrefix())
                                                                .arg(pStrap->GetLNClass())
                                                                .arg(pStrap->GetLNInst())
                                                                .arg(pStrap->GetDOName())
                                                                .arg(pStrap->GetDAName())
                                                                .arg(pStrap->GetId());
        }
        break;

        case otVTerminalConn:
        {
            PeVTerminalConn *pVTerminalConn = qobject_cast<PeVTerminalConn*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE vterminal_connection SET `txvterminal_id`='%1',`txstrap_id`='%2',`rxvterminal_id`='%3',`rxstrap_id`='%4',`straight`='%5' WHERE `connection_id`='%6'")
                                                                .arg(pVTerminalConn->GetTxVTerminal()->GetId())
                                                                .arg(pVTerminalConn->GetTxStrap() ? pVTerminalConn->GetTxStrap()->GetId() : 0)
                                                                .arg(pVTerminalConn->GetRxVTerminal()->GetId())
                                                                .arg(pVTerminalConn->GetRxStrap() ? pVTerminalConn->GetRxStrap()->GetId() : 0)
                                                                .arg(pVTerminalConn->GetStraight())
                                                                .arg(pVTerminalConn->GetId());
        }
        break;

        case otCable:
        {
            PeCable *pCable = qobject_cast<PeCable*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE cable SET `cubicle1_id`='%1',`cubicle2_id`='%2',`cable_type`='%3',`name`='%4',`name_bay`='%5',`name_number`='%6',`name_set`='%7' WHERE `cable_id`='%8'")
                                                                .arg(pCable->GetCubicle1() ? pCable->GetCubicle1()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                .arg(pCable->GetCubicle2() ? pCable->GetCubicle2()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                .arg(pCable->GetCableType())
                                                                .arg(pCable->GetName())
                                                                .arg(pCable->GetNameBay())
                                                                .arg(pCable->GetNameNumber())
                                                                .arg(pCable->GetNameSet())
                                                                .arg(pCable->GetId());
        }
        break;

        case otFiber:
        {
            PeFiber *pFiber = qobject_cast<PeFiber*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE fiber SET `cable_id`='%1',`port1_id`='%2',`port2_id`='%3',`index`='%4',`fiber_color`='%5',`pipe_color`='%6',`reserve`='%7',`infoset_ids`='%8' WHERE `fiber_id`='%9'")
                                                                .arg(pFiber->GetParentCable()->GetId())
                                                                .arg(pFiber->GetPort1() ? pFiber->GetPort1()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                .arg(pFiber->GetPort2() ? pFiber->GetPort2()->GetId() : PeProjectObject::m_iInvalidObjectId)
                                                                .arg(pFiber->GetIndex())
                                                                .arg(pFiber->GetFiberColor())
                                                                .arg(pFiber->GetPipeColor())
                                                                .arg(pFiber->GetReserve())
                                                                .arg(pFiber->GetInfoSetIds().join(QLatin1String(",")))
                                                                .arg(pFiber->GetId());
        }
        break;

        case otVolume:
        {
            PeVolume *pVolume = qobject_cast<PeVolume*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE volume SET `name`='%1',`description`='%2' WHERE `volume_id`='%3'")
                                                                .arg(pVolume->GetName())
                                                                .arg(pVolume->GetDescription())
                                                                .arg(pVolume->GetId());
        }
        break;

        case otDwg:
        {
            PeDwg *pDwg = qobject_cast<PeDwg*>(const_cast<PeProjectObject*>(&ProjectObject));
            strProcedure = QString("UPDATE dwg SET `volume_id`='%1',`type`='%2',`name`='%3',`description`='%4' WHERE `dwg_id`='%5'")
                                                                .arg(pDwg->GetParentVolume()->GetId())
                                                                .arg(pDwg->GetDwgType())
                                                                .arg(pDwg->GetName())
                                                                .arg(pDwg->GetDescription())
                                                                .arg(pDwg->GetId());
        }
        break;

        default:
            return false;
    }

    if(strProcedure.isEmpty())
    {
        DbTrans.Rollback();
        return false;
    }

    if(!pDatabase->ExecSql(strProcedure))
    {
        DbTrans.Rollback();
        return false;
    }

    return DbTrans.Commit();
}

bool PeProjectVersion::DbDeleteObject(const PeProjectObject::ObjectType &eObjectType, int iId, bool bUseTransaction)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    ProjectExplorer::DbTransaction DbTrans(this, bUseTransaction);
    MySqlWrapper::MySqlQuery query(pDatabase);

    switch(eObjectType)
    {
        case otBay:
        {
            /////////////////////////////////////////////////////////////////
            // Delete child devices
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `device_id` FROM `device` WHERE `bay_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PeProjectObject::otDevice, record.value("device_id").toInt(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current bay
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `bay` WHERE `bay_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otRoom:
        {
            /////////////////////////////////////////////////////////////////
            // Delete child cubicles
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `cubicle_id` FROM `cubicle` WHERE `room_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PeProjectObject::otCubicle, record.value("cubicle_id").toInt(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current room
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `room` WHERE `room_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otCubicle:
        {
            /////////////////////////////////////////////////////////////////
            // Delete all cubicle connections associated with current cubicle
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `connection_id` FROM `cubicle_connection` WHERE `cubicle1_id`='%1' OR `cubicle2_id`='%1' OR `passcubicle1_id`='%1' OR `passcubicle2_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PeProjectObject::otCubicleConn, record.value("connection_id").toInt(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete all cables associated with current cubicle
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `cable_id` FROM `cable` WHERE `cubicle1_id`='%1' OR `cubicle2_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PeProjectObject::otCable, record.value("cable_id").toInt(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete child devices
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `device_id` FROM `device` WHERE `cubicle_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PeProjectObject::otDevice, record.value("device_id").toInt(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current cubicle
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `cubicle` WHERE `cubicle_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otDevice:
        {
            /////////////////////////////////////////////////////////////////
            // Update all information sets associated with current device
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `infoset_id` FROM `infoset` WHERE `txied_id`='%1' OR `rxied_id`='%1' OR `switch1_id`='%1' OR `switch2_id`='%1' OR `switch3_id`='%1' OR `switch4_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                PeInfoSet *pInfoSet = m_d->m_mapIdToInfoSets.value(record.value("infoset_id").toInt());
                if(!pInfoSet)
                {
                    DbTrans.Rollback();
                    return false;
                }

                int iTxIED = pInfoSet->GetTxIED() ? pInfoSet->GetTxIED()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iRxIED = pInfoSet->GetRxIED() ? pInfoSet->GetRxIED()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iSwitch1 = pInfoSet->GetSwitch1() ? pInfoSet->GetSwitch1()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iSwitch2 = pInfoSet->GetSwitch2() ? pInfoSet->GetSwitch2()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iSwitch3 = pInfoSet->GetSwitch3() ? pInfoSet->GetSwitch3()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iSwitch4 = pInfoSet->GetSwitch4() ? pInfoSet->GetSwitch4()->GetId() : PeProjectObject::m_iInvalidObjectId;

                if(iTxIED == iId || iRxIED == iId)
                {
                    if(!DbDeleteObject(PeProjectObject::otInfoSet, pInfoSet->GetId(), false))
                    {
                        DbTrans.Rollback();
                        return false;
                    }
                }
                else
                {
                    PeInfoSet InfoSet(*pInfoSet);
                    if(iSwitch1 == iId)
                    {
                        InfoSet.SetSwitch1(0);
                        InfoSet.SetSwitch1TxPort(0);
                        InfoSet.SetSwitch1RxPort(0);
                        InfoSet.SetSwitch2(0);
                        InfoSet.SetSwitch2TxPort(0);
                        InfoSet.SetSwitch2RxPort(0);
                        InfoSet.SetSwitch3(0);
                        InfoSet.SetSwitch3TxPort(0);
                        InfoSet.SetSwitch3RxPort(0);
                        InfoSet.SetSwitch4(0);
                        InfoSet.SetSwitch4TxPort(0);
                        InfoSet.SetSwitch4RxPort(0);
                    }
                    else if(iSwitch2 == iId)
                    {
                        InfoSet.SetSwitch2(0);
                        InfoSet.SetSwitch2TxPort(0);
                        InfoSet.SetSwitch2RxPort(0);
                        InfoSet.SetSwitch3(0);
                        InfoSet.SetSwitch3TxPort(0);
                        InfoSet.SetSwitch3RxPort(0);
                        InfoSet.SetSwitch4(0);
                        InfoSet.SetSwitch4TxPort(0);
                        InfoSet.SetSwitch4RxPort(0);
                    }
                    else if(iSwitch3 == iId)
                    {
                        InfoSet.SetSwitch3(0);
                        InfoSet.SetSwitch3TxPort(0);
                        InfoSet.SetSwitch3RxPort(0);
                        InfoSet.SetSwitch4(0);
                        InfoSet.SetSwitch4TxPort(0);
                        InfoSet.SetSwitch4RxPort(0);
                    }
                    else if(iSwitch4 == iId)
                    {
                        InfoSet.SetSwitch4(0);
                        InfoSet.SetSwitch4TxPort(0);
                        InfoSet.SetSwitch4RxPort(0);
                    }

                    if(!DbUpdateObject(InfoSet, false))
                    {
                        DbTrans.Rollback();
                        return false;
                    }
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete child boards
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `board_id` FROM `board` WHERE `device_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PeProjectObject::otBoard, record.value("board_id").toInt(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Cleare device model
            /////////////////////////////////////////////////////////////////
            if(!DbClearDeviceModel(iId, false))
            {
                DbTrans.Rollback();
                return false;
            }

            /////////////////////////////////////////////////////////////////
            // Delete current device
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `device` WHERE `device_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otBoard:
        {
            /////////////////////////////////////////////////////////////////
            // Delete child ports
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `port_id` FROM `port` WHERE `board_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PeProjectObject::otPort, record.value("port_id").toInt(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current board
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `board` WHERE `board_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otPort:
        {
            /////////////////////////////////////////////////////////////////
            // Update all information sets associated with current port
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `infoset_id` FROM `infoset` WHERE `txiedport_id`='%1' OR `rxiedport_id`='%1' OR `switch1_txport_id`='%1' OR `switch1_rxport_id`='%1' OR `switch2_txport_id`='%1' OR `switch2_rxport_id`='%1' OR `switch3_txport_id`='%1' OR `switch3_rxport_id`='%1' OR `switch4_txport_id`='%1' OR `switch4_rxport_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                PeInfoSet *pInfoSet = m_d->m_mapIdToInfoSets.value(record.value("infoset_id").toInt());
                if(!pInfoSet)
                {
                    DbTrans.Rollback();
                    return false;
                }

                PeInfoSet InfoSet(*pInfoSet);
                int iTxPort = pInfoSet->GetTxPort() ? pInfoSet->GetTxPort()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iRxPort = pInfoSet->GetRxPort() ? pInfoSet->GetRxPort()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iSwitch1TxPort = pInfoSet->GetSwitch1TxPort() ? pInfoSet->GetSwitch1TxPort()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iSwitch1RxPort = pInfoSet->GetSwitch1RxPort() ? pInfoSet->GetSwitch1RxPort()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iSwitch2TxPort = pInfoSet->GetSwitch2TxPort() ? pInfoSet->GetSwitch2TxPort()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iSwitch2RxPort = pInfoSet->GetSwitch2RxPort() ? pInfoSet->GetSwitch2RxPort()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iSwitch3TxPort = pInfoSet->GetSwitch3TxPort() ? pInfoSet->GetSwitch3TxPort()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iSwitch3RxPort = pInfoSet->GetSwitch3RxPort() ? pInfoSet->GetSwitch3RxPort()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iSwitch4TxPort = pInfoSet->GetSwitch4TxPort() ? pInfoSet->GetSwitch4TxPort()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iSwitch4RxPort = pInfoSet->GetSwitch4RxPort() ? pInfoSet->GetSwitch4RxPort()->GetId() : PeProjectObject::m_iInvalidObjectId;
                if(iTxPort == iId)
                    InfoSet.SetTxPort(0);
                if(iRxPort == iId)
                    InfoSet.SetRxPort(0);
                if(iSwitch1TxPort == iId)
                    InfoSet.SetSwitch1TxPort(0);
                if(iSwitch1RxPort == iId)
                    InfoSet.SetSwitch1RxPort(0);
                if(iSwitch2TxPort == iId)
                    InfoSet.SetSwitch2TxPort(0);
                if(iSwitch2RxPort == iId)
                    InfoSet.SetSwitch2RxPort(0);
                if(iSwitch3TxPort == iId)
                    InfoSet.SetSwitch3TxPort(0);
                if(iSwitch3RxPort == iId)
                    InfoSet.SetSwitch3RxPort(0);
                if(iSwitch4TxPort == iId)
                    InfoSet.SetSwitch4TxPort(0);
                if(iSwitch4RxPort == iId)
                    InfoSet.SetSwitch4RxPort(0);

                if(!DbUpdateObject(InfoSet, false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete all fibers associated with current port
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `fiber_id` FROM `fiber` WHERE `port1_id`='%1' OR `port2_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PeProjectObject::otFiber, record.value("fiber_id").toInt(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current port
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM port WHERE `port_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otInfoSet:
        {
            /////////////////////////////////////////////////////////////////
            // Delete current information set
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `infoset` WHERE `infoset_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otCubicleConn:
        {
            /////////////////////////////////////////////////////////////////
            // Delete current cubicle connection
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `cubicle_connection` WHERE `connection_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otVTerminal:
        {
            /////////////////////////////////////////////////////////////////
            // Delete all vterminal connections associated with current vterminal
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `connection_id` FROM `vterminal_connection` WHERE `txvterminal_id`='%1' OR `rxvterminal_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PeProjectObject::otVTerminalConn, record.value("connection_id").toInt(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current vterminal
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `vterminal` WHERE `vterminal_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otStrap:
        {
            /////////////////////////////////////////////////////////////////
            // Update all vterminal connections associated with current strap
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `connection_id` FROM `vterminal_connection` WHERE `txstrap_id`='%1' OR `rxstrap_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                PeVTerminalConn *pVTerminalConn = m_d->m_mapIdToVTerminalConns.value(record.value("connection_id").toInt());
                if(!pVTerminalConn)
                {
                    DbTrans.Rollback();
                    return false;
                }

                PeVTerminalConn VTerminalConn(*pVTerminalConn);
                int iTxStrap = pVTerminalConn->GetTxStrap() ? pVTerminalConn->GetTxStrap()->GetId() : PeProjectObject::m_iInvalidObjectId;
                int iRxStrap = pVTerminalConn->GetRxStrap() ? pVTerminalConn->GetRxStrap()->GetId() : PeProjectObject::m_iInvalidObjectId;
                if(iTxStrap == iId)
                    VTerminalConn.SetTxStrap(0);
                if(iRxStrap == iId)
                    VTerminalConn.SetRxStrap(0);

                if(!DbUpdateObject(VTerminalConn, false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current strap
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `strap` WHERE `strap_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otVTerminalConn:
        {
            /////////////////////////////////////////////////////////////////
            // Delete current vterminal_connection
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `vterminal_connection` WHERE `connection_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otCable:
        {
            /////////////////////////////////////////////////////////////////
            // Delete child fibers
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `fiber_id` FROM `fiber` WHERE `cable_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PeProjectObject::otFiber, record.value("fiber_id").toInt(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current cable
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `cable` WHERE `cable_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otFiber:
        {
            /////////////////////////////////////////////////////////////////
            // Delete current fiber
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `fiber` WHERE `fiber_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otVolume:
        {
            /////////////////////////////////////////////////////////////////
            // Delete child dwgs
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `dwg_id` FROM `dwg` WHERE `volume_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PeProjectObject::otDwg, record.value("dwg_id").toInt(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current volume
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `volume` WHERE `volume_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        case otDwg:
        {
            /////////////////////////////////////////////////////////////////
            // Delete current dwg
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `dwg` WHERE `dwg_id`='%1'").arg(iId)))
            {
                DbTrans.Rollback();
                return false;
            }
        }
        break;

        default:
            return false;
    }

    return DbTrans.Commit();
}

bool PeProjectVersion::DbReadDeviceModel(int iDeviceId, QByteArray &baModelContent)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    return pDatabase->ReadBlog(QString("SELECT `icd_content` FROM `device` WHERE `device_id`='%1'").arg(iDeviceId), baModelContent);
}

bool PeProjectVersion::DbUpdateDeviceModel(int iDeviceId, const QByteArray &baModelContent, bool bUseTransaction)
{
    if(iDeviceId <= PeProjectObject::m_iInvalidObjectId)
        return false;

    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    ProjectExplorer::DbTransaction DbTrans(this, bUseTransaction);

    if(!baModelContent.isEmpty() && !baModelContent.isNull())
    {
        QString strSql = QString("UPDATE `device` SET `icd_content` = ? WHERE `device_id`='%1'").arg(iDeviceId);
        if(!pDatabase->WriteBlog(strSql, baModelContent))
        {
            DbTrans.Rollback();
            return false;
        }
    }
    else
    {
        if(!pDatabase->ExecSql(QString("UPDATE `device` set `icd_content` = NULL WHERE `device_id`='%1'").arg(iDeviceId)))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    return DbTrans.Commit();
}

bool PeProjectVersion::DbClearDeviceModel(int iDeviceId, bool bUseTransaction)
{
    if(iDeviceId <= PeProjectObject::m_iInvalidObjectId)
        return false;

    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    ProjectExplorer::DbTransaction DbTrans(this, bUseTransaction);
    MySqlWrapper::MySqlQuery query(pDatabase);

    query.Clear();
    if(!query.Exec(QString("SELECT `vterminal_id` FROM `vterminal` WHERE `device_id`='%1'").arg(iDeviceId)))
    {
        DbTrans.Rollback();
        return false;
    }

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        if(!DbDeleteObject(PeProjectObject::otVTerminal, record.value("vterminal_id").toInt(), false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    query.Clear();
    if(!query.Exec(QString("SELECT `strap_id` FROM `strap` WHERE `device_id`='%1'").arg(iDeviceId)))
    {
        DbTrans.Rollback();
        return false;
    }

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        if(!DbDeleteObject(PeProjectObject::otStrap, record.value("strap_id").toInt(), false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    if(PeDevice *pDevice = FindDeviceById(iDeviceId))
    {
        QByteArray baCommunicationContent;
        if(!DbReadSclCommunication(baCommunicationContent))
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
                    if(pSCLElementConnectedAP->GetAttribute("iedName") == pDevice->GetName())
                    {
                        pSCLElementSubNetwork->RemoveChild(pSCLElementConnectedAP, false);
                        bNeedUpdateCommunication = true;
                    }
                }
            }

            if(bNeedUpdateCommunication)
            {
                if(!DbUpdateSclCommunication(SCLElementCommunication.WriteToBinary(), false))
                {
                    DbTrans.Rollback();
                    return false;
                }
            }
        }
    }

    if(!pDatabase->ExecSql(QString("UPDATE `device` SET `config_version`='',`icd_filename`='' WHERE `device_id`='%1'").arg(iDeviceId)))
    {
        DbTrans.Rollback();
        return false;
    }

    if(!DbUpdateDeviceModel(iDeviceId, QByteArray(), false))
    {
        DbTrans.Rollback();
        return false;
    }

    return DbTrans.Commit();
}

bool PeProjectVersion::DbUpdateSclTemplate(const QByteArray &baTemplateContent, bool bUseTransaction)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    ProjectExplorer::DbTransaction DbTrans(this, bUseTransaction);

    if(!pDatabase->ExecSql("DELETE FROM `scl_template`"))
    {
        DbTrans.Rollback();
        return false;
    }

    if(!pDatabase->ExecSql("INSERT INTO `scl_template`(`template_blob`) VALUES(NULL)"))
    {
        DbTrans.Rollback();
        return false;
    }

    if(!baTemplateContent.isEmpty() && !baTemplateContent.isNull())
    {
        if(!pDatabase->WriteBlog("UPDATE `scl_template` SET `template_blob` = ?", baTemplateContent))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    return DbTrans.Commit();
}

bool PeProjectVersion::DbReadSclTemplate(QByteArray &baTemplateContent)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    return pDatabase->ReadBlog("select `template_blob` from `scl_template`", baTemplateContent);
}

bool PeProjectVersion::DbUpdateSclCommunication(const QByteArray &baCommunicationContent, bool bUseTransaction)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    ProjectExplorer::DbTransaction DbTrans(this, bUseTransaction);

    if(!pDatabase->ExecSql("DELETE FROM `scl_communication`"))
    {
        DbTrans.Rollback();
        return false;
    }

    if(!pDatabase->ExecSql("INSERT INTO `scl_communication`(`communication_blob`) VALUES(NULL)"))
    {
        DbTrans.Rollback();
        return false;
    }

    if(!baCommunicationContent.isEmpty() && !baCommunicationContent.isNull())
    {
        if(!pDatabase->WriteBlog("UPDATE `scl_communication` SET `communication_blob` = ?", baCommunicationContent))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    return DbTrans.Commit();
}

bool PeProjectVersion::DbReadSclCommunication(QByteArray &baCommunicationContent)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    return pDatabase->ReadBlog("SELECT `communication_blob` FROM `scl_communication`", baCommunicationContent);
}

bool PeProjectVersion::DbClearAllModel(bool bUseTransaction)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    ProjectExplorer::DbTransaction DbTrans(this, bUseTransaction);

    // Clear device model
    if(!pDatabase->ExecSql("UPDATE `device` set config_version='',`icd_content`=NULL"))
    {
        DbTrans.Rollback();
        return false;
    }

    // Clear vterminal
    if(!pDatabase->ExecSql("DELETE FROM `vterminal`"))
    {
        DbTrans.Rollback();
        return false;
    }

    // Clear strap
    if(!pDatabase->ExecSql("DELETE FROM `strap`"))
    {
        DbTrans.Rollback();
        return false;
    }

    // Clear vterminalconn
    if(!pDatabase->ExecSql("DELETE FROM `vterminal_connection`"))
    {
        DbTrans.Rollback();
        return false;
    }

    // Clear communication
    if(!pDatabase->ExecSql("DELETE FROM `scl_communication`"))
    {
        DbTrans.Rollback();
        return false;
    }

    // Clear template
    if(!pDatabase->ExecSql("DELETE FROM `scl_template`"))
    {
        DbTrans.Rollback();
        return false;
    }

    return DbTrans.Commit();
}

bool PeProjectVersion::DbReadSettings(QMap<QString, QString> &mapSettings)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    MySqlWrapper::MySqlQuery query(pDatabase);
    if(!query.Exec("SELECT * FROM `settings`"))
        return false;

    mapSettings.clear();
    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
        mapSettings.insert(record.value("key").toString(), record.value("value").toString());

    return true;
}

bool PeProjectVersion::DbUpdateSettings(const QMap<QString, QString> &mapSettings, bool bUseTransaction)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    ProjectExplorer::DbTransaction DbTrans(this, bUseTransaction);

    if(!pDatabase->ExecSql("DELETE FROM `settings`"))
    {
        DbTrans.Rollback();
        return false;
    }

    foreach(const QString &strKey, mapSettings.keys())
    {
        if(!pDatabase->ExecSql(QString("INSERT INTO `settings`(`key`,`value`) VALUES('%1','%2')").arg(strKey, mapSettings.value(strKey))))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    return DbTrans.Commit();
}

bool PeProjectVersion::DbReadDwgFile(int iDwgId, QByteArray &baFileContent)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    return pDatabase->ReadBlog(QString("SELECT `file` FROM `dwg` WHERE `dwg_id`='%1'").arg(iDwgId), baFileContent);
}

bool PeProjectVersion::DbUpdateDwgFile(int iDwgId, const QByteArray &baFileContent, bool bUseTransaction)
{
    if(iDwgId <= PeProjectObject::m_iInvalidObjectId)
        return false;

    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return false;

    ProjectExplorer::DbTransaction DbTrans(this, bUseTransaction);

    if(!baFileContent.isEmpty() && !baFileContent.isNull())
    {
        QString strSql = QString("UPDATE `dwg` SET `file` = ? WHERE `dwg_id`='%1'").arg(iDwgId);
        if(!pDatabase->WriteBlog(strSql, baFileContent))
        {
            DbTrans.Rollback();
            return false;
        }
    }
    else
    {
        if(!pDatabase->ExecSql(QString("UPDATE `dwg` set `file` = NULL WHERE `dwg_id`='%1'").arg(iDwgId)))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    return DbTrans.Commit();
}

int PeProjectVersion::DbGenerateNewInfoSetGroup()
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
    if(!pDatabase)
        return PeProjectObject::m_iInvalidObjectId;

    MySqlWrapper::MySqlQuery query(pDatabase);
    if(!query.Exec("SELECT MAX(infoset_id) FROM infoset"))
        return -1;

    if(query.GetResults().size() != 1)
        return -1;

    if(!query.GetResults().first().contains("MAX(infoset_id)"))
        return -1;

    return query.GetResults().first().value("MAX(infoset_id)").toInt() + 1;
}

bool PeProjectVersion::DbReadDeviceInfomation(QList<PeDevice*> &lstDevice)
{
    lstDevice.clear();

    if(m_d->m_bOpend)
    {
        foreach(PeDevice *pDevice, m_d->m_mapIdToDevices)
        {
            if(pDevice->GetDeviceType() == PeDevice::dtIED || pDevice->GetDeviceType() == PeDevice::dtSwitch)
                lstDevice.append(new PeDevice(*pDevice));
        }
    }
    else
    {
        MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
        if(!pDatabase)
            return false;

        MySqlWrapper::MySqlQuery query(pDatabase);

        query.Clear();
        if(!query.Exec(QString("SELECT * FROM `device` WHERE `device_type`=%1 OR `device_type`=%2").arg(PeDevice::dtIED).arg(PeDevice::dtSwitch)))
            return false;

        foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
        {
            PeDevice *pDevice = new PeDevice(this, record.value("device_id").toInt());
            pDevice->SetDeviceType(PeDevice::DeviceType(record.value("device_type").toInt()));
            pDevice->SetName(record.value("name").toString());
            pDevice->SetDescription(record.value("description").toString());
            pDevice->SetManufacture(record.value("manufacture").toString());
            pDevice->SetType(record.value("type").toString());
            pDevice->SetConfigVersion(record.value("config_version").toString());
            pDevice->SetVLevel(record.value("vlevel").toInt());
            pDevice->SetCubiclePos(record.value("cubicle_pos").toString());
            pDevice->SetCubicleNum(record.value("cubicle_num").toString());
            lstDevice.append(pDevice);
        }
    }

    return true;
}

bool PeProjectVersion::DbReadGeneralInformation(int &iBayNumber, int &iRoomNumber,
                                                int &iCubicleNumber, int &iIedNumber,
                                                int &iSwitchNumber, int &iCableNumber)
{
    iBayNumber = 0;
    iRoomNumber = 0;
    iCubicleNumber = 0;
    iIedNumber = 0;
    iSwitchNumber = 0;
    iCableNumber = 0;

    if(m_d->m_bOpend)
    {
        iBayNumber = m_d->m_mapIdToBays.size();
        iRoomNumber = m_d->m_mapIdToRooms.size();
        iCubicleNumber = m_d->m_mapIdToCubicles.size();
        iCableNumber = m_d->m_mapIdToCables.size();

        foreach(PeDevice *pDevice, m_d->m_mapIdToDevices)
        {
            if(pDevice->GetDeviceType() == PeDevice::dtIED)
                iIedNumber++;
            else if(pDevice->GetDeviceType() == PeDevice::dtSwitch)
                iSwitchNumber++;
        }
    }
    else
    {
        MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetProjectDb(GetId());
        if(!pDatabase)
            return false;

        MySqlWrapper::MySqlQuery query(pDatabase);

        // Read Bay Number
        query.Clear();
        if(query.Exec("SELECT COUNT(*) AS `bay_number` FROM `bay`") && !query.GetResults().isEmpty())
            iBayNumber = query.GetResults().first().value("bay_number").toInt();


        // Read Room Number
        query.Clear();
        if(query.Exec("SELECT COUNT(*) AS `room_number` FROM `room`") && !query.GetResults().isEmpty())
            iRoomNumber = query.GetResults().first().value("room_number").toInt();

        // Read Cubicle Number
        query.Clear();
        if(query.Exec("SELECT COUNT(*) AS `cubicle_number` FROM `cubicle`") && !query.GetResults().isEmpty())
            iCubicleNumber = query.GetResults().first().value("cubicle_number").toInt();

        // Read IED Number
        query.Clear();
        if(query.Exec(QString("SELECT COUNT(*) AS `ied_number` FROM `device` WHERE `device_type`=%1").arg(PeDevice::dtIED)) && !query.GetResults().isEmpty())
            iIedNumber = query.GetResults().first().value("ied_number").toInt();

        // Read Switch Number
        query.Clear();
        if(query.Exec(QString("SELECT COUNT(*) AS `switch_number` FROM `device` WHERE `device_type`=%1").arg(PeDevice::dtSwitch)) && !query.GetResults().isEmpty())
            iSwitchNumber = query.GetResults().first().value("switch_number").toInt();

        // Read Cable Number
        query.Clear();
        if(query.Exec(QString("SELECT COUNT(*) AS `cable_number` FROM `cable` WHERE `cable_type`=%1 OR `cable_type`=%2").arg(PeCable::ctOptical).arg(PeCable::ctTail)) && !query.GetResults().isEmpty())
            iCableNumber = query.GetResults().first().value("cable_number").toInt();
    }

    return true;
}

namespace ProjectExplorer {

class DbTransactionPrivate
{
public:
    DbTransactionPrivate(PeProjectVersion *pProjectVersion, bool bEnable) :
        m_pProjectVersion(pProjectVersion), m_transaction(DatabaseManager::Instance()->GetProjectDb(pProjectVersion->GetId()), bEnable)
    {
    }

    MySqlWrapper::MySqlTransaction  m_transaction;
    PeProjectVersion                *m_pProjectVersion;
};
}

DbTransaction::DbTransaction(PeProjectVersion *pProjectVersion, bool bEnable) : m_d(new DbTransactionPrivate(pProjectVersion, bEnable))
{
}

DbTransaction::~DbTransaction()
{
    delete m_d;
}

void DbTransaction::Rollback()
{
    m_d->m_transaction.Rollback();
}

bool DbTransaction::Commit()
{
    if(m_d->m_transaction.GetEnable() && !m_d->m_pProjectVersion->ValidateModifyTime())
    {
        Rollback();

        QMessageBox::information(0,
                                 tr("Reload %1").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProject::otProjectVersion)),
                                 tr("The current %1 has been modified by the other client, please reload the current %1").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProject::otProjectVersion)));
        return false;
    }

    m_d->m_transaction.Commit();

    if(m_d->m_transaction.GetEnable())
        m_d->m_pProjectVersion->UpdateModifyTime();

    return true;
}
