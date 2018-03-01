#include <QApplication>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QFileIconProvider>
#include <QGroupBox>
#include <QMessageBox>

#include "utils/readonlyview.h"
#include "utils/searchcombobox.h"
#include "utils/waitcursor.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/peport.h"
#include "projectexplorer/peinfoset.h"
#include "projectexplorer/basemanager.h"
#include "projectexplorer/pbtpbay.h"
#include "projectexplorer/pbtpdevice.h"
#include "projectexplorer/pbtpport.h"
#include "projectexplorer/pbtpinfoset.h"

#include "baytemplatedlg.h"

using namespace BayTemplate::Internal;

static bool ValidatePortConnection(ProjectExplorer::PeInfoSet *pInfoSet,
                                   ProjectExplorer::PePort *pPort,
                                   const QList<ProjectExplorer::PeInfoSet*> &lstAllInfoSets)
{
    QList<ProjectExplorer::PePort*> lstConnectedPorts;
    foreach(ProjectExplorer::PeInfoSet *pI, lstAllInfoSets)
    {
        ProjectExplorer::PePort *pPortConnected = 0;
        pI->IsContaintPort(pPort, &pPortConnected);

        if(pPortConnected && !lstConnectedPorts.contains(pPortConnected))
            lstConnectedPorts.append(pPortConnected);
    }

    if(lstConnectedPorts.isEmpty())
        return true;

    if(lstConnectedPorts.size() == 1)
    {
        ProjectExplorer::PePort *pPortConnectedOld = lstConnectedPorts.first();

        ProjectExplorer::PePort *pPortConnectedNew = 0;
        pInfoSet->IsContaintPort(pPort, &pPortConnectedNew);

        if(pPortConnectedOld == pPortConnectedNew)
            return true;
    }

    return false;
}

