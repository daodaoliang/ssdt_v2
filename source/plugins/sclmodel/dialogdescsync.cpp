#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QAction>
#include <QMessageBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>

#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peproject.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/pevterminalconn.h"
#include "projectexplorer/pevterminal.h"
#include "sclparser/sclelement.h"
#include "sclparser/sclhelp.h"
#include "utils/waitcursor.h"
#include "utils/readonlyview.h"

#include "dialogdescsync.h"

using namespace SclModel::Internal;

DialogDescSync::DialogDescSync(QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Description Synchronization"));

    m_pComboBoxProject = new QComboBox(this);
    m_pComboBoxProject->setEditable(false);
    m_pComboBoxProject->setMinimumWidth(150);

    foreach(ProjectExplorer::PeProject *pProject, ProjectExplorer::ProjectManager::Instance()->GetAllProjects())
    {
        foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, pProject->GetAllProjectVersions())
        {
            if(pProjectVersion->IsOpend())
                m_pComboBoxProject->addItem(pProjectVersion->GetDisplayIcon(), pProjectVersion->GetDisplayName(), reinterpret_cast<int>(pProjectVersion));
        }
    }

    QGroupBox *pGroupBoxProject = new QGroupBox(this);
    QHBoxLayout *pHBoxLayoutProject = new QHBoxLayout(pGroupBoxProject);
    pHBoxLayoutProject->setSpacing(5);
    pHBoxLayoutProject->addWidget(new QLabel(tr("Project:"), this));
    pHBoxLayoutProject->addWidget(m_pComboBoxProject, 1);

    SetupModelViewDevice();
    SetupModelViewDesc();

    QRadioButton *pRadioButtonProToStaDesc = new QRadioButton(tr("Synchronize Description from Process to Station"), this);
    QRadioButton *pRadioButtonStaToProDesc = new QRadioButton(tr("Synchronize Description from Station to Process"), this);

    m_pButtonGroup = new QButtonGroup(this);
    m_pButtonGroup->addButton(pRadioButtonProToStaDesc, 0);
    m_pButtonGroup->addButton(pRadioButtonStaToProDesc, 1);
    pRadioButtonProToStaDesc->setChecked(true);
    connect(m_pButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(SlotButtonGroupClicked(int)));

    QGroupBox *pGroupBox = new QGroupBox(this);
    QGridLayout *pGridLayoutGroupBox = new QGridLayout(pGroupBox);
    pGridLayoutGroupBox->addWidget(pRadioButtonProToStaDesc, 0, 0);
    pGridLayoutGroupBox->addWidget(pRadioButtonStaToProDesc, 0, 1);

    m_pButtonSyncCurrent = new QPushButton(this);
    m_pButtonSyncCurrent->setText(tr("Synchronize Current Description"));
    m_pButtonSyncCurrent->setFixedHeight(25);

    m_pButtonSyncAll = new QPushButton(this);
    m_pButtonSyncAll->setText(tr("Synchronize All Description"));
    m_pButtonSyncAll->setFixedHeight(25);

    m_pButtonExit = new QPushButton(this);
    m_pButtonExit->setText(tr("Exit"));
    m_pButtonExit->setFixedHeight(25);

    QVBoxLayout *pVBoxLayoutLeft = new QVBoxLayout;
    pVBoxLayoutLeft->addWidget(pGroupBoxProject);
    pVBoxLayoutLeft->addWidget(m_pViewDevice);

    QVBoxLayout *pVBoxLayoutRight = new QVBoxLayout;
    pVBoxLayoutRight->addWidget(pGroupBox);
    pVBoxLayoutRight->addWidget(m_pViewDesc);

    QHBoxLayout *pHBoxLayout = new QHBoxLayout;
    pHBoxLayout->setSpacing(5);
    pHBoxLayout->addLayout(pVBoxLayoutLeft);
    pHBoxLayout->addLayout(pVBoxLayoutRight);

    QHBoxLayout *pHBoxLayoutButton = new QHBoxLayout;
    pHBoxLayoutButton->setSpacing(5);
    pHBoxLayoutButton->addStretch(1);
    pHBoxLayoutButton->addWidget(m_pButtonSyncCurrent);
    pHBoxLayoutButton->addWidget(m_pButtonSyncAll);
    pHBoxLayoutButton->addWidget(m_pButtonExit);

    QVBoxLayout *pVBoxLayout = GetClientLayout();
    pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    pVBoxLayout->addLayout(pHBoxLayout);
    pVBoxLayout->addSpacing(15);
    pVBoxLayout->addLayout(pHBoxLayoutButton);

    connect(m_pComboBoxProject, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentProjectChanged(int)));
    connect(m_pViewDevice->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(SlotDeviceViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(m_pButtonSyncCurrent, SIGNAL(released()),
            this, SLOT(SlotButtonSyncCurrentReleased()));
    connect(m_pButtonSyncAll, SIGNAL(released()),
            this, SLOT(SlotButtonSyncAllReleased()));
    connect(m_pButtonExit, SIGNAL(released()),
            this, SLOT(close()));

    SlotCurrentProjectChanged(m_pComboBoxProject->currentIndex());
}

DialogDescSync::~DialogDescSync()
{
}

void DialogDescSync::SetupModelViewDevice()
{
    m_pModelDevice = new QStandardItemModel(0, 3, this);

    m_pViewDevice = new Utils::ReadOnlyListView(this);
    m_pViewDevice->setMinimumSize(330, 500);
    m_pViewDevice->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewDevice->setModel(m_pModelDevice);
    m_pViewDevice->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewDevice->setSelectionMode(QAbstractItemView::SingleSelection);
}

void DialogDescSync::SetupModelViewDesc()
{
    m_pModelDesc = new QStandardItemModel(0, 4, this);
    m_pModelDesc->setHeaderData(0, Qt::Horizontal, tr("Process Signal"));
    m_pModelDesc->setHeaderData(1, Qt::Horizontal, tr("Process Description"));
    m_pModelDesc->setHeaderData(2, Qt::Horizontal, tr("Station Signal"));
    m_pModelDesc->setHeaderData(3, Qt::Horizontal, tr("Station Description"));

    m_pViewDesc = new Utils::ReadOnlyTableView(this);
    m_pViewDesc->setMinimumSize(950, 500);
    m_pViewDesc->setAlternatingRowColors(true);
    m_pViewDesc->setSortingEnabled(true);
    m_pViewDesc->setModel(m_pModelDesc);
    m_pViewDesc->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewDesc->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pViewDesc->horizontalHeader()->setHighlightSections(false);
    m_pViewDesc->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pViewDesc->horizontalHeader()->setStretchLastSection(true);
    m_pViewDesc->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewDesc->verticalHeader()->hide();
    m_pViewDesc->setShowGrid(false);
    m_pViewDesc->setColumnWidth(0, 200);
    m_pViewDesc->setColumnWidth(1, 250);
    m_pViewDesc->setColumnWidth(2, 200);
    m_pViewDesc->setColumnWidth(3, 250);
}

SclParser::SCLElement* DialogDescSync::GetDOIFromRef(SclParser::SCLElement *pSCLElementIED, const QString &strRef)
{
    QStringList lstRefs = strRef.split(QLatin1Char('/'));
    if(lstRefs.size() != 2)
        return 0;

    const QString strLdInst = lstRefs.first();
    if(strLdInst.isEmpty())
        return 0;

    SclParser::SCLElement *pSCLElementLDevice = SclParser::SCLHelp::GetLDeviceFromIEDAndInst(pSCLElementIED, strLdInst);
    if(!pSCLElementLDevice)
        return 0;

    QStringList lstLnAndDo = lstRefs.at(1).split(QLatin1Char('.'));
    if(lstLnAndDo.size() < 2)
        return 0;

    SclParser::SCLElement *pSCLElementLN = pSCLElementLDevice->FindChild(lstLnAndDo.takeFirst());
    if(!pSCLElementLN)
        return 0;

    return SclParser::SCLHelp::GetDOIFromLNAndName(pSCLElementLN, lstLnAndDo.join(QLatin1Char('.')));
}

bool DialogDescSync::SyncDeviceDesc(ProjectExplorer::PeDevice *pDevice, bool bProToSta)
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = pDevice->GetProjectVersion();
    if(!pProjectVersion)
        return false;

    // Get map
    QMap<QString, QString> mapProToSta;
    QByteArray baDeviceContent;
    if(!pProjectVersion->DbReadDeviceModel(pDevice->GetId(), baDeviceContent))
        return false;

    if(baDeviceContent.isEmpty() || baDeviceContent.isNull())
        return false;

    SclParser::SCLElement SCLElementIED("IED", 0, 0);
    SCLElementIED.ReadFromBinary(baDeviceContent);

    foreach(SclParser::SCLElement *pSCLElementPrivate, SCLElementIED.FindChildByType("Private"))
    {
        if(pSCLElementPrivate->GetAttribute("type") != QLatin1String("Signal Map"))
            continue;

        QStringList lstValue = pSCLElementPrivate->GetValue().trimmed().split(QLatin1Char(';'));
        if(lstValue.size() != 2)
            continue;

        mapProToSta[lstValue.at(1)] = lstValue.at(0);
    }

    if(mapProToSta.isEmpty())
        return true;

    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, pProjectVersion->GetAllVTerminalConns())
    {
        ProjectExplorer::PeVTerminal *pVTerminalTx = pVTerminalConn->GetTxVTerminal();
        ProjectExplorer::PeVTerminal *pVTerminalRx = pVTerminalConn->GetRxVTerminal();
        if(!pVTerminalTx || !pVTerminalRx)
            continue;

        ProjectExplorer::PeDevice *pDeviceTx = pVTerminalTx->GetParentDevice();
        ProjectExplorer::PeDevice *pDeviceRx = pVTerminalRx->GetParentDevice();
        if(!pDeviceTx || !pDeviceRx)
            continue;

        if(pDeviceRx != pDevice)
            continue;

        QString strProRef = QString("%1/%2%3%4.%5").arg(pVTerminalRx->GetLDInst())
                                                   .arg(pVTerminalRx->GetLNPrefix())
                                                   .arg(pVTerminalRx->GetLNClass())
                                                   .arg(pVTerminalRx->GetLNInst())
                                                   .arg(pVTerminalRx->GetDOName());
        if(mapProToSta.contains(strProRef))
        {
            QString strStaRef = mapProToSta.take(strProRef);
            if(strStaRef.isEmpty())
                continue;

            SclParser::SCLElement *pSCLElementDOISta = GetDOIFromRef(&SCLElementIED, strStaRef);
            if(!pSCLElementDOISta)
                continue;

            if(bProToSta)
            {
                pSCLElementDOISta->SetAttribute("desc", pVTerminalTx->GetProDesc());
            }
            else
            {
                ProjectExplorer::PeVTerminal vterminal(*pVTerminalTx);
                vterminal.SetProDesc(pSCLElementDOISta->GetAttribute("desc"));
                if(pProjectVersion->DbUpdateObject(vterminal, false))
                    pProjectVersion->UpdateObject(vterminal, true);
            }
        }
    }

    if(bProToSta)
        pProjectVersion->DbUpdateDeviceModel(pDevice->GetId(), SCLElementIED.WriteToBinary(), false);

    return true;
}

