#include <QAction>
#include <QMenu>
#include <QToolButton>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QHBoxLayout>

#include "utils/waitcursor.h"
#include "utils/searchcombobox.h"
#include "styledui/styledbar.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/peinfoset.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/peport.h"

#include "core/configwidget.h"
#include "core/configmodelview.h"

#include "configwidgetinfoset.h"

static const char* const g_szColumnSettingName  = "InfoSet";

using namespace PhyNetwork::Internal;

ConfigWidgetInfoSet::ConfigWidgetInfoSet(QWidget *pParent) :
    Core::ConfigWidget(pParent),
    m_pActionDelete(0)
{
    m_lstColumnInfo << ConfigWidget::COLUMN_INFO(tr("Name"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Description"), 200, true)
                    << ConfigWidget::COLUMN_INFO(tr("Type"), 100, true)
                    << ConfigWidget::COLUMN_INFO(tr("Send IED"), 120, true)
                    << ConfigWidget::COLUMN_INFO(tr("First Level Switch"), 120, true)
                    << ConfigWidget::COLUMN_INFO(tr("Second Level Switch"), 120, true)
                    << ConfigWidget::COLUMN_INFO(tr("Third Level Switch"), 120, true)
                    << ConfigWidget::COLUMN_INFO(tr("Fourth Level Switch"), 120, true)
                    << ConfigWidget::COLUMN_INFO(tr("Receive IED"), 120, true);

    ReadSettings(g_szColumnSettingName);
}

ConfigWidgetInfoSet::~ConfigWidgetInfoSet()
{
    SaveSettings(g_szColumnSettingName);
}

void ConfigWidgetInfoSet::SetupConfigWidget()
{
    Core::ConfigWidget::SetupConfigWidget();

    StyledUi::StyledBar *pStyledBar = new StyledUi::StyledBar(this);
    pStyledBar->setFixedHeight(30);

    m_pComboBoxTypeFilter = new QComboBox(pStyledBar);
    m_pComboBoxTypeFilter->setMinimumWidth(100);
    m_pComboBoxTypeFilter->setEditable(false);
    m_pComboBoxTypeFilter->addItem(tr("All"), ProjectExplorer::PeInfoSet::itNone);
    m_pComboBoxTypeFilter->addItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(ProjectExplorer::PeInfoSet::itGoose), ProjectExplorer::PeInfoSet::itGoose);
    m_pComboBoxTypeFilter->addItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(ProjectExplorer::PeInfoSet::itSv), ProjectExplorer::PeInfoSet::itSv);
    m_pComboBoxTypeFilter->addItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(ProjectExplorer::PeInfoSet::itTime), ProjectExplorer::PeInfoSet::itTime);
    m_pComboBoxTypeFilter->addItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(ProjectExplorer::PeInfoSet::itMix), ProjectExplorer::PeInfoSet::itMix);
    m_pComboBoxTypeFilter->setCurrentIndex(0);

    m_pComboBoxSendIEDFilter = new Utils::SearchComboBox(pStyledBar);
    m_pComboBoxSendIEDFilter->setMinimumWidth(100);
    m_pComboBoxSendIEDFilter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_pComboBoxReceiveIEDFilter = new Utils::SearchComboBox(pStyledBar);
    m_pComboBoxReceiveIEDFilter->setMinimumWidth(100);
    m_pComboBoxReceiveIEDFilter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_pComboBoxSwitchFilter = new Utils::SearchComboBox(pStyledBar);
    m_pComboBoxSwitchFilter->setMinimumWidth(100);
    m_pComboBoxSwitchFilter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QToolButton *pToolButtonDelete = new QToolButton(pStyledBar);
    pToolButtonDelete->setFocusPolicy(Qt::NoFocus);
    pToolButtonDelete->setAutoRaise(true);
    pToolButtonDelete->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonDelete->setDefaultAction(m_pActionDelete);

    QHBoxLayout *pHBoxLayout = new QHBoxLayout(pStyledBar);
    pHBoxLayout->setContentsMargins(5, 0, 0, 0);
    pHBoxLayout->setSpacing(5);

    pHBoxLayout->addWidget(new QLabel(tr("Type Filter:"), pStyledBar), 0, Qt::AlignLeft);
    pHBoxLayout->addWidget(m_pComboBoxTypeFilter, 0, Qt::AlignLeft);
    pHBoxLayout->addSpacing(15);
    pHBoxLayout->addWidget(new QLabel(tr("Send IED Filter:"), pStyledBar));
    pHBoxLayout->addWidget(m_pComboBoxSendIEDFilter);
    pHBoxLayout->addSpacing(15);
    pHBoxLayout->addWidget(new QLabel(tr("Receive IED Filter:"), pStyledBar));
    pHBoxLayout->addWidget(m_pComboBoxReceiveIEDFilter);
    pHBoxLayout->addSpacing(15);
    pHBoxLayout->addWidget(new QLabel(tr("Switch Filter:"), pStyledBar));
    pHBoxLayout->addWidget(m_pComboBoxSwitchFilter);
    pHBoxLayout->addSpacing(30);
    pHBoxLayout->addWidget(pToolButtonDelete, 0, Qt::AlignRight);

    QVBoxLayout *pVBoxLayout = static_cast<QVBoxLayout*>(layout());
    pVBoxLayout->insertWidget(0, pStyledBar);

    connect(m_pComboBoxTypeFilter, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotFilterChanged(int)));
    connect(m_pComboBoxSendIEDFilter, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotFilterChanged(int)));
    connect(m_pComboBoxReceiveIEDFilter, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotFilterChanged(int)));
    connect(m_pComboBoxSwitchFilter, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotFilterChanged(int)));
}

