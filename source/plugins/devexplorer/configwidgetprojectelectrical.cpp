#include <QAction>
#include <QMenu>

#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/pedevice.h"

#include "core/configmodelview.h"

#include "configwidgetprojectelectrical.h"
#include "devexplorermodewidget.h"

static const char* const g_szColumnSettingName  = "ProjectElectrical";

using namespace DevExplorer::Internal;

ConfigWidgetProjectElectrical::ConfigWidgetProjectElectrical(QWidget *pParent) :
    ConfigWidgetDeviceObject(pParent),
    m_pActionNewBay(0),
    m_pActionNewIED(0),
    m_pActionNewSwitch(0),
    m_pActionDelete(0),
    m_pActionProperty(0)
{
    m_lstColumnInfo << ConfigWidget::COLUMN_INFO(tr("Name"), 300, true)
                    << ConfigWidget::COLUMN_INFO(tr("Number"), 300, true)
                    << ConfigWidget::COLUMN_INFO(tr("Type"), 300, true);

    ReadSettings(g_szColumnSettingName);
}

ConfigWidgetProjectElectrical::~ConfigWidgetProjectElectrical()
{
    SaveSettings(g_szColumnSettingName);
}

bool ConfigWidgetProjectElectrical::BuildModelData()
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = qobject_cast<ProjectExplorer::PeProjectVersion*>(m_pProjectObject);
    if(!pProjectVersion)
        return false;

    foreach(ProjectExplorer::PeBay *pBay, pProjectVersion->GetAllBays())
    {
        m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pBay), pBay));
    }

    foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
    {
        if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF && !pDevice->GetParentBay())
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pDevice), pDevice));
    }

    return true;
}

QStringList ConfigWidgetProjectElectrical::RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    QString strBayNumber = "";
    if(ProjectExplorer::PeBay *pBay = qobject_cast<ProjectExplorer::PeBay*>(pProjectObject))
        strBayNumber = pBay->GetNumber();

    return QStringList() << pProjectObject->GetDisplayName()
                         << strBayNumber
                         << pProjectObject->GetObjectTypeName();
}

QList<QAction*> ConfigWidgetProjectElectrical::GetContextMenuActions()
{
    QList<QAction*> lstContextMenuActons;
    QMenu *pMenuAddNew = new QMenu(tr("&Add New"), this);

    if(!m_pActionNewBay)
    {
        m_pActionNewBay = new QAction(tr("New %1...").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otBay)), this);
        m_pActionNewBay->setEnabled(true);
        connect(m_pActionNewBay, SIGNAL(triggered()), this, SLOT(SlotActionNewBay()));
    }
    pMenuAddNew->addAction(m_pActionNewBay);

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

QList<QAction*> ConfigWidgetProjectElectrical::GetToolbarActions()
{
    QList<QAction*> lstToolbarActions;

    QMenu *pMenu = new QMenu(tr("Add New"), this);
    if(m_pActionNewBay)
        pMenu->addAction(m_pActionNewBay);

    if(m_pActionNewIED)
        pMenu->addAction(m_pActionNewIED);

    if(m_pActionNewSwitch)
        pMenu->addAction(m_pActionNewSwitch);

    lstToolbarActions.append(pMenu->menuAction());
    pMenu->menuAction()->setIcon(QIcon(":/devexplorer/images/oper_add.png"));
    connect(pMenu->menuAction(), SIGNAL(triggered()), this, SLOT(SlotActionNewBay()));

    if(m_pActionDelete)
        lstToolbarActions.append(m_pActionDelete);

    if(m_pActionProperty)
        lstToolbarActions.append(m_pActionProperty);

    return lstToolbarActions;
}

void ConfigWidgetProjectElectrical::UpdateActions(const QModelIndexList &lstSelectedIndex)
{
    m_pActionNewBay->setEnabled(true);
    m_pActionNewIED->setEnabled(true);
    m_pActionNewSwitch->setEnabled(true);

    m_pActionDelete->setEnabled(!lstSelectedIndex.isEmpty());
    m_pActionProperty->setEnabled(lstSelectedIndex.size() == 1);
}

void ConfigWidgetProjectElectrical::PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject)
{
    DevExplorerModeWidget::Instance()->UpdateObject(pProjectObject);
}

void ConfigWidgetProjectElectrical::SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(pProjectObject->GetProjectVersion() != m_pProjectObject)
        return;

    const ProjectExplorer::PeProjectObject::ObjectType eObjectType = pProjectObject->GetObjectType();
    if(eObjectType == ProjectExplorer::PeProjectObject::otBay)
    {
        if(qobject_cast<ProjectExplorer::PeBay*>(pProjectObject))
        {
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pProjectObject), pProjectObject));;
        }
    }
    else if(eObjectType == ProjectExplorer::PeProjectObject::otDevice)
    {
        if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
        {
            if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF && !pDevice->GetParentBay())
                m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pProjectObject), pProjectObject));;
        }
    }
}

void ConfigWidgetProjectElectrical::SlotProjectDeviceBayChanged(ProjectExplorer::PeDevice *pDevice, ProjectExplorer::PeBay *pOldBay)
{
    if(!pDevice || pDevice->GetProjectVersion() != m_pProjectObject)
        return;

    ProjectExplorer::PeBay *pNewBay = pDevice->GetParentBay();
    if(pNewBay == 0)
    {
        m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pDevice), pDevice));
    }
    else if(pOldBay == 0)
    {
        if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromProjectObject(pDevice))
            m_pModel->RemoveRow(pConfigRow);
    }
}

void ConfigWidgetProjectElectrical::SlotActionNewBay()
{
    DevExplorerModeWidget::Instance()->CreateBay(m_pProjectObject);
}

void ConfigWidgetProjectElectrical::SlotActionNewIED()
{
    DevExplorerModeWidget::Instance()->CreateIED(m_pProjectObject);
}

void ConfigWidgetProjectElectrical::SlotActionNewSwitch()
{
    DevExplorerModeWidget::Instance()->CreateSwitch(m_pProjectObject);
}

void ConfigWidgetProjectElectrical::SlotActionDelete()
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

void ConfigWidgetProjectElectrical::SlotActionProperty()
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
