#include <QAction>
#include <QMenu>

#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pecubicleconn.h"

#include "core/configwidget.h"
#include "core/configmodelview.h"

#include "configwidgetcubicleconn.h"

static const char* const g_szColumnSettingName  = "CubicleConn";

using namespace PhyNetwork::Internal;

ConfigWidgetCubicleConn::ConfigWidgetCubicleConn(QWidget *pParent) :
    Core::ConfigWidget(pParent),
    m_pCurrentCubicle(0)
{
    m_lstColumnInfo << ConfigWidget::COLUMN_INFO(tr("Current Cubicle"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Use Current Odf"), 100, true)
                    << ConfigWidget::COLUMN_INFO(tr("Current Pass Cubicle"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Side Pass Cubicle"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Use Side Odf"), 100, true)
                    << ConfigWidget::COLUMN_INFO(tr("Side Cubicle"), 200, true);

    ReadSettings(g_szColumnSettingName);
}

ConfigWidgetCubicleConn::~ConfigWidgetCubicleConn()
{
    SaveSettings(g_szColumnSettingName);
}

bool ConfigWidgetCubicleConn::BuildModelData()
{
    if(!m_pCurrentCubicle)
        return true;

    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pCurrentCubicle->GetProjectVersion();
    if(!pProjectVersion)
        return true;

    foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, pProjectVersion->GetAllCubicleConns())
    {
        if(pCubicleConn->GetCubicle1() == m_pCurrentCubicle || pCubicleConn->GetCubicle2() == m_pCurrentCubicle)
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pCubicleConn), pCubicleConn));
    }

    return true;
}

QStringList ConfigWidgetCubicleConn::RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    QStringList lstRowData;

    if(ProjectExplorer::PeCubicleConn *pCubicleConn = qobject_cast<ProjectExplorer::PeCubicleConn*>(pProjectObject))
    {
        if(pCubicleConn->GetCubicle1() == m_pCurrentCubicle)
        {
            lstRowData << (pCubicleConn->GetCubicle1() ? pCubicleConn->GetCubicle1()->GetDisplayName() : "")
                       << (pCubicleConn->GetUseOdf1() ? tr("True") : tr("False"))
                       << (pCubicleConn->GetPassCubicle1() ? pCubicleConn->GetPassCubicle1()->GetDisplayName() : "")
                       << (pCubicleConn->GetPassCubicle2() ? pCubicleConn->GetPassCubicle2()->GetDisplayName() : "")
                       << (pCubicleConn->GetUseOdf2() ? tr("True") : tr("False"))
                       << (pCubicleConn->GetCubicle2() ? pCubicleConn->GetCubicle2()->GetDisplayName() : "");
        }
        else if(pCubicleConn->GetCubicle2() == m_pCurrentCubicle)
        {
            lstRowData << (pCubicleConn->GetCubicle2() ? pCubicleConn->GetCubicle2()->GetDisplayName() : "")
                       << (pCubicleConn->GetUseOdf2() ? tr("True") : tr("False"))
                       << (pCubicleConn->GetPassCubicle2() ? pCubicleConn->GetPassCubicle2()->GetDisplayName() : "")
                       << (pCubicleConn->GetPassCubicle1() ? pCubicleConn->GetPassCubicle1()->GetDisplayName() : "")
                       << (pCubicleConn->GetUseOdf1() ? tr("True") : tr("False"))
                       << (pCubicleConn->GetCubicle1() ? pCubicleConn->GetCubicle1()->GetDisplayName() : "");
        }
    }

    return lstRowData;
}

void ConfigWidgetCubicleConn::UpdateActions(const QModelIndexList &lstSelectedIndex)
{
    Q_UNUSED(lstSelectedIndex)

    if(m_pModel)
    {
        ProjectExplorer::PeCubicleConn *pCubicleConn = 0;
        if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromIndex(m_pView->selectionModel()->currentIndex()))
            pCubicleConn = qobject_cast<ProjectExplorer::PeCubicleConn*>(pConfigRow->GetProjectObject());

        emit sigCurrentCubicleConnChanged(pCubicleConn);
    }
}

void ConfigWidgetCubicleConn::SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!m_pModel)
        return;

    if(ProjectExplorer::PeCubicleConn *pCubicleConn = qobject_cast<ProjectExplorer::PeCubicleConn*>(pProjectObject))
    {
        if(pCubicleConn->GetCubicle1() == m_pCurrentCubicle || pCubicleConn->GetCubicle2() == m_pCurrentCubicle)
        {
            Core::ConfigRow *pConfigRow = new Core::ConfigRow(RowDataFromProjectObject(pCubicleConn), pCubicleConn);
            m_pModel->AppendRow(pConfigRow);
            m_pView->SelectConfigRow(pConfigRow);
        }
    }
}

void ConfigWidgetCubicleConn::SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!m_pModel)
        return;

    if(ProjectExplorer::PeCubicleConn *pCubicleConn = qobject_cast<ProjectExplorer::PeCubicleConn*>(pProjectObject))
    {
        if(pCubicleConn->GetCubicle1() == m_pCurrentCubicle || pCubicleConn->GetCubicle2() == m_pCurrentCubicle)
        {
            if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromProjectObject(pProjectObject))
            {
                m_pModel->UpdateRow(RowDataFromProjectObject(pProjectObject), pConfigRow);
                m_pView->SelectConfigRow(pConfigRow);
            }
        }
    }
}

void ConfigWidgetCubicleConn::SlotSetCurrentCubicle(ProjectExplorer::PeCubicle *pCurrentCubicle)
{
    if(m_pCurrentCubicle == pCurrentCubicle)
        return;

    m_pCurrentCubicle = pCurrentCubicle;
    BuildModel(m_pCurrentCubicle);
}