bool ConfigWidgetInfoSet::BuildModel(ProjectExplorer::PeProjectObject *pProjectObject)
{
    bool bRet = Core::ConfigWidget::BuildModel(pProjectObject);

    UpdateFilberIEDs();

    return bRet;
}

bool ConfigWidgetInfoSet::BuildModelData()
{
    ProjectExplorer::PeBay *pBay = qobject_cast<ProjectExplorer::PeBay*>(m_pProjectObject);
    if(!pBay)
        return false;

    foreach(ProjectExplorer::PeInfoSet *pInfoSet, pBay->GetProjectVersion()->GetAllInfoSets())
    {
        if(CheckVInfoSet(pInfoSet))
        {
            m_pModel->AppendRow(new Core::ConfigRow(RowDataFromProjectObject(pInfoSet), pInfoSet));
            UpdateCompleteStatus(pInfoSet);
        }
    }

    return true;
}

QStringList ConfigWidgetInfoSet::RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    QStringList lstRowData;

    if(ProjectExplorer::PeInfoSet *pInfoSet = qobject_cast<ProjectExplorer::PeInfoSet*>(pProjectObject))
    {
        lstRowData << pInfoSet->GetName()
                   << pInfoSet->GetDescription()
                   << ProjectExplorer::PeInfoSet::GetInfoSetTypeName(pInfoSet->GetInfoSetType());

        QString strTxIED = "";
        if(ProjectExplorer::PeDevice *pTxIED = pInfoSet->GetTxIED())
        {
            strTxIED = pTxIED->GetDisplayName();
            if(ProjectExplorer::PePort *pTxPort = pInfoSet->GetTxPort())
                strTxIED = QString("%1<%2:%3>").arg(strTxIED).arg(pTxPort->GetParentBoard()->GetDisplayName()).arg(pTxPort->GetDisplayName());
        }
        lstRowData << strTxIED;

        QString strSwitch1 = "";
        if(ProjectExplorer::PeDevice *pSwitch1 = pInfoSet->GetSwitch1())
        {
            strSwitch1 = pSwitch1->GetDisplayName();
            if(ProjectExplorer::PePort *pSwitch1TxPort = pInfoSet->GetSwitch1TxPort())
                strSwitch1 = QString("%1<%2>").arg(strSwitch1).arg(pSwitch1TxPort->GetDisplayName());

            if(ProjectExplorer::PePort *pSwitch1RxPort = pInfoSet->GetSwitch1RxPort())
                strSwitch1 = QString("<%1>%2").arg(pSwitch1RxPort->GetDisplayName()).arg(strSwitch1);
        }
        lstRowData << strSwitch1;

        QString strSwitch2 = "";
        if(ProjectExplorer::PeDevice *pSwitch2 = pInfoSet->GetSwitch2())
        {
            strSwitch2 = pSwitch2->GetDisplayName();
            if(ProjectExplorer::PePort *pSwitch2TxPort = pInfoSet->GetSwitch2TxPort())
                strSwitch2 = QString("%1<%2>").arg(strSwitch2).arg(pSwitch2TxPort->GetDisplayName());

            if(ProjectExplorer::PePort *pSwitch2RxPort = pInfoSet->GetSwitch2RxPort())
                strSwitch2 = QString("<%1>%2").arg(pSwitch2RxPort->GetDisplayName()).arg(strSwitch2);
        }
        lstRowData << strSwitch2;

        QString strSwitch3 = "";
        if(ProjectExplorer::PeDevice *pSwitch3 = pInfoSet->GetSwitch3())
        {
            strSwitch3 = pSwitch3->GetDisplayName();
            if(ProjectExplorer::PePort *pSwitch3TxPort = pInfoSet->GetSwitch3TxPort())
                strSwitch3 = QString("%1<%2>").arg(strSwitch3).arg(pSwitch3TxPort->GetDisplayName());

            if(ProjectExplorer::PePort *pSwitch3RxPort = pInfoSet->GetSwitch3RxPort())
                strSwitch3 = QString("<%1>%2").arg(pSwitch3RxPort->GetDisplayName()).arg(strSwitch3);
        }
        lstRowData << strSwitch3;

        QString strSwitch4 = "";
        if(ProjectExplorer::PeDevice *pSwitch4 = pInfoSet->GetSwitch4())
        {
            strSwitch4 = pSwitch4->GetDisplayName();
            if(ProjectExplorer::PePort *pSwitch4TxPort = pInfoSet->GetSwitch4TxPort())
                strSwitch4 = QString("%1<%2>").arg(strSwitch4).arg(pSwitch4TxPort->GetDisplayName());

            if(ProjectExplorer::PePort *pSwitch4RxPort = pInfoSet->GetSwitch4RxPort())
                strSwitch4 = QString("<%1>%2").arg(pSwitch4RxPort->GetDisplayName()).arg(strSwitch4);
        }
        lstRowData << strSwitch4;

        QString strRxIED = "";
        if(ProjectExplorer::PeDevice *pRxIED = pInfoSet->GetRxIED())
        {
            strRxIED = pRxIED->GetDisplayName();
            if(ProjectExplorer::PePort *pRxPort = pInfoSet->GetRxPort())
                strRxIED = QString("<%1:%2>%3").arg(pRxPort->GetParentBoard()->GetDisplayName()).arg(pRxPort->GetDisplayName()).arg(strRxIED);
        }
        lstRowData << strRxIED;
    }

    return lstRowData;
}

