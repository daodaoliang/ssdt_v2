#include <QAction>
#include <QMenu>


#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pedevice.h"

#include "core/configmodelview.h"

#include "configwidgetcubicle.h"
#include "devexplorermodewidget.h"

static const char* const g_szColumnSettingName  = "Cubicle";

using namespace DevExplorer::Internal;

ConfigWidgetCubicle::ConfigWidgetCubicle(QWidget *pParent) :
    ConfigWidgetDeviceObject(pParent),
    m_pActionNewIED(0),
    m_pActionNewSwitch(0),
    m_pActionDelete(0),
    m_pActionProperty(0)
{
    m_lstColumnInfo << ConfigWidget::COLUMN_INFO(tr("Name"), 150, true)
                    << ConfigWidget::COLUMN_INFO(tr("Type"), 100, true)
                    << ConfigWidget::COLUMN_INFO(tr("Manufacture"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Voltage Level"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Cubicle Position"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Cubicle Number"), 200, true);

    ReadSettings(g_szColumnSettingName);
}

ConfigWidgetCubicle::~ConfigWidgetCubicle()
{
    SaveSettings(g_szColumnSettingName);
}

bool ConfigWidgetCubicle::BuildModelData()
{
    ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(m_pProjectObject);
    if(!pCubicle)
        return false;

    foreach(ProjectExplorer::PeDevice *pDevice, pCubicle->GetChildDevices())
    {
        if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pDevice), pDevice));
    }

    return true;
}

QStringList ConfigWidgetCubicle::RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    QStringList lstRowData;

    if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
    {
        lstRowData << pDevice->GetDisplayName()
                   << pDevice->GetObjectTypeName()
                   << pDevice->GetManufacture()
                   << (pDevice->GetVLevel() == 0 ? tr("None") : QString::number(pDevice->GetVLevel()))
                   << pDevice->GetCubiclePos()
                   << pDevice->GetCubicleNum();
    }

    return lstRowData;
}

QList<QAction*> ConfigWidgetCubicle::GetContextMenuActions()
{
    QList<QAction*> lstContextMenuActons;
    QMenu *pMenuAddNew = new QMenu(tr("&Add New"), this);

    if(!m_pActionNewIED)
    {
        m_pActionNewIED = new QAction(tr("New %1...").arg(ProjectExplorer::PeDevice::GetDeviceTypeName(ProjectExplorer::PeDevice::dtIED)), this);
        m_pActionNewIED->setEnabled(true);
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

QList<QAction*> ConfigWidgetCubicle::GetToolbarActions()
{
    QList<QAction*> lstToolbarActions;

    QMenu *pMenu = new QMenu(tr("Add New"), this);
    if(m_pActionNewIED)
        pMenu->addAction(m_pActionNewIED);

    if(m_pActionNewSwitch)
        pMenu->addAction(m_pActionNewSwitch);

    lstToolbarActions.append(pMenu->menuAction());
    pMenu->menuAction()->setIcon(QIcon(":/devexplorer/images/oper_add.png"));
    connect(pMenu->menuAction(), SIGNAL(triggered()), this, SLOT(SlotActionNewIED()));

    if(m_pActionDelete)
        lstToolbarActions.append(m_pActionDelete);

    if(m_pActionProperty)
        lstToolbarActions.append(m_pActionProperty);

    return lstToolbarActions;
}

void ConfigWidgetCubicle::UpdateActions(const QModelIndexList &lstSelectedIndex)
{
    m_pActionNewIED->setEnabled(true);
    m_pActionNewSwitch->setEnabled(true);
    m_pActionDelete->setEnabled(!lstSelectedIndex.isEmpty());
    m_pActionProperty->setEnabled(lstSelectedIndex.size() == 1);
}

void ConfigWidgetCubicle::PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject)
{
    DevExplorerModeWidget::Instance()->UpdateObject(pProjectObject);
}

void ConfigWidgetCubicle::SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
    {
        if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF && pDevice->GetParentCubicle() == m_pProjectObject)
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pDevice), pDevice));
    }
}

void ConfigWidgetCubicle::SlotProjectObjectParentChanged(ProjectExplorer::PeProjectObject *pProjectObject, ProjectExplorer::PeProjectObject *pOldParentProjectObject)
{
    if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
    {
        if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
            return;

        ProjectExplorer::PeCubicle *pNewParentCubicle = pDevice->GetParentCubicle();
        ProjectExplorer::PeCubicle *pOldParentCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pOldParentProjectObject);

        if(pNewParentCubicle == m_pProjectObject)
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pDevice), pDevice));
        else if(pOldParentCubicle == m_pProjectObject)
            SlotProjectObjectAboutToBeDeleted(pProjectObject);
    }
}

void ConfigWidgetCubicle::SlotActionNewIED()
{
    DevExplorerModeWidget::Instance()->CreateIED(m_pProjectObject);
}

void ConfigWidgetCubicle::SlotActionNewSwitch()
{
    DevExplorerModeWidget::Instance()->CreateSwitch(m_pProjectObject);
}

void ConfigWidgetCubicle::SlotActionDelete()
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

void ConfigWidgetCubicle::SlotActionProperty()
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
