#include <QAction>
#include <QMenu>

#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pedevice.h"

#include "core/configmodelview.h"

#include "configwidgetprojectphysical.h"
#include "devexplorermodewidget.h"

static const char* const g_szColumnSettingName  = "ProjectPhysical";

using namespace DevExplorer::Internal;

ConfigWidgetProjectPhysical::ConfigWidgetProjectPhysical(QWidget *pParent) :
    ConfigWidgetDeviceObject(pParent),
    m_pActionNewRoom(0),
    m_pActionNewCubicle(0),
    m_pActionNewIED(0),
    m_pActionNewSwitch(0),
    m_pActionDelete(0),
    m_pActionProperty(0)
{
    m_lstColumnInfo << ConfigWidget::COLUMN_INFO(tr("Name"), 300, true)
                    << ConfigWidget::COLUMN_INFO(tr("Type"), 600, true);

    ReadSettings(g_szColumnSettingName);
}

ConfigWidgetProjectPhysical::~ConfigWidgetProjectPhysical()
{
    SaveSettings(g_szColumnSettingName);
}

bool ConfigWidgetProjectPhysical::BuildModelData()
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = qobject_cast<ProjectExplorer::PeProjectVersion*>(m_pProjectObject);
    if(!pProjectVersion)
        return false;

    foreach(ProjectExplorer::PeRoom *pRoom, pProjectVersion->GetAllRooms())
    {
        m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pRoom), pRoom));
    }

    foreach(ProjectExplorer::PeCubicle *pCubicle, pProjectVersion->GetAllCubicles())
    {
        if(!pCubicle->GetParentRoom())
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pCubicle), pCubicle));
    }

    foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
    {
        if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF && !pDevice->GetParentCubicle())
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pDevice), pDevice));
    }

    return true;
}

QStringList ConfigWidgetProjectPhysical::RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    return QStringList() << pProjectObject->GetDisplayName()
                         << pProjectObject->GetObjectTypeName();
}

QList<QAction*> ConfigWidgetProjectPhysical::GetContextMenuActions()
{
    QList<QAction*> lstContextMenuActons;
    QMenu *pMenuAddNew = new QMenu(tr("&Add New"), this);

    if(!m_pActionNewRoom)
    {
        m_pActionNewRoom = new QAction(tr("New %1...").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otRoom)), this);
        m_pActionNewRoom->setEnabled(true);
        connect(m_pActionNewRoom, SIGNAL(triggered()), this, SLOT(SlotActionNewRoom()));
    }
    pMenuAddNew->addAction(m_pActionNewRoom);

    if(!m_pActionNewCubicle)
    {
        m_pActionNewCubicle = new QAction(tr("New %1...").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otCubicle)), this);
        m_pActionNewCubicle->setEnabled(false);
        connect(m_pActionNewCubicle, SIGNAL(triggered()), this, SLOT(SlotActionNewCubicle()));
    }
    pMenuAddNew->addAction(m_pActionNewCubicle);

    if(!m_pActionNewIED)
    {
        m_pActionNewIED = new QAction(tr("New %1...").arg(ProjectExplorer::PeDevice::GetDeviceTypeName(ProjectExplorer::PeDevice::dtIED)), this);
        m_pActionNewIED->setEnabled(false);
        connect(m_pActionNewIED, SIGNAL(triggered()), this, SLOT(SlotActionNewIED()));
    }
    pMenuAddNew->addAction(m_pActionNewIED);

    if(!m_pActionNewSwitch)
    {
        m_pActionNewSwitch = new QAction(tr("New %1...").arg(ProjectExplorer::PeDevice::GetDeviceTypeName(ProjectExplorer::PeDevice::dtSwitch)), this);
        m_pActionNewSwitch->setEnabled(false);
        connect(m_pActionNewSwitch, SIGNAL(triggered()), this, SLOT(SlotActionNewSwitch()));
    }
    pMenuAddNew->addAction(m_pActionNewSwitch);

    lstContextMenuActons.append(pMenuAddNew->menuAction());

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

QList<QAction*> ConfigWidgetProjectPhysical::GetToolbarActions()
{
    QList<QAction*> lstToolbarActions;

    QMenu *pMenu = new QMenu(tr("Add New"), this);
    if(m_pActionNewRoom)
        pMenu->addAction(m_pActionNewRoom);

    if(m_pActionNewCubicle)
        pMenu->addAction(m_pActionNewCubicle);

    if(m_pActionNewIED)
        pMenu->addAction(m_pActionNewIED);

    if(m_pActionNewSwitch)
        pMenu->addAction(m_pActionNewSwitch);

    lstToolbarActions.append(pMenu->menuAction());
    pMenu->menuAction()->setIcon(QIcon(":/devexplorer/images/oper_add.png"));
    connect(pMenu->menuAction(), SIGNAL(triggered()), this, SLOT(SlotActionNewRoom()));

    if(m_pActionDelete)
        lstToolbarActions.append(m_pActionDelete);

    if(m_pActionProperty)
        lstToolbarActions.append(m_pActionProperty);

    return lstToolbarActions;
}

