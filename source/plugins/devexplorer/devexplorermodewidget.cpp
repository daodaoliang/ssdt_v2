#include <QApplication>
#include <QLabel>
#include <QToolButton>
#include <QPainter>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QClipboard>

#include "utils/waitcursor.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/peport.h"

#include "core/core_constants.h"
#include "core/mainwindow.h"
#include "core/actionmanager.h"
#include "core/actioncontainer.h"
#include "core/configcoreoperation.h"

#include "devexplorermodewidget.h"
#include "configwidgetprojectphysical.h"
#include "configwidgetprojectelectrical.h"
#include "configwidgetbay.h"
#include "configwidgetroom.h"
#include "configwidgetcubicle.h"
#include "configwidgetswitch.h"
#include "configwidgetiedboard.h"
#include "configwidgetiedport.h"
#include "propertydlgbay.h"
#include "propertydlgroom.h"
#include "propertydlgcubicle.h"
#include "propertydlgdevice.h"
#include "propertydlgboard.h"
#include "propertydlgfiberport.h"

const char * const g_szUID_ContextMenuAddNew    = "DevExplorer.ContextMenu.AddNew";

using namespace DevExplorer::Internal;

///////////////////////////////////////////////////////////////////////
// DeviceToolWidget member functions
///////////////////////////////////////////////////////////////////////
DeviceToolWidget::DeviceToolWidget(QWidget *pParent) : StyledUi::StyledBar(pParent)
{
    m_pLabelDisplayIcon = new QLabel(this);
    m_pLabelDisplayName = new QLabel(this);
    m_pHBoxLayout = new QHBoxLayout(this);
    m_pHBoxLayout->setContentsMargins(10, 0, 0, 0);
    m_pHBoxLayout->addWidget(m_pLabelDisplayIcon);
    m_pHBoxLayout->addWidget(m_pLabelDisplayName);
    m_pHBoxLayout->addStretch(1);
}

DeviceToolWidget::~DeviceToolWidget()
{
}

void DeviceToolWidget::SetConfigWidget(ConfigWidgetDeviceObject *pConfigWidget)
{
    if(QWidgetItem *pWidgetItem = dynamic_cast<QWidgetItem*>(m_pHBoxLayout->itemAt(3)))
    {
        if(QWidget *pToolBar = pWidgetItem->widget())
        {
            m_pHBoxLayout->removeWidget(pToolBar);
            pToolBar->hide();
        }
    }

    if(pConfigWidget)
    {
        m_pLabelDisplayIcon->setPixmap(pConfigWidget->GetProjectObject()->GetDisplayIcon().pixmap(QSize(18, 18)));
        m_pLabelDisplayName->setText("<font><B>" + pConfigWidget->GetProjectObject()->GetDisplayName() + "</B></font>");
        if(QWidget *pToolBar = pConfigWidget->GetToolBar())
        {
            m_pHBoxLayout->addWidget(pToolBar);
            pToolBar->show();
        }
    }
    else
    {
        m_pLabelDisplayIcon->clear();
        m_pLabelDisplayName->clear();
    }

    setVisible(pConfigWidget);
}

///////////////////////////////////////////////////////////////////////
// DevExplorerModeWidget member functions
///////////////////////////////////////////////////////////////////////
DevExplorerModeWidget* DevExplorerModeWidget::m_pInstance = 0;
DevExplorerModeWidget::DevExplorerModeWidget(QWidget *pParent) :
    StyledUi::StyledWidget(pParent)
{
    m_pInstance = this;

    SetupContextMenuActions();

    m_pDeviceToolWidget = new DeviceToolWidget(this);
    m_pStackedWidget = new QStackedWidget(this);

    QVBoxLayout *pVBoxLayout = new QVBoxLayout(this);
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->setSpacing(0);
    pVBoxLayout->addWidget(m_pDeviceToolWidget);
    pVBoxLayout->addWidget(m_pStackedWidget);

    ActivateObject(0, false);
}

DevExplorerModeWidget::~DevExplorerModeWidget()
{
}

DevExplorerModeWidget* DevExplorerModeWidget::Instance()
{
    return m_pInstance;
}

ProjectExplorer::PeBay* DevExplorerModeWidget::CreateBay(ProjectExplorer::PeProjectObject *pParentObject)
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = qobject_cast<ProjectExplorer::PeProjectVersion*>(pParentObject);
    if(!pProjectVersion)
        return 0;

    ProjectExplorer::PeBay bay;
    bay.SetProjectVersion(pProjectVersion);

    PropertyDlgBay dlg(&bay, this);
    if(dlg.exec() != QDialog::Accepted)
        return 0;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(pProjectVersion->DbCreateObject(bay, true))
        return qobject_cast<ProjectExplorer::PeBay*>(pProjectVersion->CreateObject(bay));

    return 0;
}

void DevExplorerModeWidget::UpdateBay(ProjectExplorer::PeBay *pBay)
{
    if(!pBay)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pBay->GetProjectVersion();
    if(!pProjectVersion)
        return;

    ProjectExplorer::PeBay bay(*pBay);
    PropertyDlgBay dlg(&bay, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(pProjectVersion->DbUpdateObject(bay, true))
        pProjectVersion->UpdateObject(bay);
}

ProjectExplorer::PeRoom* DevExplorerModeWidget::CreateRoom(ProjectExplorer::PeProjectObject *pParentObject)
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = qobject_cast<ProjectExplorer::PeProjectVersion*>(pParentObject);
    if(!pProjectVersion)
        return 0;

    ProjectExplorer::PeRoom room;
    room.SetProjectVersion(pProjectVersion);

    QStringList lstRoomNumbers;
    foreach(ProjectExplorer::PeRoom *pRoom, pProjectVersion->GetAllRooms())
        lstRoomNumbers.append(pRoom->GetNumber());

    for(int i = 1; i < 100; i++)
    {
        QString strRoomNumber = QString("R%1").arg(i, 2, 10, QLatin1Char('0'));
        if(!lstRoomNumbers.contains(strRoomNumber))
        {
            room.SetNumber(strRoomNumber);
            break;
        }
    }

    PropertyDlgRoom dlg(&room, this);
    if(dlg.exec() != QDialog::Accepted)
        return 0;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(pProjectVersion->DbCreateObject(room, true))
        return qobject_cast<ProjectExplorer::PeRoom*>(pProjectVersion->CreateObject(room));

    return 0;
}