static QString GetPortPath(ProjectExplorer::PePort *pPortTx, ProjectExplorer::PePort *pPortRx)
{
    QString strPortPath;

    if(pPortTx && pPortRx)
    {
        if(pPortTx->GetParentBoard() == pPortRx->GetParentBoard())
        {
            if(ProjectExplorer::PeDevice *pDevice = pPortTx->GetParentDevice())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    strPortPath = QString("%1<%2:%3/%4>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetParentBoard()->GetDisplayName()).arg(pPortTx->GetDisplayName()).arg(pPortRx->GetDisplayName());
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                    strPortPath = QString("%1<%2/%3>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetDisplayName()).arg(pPortRx->GetDisplayName());
            }
        }
        else
        {
            if(ProjectExplorer::PeDevice *pDevice = pPortTx->GetParentDevice())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    strPortPath = QString("%1<%2:%3>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetParentBoard()->GetDisplayName()).arg(pPortTx->GetDisplayName());
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                    strPortPath = QString("%1<%2>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetDisplayName());
            }

            if(ProjectExplorer::PeDevice *pDevice = pPortRx->GetParentDevice())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    strPortPath = QString("%1\n%2<%3:%4>").arg(strPortPath).arg(pDevice->GetDisplayName()).arg(pPortRx->GetParentBoard()->GetDisplayName()).arg(pPortRx->GetDisplayName());
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                    strPortPath = QString("%1\n%2<%3>").arg(strPortPath).arg(pDevice->GetDisplayName()).arg(pPortRx->GetDisplayName());
            }
        }
    }
    else if(pPortTx && !pPortRx)
    {
        if(ProjectExplorer::PeDevice *pDevice = pPortTx->GetParentDevice())
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                strPortPath = QString("%1<%2:%3>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetParentBoard()->GetDisplayName()).arg(pPortTx->GetDisplayName());
            else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                strPortPath = QString("%1<%2>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetDisplayName());
        }
    }
    else if(pPortRx && !pPortTx)
    {
        if(ProjectExplorer::PeDevice *pDevice = pPortRx->GetParentDevice())
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                strPortPath = QString("%1<%2:%3>").arg(pDevice->GetDisplayName()).arg(pPortRx->GetParentBoard()->GetDisplayName()).arg(pPortRx->GetDisplayName());
            else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                strPortPath = QString("%1<%2>").arg(pDevice->GetDisplayName()).arg(pPortRx->GetDisplayName());
        }
    }

    return strPortPath;
}

///////////////////////////////////////////////////////////////////////
// SelectProjectObjectDlg member functions
///////////////////////////////////////////////////////////////////////
SelectProjectObjectDlg::SelectProjectObjectDlg(const QString &strProjectObjectTypeName,
                                               const QList<ProjectExplorer::PeProjectObject*> lstProjectObjects,
                                               ProjectExplorer::PeProjectObject *pSelectedProjectObject,
                                               QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Select %1").arg(strProjectObjectTypeName));

    m_pComboBox = new Utils::SearchComboBox(this);
    foreach(ProjectExplorer::PeProjectObject *pProjectObject, lstProjectObjects)
    {
        if(ProjectExplorer::PePort *pPort = qobject_cast<ProjectExplorer::PePort*>(pProjectObject))
            m_pComboBox->addItem(pPort->GetDisplayIcon(), pPort->GetFullDisplayName(), reinterpret_cast<int>(pPort));
        else
            m_pComboBox->addItem(pProjectObject->GetDisplayIcon(), pProjectObject->GetDisplayName(), reinterpret_cast<int>(pProjectObject));
    }

    if(pSelectedProjectObject)
    {
        int iIndex = m_pComboBox->findData(reinterpret_cast<int>(pSelectedProjectObject));
        if(iIndex >= 0)
            m_pComboBox->setCurrentIndex(iIndex);
    }

    QGroupBox *pGroupBox = new QGroupBox(this);
    QHBoxLayout *pHBoxLayout = new QHBoxLayout(pGroupBox);
    pHBoxLayout->addWidget(m_pComboBox);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pVBoxLayout = GetClientLayout();
    pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    pVBoxLayout->addWidget(pGroupBox);
    pVBoxLayout->addSpacing(15);
    pVBoxLayout->addWidget(pDialogButtonBox);
}

ProjectExplorer::PeProjectObject* SelectProjectObjectDlg::GetSelectedProjectObject() const
{
    int iCurrentIndex = m_pComboBox->currentIndex();
    if(iCurrentIndex >= 0)
        return reinterpret_cast<ProjectExplorer::PeProjectObject*>(m_pComboBox->itemData(iCurrentIndex).toInt());

    return 0;
}

///////////////////////////////////////////////////////////////////////
// BayTemplateDlg member functions
///////////////////////////////////////////////////////////////////////
BayTemplateDlg::BayTemplateDlg(QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Bay Template"));
    setMinimumSize(QSize(1200, 600));

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pVBoxLayoutRight = new QVBoxLayout;
    pVBoxLayoutRight->addWidget(SetupTpInfoSetModelView());
    pVBoxLayoutRight->addSpacing(5);
    pVBoxLayoutRight->addWidget(SetupReplaceTpInfoSetModelView());

    QHBoxLayout *pHBoxLayoutCenter = new QHBoxLayout;
    pHBoxLayoutCenter->addWidget(SetupExplorerModelView());
    pHBoxLayoutCenter->addSpacing(5);
    pHBoxLayoutCenter->addLayout(pVBoxLayoutRight);

    QVBoxLayout *pVBoxLayoutMain = GetClientLayout();
    pVBoxLayoutMain->setContentsMargins(10, 10, 10, 10);
    pVBoxLayoutMain->addLayout(pHBoxLayoutCenter);
    pVBoxLayoutMain->addSpacing(15);
    pVBoxLayoutMain->addWidget(pDialogButtonBox);
}

BayTemplateDlg::~BayTemplateDlg()
{
}

QSize BayTemplateDlg::sizeHint() const
{
    return QSize(1050, 650);
}

QWidget* BayTemplateDlg::SetupExplorerModelView()
{
    m_pModelExplorer = new QStandardItemModel(0, 1, this);
    m_pViewExplorer = new Utils::ReadOnlyTreeView(this);
    m_pViewExplorer->setFixedWidth(200);
    m_pViewExplorer->setIndentation(m_pViewExplorer->indentation() * 9/10);
    m_pViewExplorer->setUniformRowHeights(true);
    m_pViewExplorer->setTextElideMode(Qt::ElideNone);
    m_pViewExplorer->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_pViewExplorer->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewExplorer->setDragEnabled(true);
    m_pViewExplorer->setHeaderHidden(true);
#if QT_VERSION >= 0x050000
    m_pViewExplorer->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_pViewExplorer->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    m_pViewExplorer->header()->setStretchLastSection(false);
    m_pViewExplorer->setModel(m_pModelExplorer);

    QMap<int, QList<ProjectExplorer::PbTpBay*> > mapVLevelToTpBays;
    foreach(ProjectExplorer::PbTpBay *pTpBay, ProjectExplorer::BaseManager::Instance()->GetAllTpBays())
    {
        QList<ProjectExplorer::PbTpBay*> &lstTpBays = mapVLevelToTpBays[pTpBay->GetVLevel()];
        lstTpBays.append(pTpBay);
    }

    QFileIconProvider FileIconProvider;

    QList<int> lstVLevels = mapVLevelToTpBays.keys();
    qSort(lstVLevels);
    foreach(int iVLevel, lstVLevels)
    {
        const QString strVLevel = QString("%1 kV").arg(iVLevel);
        QStandardItem *pItemVLevel = new QStandardItem(FileIconProvider.icon(QFileIconProvider::Folder), strVLevel);
        m_pModelExplorer->appendRow(pItemVLevel);

        QList<ProjectExplorer::PbTpBay*> lstTpBays = mapVLevelToTpBays.value(iVLevel);
        qSort(lstTpBays.begin(), lstTpBays.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
        foreach(ProjectExplorer::PbTpBay *pTpBay, lstTpBays)
        {
            QStandardItem *pItemTpBay = new QStandardItem(pTpBay->GetDisplayIcon(), pTpBay->GetDisplayName());
            pItemTpBay->setData(reinterpret_cast<int>(pTpBay));
            pItemVLevel->appendRow(pItemTpBay);
        }
    }

    m_pViewExplorer->expandAll();

    connect(m_pViewExplorer->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(SlotExplorerViewSelectionChanged(const QItemSelection &, const QItemSelection &)));

    return m_pViewExplorer;
}

QWidget* BayTemplateDlg::SetupTpInfoSetModelView()
{
    m_pModelTpInfoSet = new QStandardItemModel(0, 8);
    m_pModelTpInfoSet->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Type") << tr("Send IED")
                                                               << tr("First Level Switch") << tr("Second Level Switch")
                                                               << tr("Third Level Switch") << tr("Fourth Level Switch") << tr("Receive IED"));
    m_pViewTpInfoSet = new Utils::ReadOnlyTableView(this);
    m_pViewTpInfoSet->setAlternatingRowColors(true);
    m_pViewTpInfoSet->setSortingEnabled(true);
    m_pViewTpInfoSet->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewTpInfoSet->setModel(m_pModelTpInfoSet);
    m_pViewTpInfoSet->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewTpInfoSet->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pViewTpInfoSet->horizontalHeader()->setHighlightSections(false);
    m_pViewTpInfoSet->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pViewTpInfoSet->horizontalHeader()->setStretchLastSection(true);
    m_pViewTpInfoSet->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewTpInfoSet->verticalHeader()->hide();
    m_pViewTpInfoSet->setShowGrid(false);
    m_pViewTpInfoSet->setColumnWidth(0, 200);
    m_pViewTpInfoSet->setColumnWidth(1, 60);
    m_pViewTpInfoSet->setColumnWidth(2, 200);
    m_pViewTpInfoSet->setColumnWidth(3, 200);
    m_pViewTpInfoSet->setColumnWidth(4, 200);
    m_pViewTpInfoSet->setColumnWidth(5, 200);
    m_pViewTpInfoSet->setColumnWidth(6, 200);
    m_pViewTpInfoSet->setColumnWidth(7, 200);

    return m_pViewTpInfoSet;
}

QWidget* BayTemplateDlg::SetupReplaceTpInfoSetModelView()
{
    m_pComboBoxProjectVersion = new QComboBox(this);
    m_pComboBoxProjectVersion->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *pHBoxLayoutProjectVersion = new QHBoxLayout;
    pHBoxLayoutProjectVersion->addWidget(new QLabel(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otProject) + ":", this));
    pHBoxLayoutProjectVersion->addSpacing(5);
    pHBoxLayoutProjectVersion->addWidget(m_pComboBoxProjectVersion);

    QList<ProjectExplorer::PeProjectVersion*> lstProjectVersions;
    foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, ProjectExplorer::ProjectManager::Instance()->GetAllProjectVersions())
    {
        if(pProjectVersion->IsOpend())
            lstProjectVersions.append(pProjectVersion);
    }
    qSort(lstProjectVersions.begin(), lstProjectVersions.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, lstProjectVersions)
        m_pComboBoxProjectVersion->addItem(pProjectVersion->GetDisplayIcon(), pProjectVersion->GetDisplayName(), reinterpret_cast<int>(pProjectVersion));

    connect(m_pComboBoxProjectVersion, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentProjectVersionChanged(int)));

    m_pModelReplaceTpInfoSet = new QStandardItemModel(0, 2);
    m_pModelReplaceTpInfoSet->setHorizontalHeaderLabels(QStringList() << tr("Template Device/Port")
                                                                      << tr("Project Device/Port"));

    m_pViewReplaceTpInfoSet = new Utils::ReadOnlyTreeView(this);
    m_pViewReplaceTpInfoSet->setUniformRowHeights(true);
    m_pViewReplaceTpInfoSet->setTextElideMode(Qt::ElideNone);
    m_pViewReplaceTpInfoSet->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_pViewReplaceTpInfoSet->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pViewReplaceTpInfoSet->setModel(m_pModelReplaceTpInfoSet);
    m_pViewReplaceTpInfoSet->setColumnWidth(0, 450);
    m_pViewReplaceTpInfoSet->setColumnWidth(1, 450);
    connect(m_pViewReplaceTpInfoSet, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotDoubleClicked(const QModelIndex&)));

    QFrame* pFrame = new QFrame(this);
    pFrame->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QWidget *pWidget = new QWidget(this);
    QVBoxLayout *pVBoxLayout = new QVBoxLayout(pWidget);
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->addWidget(pFrame);
    pVBoxLayout->addLayout(pHBoxLayoutProjectVersion);
    pVBoxLayout->addWidget(m_pViewReplaceTpInfoSet);

    return pWidget;
}

