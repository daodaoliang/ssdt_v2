#include <QAction>
#include <QToolButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>

#include "utils/waitcursor.h"
#include "styledui/styledbar.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/pevterminalconn.h"
#include "projectexplorer/pevterminal.h"
#include "projectexplorer/pestrap.h"

#include "core/configmodelview.h"

#include "configwidgetvterminalconn.h"

static const char* const g_szColumnSettingName  = "VTerminalConn";

using namespace SclModel::Internal;

ConfigWidgetVTerminalConn::ConfigWidgetVTerminalConn(QWidget *pParent) :
    Core::ConfigWidget(pParent),
    m_pCurrentDevice(0),
    m_pSideDevice(0),
    m_pActionDelete(0),
    m_pActionMark(0)
{
    m_lstColumnInfo << ConfigWidget::COLUMN_INFO(tr("Current IED"), 150, true)
                    << ConfigWidget::COLUMN_INFO(tr("Current Signal"), 250, true)
                    << ConfigWidget::COLUMN_INFO(tr("Current Strap"), 150, true)
                    << ConfigWidget::COLUMN_INFO(tr("Direction"), 80, true)
                    << ConfigWidget::COLUMN_INFO(tr("Side IED"), 150, true)
                    << ConfigWidget::COLUMN_INFO(tr("Side Strap"), 150, true)
                    << ConfigWidget::COLUMN_INFO(tr("Side Signal"), 250, true);

    ReadSettings(g_szColumnSettingName);
}

ConfigWidgetVTerminalConn::~ConfigWidgetVTerminalConn()
{
    SaveSettings(g_szColumnSettingName);
}

void ConfigWidgetVTerminalConn::SetupConfigWidget()
{
    Core::ConfigWidget::SetupConfigWidget();

    StyledUi::StyledBar *pStyledBar = new StyledUi::StyledBar(this);
    pStyledBar->setFixedHeight(30);

    m_pCheckBoxShowAll =  new QCheckBox(tr("Show All Connections"), pStyledBar);
    m_pCheckBoxShowAll->setCheckState(Qt::Checked);

    m_pComboBoxConnectionType = new QComboBox(pStyledBar);
    m_pComboBoxConnectionType->setMinimumWidth(100);
    m_pComboBoxConnectionType->setEditable(false);
    m_pComboBoxConnectionType->addItem(tr("All"));
    m_pComboBoxConnectionType->addItem("GOOSE");
    m_pComboBoxConnectionType->addItem("SV");
    m_pComboBoxConnectionType->setCurrentIndex(0);

    QToolButton *pToolButtonMark = new QToolButton(pStyledBar);
    pToolButtonMark->setFocusPolicy(Qt::NoFocus);
    pToolButtonMark->setAutoRaise(true);
    pToolButtonMark->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonMark->setDefaultAction(m_pActionMark);

    QToolButton *pToolButtonDelete = new QToolButton(pStyledBar);
    pToolButtonDelete->setFocusPolicy(Qt::NoFocus);
    pToolButtonDelete->setAutoRaise(true);
    pToolButtonDelete->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonDelete->setDefaultAction(m_pActionDelete);

    QHBoxLayout *pHBoxLayout = new QHBoxLayout(pStyledBar);
    pHBoxLayout->setContentsMargins(5, 0, 0, 0);
    pHBoxLayout->setSpacing(0);

    pHBoxLayout->addWidget(m_pCheckBoxShowAll);
    pHBoxLayout->addSpacing(15);
    pHBoxLayout->addWidget(new QLabel(tr("Connection Filter:"), pStyledBar));
    pHBoxLayout->addWidget(m_pComboBoxConnectionType);
    pHBoxLayout->addStretch(1);
    pHBoxLayout->addWidget(pToolButtonMark);
    pHBoxLayout->addWidget(pToolButtonDelete);

    QVBoxLayout *pVBoxLayout = static_cast<QVBoxLayout*>(layout());
    pVBoxLayout->insertWidget(0, pStyledBar);

    connect(m_pCheckBoxShowAll, SIGNAL(toggled(bool)),
            this, SLOT(SlotShowAllConnectionChanged(bool)));
    connect(m_pComboBoxConnectionType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotConnectionTypeChanged(int)));
}