void DevExplorerModeWidget::UpdateRoom(ProjectExplorer::PeRoom *pRoom)
{
    if(!pRoom)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pRoom->GetProjectVersion();
    if(!pProjectVersion)
        return;

    ProjectExplorer::PeRoom room(*pRoom);
    PropertyDlgRoom dlg(&room, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(pProjectVersion->DbUpdateObject(room, true))
        pProjectVersion->UpdateObject(room);
}

ProjectExplorer::PeCubicle* DevExplorerModeWidget::CreateCubicle(ProjectExplorer::PeProjectObject *pParentObject)
{
    if(!pParentObject)
        return 0;

    ProjectExplorer::PeRoom *pRoom = 0;
    ProjectExplorer::PeProjectVersion *pProjectVersion = 0;
    if(pParentObject->GetObjectType() == ProjectExplorer::PeProjectObject::otRoom)
    {
        pRoom = qobject_cast<ProjectExplorer::PeRoom*>(pParentObject);
        pProjectVersion = pRoom->GetProjectVersion();
    }
    else if(pParentObject->GetObjectType() == ProjectExplorer::PeProjectObject::otProjectVersion)
    {
        pProjectVersion = qobject_cast<ProjectExplorer::PeProjectVersion*>(pParentObject);
    }

    if(!pProjectVersion)
        return 0;

    ProjectExplorer::PeCubicle cubicle;
    cubicle.SetProjectVersion(pProjectVersion);
    cubicle.SetParentRoom(pRoom);

    QStringList lstCubicleNumbers;
    foreach(ProjectExplorer::PeCubicle *pCubicle, pProjectVersion->GetAllCubicles())
        lstCubicleNumbers.append(pCubicle->GetNumber());

    for(int i = 1; i < 100; i++)
    {
        QString strCubicleNumber = QString("C%1").arg(i, 2, 10, QLatin1Char('0'));
        if(!lstCubicleNumbers.contains(strCubicleNumber))
        {
            cubicle.SetNumber(strCubicleNumber);
            break;
        }
    }

    PropertyDlgCubicle dlg(&cubicle, this);
    if(dlg.exec() != QDialog::Accepted)
        return 0;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(pProjectVersion->DbCreateObject(cubicle, true))
        return qobject_cast<ProjectExplorer::PeCubicle*>(pProjectVersion->CreateObject(cubicle));

    return 0;
}

void DevExplorerModeWidget::UpdateCubicle(ProjectExplorer::PeCubicle *pCubicle)
{
    if(!pCubicle)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pCubicle->GetProjectVersion();
    if(!pProjectVersion)
        return;

    ProjectExplorer::PeCubicle cubicle(*pCubicle);
    PropertyDlgCubicle dlg(&cubicle, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(pProjectVersion->DbUpdateObject(cubicle, true))
        pProjectVersion->UpdateObject(cubicle);
}

ProjectExplorer::PeDevice* DevExplorerModeWidget::CreateIED(ProjectExplorer::PeProjectObject *pParentObject)
{
    if(!pParentObject)
        return 0;

    ProjectExplorer::PeBay *pBay = 0;
    ProjectExplorer::PeCubicle *pCubicle = 0;
    ProjectExplorer::PeProjectVersion *pProjectVersion = 0;
    if(pParentObject->GetObjectType() == ProjectExplorer::PeProjectObject::otBay)
    {
        pBay = qobject_cast<ProjectExplorer::PeBay*>(pParentObject);
        pProjectVersion = pBay->GetProjectVersion();
    }
    else if(pParentObject->GetObjectType() == ProjectExplorer::PeProjectObject::otCubicle)
    {
        pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pParentObject);
        pProjectVersion = pCubicle->GetProjectVersion();
    }
    else if(pParentObject->GetObjectType() == ProjectExplorer::PeProjectObject::otProjectVersion)
    {
        pProjectVersion = qobject_cast<ProjectExplorer::PeProjectVersion*>(pParentObject);
    }

    if(!pProjectVersion)
        return 0;

    ProjectExplorer::PeDevice device;
    device.SetDeviceType(ProjectExplorer::PeDevice::dtIED);
    device.SetProjectVersion(pProjectVersion);
    device.SetParentCubicle(pCubicle);
    device.SetParentBay(pBay);

    PropertyDlgDevice dlg(&device, 0, this);
    if(dlg.exec() != QDialog::Accepted)
        return 0;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    return Core::ConfigCoreOperation::Instance()->CreateDeviceFromLibDevice(device, dlg.GetLibDevice());
}

ProjectExplorer::PeDevice* DevExplorerModeWidget::CreateSwitch(ProjectExplorer::PeProjectObject *pParentObject)
{
    if(!pParentObject)
        return 0;

    ProjectExplorer::PeBay *pBay = 0;
    ProjectExplorer::PeCubicle *pCubicle = 0;
    ProjectExplorer::PeProjectVersion *pProjectVersion = 0;
    if(pParentObject->GetObjectType() == ProjectExplorer::PeProjectObject::otBay)
    {
        pBay = qobject_cast<ProjectExplorer::PeBay*>(pParentObject);
        pProjectVersion = pBay->GetProjectVersion();
    }
    else if(pParentObject->GetObjectType() == ProjectExplorer::PeProjectObject::otCubicle)
    {
        pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pParentObject);
        pProjectVersion = pCubicle->GetProjectVersion();
    }
    else if(pParentObject->GetObjectType() == ProjectExplorer::PeProjectObject::otProjectVersion)
    {
        pProjectVersion = qobject_cast<ProjectExplorer::PeProjectVersion*>(pParentObject);
    }

    if(!pProjectVersion)
        return 0;

    ProjectExplorer::PeDevice device;
    device.SetDeviceType(ProjectExplorer::PeDevice::dtSwitch);
    device.SetProjectVersion(pProjectVersion);
    device.SetParentCubicle(pCubicle);
    device.SetParentBay(pBay);

    PropertyDlgDevice dlg(&device, 0, this);
    if(dlg.exec() != QDialog::Accepted)
        return 0;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    return Core::ConfigCoreOperation::Instance()->CreateDeviceFromLibDevice(device, dlg.GetLibDevice());
}

