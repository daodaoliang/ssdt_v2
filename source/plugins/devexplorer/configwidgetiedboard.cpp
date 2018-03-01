#include <QAction>

#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"

#include "core/configmodelview.h"

#include "configwidgetiedboard.h"
#include "devexplorermodewidget.h"

static const char* const g_szColumnSettingName  = "IED_Board";

using namespace DevExplorer::Internal;

ConfigWidgetIEDBoard::ConfigWidgetIEDBoard(QWidget *pParent) :
    ConfigWidgetDeviceObject(pParent),
    m_pActionNewBoard(0),
    m_pActionDelete(0),
    m_pActionProperty(0)
{
    m_lstColumnInfo << ConfigWidget::COLUMN_INFO(tr("Position"), 300, true)
                    << ConfigWidget::COLUMN_INFO(tr("Type"), 250, true)
                    << ConfigWidget::COLUMN_INFO(tr("Description"), 750, true);

    ReadSettings(g_szColumnSettingName);
}

ConfigWidgetIEDBoard::~ConfigWidgetIEDBoard()
{
    SaveSettings(g_szColumnSettingName);
}

bool ConfigWidgetIEDBoard::BuildModelData()
{
    ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(m_pProjectObject);
    if(!pDevice || pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtIED)
        return false;

    foreach(ProjectExplorer::PeBoard *pBoard, pDevice->GetChildBoards())
        m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pBoard), pBoard));

    return true;
}

QStringList ConfigWidgetIEDBoard::RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    QStringList lstRowData;

    if(ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pProjectObject))
    {
        lstRowData << pBoard->GetPosition()
                   << pBoard->GetType()
                   << pBoard->GetDescription();
    }

    return lstRowData;
}

QList<QAction*> ConfigWidgetIEDBoard::GetContextMenuActions()
{
    QList<QAction*> lstContextMenuActons;

    if(!m_pActionNewBoard)
    {
        m_pActionNewBoard = new QAction(QIcon(":/devexplorer/images/oper_add.png"), tr("New %1...").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otBoard)), this);
        m_pActionNewBoard->setEnabled(false);
        connect(m_pActionNewBoard, SIGNAL(triggered()), this, SLOT(SlotActionNewBoard()));
    }
    lstContextMenuActons.append(m_pActionNewBoard);

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

QList<QAction*> ConfigWidgetIEDBoard::GetToolbarActions()
{
    QList<QAction*> lstToolbarActions;

    if(m_pActionNewBoard)
        lstToolbarActions.append(m_pActionNewBoard);

    if(m_pActionDelete)
        lstToolbarActions.append(m_pActionDelete);

    if(m_pActionProperty)
        lstToolbarActions.append(m_pActionProperty);

    return lstToolbarActions;
}

void ConfigWidgetIEDBoard::UpdateActions(const QModelIndexList &lstSelectedIndex)
{
    m_pActionNewBoard->setEnabled(true);
    m_pActionDelete->setEnabled(!lstSelectedIndex.isEmpty());
    m_pActionProperty->setEnabled(lstSelectedIndex.size() == 1);
}

void ConfigWidgetIEDBoard::PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject)
{
    DevExplorerModeWidget::Instance()->UpdateObject(pProjectObject);
}

void ConfigWidgetIEDBoard::SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pProjectObject))
    {
        if(pBoard->GetParentDevice() == m_pProjectObject)
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pBoard), pBoard));
    }
}

void ConfigWidgetIEDBoard::SlotActionNewBoard()
{
    DevExplorerModeWidget::Instance()->CreateBoard(qobject_cast<ProjectExplorer::PeDevice*>(m_pProjectObject));
}

void ConfigWidgetIEDBoard::SlotActionDelete()
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

void ConfigWidgetIEDBoard::SlotActionProperty()
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