bool ConfigWidgetVTerminalConn::BuildModelData()
{
    if(!m_pCurrentDevice)
        return true;

    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pCurrentDevice->GetProjectVersion();
    if(!pProjectVersion)
        return true;

    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, pProjectVersion->GetAllVTerminalConns())
    {
        if(CheckVTerminalConn(pVTerminalConn))
        {
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pVTerminalConn), pVTerminalConn));
            UpdateMarkStatus(pVTerminalConn);
        }
    }

    return true;
}

QStringList ConfigWidgetVTerminalConn::RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    QStringList lstRowData;

    if(ProjectExplorer::PeVTerminalConn *pVTerminalConn = qobject_cast<ProjectExplorer::PeVTerminalConn*>(pProjectObject))
    {
        ProjectExplorer::PeVTerminal *pVTerminalTx = pVTerminalConn->GetTxVTerminal();
        ProjectExplorer::PeVTerminal *pVTerminalRx = pVTerminalConn->GetRxVTerminal();
        if(!pVTerminalTx || !pVTerminalRx || !pVTerminalTx->GetParentDevice() || !pVTerminalTx->GetParentDevice())
            return lstRowData;

        ProjectExplorer::PeVTerminal *pVTerminalCurrent = 0;
        ProjectExplorer::PeVTerminal *pVTerminalSide = 0;
        ProjectExplorer::PeStrap *pStrapCurrent = 0;
        ProjectExplorer::PeStrap *pStrapSide = 0;
        QString strDirection;

        if(pVTerminalTx->GetParentDevice() == m_pCurrentDevice)
        {
            pVTerminalCurrent = pVTerminalTx;
            pVTerminalSide = pVTerminalRx;
            pStrapCurrent = pVTerminalConn->GetTxStrap();
            pStrapSide = pVTerminalConn->GetRxStrap();
            strDirection = ">>>";
        }
        else if(pVTerminalRx->GetParentDevice() == m_pCurrentDevice)
        {
            pVTerminalCurrent = pVTerminalRx;
            pVTerminalSide = pVTerminalTx;
            pStrapCurrent = pVTerminalConn->GetRxStrap();
            pStrapSide = pVTerminalConn->GetTxStrap();
            strDirection = "<<<";
        }

        if(!pVTerminalCurrent || !pVTerminalSide)
            return lstRowData;


        lstRowData << pVTerminalCurrent->GetParentDevice()->GetDisplayName()
                   << pVTerminalCurrent->GetDisplayName()
                   << (pStrapCurrent ? pStrapCurrent->GetDisplayName() : "")
                   << strDirection
                   << pVTerminalSide->GetParentDevice()->GetDisplayName()
                   << (pStrapSide ? pStrapSide->GetDisplayName() : "")
                   << pVTerminalSide->GetDisplayName();
    }

    return lstRowData;
}

QList<QAction*> ConfigWidgetVTerminalConn::GetContextMenuActions()
{
    QList<QAction*> lstContextMenuActons;

    if(!m_pActionDelete)
    {
        m_pActionDelete = new QAction(QIcon(":/sclmodel/images/oper_remove.png"), tr("Delete"), this);
        m_pActionDelete->setEnabled(false);
        connect(m_pActionDelete, SIGNAL(triggered()), this, SLOT(SlotActionDelete()));
    }
    lstContextMenuActons.append(m_pActionDelete);

    if(!m_pActionMark)
    {
        m_pActionMark = new QAction(QIcon(":/sclmodel/images/oper_mark.png"), tr("Mark P2P"), this);
        m_pActionMark->setEnabled(false);
        connect(m_pActionMark, SIGNAL(triggered()), this, SLOT(SlotActionMark()));
    }
    lstContextMenuActons.append(m_pActionMark);

    QAction *pSeparator = new QAction(this);
    pSeparator->setSeparator(true);
    lstContextMenuActons.append(pSeparator);

    return lstContextMenuActons;
}

void ConfigWidgetVTerminalConn::UpdateActions(const QModelIndexList &lstSelectedIndex)
{
    m_pActionDelete->setEnabled(!lstSelectedIndex.isEmpty());
    m_pActionMark->setEnabled(!lstSelectedIndex.isEmpty());
}

