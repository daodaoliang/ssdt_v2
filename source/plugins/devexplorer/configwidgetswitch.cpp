#include <QAction>
#include <QMenu>

#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/peport.h"

#include "core/configmodelview.h"

#include "configwidgetswitch.h"
#include "devexplorermodewidget.h"

static const char* const g_szColumnSettingName  = "Switch";

using namespace DevExplorer::Internal;

ConfigWidgetSwitch::ConfigWidgetSwitch(QWidget *pParent) :
    ConfigWidgetDeviceObject(pParent),
    m_pActionNewPort(0),
    m_pActionDelete(0),
    m_pActionProperty(0)
{
    m_lstColumnInfo << ConfigWidget::COLUMN_INFO(tr("Name"), 150, true)
                    << ConfigWidget::COLUMN_INFO(tr("Index"), 100, true)
                    << ConfigWidget::COLUMN_INFO(tr("Type"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Direction"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Fiber Plug"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Fiber Mode"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Cascade"), 100, true);

    ReadSettings(g_szColumnSettingName);
}

ConfigWidgetSwitch::~ConfigWidgetSwitch()
{
    SaveSettings(g_szColumnSettingName);
}

bool ConfigWidgetSwitch::BuildModelData()
{
    ProjectExplorer::PeBoard *pBoard = GetSwitchBoard();
    if(!pBoard)
        return false;

    foreach(ProjectExplorer::PePort *pPort, pBoard->GetChildPorts())
        m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pPort), pPort));

    return true;
}

QStringList ConfigWidgetSwitch::RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    QStringList lstRowData;

    if(ProjectExplorer::PePort *pPort = qobject_cast<ProjectExplorer::PePort*>(pProjectObject))
    {
        lstRowData << pPort->GetDisplayName()
                   << QString("%1").arg(pPort->GetGroup(), 2, 10, QLatin1Char('0'))
                   << ProjectExplorer::PePort::GetPortTypeName(pPort->GetPortType())
                   << ProjectExplorer::PePort::GetPortDirectionName(pPort->GetPortDirection())
                   << ProjectExplorer::PePort::GetFiberPlugName(pPort->GetFiberPlug())
                   << ProjectExplorer::PePort::GetFiberModeName(pPort->GetFiberMode())
                   << (pPort->GetCascade() ? tr("True") : tr("False"));
    }

    return lstRowData;
}

QList<QAction*> ConfigWidgetSwitch::GetContextMenuActions()
{
    QList<QAction*> lstContextMenuActons;

    if(!m_pActionNewPort)
    {
        m_pActionNewPort = new QAction(QIcon(":/devexplorer/images/oper_add.png"), tr("New %1...").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otPort)), this);
        m_pActionNewPort->setEnabled(true);
        connect(m_pActionNewPort, SIGNAL(triggered()), this, SLOT(SlotActionNewPort()));
    }
    lstContextMenuActons.append(m_pActionNewPort);

    QAction *pSeparator = new QAction(this);
    pSeparator->setSeparator(true);
    lstContextMenuActons.append(pSeparator);

    if(!m_pActionDelete)
    {
        m_pActionDelete = new QAction(QIcon(":/devexplorer/images/oper_remove.png"), tr("&Delete..."), this);
        m_pActionDelete->setEnabled(false);
        connect(m_pActionDelete, SIGNAL(triggered()), this, SLOT(SlotActionDelete()));
    }
    lstContextMenuActons.append(m_pActionDelete);

    pSeparator = new QAction(this);
    pSeparator->setSeparator(true);
    lstContextMenuActons.append(pSeparator);

    if(!m_pActionProperty)
    {
        m_pActionProperty = new QAction(QIcon(":/devexplorer/images/oper_edit.png"), tr("&Property..."), this);
        m_pActionProperty->setEnabled(false);
        connect(m_pActionProperty, SIGNAL(triggered()), this, SLOT(SlotActionProperty()));
    }
    lstContextMenuActons.append(m_pActionProperty);

    return lstContextMenuActons;
}

QList<QAction*> ConfigWidgetSwitch::GetToolbarActions()
{
    QList<QAction*> lstToolbarActions;

    if(m_pActionNewPort)
        lstToolbarActions.append(m_pActionNewPort);

    if(m_pActionDelete)
        lstToolbarActions.append(m_pActionDelete);

    if(m_pActionProperty)
        lstToolbarActions.append(m_pActionProperty);

    return lstToolbarActions;
}

void ConfigWidgetSwitch::UpdateActions(const QModelIndexList &lstSelectedIndex)
{
    m_pActionNewPort->setEnabled(true);
    m_pActionDelete->setEnabled(!lstSelectedIndex.isEmpty());
    m_pActionProperty->setEnabled(lstSelectedIndex.size() == 1);
}

void ConfigWidgetSwitch::PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject)
{
    DevExplorerModeWidget::Instance()->UpdateObject(pProjectObject);
}

ProjectExplorer::PeBoard* ConfigWidgetSwitch::GetSwitchBoard() const
{
    ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(m_pProjectObject);
    if(!pDevice || pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtSwitch)
        return 0;

    if(pDevice->GetChildBoards().isEmpty())
    {
        ProjectExplorer::PeProjectVersion *pProjectVersion = pDevice->GetProjectVersion();
        if(!pProjectVersion)
            return 0;

        ProjectExplorer::PeBoard board;
        board.SetProjectVersion(pProjectVersion);
        board.SetPosition("B01");
        board.SetType("SWITCH");
        board.SetParentDevice(pDevice);

        if(pProjectVersion->DbCreateObject(board, true))
            pProjectVersion->CreateObject(board);
    }

    return pDevice->GetChildBoards().size() == 1 ? pDevice->GetChildBoards().first() : 0;
}

void ConfigWidgetSwitch::SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(ProjectExplorer::PePort *pPort = qobject_cast<ProjectExplorer::PePort*>(pProjectObject))
    {
        if(pPort->GetParentBoard() && pPort->GetParentBoard() == GetSwitchBoard())
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pPort), pPort));
    }
}

void ConfigWidgetSwitch::SlotActionNewPort()
{
    if(ProjectExplorer::PeBoard *pBoard = GetSwitchBoard())
        DevExplorerModeWidget::Instance()->CreateFiberPort(pBoard);
}

void ConfigWidgetSwitch::SlotActionDelete()
{
    QModelIndexList lstSelectedIndex = m_pView->selectionModel()->selectedRows();
    qSort(lstSelectedIndex);

    QList<ProjectExplorer::PeProjectObject*> lstDeleteProjectObject;
    foreach(const QModelIndex &index, lstSelectedIndex)
    {
        if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromIndex(index))
        {
            if(ProjectExplorer::PeProjectObject *pProjectObject = pConfigRow->GetProjectObject())
                lstDeleteProjectObject.append(pProjectObject);
        }
    }

    DevExplorerModeWidget::Instance()->DeleteObjects(lstDeleteProjectObject);
}

void ConfigWidgetSwitch::SlotActionProperty()
{
    QItemSelectionModel *pSelectionModel = m_pView->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
    if(lstSelectedIndex.size() == 1)
    {
        if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromIndex(lstSelectedIndex.first()))
        {
            if(ProjectExplorer::PeProjectObject *pProjectObject = pConfigRow->GetProjectObject())
                PropertyRequested(pProjectObject);
        }
    }
}