void DevExplorerModeWidget::UpdateDevice(ProjectExplorer::PeDevice *pDevice)
{
    if(!pDevice)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pDevice->GetProjectVersion();
    if(!pProjectVersion)
        return;

    ProjectExplorer::PeDevice device(*pDevice);
    PropertyDlgDevice dlg(&device, 0, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(pProjectVersion->DbUpdateObject(device, true))
        pProjectVersion->UpdateObject(device);
}

ProjectExplorer::PeBoard* DevExplorerModeWidget::CreateBoard(ProjectExplorer::PeDevice *pParentDevice)
{
    if(!pParentDevice)
        return 0;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pParentDevice->GetProjectVersion();
    if(!pProjectVersion)
        return 0;

    ProjectExplorer::PeBoard board;
    board.SetProjectVersion(pProjectVersion);
    board.SetParentDevice(pParentDevice);

    PropertyDlgBoard dlg(&board, this);
    if(dlg.exec() != QDialog::Accepted)
        return 0;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(pProjectVersion->DbCreateObject(board, true))
        return qobject_cast<ProjectExplorer::PeBoard*>(pProjectVersion->CreateObject(board));

    return 0;
}

void DevExplorerModeWidget::UpdateBoard(ProjectExplorer::PeBoard *pBoard)
{
    if(!pBoard)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pBoard->GetProjectVersion();
    if(!pProjectVersion)
        return;

    ProjectExplorer::PeBoard board(*pBoard);
    PropertyDlgBoard dlg(&board, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(pProjectVersion->DbUpdateObject(board, true))
        pProjectVersion->UpdateObject(board);
}

ProjectExplorer::PeBoard* DevExplorerModeWidget::CloneBoard(ProjectExplorer::PeBoard *pSrcBoard, ProjectExplorer::PeDevice *pParentDevice)
{
    if(!pSrcBoard || !pParentDevice)
        return 0;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pParentDevice->GetProjectVersion();
    if(!pProjectVersion)
        return 0;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    ///////////////////////////////////////////////////////////////////////
    // Prepare
    ///////////////////////////////////////////////////////////////////////
    QStringList lstAllPositions;
    foreach(ProjectExplorer::PeBoard *pBoard, pParentDevice->GetChildBoards())
        lstAllPositions.append(pBoard->GetPosition());

    QString strDstPosition = pSrcBoard->GetPosition();
    int iCopyIndex = 0;
    while(lstAllPositions.contains(strDstPosition))
    {
        if(iCopyIndex == 0)
            strDstPosition = QString("%1_copy").arg(pSrcBoard->GetPosition());
        else
            strDstPosition = QString("%1_copy%2").arg(pSrcBoard->GetPosition()).arg(iCopyIndex);

        iCopyIndex++;
    }

    ProjectExplorer::PeBoard board(*pSrcBoard);
    board.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    board.SetProjectVersion(pProjectVersion);
    board.SetParentDevice(pParentDevice);
    board.SetPosition(strDstPosition);

    QList<ProjectExplorer::PePort> lstPorts;
    foreach(ProjectExplorer::PePort *pPort, pSrcBoard->GetChildPorts())
    {
        ProjectExplorer::PePort port(*pPort);
        port.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        port.SetProjectVersion(pProjectVersion);
        port.SetParentBoard(&board);
        lstPorts.append(port);
    }

    ///////////////////////////////////////////////////////////////////////
    // Handle Database
    ///////////////////////////////////////////////////////////////////////
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

    // Create new board
    if(!pProjectVersion->DbCreateObject(board, false))
    {
        DbTrans.Rollback();
        return 0;
    }

    // Create new ports
    for(int i = 0; i < lstPorts.size(); i++)
    {
        ProjectExplorer::PePort &port = lstPorts[i];
        if(!pProjectVersion->DbCreateObject(port, false))
        {
            DbTrans.Rollback();
            return 0;
        }
    }

    if(!DbTrans.Commit())
        return 0;

    ///////////////////////////////////////////////////////////////////////
    // Handle Object
    ///////////////////////////////////////////////////////////////////////

    if(ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pProjectVersion->CreateObject(board)))
    {
        for(int i = 0; i < lstPorts.size(); i++)
        {
            ProjectExplorer::PePort &port = lstPorts[i];
            port.SetParentBoard(pBoard);

            pProjectVersion->CreateObject(port);
        }

        return pBoard;
    }

    return 0;
}

ProjectExplorer::PePort* DevExplorerModeWidget::CreateFiberPort(ProjectExplorer::PeBoard *pParentBoard)
{
    if(!pParentBoard)
        return 0;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pParentBoard->GetProjectVersion();
    if(!pProjectVersion)
        return 0;

    ProjectExplorer::PePort port;
    port.SetProjectVersion(pProjectVersion);
    port.SetParentBoard(pParentBoard);
    port.SetPortType(ProjectExplorer::PePort::ptFiber);
    port.SetPortDirection(ProjectExplorer::PePort::pdRT);

    PropertyDlgFiberPort dlg(&port, this);
    if(dlg.exec() != QDialog::Accepted)
        return 0;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    const int iNewPortNumber = dlg.GetNewPortNumber();
    int iGroup = port.GetGroup();
    QList<int> lstExistGroups;
    foreach(ProjectExplorer::PePort *pPort, pParentBoard->GetChildPorts())
    {
        if(!lstExistGroups.contains(pPort->GetGroup()))
            lstExistGroups.append(pPort->GetGroup());
    }

    QList<ProjectExplorer::PePort> lstPorts;
    if(port.GetPortDirection() == ProjectExplorer::PePort::pdRT)
    {
        const QStringList lstPortName = port.GetName().split(PropertyDlgFiberPort::m_strSpliter);
        Q_ASSERT(lstPortName.size() == 2);

        for(int i = 0; i < iNewPortNumber; i++)
        {
            while(lstExistGroups.contains(iGroup))
                iGroup++;
            lstExistGroups.append(iGroup);

            ProjectExplorer::PePort portTx = port;
            portTx.SetGroup(iGroup);
            portTx.SetPortDirection(ProjectExplorer::PePort::pdTx);
            portTx.SetName(iNewPortNumber == 1 ? lstPortName.at(0) : QString("%1%2").arg(lstPortName.at(0)).arg(iGroup, 2, 10, QLatin1Char('0')));
            lstPorts.append(portTx);

            ProjectExplorer::PePort portRx = port;
            portRx.SetGroup(iGroup);
            portRx.SetPortDirection(ProjectExplorer::PePort::pdRx);
            portRx.SetName(iNewPortNumber == 1 ? lstPortName.at(1) : QString("%1%2").arg(lstPortName.at(1)).arg(iGroup, 2, 10, QLatin1Char('0')));
            lstPorts.append(portRx);
        }
    }
    else
    {
        for(int i = 0; i < iNewPortNumber; i++)
        {
            while(lstExistGroups.contains(iGroup))
                iGroup++;
            lstExistGroups.append(iGroup);

            ProjectExplorer::PePort portTemp = port;
            portTemp.SetGroup(iGroup);
            portTemp.SetName(iNewPortNumber == 1 ? port.GetName() : QString("%1%2").arg(port.GetName()).arg(iGroup, 2, 10, QLatin1Char('0')));
            lstPorts.append(portTemp);
        }
    }

    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
    for(int i = 0; i < lstPorts.size(); i++)
    {
        ProjectExplorer::PePort &port = lstPorts[i];
        if(!pProjectVersion->DbCreateObject(port, false))
        {
            DbTrans.Rollback();
            return 0;
        }
    }

    if(!DbTrans.Commit())
        return 0;

    ProjectExplorer::PePort *pPortReturned = 0;
    foreach(const ProjectExplorer::PePort &port, lstPorts)
        pPortReturned = qobject_cast<ProjectExplorer::PePort*>(pProjectVersion->CreateObject(port));

    return pPortReturned;
}

void DevExplorerModeWidget::UpdatePort(ProjectExplorer::PePort *pPort)
{
    if(!pPort)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pPort->GetProjectVersion();
    if(!pProjectVersion)
        return;

    if(pPort->GetPortType() == ProjectExplorer::PePort::ptFiber)
    {
        ProjectExplorer::PePort port(*pPort);
        ProjectExplorer::PePort *pPortPair = pPort->GetPairPort();
        if(pPortPair)
        {
            port.SetPortDirection(ProjectExplorer::PePort::pdRT);
            if(pPort->GetPortDirection() == ProjectExplorer::PePort::pdTx)
                port.SetName(pPort->GetName() + PropertyDlgFiberPort::m_strSpliter + pPortPair->GetName());
            else if(pPort->GetPortDirection() == ProjectExplorer::PePort::pdRx)
                port.SetName(pPortPair->GetName() + PropertyDlgFiberPort::m_strSpliter + pPort->GetName());
        }

        PropertyDlgFiberPort dlg(&port, this);
        if(dlg.exec() != QDialog::Accepted)
            return;

        Utils::WaitCursor cursor;
        Q_UNUSED(cursor)

        if(port.GetPortDirection() == ProjectExplorer::PePort::pdRT)
        {
            const QStringList lstPortName = port.GetName().split(PropertyDlgFiberPort::m_strSpliter);
            Q_ASSERT(lstPortName.size() == 2);

            ProjectExplorer::PePort portPair(port);
            if(pPort->GetPortDirection() == ProjectExplorer::PePort::pdTx)
            {
                port.SetName(lstPortName.at(0));
                port.SetPortDirection(ProjectExplorer::PePort::pdTx);

                portPair.SetId(pPortPair->GetId());
                portPair.SetName(lstPortName.at(1));
                portPair.SetPortDirection(ProjectExplorer::PePort::pdRx);
            }
            else if(pPort->GetPortDirection() == ProjectExplorer::PePort::pdRx)
            {
                port.SetName(lstPortName.at(1));
                port.SetPortDirection(ProjectExplorer::PePort::pdRx);

                portPair.SetId(pPortPair->GetId());
                portPair.SetName(lstPortName.at(0));
                portPair.SetPortDirection(ProjectExplorer::PePort::pdTx);
            }

            ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
            if(!pProjectVersion->DbUpdateObject(port, false))
            {
                DbTrans.Rollback();
                return;
            }
            if(!pProjectVersion->DbUpdateObject(portPair, false))
            {
                DbTrans.Rollback();
                return;
            }
            if(!DbTrans.Commit())
                return;

            if(pProjectVersion->UpdateObject(port))
                pProjectVersion->UpdateObject(portPair);

            return;
        }

        if(pProjectVersion->DbUpdateObject(port, true))
            pProjectVersion->UpdateObject(port);
    }
}

void DevExplorerModeWidget::DeleteObjects(const QList<ProjectExplorer::PeProjectObject*> &lstProjectObjects)
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = 0;

    QList<ProjectExplorer::PeProjectObject*> lstDeleteProjectObjects;
    foreach(ProjectExplorer::PeProjectObject *pProjectObject, lstProjectObjects)
    {
        if(!pProjectVersion)
            pProjectVersion = pProjectObject->GetProjectVersion();
        else if(pProjectVersion != pProjectObject->GetProjectVersion())
            return;

        if(lstDeleteProjectObjects.contains(pProjectObject))
            continue;

        if(ProjectExplorer::PePort *pPort = qobject_cast<ProjectExplorer::PePort*>(pProjectObject))
        {
            if(ProjectExplorer::PePort *pPairPort = pPort->GetPairPort())
                lstDeleteProjectObjects.append(pPairPort);
        }

        lstDeleteProjectObjects.append(pProjectObject);
    }

    if(!pProjectVersion || lstDeleteProjectObjects.isEmpty())
        return;

    QString strItems;
    foreach(ProjectExplorer::PeProjectObject *pProjectObject, lstDeleteProjectObjects)
        strItems = QString("%1\n%2: %3").arg(strItems).arg(pProjectObject->GetObjectTypeName()).arg(pProjectObject->GetDisplayName());

    if(QMessageBox::question(this,
                             tr("Delete Confirmation"),
                             tr("Are you sure you want to delete following items?") + "\n" + strItems,
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No) == QMessageBox::Yes)
    {
        Utils::WaitCursor cursor;
        Q_UNUSED(cursor)

        ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
        foreach(ProjectExplorer::PeProjectObject *pProjectObject, lstDeleteProjectObjects)
        {
            if(!pProjectVersion->DbDeleteObject(pProjectObject->GetObjectType(), pProjectObject->GetId(), false))
            {
                DbTrans.Rollback();
                return;
            }
        }
        if(!DbTrans.Commit())
            return;

        foreach(ProjectExplorer::PeProjectObject *pProjectObject, lstDeleteProjectObjects)
        {
            if(GetCurrentConfigWidget()->GetProjectObject() == pProjectObject)
                ActivateObject(pProjectObject->GetProjectVersion(), false);

            pProjectVersion->DeleteObject(pProjectObject);
        }
    }
}

