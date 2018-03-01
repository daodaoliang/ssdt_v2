#include <QAction>

#include "projectexplorer/peroom.h"
#include "projectexplorer/pecubicle.h"

#include "core/configmodelview.h"

#include "configwidgetroom.h"
#include "devexplorermodewidget.h"

static const char* const g_szColumnSettingName  = "Room";

using namespace DevExplorer::Internal;

ConfigWidgetRoom::ConfigWidgetRoom(QWidget *pParent) :
    ConfigWidgetDeviceObject(pParent),
    m_pActionNewCubicle(0),
    m_pActionDelete(0),
    m_pActionProperty(0)
{
    m_lstColumnInfo << ConfigWidget::COLUMN_INFO(tr("Name"), 300, true)
                    << ConfigWidget::COLUMN_INFO(tr("Type"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Manufacture"), 300, true);

    ReadSettings(g_szColumnSettingName);
}

ConfigWidgetRoom::~ConfigWidgetRoom()
{
    SaveSettings(g_szColumnSettingName);
}

bool ConfigWidgetRoom::BuildModelData()
{
    ProjectExplorer::PeRoom *pRoom = qobject_cast<ProjectExplorer::PeRoom*>(m_pProjectObject);
    if(!pRoom)
        return false;

    foreach(ProjectExplorer::PeCubicle *pCubicle, pRoom->GetChildCubicles())
        m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pCubicle), pCubicle));

    return true;
}

QStringList ConfigWidgetRoom::RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    QStringList lstRowData;

    if(ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectObject))
    {
        lstRowData << pCubicle->GetDisplayName()
                   << pCubicle->GetObjectTypeName()
                   << pCubicle->GetManufacture();
    }

    return lstRowData;
}

QList<QAction*> ConfigWidgetRoom::GetContextMenuActions()
{
    QList<QAction*> lstContextMenuActons;

    if(!m_pActionNewCubicle)
    {
        m_pActionNewCubicle = new QAction(QIcon(":/devexplorer/images/oper_add.png"), tr("New %1...").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otCubicle)), this);
        m_pActionNewCubicle->setEnabled(false);
        connect(m_pActionNewCubicle, SIGNAL(triggered()), this, SLOT(SlotActionNewCubicle()));
    }
    lstContextMenuActons.append(m_pActionNewCubicle);

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

QList<QAction*> ConfigWidgetRoom::GetToolbarActions()
{
    QList<QAction*> lstToolbarActions;

    if(m_pActionNewCubicle)
        lstToolbarActions.append(m_pActionNewCubicle);

    if(m_pActionDelete)
        lstToolbarActions.append(m_pActionDelete);

    if(m_pActionProperty)
        lstToolbarActions.append(m_pActionProperty);

    return lstToolbarActions;
}

void ConfigWidgetRoom::UpdateActions(const QModelIndexList &lstSelectedIndex)
{
    m_pActionNewCubicle->setEnabled(true);
    m_pActionDelete->setEnabled(!lstSelectedIndex.isEmpty());
    m_pActionProperty->setEnabled(lstSelectedIndex.size() == 1);
}

void ConfigWidgetRoom::PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject)
{
    DevExplorerModeWidget::Instance()->UpdateObject(pProjectObject);
}

void ConfigWidgetRoom::SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectObject))
    {
        if(pCubicle->GetParentRoom() == m_pProjectObject)
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pCubicle), pCubicle));
    }
}

void ConfigWidgetRoom::SlotProjectObjectParentChanged(ProjectExplorer::PeProjectObject *pProjectObject, ProjectExplorer::PeProjectObject *pOldParentProjectObject)
{
    if(ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectObject))
    {
        ProjectExplorer::PeRoom *pNewParentRoom = pCubicle->GetParentRoom();
        ProjectExplorer::PeRoom *pOldParentRoom = qobject_cast<ProjectExplorer::PeRoom*>(pOldParentProjectObject);

        if(pNewParentRoom == m_pProjectObject)
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pCubicle), pCubicle));
        else if(pOldParentRoom == m_pProjectObject)
            SlotProjectObjectAboutToBeDeleted(pProjectObject);
    }
}

void ConfigWidgetRoom::SlotActionNewCubicle()
{
    DevExplorerModeWidget::Instance()->CreateCubicle(m_pProjectObject);
}

void ConfigWidgetRoom::SlotActionDelete()
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

void ConfigWidgetRoom::SlotActionProperty()
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