QString BayTemplateDlg::ValidInfoset(ProjectExplorer::PeInfoSet *pInfoSet, const QList<ProjectExplorer::PeInfoSet*> &lstAllInfoSets)
{
    if(!pInfoSet)
        return "";

    ProjectExplorer::PePort *pPortError = 0;
    if(pInfoSet->GetTxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetTxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetTxPort();
    else if(pInfoSet->GetRxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetRxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetRxPort();
    else if(pInfoSet->GetSwitch1RxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch1RxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch1RxPort();
    else if(pInfoSet->GetSwitch1TxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch1TxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch1TxPort();
    else if(pInfoSet->GetSwitch2RxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch2RxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch2RxPort();
    else if(pInfoSet->GetSwitch2TxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch2TxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch2TxPort();
    else if(pInfoSet->GetSwitch3RxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch3RxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch3RxPort();
    else if(pInfoSet->GetSwitch3TxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch3TxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch3TxPort();
    else if(pInfoSet->GetSwitch4RxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch4RxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch4RxPort();
    else if(pInfoSet->GetSwitch4TxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch4TxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch4TxPort();

    if(pPortError)
        return tr("The port '%1' has connected to a different port in other information logic.").arg(GetPortPath(pPortError, 0));

    return "";
}

void BayTemplateDlg::accept()
{
    QModelIndexList lstSelectedIndex = m_pViewExplorer->selectionModel()->selectedRows();
    if(lstSelectedIndex.size() != 1)
        return;

    QStandardItem *pItem = m_pModelExplorer->itemFromIndex(lstSelectedIndex.first());
    if(!pItem)
        return;

    ProjectExplorer::PbTpBay *pTpBay = reinterpret_cast<ProjectExplorer::PbTpBay*>(pItem->data().toInt());
    if(!pTpBay)
        return;

    int iCurrentIndex = m_pComboBoxProjectVersion->currentIndex();
    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxProjectVersion->itemData(iCurrentIndex).toInt());
    if(!pProjectVersion)
        return;

    QMap<ProjectExplorer::PbTpDevice*, ProjectExplorer::PeDevice*> mapTemplateToProjectDevices;
    QMap<ProjectExplorer::PbTpPort*, ProjectExplorer::PePort*> mapTemplateToProjectPorts;

    for(int i = 0; i < m_pModelReplaceTpInfoSet->rowCount(); i++)
    {
        QStandardItem *pItemTemplateDevice = m_pModelReplaceTpInfoSet->item(i, 0);
        ProjectExplorer::PbTpDevice *pTemplateDevice = reinterpret_cast<ProjectExplorer::PbTpDevice*>(pItemTemplateDevice->data().toInt());

        QStandardItem *pItemProjectDevice = m_pModelReplaceTpInfoSet->item(i, 1);
        ProjectExplorer::PeDevice *pProjectDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(pItemProjectDevice->data().toInt());
        if(!pProjectDevice)
        {
            QMessageBox::critical(this, tr("Error"), tr("Project %1 NOT specified").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otDevice)));
            m_pViewReplaceTpInfoSet->selectionModel()->select(pItemProjectDevice->index(), QItemSelectionModel::Select);
            m_pViewReplaceTpInfoSet->scrollTo(pItemProjectDevice->index());

            return;
        }

        if(mapTemplateToProjectDevices.values().contains(pProjectDevice))
        {
            QMessageBox::critical(this, tr("Error"), tr("Project %1 can NOT be duplicate").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otDevice)));
            m_pViewReplaceTpInfoSet->selectionModel()->select(pItemProjectDevice->index(), QItemSelectionModel::Select);
            m_pViewReplaceTpInfoSet->scrollTo(pItemProjectDevice->index());

            return;
        }
        mapTemplateToProjectDevices.insert(pTemplateDevice, pProjectDevice);

        for(int j = 0; j < pItemTemplateDevice->rowCount(); j++)
        {
            QStandardItem *pItemTemplatePort = pItemTemplateDevice->child(j, 0);
            ProjectExplorer::PbTpPort *pTemplatePort = reinterpret_cast<ProjectExplorer::PbTpPort*>(pItemTemplatePort->data().toInt());

            QStandardItem *pItemProjectPort = pItemTemplateDevice->child(j, 1);
            ProjectExplorer::PePort *pProjectPort = reinterpret_cast<ProjectExplorer::PePort*>(pItemProjectPort->data().toInt());
            if(!pProjectPort)
            {
                QMessageBox::critical(this, tr("Error"), tr("Project %1 NOT specified").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otPort)));
                m_pViewReplaceTpInfoSet->selectionModel()->select(pItemProjectPort->index(), QItemSelectionModel::Select);
                m_pViewReplaceTpInfoSet->scrollTo(pItemProjectPort->index());

                return;
            }

            if(mapTemplateToProjectPorts.values().contains(pProjectPort))
            {
                QMessageBox::critical(this, tr("Error"), tr("Project %1 can NOT be duplicate").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otPort)));
                m_pViewReplaceTpInfoSet->selectionModel()->select(pItemProjectPort->index(), QItemSelectionModel::Select);
                m_pViewReplaceTpInfoSet->scrollTo(pItemProjectPort->index());

                return;
            }
            mapTemplateToProjectPorts.insert(pTemplatePort, pProjectPort);
        }
    }

    if(mapTemplateToProjectDevices.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No template %1 specified").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otInfoSet)));
        return;
    }

    int iCurrrentGroup = pProjectVersion->DbGenerateNewInfoSetGroup();
    QStringList lstExtraInfoSetNames;
    QList<ProjectExplorer::PeInfoSet> lstCreatedInfoSets;
    QList<ProjectExplorer::PbTpInfoSet*> lstTpInfoSets = pTpBay->GetChildInfoSets();
    while(!lstTpInfoSets.isEmpty())
    {
        // Create InfoSet
        ProjectExplorer::PeInfoSet InfoSet(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        ProjectExplorer::PbTpInfoSet *pTpInfoSet = lstTpInfoSets.takeFirst();

        if(ProjectExplorer::PbTpDevice *pTxIED = pTpInfoSet->GetTxIED())
            InfoSet.SetTxIED(mapTemplateToProjectDevices.value(pTxIED));
        if(ProjectExplorer::PbTpPort *pTxPort = pTpInfoSet->GetTxPort())
            InfoSet.SetTxPort(mapTemplateToProjectPorts.value(pTxPort));
        if(ProjectExplorer::PbTpDevice *pRxIED = pTpInfoSet->GetRxIED())
            InfoSet.SetRxIED(mapTemplateToProjectDevices.value(pRxIED));
        if(ProjectExplorer::PbTpPort *pRxPort = pTpInfoSet->GetRxPort())
            InfoSet.SetRxPort(mapTemplateToProjectPorts.value(pRxPort));
        if(ProjectExplorer::PbTpDevice *pSwitch1 = pTpInfoSet->GetSwitch1())
            InfoSet.SetSwitch1(mapTemplateToProjectDevices.value(pSwitch1));
        if(ProjectExplorer::PbTpPort *pSwitch1TxPort = pTpInfoSet->GetSwitch1TxPort())
            InfoSet.SetSwitch1TxPort(mapTemplateToProjectPorts.value(pSwitch1TxPort));
        if(ProjectExplorer::PbTpPort *pSwitch1RxPort = pTpInfoSet->GetSwitch1RxPort())
            InfoSet.SetSwitch1RxPort(mapTemplateToProjectPorts.value(pSwitch1RxPort));
        if(ProjectExplorer::PbTpDevice *pSwitch2 = pTpInfoSet->GetSwitch2())
            InfoSet.SetSwitch2(mapTemplateToProjectDevices.value(pSwitch2));
        if(ProjectExplorer::PbTpPort *pSwitch2TxPort = pTpInfoSet->GetSwitch2TxPort())
            InfoSet.SetSwitch2TxPort(mapTemplateToProjectPorts.value(pSwitch2TxPort));
        if(ProjectExplorer::PbTpPort *pSwitch2RxPort = pTpInfoSet->GetSwitch2RxPort())
            InfoSet.SetSwitch2RxPort(mapTemplateToProjectPorts.value(pSwitch2RxPort));
        if(ProjectExplorer::PbTpDevice *pSwitch3 = pTpInfoSet->GetSwitch3())
            InfoSet.SetSwitch3(mapTemplateToProjectDevices.value(pSwitch3));
        if(ProjectExplorer::PbTpPort *pSwitch3TxPort = pTpInfoSet->GetSwitch3TxPort())
            InfoSet.SetSwitch3TxPort(mapTemplateToProjectPorts.value(pSwitch3TxPort));
        if(ProjectExplorer::PbTpPort *pSwitch3RxPort = pTpInfoSet->GetSwitch3RxPort())
            InfoSet.SetSwitch3RxPort(mapTemplateToProjectPorts.value(pSwitch3RxPort));
        if(ProjectExplorer::PbTpDevice *pSwitch4 = pTpInfoSet->GetSwitch4())
            InfoSet.SetSwitch4(mapTemplateToProjectDevices.value(pSwitch4));
        if(ProjectExplorer::PbTpPort *pSwitch4TxPort = pTpInfoSet->GetSwitch4TxPort())
            InfoSet.SetSwitch4TxPort(mapTemplateToProjectPorts.value(pSwitch4TxPort));
        if(ProjectExplorer::PbTpPort *pSwitch4RxPort = pTpInfoSet->GetSwitch4RxPort())
            InfoSet.SetSwitch4RxPort(mapTemplateToProjectPorts.value(pSwitch4RxPort));

        InfoSet.UpdateName(lstExtraInfoSetNames);
        if(!InfoSet.GetName().isEmpty())
            lstExtraInfoSetNames.append(InfoSet.GetName());
        InfoSet.SetDescription(pTpInfoSet->GetName());
        InfoSet.SetInfoSetType(ProjectExplorer::PeInfoSet::InfoSetType(pTpInfoSet->GetInfoSetType()));
        InfoSet.SetGroup(iCurrrentGroup);

        lstCreatedInfoSets.append(InfoSet);

        // Create Pair InfoSet
        if(ProjectExplorer::PbTpInfoSet *pTpInfoSetPair = pTpInfoSet->GetPairInfoSet())
        {
            lstTpInfoSets.removeAll(pTpInfoSetPair);

            ProjectExplorer::PeInfoSet InfoSetPair(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);

            if(ProjectExplorer::PbTpDevice *pTxIED = pTpInfoSetPair->GetTxIED())
                InfoSetPair.SetTxIED(mapTemplateToProjectDevices.value(pTxIED));
            if(ProjectExplorer::PbTpPort *pTxPort = pTpInfoSetPair->GetTxPort())
                InfoSetPair.SetTxPort(mapTemplateToProjectPorts.value(pTxPort));
            if(ProjectExplorer::PbTpDevice *pRxIED = pTpInfoSetPair->GetRxIED())
                InfoSetPair.SetRxIED(mapTemplateToProjectDevices.value(pRxIED));
            if(ProjectExplorer::PbTpPort *pRxPort = pTpInfoSetPair->GetRxPort())
                InfoSetPair.SetRxPort(mapTemplateToProjectPorts.value(pRxPort));
            if(ProjectExplorer::PbTpDevice *pSwitch1 = pTpInfoSetPair->GetSwitch1())
                InfoSetPair.SetSwitch1(mapTemplateToProjectDevices.value(pSwitch1));
            if(ProjectExplorer::PbTpPort *pSwitch1TxPort = pTpInfoSetPair->GetSwitch1TxPort())
                InfoSetPair.SetSwitch1TxPort(mapTemplateToProjectPorts.value(pSwitch1TxPort));
            if(ProjectExplorer::PbTpPort *pSwitch1RxPort = pTpInfoSetPair->GetSwitch1RxPort())
                InfoSetPair.SetSwitch1RxPort(mapTemplateToProjectPorts.value(pSwitch1RxPort));
            if(ProjectExplorer::PbTpDevice *pSwitch2 = pTpInfoSetPair->GetSwitch2())
                InfoSetPair.SetSwitch2(mapTemplateToProjectDevices.value(pSwitch2));
            if(ProjectExplorer::PbTpPort *pSwitch2TxPort = pTpInfoSetPair->GetSwitch2TxPort())
                InfoSetPair.SetSwitch2TxPort(mapTemplateToProjectPorts.value(pSwitch2TxPort));
            if(ProjectExplorer::PbTpPort *pSwitch2RxPort = pTpInfoSetPair->GetSwitch2RxPort())
                InfoSetPair.SetSwitch2RxPort(mapTemplateToProjectPorts.value(pSwitch2RxPort));
            if(ProjectExplorer::PbTpDevice *pSwitch3 = pTpInfoSetPair->GetSwitch3())
                InfoSetPair.SetSwitch3(mapTemplateToProjectDevices.value(pSwitch3));
            if(ProjectExplorer::PbTpPort *pSwitch3TxPort = pTpInfoSetPair->GetSwitch3TxPort())
                InfoSetPair.SetSwitch3TxPort(mapTemplateToProjectPorts.value(pSwitch3TxPort));
            if(ProjectExplorer::PbTpPort *pSwitch3RxPort = pTpInfoSetPair->GetSwitch3RxPort())
                InfoSetPair.SetSwitch3RxPort(mapTemplateToProjectPorts.value(pSwitch3RxPort));
            if(ProjectExplorer::PbTpDevice *pSwitch4 = pTpInfoSetPair->GetSwitch4())
                InfoSetPair.SetSwitch4(mapTemplateToProjectDevices.value(pSwitch4));
            if(ProjectExplorer::PbTpPort *pSwitch4TxPort = pTpInfoSetPair->GetSwitch4TxPort())
                InfoSetPair.SetSwitch4TxPort(mapTemplateToProjectPorts.value(pSwitch4TxPort));
            if(ProjectExplorer::PbTpPort *pSwitch4RxPort = pTpInfoSetPair->GetSwitch4RxPort())
                InfoSetPair.SetSwitch4RxPort(mapTemplateToProjectPorts.value(pSwitch4RxPort));

            InfoSetPair.UpdateName(lstExtraInfoSetNames);
            if(!InfoSetPair.GetName().isEmpty())
                lstExtraInfoSetNames.append(InfoSetPair.GetName());
            InfoSetPair.SetDescription(pTpInfoSetPair->GetName());
            InfoSetPair.SetInfoSetType(ProjectExplorer::PeInfoSet::InfoSetType(pTpInfoSetPair->GetInfoSetType()));
            InfoSetPair.SetGroup(iCurrrentGroup);

            lstCreatedInfoSets.append(InfoSetPair);
        }

        iCurrrentGroup++;
    }

    ////////////////////////////////////////////////////////////////////
    // Validate Infosets
    ////////////////////////////////////////////////////////////////////
    foreach(ProjectExplorer::PeInfoSet InfoSet, lstCreatedInfoSets)
    {
        QString strError = ValidInfoset(&InfoSet, pProjectVersion->GetAllInfoSets());
        if(!strError.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), strError);
            return;
        }
    }

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    ///////////////////////////////////////////////////////////////////////
    // Handle Database
    ///////////////////////////////////////////////////////////////////////
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

    // Create new infoset
    for(int i = 0; i < lstCreatedInfoSets.size(); i++)
    {
        ProjectExplorer::PeInfoSet &InfoSet = lstCreatedInfoSets[i];
        if(!pProjectVersion->DbCreateObject(InfoSet, false))
        {
            DbTrans.Rollback();
            return;
        }

    }

    if(!DbTrans.Commit())
        return;

    ///////////////////////////////////////////////////////////////////////
    // Handle Object
    ///////////////////////////////////////////////////////////////////////

    // Create new infoset
    foreach(ProjectExplorer::PeInfoSet InfoSet, lstCreatedInfoSets)
    {
        if(!pProjectVersion->CreateObject(InfoSet))
            return;
    }

    QDialog::accept();
}

