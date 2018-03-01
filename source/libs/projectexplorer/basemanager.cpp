#include "mysqlwrapper/mysqldatabase.h"
#include "mysqlwrapper/mysqlquery.h"
#include "mysqlwrapper/mysqltransaction.h"

#include "basemanager.h"
#include "databasemanager.h"
#include "pbdevicecategory.h"
#include "pbbaycategory.h"
#include "pbvlevel.h"
#include "pbmanufacture.h"
#include "pbdevicetype.h"
#include "pblibdevice.h"
#include "pblibboard.h"
#include "pblibport.h"
#include "pbtpbay.h"
#include "pbtpdevice.h"
#include "pbtpport.h"
#include "pbtpinfoset.h"

namespace ProjectExplorer {

class BaseManagerPrivate
{
public:
    BaseManagerPrivate()
    {
    }

    static BaseManager              *m_pInstance;

    QMap<int, PbDeviceCategory*>    m_mapIdToDeviceCategories;
    QMap<int, PbBayCategory*>       m_mapIdToBayCategories;
    QMap<int, PbVLevel*>            m_mapIdToVLevels;
    QMap<int, PbManufacture*>       m_mapIdToManufactures;
    QMap<int, PbDeviceType*>        m_mapIdToDeviceTypes;
    QMap<int, PbLibDevice*>         m_mapIdToLibDevices;
    QMap<int, PbLibBoard*>          m_mapIdToLibBoards;
    QMap<int, PbLibPort*>           m_mapIdToLibPorts;
    QMap<int, PbTpBay*>             m_mapIdToTpBays;
    QMap<int, PbTpDevice*>          m_mapIdToTpDevices;
    QMap<int, PbTpPort*>            m_mapIdToTpPorts;
    QMap<int, PbTpInfoSet*>         m_mapIdToTpInfoSets;
};

BaseManager *BaseManagerPrivate::m_pInstance = 0;

} // namespace ProjectExplorer

using namespace ProjectExplorer;

BaseManager::BaseManager(QObject *pParent) : QObject(pParent), m_d(new BaseManagerPrivate)
{
    BaseManagerPrivate::m_pInstance = this;
}

BaseManager::~BaseManager()
{
    delete m_d;
    BaseManagerPrivate::m_pInstance = 0;
}

BaseManager* BaseManager::Instance()
{
    return BaseManagerPrivate::m_pInstance;
}