void DevExplorerModeWidget::UpdateObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!pProjectObject)
        return;

    const ProjectExplorer::PeProjectObject::ObjectType eObjectType = pProjectObject->GetObjectType();
    if(eObjectType == ProjectExplorer::PeProjectObject::otBay)
        UpdateBay(qobject_cast<ProjectExplorer::PeBay*>(pProjectObject));
    else if(eObjectType == ProjectExplorer::PeProjectObject::otRoom)
        UpdateRoom(qobject_cast<ProjectExplorer::PeRoom*>(pProjectObject));
    else if(eObjectType == ProjectExplorer::PeProjectObject::otCubicle)
        UpdateCubicle(qobject_cast<ProjectExplorer::PeCubicle*>(pProjectObject));
    else if(eObjectType == ProjectExplorer::PeProjectObject::otDevice)
        UpdateDevice(qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject));
    else if(eObjectType == ProjectExplorer::PeProjectObject::otBoard)
        UpdateBoard(qobject_cast<ProjectExplorer::PeBoard*>(pProjectObject));
    else if(eObjectType == ProjectExplorer::PeProjectObject::otPort)
        UpdatePort(qobject_cast<ProjectExplorer::PePort*>(pProjectObject));
}

void DevExplorerModeWidget::ActivateObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    if(!pProjectObject)
    {
        m_pDeviceToolWidget->SetConfigWidget(0);
        SetCurrentConfigWidget(0);

        return;
    }

    ProjectExplorer::PeProjectObject *pProjectObjectCurrent = 0;
    ConfigWidgetDeviceObject *pConfigWidget = GetCurrentConfigWidget();
    if(pConfigWidget)
        pProjectObjectCurrent = pConfigWidget->GetProjectObject();

    if(pProjectObject == pProjectObjectCurrent)
    {
        ConfigWidgetProjectElectrical *pConfigWidgetProjectElectrical = qobject_cast<ConfigWidgetProjectElectrical*>(pConfigWidget);
        ConfigWidgetProjectPhysical *pConfigWidgetProjectPhysical = qobject_cast<ConfigWidgetProjectPhysical*>(pConfigWidget);

        if((!pConfigWidgetProjectElectrical && !pConfigWidgetProjectPhysical) ||
           (bBayObject && pConfigWidgetProjectElectrical) ||
           (!bBayObject && pConfigWidgetProjectPhysical))
        {
            return;
        }
    }

    ConfigWidgetDeviceObject *pConfigWidgetNew = CreateConfigWidgetFromObject(pProjectObject, bBayObject);
    if(!pConfigWidgetNew)
        return;

    if(!pConfigWidgetNew->BuildModel(pProjectObject))
    {
        delete pConfigWidgetNew;
        return;
    }

    m_pDeviceToolWidget->SetConfigWidget(pConfigWidgetNew);
    SetCurrentConfigWidget(pConfigWidgetNew);
}

void DevExplorerModeWidget::SetContextMenuObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    m_pContextMenuActionNewBay->setEnabled(false);
    m_pContextMenuActionNewBay->setVisible(false);
    m_pContextMenuActionNewRoom->setEnabled(false);
    m_pContextMenuActionNewRoom->setVisible(false);
    m_pContextMenuActionNewCubicle->setEnabled(false);
    m_pContextMenuActionNewCubicle->setVisible(false);
    m_pContextMenuActionNewIED->setEnabled(false);
    m_pContextMenuActionNewIED->setVisible(false);
    m_pContextMenuActionNewSwitch->setEnabled(false);
    m_pContextMenuActionNewSwitch->setVisible(false);
    m_pContextMenuActionNewBoard->setEnabled(false);
    m_pContextMenuActionNewBoard->setVisible(false);
    m_pContextMenuActionNewFiberPort->setEnabled(false);
    m_pContextMenuActionNewFiberPort->setVisible(false);
    m_pContextMenuActionDelete->setEnabled(false);
    m_pContextMenuActionDelete->setVisible(false);
    m_pContextMenuActionProperty->setEnabled(false);
    m_pContextMenuActionProperty->setVisible(false);
    m_pContextMenuActionCopy->setEnabled(false);
    m_pContextMenuActionCopy->setVisible(false);
    m_pContextMenuActionPaste->setEnabled(false);
    m_pContextMenuActionPaste->setVisible(false);

    if(pProjectObject)
    {
        m_pContextMenuActionDelete->setEnabled(true);
        m_pContextMenuActionDelete->setVisible(true);
        m_pContextMenuActionProperty->setEnabled(true);
        m_pContextMenuActionProperty->setVisible(true);

        m_pContextMenuActionCopy->setVisible(true);
        m_pContextMenuActionPaste->setVisible(true);

        const ProjectExplorer::PeProjectObject::ObjectType eObjectType = pProjectObject->GetObjectType();
        if(eObjectType == ProjectExplorer::PeProjectObject::otProjectVersion)
        {
            if(bBayObject)
            {
                m_pContextMenuActionNewBay->setEnabled(true);
                m_pContextMenuActionNewBay->setVisible(true);
            }
            else
            {
                m_pContextMenuActionNewRoom->setEnabled(true);
                m_pContextMenuActionNewRoom->setVisible(true);
                m_pContextMenuActionNewCubicle->setEnabled(true);
                m_pContextMenuActionNewCubicle->setVisible(true);
            }
            m_pContextMenuActionNewIED->setEnabled(true);
            m_pContextMenuActionNewIED->setVisible(true);
            m_pContextMenuActionNewSwitch->setEnabled(true);
            m_pContextMenuActionNewSwitch->setVisible(true);
            m_pContextMenuActionPaste->setEnabled(m_pCopiedProjectObject && (m_pCopiedProjectObject->GetObjectType() == ProjectExplorer::PeProjectObject::otDevice || m_pCopiedProjectObject->GetObjectType() == ProjectExplorer::PeProjectObject::otCubicle));
        }
        else if(eObjectType == ProjectExplorer::PeProjectObject::otBay)
        {
            m_pContextMenuActionNewIED->setEnabled(true);
            m_pContextMenuActionNewIED->setVisible(true);
            m_pContextMenuActionNewSwitch->setEnabled(true);
            m_pContextMenuActionNewSwitch->setVisible(true);
            m_pContextMenuActionPaste->setEnabled(m_pCopiedProjectObject && m_pCopiedProjectObject->GetObjectType() == ProjectExplorer::PeProjectObject::otDevice);
        }
        else if(eObjectType == ProjectExplorer::PeProjectObject::otRoom)
        {
            m_pContextMenuActionNewCubicle->setEnabled(true);
            m_pContextMenuActionNewCubicle->setVisible(true);
            m_pContextMenuActionPaste->setEnabled(m_pCopiedProjectObject && m_pCopiedProjectObject->GetObjectType() == ProjectExplorer::PeProjectObject::otCubicle);
        }
        else if(eObjectType == ProjectExplorer::PeProjectObject::otCubicle)
        {
            m_pContextMenuActionNewIED->setEnabled(true);
            m_pContextMenuActionNewIED->setVisible(true);
            m_pContextMenuActionNewSwitch->setEnabled(true);
            m_pContextMenuActionNewSwitch->setVisible(true);
            m_pContextMenuActionCopy->setEnabled(true);
            m_pContextMenuActionPaste->setEnabled(m_pCopiedProjectObject && m_pCopiedProjectObject->GetObjectType() == ProjectExplorer::PeProjectObject::otDevice);
        }
        else if(eObjectType == ProjectExplorer::PeProjectObject::otDevice)
        {
            if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                {
                    m_pContextMenuActionNewBoard->setEnabled(true);
                    m_pContextMenuActionNewBoard->setVisible(true);
                    m_pContextMenuActionCopy->setEnabled(true);
                    m_pContextMenuActionPaste->setEnabled(m_pCopiedProjectObject && m_pCopiedProjectObject->GetObjectType() == ProjectExplorer::PeProjectObject::otBoard);
                }
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                {
                    m_pContextMenuActionNewFiberPort->setEnabled(true);
                    m_pContextMenuActionNewFiberPort->setVisible(true);
                    m_pContextMenuActionCopy->setEnabled(true);
                }
            }
        }
        else if(eObjectType == ProjectExplorer::PeProjectObject::otBoard)
        {
            m_pContextMenuActionNewFiberPort->setEnabled(true);
            m_pContextMenuActionNewFiberPort->setVisible(true);
            m_pContextMenuActionCopy->setEnabled(true);
        }
    }
}