QList<QAction*> ConfigWidgetInfoSet::GetContextMenuActions()
{
    QList<QAction*> lstContextMenuActons;

    if(!m_pActionDelete)
    {
        m_pActionDelete = new QAction(QIcon(":/phynetwork/images/oper_remove.png"), tr("Delete"), this);
        m_pActionDelete->setEnabled(false);
        connect(m_pActionDelete, SIGNAL(triggered()), this, SLOT(SlotActionDelete()));
    }
    lstContextMenuActons.append(m_pActionDelete);

    return lstContextMenuActons;
}

QList<QAction*> ConfigWidgetInfoSet::GetToolbarActions()
{
    QList<QAction*> lstToolbarActions;

    if(m_pActionDelete)
        lstToolbarActions.append(m_pActionDelete);

    return lstToolbarActions;
}

void ConfigWidgetInfoSet::UpdateActions(const QModelIndexList &lstSelectedIndex)
{
    m_pActionDelete->setEnabled(!lstSelectedIndex.isEmpty());
}

void ConfigWidgetInfoSet::PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(ProjectExplorer::PeInfoSet *pInfoSet = qobject_cast<ProjectExplorer::PeInfoSet*>(pProjectObject))
        emit sigCurrentInfoSetChanged(pInfoSet);
}

bool ConfigWidgetInfoSet::CheckVInfoSet(ProjectExplorer::PeInfoSet *pInfoSet) const
{
    if(!pInfoSet)
        return false;

    if(pInfoSet->GetInfoSetType() == ProjectExplorer::PeInfoSet::itNone)
        return false;

    ProjectExplorer::PeBay *pBay = qobject_cast<ProjectExplorer::PeBay*>(m_pProjectObject);
    if(!pBay)
        return false;

    const QList<ProjectExplorer::PeDevice*> lstDevices = pBay->GetChildDevices();
    if(!lstDevices.contains(pInfoSet->GetTxIED()) && !lstDevices.contains(pInfoSet->GetRxIED()))
        return false;

    ProjectExplorer::PeInfoSet::InfoSetType eInfoSetType = ProjectExplorer::PeInfoSet::itNone;
    int iIndex = m_pComboBoxTypeFilter->currentIndex();
    if(iIndex >= 0)
        eInfoSetType = ProjectExplorer::PeInfoSet::InfoSetType(m_pComboBoxTypeFilter->itemData(iIndex).toInt());
    if(eInfoSetType != ProjectExplorer::PeInfoSet::itNone && eInfoSetType != pInfoSet->GetInfoSetType())
        return false;

    ProjectExplorer::PeDevice *pSendDevice = 0;
    iIndex = m_pComboBoxSendIEDFilter->currentIndex();
    if(iIndex >= 0)
        pSendDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSendIEDFilter->itemData(iIndex).toInt());
    if(pSendDevice && pSendDevice != pInfoSet->GetTxIED())
        return false;

    ProjectExplorer::PeDevice *pReceiveDevice = 0;
    iIndex = m_pComboBoxReceiveIEDFilter->currentIndex();
    if(iIndex >= 0)
        pReceiveDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxReceiveIEDFilter->itemData(iIndex).toInt());
    if(pReceiveDevice && pReceiveDevice != pInfoSet->GetRxIED())
        return false;

    ProjectExplorer::PeDevice *pSwitch = 0;
    iIndex = m_pComboBoxSwitchFilter->currentIndex();
    if(iIndex >= 0)
        pSwitch = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitchFilter->itemData(iIndex).toInt());
    if(pSwitch && pSwitch != pInfoSet->GetSwitch1() && pSwitch != pInfoSet->GetSwitch2() && pSwitch != pInfoSet->GetSwitch3() && pSwitch != pInfoSet->GetSwitch4())
        return false;

    return true;
}