bool BaseManager::Initialize()
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    MySqlWrapper::MySqlQuery query(pDatabase);

    // Read Device Category
    query.Clear();
    if(!query.Exec("SELECT * FROM base_devicecategory"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PbDeviceCategory *pDeviceCategory = new PbDeviceCategory(record.value("category_id").toInt());
        pDeviceCategory->SetName(record.value("name").toString());
        pDeviceCategory->SetCode(record.value("code").toString());
        m_d->m_mapIdToDeviceCategories.insert(pDeviceCategory->GetId(), pDeviceCategory);
    }

    // Read Bay Category
    query.Clear();
    if(!query.Exec("SELECT * FROM base_baycategory"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PbBayCategory *pBayCategory = new PbBayCategory(record.value("category_id").toInt());
        pBayCategory->SetName(record.value("name").toString());
        pBayCategory->SetCode(record.value("code").toString());
        m_d->m_mapIdToBayCategories.insert(pBayCategory->GetId(), pBayCategory);
    }

    // Read VLevel
    query.Clear();
    if(!query.Exec("SELECT * FROM base_vlevel"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PbVLevel *pVLevel = new PbVLevel(record.value("vlevel_id").toInt());
        pVLevel->SetVLevel(record.value("vlevel").toInt());
        pVLevel->SetCode(record.value("code").toString());
        m_d->m_mapIdToVLevels.insert(pVLevel->GetId(), pVLevel);
    }

    // Read Manufacture
    query.Clear();
    if(!query.Exec("SELECT * FROM base_manufacture"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PbManufacture *pManufacture = new PbManufacture(record.value("manufacture_id").toInt());
        pManufacture->SetName(record.value("name").toString());
        m_d->m_mapIdToManufactures.insert(pManufacture->GetId(), pManufacture);
    }

    // Read DeviceType
    query.Clear();
    if(!query.Exec("SELECT * FROM base_devicetype"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PbDeviceType *pDeviceType = new PbDeviceType(record.value("type_id").toInt());
        pDeviceType->SetName(record.value("name").toString());
        m_d->m_mapIdToDeviceTypes.insert(pDeviceType->GetId(), pDeviceType);
    }

    // Read LibDevice
    query.Clear();
    if(!query.Exec("SELECT * FROM `lib_device`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PbLibDevice *pLibDevice = new PbLibDevice(record.value("device_id").toInt());
        pLibDevice->SetDeviceType(PbLibDevice::DeviceType(record.value("device_type").toInt()));
        pLibDevice->SetManufacture(record.value("manufacture").toString());
        pLibDevice->SetDeviceCategory(record.value("device_category").toString());
        pLibDevice->SetBayCategory(record.value("bay_category").toString());
        pLibDevice->SetType(record.value("type").toString());
        pLibDevice->SetIcdFileName(record.value("icd_filename").toString());
        m_d->m_mapIdToLibDevices.insert(pLibDevice->GetId(), pLibDevice);
    }

    // Read LibBoard
    query.Clear();
    if(!query.Exec("SELECT * FROM `lib_board`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        const int iDeviceId = record.value("device_id").toInt();
        if(PbLibDevice *pLibDevice = FindLibDeviceById(iDeviceId))
        {
            PbLibBoard *pLibBoard = new PbLibBoard(record.value("board_id").toInt());
            pLibBoard->SetPosition(record.value("position").toString());
            pLibBoard->SetDescription(record.value("description").toString());
            pLibBoard->SetType(record.value("type").toString());

            pLibBoard->SetParentDevice(pLibDevice);
            pLibDevice->AddChildBoard(pLibBoard);

            m_d->m_mapIdToLibBoards.insert(pLibBoard->GetId(), pLibBoard);
        }
    }

    // Read LibPort
    query.Clear();
    if(!query.Exec("SELECT * FROM `lib_port`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        const int iBoardId = record.value("board_id").toInt();
        if(PbLibBoard *pLibBoard = FindLibBoardById(iBoardId))
        {
            PbLibPort *pLibPort = new PbLibPort(record.value("port_id").toInt());
            pLibPort->SetName(record.value("name").toString());
            pLibPort->SetGroup(record.value("group").toInt());
            pLibPort->SetPortType(PbLibPort::PortType(record.value("type").toInt()));
            pLibPort->SetPortDirection(PbLibPort::PortDirection(record.value("direction").toInt()));
            pLibPort->SetFiberPlug(PbLibPort::FiberPlug(record.value("fiber_plug").toInt()));
            pLibPort->SetFiberMode(PbLibPort::FiberMode(record.value("fiber_mode").toInt()));
            pLibPort->SetCascade(record.value("cascade").toBool());

            pLibPort->SetParentBoard(pLibBoard);
            pLibBoard->AddChildPort(pLibPort);

            m_d->m_mapIdToLibPorts.insert(pLibPort->GetId(), pLibPort);
        }
    }

    // Read TpBay
    query.Clear();
    if(!query.Exec("SELECT * FROM `tp_bay`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        PbTpBay *pTpBay = new PbTpBay(record.value("bay_id").toInt());
        pTpBay->SetName(record.value("name").toString());
        pTpBay->SetVLevel(record.value("vlevel").toInt());

        m_d->m_mapIdToTpBays.insert(pTpBay->GetId(), pTpBay);
    }

    // Read TpDevice
    query.Clear();
    if(!query.Exec("SELECT * FROM `tp_device`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        const int iBayId = record.value("bay_id").toInt();
        if(PbTpBay *pTpBay = FindTpBayById(iBayId))
        {
            PbTpDevice *pTpDevice = new PbTpDevice(record.value("device_id").toInt());
            pTpDevice->SetName(record.value("name").toString());
            pTpDevice->SetDeviceType(PbTpDevice::DeviceType(record.value("device_type").toInt()));

            pTpDevice->SetParentBay(pTpBay);
            pTpBay->AddChildDevice(pTpDevice);

            m_d->m_mapIdToTpDevices.insert(pTpDevice->GetId(), pTpDevice);
        }
    }

    // Read TpPort
    query.Clear();
    if(!query.Exec("SELECT * FROM `tp_port`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        const int iDeviceId = record.value("device_id").toInt();
        if(PbTpDevice *pTpDevice = FindTpDeviceById(iDeviceId))
        {
            PbTpPort *pTpPort = new PbTpPort(record.value("port_id").toInt());
            pTpPort->SetName(record.value("name").toString());
            pTpPort->SetGroup(record.value("group").toInt());
            pTpPort->SetPortDirection(PbTpPort::PortDirection(record.value("direction").toInt()));

            pTpPort->SetParentDevice(pTpDevice);
            pTpDevice->AddChildPort(pTpPort);

            m_d->m_mapIdToTpPorts.insert(pTpPort->GetId(), pTpPort);
        }
    }

    // Read TpInfoSet
    query.Clear();
    if(!query.Exec("SELECT * FROM `tp_infoset`"))
        return false;

    foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
    {
        const int iBayId = record.value("bay_id").toInt();
        if(PbTpBay *pTpBay = FindTpBayById(iBayId))
        {
            PbTpInfoSet *pTpInfoSet = new PbTpInfoSet(record.value("infoset_id").toInt());
            pTpInfoSet->SetName(record.value("name").toString());
            pTpInfoSet->SetInfoSetType(PbTpInfoSet::InfoSetType(record.value("type").toInt()));
            pTpInfoSet->SetGroup(record.value("group").toInt());
            pTpInfoSet->SetTxIED(FindTpDeviceById(record.value("txied_id").toInt()));
            pTpInfoSet->SetTxPort(FindTpPortById(record.value("txiedport_id").toInt()));
            pTpInfoSet->SetRxIED(FindTpDeviceById(record.value("rxied_id").toInt()));
            pTpInfoSet->SetRxPort(FindTpPortById(record.value("rxiedport_id").toInt()));
            pTpInfoSet->SetSwitch1(FindTpDeviceById(record.value("switch1_id").toInt()));
            pTpInfoSet->SetSwitch1TxPort(FindTpPortById(record.value("switch1_txport_id").toInt()));
            pTpInfoSet->SetSwitch1RxPort(FindTpPortById(record.value("switch1_rxport_id").toInt()));
            pTpInfoSet->SetSwitch2(FindTpDeviceById(record.value("switch2_id").toInt()));
            pTpInfoSet->SetSwitch2TxPort(FindTpPortById(record.value("switch2_txport_id").toInt()));
            pTpInfoSet->SetSwitch2RxPort(FindTpPortById(record.value("switch2_rxport_id").toInt()));
            pTpInfoSet->SetSwitch3(FindTpDeviceById(record.value("switch3_id").toInt()));
            pTpInfoSet->SetSwitch3TxPort(FindTpPortById(record.value("switch3_txport_id").toInt()));
            pTpInfoSet->SetSwitch3RxPort(FindTpPortById(record.value("switch3_rxport_id").toInt()));
            pTpInfoSet->SetSwitch4(FindTpDeviceById(record.value("switch4_id").toInt()));
            pTpInfoSet->SetSwitch4TxPort(FindTpPortById(record.value("switch4_txport_id").toInt()));
            pTpInfoSet->SetSwitch4RxPort(FindTpPortById(record.value("switch4_rxport_id").toInt()));

            pTpInfoSet->SetParentBay(pTpBay);
            pTpBay->AddChildInfoSet(pTpInfoSet);

            m_d->m_mapIdToTpInfoSets.insert(pTpInfoSet->GetId(), pTpInfoSet);
        }
    }

    emit sigInitialized();

    return true;
}

void BaseManager::Uninitialize()
{
    qDeleteAll(m_d->m_mapIdToDeviceCategories);
    m_d->m_mapIdToDeviceCategories.clear();

    qDeleteAll(m_d->m_mapIdToBayCategories);
    m_d->m_mapIdToBayCategories.clear();

    qDeleteAll(m_d->m_mapIdToVLevels);
    m_d->m_mapIdToVLevels.clear();

    qDeleteAll(m_d->m_mapIdToManufactures);
    m_d->m_mapIdToManufactures.clear();

    qDeleteAll(m_d->m_mapIdToLibDevices);
    m_d->m_mapIdToLibDevices.clear();

    qDeleteAll(m_d->m_mapIdToLibBoards);
    m_d->m_mapIdToLibBoards.clear();

    qDeleteAll(m_d->m_mapIdToLibPorts);
    m_d->m_mapIdToLibPorts.clear();

    qDeleteAll(m_d->m_mapIdToTpBays);
    m_d->m_mapIdToTpBays.clear();

    emit sigUninitialized();
}

bool BaseManager::Reinitialize()
{
    Uninitialize();
    return Initialize();
}

PbBaseObject* BaseManager::CreateObject(const PbBaseObject &BaseObject, bool bNotify )
{
    PbBaseObject *pBaseObjectCreated = 0;

    const PbBaseObject::ObjectType &eObjectType = BaseObject.GetObjectType();
    switch(eObjectType)
    {
        case PbBaseObject::botDeviceCategory:
        {
            PbDeviceCategory *pDeviceCategoryTemp = qobject_cast<PbDeviceCategory*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pDeviceCategoryTemp)
                return 0;

            PbDeviceCategory *pDeviceCategory = new PbDeviceCategory(pDeviceCategoryTemp->GetId());
            pDeviceCategory->SetName(pDeviceCategoryTemp->GetName());
            pDeviceCategory->SetCode(pDeviceCategoryTemp->GetCode());

            m_d->m_mapIdToDeviceCategories.insert(pDeviceCategory->GetId(), pDeviceCategory);
            pBaseObjectCreated = pDeviceCategory;
        }
        break;

        case PbBaseObject::botBayCategory:
        {
            PbBayCategory *pBayCategoryTemp = qobject_cast<PbBayCategory*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pBayCategoryTemp)
                return 0;

            PbBayCategory *pBayCategory = new PbBayCategory(pBayCategoryTemp->GetId());
            pBayCategory->SetName(pBayCategoryTemp->GetName());
            pBayCategory->SetCode(pBayCategoryTemp->GetCode());

            m_d->m_mapIdToBayCategories.insert(pBayCategory->GetId(), pBayCategory);
            pBaseObjectCreated = pBayCategory;
        }
        break;

        case PbBaseObject::botVLevel:
        {
            PbVLevel *pVLevelTemp = qobject_cast<PbVLevel*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pVLevelTemp)
                return 0;

            PbVLevel *pVLevel = new PbVLevel(pVLevelTemp->GetId());
            pVLevel->SetVLevel(pVLevelTemp->GetVLevel());
            pVLevel->SetCode(pVLevelTemp->GetCode());

            m_d->m_mapIdToVLevels.insert(pVLevel->GetId(), pVLevel);
            pBaseObjectCreated = pVLevel;
        }
        break;

        case PbBaseObject::botManufacture:
        {
            PbManufacture *pManufactureTemp = qobject_cast<PbManufacture*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pManufactureTemp)
                return 0;

            PbManufacture *pManufacture = new PbManufacture(pManufactureTemp->GetId());
            pManufacture->SetName(pManufactureTemp->GetName());

            m_d->m_mapIdToManufactures.insert(pManufacture->GetId(), pManufacture);
            pBaseObjectCreated = pManufacture;
        }
        break;

        case PbBaseObject::botDeviceType:
        {
            PbDeviceType *pDeviceTypeTemp = qobject_cast<PbDeviceType*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pDeviceTypeTemp)
                return 0;

            PbDeviceType *pDeviceType = new PbDeviceType(pDeviceTypeTemp->GetId());
            pDeviceType->SetName(pDeviceTypeTemp->GetName());

            m_d->m_mapIdToDeviceTypes.insert(pDeviceType->GetId(), pDeviceType);
            pBaseObjectCreated = pDeviceType;
        }
        break;

        case PbBaseObject::botLibDevice:
        {
            PbLibDevice *pLibDeviceTemp = qobject_cast<PbLibDevice*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pLibDeviceTemp)
                return 0;

            PbLibDevice *pLibDevice = new PbLibDevice(pLibDeviceTemp->GetId());
            pLibDevice->SetDeviceType(pLibDeviceTemp->GetDeviceType());
            pLibDevice->SetManufacture(pLibDeviceTemp->GetManufacture());
            pLibDevice->SetDeviceCategory(pLibDeviceTemp->GetDeviceCategory());
            pLibDevice->SetBayCategory(pLibDeviceTemp->GetBayCategory());
            pLibDevice->SetType(pLibDeviceTemp->GetType());
            pLibDevice->SetIcdFileName(pLibDeviceTemp->GetIcdFileName());

            m_d->m_mapIdToLibDevices.insert(pLibDevice->GetId(), pLibDevice);
            pBaseObjectCreated = pLibDevice;
        }
        break;

        case PbBaseObject::botLibBoard:
        {
            PbLibBoard *pLibBoardTemp = qobject_cast<PbLibBoard*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pLibBoardTemp)
                return 0;

            PbLibDevice *pParentLibDevice = pLibBoardTemp->GetParentDevice();
            if(!pParentLibDevice)
                return 0;

            PbLibBoard *pLibBoard = new PbLibBoard(pLibBoardTemp->GetId());
            pLibBoard->SetPosition(pLibBoardTemp->GetPosition());
            pLibBoard->SetDescription(pLibBoardTemp->GetDescription());
            pLibBoard->SetType(pLibBoardTemp->GetType());
            pLibBoard->SetParentDevice(pParentLibDevice);
            pParentLibDevice->AddChildBoard(pLibBoard);

            m_d->m_mapIdToLibBoards.insert(pLibBoard->GetId(), pLibBoard);
            pBaseObjectCreated = pLibBoard;
        }
        break;

        case PbBaseObject::botLibPort:
        {
            PbLibPort *pLibPortTemp = qobject_cast<PbLibPort*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pLibPortTemp)
                return 0;

            PbLibBoard *pParentLibBoard = pLibPortTemp->GetParentBoard();
            if(!pParentLibBoard)
                return 0;

            PbLibPort *pLibPort = new PbLibPort(pLibPortTemp->GetId());
            pLibPort->SetName(pLibPortTemp->GetName());
            pLibPort->SetGroup(pLibPortTemp->GetGroup());
            pLibPort->SetPortType(pLibPortTemp->GetPortType());
            pLibPort->SetPortDirection(pLibPortTemp->GetPortDirection());
            pLibPort->SetFiberPlug(pLibPortTemp->GetFiberPlug());
            pLibPort->SetFiberMode(pLibPortTemp->GetFiberMode());
            pLibPort->SetCascade(pLibPortTemp->GetCascade());
            pLibPort->SetParentBoard(pParentLibBoard);
            pParentLibBoard->AddChildPort(pLibPort);

            m_d->m_mapIdToLibPorts.insert(pLibPort->GetId(), pLibPort);
            pBaseObjectCreated = pLibPort;
        }
        break;

        case PbBaseObject::botTpBay:
        {
            PbTpBay *pTpBayTemp = qobject_cast<PbTpBay*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pTpBayTemp)
                return 0;

            PbTpBay *pTpBay = new PbTpBay(pTpBayTemp->GetId());
            pTpBay->SetName(pTpBayTemp->GetName());
            pTpBay->SetVLevel(pTpBayTemp->GetVLevel());

            m_d->m_mapIdToTpBays.insert(pTpBay->GetId(), pTpBay);
            pBaseObjectCreated = pTpBay;
        }
        break;

        case PbBaseObject::botTpDevice:
        {
            PbTpDevice *pTpDeviceTemp = qobject_cast<PbTpDevice*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pTpDeviceTemp)
                return 0;

            PbTpBay *pParentTpBay = pTpDeviceTemp->GetParentBay();
            if(!pParentTpBay)
                return 0;

            PbTpDevice *pTpDevice = new PbTpDevice(pTpDeviceTemp->GetId());
            pTpDevice->SetName(pTpDeviceTemp->GetName());
            pTpDevice->SetDeviceType(pTpDeviceTemp->GetDeviceType());
            pTpDevice->SetParentBay(pParentTpBay);
            pParentTpBay->AddChildDevice(pTpDevice);

            m_d->m_mapIdToTpDevices.insert(pTpDevice->GetId(), pTpDevice);
            pBaseObjectCreated = pTpDevice;
        }
        break;

        case PbBaseObject::botTpPort:
        {
            PbTpPort *pTpPortTemp = qobject_cast<PbTpPort*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pTpPortTemp)
                return 0;

            PbTpDevice *pParentTpDevice = pTpPortTemp->GetParentDevice();
            if(!pParentTpDevice)
                return 0;

            PbTpPort *pTpPort = new PbTpPort(pTpPortTemp->GetId());
            pTpPort->SetName(pTpPortTemp->GetName());
            pTpPort->SetGroup(pTpPortTemp->GetGroup());
            pTpPort->SetPortDirection(pTpPortTemp->GetPortDirection());
            pTpPort->SetParentDevice(pParentTpDevice);
            pParentTpDevice->AddChildPort(pTpPort);

            m_d->m_mapIdToTpPorts.insert(pTpPort->GetId(), pTpPort);
            pBaseObjectCreated = pTpPort;
        }
        break;

        case PbBaseObject::botTpInfoSet:
        {
            PbTpInfoSet *pTpInfoSetTemp = qobject_cast<PbTpInfoSet*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pTpInfoSetTemp)
                return 0;

            PbTpBay *pParentTpBay = pTpInfoSetTemp->GetParentBay();
            if(!pParentTpBay)
                return 0;

            PbTpInfoSet *pTpInfoSet = new PbTpInfoSet(pTpInfoSetTemp->GetId());
            pTpInfoSet->SetName(pTpInfoSetTemp->GetName());
            pTpInfoSet->SetInfoSetType(pTpInfoSetTemp->GetInfoSetType());
            pTpInfoSet->SetGroup(pTpInfoSetTemp->GetGroup());
            pTpInfoSet->SetTxIED(pTpInfoSetTemp->GetTxIED());
            pTpInfoSet->SetTxPort(pTpInfoSetTemp->GetTxPort());
            pTpInfoSet->SetRxIED(pTpInfoSetTemp->GetRxIED());
            pTpInfoSet->SetRxPort(pTpInfoSetTemp->GetRxPort());
            pTpInfoSet->SetSwitch1(pTpInfoSetTemp->GetSwitch1());
            pTpInfoSet->SetSwitch1TxPort(pTpInfoSetTemp->GetSwitch1TxPort());
            pTpInfoSet->SetSwitch1RxPort(pTpInfoSetTemp->GetSwitch1RxPort());
            pTpInfoSet->SetSwitch2(pTpInfoSetTemp->GetSwitch2());
            pTpInfoSet->SetSwitch2TxPort(pTpInfoSetTemp->GetSwitch2TxPort());
            pTpInfoSet->SetSwitch2RxPort(pTpInfoSetTemp->GetSwitch2RxPort());
            pTpInfoSet->SetSwitch3(pTpInfoSetTemp->GetSwitch3());
            pTpInfoSet->SetSwitch3TxPort(pTpInfoSetTemp->GetSwitch3TxPort());
            pTpInfoSet->SetSwitch3RxPort(pTpInfoSetTemp->GetSwitch3RxPort());
            pTpInfoSet->SetSwitch4(pTpInfoSetTemp->GetSwitch4());
            pTpInfoSet->SetSwitch4TxPort(pTpInfoSetTemp->GetSwitch4TxPort());
            pTpInfoSet->SetSwitch4RxPort(pTpInfoSetTemp->GetSwitch4RxPort());
            pTpInfoSet->SetParentBay(pParentTpBay);
            pParentTpBay->AddChildInfoSet(pTpInfoSet);

            m_d->m_mapIdToTpInfoSets.insert(pTpInfoSet->GetId(), pTpInfoSet);
            pBaseObjectCreated = pTpInfoSet;
        }
        break;
    }

    if(pBaseObjectCreated && bNotify)
        emit sigObjectCreated(pBaseObjectCreated);

    return pBaseObjectCreated;
}