void DevExplorerModeWidget::SetupContextMenuActions()
{
    QList<int> lstGlobalContexts = QList<int>() << Core::Constants::g_iUID_Context_Global;

    Core::CActionManager *pActionManager = Core::MainWindow::Instance()->GetActionManager();
    Core::CActionContainer *pActionContainerContextMenu = pActionManager->GetActionContainer(Core::Constants::g_szUID_ContextMenu);

    Core::CActionContainer *pActionContainerContextMenuAdd = pActionManager->CreateMenu(g_szUID_ContextMenuAddNew);
    pActionContainerContextMenuAdd->GetMenu()->setTitle(tr("Add &New"));
    pActionContainerContextMenu->AddMenu(pActionContainerContextMenuAdd, Core::Constants::g_szUID_ContextMenuGroup_DevObject);

    m_pContextMenuActionNewBay = new QAction(ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otBay), tr("New %1...").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otBay)), this);
    Core::CCommand *pCommand = pActionManager->RegisterAction(m_pContextMenuActionNewBay, QLatin1String("DevExplorer.ContextMenuAction.NewBay"), lstGlobalContexts);
    pActionContainerContextMenuAdd->AddAction(pCommand);
    connect(m_pContextMenuActionNewBay, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionNewBay()));

    m_pContextMenuActionNewRoom = new QAction(ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otRoom), tr("New %1...").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otRoom)), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionNewRoom, QLatin1String("DevExplorer.ContextMenuAction.NewRoom"), lstGlobalContexts);
    pActionContainerContextMenuAdd->AddAction(pCommand);
    connect(m_pContextMenuActionNewRoom, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionNewRoom()));

    m_pContextMenuActionNewCubicle = new QAction(ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otCubicle), tr("New %1...").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otCubicle)), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionNewCubicle, QLatin1String("DevExplorer.ContextMenuAction.NewCubicle"), lstGlobalContexts);
    pActionContainerContextMenuAdd->AddAction(pCommand);
    connect(m_pContextMenuActionNewCubicle, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionNewCubicle()));

    m_pContextMenuActionNewIED = new QAction(ProjectExplorer::PeDevice::GetDeviceIcon(ProjectExplorer::PeDevice::dtIED), tr("New %1...").arg(ProjectExplorer::PeDevice::GetDeviceTypeName(ProjectExplorer::PeDevice::dtIED)), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionNewIED, QLatin1String("DevExplorer.ContextMenuAction.NewIED"), lstGlobalContexts);
    pActionContainerContextMenuAdd->AddAction(pCommand);
    connect(m_pContextMenuActionNewIED, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionNewIED()));

    m_pContextMenuActionNewSwitch = new QAction(ProjectExplorer::PeDevice::GetDeviceIcon(ProjectExplorer::PeDevice::dtSwitch), tr("New %1...").arg(ProjectExplorer::PeDevice::GetDeviceTypeName(ProjectExplorer::PeDevice::dtSwitch)), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionNewSwitch, QLatin1String("DevExplorer.ContextMenuAction.NewSwitch"), lstGlobalContexts);
    pActionContainerContextMenuAdd->AddAction(pCommand);
    connect(m_pContextMenuActionNewSwitch, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionNewSwitch()));

    m_pContextMenuActionNewBoard = new QAction(ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otBoard), tr("New %1...").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otBoard)), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionNewBoard, QLatin1String("DevExplorer.ContextMenuAction.NewBoard"), lstGlobalContexts);
    pActionContainerContextMenuAdd->AddAction(pCommand);
    connect(m_pContextMenuActionNewBoard, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionNewBoard()));

    m_pContextMenuActionNewFiberPort = new QAction(ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otPort), tr("New %1...").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otPort)), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionNewFiberPort, QLatin1String("DevExplorer.ContextMenuAction.NewFiberPort"), lstGlobalContexts);
    pActionContainerContextMenuAdd->AddAction(pCommand);
    connect(m_pContextMenuActionNewFiberPort, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionNewFiberPort()));

    m_pContextMenuActionDelete = new QAction(QIcon(":/devexplorer/images/oper_remove.png"), tr("&Delete"), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionDelete, QLatin1String("DevExplorer.ContextMenuAction.Delete"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Core::Constants::g_szUID_ContextMenuGroup_DevObject);
    connect(m_pContextMenuActionDelete, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionDelete()));

    m_pContextMenuActionProperty = new QAction(QIcon(":/devexplorer/images/oper_edit.png"), tr("&Property"), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionProperty, QLatin1String("DevExplorer.ContextMenuAction.Property"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Core::Constants::g_szUID_ContextMenuGroup_DevObject);
    connect(m_pContextMenuActionProperty, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionProperty()));

    QAction *pActionSeparator = new QAction(this);
    pActionSeparator->setSeparator(true);
    pCommand = pActionManager->RegisterAction(pActionSeparator, QLatin1String("DevExplorer.Action.Separator.CopyPaste"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Core::Constants::g_szUID_ContextMenuGroup_DevObject);

    m_pContextMenuActionCopy = new QAction(tr("Cop&y"), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionCopy, QLatin1String("DevExplorer.ContextMenuAction.Copy"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Core::Constants::g_szUID_ContextMenuGroup_DevObject);
    connect(m_pContextMenuActionCopy, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionCopy()));

    m_pContextMenuActionPaste = new QAction(tr("&Paste"), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionPaste, QLatin1String("DevExplorer.ContextMenuAction.Paste"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Core::Constants::g_szUID_ContextMenuGroup_DevObject);
    connect(m_pContextMenuActionPaste, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionPaste()));
}

void DevExplorerModeWidget::SetCurrentConfigWidget(ConfigWidgetDeviceObject *pConfigWidget)
{
    while(m_pStackedWidget->count())
    {
        QWidget *pWidget = m_pStackedWidget->widget(0);
        m_pStackedWidget->removeWidget(pWidget);
        delete pWidget;
    }

    if(pConfigWidget)
        m_pStackedWidget->addWidget(pConfigWidget);
}

ConfigWidgetDeviceObject* DevExplorerModeWidget::GetCurrentConfigWidget() const
{
    return qobject_cast<ConfigWidgetDeviceObject*>(m_pStackedWidget->currentWidget());
}

ConfigWidgetDeviceObject* DevExplorerModeWidget::CreateConfigWidgetFromObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    if(!pProjectObject)
        return 0;

    ConfigWidgetDeviceObject *pConfigWidget = 0;

    const ProjectExplorer::PeProjectObject::ObjectType eObjectType = pProjectObject->GetObjectType();
    if(eObjectType == ProjectExplorer::PeProjectObject::otProjectVersion)
    {
        if(bBayObject)
            pConfigWidget = new ConfigWidgetProjectElectrical(this);
        else
            pConfigWidget = new ConfigWidgetProjectPhysical(this);
    }
    else if(eObjectType == ProjectExplorer::PeProjectObject::otBay)
    {
        pConfigWidget = new ConfigWidgetBay(this);
    }
    else if(eObjectType == ProjectExplorer::PeProjectObject::otRoom)
    {
        pConfigWidget = new ConfigWidgetRoom(this);
    }
    else if(eObjectType == ProjectExplorer::PeProjectObject::otCubicle)
    {
        pConfigWidget = new ConfigWidgetCubicle(this);
    }
    else if(eObjectType == ProjectExplorer::PeProjectObject::otDevice)
    {
        if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                pConfigWidget = new ConfigWidgetSwitch(this);
            else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                pConfigWidget = new ConfigWidgetIEDBoard(this);
        }
    }
    else if(eObjectType == ProjectExplorer::PeProjectObject::otBoard)
    {
        pConfigWidget = new ConfigWidgetIEDPort(this);
    }

    if(pConfigWidget)
        pConfigWidget->SetupConfigWidget();

    return pConfigWidget;
}

void DevExplorerModeWidget::SlotContextMenuActionNewBay()
{
    Core::MainWindow::Instance()->SwitchMode(QLatin1String("Mode.DevExplorer"));

    bool bBayObject;
    if(ProjectExplorer::PeProjectObject *pProjectObject = Core::MainWindow::Instance()->GetContextMenuObject(&bBayObject))
    {
        Core::MainWindow::Instance()->SetActiveObject(pProjectObject, bBayObject);
        CreateBay(Core::MainWindow::Instance()->GetContextMenuObject());
    }
}