void BayTemplateDlg::SlotExplorerViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    QModelIndexList lstSelectedIndex = m_pViewExplorer->selectionModel()->selectedRows();
    if(lstSelectedIndex.size() != 1)
        return;

    QStandardItem *pItem = m_pModelExplorer->itemFromIndex(lstSelectedIndex.first());
    if(!pItem)
        return;

    ProjectExplorer::PbTpBay *pTpBay = reinterpret_cast<ProjectExplorer::PbTpBay*>(pItem->data().toInt());
    if(!pTpBay)
        return;

    ////////////////////////////////////////////////////////////
    // Build TpInfoSet Model
    ////////////////////////////////////////////////////////////
    QMap<ProjectExplorer::PbTpDevice*, QList<ProjectExplorer::PbTpPort*> > mapFromTpDeviceToTpPorts;

    m_pModelTpInfoSet->removeRows(0, m_pModelTpInfoSet->rowCount());
    foreach(ProjectExplorer::PbTpInfoSet *pTpInfoSet, pTpBay->GetChildInfoSets())
    {
        ProjectExplorer::PbTpDevice *pTxIED = pTpInfoSet->GetTxIED();
        ProjectExplorer::PbTpDevice *pRxIED = pTpInfoSet->GetRxIED();
        ProjectExplorer::PbTpDevice *pSwitch1 = pTpInfoSet->GetSwitch1();
        ProjectExplorer::PbTpDevice *pSwitch2 = pTpInfoSet->GetSwitch2();
        ProjectExplorer::PbTpDevice *pSwitch3 = pTpInfoSet->GetSwitch3();
        ProjectExplorer::PbTpDevice *pSwitch4 = pTpInfoSet->GetSwitch4();

        QList<QStandardItem*> lstItems;

        QStandardItem *pItem = new QStandardItem(pTpInfoSet->GetDisplayIcon(), pTpInfoSet->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pTpInfoSet));
        lstItems.append(pItem);

        pItem = new QStandardItem(ProjectExplorer::PbTpInfoSet::GetInfoSetTypeName(pTpInfoSet->GetInfoSetType()));
        pItem->setData(reinterpret_cast<int>(pTpInfoSet));
        lstItems.append(pItem);

        QString strTxIED = "";
        if(pTxIED)
        {
            QList<ProjectExplorer::PbTpPort*> &lstTpPorts = mapFromTpDeviceToTpPorts[pTxIED];

            strTxIED = pTxIED->GetDisplayName();
            if(ProjectExplorer::PbTpPort *pTxPort = pTpInfoSet->GetTxPort())
            {
                strTxIED = QString("%1<%2>").arg(strTxIED).arg(pTxPort->GetDisplayName());

                if(!lstTpPorts.contains(pTxPort))
                    lstTpPorts.append(pTxPort);
            }
        }
        pItem = new QStandardItem(strTxIED);
        pItem->setData(reinterpret_cast<int>(pTpInfoSet));
        lstItems.append(pItem);

        QString strSwitch1 = "";
        if(pSwitch1)
        {
            QList<ProjectExplorer::PbTpPort*> &lstTpPorts = mapFromTpDeviceToTpPorts[pSwitch1];

            strSwitch1 = pSwitch1->GetDisplayName();
            if(ProjectExplorer::PbTpPort *pSwitch1TxPort = pTpInfoSet->GetSwitch1TxPort())
            {
                strSwitch1 = QString("%1<%2>").arg(strSwitch1).arg(pSwitch1TxPort->GetDisplayName());

                if(!lstTpPorts.contains(pSwitch1TxPort))
                    lstTpPorts.append(pSwitch1TxPort);
            }

            if(ProjectExplorer::PbTpPort *pSwitch1RxPort = pTpInfoSet->GetSwitch1RxPort())
            {
                strSwitch1 = QString("<%1>%2").arg(pSwitch1RxPort->GetDisplayName()).arg(strSwitch1);

                if(!lstTpPorts.contains(pSwitch1RxPort))
                    lstTpPorts.append(pSwitch1RxPort);
            }
        }
        pItem = new QStandardItem(strSwitch1);
        pItem->setData(reinterpret_cast<int>(pTpInfoSet));
        lstItems.append(pItem);

        QString strSwitch2 = "";
        if(pSwitch2)
        {
            QList<ProjectExplorer::PbTpPort*> &lstTpPorts = mapFromTpDeviceToTpPorts[pSwitch2];

            strSwitch2 = pSwitch2->GetDisplayName();
            if(ProjectExplorer::PbTpPort *pSwitch2TxPort = pTpInfoSet->GetSwitch2TxPort())
            {
                strSwitch2 = QString("%1<%2>").arg(strSwitch2).arg(pSwitch2TxPort->GetDisplayName());

                if(!lstTpPorts.contains(pSwitch2TxPort))
                    lstTpPorts.append(pSwitch2TxPort);
            }

            if(ProjectExplorer::PbTpPort *pSwitch2RxPort = pTpInfoSet->GetSwitch2RxPort())
            {
                strSwitch2 = QString("<%1>%2").arg(pSwitch2RxPort->GetDisplayName()).arg(strSwitch2);

                if(!lstTpPorts.contains(pSwitch2RxPort))
                    lstTpPorts.append(pSwitch2RxPort);
            }
        }
        pItem = new QStandardItem(strSwitch2);
        pItem->setData(reinterpret_cast<int>(pTpInfoSet));
        lstItems.append(pItem);

        QString strSwitch3 = "";
        if(pSwitch3)
        {
            QList<ProjectExplorer::PbTpPort*> &lstTpPorts = mapFromTpDeviceToTpPorts[pSwitch3];

            strSwitch3 = pSwitch3->GetDisplayName();
            if(ProjectExplorer::PbTpPort *pSwitch3TxPort = pTpInfoSet->GetSwitch3TxPort())
            {
                strSwitch3 = QString("%1<%2>").arg(strSwitch3).arg(pSwitch3TxPort->GetDisplayName());

                if(!lstTpPorts.contains(pSwitch3TxPort))
                    lstTpPorts.append(pSwitch3TxPort);
            }

            if(ProjectExplorer::PbTpPort *pSwitch3RxPort = pTpInfoSet->GetSwitch3RxPort())
            {
                strSwitch3 = QString("<%1>%2").arg(pSwitch3RxPort->GetDisplayName()).arg(strSwitch3);

                if(!lstTpPorts.contains(pSwitch3RxPort))
                    lstTpPorts.append(pSwitch3RxPort);
            }
        }
        pItem = new QStandardItem(strSwitch3);
        pItem->setData(reinterpret_cast<int>(pTpInfoSet));
        lstItems.append(pItem);

        QString strSwitch4 = "";
        if(pSwitch4)
        {
            QList<ProjectExplorer::PbTpPort*> &lstTpPorts = mapFromTpDeviceToTpPorts[pSwitch4];

            strSwitch4 = pSwitch4->GetDisplayName();
            if(ProjectExplorer::PbTpPort *pSwitch4TxPort = pTpInfoSet->GetSwitch4TxPort())
            {
                strSwitch4 = QString("%1<%2>").arg(strSwitch4).arg(pSwitch4TxPort->GetDisplayName());

                if(!lstTpPorts.contains(pSwitch4TxPort))
                    lstTpPorts.append(pSwitch4TxPort);
            }

            if(ProjectExplorer::PbTpPort *pSwitch4RxPort = pTpInfoSet->GetSwitch4RxPort())
            {
                strSwitch4 = QString("<%1>%2").arg(pSwitch4RxPort->GetDisplayName()).arg(strSwitch4);

                if(!lstTpPorts.contains(pSwitch4RxPort))
                    lstTpPorts.append(pSwitch4RxPort);
            }
        }
        pItem = new QStandardItem(strSwitch4);
        pItem->setData(reinterpret_cast<int>(pTpInfoSet));
        lstItems.append(pItem);

        QString strRxIED = "";
        if(pRxIED)
        {
            QList<ProjectExplorer::PbTpPort*> &lstTpPorts = mapFromTpDeviceToTpPorts[pRxIED];

            strRxIED = pRxIED->GetDisplayName();
            if(ProjectExplorer::PbTpPort *pRxPort = pTpInfoSet->GetRxPort())
            {
                strRxIED = QString("<%1>%3").arg(pRxPort->GetDisplayName()).arg(strRxIED);

                if(!lstTpPorts.contains(pRxPort))
                    lstTpPorts.append(pRxPort);
            }
        }
        pItem = new QStandardItem(strRxIED);
        pItem->setData(reinterpret_cast<int>(pTpInfoSet));
        lstItems.append(pItem);

        m_pModelTpInfoSet->appendRow(lstItems);
    }

    ////////////////////////////////////////////////////////////
    // Build ReplaceTpInfoSet Model
    ////////////////////////////////////////////////////////////
    m_pModelReplaceTpInfoSet->removeRows(0, m_pModelReplaceTpInfoSet->rowCount());
    foreach(ProjectExplorer::PbTpDevice *pTpDevice, mapFromTpDeviceToTpPorts.keys())
    {
        QList<QStandardItem*> lstDeviceItems;

        QStandardItem *pItemTemplateDevice = new QStandardItem(pTpDevice->GetDisplayIcon(), pTpDevice->GetDisplayName());
        pItemTemplateDevice->setData(reinterpret_cast<int>(pTpDevice));
        lstDeviceItems.append(pItemTemplateDevice);

        lstDeviceItems.append(new QStandardItem);

        m_pModelReplaceTpInfoSet->appendRow(lstDeviceItems);

        QList<ProjectExplorer::PbTpPort*> lstTpPorts = mapFromTpDeviceToTpPorts.value(pTpDevice);
        qSort(lstTpPorts.begin(), lstTpPorts.end(), ProjectExplorer::PbTpPort::CompareGroup);
        foreach(ProjectExplorer::PbTpPort *pTpPort, lstTpPorts)
        {
            QList<QStandardItem*> lstPortItems;

            QStandardItem *pItemTemplatePort = new QStandardItem(pTpPort->GetDisplayIcon(), pTpPort->GetDisplayName());
            pItemTemplatePort->setData(reinterpret_cast<int>(pTpPort));
            lstPortItems.append(pItemTemplatePort);

            lstPortItems.append(new QStandardItem);

            pItemTemplateDevice->appendRow(lstPortItems);
        }
    }
    m_pViewReplaceTpInfoSet->expandAll();

    SlotCurrentProjectVersionChanged(m_pComboBoxProjectVersion->currentIndex());
}