PbBaseObject* BaseManager::UpdateObject(const PbBaseObject &BaseObject, bool bNotify )
{
    const PbBaseObject::ObjectType &eObjectType = BaseObject.GetObjectType();
    switch(eObjectType)
    {
        case PbBaseObject::botDeviceCategory:
        {
            PbDeviceCategory *pDeviceCategoryTemp = qobject_cast<PbDeviceCategory*>(const_cast<PbBaseObject*>(&BaseObject));
            PbDeviceCategory *pDeviceCategory = FindDeviceCategoryById(BaseObject.GetId());
            if(!pDeviceCategoryTemp || !pDeviceCategory)
                return 0;

            // Update property
            if(pDeviceCategory->GetName() != pDeviceCategoryTemp->GetName() ||
               pDeviceCategory->GetCode() != pDeviceCategoryTemp->GetCode())
            {
                pDeviceCategory->SetName(pDeviceCategoryTemp->GetName());
                pDeviceCategory->SetCode(pDeviceCategoryTemp->GetCode());

                if(bNotify)
                    emit sigObjectPropertyChanged(pDeviceCategory);
            }

            return pDeviceCategory;
        }
        break;

        case PbBaseObject::botBayCategory:
        {
            PbBayCategory *pBayCategoryTemp = qobject_cast<PbBayCategory*>(const_cast<PbBaseObject*>(&BaseObject));
            PbBayCategory *pBayCategory = FindBayCategoryById(BaseObject.GetId());
            if(!pBayCategoryTemp || !pBayCategory)
                return 0;

            // Update property
            if(pBayCategory->GetName() != pBayCategoryTemp->GetName() ||
               pBayCategory->GetCode() != pBayCategoryTemp->GetCode())
            {
                pBayCategory->SetName(pBayCategoryTemp->GetName());
                pBayCategory->SetCode(pBayCategoryTemp->GetCode());

                if(bNotify)
                    emit sigObjectPropertyChanged(pBayCategory);
            }

            return pBayCategory;
        }
        break;

        case PbBaseObject::botVLevel:
        {
            PbVLevel *pVLevelTemp = qobject_cast<PbVLevel*>(const_cast<PbBaseObject*>(&BaseObject));
            PbVLevel *pVLevel = FindVLevelById(BaseObject.GetId());
            if(!pVLevelTemp || !pVLevel)
                return 0;

            // Update property
            if(pVLevel->GetVLevel() != pVLevelTemp->GetVLevel() ||
               pVLevel->GetCode() != pVLevelTemp->GetCode())
            {
                pVLevel->SetVLevel(pVLevelTemp->GetVLevel());
                pVLevel->SetCode(pVLevelTemp->GetCode());

                if(bNotify)
                    emit sigObjectPropertyChanged(pVLevel);
            }

            return pVLevel;
        }
        break;

        case PbBaseObject::botManufacture:
        {
            PbManufacture *pManufactureTemp = qobject_cast<PbManufacture*>(const_cast<PbBaseObject*>(&BaseObject));
            PbManufacture *pManufacture = FindManufactureById(BaseObject.GetId());
            if(!pManufactureTemp || !pManufacture)
                return 0;

            // Update property
            if(pManufacture->GetName() != pManufactureTemp->GetName())
            {
                pManufacture->SetName(pManufactureTemp->GetName());

                if(bNotify)
                    emit sigObjectPropertyChanged(pManufacture);
            }

            return pManufacture;
        }
        break;

        case PbBaseObject::botDeviceType:
        {
            PbDeviceType *pDeviceTypeTemp = qobject_cast<PbDeviceType*>(const_cast<PbBaseObject*>(&BaseObject));
            PbDeviceType *pDeviceType = FindDeviceTypeById(BaseObject.GetId());
            if(!pDeviceTypeTemp || !pDeviceType)
                return 0;

            // Update property
            if(pDeviceType->GetName() != pDeviceTypeTemp->GetName())
            {
                pDeviceType->SetName(pDeviceTypeTemp->GetName());

                if(bNotify)
                    emit sigObjectPropertyChanged(pDeviceType);
            }

            return pDeviceType;
        }
        break;

        case PbBaseObject::botLibDevice:
        {
            PbLibDevice *pLibDeviceTemp = qobject_cast<PbLibDevice*>(const_cast<PbBaseObject*>(&BaseObject));
            PbLibDevice *pLibDevice = FindLibDeviceById(BaseObject.GetId());
            if(!pLibDeviceTemp || !pLibDevice)
                return 0;

            // Update property
            if(pLibDevice->GetDeviceType() != pLibDeviceTemp->GetDeviceType() ||
               pLibDevice->GetManufacture() != pLibDeviceTemp->GetManufacture() ||
               pLibDevice->GetDeviceCategory() != pLibDeviceTemp->GetDeviceCategory() ||
               pLibDevice->GetBayCategory() != pLibDeviceTemp->GetBayCategory() ||
               pLibDevice->GetType() != pLibDeviceTemp->GetType() ||
               pLibDevice->GetIcdFileName() != pLibDeviceTemp->GetIcdFileName())
            {
                pLibDevice->SetDeviceType(pLibDeviceTemp->GetDeviceType());
                pLibDevice->SetManufacture(pLibDeviceTemp->GetManufacture());
                pLibDevice->SetDeviceCategory(pLibDeviceTemp->GetDeviceCategory());
                pLibDevice->SetBayCategory(pLibDeviceTemp->GetBayCategory());
                pLibDevice->SetType(pLibDeviceTemp->GetType());
                pLibDevice->SetIcdFileName(pLibDeviceTemp->GetIcdFileName());

                if(bNotify)
                    emit sigObjectPropertyChanged(pLibDevice);
            }

            return pLibDevice;
        }
        break;

        case PbBaseObject::botLibBoard:
        {
            PbLibBoard *pLibBoardTemp = qobject_cast<PbLibBoard*>(const_cast<PbBaseObject*>(&BaseObject));
            PbLibBoard *pLibBoard = FindLibBoardById(BaseObject.GetId());
            if(!pLibBoardTemp || !pLibBoard || pLibBoard->GetParentDevice() != pLibBoardTemp->GetParentDevice())
                return 0;

            if(pLibBoard->GetPosition() != pLibBoardTemp->GetPosition() ||
               pLibBoard->GetDescription() != pLibBoardTemp->GetDescription() ||
               pLibBoard->GetType() != pLibBoardTemp->GetType())
            {
                pLibBoard->SetPosition(pLibBoardTemp->GetPosition());
                pLibBoard->SetDescription(pLibBoardTemp->GetDescription());
                pLibBoard->SetType(pLibBoardTemp->GetType());

                if(bNotify)
                    emit sigObjectPropertyChanged(pLibBoard);
            }

            return pLibBoard;
        }
        break;

        case PbBaseObject::botLibPort:
        {
            PbLibPort *pLibPortTemp = qobject_cast<PbLibPort*>(const_cast<PbBaseObject*>(&BaseObject));
            PbLibPort *pLibPort = FindLibPortById(BaseObject.GetId());
            if(!pLibPortTemp || !pLibPort || pLibPort->GetParentBoard() != pLibPortTemp->GetParentBoard())
                return 0;

            if(pLibPort->GetName() != pLibPortTemp->GetName() ||
               pLibPort->GetGroup() != pLibPortTemp->GetGroup() ||
               pLibPort->GetPortType() != pLibPortTemp->GetPortType() ||
               pLibPort->GetPortDirection() != pLibPortTemp->GetPortDirection() ||
               pLibPort->GetFiberPlug() != pLibPortTemp->GetFiberPlug() ||
               pLibPort->GetFiberMode() != pLibPortTemp->GetFiberMode() ||
               pLibPort->GetCascade() != pLibPortTemp->GetCascade())
            {
                pLibPort->SetName(pLibPortTemp->GetName());
                pLibPort->SetGroup(pLibPortTemp->GetGroup());
                pLibPort->SetPortType(pLibPortTemp->GetPortType());
                pLibPort->SetPortDirection(pLibPortTemp->GetPortDirection());
                pLibPort->SetFiberPlug(pLibPortTemp->GetFiberPlug());
                pLibPort->SetFiberMode(pLibPortTemp->GetFiberMode());
                pLibPort->SetCascade(pLibPortTemp->GetCascade());

                if(bNotify)
                    emit sigObjectPropertyChanged(pLibPort);
            }

            return pLibPort;
        }
        break;

        case PbBaseObject::botTpBay:
        {
            PbTpBay *pTpBayTemp = qobject_cast<PbTpBay*>(const_cast<PbBaseObject*>(&BaseObject));
            PbTpBay *pTpBay = FindTpBayById(BaseObject.GetId());
            if(!pTpBayTemp || !pTpBay)
                return 0;

            if(pTpBay->GetName() != pTpBayTemp->GetName() ||
               pTpBay->GetVLevel() != pTpBayTemp->GetVLevel())
            {
                pTpBay->SetName(pTpBayTemp->GetName());
                pTpBay->SetVLevel(pTpBayTemp->GetVLevel());

                if(bNotify)
                    emit sigObjectPropertyChanged(pTpBay);
            }

            return pTpBay;
        }
        break;

        case PbBaseObject::botTpDevice:
        {
            PbTpDevice *pTpDeviceTemp = qobject_cast<PbTpDevice*>(const_cast<PbBaseObject*>(&BaseObject));
            PbTpDevice *pTpDevice = FindTpDeviceById(BaseObject.GetId());
            if(!pTpDeviceTemp || !pTpDevice || pTpDevice->GetParentBay() != pTpDeviceTemp->GetParentBay())
                return 0;

            if(pTpDevice->GetName() != pTpDeviceTemp->GetName() ||
               pTpDevice->GetDeviceType() != pTpDeviceTemp->GetDeviceType())
            {
                pTpDevice->SetName(pTpDeviceTemp->GetName());
                pTpDevice->SetDeviceType(pTpDeviceTemp->GetDeviceType());

                if(bNotify)
                    emit sigObjectPropertyChanged(pTpDevice);
            }

            return pTpDevice;
        }
        break;

        case PbBaseObject::botTpPort:
        {
            PbTpPort *pTpPortTemp = qobject_cast<PbTpPort*>(const_cast<PbBaseObject*>(&BaseObject));
            PbTpPort *pTpPort = FindTpPortById(BaseObject.GetId());
            if(!pTpPortTemp || !pTpPort || pTpPort->GetParentDevice() != pTpPortTemp->GetParentDevice())
                return 0;

            if(pTpPort->GetName() != pTpPortTemp->GetName() ||
               pTpPort->GetGroup() != pTpPortTemp->GetGroup() ||
               pTpPort->GetPortDirection() != pTpPortTemp->GetPortDirection())
            {
                pTpPort->SetName(pTpPortTemp->GetName());
                pTpPort->SetGroup(pTpPortTemp->GetGroup());
                pTpPort->SetPortDirection(pTpPortTemp->GetPortDirection());

                if(bNotify)
                    emit sigObjectPropertyChanged(pTpPort);
            }

            return pTpPort;
        }
        break;

        case PbBaseObject::botTpInfoSet:
        {
            PbTpInfoSet *pTpInfoSetTemp = qobject_cast<PbTpInfoSet*>(const_cast<PbBaseObject*>(&BaseObject));
            PbTpInfoSet *pTpInfoSet = FindTpInfoSetById(BaseObject.GetId());
            if(!pTpInfoSetTemp || !pTpInfoSet || pTpInfoSet->GetParentBay() != pTpInfoSetTemp->GetParentBay())
                return 0;

            if(pTpInfoSet->GetName() != pTpInfoSetTemp->GetName() ||
               pTpInfoSet->GetInfoSetType() != pTpInfoSetTemp->GetInfoSetType() ||
               pTpInfoSet->GetTxIED() != pTpInfoSetTemp->GetTxIED() ||
               pTpInfoSet->GetTxPort() != pTpInfoSetTemp->GetTxPort() ||
               pTpInfoSet->GetRxIED() != pTpInfoSetTemp->GetRxIED() ||
               pTpInfoSet->GetRxPort() != pTpInfoSetTemp->GetRxPort() ||
               pTpInfoSet->GetSwitch1() != pTpInfoSetTemp->GetSwitch1() ||
               pTpInfoSet->GetSwitch1TxPort() != pTpInfoSetTemp->GetSwitch1TxPort() ||
               pTpInfoSet->GetSwitch1RxPort() != pTpInfoSetTemp->GetSwitch1RxPort() ||
               pTpInfoSet->GetSwitch2() != pTpInfoSetTemp->GetSwitch2() ||
               pTpInfoSet->GetSwitch2TxPort() != pTpInfoSetTemp->GetSwitch2TxPort() ||
               pTpInfoSet->GetSwitch2RxPort() != pTpInfoSetTemp->GetSwitch2RxPort() ||
               pTpInfoSet->GetSwitch3() != pTpInfoSetTemp->GetSwitch3() ||
               pTpInfoSet->GetSwitch3TxPort() != pTpInfoSetTemp->GetSwitch3TxPort() ||
               pTpInfoSet->GetSwitch3RxPort() != pTpInfoSetTemp->GetSwitch3RxPort() ||
               pTpInfoSet->GetSwitch4() != pTpInfoSetTemp->GetSwitch4() ||
               pTpInfoSet->GetSwitch4TxPort() != pTpInfoSetTemp->GetSwitch4TxPort() ||
               pTpInfoSet->GetSwitch4RxPort() != pTpInfoSetTemp->GetSwitch4RxPort())
            {
                pTpInfoSet->SetName(pTpInfoSetTemp->GetName());
                pTpInfoSet->SetInfoSetType(pTpInfoSetTemp->GetInfoSetType());
                pTpInfoSet->SetGroup(pTpInfoSetTemp->GetGroup());
                pTpInfoSet->SetTxIED(pTpInfoSetTemp->GetTxIED());
                pTpInfoSet->SetTxPort(pTpInfoSetTemp->GetTxPort());
                pTpInfoSet->SetRxIED(pTpInfoSetTemp->GetRxIED());
                pTpInfoSet->SetRxPort(pTpInfoSetTemp->GetRxPort());
                pTpInfoSet->SetSwitch1(pTpInfoSetTemp->GetSwitch1());
                pTpInfoSet->SetSwitch1TxPort(pTpInfoSetTemp->GetSwitch1TxPort());
                pTpInfoSet->SetSwitch1RxPort(pTpInfoSetTemp->GetSwitch1RxPort());
                pTpInfoSet->SetSwitch2(pTpInfoSetTemp->GetSwitch2());
                pTpInfoSet->SetSwitch2TxPort(pTpInfoSetTemp->GetSwitch2TxPort());
                pTpInfoSet->SetSwitch2RxPort(pTpInfoSetTemp->GetSwitch2RxPort());
                pTpInfoSet->SetSwitch3(pTpInfoSetTemp->GetSwitch3());
                pTpInfoSet->SetSwitch3TxPort(pTpInfoSetTemp->GetSwitch3TxPort());
                pTpInfoSet->SetSwitch3RxPort(pTpInfoSetTemp->GetSwitch3RxPort());
                pTpInfoSet->SetSwitch4(pTpInfoSetTemp->GetSwitch4());
                pTpInfoSet->SetSwitch4TxPort(pTpInfoSetTemp->GetSwitch4TxPort());
                pTpInfoSet->SetSwitch4RxPort(pTpInfoSetTemp->GetSwitch4RxPort());

                if(bNotify)
                    emit sigObjectPropertyChanged(pTpInfoSet);
            }

            return pTpInfoSet;
        }
        break;
    }

    return 0;
}