void DevExplorerModeWidget::SlotContextMenuActionNewRoom()
{
    Core::MainWindow::Instance()->SwitchMode(QLatin1String("Mode.DevExplorer"));

    bool bBayObject;
    if(ProjectExplorer::PeProjectObject *pProjectObject = Core::MainWindow::Instance()->GetContextMenuObject(&bBayObject))
    {
        Core::MainWindow::Instance()->SetActiveObject(pProjectObject, bBayObject);
        CreateRoom(pProjectObject);
    }
}

void DevExplorerModeWidget::SlotContextMenuActionNewCubicle()
{
    Core::MainWindow::Instance()->SwitchMode(QLatin1String("Mode.DevExplorer"));

    bool bBayObject;
    if(ProjectExplorer::PeProjectObject *pProjectObject = Core::MainWindow::Instance()->GetContextMenuObject(&bBayObject))
    {
        Core::MainWindow::Instance()->SetActiveObject(pProjectObject, bBayObject);
        CreateCubicle(pProjectObject);
    }
}

void DevExplorerModeWidget::SlotContextMenuActionNewIED()
{
    Core::MainWindow::Instance()->SwitchMode(QLatin1String("Mode.DevExplorer"));

    bool bBayObject;
    if(ProjectExplorer::PeProjectObject *pProjectObject = Core::MainWindow::Instance()->GetContextMenuObject(&bBayObject))
    {
        Core::MainWindow::Instance()->SetActiveObject(pProjectObject, bBayObject);
        CreateIED(pProjectObject);
    }
}

void DevExplorerModeWidget::SlotContextMenuActionNewSwitch()
{
    Core::MainWindow::Instance()->SwitchMode(QLatin1String("Mode.DevExplorer"));

    bool bBayObject;
    if(ProjectExplorer::PeProjectObject *pProjectObject = Core::MainWindow::Instance()->GetContextMenuObject(&bBayObject))
    {
        Core::MainWindow::Instance()->SetActiveObject(pProjectObject, bBayObject);
        CreateSwitch(pProjectObject);
    }
}

void DevExplorerModeWidget::SlotContextMenuActionNewBoard()
{
    Core::MainWindow::Instance()->SwitchMode(QLatin1String("Mode.DevExplorer"));

    bool bBayObject;
    if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(Core::MainWindow::Instance()->GetContextMenuObject(&bBayObject)))
    {
        Core::MainWindow::Instance()->SetActiveObject(pDevice, bBayObject);
        CreateBoard(pDevice);
    }
}

void DevExplorerModeWidget::SlotContextMenuActionNewFiberPort()
{
    Core::MainWindow::Instance()->SwitchMode(QLatin1String("Mode.DevExplorer"));

    bool bBayObject;
    if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(Core::MainWindow::Instance()->GetContextMenuObject(&bBayObject)))
    {
        if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch && pDevice->GetChildBoards().size() == 1)
        {
            Core::MainWindow::Instance()->SetActiveObject(pDevice, bBayObject);
            CreateFiberPort(pDevice->GetChildBoards().first());
        }
    }
    else if(ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(Core::MainWindow::Instance()->GetContextMenuObject(&bBayObject)))
    {
        Core::MainWindow::Instance()->SetActiveObject(pBoard, bBayObject);
        CreateFiberPort(pBoard);
    }
}

void DevExplorerModeWidget::SlotContextMenuActionDelete()
{
    Core::MainWindow::Instance()->SwitchMode(QLatin1String("Mode.DevExplorer"));

    if(ProjectExplorer::PeProjectObject *pProjectObject = Core::MainWindow::Instance()->GetContextMenuObject())
        DeleteObjects(QList<ProjectExplorer::PeProjectObject*>() << pProjectObject);
}

void DevExplorerModeWidget::SlotContextMenuActionProperty()
{
    Core::MainWindow::Instance()->SwitchMode(QLatin1String("Mode.DevExplorer"));

    if(ProjectExplorer::PeProjectObject *pProjectObject = Core::MainWindow::Instance()->GetContextMenuObject())
        UpdateObject(pProjectObject);
}

void DevExplorerModeWidget::SlotContextMenuActionCopy()
{
    m_pCopiedProjectObject = Core::MainWindow::Instance()->GetContextMenuObject();
}