void ConfigWidgetInfoSet::UpdateCompleteStatus(ProjectExplorer::PeInfoSet *pInfoSet)
{
    if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromProjectObject(pInfoSet))
    {
        QModelIndex index = m_pModel->IndexFromConfigRow(pConfigRow);
        if(!index.isValid())
            return;

        m_pModel->setData(index, (pInfoSet->IsComplete() ? QColor(Qt::black) : QColor(Qt::red)), Qt::ForegroundRole);
    }
}

void ConfigWidgetInfoSet::UpdateFilberIEDs()
{
    ProjectExplorer::PeDevice *pSendIED = 0;
    int iIndex = m_pComboBoxSendIEDFilter->currentIndex();
    if(iIndex > 0)
        pSendIED = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSendIEDFilter->itemData(iIndex).toInt());

    ProjectExplorer::PeDevice *pReceiveIED = 0;
    iIndex = m_pComboBoxReceiveIEDFilter->currentIndex();
    if(iIndex > 0)
        pReceiveIED = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxReceiveIEDFilter->itemData(iIndex).toInt());

    ProjectExplorer::PeDevice *pSwitch = 0;
    iIndex = m_pComboBoxSwitchFilter->currentIndex();
    if(iIndex > 0)
        pSwitch = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSwitchFilter->itemData(iIndex).toInt());

    m_pComboBoxSendIEDFilter->blockSignals(true);
    m_pComboBoxReceiveIEDFilter->blockSignals(true);
    m_pComboBoxSwitchFilter->blockSignals(true);

    m_pComboBoxSendIEDFilter->clear();
    m_pComboBoxSendIEDFilter->addItem(tr("All"), 0);
    m_pComboBoxReceiveIEDFilter->clear();
    m_pComboBoxReceiveIEDFilter->addItem(tr("All"), 0);
    m_pComboBoxSwitchFilter->clear();
    m_pComboBoxSwitchFilter->addItem(tr("All"), 0);

    if(ProjectExplorer::PeBay *pBay = qobject_cast<ProjectExplorer::PeBay*>(m_pProjectObject))
    {
        QList<ProjectExplorer::PeDevice*> lstSendIEDs, lstReceiveIEDs, lstSwitches;
        foreach(ProjectExplorer::PeInfoSet *pInfoSet, pBay->GetProjectVersion()->GetAllInfoSets())
        {
            if(pInfoSet->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone &&
               (pBay->GetChildDevices().contains(pInfoSet->GetTxIED()) || pBay->GetChildDevices().contains(pInfoSet->GetRxIED())))
            {
                if(pInfoSet->GetTxIED() && !lstSendIEDs.contains(pInfoSet->GetTxIED()))
                    lstSendIEDs.append(pInfoSet->GetTxIED());

                if(pInfoSet->GetRxIED() && !lstReceiveIEDs.contains(pInfoSet->GetRxIED()))
                    lstReceiveIEDs.append(pInfoSet->GetRxIED());

                if(pInfoSet->GetSwitch1() && !lstSwitches.contains(pInfoSet->GetSwitch1()))
                    lstSwitches.append(pInfoSet->GetSwitch1());

                if(pInfoSet->GetSwitch2() && !lstSwitches.contains(pInfoSet->GetSwitch2()))
                    lstSwitches.append(pInfoSet->GetSwitch2());

                if(pInfoSet->GetSwitch3() && !lstSwitches.contains(pInfoSet->GetSwitch3()))
                    lstSwitches.append(pInfoSet->GetSwitch3());

                if(pInfoSet->GetSwitch4() && !lstSwitches.contains(pInfoSet->GetSwitch4()))
                    lstSwitches.append(pInfoSet->GetSwitch4());
            }
        }

        qSort(lstSendIEDs.begin(), lstSendIEDs.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
        foreach(ProjectExplorer::PeDevice *pDevice, lstSendIEDs)
            m_pComboBoxSendIEDFilter->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));

        qSort(lstReceiveIEDs.begin(), lstReceiveIEDs.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
        foreach(ProjectExplorer::PeDevice *pDevice, lstReceiveIEDs)
            m_pComboBoxReceiveIEDFilter->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));

        qSort(lstSwitches.begin(), lstSwitches.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
        foreach(ProjectExplorer::PeDevice *pDevice, lstSwitches)
            m_pComboBoxSwitchFilter->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));
    }

    if(!pSendIED)
    {
        m_pComboBoxSendIEDFilter->setCurrentIndex(0);
    }
    else
    {
        iIndex = m_pComboBoxSendIEDFilter->findData(reinterpret_cast<int>(pSendIED));
        if(iIndex < 0)
        {
            m_pComboBoxSendIEDFilter->setCurrentIndex(0);
            SlotFilterChanged(0);
        }
        else
        {
            m_pComboBoxSendIEDFilter->setCurrentIndex(iIndex);
        }
    }

    if(!pReceiveIED)
    {
        m_pComboBoxReceiveIEDFilter->setCurrentIndex(0);
    }
    else
    {
        iIndex = m_pComboBoxReceiveIEDFilter->findData(reinterpret_cast<int>(pReceiveIED));
        if(iIndex < 0)
        {
            m_pComboBoxReceiveIEDFilter->setCurrentIndex(0);
            SlotFilterChanged(0);
        }
        else
        {
            m_pComboBoxReceiveIEDFilter->setCurrentIndex(iIndex);
        }
    }

    if(!pSwitch)
    {
        m_pComboBoxSwitchFilter->setCurrentIndex(0);
    }
    else
    {
        iIndex = m_pComboBoxSwitchFilter->findData(reinterpret_cast<int>(pSwitch));
        if(iIndex < 0)
        {
            m_pComboBoxSwitchFilter->setCurrentIndex(0);
            SlotFilterChanged(0);
        }
        else
        {
            m_pComboBoxSwitchFilter->setCurrentIndex(iIndex);
        }
    }

    m_pComboBoxSendIEDFilter->blockSignals(false);
    m_pComboBoxReceiveIEDFilter->blockSignals(false);
    m_pComboBoxSwitchFilter->blockSignals(false);
}