void DialogDescSync::SlotCurrentProjectChanged(int iCurrentIndex)
{
    m_pModelDevice->removeRows(0, m_pModelDevice->rowCount());
    m_pModelDesc->removeRows(0, m_pModelDesc->rowCount());

    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxProject->itemData(iCurrentIndex).toInt());
    if(!pProjectVersion)
        return;

    // Build model for device
    QList<ProjectExplorer::PeDevice*> lstDevices = pProjectVersion->GetAllDevices();
    qSort(lstDevices.begin(), lstDevices.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    foreach(ProjectExplorer::PeDevice *pDevice, lstDevices)
    {
        if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtIED)
            continue;

        QStandardItem *pItem = new QStandardItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pDevice));
        m_pModelDevice->appendRow(pItem);
    }

    if(QStandardItem *pItem = m_pModelDevice->item(0))
        m_pViewDevice->selectionModel()->select(pItem->index(), QItemSelectionModel::Select);
}

void DialogDescSync::SlotDeviceViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    m_pModelDesc->removeRows(0, m_pModelDesc->rowCount());

    QModelIndexList lstIndex = m_pViewDevice->selectionModel()->selectedRows();
    if(lstIndex.size() != 1)
        return;

    QStandardItem *pItem = m_pModelDevice->itemFromIndex(lstIndex.first());
    if(!pItem)
        return;

    ProjectExplorer::PeDevice *pDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(pItem->data().toInt());
    if(!pDevice)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pDevice->GetProjectVersion();
    if(!pProjectVersion)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    // Get map
    QMap<QString, QString> mapProToSta;
    QByteArray baDeviceContent;
    if(!pProjectVersion->DbReadDeviceModel(pDevice->GetId(), baDeviceContent))
        return;

    if(baDeviceContent.isEmpty() || baDeviceContent.isNull())
        return;

    SclParser::SCLElement SCLElementIED("IED", 0, 0);
    SCLElementIED.ReadFromBinary(baDeviceContent);

    foreach(SclParser::SCLElement *pSCLElementPrivate, SCLElementIED.FindChildByType("Private"))
    {
        if(pSCLElementPrivate->GetAttribute("type") != QLatin1String("Signal Map"))
            continue;

        QStringList lstValue = pSCLElementPrivate->GetValue().trimmed().split(QLatin1Char(';'));
        if(lstValue.size() != 2)
            continue;

        mapProToSta[lstValue.at(1)] = lstValue.at(0);
    }

    if(mapProToSta.isEmpty())
        return;

    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, pProjectVersion->GetAllVTerminalConns())
    {
        ProjectExplorer::PeVTerminal *pVTerminalTx = pVTerminalConn->GetTxVTerminal();
        ProjectExplorer::PeVTerminal *pVTerminalRx = pVTerminalConn->GetRxVTerminal();
        if(!pVTerminalTx || !pVTerminalRx)
            continue;

        ProjectExplorer::PeDevice *pDeviceTx = pVTerminalTx->GetParentDevice();
        ProjectExplorer::PeDevice *pDeviceRx = pVTerminalRx->GetParentDevice();
        if(!pDeviceTx || !pDeviceRx)
            continue;

        if(pDeviceRx != pDevice)
            continue;

        QString strProRef = QString("%1/%2%3%4.%5").arg(pVTerminalRx->GetLDInst())
                                                   .arg(pVTerminalRx->GetLNPrefix())
                                                   .arg(pVTerminalRx->GetLNClass())
                                                   .arg(pVTerminalRx->GetLNInst())
                                                   .arg(pVTerminalRx->GetDOName());
        if(mapProToSta.contains(strProRef))
        {
            QString strStaRef = mapProToSta.take(strProRef);
            if(strStaRef.isEmpty())
                continue;

            SclParser::SCLElement *pSCLElementDOISta = GetDOIFromRef(&SCLElementIED, strStaRef);
            if(!pSCLElementDOISta)
                continue;

            QList<QStandardItem*> lstItems;
            lstItems.append(new QStandardItem(QString("%1%2/%3%4%5.%6").arg(pDeviceTx->GetName())
                                                     .arg(pVTerminalTx->GetLDInst())
                                                     .arg(pVTerminalTx->GetLNPrefix())
                                                     .arg(pVTerminalTx->GetLNClass())
                                                     .arg(pVTerminalTx->GetLNInst())
                                                     .arg(pVTerminalTx->GetDOName())));
            lstItems.append(new QStandardItem(QString("%1/%2").arg(pDeviceTx->GetDescription())
                                                              .arg(pVTerminalTx->GetProDesc())));
            lstItems.append(new QStandardItem(strStaRef));
            lstItems.append(new QStandardItem(pSCLElementDOISta->GetAttribute("desc")));

            m_pModelDesc->appendRow(lstItems);
        }
    }
}

void DialogDescSync::SlotButtonSyncCurrentReleased()
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    QModelIndexList lstIndex = m_pViewDevice->selectionModel()->selectedRows();
    if(lstIndex.size() != 1)
        return;

    QStandardItem *pItem = m_pModelDevice->itemFromIndex(lstIndex.first());
    if(!pItem)
        return;

    ProjectExplorer::PeDevice *pDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(pItem->data().toInt());
    if(!pDevice)
        return;

    SyncDeviceDesc(pDevice, (m_pButtonGroup->checkedId() == 0));
    SlotDeviceViewSelectionChanged(QItemSelection(), QItemSelection());
}

void DialogDescSync::SlotButtonSyncAllReleased()
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    for(int i = 0; i < m_pModelDevice->rowCount(); i++)
    {
        if(QStandardItem *pItem = m_pModelDevice->item(i))
        {
            if(ProjectExplorer::PeDevice *pDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(pItem->data().toInt()))
                SyncDeviceDesc(pDevice, (m_pButtonGroup->checkedId() == 0));
        }
    }

    SlotDeviceViewSelectionChanged(QItemSelection(), QItemSelection());
}