void BayTemplateDlg::SlotCurrentProjectVersionChanged(int iCurrentIndex)
{
    Q_UNUSED(iCurrentIndex);

    for(int i = 0; i < m_pModelReplaceTpInfoSet->rowCount(); i++)
    {
        QStandardItem *pItemProjectDevice = m_pModelReplaceTpInfoSet->item(i, 1);
        pItemProjectDevice->setIcon(QIcon());
        pItemProjectDevice->setText(tr("No Device Specified"));
        pItemProjectDevice->setForeground(Qt::red);
        pItemProjectDevice->setData(0);

        QStandardItem *pItemTemplateDevice = m_pModelReplaceTpInfoSet->item(i, 0);
        for(int j = 0; j < pItemTemplateDevice->rowCount(); j++)
        {
            QStandardItem *pItemProjectPort = pItemTemplateDevice->child(j, 1);
            pItemProjectPort->setIcon(QIcon());
            pItemProjectPort->setText(tr("No Port Specified"));
            pItemProjectPort->setForeground(Qt::red);
            pItemProjectPort->setData(0);
        }
    }
}

void BayTemplateDlg::SlotDoubleClicked(const QModelIndex &index)
{
    int iCurrentIndex = m_pComboBoxProjectVersion->currentIndex();
    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxProjectVersion->itemData(iCurrentIndex).toInt());
    if(!pProjectVersion)
        return;

    QStandardItem *pItem = m_pModelReplaceTpInfoSet->itemFromIndex(index);
    if(pItem->column() != 1)
        return;

    if(!pItem->parent()) // Device
    {
        QStandardItem *pItemTemplateDevice = m_pModelReplaceTpInfoSet->itemFromIndex(m_pModelReplaceTpInfoSet->index(pItem->row(), 0));
        if(!pItemTemplateDevice)
            return;

        ProjectExplorer::PbTpDevice *pTemplateDevice = reinterpret_cast<ProjectExplorer::PbTpDevice*>(pItemTemplateDevice->data().toInt());
        if(!pTemplateDevice)
            return;

        QList<ProjectExplorer::PeProjectObject*> lstDevices;
        foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
        {
            if(((pTemplateDevice->GetDeviceType() == ProjectExplorer::PbTpDevice::dtIED) && (pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)) ||
               ((pTemplateDevice->GetDeviceType() == ProjectExplorer::PbTpDevice::dtSwitch) && (pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)))
            {
                lstDevices.append(pDevice);
            }
        }

        ProjectExplorer::PeDevice *pProjectDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(pItem->data().toInt());
        SelectProjectObjectDlg dlg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otDevice),
                                   lstDevices,
                                   pProjectDevice,
                                   this);
        if(dlg.exec() != QDialog::Accepted)
            return;

        pProjectDevice = qobject_cast<ProjectExplorer::PeDevice*>(dlg.GetSelectedProjectObject());
        if(!pProjectDevice)
            return;

        pItem->setIcon(pProjectDevice->GetDisplayIcon());
        pItem->setText(pProjectDevice->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pProjectDevice));
        pItem->setForeground(Qt::black);

        for(int i = 0; i < pItemTemplateDevice->rowCount(); i++)
        {
            QStandardItem *pItemProjectPort = pItemTemplateDevice->child(i, 1);
            pItemProjectPort->setIcon(QIcon());
            pItemProjectPort->setText(tr("No Port Specified"));
            pItemProjectPort->setForeground(Qt::red);
            pItemProjectPort->setData(0);
        }
    }
    else // Port
    {
        QStandardItem *pItemTemplateDevice = pItem->parent();
        if(!pItemTemplateDevice)
            return;

        ProjectExplorer::PbTpDevice *pTemplateDevice = reinterpret_cast<ProjectExplorer::PbTpDevice*>(pItemTemplateDevice->data().toInt());
        if(!pTemplateDevice)
            return;

        QStandardItem *pItemProjectDevice = m_pModelReplaceTpInfoSet->item(pItemTemplateDevice->row(), 1);
        if(!pItemProjectDevice)
            return;

        ProjectExplorer::PeDevice *pProjectDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(pItemProjectDevice->data().toInt());
        if(!pProjectDevice)
            return;

        QStandardItem *pItemTemplatePort = pItemTemplateDevice->child(pItem->row(), 0);
        if(!pItemTemplatePort)
            return;

        ProjectExplorer::PbTpPort *pTemplatePort = reinterpret_cast<ProjectExplorer::PbTpPort*>(pItemTemplatePort->data().toInt());
        if(!pTemplatePort)
            return;

        QList<ProjectExplorer::PeProjectObject*> lstPorts;
        foreach(ProjectExplorer::PeBoard *pBoard, pProjectDevice->GetChildBoards())
        {
            foreach(ProjectExplorer::PePort *pPort, pBoard->GetChildPorts())
            {
                if(((pTemplatePort->GetPortDirection() == ProjectExplorer::PbTpPort::pdRx) && (pPort->GetPortDirection() == ProjectExplorer::PePort::pdRx)) ||
                   ((pTemplatePort->GetPortDirection() == ProjectExplorer::PbTpPort::pdTx) && (pPort->GetPortDirection() == ProjectExplorer::PePort::pdTx)) ||
                   ((pTemplatePort->GetPortDirection() == ProjectExplorer::PbTpPort::pdRT) && (pPort->GetPortDirection() == ProjectExplorer::PePort::pdRT)))
                {
                    lstPorts.append(pPort);
                }
            }
        }

        ProjectExplorer::PePort *pProjectPort = reinterpret_cast<ProjectExplorer::PePort*>(pItem->data().toInt());
        SelectProjectObjectDlg dlg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otPort),
                                   lstPorts,
                                   pProjectPort,
                                   this);
        if(dlg.exec() != QDialog::Accepted)
            return;

        pProjectPort = qobject_cast<ProjectExplorer::PePort*>(dlg.GetSelectedProjectObject());
        if(!pProjectPort)
            return;

        pItem->setIcon(pProjectPort->GetDisplayIcon());
        pItem->setText(pProjectPort->GetFullDisplayName());
        pItem->setData(reinterpret_cast<int>(pProjectPort));
        pItem->setForeground(Qt::black);
    }
}