void DevExplorerModeWidget::SlotContextMenuActionPaste()
{
    if(!m_pCopiedProjectObject)
        return;

    bool bBayObject;
    ProjectExplorer::PeProjectObject *pPastedProjectObject = Core::MainWindow::Instance()->GetContextMenuObject(&bBayObject);
    if(!pPastedProjectObject)
        return;

    ProjectExplorer::PeProjectVersion *pSrcProjectVersion = m_pCopiedProjectObject->GetProjectVersion();
    if(!pSrcProjectVersion)
        return;

    ProjectExplorer::PeProjectVersion *pDstProjectVersion = pPastedProjectObject->GetProjectVersion();
    if(!pDstProjectVersion)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(ProjectExplorer::PeCubicle *pSrcCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(m_pCopiedProjectObject))
    {
        ///////////////////////////////////////////////////////////////////////
        // Prepare
        ///////////////////////////////////////////////////////////////////////

        QStringList lstAllCubicleNumbers;
        foreach(ProjectExplorer::PeCubicle *pCubicle, pDstProjectVersion->GetAllCubicles())
            lstAllCubicleNumbers.append(pCubicle->GetNumber());

        QString strDstCubicleNumber = pSrcCubicle->GetNumber();
        int iIndex = 0;
        while(lstAllCubicleNumbers.contains(strDstCubicleNumber))
        {
            if(iIndex == 0)
                strDstCubicleNumber = QString("%1_copy").arg(pSrcCubicle->GetNumber());
            else
                strDstCubicleNumber = QString("%1_copy%2").arg(pSrcCubicle->GetNumber()).arg(iIndex);

            iIndex++;
        }

        ProjectExplorer::PeCubicle cubicle(*pSrcCubicle);
        cubicle.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        cubicle.SetProjectVersion(pDstProjectVersion);
        cubicle.SetNumber(strDstCubicleNumber);

        if(qobject_cast<ProjectExplorer::PeProjectVersion*>(pPastedProjectObject))
            cubicle.SetParentRoom(0);
        else if(ProjectExplorer::PeRoom *pParentRoom = qobject_cast<ProjectExplorer::PeRoom*>(pPastedProjectObject))
            cubicle.SetParentRoom(pParentRoom);
        else
            return;

        QStringList lstAllDeviceNames;
        foreach(ProjectExplorer::PeDevice *pDevice, pDstProjectVersion->GetAllDevices())
            lstAllDeviceNames.append(pDevice->GetName());

        QList<ProjectExplorer::PeDevice*> lstSrcDevices;
        QList<ProjectExplorer::PeDevice> lstDevices;
        QList<ProjectExplorer::PeBoard> lstBoards;
        QList<ProjectExplorer::PePort> lstPorts;
        QList<ProjectExplorer::PeVTerminal> lstVTerminals;
        QList<ProjectExplorer::PeStrap> lstStraps;
        foreach(ProjectExplorer::PeDevice *pDevice, pSrcCubicle->GetChildDevices())
        {
            QString strDstDeviceName = pDevice->GetName();
            int iIndex = 0;
            while(lstAllDeviceNames.contains(strDstDeviceName))
            {
                if(iIndex == 0)
                    strDstDeviceName = QString("%1_copy").arg(pDevice->GetName());
                else
                    strDstDeviceName = QString("%1_copy%2").arg(pDevice->GetName()).arg(iIndex);

                iIndex++;
            }
            lstAllDeviceNames.append(strDstDeviceName);

            ProjectExplorer::PeDevice deviceTemp(*pDevice);
            deviceTemp.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            deviceTemp.SetProjectVersion(pDstProjectVersion);
            deviceTemp.SetParentCubicle(&cubicle);
            if(pSrcProjectVersion != pDstProjectVersion)
                deviceTemp.SetParentBay(0);
            deviceTemp.SetName(strDstDeviceName);
            lstDevices.append(deviceTemp);
            lstSrcDevices.append(pDevice);

            ProjectExplorer::PeDevice &device = lstDevices.last();
            foreach(ProjectExplorer::PeBoard *pBoard, pDevice->GetChildBoards())
            {
                ProjectExplorer::PeBoard boardTemp(*pBoard);
                boardTemp.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                boardTemp.SetProjectVersion(pDstProjectVersion);
                boardTemp.SetParentDevice(&device);
                lstBoards.append(boardTemp);

                ProjectExplorer::PeBoard &board = lstBoards.last();
                foreach(ProjectExplorer::PePort *pPort, pBoard->GetChildPorts())
                {
                    ProjectExplorer::PePort port(*pPort);
                    port.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                    port.SetProjectVersion(pDstProjectVersion);
                    port.SetParentBoard(&board);
                    lstPorts.append(port);
                }
            }

            foreach(ProjectExplorer::PeVTerminal *pVTerminal, pDevice->GetVTerminals())
            {
                ProjectExplorer::PeVTerminal vterminal(*pVTerminal);
                vterminal.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                vterminal.SetProjectVersion(pDstProjectVersion);
                vterminal.SetParentDevice(&device);
                lstVTerminals.append(vterminal);
            }

            foreach(ProjectExplorer::PeStrap *pStrap, pDevice->GetStraps())
            {
                ProjectExplorer::PeStrap strap(*pStrap);
                strap.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                strap.SetProjectVersion(pDstProjectVersion);
                strap.SetParentDevice(&device);
                lstStraps.append(strap);
            }
        }

        ///////////////////////////////////////////////////////////////////////
        // Handle Database
        ///////////////////////////////////////////////////////////////////////
        ProjectExplorer::DbTransaction DbTrans(pDstProjectVersion, true);

        // Create new cubicle
        if(!pDstProjectVersion->DbCreateObject(cubicle, false))
        {
            DbTrans.Rollback();
            return;
        }

        // Create new devices
        for(int i = 0; i < lstDevices.size(); i++)
        {
            ProjectExplorer::PeDevice *pSrcDevice = lstSrcDevices.at(i);
            ProjectExplorer::PeDevice &device = lstDevices[i];

            if(!pDstProjectVersion->DbCreateObject(device, false))
            {
                DbTrans.Rollback();
                return;
            }

            QByteArray baModelContent;
            if(!pSrcProjectVersion->DbReadDeviceModel(pSrcDevice->GetId(), baModelContent) ||
               !pDstProjectVersion->DbUpdateDeviceModel(device.GetId(), baModelContent, false))
            {
                DbTrans.Rollback();
                return;
            }
        }

        // Create new boards
        for(int i = 0; i < lstBoards.size(); i++)
        {
            ProjectExplorer::PeBoard &board = lstBoards[i];
            if(!pDstProjectVersion->DbCreateObject(board, false))
            {
                DbTrans.Rollback();
                return;
            }
        }

        // Create new ports
        for(int i = 0; i < lstPorts.size(); i++)
        {
            ProjectExplorer::PePort &port = lstPorts[i];
            if(!pDstProjectVersion->DbCreateObject(port, false))
            {
                DbTrans.Rollback();
                return;
            }
        }

        // Create new vterminals
        for(int i = 0; i < lstVTerminals.size(); i++)
        {
            ProjectExplorer::PeVTerminal &vterminal = lstVTerminals[i];
            if(!pDstProjectVersion->DbCreateObject(vterminal, false))
            {
                DbTrans.Rollback();
                return;
            }
        }

        // Create new straps
        for(int i = 0; i < lstStraps.size(); i++)
        {
            ProjectExplorer::PeStrap &strap = lstStraps[i];
            if(!pDstProjectVersion->DbCreateObject(strap, false))
            {
                DbTrans.Rollback();
                return;
            }
        }

        if(!DbTrans.Commit())
            return;

        ///////////////////////////////////////////////////////////////////////
        // Handle Object
        ///////////////////////////////////////////////////////////////////////

        ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pDstProjectVersion->CreateObject(cubicle));
        if(!pCubicle)
            return;

        // Create new devices
        QMap<ProjectExplorer::PeDevice*, ProjectExplorer::PeDevice*> mapTempToCreateDevice;
        for(int i = 0; i < lstDevices.size(); i++)
        {
            ProjectExplorer::PeDevice *pSrcDevice = lstSrcDevices.at(i);
            ProjectExplorer::PeDevice &device = lstDevices[i];

            device.SetParentCubicle(pCubicle);

            ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pDstProjectVersion->CreateObject(device));
            if(!pDevice)
                return;
            pDstProjectVersion->UpdateDeviceModelStatus(pDevice, pSrcDevice->GetHasModel());

            mapTempToCreateDevice.insert(&device, pDevice);
        }

        // Create new boards
        QMap<ProjectExplorer::PeBoard*, ProjectExplorer::PeBoard*> mapTempToCreateBoard;
        for(int i = 0; i < lstBoards.size(); i++)
        {
            ProjectExplorer::PeBoard &board = lstBoards[i];

            if(ProjectExplorer::PeDevice *pDevice = mapTempToCreateDevice.value(board.GetParentDevice(), 0))
                board.SetParentDevice(pDevice);

            ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pDstProjectVersion->CreateObject(board));
            if(!pBoard)
                return;

            mapTempToCreateBoard.insert(&board, pBoard);
        }

        // Create new ports
        for(int i = 0; i < lstPorts.size(); i++)
        {
            ProjectExplorer::PePort &port = lstPorts[i];

            if(ProjectExplorer::PeBoard *pBoard = mapTempToCreateBoard.value(port.GetParentBoard(), 0))
                port.SetParentBoard(pBoard);

            ProjectExplorer::PePort *pPort = qobject_cast<ProjectExplorer::PePort*>(pDstProjectVersion->CreateObject(port));
            if(!pPort)
                return;
        }

        // Create new vterminals
        for(int i = 0; i < lstVTerminals.size(); i++)
        {
            ProjectExplorer::PeVTerminal &vterminal = lstVTerminals[i];

            if(ProjectExplorer::PeDevice *pDevice = mapTempToCreateDevice.value(vterminal.GetParentDevice(), 0))
                vterminal.SetParentDevice(pDevice);

            ProjectExplorer::PeVTerminal *pVTerminal = qobject_cast<ProjectExplorer::PeVTerminal*>(pDstProjectVersion->CreateObject(vterminal));
            if(!pVTerminal)
                return;
        }

        // Create new straps
        for(int i = 0; i < lstStraps.size(); i++)
        {
            ProjectExplorer::PeStrap &strap = lstStraps[i];

            if(ProjectExplorer::PeDevice *pDevice = mapTempToCreateDevice.value(strap.GetParentDevice(), 0))
                strap.SetParentDevice(pDevice);

            ProjectExplorer::PeStrap *pStrap = qobject_cast<ProjectExplorer::PeStrap*>(pDstProjectVersion->CreateObject(strap));
            if(!pStrap)
                return;
        }
    }
    else if(ProjectExplorer::PeDevice *pSrcDevice = qobject_cast<ProjectExplorer::PeDevice*>(m_pCopiedProjectObject))
    {
        ///////////////////////////////////////////////////////////////////////
        // Prepare
        ///////////////////////////////////////////////////////////////////////
        QStringList lstAllDeviceNames;
        foreach(ProjectExplorer::PeDevice *pDevice, pDstProjectVersion->GetAllDevices())
            lstAllDeviceNames.append(pDevice->GetName());

        QString strDstDeviceName = pSrcDevice->GetName();
        int iIndex = 0;
        while(lstAllDeviceNames.contains(strDstDeviceName))
        {
            if(iIndex == 0)
                strDstDeviceName = QString("%1_copy").arg(pSrcDevice->GetName());
            else
                strDstDeviceName = QString("%1_copy%2").arg(pSrcDevice->GetName()).arg(iIndex);

            iIndex++;
        }

        ProjectExplorer::PeDevice device(*pSrcDevice);
        device.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        device.SetProjectVersion(pDstProjectVersion);
        device.SetName(strDstDeviceName);

        if(qobject_cast<ProjectExplorer::PeProjectVersion*>(pPastedProjectObject))
        {
            if(pSrcProjectVersion != pDstProjectVersion)
            {
                device.SetParentBay(0);
                device.SetParentCubicle(0);
            }
            else
            {
                if(bBayObject)
                    device.SetParentBay(0);
                else
                    device.SetParentCubicle(0);
            }
        }
        else if(ProjectExplorer::PeCubicle *pParentCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pPastedProjectObject))
        {
            device.SetParentCubicle(pParentCubicle);
            if(pSrcProjectVersion != pDstProjectVersion)
                device.SetParentBay(0);
        }
        else if(ProjectExplorer::PeBay *pParentBay = qobject_cast<ProjectExplorer::PeBay*>(pPastedProjectObject))
        {
            device.SetParentBay(pParentBay);
            if(pSrcProjectVersion != pDstProjectVersion)
                device.SetParentCubicle(0);
        }
        else
        {
            return;
        }

        QList<ProjectExplorer::PeBoard> lstBoards;
        QList<ProjectExplorer::PePort> lstPorts;
        foreach(ProjectExplorer::PeBoard *pBoard, pSrcDevice->GetChildBoards())
        {
            ProjectExplorer::PeBoard boardTemp(*pBoard);
            boardTemp.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            boardTemp.SetProjectVersion(pDstProjectVersion);
            boardTemp.SetParentDevice(&device);
            lstBoards.append(boardTemp);

            ProjectExplorer::PeBoard &board = lstBoards.last();
            foreach(ProjectExplorer::PePort *pPort, pBoard->GetChildPorts())
            {
                ProjectExplorer::PePort port(*pPort);
                port.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                port.SetProjectVersion(pDstProjectVersion);
                port.SetParentBoard(&board);
                lstPorts.append(port);
            }
        }

        QList<ProjectExplorer::PeVTerminal> lstVTerminals;
        foreach(ProjectExplorer::PeVTerminal *pVTerminal, pSrcDevice->GetVTerminals())
        {
            ProjectExplorer::PeVTerminal vterminal(*pVTerminal);
            vterminal.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            vterminal.SetProjectVersion(pDstProjectVersion);
            vterminal.SetParentDevice(&device);
            lstVTerminals.append(vterminal);
        }

        QList<ProjectExplorer::PeStrap> lstStraps;
        foreach(ProjectExplorer::PeStrap *pStrap, pSrcDevice->GetStraps())
        {
            ProjectExplorer::PeStrap strap(*pStrap);
            strap.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            strap.SetProjectVersion(pDstProjectVersion);
            strap.SetParentDevice(&device);
            lstStraps.append(strap);
        }

        ///////////////////////////////////////////////////////////////////////
        // Handle Database
        ///////////////////////////////////////////////////////////////////////
        ProjectExplorer::DbTransaction DbTrans(pDstProjectVersion, true);

        // Create new devices
        if(!pDstProjectVersion->DbCreateObject(device, false))
        {
            DbTrans.Rollback();
            return;
        }

        QByteArray baModelContent;
        if(!pSrcProjectVersion->DbReadDeviceModel(pSrcDevice->GetId(), baModelContent) ||
           !pDstProjectVersion->DbUpdateDeviceModel(device.GetId(), baModelContent, false))
        {
            DbTrans.Rollback();
            return;
        }

        // Create new boards
        for(int i = 0; i < lstBoards.size(); i++)
        {
            ProjectExplorer::PeBoard &board = lstBoards[i];
            if(!pDstProjectVersion->DbCreateObject(board, false))
            {
                DbTrans.Rollback();
                return;
            }
        }

        // Create new ports
        for(int i = 0; i < lstPorts.size(); i++)
        {
            ProjectExplorer::PePort &port = lstPorts[i];
            if(!pDstProjectVersion->DbCreateObject(port, false))
            {
                DbTrans.Rollback();
                return;
            }
        }

        // Create new vterminals
        for(int i = 0; i < lstVTerminals.size(); i++)
        {
            ProjectExplorer::PeVTerminal &vterminal = lstVTerminals[i];
            if(!pDstProjectVersion->DbCreateObject(vterminal, false))
            {
                DbTrans.Rollback();
                return;
            }
        }

        // Create new straps
        for(int i = 0; i < lstStraps.size(); i++)
        {
            ProjectExplorer::PeStrap &strap = lstStraps[i];
            if(!pDstProjectVersion->DbCreateObject(strap, false))
            {
                DbTrans.Rollback();
                return;
            }
        }

        if(!DbTrans.Commit())
            return;

        ///////////////////////////////////////////////////////////////////////
        // Handle Object
        ///////////////////////////////////////////////////////////////////////

        // Create new devices
        ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pDstProjectVersion->CreateObject(device));
        if(!pDevice)
            return;
        pDstProjectVersion->UpdateDeviceModelStatus(pDevice, pSrcDevice->GetHasModel());

        // Create new boards
        QMap<ProjectExplorer::PeBoard*, ProjectExplorer::PeBoard*> mapTempToCreateBoard;
        for(int i = 0; i < lstBoards.size(); i++)
        {
            ProjectExplorer::PeBoard &board = lstBoards[i];
            board.SetParentDevice(pDevice);

            ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pDstProjectVersion->CreateObject(board));
            if(!pBoard)
                return;

            mapTempToCreateBoard.insert(&board, pBoard);
        }

        // Create new ports
        for(int i = 0; i < lstPorts.size(); i++)
        {
            ProjectExplorer::PePort &port = lstPorts[i];

            if(ProjectExplorer::PeBoard *pBoard = mapTempToCreateBoard.value(port.GetParentBoard(), 0))
                port.SetParentBoard(pBoard);

            ProjectExplorer::PePort *pPort = qobject_cast<ProjectExplorer::PePort*>(pDstProjectVersion->CreateObject(port));
            if(!pPort)
                return;
        }

        // Create new vterminals
        for(int i = 0; i < lstVTerminals.size(); i++)
        {
            ProjectExplorer::PeVTerminal &vterminal = lstVTerminals[i];
            vterminal.SetParentDevice(pDevice);

            ProjectExplorer::PeVTerminal *pVTerminal = qobject_cast<ProjectExplorer::PeVTerminal*>(pDstProjectVersion->CreateObject(vterminal));
            if(!pVTerminal)
                return;
        }

        // Create new straps
        for(int i = 0; i < lstStraps.size(); i++)
        {
            ProjectExplorer::PeStrap &strap = lstStraps[i];
            strap.SetParentDevice(pDevice);

            ProjectExplorer::PeStrap *pStrap = qobject_cast<ProjectExplorer::PeStrap*>(pDstProjectVersion->CreateObject(strap));
            if(!pStrap)
                return;
        }
    }
    else if(ProjectExplorer::PeBoard *pSrcBoard = qobject_cast<ProjectExplorer::PeBoard*>(m_pCopiedProjectObject))
    {
        if(ProjectExplorer::PeDevice *pParentDevice = qobject_cast<ProjectExplorer::PeDevice*>(pPastedProjectObject))
        {
            if(pParentDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtIED)
                return;

            ///////////////////////////////////////////////////////////////////////
            // Prepare
            ///////////////////////////////////////////////////////////////////////
            QStringList lstAllPositions;
            foreach(ProjectExplorer::PeBoard *pBoard, pParentDevice->GetChildBoards())
                lstAllPositions.append(pBoard->GetPosition());

            QString strDstPosition = pSrcBoard->GetPosition();
            int iCopyIndex = 0;
            while(lstAllPositions.contains(strDstPosition))
            {
                if(iCopyIndex == 0)
                    strDstPosition = QString("%1_copy").arg(pSrcBoard->GetPosition());
                else
                    strDstPosition = QString("%1_copy%2").arg(pSrcBoard->GetPosition()).arg(iCopyIndex);

                iCopyIndex++;
            }

            ProjectExplorer::PeBoard board(*pSrcBoard);
            board.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            board.SetProjectVersion(pDstProjectVersion);
            board.SetParentDevice(pParentDevice);
            board.SetPosition(strDstPosition);

            QList<ProjectExplorer::PePort> lstPorts;
            foreach(ProjectExplorer::PePort *pPort, pSrcBoard->GetChildPorts())
            {
                ProjectExplorer::PePort port(*pPort);
                port.SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                port.SetProjectVersion(pDstProjectVersion);
                port.SetParentBoard(&board);
                lstPorts.append(port);
            }

            ///////////////////////////////////////////////////////////////////////
            // Handle Database
            ///////////////////////////////////////////////////////////////////////
            ProjectExplorer::DbTransaction DbTrans(pDstProjectVersion, true);

            // Create new board
            if(!pDstProjectVersion->DbCreateObject(board, false))
            {
                DbTrans.Rollback();
                return;
            }

            // Create new ports
            for(int i = 0; i < lstPorts.size(); i++)
            {
                ProjectExplorer::PePort &port = lstPorts[i];
                if(!pDstProjectVersion->DbCreateObject(port, false))
                {
                    DbTrans.Rollback();
                    return;
                }
            }

            if(!DbTrans.Commit())
                return;

            ///////////////////////////////////////////////////////////////////////
            // Handle Object
            ///////////////////////////////////////////////////////////////////////

            if(ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pDstProjectVersion->CreateObject(board)))
            {
                for(int i = 0; i < lstPorts.size(); i++)
                {
                    ProjectExplorer::PePort &port = lstPorts[i];
                    port.SetParentBoard(pBoard);

                    pDstProjectVersion->CreateObject(port);
                }
            }
        }
    }
}