bool BaseManager::DeleteObject(PbBaseObject *pBaseObject, bool bNotify )
{
    if(!pBaseObject)
        return false;

    const PbBaseObject::ObjectType &eObjectType = pBaseObject->GetObjectType();
    switch(eObjectType)
    {
        case PbBaseObject::botDeviceCategory:
        {
            PbDeviceCategory *pDeviceCategory = qobject_cast<PbDeviceCategory*>(pBaseObject);
            if(!pDeviceCategory)
                return false;

            if(bNotify)
                emit sigObjectAboutToBeDeleted(pDeviceCategory);

            m_d->m_mapIdToDeviceCategories.remove(pDeviceCategory->GetId());
            delete pDeviceCategory;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case PbBaseObject::botBayCategory:
        {
            PbBayCategory *pBayCategory = qobject_cast<PbBayCategory*>(pBaseObject);
            if(!pBayCategory)
                return false;

            if(bNotify)
                emit sigObjectAboutToBeDeleted(pBayCategory);

            m_d->m_mapIdToBayCategories.remove(pBayCategory->GetId());
            delete pBayCategory;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case PbBaseObject::botVLevel:
        {
            PbVLevel *pVLevel = qobject_cast<PbVLevel*>(pBaseObject);
            if(!pVLevel)
                return false;

            if(bNotify)
                emit sigObjectAboutToBeDeleted(pVLevel);

            m_d->m_mapIdToVLevels.remove(pVLevel->GetId());
            delete pVLevel;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case PbBaseObject::botManufacture:
        {
            PbManufacture *pManufacture = qobject_cast<PbManufacture*>(pBaseObject);
            if(!pManufacture)
                return false;

            if(bNotify)
                emit sigObjectAboutToBeDeleted(pManufacture);

            m_d->m_mapIdToManufactures.remove(pManufacture->GetId());
            delete pManufacture;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case PbBaseObject::botDeviceType:
        {
            PbDeviceType *pDeviceType = qobject_cast<PbDeviceType*>(pBaseObject);
            if(!pDeviceType)
                return false;

            if(bNotify)
                emit sigObjectAboutToBeDeleted(pDeviceType);

            m_d->m_mapIdToDeviceTypes.remove(pDeviceType->GetId());
            delete pDeviceType;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case PbBaseObject::botLibDevice:
        {
            PbLibDevice *pLibDevice = qobject_cast<PbLibDevice*>(pBaseObject);
            if(!pLibDevice)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete child boards
            /////////////////////////////////////////////////////////////////
            foreach(PbLibBoard *pLibBoard, pLibDevice->GetChildBoards())
                DeleteObject(pLibBoard, bNotify);

            /////////////////////////////////////////////////////////////////
            // Delete current device
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pLibDevice);

            // Delete current device
            m_d->m_mapIdToLibDevices.remove(pLibDevice->GetId());
            delete pLibDevice;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case PbBaseObject::botLibBoard:
        {
            PbLibBoard *pLibBoard = qobject_cast<PbLibBoard*>(pBaseObject);
            if(!pLibBoard)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete child ports
            /////////////////////////////////////////////////////////////////
            foreach(PbLibPort *pLibPort, pLibBoard->GetChildPorts())
                DeleteObject(pLibPort, bNotify);

            /////////////////////////////////////////////////////////////////
            // Delete current board
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pLibBoard);

            // Remove current board from parent device
            if(PbLibDevice *pLibDevice = pLibBoard->GetParentDevice())
                pLibDevice->RemoveChildBoard(pLibBoard);

            // Delete current board
            m_d->m_mapIdToLibBoards.remove(pLibBoard->GetId());
            delete pLibBoard;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case PbBaseObject::botLibPort:
        {
            PbLibPort *pLibPort = qobject_cast<PbLibPort*>(pBaseObject);
            if(!pLibPort)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete current port
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pLibPort);

            // Remove current port from parent board
            if(PbLibBoard *pLibBoard = pLibPort->GetParentBoard())
                pLibBoard->RemoveChildPort(pLibPort);

            // Delete current port
            m_d->m_mapIdToLibPorts.remove(pLibPort->GetId());
            delete pLibPort;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case PbBaseObject::botTpBay:
        {
            PbTpBay *pTpBay = qobject_cast<PbTpBay*>(pBaseObject);
            if(!pTpBay)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete child infosets
            /////////////////////////////////////////////////////////////////
            foreach(PbTpInfoSet *pTpInfoSet, pTpBay->GetChildInfoSets())
                DeleteObject(pTpInfoSet, bNotify);

            /////////////////////////////////////////////////////////////////
            // Delete child devices
            /////////////////////////////////////////////////////////////////
            foreach(PbTpDevice *pTpDevice, pTpBay->GetChildDevices())
                DeleteObject(pTpDevice, bNotify);

            /////////////////////////////////////////////////////////////////
            // Delete current bay
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pTpBay);

            // Delete current bay
            m_d->m_mapIdToTpBays.remove(pTpBay->GetId());
            delete pTpBay;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case PbBaseObject::botTpDevice:
        {
            PbTpDevice *pTpDevice = qobject_cast<PbTpDevice*>(pBaseObject);
            if(!pTpDevice)
                return false;

            /////////////////////////////////////////////////////////////////
            // Update all information set associated with current device
            /////////////////////////////////////////////////////////////////
            foreach(PbTpInfoSet *pTpInfoSet, m_d->m_mapIdToTpInfoSets.values())
            {
                if(pTpInfoSet->GetTxIED() == pTpDevice || pTpInfoSet->GetRxIED() == pTpDevice)
                {
                    DeleteObject(pTpInfoSet, bNotify);
                }
                else if(pTpInfoSet->GetSwitch1() == pTpDevice)
                {
                    PbTpInfoSet TpInfoSet(*pTpInfoSet);
                    TpInfoSet.SetSwitch1(0);
                    TpInfoSet.SetSwitch1TxPort(0);
                    TpInfoSet.SetSwitch1RxPort(0);
                    TpInfoSet.SetSwitch2(0);
                    TpInfoSet.SetSwitch2TxPort(0);
                    TpInfoSet.SetSwitch2RxPort(0);
                    TpInfoSet.SetSwitch3(0);
                    TpInfoSet.SetSwitch3TxPort(0);
                    TpInfoSet.SetSwitch3RxPort(0);
                    TpInfoSet.SetSwitch4(0);
                    TpInfoSet.SetSwitch4TxPort(0);
                    TpInfoSet.SetSwitch4RxPort(0);

                    UpdateObject(TpInfoSet, bNotify);
                }
                else if(pTpInfoSet->GetSwitch2() == pTpDevice)
                {
                    PbTpInfoSet TpInfoSet(*pTpInfoSet);
                    TpInfoSet.SetSwitch2(0);
                    TpInfoSet.SetSwitch2TxPort(0);
                    TpInfoSet.SetSwitch2RxPort(0);
                    TpInfoSet.SetSwitch3(0);
                    TpInfoSet.SetSwitch3TxPort(0);
                    TpInfoSet.SetSwitch3RxPort(0);
                    TpInfoSet.SetSwitch4(0);
                    TpInfoSet.SetSwitch4TxPort(0);
                    TpInfoSet.SetSwitch4RxPort(0);

                    UpdateObject(TpInfoSet, bNotify);
                }
                else if(pTpInfoSet->GetSwitch3() == pTpDevice)
                {
                    PbTpInfoSet TpInfoSet(*pTpInfoSet);
                    TpInfoSet.SetSwitch3(0);
                    TpInfoSet.SetSwitch3TxPort(0);
                    TpInfoSet.SetSwitch3RxPort(0);
                    TpInfoSet.SetSwitch4(0);
                    TpInfoSet.SetSwitch4TxPort(0);
                    TpInfoSet.SetSwitch4RxPort(0);

                    UpdateObject(TpInfoSet, bNotify);
                }
                else if(pTpInfoSet->GetSwitch4() == pTpDevice)
                {
                    PbTpInfoSet TpInfoSet(*pTpInfoSet);
                    TpInfoSet.SetSwitch4(0);
                    TpInfoSet.SetSwitch4TxPort(0);
                    TpInfoSet.SetSwitch4RxPort(0);

                    UpdateObject(TpInfoSet, bNotify);
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete child ports
            /////////////////////////////////////////////////////////////////
            foreach(PbTpPort *pTpPort, pTpDevice->GetChildPorts())
                DeleteObject(pTpPort, bNotify);

            /////////////////////////////////////////////////////////////////
            // Delete current device
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pTpDevice);

            // Remove current device from parent bay
            if(PbTpBay *pTpBay = pTpDevice->GetParentBay())
                pTpBay->RemoveChildDevice(pTpDevice);

            // Delete current device
            m_d->m_mapIdToTpDevices.remove(pTpDevice->GetId());
            delete pTpDevice;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case PbBaseObject::botTpPort:
        {
            PbTpPort *pTpPort = qobject_cast<PbTpPort*>(pBaseObject);
            if(!pTpPort)
                return false;

            /////////////////////////////////////////////////////////////////
            // Update all information sets associated with current port
            /////////////////////////////////////////////////////////////////
            foreach(PbTpInfoSet *pTpInfoSet, m_d->m_mapIdToTpInfoSets.values())
            {
                PbTpInfoSet TpInfoSet(*pTpInfoSet);
                if(TpInfoSet.ReplacePort(pTpPort, 0))
                    UpdateObject(TpInfoSet, bNotify);
            }

            /////////////////////////////////////////////////////////////////
            // Delete current port
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pTpPort);

            // Remove current port from parent board
            if(PbTpDevice *pTpDevice = pTpPort->GetParentDevice())
                pTpDevice->RemoveChildPort(pTpPort);

            // Delete current port
            m_d->m_mapIdToTpPorts.remove(pTpPort->GetId());
            delete pTpPort;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        case PbBaseObject::botTpInfoSet:
        {
            PbTpInfoSet *pTpInfoSet = qobject_cast<PbTpInfoSet*>(pBaseObject);
            if(!pTpInfoSet)
                return false;

            /////////////////////////////////////////////////////////////////
            // Delete current information set
            /////////////////////////////////////////////////////////////////
            if(bNotify)
                emit sigObjectAboutToBeDeleted(pTpInfoSet);

            // Remove current infoset from parent bay
            if(PbTpBay *pTpBay = pTpInfoSet->GetParentBay())
                pTpBay->RemoveChildInfoSet(pTpInfoSet);

            // Delete current infoset
            m_d->m_mapIdToTpInfoSets.remove(pTpInfoSet->GetId());
            delete pTpInfoSet;

            if(bNotify)
                emit sigObjectDeleted();
        }
        break;

        default:
            return false;
    }

    return true;
}

QList<PbDeviceCategory*> BaseManager::GetAllDeviceCategories() const
{
    return m_d->m_mapIdToDeviceCategories.values();
}

QList<PbBayCategory*> BaseManager::GetAllBayCategories() const
{
    return m_d->m_mapIdToBayCategories.values();
}

QList<PbVLevel*> BaseManager::GetAllVLevels() const
{
    return m_d->m_mapIdToVLevels.values();
}

QList<PbManufacture*> BaseManager::GetAllManufactures() const
{
    return m_d->m_mapIdToManufactures.values();
}

QList<PbDeviceType*> BaseManager::GetAllDeviceTypes() const
{
    return m_d->m_mapIdToDeviceTypes.values();
}

QList<PbLibDevice*> BaseManager::GetAllLibDevices() const
{
    return m_d->m_mapIdToLibDevices.values();
}

QList<PbLibBoard*> BaseManager::GetAllLibBoards() const
{
    return m_d->m_mapIdToLibBoards.values();
}

QList<PbLibPort*> BaseManager::GetAllLibPorts() const
{
    return m_d->m_mapIdToLibPorts.values();
}

QList<PbTpBay*> BaseManager::GetAllTpBays() const
{
    return m_d->m_mapIdToTpBays.values();
}

QList<PbTpDevice*> BaseManager::GetAllTpDevices() const
{
    return m_d->m_mapIdToTpDevices.values();
}

QList<PbTpPort*> BaseManager::GetAllTpPorts() const
{
    return m_d->m_mapIdToTpPorts.values();
}

QList<PbTpInfoSet*> BaseManager::GetAllTpInfoSets() const
{
    return m_d->m_mapIdToTpInfoSets.values();
}

PbDeviceCategory* BaseManager::FindDeviceCategoryById(int iId) const
{
    return m_d->m_mapIdToDeviceCategories.value(iId, 0);
}

PbBayCategory* BaseManager::FindBayCategoryById(int iId) const
{
    return m_d->m_mapIdToBayCategories.value(iId, 0);
}

PbVLevel* BaseManager::FindVLevelById(int iId) const
{
    return m_d->m_mapIdToVLevels.value(iId, 0);
}

PbManufacture* BaseManager::FindManufactureById(int iId) const
{
    return m_d->m_mapIdToManufactures.value(iId, 0);
}

PbDeviceType* BaseManager::FindDeviceTypeById(int iId) const
{
    return m_d->m_mapIdToDeviceTypes.value(iId, 0);
}

PbLibDevice* BaseManager::FindLibDeviceById(int iId) const
{
    return m_d->m_mapIdToLibDevices.value(iId, 0);
}

PbLibBoard* BaseManager::FindLibBoardById(int iId) const
{
    return m_d->m_mapIdToLibBoards.value(iId, 0);
}

PbLibPort* BaseManager::FindLibPortById(int iId) const
{
    return m_d->m_mapIdToLibPorts.value(iId, 0);
}

PbTpBay* BaseManager::FindTpBayById(int iId) const
{
    return m_d->m_mapIdToTpBays.value(iId, 0);
}

PbTpDevice* BaseManager::FindTpDeviceById(int iId) const
{
    return m_d->m_mapIdToTpDevices.value(iId, 0);
}

PbTpPort* BaseManager::FindTpPortById(int iId) const
{
    return m_d->m_mapIdToTpPorts.value(iId, 0);
}

PbTpInfoSet* BaseManager::FindTpInfoSetById(int iId) const
{
    return m_d->m_mapIdToTpInfoSets.value(iId, 0);
}

bool BaseManager::DbCreateObject(PbBaseObject &BaseObject)
{
    QString strSql;
    const PbBaseObject::ObjectType &eObjectType = BaseObject.GetObjectType();
    switch(eObjectType)
    {
        case PbBaseObject::botDeviceCategory:
        {
            PbDeviceCategory *pDeviceCategory = qobject_cast<PbDeviceCategory*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pDeviceCategory)
                return false;

            strSql = QString("INSERT INTO base_devicecategory(`name`,`code`) "
                             "VALUES('%1','%2')").arg(pDeviceCategory->GetName())
                                                 .arg(pDeviceCategory->GetCode());
        }
        break;

        case PbBaseObject::botBayCategory:
        {
            PbBayCategory *pBayCategory = qobject_cast<PbBayCategory*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pBayCategory)
                return false;

            strSql = QString("INSERT INTO base_baycategory(`name`,`code`) "
                             "VALUES('%1','%2')").arg(pBayCategory->GetName())
                                                 .arg(pBayCategory->GetCode());
        }
        break;

        case PbBaseObject::botVLevel:
        {
            PbVLevel *pVLevel = qobject_cast<PbVLevel*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pVLevel)
                return false;

            strSql = QString("INSERT INTO base_vlevel(`vlevel`,`code`) "
                             "VALUES('%1','%2')").arg(pVLevel->GetVLevel())
                                                 .arg(pVLevel->GetCode());
        }
        break;

        case PbBaseObject::botManufacture:
        {
            PbManufacture *pManufacture = qobject_cast<PbManufacture*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pManufacture)
                return false;

            strSql = QString("INSERT INTO base_manufacture(`name`) "
                             "VALUES('%1')").arg(pManufacture->GetName());
        }
        break;

        case PbBaseObject::botDeviceType:
        {
            PbDeviceType *pDeviceType = qobject_cast<PbDeviceType*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pDeviceType)
                return false;

            strSql = QString("INSERT INTO base_devicetype(`name`) "
                             "VALUES('%1')").arg(pDeviceType->GetName());
        }
        break;

        case PbBaseObject::botLibDevice:
        {
            PbLibDevice *pLibDevice = qobject_cast<PbLibDevice*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pLibDevice)
                return false;

            strSql = QString("INSERT INTO lib_device(`device_type`,`manufacture`,`device_category`,`bay_category`,`type`,`icd_filename`) "
                             "VALUES('%1','%2','%3','%4','%5','%6')").arg(pLibDevice->GetDeviceType())
                                                                     .arg(pLibDevice->GetManufacture())
                                                                     .arg(pLibDevice->GetDeviceCategory())
                                                                     .arg(pLibDevice->GetBayCategory())
                                                                     .arg(pLibDevice->GetType())
                                                                     .arg(pLibDevice->GetIcdFileName());
        }
        break;

        case PbBaseObject::botLibBoard:
        {
            PbLibBoard *pLibBoard = qobject_cast<PbLibBoard*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pLibBoard)
                return false;

            PbLibDevice *pParentDevice = pLibBoard->GetParentDevice();
            if(!pParentDevice)
                return false;

            strSql = QString("INSERT INTO lib_board(`device_id`,`position`,`description`,`type`) "
                             "VALUES('%1','%2','%3','%4')").arg(pParentDevice->GetId())
                                                           .arg(pLibBoard->GetPosition())
                                                           .arg(pLibBoard->GetDescription())
                                                           .arg(pLibBoard->GetType());
        }
        break;

        case PbBaseObject::botLibPort:
        {
            PbLibPort *pLibPort = qobject_cast<PbLibPort*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pLibPort)
                return false;

            PbLibBoard *pParentBoard = pLibPort->GetParentBoard();
            if(!pParentBoard)
                return false;

            strSql = QString("INSERT INTO lib_port(`board_id`,`name`,`group`,`type`,`direction`,`fiber_plug`,`fiber_mode`,`cascade`) "
                             "VALUES('%1','%2','%3','%4','%5','%6','%7','%8')").arg(pParentBoard->GetId())
                                                                               .arg(pLibPort->GetName())
                                                                               .arg(pLibPort->GetGroup())
                                                                               .arg(int(pLibPort->GetPortType()))
                                                                               .arg(int(pLibPort->GetPortDirection()))
                                                                               .arg(int(pLibPort->GetFiberPlug()))
                                                                               .arg(int(pLibPort->GetFiberMode()))
                                                                               .arg(int(pLibPort->GetCascade()));
        }
        break;

        case PbBaseObject::botTpBay:
        {
            PbTpBay *pTpBay = qobject_cast<PbTpBay*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pTpBay)
                return false;

            strSql = QString("INSERT INTO tp_bay(`name`,`vlevel`) "
                             "VALUES('%1','%2')").arg(pTpBay->GetName())
                                                 .arg(pTpBay->GetVLevel());
        }
        break;

        case PbBaseObject::botTpDevice:
        {
            PbTpDevice *pTpDevice = qobject_cast<PbTpDevice*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pTpDevice)
                return false;

            PbTpBay *pParentBay = pTpDevice->GetParentBay();
            if(!pParentBay)
                return false;

            strSql = QString("INSERT INTO tp_device(`bay_id`,`name`,`device_type`) "
                             "VALUES('%1','%2','%3')").arg(pParentBay->GetId())
                                                      .arg(pTpDevice->GetName())
                                                      .arg(pTpDevice->GetDeviceType());
        }
        break;

        case PbBaseObject::botTpPort:
        {
            PbTpPort *pTpPort = qobject_cast<PbTpPort*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pTpPort)
                return false;

            PbTpDevice *pParentDevice = pTpPort->GetParentDevice();
            if(!pParentDevice)
                return false;

            strSql = QString("INSERT INTO tp_port(`device_id`,`name`,`group`,`direction`) "
                             "VALUES('%1','%2','%3','%4')").arg(pParentDevice->GetId())
                                                           .arg(pTpPort->GetName())
                                                           .arg(pTpPort->GetGroup())
                                                           .arg(int(pTpPort->GetPortDirection()));
        }
        break;

        case PbBaseObject::botTpInfoSet:
        {
            PbTpInfoSet *pTpInfoSet = qobject_cast<PbTpInfoSet*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pTpInfoSet)
                return false;

            PbTpBay *pTpBay = pTpInfoSet->GetParentBay();
            if(!pTpBay)
                return false;

            strSql = QString("INSERT INTO tp_infoset(`bay_id`,`name`,`type`,`group`,`txied_id`,`txiedport_id`,`rxied_id`,`rxiedport_id`,`switch1_id`,`switch1_txport_id`,`switch1_rxport_id`,`switch2_id`,`switch2_txport_id`,`switch2_rxport_id`,`switch3_id`,`switch3_txport_id`,`switch3_rxport_id`,`switch4_id`,`switch4_txport_id`,`switch4_rxport_id`) "
                             "VALUES('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13','%14','%15','%16','%17','%18','%19','%20')").arg(pTpBay->GetId())
                                                                                                                                                      .arg(pTpInfoSet->GetName())
                                                                                                                                                      .arg(pTpInfoSet->GetInfoSetType())
                                                                                                                                                      .arg(pTpInfoSet->GetGroup())
                                                                                                                                                      .arg(pTpInfoSet->GetTxIED() ? pTpInfoSet->GetTxIED()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetTxPort() ? pTpInfoSet->GetTxPort()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetRxIED() ? pTpInfoSet->GetRxIED()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetRxPort() ? pTpInfoSet->GetRxPort()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetSwitch1() ? pTpInfoSet->GetSwitch1()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetSwitch1TxPort() ? pTpInfoSet->GetSwitch1TxPort()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetSwitch1RxPort() ? pTpInfoSet->GetSwitch1RxPort()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetSwitch2() ? pTpInfoSet->GetSwitch2()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetSwitch2TxPort() ? pTpInfoSet->GetSwitch2TxPort()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetSwitch2RxPort() ? pTpInfoSet->GetSwitch2RxPort()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetSwitch3() ? pTpInfoSet->GetSwitch3()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetSwitch3TxPort() ? pTpInfoSet->GetSwitch3TxPort()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetSwitch3RxPort() ? pTpInfoSet->GetSwitch3RxPort()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetSwitch4() ? pTpInfoSet->GetSwitch4()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetSwitch4TxPort() ? pTpInfoSet->GetSwitch4TxPort()->GetId() : 0)
                                                                                                                                                      .arg(pTpInfoSet->GetSwitch4RxPort() ? pTpInfoSet->GetSwitch4RxPort()->GetId() : 0);
        }
        break;
    }

    if(strSql.isEmpty())
        return false;

    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    if(!pDatabase->ExecSql(strSql))
        return false;

    int iReturnId = pDatabase->GetInsertId();
    if(iReturnId <= PbBaseObject::m_iInvalidObjectId)
        return false;

    BaseObject.SetId(iReturnId);
    return true;
}