void ConfigWidgetInfoSet::SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!m_pModel)
        return;

    UpdateFilberIEDs();

    if(ProjectExplorer::PeInfoSet *pInfoSet = qobject_cast<ProjectExplorer::PeInfoSet*>(pProjectObject))
    {
        if(CheckVInfoSet(pInfoSet))
        {
            Core::ConfigRow *pConfigRow = new Core::ConfigRow(RowDataFromProjectObject(pProjectObject), pProjectObject);
            m_pModel->AppendRow(pConfigRow);
            m_pView->SelectConfigRow(pConfigRow);

            UpdateCompleteStatus(pInfoSet);
        }
    }
}

void ConfigWidgetInfoSet::SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!m_pModel)
        return;

    UpdateFilberIEDs();

    if(ProjectExplorer::PeInfoSet *pInfoSet = qobject_cast<ProjectExplorer::PeInfoSet*>(pProjectObject))
    {
        if(pInfoSet->GetInfoSetType() == ProjectExplorer::PeInfoSet::itNone)
        {
            SlotProjectObjectAboutToBeDeleted(pProjectObject);
        }
        else if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromProjectObject(pProjectObject))
        {
            m_pModel->UpdateRow(RowDataFromProjectObject(pProjectObject), pConfigRow);
            m_pView->SelectConfigRow(pConfigRow);

            UpdateCompleteStatus(pInfoSet);
        }
        else
        {
            SlotProjectObjectCreated(pProjectObject);
        }
    }
}