void ConfigWidgetProjectPhysical::UpdateActions(const QModelIndexList &lstSelectedIndex)
{
    m_pActionNewRoom->setEnabled(true);
    m_pActionNewCubicle->setEnabled(true);
    m_pActionNewIED->setEnabled(true);
    m_pActionNewSwitch->setEnabled(true);

    m_pActionDelete->setEnabled(!lstSelectedIndex.isEmpty());
    m_pActionProperty->setEnabled(lstSelectedIndex.size() == 1);
}

void ConfigWidgetProjectPhysical::PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject)
{
    DevExplorerModeWidget::Instance()->UpdateObject(pProjectObject);
}

void ConfigWidgetProjectPhysical::SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(pProjectObject->GetProjectVersion() != m_pProjectObject)
        return;

    const ProjectExplorer::PeProjectObject::ObjectType eObjectType = pProjectObject->GetObjectType();
    if(eObjectType == ProjectExplorer::PeProjectObject::otRoom)
    {
        if(qobject_cast<ProjectExplorer::PeRoom*>(pProjectObject))
        {
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pProjectObject), pProjectObject));;
        }
    }
    else if(eObjectType == ProjectExplorer::PeProjectObject::otCubicle)
    {
        if(ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectObject))
        {
            if(pCubicle->GetParentRoom() == 0)
                m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pProjectObject), pProjectObject));;
        }
    }
    else if(eObjectType == ProjectExplorer::PeProjectObject::otDevice)
    {
        if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
        {
            if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF && pDevice->GetParentCubicle() == 0)
                m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pProjectObject), pProjectObject));;
        }
    }
}

void ConfigWidgetProjectPhysical::SlotProjectObjectParentChanged(ProjectExplorer::PeProjectObject *pProjectObject, ProjectExplorer::PeProjectObject *pOldParentProjectObject)
{
    if(pProjectObject->GetProjectVersion() != m_pProjectObject)
        return;

    const ProjectExplorer::PeProjectObject::ObjectType eObjectType = pProjectObject->GetObjectType();
    if(eObjectType == ProjectExplorer::PeProjectObject::otCubicle)
    {
        if(ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectObject))
        {
            ProjectExplorer::PeRoom *pNewParentRoom = pCubicle->GetParentRoom();
            ProjectExplorer::PeRoom *pOldParentRoom = qobject_cast<ProjectExplorer::PeRoom*>(pOldParentProjectObject);

            if(pNewParentRoom == 0)
            {
                m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pProjectObject), pProjectObject));;
            }
            else if(pOldParentRoom == 0)
            {
                if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromProjectObject(pProjectObject))
                    m_pModel->RemoveRow(pConfigRow);
            }
        }
    }
    else if(eObjectType == ProjectExplorer::PeProjectObject::otDevice)
    {
        if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
        {
            ProjectExplorer::PeCubicle *pNewParentCubicle = pDevice->GetParentCubicle();
            ProjectExplorer::PeCubicle *pOldParentCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pOldParentProjectObject);

            if(pNewParentCubicle == 0)
            {
                m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pProjectObject), pProjectObject));
            }
            else if(pOldParentCubicle == 0)
            {
                if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromProjectObject(pProjectObject))
                    m_pModel->RemoveRow(pConfigRow);
            }
        }
    }
}

void ConfigWidgetProjectPhysical::SlotActionNewRoom()
{
    DevExplorerModeWidget::Instance()->CreateRoom(m_pProjectObject);
}

void ConfigWidgetProjectPhysical::SlotActionNewCubicle()
{
    DevExplorerModeWidget::Instance()->CreateCubicle(m_pProjectObject);
}

void ConfigWidgetProjectPhysical::SlotActionNewIED()
{
    DevExplorerModeWidget::Instance()->CreateIED(m_pProjectObject);
}

void ConfigWidgetProjectPhysical::SlotActionNewSwitch()
{
    DevExplorerModeWidget::Instance()->CreateSwitch(m_pProjectObject);
}

void ConfigWidgetProjectPhysical::SlotActionDelete()
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

void ConfigWidgetProjectPhysical::SlotActionProperty()
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