bool BaseManager::DbUpdateObject(const PbBaseObject &BaseObject)
{
    QString strSql;
    const PbBaseObject::ObjectType &eObjectType = BaseObject.GetObjectType();
    switch(eObjectType)
    {
        case PbBaseObject::botDeviceCategory:
        {
            PbDeviceCategory *pDeviceCategory = qobject_cast<PbDeviceCategory*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pDeviceCategory)
                return false;

            strSql = QString("UPDATE base_devicecategory SET `name`='%1',`code`='%2' WHERE `category_id`='%3'")
                                                                .arg(pDeviceCategory->GetName())
                                                                .arg(pDeviceCategory->GetCode())
                                                                .arg(pDeviceCategory->GetId());
        }
        break;

        case PbBaseObject::botBayCategory:
        {
            PbBayCategory *pBayCategory = qobject_cast<PbBayCategory*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pBayCategory)
                return false;

            strSql = QString("UPDATE base_baycategory SET `name`='%1',`code`='%2' WHERE `category_id`='%3'")
                                                                .arg(pBayCategory->GetName())
                                                                .arg(pBayCategory->GetCode())
                                                                .arg(pBayCategory->GetId());
        }
        break;

        case PbBaseObject::botVLevel:
        {
            PbVLevel *pVLevel = qobject_cast<PbVLevel*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pVLevel)
                return false;

            strSql = QString("UPDATE base_vlevel SET `vlevel`='%1',`code`='%2' WHERE `vlevel_id`='%3'")
                                                                .arg(pVLevel->GetVLevel())
                                                                .arg(pVLevel->GetCode())
                                                                .arg(pVLevel->GetId());
        }
        break;

        case PbBaseObject::botManufacture:
        {
            PbManufacture *pManufacture = qobject_cast<PbManufacture*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pManufacture)
                return false;

            strSql = QString("UPDATE base_manufacture SET `name`='%1' WHERE `manufacture_id`='%2'")
                                                                .arg(pManufacture->GetName())
                                                                .arg(pManufacture->GetId());
        }
        break;

        case PbBaseObject::botDeviceType:
        {
            PbDeviceType *pDeviceType = qobject_cast<PbDeviceType*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pDeviceType)
                return false;

            strSql = QString("UPDATE base_devicetype SET `name`='%1' WHERE `type_id`='%2'")
                                                                .arg(pDeviceType->GetName())
                                                                .arg(pDeviceType->GetId());
        }
        break;

        case PbBaseObject::botLibDevice:
        {
            PbLibDevice *pLibDevice = qobject_cast<PbLibDevice*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pLibDevice)
                return false;

            strSql = QString("UPDATE lib_device SET `device_type`='%1',`manufacture`='%2',`device_category`='%3',`bay_category`='%4',`type`='%5',`icd_filename`='%6' WHERE `device_id`='%7'")
                                                                .arg(pLibDevice->GetDeviceType())
                                                                .arg(pLibDevice->GetManufacture())
                                                                .arg(pLibDevice->GetDeviceCategory())
                                                                .arg(pLibDevice->GetBayCategory())
                                                                .arg(pLibDevice->GetType())
                                                                .arg(pLibDevice->GetIcdFileName())
                                                                .arg(pLibDevice->GetId());
        }
        break;

        case PbBaseObject::botLibBoard:
        {
            PbLibBoard *pLibBoard = qobject_cast<PbLibBoard*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pLibBoard)
                return false;

            strSql = QString("UPDATE lib_board SET `device_id`='%1',`position`='%2',`description`='%3',`type`='%4' WHERE `board_id`='%5'")
                                                                .arg(pLibBoard->GetParentDevice()->GetId())
                                                                .arg(pLibBoard->GetPosition())
                                                                .arg(pLibBoard->GetDescription())
                                                                .arg(pLibBoard->GetType())
                                                                .arg(pLibBoard->GetId());
        }
        break;

        case PbBaseObject::botLibPort:
        {
            PbLibPort *pLibPort = qobject_cast<PbLibPort*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pLibPort)
                return false;

            strSql = QString("UPDATE lib_port SET `board_id`='%1',`name`='%2',`group`='%3',`type`='%4',`direction`='%5',`fiber_plug`='%6',`fiber_mode`='%7',`cascade`='%8' WHERE `port_id`='%9'")
                                                                .arg(pLibPort->GetParentBoard()->GetId())
                                                                .arg(pLibPort->GetName())
                                                                .arg(pLibPort->GetGroup())
                                                                .arg(int(pLibPort->GetPortType()))
                                                                .arg(int(pLibPort->GetPortDirection()))
                                                                .arg(int(pLibPort->GetFiberPlug()))
                                                                .arg(int(pLibPort->GetFiberMode()))
                                                                .arg(int(pLibPort->GetCascade()))
                                                                .arg(pLibPort->GetId());
        }
        break;

        case PbBaseObject::botTpBay:
        {
            PbTpBay *pTpBay = qobject_cast<PbTpBay*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pTpBay)
                return false;

            strSql = QString("UPDATE tp_bay SET `name`='%1',`vlevel`='%2' WHERE `bay_id`='%3'")
                                                                .arg(pTpBay->GetName())
                                                                .arg(pTpBay->GetVLevel())
                                                                .arg(pTpBay->GetId());
        }
        break;

        case PbBaseObject::botTpDevice:
        {
            PbTpDevice *pTpDevice = qobject_cast<PbTpDevice*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pTpDevice)
                return false;

            strSql = QString("UPDATE tp_device SET `bay_id`='%1',`name`='%2',`device_type`='%3' WHERE `device_id`='%4'")
                                                                .arg(pTpDevice->GetParentBay()->GetId())
                                                                .arg(pTpDevice->GetName())
                                                                .arg(pTpDevice->GetDeviceType())
                                                                .arg(pTpDevice->GetId());
        }
        break;

        case PbBaseObject::botTpPort:
        {
            PbTpPort *pTpPort = qobject_cast<PbTpPort*>(const_cast<PbBaseObject*>(&BaseObject));
            if(!pTpPort)
                return false;

            strSql = QString("UPDATE tp_port SET `device_id`='%1',`name`='%2',`group`='%3',`direction`='%4' WHERE `port_id`='%5'")
                                                                .arg(pTpPort->GetParentDevice()->GetId())
                                                                .arg(pTpPort->GetName())
                                                                .arg(pTpPort->GetGroup())
                                                                .arg(int(pTpPort->GetPortDirection()))
                                                                .arg(pTpPort->GetId());
        }
        break;

        case PbBaseObject::botTpInfoSet:
        {
            PbTpInfoSet *pTpInfoSet = qobject_cast<PbTpInfoSet*>(const_cast<PbBaseObject*>(&BaseObject));
            strSql = QString("UPDATE tp_infoset SET `bay_id`='%1',`name`='%2',`type`='%3',`group`='%4',`txied_id`='%5',`txiedport_id`='%6',`rxied_id`='%7',`rxiedport_id`='%8',`switch1_id`='%9',`switch1_txport_id`='%10',`switch1_rxport_id`='%11',`switch2_id`='%12',`switch2_txport_id`='%13',`switch2_rxport_id`='%14',`switch3_id`='%15',`switch3_txport_id`='%16',`switch3_rxport_id`='%17',`switch4_id`='%18',`switch4_txport_id`='%19',`switch4_rxport_id`='%20' WHERE `infoset_id`='%21'")
                                                                .arg(pTpInfoSet->GetParentBay()->GetId())
                                                                .arg(pTpInfoSet->GetName())
                                                                .arg(pTpInfoSet->GetInfoSetType())
                                                                .arg(pTpInfoSet->GetGroup())
                                                                .arg(pTpInfoSet->GetTxIED() ? pTpInfoSet->GetTxIED()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetTxPort() ? pTpInfoSet->GetTxPort()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetRxIED() ? pTpInfoSet->GetRxIED()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetRxPort() ? pTpInfoSet->GetRxPort()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetSwitch1() ? pTpInfoSet->GetSwitch1()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetSwitch1TxPort() ? pTpInfoSet->GetSwitch1TxPort()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetSwitch1RxPort() ? pTpInfoSet->GetSwitch1RxPort()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetSwitch2() ? pTpInfoSet->GetSwitch2()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetSwitch2TxPort() ? pTpInfoSet->GetSwitch2TxPort()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetSwitch2RxPort() ? pTpInfoSet->GetSwitch2RxPort()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetSwitch3() ? pTpInfoSet->GetSwitch3()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetSwitch3TxPort() ? pTpInfoSet->GetSwitch3TxPort()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetSwitch3RxPort() ? pTpInfoSet->GetSwitch3RxPort()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetSwitch4() ? pTpInfoSet->GetSwitch4()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetSwitch4TxPort() ? pTpInfoSet->GetSwitch4TxPort()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetSwitch4RxPort() ? pTpInfoSet->GetSwitch4RxPort()->GetId() : 0)
                                                                .arg(pTpInfoSet->GetId());
        }
        break;
    }

    if(strSql.isEmpty())
        return false;

    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    return pDatabase->ExecSql(strSql);
}