void ConfigWidgetVTerminalConn::PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(ProjectExplorer::PeVTerminalConn *pVTerminalConn = qobject_cast<ProjectExplorer::PeVTerminalConn*>(pProjectObject))
        emit sigCurrentVTerminalConnChanged(pVTerminalConn);
}

bool ConfigWidgetVTerminalConn::CheckVTerminalConn(ProjectExplorer::PeVTerminalConn *pVTerminalConn) const
{
    ProjectExplorer::PeVTerminal *pVTerminalTx = pVTerminalConn->GetTxVTerminal();
    ProjectExplorer::PeVTerminal *pVTerminalRx = pVTerminalConn->GetRxVTerminal();
    if(!pVTerminalTx || !pVTerminalRx)
        return false;

    const int iConnectionType = m_pComboBoxConnectionType->currentIndex();
    if(iConnectionType == 1) // GOOSE
    {
        if(pVTerminalTx->GetType() != ProjectExplorer::PeVTerminal::ttGoose ||
           pVTerminalTx->GetType() != ProjectExplorer::PeVTerminal::ttGoose)
        {
            return false;
        }
    }
    else if(iConnectionType == 2) // SV
    {
        if(pVTerminalTx->GetType() != ProjectExplorer::PeVTerminal::ttSv ||
           pVTerminalTx->GetType() != ProjectExplorer::PeVTerminal::ttSv)
        {
            return false;
        }
    }

    ProjectExplorer::PeDevice *pDeviceTx = pVTerminalTx->GetParentDevice();
    ProjectExplorer::PeDevice *pDeviceRx = pVTerminalRx->GetParentDevice();
    if(!pDeviceTx || !pDeviceRx)
        return false;

    return m_pCheckBoxShowAll->isChecked() ? (pDeviceTx == m_pCurrentDevice || pDeviceRx == m_pCurrentDevice) :
                                             ((pDeviceTx == m_pCurrentDevice && pDeviceRx == m_pSideDevice) || (pDeviceRx == m_pCurrentDevice && pDeviceTx == m_pSideDevice));
}

void ConfigWidgetVTerminalConn::UpdateMarkStatus(ProjectExplorer::PeVTerminalConn *pVTerminalConn)
{
    if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromProjectObject(pVTerminalConn))
    {
        QModelIndex index = m_pModel->IndexFromConfigRow(pConfigRow);
        if(!index.isValid())
            return;

        QFont font;
        if(pVTerminalConn->GetStraight())
            font.setBold(true);

        m_pModel->setData(index, font, Qt::FontRole);
    }
}

void ConfigWidgetVTerminalConn::SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!m_pModel)
        return;

    if(ProjectExplorer::PeVTerminalConn *pVTerminalConn = qobject_cast<ProjectExplorer::PeVTerminalConn*>(pProjectObject))
    {
        if(CheckVTerminalConn(pVTerminalConn))
        {
            Core::ConfigRow *pConfigRow = new Core::ConfigRow(RowDataFromProjectObject(pProjectObject), pProjectObject);
            m_pModel->AppendRow(pConfigRow);
            m_pView->SelectConfigRow(pConfigRow);

            UpdateMarkStatus(pVTerminalConn);
        }
    }
}

void ConfigWidgetVTerminalConn::SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!m_pModel)
        return;

    if(ProjectExplorer::PeVTerminalConn *pVTerminalConn = qobject_cast<ProjectExplorer::PeVTerminalConn*>(pProjectObject))
    {
        UpdateMarkStatus(pVTerminalConn);
    }
    else if(ProjectExplorer::PeVTerminal *pVTerminal = qobject_cast<ProjectExplorer::PeVTerminal*>(pProjectObject))
    {
        for(int i = 0; i < m_pModel->rowCount(); i++)
        {
            if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromRow(i))
            {
                if(ProjectExplorer::PeVTerminalConn *pVTerminalConn = qobject_cast<ProjectExplorer::PeVTerminalConn*>(pConfigRow->GetProjectObject()))
                {
                    if(pVTerminalConn->GetTxVTerminal() == pVTerminal || pVTerminalConn->GetRxVTerminal() == pVTerminal)
                    {
                        m_pModel->UpdateRow(RowDataFromProjectObject(pVTerminalConn), pConfigRow);
                        m_pView->SelectConfigRow(pConfigRow);
                    }
                }
            }
        }
    }
}