void ConfigWidgetInfoSet::SlotProjectObjectDeleted()
{
    UpdateFilberIEDs();
}

void ConfigWidgetInfoSet::SlotFilterChanged(int iIndex)
{
    Q_UNUSED(iIndex)

    UpdateModel();
}

void ConfigWidgetInfoSet::SlotActionDelete()
{
    QModelIndexList lstSelectedIndex = m_pView->selectionModel()->selectedRows();
    qSort(lstSelectedIndex);

    QList<ProjectExplorer::PeInfoSet*> lstDeleteInfoSets;
    foreach(const QModelIndex &index, lstSelectedIndex)
    {
        if(Core::ConfigRow *pConfigRow = m_pModel->ConfigRowFromIndex(index))
        {
            if(ProjectExplorer::PeInfoSet *pInfoSet = qobject_cast<ProjectExplorer::PeInfoSet*>(pConfigRow->GetProjectObject()))
            {
                if(!lstDeleteInfoSets.contains(pInfoSet))
                    lstDeleteInfoSets.append(pInfoSet);

                if(ProjectExplorer::PeInfoSet *pInfoSetPair = pInfoSet->GetPairInfoSet())
                {
                    if(!lstDeleteInfoSets.contains(pInfoSetPair))
                        lstDeleteInfoSets.append(pInfoSetPair);
                }
            }
        }
    }

    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pProjectObject ? m_pProjectObject->GetProjectVersion() : 0;
    if(!pProjectVersion || lstDeleteInfoSets.isEmpty())
        return;

    QString strItems;
    foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstDeleteInfoSets)
    {
        if(pInfoSet->GetInfoSetType() == ProjectExplorer::PeInfoSet::itNone)
            continue;

        strItems = QString("%1\n%2: %3").arg(strItems).arg(pInfoSet->GetObjectTypeName()).arg(pInfoSet->GetDisplayName());
    }

    if(QMessageBox::question(this,
                             tr("Delete Confirmation"),
                             tr("Are you sure you want to delete following items?") + "\n" + strItems,
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No) == QMessageBox::Yes)
    {
        Utils::WaitCursor cursor;
        Q_UNUSED(cursor)

        ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
        foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstDeleteInfoSets)
        {
            if(!pProjectVersion->DbDeleteObject(pInfoSet->GetObjectType(), pInfoSet->GetId(), false))
            {
                DbTrans.Rollback();
                return;
            }
        }
        if(!DbTrans.Commit())
            return;

        foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstDeleteInfoSets)
            pProjectVersion->DeleteObject(pInfoSet);
    }
}