bool BaseManager::DbDeleteObject(const PbBaseObject::ObjectType &eObjectType, int iId, bool bUseTransaction)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    MySqlWrapper::MySqlTransaction transaction(pDatabase, bUseTransaction);
    MySqlWrapper::MySqlQuery query(pDatabase);

    switch(eObjectType)
    {
        case PbBaseObject::botDeviceCategory:
        {
            if(!pDatabase->ExecSql(QString("DELETE FROM `base_devicecategory` WHERE `category_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }
        }
        break;

        case PbBaseObject::botBayCategory:
        {
            if(!pDatabase->ExecSql(QString("DELETE FROM `base_baycategory` WHERE `category_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }
        }
        break;

        case PbBaseObject::botVLevel:
        {
            if(!pDatabase->ExecSql(QString("DELETE FROM `base_vlevel` WHERE `vlevel_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }
        }
        break;

        case PbBaseObject::botManufacture:
        {
            if(!pDatabase->ExecSql(QString("DELETE FROM `base_manufacture` WHERE `manufacture_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }
        }
        break;

        case PbBaseObject::botDeviceType:
        {
            if(!pDatabase->ExecSql(QString("DELETE FROM `base_devicetype` WHERE `type_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }
        }
        break;

        case PbBaseObject::botLibDevice:
        {
            /////////////////////////////////////////////////////////////////
            // Delete child boards
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `board_id` FROM `lib_board` WHERE `device_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PbBaseObject::botLibBoard, record.value("board_id").toInt(), false))
                {
                    transaction.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current device
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `lib_device` WHERE `device_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }
        }
        break;

        case PbBaseObject::botLibBoard:
        {
            /////////////////////////////////////////////////////////////////
            // Delete child ports
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `port_id` FROM `lib_port` WHERE `board_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PbBaseObject::botLibPort, record.value("port_id").toInt(), false))
                {
                    transaction.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current board
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `lib_board` WHERE `board_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }
        }
        break;

        case PbBaseObject::botLibPort:
        {
            /////////////////////////////////////////////////////////////////
            // Delete current port
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM lib_port WHERE `port_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }
        }
        break;

        case PbBaseObject::botTpBay:
        {
            /////////////////////////////////////////////////////////////////
            // Delete child infosets
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `infoset_id` FROM `tp_infoset` WHERE `bay_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PbBaseObject::botTpInfoSet, record.value("infoset_id").toInt(), false))
                {
                    transaction.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete child devices
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `device_id` FROM `tp_device` WHERE `bay_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PbBaseObject::botTpDevice, record.value("device_id").toInt(), false))
                {
                    transaction.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current bay
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM tp_bay WHERE `bay_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }
        }
        break;

        case PbBaseObject::botTpDevice:
        {
            /////////////////////////////////////////////////////////////////
            // Update all information sets associated with current device
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `infoset_id` FROM `tp_infoset` WHERE `txied_id`='%1' OR `rxied_id`='%1' OR `switch1_id`='%1' OR `switch2_id`='%1' OR `switch3_id`='%1' OR `switch4_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                PbTpInfoSet *pTpInfoSet = m_d->m_mapIdToTpInfoSets.value(record.value("infoset_id").toInt());
                if(!pTpInfoSet)
                {
                    transaction.Rollback();
                    return false;
                }

                int iTxIED = pTpInfoSet->GetTxIED() ? pTpInfoSet->GetTxIED()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iRxIED = pTpInfoSet->GetRxIED() ? pTpInfoSet->GetRxIED()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iSwitch1 = pTpInfoSet->GetSwitch1() ? pTpInfoSet->GetSwitch1()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iSwitch2 = pTpInfoSet->GetSwitch2() ? pTpInfoSet->GetSwitch2()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iSwitch3 = pTpInfoSet->GetSwitch3() ? pTpInfoSet->GetSwitch3()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iSwitch4 = pTpInfoSet->GetSwitch4() ? pTpInfoSet->GetSwitch4()->GetId() : PbBaseObject::m_iInvalidObjectId;

                if(iTxIED == iId || iRxIED == iId)
                {
                    if(!DbDeleteObject(PbBaseObject::botTpInfoSet, pTpInfoSet->GetId(), false))
                    {
                        transaction.Rollback();
                        return false;
                    }
                }
                else
                {
                    PbTpInfoSet TpInfoSet(*pTpInfoSet);
                    if(iSwitch1 == iId)
                    {
                        TpInfoSet.SetSwitch1(0);
                        TpInfoSet.SetSwitch1TxPort(0);
                        TpInfoSet.SetSwitch1RxPort(0);
                        TpInfoSet.SetSwitch2(0);
                        TpInfoSet.SetSwitch2TxPort(0);
                        TpInfoSet.SetSwitch2RxPort(0);
                        TpInfoSet.SetSwitch3(0);
                        TpInfoSet.SetSwitch3TxPort(0);
                        TpInfoSet.SetSwitch3RxPort(0);
                        TpInfoSet.SetSwitch4(0);
                        TpInfoSet.SetSwitch4TxPort(0);
                        TpInfoSet.SetSwitch4RxPort(0);
                    }
                    else if(iSwitch2 == iId)
                    {
                        TpInfoSet.SetSwitch2(0);
                        TpInfoSet.SetSwitch2TxPort(0);
                        TpInfoSet.SetSwitch2RxPort(0);
                        TpInfoSet.SetSwitch3(0);
                        TpInfoSet.SetSwitch3TxPort(0);
                        TpInfoSet.SetSwitch3RxPort(0);
                        TpInfoSet.SetSwitch4(0);
                        TpInfoSet.SetSwitch4TxPort(0);
                        TpInfoSet.SetSwitch4RxPort(0);
                    }
                    else if(iSwitch3 == iId)
                    {
                        TpInfoSet.SetSwitch3(0);
                        TpInfoSet.SetSwitch3TxPort(0);
                        TpInfoSet.SetSwitch3RxPort(0);
                        TpInfoSet.SetSwitch4(0);
                        TpInfoSet.SetSwitch4TxPort(0);
                        TpInfoSet.SetSwitch4RxPort(0);
                    }
                    else if(iSwitch4 == iId)
                    {
                        TpInfoSet.SetSwitch4(0);
                        TpInfoSet.SetSwitch4TxPort(0);
                        TpInfoSet.SetSwitch4RxPort(0);
                    }

                    if(!DbUpdateObject(TpInfoSet))
                    {
                        transaction.Rollback();
                        return false;
                    }
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete child ports
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `port_id` FROM `tp_port` WHERE `device_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                if(!DbDeleteObject(PbBaseObject::botTpPort, record.value("port_id").toInt(), false))
                {
                    transaction.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current device
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM tp_device WHERE `device_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }
        }
        break;

        case PbBaseObject::botTpPort:
        {
            /////////////////////////////////////////////////////////////////
            // Update all information sets associated with current port
            /////////////////////////////////////////////////////////////////
            query.Clear();
            if(!query.Exec(QString("SELECT `infoset_id` FROM `tp_infoset` WHERE `txiedport_id`='%1' OR `rxiedport_id`='%1' OR `switch1_txport_id`='%1' OR `switch1_rxport_id`='%1' OR `switch2_txport_id`='%1' OR `switch2_rxport_id`='%1' OR `switch3_txport_id`='%1' OR `switch3_rxport_id`='%1' OR `switch4_txport_id`='%1' OR `switch4_rxport_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }

            foreach(const MySqlWrapper::MySqlRecord &record, query.GetResults())
            {
                PbTpInfoSet *pTpInfoSet = m_d->m_mapIdToTpInfoSets.value(record.value("infoset_id").toInt());
                if(!pTpInfoSet)
                {
                    transaction.Rollback();
                    return false;
                }

                PbTpInfoSet TpInfoSet(*pTpInfoSet);
                int iTxPort = pTpInfoSet->GetTxPort() ? pTpInfoSet->GetTxPort()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iRxPort = pTpInfoSet->GetRxPort() ? pTpInfoSet->GetRxPort()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iSwitch1TxPort = pTpInfoSet->GetSwitch1TxPort() ? pTpInfoSet->GetSwitch1TxPort()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iSwitch1RxPort = pTpInfoSet->GetSwitch1RxPort() ? pTpInfoSet->GetSwitch1RxPort()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iSwitch2TxPort = pTpInfoSet->GetSwitch2TxPort() ? pTpInfoSet->GetSwitch2TxPort()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iSwitch2RxPort = pTpInfoSet->GetSwitch2RxPort() ? pTpInfoSet->GetSwitch2RxPort()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iSwitch3TxPort = pTpInfoSet->GetSwitch3TxPort() ? pTpInfoSet->GetSwitch3TxPort()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iSwitch3RxPort = pTpInfoSet->GetSwitch3RxPort() ? pTpInfoSet->GetSwitch3RxPort()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iSwitch4TxPort = pTpInfoSet->GetSwitch4TxPort() ? pTpInfoSet->GetSwitch4TxPort()->GetId() : PbBaseObject::m_iInvalidObjectId;
                int iSwitch4RxPort = pTpInfoSet->GetSwitch4RxPort() ? pTpInfoSet->GetSwitch4RxPort()->GetId() : PbBaseObject::m_iInvalidObjectId;
                if(iTxPort == iId)
                    TpInfoSet.SetTxPort(0);
                if(iRxPort == iId)
                    TpInfoSet.SetRxPort(0);
                if(iSwitch1TxPort == iId)
                    TpInfoSet.SetSwitch1TxPort(0);
                if(iSwitch1RxPort == iId)
                    TpInfoSet.SetSwitch1RxPort(0);
                if(iSwitch2TxPort == iId)
                    TpInfoSet.SetSwitch2TxPort(0);
                if(iSwitch2RxPort == iId)
                    TpInfoSet.SetSwitch2RxPort(0);
                if(iSwitch3TxPort == iId)
                    TpInfoSet.SetSwitch3TxPort(0);
                if(iSwitch3RxPort == iId)
                    TpInfoSet.SetSwitch3RxPort(0);
                if(iSwitch4TxPort == iId)
                    TpInfoSet.SetSwitch4TxPort(0);
                if(iSwitch4RxPort == iId)
                    TpInfoSet.SetSwitch4RxPort(0);

                if(!DbUpdateObject(TpInfoSet))
                {
                    transaction.Rollback();
                    return false;
                }
            }

            /////////////////////////////////////////////////////////////////
            // Delete current port
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM tp_port WHERE `port_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }
        }
        break;

        case PbBaseObject::botTpInfoSet:
        {
            /////////////////////////////////////////////////////////////////
            // Delete current information set
            /////////////////////////////////////////////////////////////////
            if(!pDatabase->ExecSql(QString("DELETE FROM `tp_infoset` WHERE `infoset_id`='%1'").arg(iId)))
            {
                transaction.Rollback();
                return false;
            }
        }
        break;

        default:
            return false;
    }

    transaction.Commit();
    return true;
}

bool BaseManager::DbReadLibDeviceModel(int iDeviceId, QByteArray &baModelContent)
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    return pDatabase->ReadBlog(QString("SELECT `icd_content` FROM `lib_device` WHERE `device_id`='%1'").arg(iDeviceId), baModelContent);
}

bool BaseManager::DbUpdateLibDeviceModel(int iDeviceId, const QByteArray &baModelContent)
{
    if(iDeviceId <= PbBaseObject::m_iInvalidObjectId)
        return false;

    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return false;

    if(!baModelContent.isEmpty() && !baModelContent.isNull())
    {
        QString strSql = QString("UPDATE `lib_device` SET `icd_content` = ? WHERE `device_id`='%1'").arg(iDeviceId);
        if(!pDatabase->WriteBlog(strSql, baModelContent))
            return false;
    }
    else
    {
        if(!pDatabase->ExecSql(QString("UPDATE `lib_device` set `icd_content` = NULL WHERE `device_id`='%1'").arg(iDeviceId)))
            return false;
    }

    return true;
}

int BaseManager::DbGenerateNewTpInfoSetGroup()
{
    MySqlWrapper::MySqlDatabase *pDatabase = DatabaseManager::Instance()->GetBaseDb();
    if(!pDatabase)
        return -1;

    MySqlWrapper::MySqlQuery query(pDatabase);
    if(!query.Exec("SELECT MAX(infoset_id) FROM tp_infoset"))
        return -1;

    if(query.GetResults().size() != 1)
        return -1;

    if(!query.GetResults().first().contains("MAX(infoset_id)"))
        return -1;

    return query.GetResults().first().value("MAX(infoset_id)").toInt() + 1;
}