void ConfigWidgetVTerminalConn::SlotSetCurrentDevice(ProjectExplorer::PeDevice *pCurrentDevice)
{
    if(m_pCurrentDevice == pCurrentDevice)
        return;

    m_pCurrentDevice = pCurrentDevice;
    BuildModel(m_pCurrentDevice);
}

void ConfigWidgetVTerminalConn::SlotSetSideDevice(ProjectExplorer::PeDevice *pSideDevice)
{
    if(m_pSideDevice == pSideDevice)
        return;

    m_pSideDevice = pSideDevice;

    if(!m_pCheckBoxShowAll->isChecked())
        UpdateModel();
}

void ConfigWidgetVTerminalConn::SlotActivateVTerminalConn(ProjectExplorer::PeVTerminalConn *pVTerminalConn)
{
    if(!pVTerminalConn)
        return;

    for(int i = 0; i < m_pModel->rowCount(); i++)
    {
        if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromRow(i))
        {
            if(ProjectExplorer::PeVTerminalConn *pVC = qobject_cast<ProjectExplorer::PeVTerminalConn*>(pConfigRow->GetProjectObject()))
            {
                if(pVC == pVTerminalConn)
                {
                    m_pView->SelectConfigRow(pConfigRow);
                    m_pView->scrollTo(m_pModel->IndexFromConfigRow(pConfigRow), QAbstractItemView::PositionAtCenter);

                    return;
                }
            }
        }
    }
}

void ConfigWidgetVTerminalConn::SlotActionDelete()
{
    QList<ProjectExplorer::PeProjectObject*> lstDeletedObjects;
    foreach(const QModelIndex &index, m_pView->selectionModel()->selectedRows())
    {
        if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromIndex(index))
        {
            if(ProjectExplorer::PeProjectObject *pProjectObject = pConfigRow->GetProjectObject())
                lstDeletedObjects.append(pProjectObject);
        }
    }

    if(QMessageBox::question(this,
                             tr("Delete Confirmation"),
                             tr("Are you sure you want to delete these virtual connections?"),
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No) == QMessageBox::Yes)
    {
        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        foreach(ProjectExplorer::PeProjectObject *pProjectObject, lstDeletedObjects)
        {
            ProjectExplorer::PeProjectVersion *pProjectVersion = pProjectObject->GetProjectVersion();
            if(!pProjectVersion)
                return;

            if(pProjectVersion->DbDeleteObject(pProjectObject->GetObjectType(), pProjectObject->GetId(), true))
                pProjectVersion->DeleteObject(pProjectObject);
        }
    }
}

void ConfigWidgetVTerminalConn::SlotActionMark()
{
    QList<ProjectExplorer::PeVTerminalConn*> lstMarkedVTerminalConns;
    foreach(const QModelIndex &index, m_pView->selectionModel()->selectedRows())
    {
        if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromIndex(index))
        {
            if(ProjectExplorer::PeVTerminalConn *pVTerminalConn = qobject_cast<ProjectExplorer::PeVTerminalConn*>(pConfigRow->GetProjectObject()))
                lstMarkedVTerminalConns.append(pVTerminalConn);
        }
    }

    Utils::WaitCursor waitcursor;
    Q_UNUSED(waitcursor)

    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, lstMarkedVTerminalConns)
    {
        ProjectExplorer::PeProjectVersion *pProjectVersion = pVTerminalConn->GetProjectVersion();
        if(!pProjectVersion)
            return;

        ProjectExplorer::PeVTerminalConn VTerminalConn(*pVTerminalConn);
        VTerminalConn.SetStraight(!VTerminalConn.GetStraight());

        if(pProjectVersion->DbUpdateObject(VTerminalConn, true))
            pProjectVersion->UpdateObject(VTerminalConn);
    }
}

void ConfigWidgetVTerminalConn::SlotShowAllConnectionChanged(bool bShow)
{
    Q_UNUSED(bShow)

    UpdateModel();
}

void ConfigWidgetVTerminalConn::SlotConnectionTypeChanged(int iIndex)
{
    Q_UNUSED(iIndex)

    UpdateModel();
}
