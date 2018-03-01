#include <QApplication>
#include <QStackedWidget>
#include <QTabBar>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QToolButton>
#include <QAction>
#include <QVBoxLayout>

#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pedevice.h"
#include "sclparser/sclelement.h"
#include "styledui/styledbar.h"
#include "utils/waitcursor.h"
#include "utils/readonlyview.h"

#include "subnetworkmodewidget.h"
#include "propertydlgaddress.h"
#include "propertydlggse.h"
#include "propertydlgsmv.h"
#include "dialogbatchedit.h"

using namespace SclModel::Internal;

SubNetworkModeWidget::SubNetworkModeWidget(QWidget *pParent)
    : StyledUi::StyledWidget(pParent)
{
    SetupAddrModelView();
    SetupGseModelView();
    SetupSmvModelView();

    m_pTabBar = new QTabBar(this);
    m_pTabBar->setShape(QTabBar::RoundedSouth);
    m_pTabBar->setExpanding(false);
    m_pTabBar->addTab(tr("Address"));
    m_pTabBar->addTab(tr("GSE"));
    m_pTabBar->addTab(tr("SMV"));

    m_pStackedWidget = new QStackedWidget(this);
    m_pStackedWidget->addWidget(m_pViewAddr);
    m_pStackedWidget->addWidget(m_pViewGse);
    m_pStackedWidget->addWidget(m_pViewSmv);

    QVBoxLayout *pVBoxLayoutMain = new QVBoxLayout(this);
    pVBoxLayoutMain->setContentsMargins(0, 0, 0, 0);
    pVBoxLayoutMain->setSpacing(0);
    pVBoxLayoutMain->addWidget(SetupToolBar());
    pVBoxLayoutMain->addWidget(m_pStackedWidget);
    pVBoxLayoutMain->addWidget(m_pTabBar);

    connect(m_pTabBar, SIGNAL(currentChanged(int)),
            m_pStackedWidget, SLOT(setCurrentIndex(int)));
    connect(m_pTabBar, SIGNAL(currentChanged(int)),
            this, SLOT(SlotCurrentTabChanged(int)));
}

SubNetworkModeWidget::~SubNetworkModeWidget()
{
}

void SubNetworkModeWidget::SetProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    ProjectExplorer::PeProjectVersion *pProjectVersionNew = pProjectObject ? pProjectObject->GetProjectVersion() : 0;
    if(m_pProjectVersion != pProjectVersionNew)
    {
        SclParser::SCLElement *pSCLElementCommunication = 0;

        m_pProjectVersion = pProjectVersionNew;
        if(m_pProjectVersion)
        {
            QByteArray baCommunicationContent;
            if(m_pProjectVersion->DbReadSclCommunication(baCommunicationContent) && !baCommunicationContent.isNull() && !baCommunicationContent.isEmpty())
            {
                pSCLElementCommunication = new SclParser::SCLElement("Communication", 0, 0);
                pSCLElementCommunication->ReadFromBinary(baCommunicationContent);
            }
        }

        BuildAddrModel(pSCLElementCommunication);
        BuildGseModel(pSCLElementCommunication);
        BuildSmvModel(pSCLElementCommunication);

        if(pSCLElementCommunication)
            delete pSCLElementCommunication;
    }

    m_pStyleBar->setVisible(pProjectObject);
    m_pTabBar->setVisible(pProjectObject);
    m_pStackedWidget->setVisible(pProjectObject);

    UpdateActions();
}

QWidget* SubNetworkModeWidget::SetupToolBar()
{
    m_pStyleBar = new StyledUi::StyledBar(this);

    m_pActionRefresh = new QAction(QIcon(":/sclmodel/images/oper_refresh.png"), tr("Refresh"), m_pStyleBar);
    connect(m_pActionRefresh, SIGNAL(triggered()), this, SLOT(SlotActionRefresh()));
    QToolButton *pToolButtonRefresh = new QToolButton(m_pStyleBar);
    pToolButtonRefresh->setFocusPolicy(Qt::NoFocus);
    pToolButtonRefresh->setAutoRaise(true);
    pToolButtonRefresh->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonRefresh->setDefaultAction(m_pActionRefresh);

    m_pActionBatchEdit = new QAction(QIcon(":/sclmodel/images/oper_batchedit.png"), tr("Batch Edit"), this);
    connect(m_pActionBatchEdit, SIGNAL(triggered()), this, SLOT(SlotActionBatchEdit()));
    QToolButton *pToolButtonBatchEdit = new QToolButton(m_pStyleBar);
    pToolButtonBatchEdit->setFocusPolicy(Qt::NoFocus);
    pToolButtonBatchEdit->setAutoRaise(true);
    pToolButtonBatchEdit->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonBatchEdit->setDefaultAction(m_pActionBatchEdit);

    m_pActionEdit = new QAction(QIcon(":/sclmodel/images/oper_edit.png"), tr("Edit"), this);
    connect(m_pActionEdit, SIGNAL(triggered()), this, SLOT(SlotActionEdit()));
    QToolButton *pToolButtonEdit = new QToolButton(m_pStyleBar);
    pToolButtonEdit->setFocusPolicy(Qt::NoFocus);
    pToolButtonEdit->setAutoRaise(true);
    pToolButtonEdit->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonEdit->setDefaultAction(m_pActionEdit);

    QHBoxLayout *pHBoxLayout = new QHBoxLayout(m_pStyleBar);
    pHBoxLayout->setContentsMargins(10, 0, 0, 0);
    pHBoxLayout->addStretch(1);
    pHBoxLayout->addWidget(pToolButtonRefresh);
    pHBoxLayout->addWidget(pToolButtonBatchEdit);
    pHBoxLayout->addWidget(pToolButtonEdit);

    QAction *pActionSeparatorAddr = new QAction(this);
    pActionSeparatorAddr->setSeparator(true);

    QAction *pActionSeparatorGse = new QAction(this);
    pActionSeparatorGse->setSeparator(true);

    QAction *pActionSeparatorSmv = new QAction(this);
    pActionSeparatorSmv->setSeparator(true);

    m_pViewAddr->addAction(m_pActionRefresh);
    m_pViewAddr->addAction(pActionSeparatorAddr);
    m_pViewAddr->addAction(m_pActionBatchEdit);
    m_pViewAddr->addAction(m_pActionEdit);

    m_pViewGse->addAction(m_pActionRefresh);
    m_pViewGse->addAction(pActionSeparatorGse);
    m_pViewGse->addAction(m_pActionBatchEdit);
    m_pViewGse->addAction(m_pActionEdit);

    m_pViewSmv->addAction(m_pActionRefresh);
    m_pViewSmv->addAction(pActionSeparatorSmv);
    m_pViewSmv->addAction(m_pActionBatchEdit);
    m_pViewSmv->addAction(m_pActionEdit);

    return m_pStyleBar;
}

void SubNetworkModeWidget::SetupAddrModelView()
{
    m_pViewAddr = new Utils::ReadOnlyTableView(this);
    m_pViewAddr->setObjectName("Styled_TableView");
    m_pViewAddr->setFrameStyle(QFrame::NoFrame);
    m_pViewAddr->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewAddr->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pViewAddr->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewAddr->setSortingEnabled(true);
    m_pViewAddr->verticalHeader()->hide();
#if QT_VERSION >= 0x050000
    m_pViewAddr->verticalHeader()->setSectionsClickable(false);
    m_pViewAddr->horizontalHeader()->setSectionsClickable(true);
    m_pViewAddr->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
    m_pViewAddr->verticalHeader()->setClickable(false);
    m_pViewAddr->horizontalHeader()->setClickable(true);
    m_pViewAddr->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
    m_pViewAddr->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewAddr->horizontalHeader()->setHighlightSections(false);
    m_pViewAddr->horizontalHeader()->setStretchLastSection(true);
    m_pViewAddr->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewAddr->setShowGrid(false);

    m_pModelAddr = new QStandardItemModel(0, 5, this);
    m_pModelAddr->setHeaderData(0, Qt::Horizontal, tr("Subnetwork"));
    m_pModelAddr->setHeaderData(1, Qt::Horizontal, tr("Device"));
    m_pModelAddr->setHeaderData(2, Qt::Horizontal, tr("AccessPoint"));
    m_pModelAddr->setHeaderData(3, Qt::Horizontal, tr("IP"));
    m_pModelAddr->setHeaderData(4, Qt::Horizontal, tr("IP-SUBNET"));
    m_pViewAddr->setModel(m_pModelAddr);

    m_pViewAddr->setColumnWidth(0, 200);
    m_pViewAddr->setColumnWidth(1, 250);
    m_pViewAddr->setColumnWidth(2, 100);
    m_pViewAddr->setColumnWidth(3, 210);
    m_pViewAddr->setColumnWidth(4, 210);

    connect(m_pViewAddr->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(m_pViewAddr, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewDoubleClicked(const QModelIndex&)));
}

void SubNetworkModeWidget::SetupGseModelView()
{
    m_pViewGse = new Utils::ReadOnlyTableView(this);
    m_pViewGse->setObjectName("Styled_TableView");
    m_pViewGse->setFrameStyle(QFrame::NoFrame);
    m_pViewGse->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewGse->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pViewGse->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewGse->setSortingEnabled(true);
    m_pViewGse->verticalHeader()->hide();
#if QT_VERSION >= 0x050000
    m_pViewGse->verticalHeader()->setSectionsClickable(false);
    m_pViewGse->horizontalHeader()->setSectionsClickable(true);
    m_pViewGse->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
    m_pViewGse->verticalHeader()->setClickable(false);
    m_pViewGse->horizontalHeader()->setClickable(true);
    m_pViewGse->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
    m_pViewGse->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewGse->horizontalHeader()->setHighlightSections(false);
    m_pViewGse->horizontalHeader()->setStretchLastSection(true);
    m_pViewGse->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewGse->setShowGrid(false);

    m_pModelGse = new QStandardItemModel(0, 11, this);
    m_pModelGse->setHeaderData(0, Qt::Horizontal, tr("Subnetwork"));
    m_pModelGse->setHeaderData(1, Qt::Horizontal, tr("Device"));
    m_pModelGse->setHeaderData(2, Qt::Horizontal, tr("AccessPoint"));
    m_pModelGse->setHeaderData(3, Qt::Horizontal, tr("LDevice"));
    m_pModelGse->setHeaderData(4, Qt::Horizontal, tr("Control Block"));
    m_pModelGse->setHeaderData(5, Qt::Horizontal, tr("Mac Address"));
    m_pModelGse->setHeaderData(6, Qt::Horizontal, tr("VLan Id"));
    m_pModelGse->setHeaderData(7, Qt::Horizontal, tr("VLan Priority"));
    m_pModelGse->setHeaderData(8, Qt::Horizontal, tr("Application Id"));
    m_pModelGse->setHeaderData(9, Qt::Horizontal, tr("Minimum Time"));
    m_pModelGse->setHeaderData(10, Qt::Horizontal, tr("Maximum Time"));
    m_pViewGse->setModel(m_pModelGse);

    m_pViewGse->setColumnWidth(0, 200);
    m_pViewGse->setColumnWidth(1, 250);
    m_pViewGse->setColumnWidth(2, 80);
    m_pViewGse->setColumnWidth(3, 80);
    m_pViewGse->setColumnWidth(4, 80);
    m_pViewGse->setColumnWidth(5, 150);
    m_pViewGse->setColumnWidth(6, 80);
    m_pViewGse->setColumnWidth(7, 80);
    m_pViewGse->setColumnWidth(8, 80);
    m_pViewGse->setColumnWidth(9, 80);
    m_pViewGse->setColumnWidth(10, 80);

    connect(m_pViewGse->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(m_pViewGse, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewDoubleClicked(const QModelIndex&)));
}

void SubNetworkModeWidget::SetupSmvModelView()
{
    m_pViewSmv = new Utils::ReadOnlyTableView(this);
    m_pViewSmv->setObjectName("Styled_TableView");
    m_pViewSmv->setFrameStyle(QFrame::NoFrame);
    m_pViewSmv->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewSmv->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pViewSmv->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewSmv->setSortingEnabled(true);
    m_pViewSmv->verticalHeader()->hide();
#if QT_VERSION >= 0x050000
    m_pViewSmv->verticalHeader()->setSectionsClickable(false);
    m_pViewSmv->horizontalHeader()->setSectionsClickable(true);
    m_pViewSmv->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
    m_pViewSmv->verticalHeader()->setClickable(false);
    m_pViewSmv->horizontalHeader()->setClickable(true);
    m_pViewSmv->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
    m_pViewSmv->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewSmv->horizontalHeader()->setHighlightSections(false);
    m_pViewSmv->horizontalHeader()->setStretchLastSection(true);
    m_pViewSmv->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewSmv->setShowGrid(false);

    m_pModelSmv = new QStandardItemModel(0, 9, this);
    m_pModelSmv->setHeaderData(0, Qt::Horizontal, tr("Subnetwork"));
    m_pModelSmv->setHeaderData(1, Qt::Horizontal, tr("Device"));
    m_pModelSmv->setHeaderData(2, Qt::Horizontal, tr("AccessPoint"));
    m_pModelSmv->setHeaderData(3, Qt::Horizontal, tr("LDevice"));
    m_pModelSmv->setHeaderData(4, Qt::Horizontal, tr("Control Block"));
    m_pModelSmv->setHeaderData(5, Qt::Horizontal, tr("Mac Address"));
    m_pModelSmv->setHeaderData(6, Qt::Horizontal, tr("VLan Id"));
    m_pModelSmv->setHeaderData(7, Qt::Horizontal, tr("VLan Priority"));
    m_pModelSmv->setHeaderData(8, Qt::Horizontal, tr("Application Id"));
    m_pViewSmv->setModel(m_pModelSmv);

    m_pViewSmv->setColumnWidth(0, 200);
    m_pViewSmv->setColumnWidth(1, 250);
    m_pViewSmv->setColumnWidth(2, 80);
    m_pViewSmv->setColumnWidth(3, 80);
    m_pViewSmv->setColumnWidth(4, 80);
    m_pViewSmv->setColumnWidth(5, 150);
    m_pViewSmv->setColumnWidth(6, 80);
    m_pViewSmv->setColumnWidth(7, 80);
    m_pViewSmv->setColumnWidth(8, 80);

    connect(m_pViewSmv->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(m_pViewSmv, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewDoubleClicked(const QModelIndex&)));
}

void SubNetworkModeWidget::BuildAddrModel(SclParser::SCLElement *pSCLElementCommunication)
{
    m_pModelAddr->removeRows(0, m_pModelAddr->rowCount());

    if(!pSCLElementCommunication)
        return;

    QMap<QString, QString> mapNameToNameDesc;
    foreach(ProjectExplorer::PeDevice *pDevice, m_pProjectVersion->GetAllDevices())
    {
        QString strName = pDevice->GetName();
        if(!strName.isEmpty())
            mapNameToNameDesc.insert(strName, pDevice->GetDisplayName());
    }

    foreach(SclParser::SCLElement *pSCLElementSubNetwork, pSCLElementCommunication->FindChildByType("SubNetwork"))
    {
        foreach(SclParser::SCLElement *pSCLElementConnectedAP, pSCLElementSubNetwork->FindChildByType("ConnectedAP"))
        {
            if(SclParser::SCLElement *pSCLElementAddress = pSCLElementConnectedAP->GetFirstChild("Address"))
            {
                const QString strSubNetworkName = pSCLElementSubNetwork->GetAttribute("name");
                const QString strSubNetworkDesc = pSCLElementSubNetwork->GetAttribute("desc");
                const QString strSubNetwork = strSubNetworkDesc.isEmpty() ? strSubNetworkName : QString("%1:%2").arg(strSubNetworkName).arg(strSubNetworkDesc);
                const QString strIEDName = pSCLElementConnectedAP->GetAttribute("iedName");
                const QString strDevice = mapNameToNameDesc.value(strIEDName, strIEDName);
                const QString strAcceessPoint = pSCLElementConnectedAP->GetAttribute("apName");

                QString strIP, strSubnet;
                foreach(SclParser::SCLElement *pSCLElementP, pSCLElementAddress->FindChildByType("P"))
                {
                    const QString strPType = pSCLElementP->GetAttribute("type");
                    if(strPType == QLatin1String("IP"))
                        strIP = pSCLElementP->GetValue();
                    else if(strPType == QLatin1String("IP-SUBNET"))
                        strSubnet = pSCLElementP->GetValue();
                }

                QList<QStandardItem*> lstItems;

                QStandardItem *pItem = new QStandardItem(strSubNetwork);
                pItem->setData(strSubNetworkName);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strDevice);
                pItem->setData(strIEDName);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strAcceessPoint);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strIP);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strSubnet);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                m_pModelAddr->appendRow(lstItems);
            }
        }
    }
}

void SubNetworkModeWidget::BuildGseModel(SclParser::SCLElement *pSCLElementCommunication)
{
    m_pModelGse->removeRows(0, m_pModelGse->rowCount());

    if(!pSCLElementCommunication)
        return;

    QMap<QString, QString> mapNameToNameDesc;
    foreach(ProjectExplorer::PeDevice *pDevice, m_pProjectVersion->GetAllDevices())
    {
        QString strName = pDevice->GetName();
        if(!strName.isEmpty())
            mapNameToNameDesc.insert(strName, pDevice->GetDisplayName());
    }

    foreach(SclParser::SCLElement *pSCLElementSubNetwork, pSCLElementCommunication->FindChildByType("SubNetwork"))
    {
        foreach(SclParser::SCLElement *pSCLElementConnectedAP, pSCLElementSubNetwork->FindChildByType("ConnectedAP"))
        {
            foreach(SclParser::SCLElement *pSCLElementGSE, pSCLElementConnectedAP->FindChildByType("GSE"))
            {
                const QString strSubNetworkName = pSCLElementSubNetwork->GetAttribute("name");
                const QString strSubNetworkDesc = pSCLElementSubNetwork->GetAttribute("desc");
                const QString strSubNetwork = strSubNetworkDesc.isEmpty() ? strSubNetworkName : QString("%1:%2").arg(strSubNetworkName).arg(strSubNetworkDesc);
                const QString strIEDName = pSCLElementConnectedAP->GetAttribute("iedName");
                const QString strDevice = mapNameToNameDesc.value(strIEDName, strIEDName);
                const QString strAcceessPoint = pSCLElementConnectedAP->GetAttribute("apName");
                const QString strLDevice = pSCLElementGSE->GetAttribute("ldInst");
                const QString strControlBlock = pSCLElementGSE->GetAttribute("cbName");

                QString strMac, strVLanId, strVLanPriority, strAppId;
                if(SclParser::SCLElement *pSCLElementAddress = pSCLElementGSE->GetFirstChild("Address"))
                {
                    foreach(SclParser::SCLElement *pSCLElementP, pSCLElementAddress->FindChildByType("P"))
                    {
                        const QString strPType = pSCLElementP->GetAttribute("type");
                        if(strPType == QLatin1String("MAC-Address"))
                            strMac = pSCLElementP->GetValue();
                        else if(strPType == QLatin1String("VLAN-ID"))
                            strVLanId = pSCLElementP->GetValue();
                        else if(strPType == QLatin1String("VLAN-PRIORITY"))
                            strVLanPriority = pSCLElementP->GetValue();
                        else if(strPType == QLatin1String("APPID"))
                            strAppId = pSCLElementP->GetValue();
                    }
                }

                QString strMinTime;
                if(SclParser::SCLElement *pSCLElementMinTime = pSCLElementGSE->GetFirstChild("MinTime"))
                    strMinTime = pSCLElementMinTime->GetValue();

                QString strMaxTime;
                if(SclParser::SCLElement *pSCLElementMaxTime = pSCLElementGSE->GetFirstChild("MaxTime"))
                    strMaxTime = pSCLElementMaxTime->GetValue();

                QList<QStandardItem*> lstItems;

                QStandardItem *pItem = new QStandardItem(strSubNetwork);
                pItem->setData(strSubNetworkName);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strDevice);
                pItem->setData(strIEDName);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strAcceessPoint);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strLDevice);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strControlBlock);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strMac);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strVLanId);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strVLanPriority);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strAppId);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strMinTime);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strMaxTime);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                m_pModelGse->appendRow(lstItems);
            }
        }
    }
}

void SubNetworkModeWidget::BuildSmvModel(SclParser::SCLElement *pSCLElementCommunication)
{
    m_pModelSmv->removeRows(0, m_pModelSmv->rowCount());

    if(!pSCLElementCommunication)
        return;

    QMap<QString, QString> mapNameToNameDesc;
    foreach(ProjectExplorer::PeDevice *pDevice, m_pProjectVersion->GetAllDevices())
    {
        QString strName = pDevice->GetName();
        if(!strName.isEmpty())
            mapNameToNameDesc.insert(strName, pDevice->GetDisplayName());
    }

    foreach(SclParser::SCLElement *pSCLElementSubNetwork, pSCLElementCommunication->FindChildByType("SubNetwork"))
    {
        foreach(SclParser::SCLElement *pSCLElementConnectedAP, pSCLElementSubNetwork->FindChildByType("ConnectedAP"))
        {
            foreach(SclParser::SCLElement *pSCLElementSMV, pSCLElementConnectedAP->FindChildByType("SMV"))
            {
                const QString strSubNetworkName = pSCLElementSubNetwork->GetAttribute("name");
                const QString strSubNetworkDesc = pSCLElementSubNetwork->GetAttribute("desc");
                const QString strSubNetwork = strSubNetworkDesc.isEmpty() ? strSubNetworkName : QString("%1:%2").arg(strSubNetworkName).arg(strSubNetworkDesc);
                const QString strIEDName = pSCLElementConnectedAP->GetAttribute("iedName");
                const QString strDevice = mapNameToNameDesc.value(strIEDName, strIEDName);
                const QString strAcceessPoint = pSCLElementConnectedAP->GetAttribute("apName");
                const QString strLDevice = pSCLElementSMV->GetAttribute("ldInst");
                const QString strControlBlock = pSCLElementSMV->GetAttribute("cbName");

                QString strMac, strVLanId, strVLanPriority, strAppId;
                if(SclParser::SCLElement *pSCLElementAddress = pSCLElementSMV->GetFirstChild("Address"))
                {
                    foreach(SclParser::SCLElement *pSCLElementP, pSCLElementAddress->FindChildByType("P"))
                    {
                        const QString strPType = pSCLElementP->GetAttribute("type");
                        if(strPType == QLatin1String("MAC-Address"))
                            strMac = pSCLElementP->GetValue();
                        else if(strPType == QLatin1String("VLAN-ID"))
                            strVLanId = pSCLElementP->GetValue();
                        else if(strPType == QLatin1String("VLAN-PRIORITY"))
                            strVLanPriority = pSCLElementP->GetValue();
                        else if(strPType == QLatin1String("APPID"))
                            strAppId = pSCLElementP->GetValue();
                    }
                }

                QList<QStandardItem*> lstItems;

                QStandardItem *pItem = new QStandardItem(strSubNetwork);
                pItem->setData(strSubNetworkName);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strDevice);
                pItem->setData(strIEDName);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strAcceessPoint);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strLDevice);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strControlBlock);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strMac);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strVLanId);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strVLanPriority);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                pItem = new QStandardItem(strAppId);
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstItems.append(pItem);

                m_pModelSmv->appendRow(lstItems);
            }
        }
    }
}

SclParser::SCLElement* SubNetworkModeWidget::FindElementAddr(SclParser::SCLElement *pSCLElementCommunication, const QString &strSubnetwork, const QString &strIED, const QString &strAccessPoint)
{
    if(!pSCLElementCommunication)
        return 0;

    foreach(SclParser::SCLElement *pSCLElementSubNetwork, pSCLElementCommunication->FindChildByType("SubNetwork"))
    {
        if(pSCLElementSubNetwork->GetAttribute("name") != strSubnetwork)
            continue;

        foreach(SclParser::SCLElement *pSCLElementConnectedAP, pSCLElementSubNetwork->FindChildByType("ConnectedAP"))
        {
            if(pSCLElementConnectedAP->GetAttribute("iedName") != strIED || pSCLElementConnectedAP->GetAttribute("apName") != strAccessPoint)
                continue;

            return pSCLElementConnectedAP->GetFirstChild("Address");
        }
    }

    return 0;
}

SclParser::SCLElement* SubNetworkModeWidget::FindElementGse(SclParser::SCLElement *pSCLElementCommunication, const QString &strSubnetwork, const QString &strIED, const QString &strAccessPoint, const QString &strLDevice, const QString &strControlBlock)
{
    if(!pSCLElementCommunication)
        return 0;

    foreach(SclParser::SCLElement *pSCLElementSubNetwork, pSCLElementCommunication->FindChildByType("SubNetwork"))
    {
        if(pSCLElementSubNetwork->GetAttribute("name") != strSubnetwork)
            continue;

        foreach(SclParser::SCLElement *pSCLElementConnectedAP, pSCLElementSubNetwork->FindChildByType("ConnectedAP"))
        {
            if(pSCLElementConnectedAP->GetAttribute("iedName") != strIED || pSCLElementConnectedAP->GetAttribute("apName") != strAccessPoint)
                continue;

            foreach(SclParser::SCLElement *pSCLElementGSE, pSCLElementConnectedAP->FindChildByType("GSE"))
            {
                if(pSCLElementGSE->GetAttribute("ldInst") == strLDevice && pSCLElementGSE->GetAttribute("cbName") == strControlBlock)
                    return pSCLElementGSE;
            }
        }
    }

    return 0;
}

SclParser::SCLElement* SubNetworkModeWidget::FindElementSmv(SclParser::SCLElement *pSCLElementCommunication, const QString &strSubnetwork, const QString &strIED, const QString &strAccessPoint, const QString &strLDevice, const QString &strControlBlock)
{
    if(!pSCLElementCommunication)
        return 0;

    foreach(SclParser::SCLElement *pSCLElementSubNetwork, pSCLElementCommunication->FindChildByType("SubNetwork"))
    {
        if(pSCLElementSubNetwork->GetAttribute("name") != strSubnetwork)
            continue;

        foreach(SclParser::SCLElement *pSCLElementConnectedAP, pSCLElementSubNetwork->FindChildByType("ConnectedAP"))
        {
            if(pSCLElementConnectedAP->GetAttribute("iedName") != strIED || pSCLElementConnectedAP->GetAttribute("apName") != strAccessPoint)
                continue;

            foreach(SclParser::SCLElement *pSCLElementSMV, pSCLElementConnectedAP->FindChildByType("SMV"))
            {
                if(pSCLElementSMV->GetAttribute("ldInst") == strLDevice && pSCLElementSMV->GetAttribute("cbName") == strControlBlock)
                    return pSCLElementSMV;
            }
        }
    }

    return 0;
}

void SubNetworkModeWidget::UpdateActions()
{
    m_pActionRefresh->setEnabled(m_pProjectVersion);
    m_pActionBatchEdit->setEnabled(false);
    m_pActionEdit->setEnabled(false);

    Utils::ReadOnlyTableView *pView = 0;
    const int iCurrentTab = m_pTabBar->currentIndex();
    if(iCurrentTab == 0) // Addr
        pView = m_pViewAddr;
    else if(iCurrentTab == 1) // GSE
        pView = m_pViewGse;
    else if(iCurrentTab == 2) // SMV
        pView = m_pViewSmv;

    if(!pView)
        return;

    int iSelectedRowSize = pView->selectionModel()->selectedRows().size();
    m_pActionBatchEdit->setEnabled(iSelectedRowSize > 0);
    m_pActionEdit->setEnabled(iSelectedRowSize == 1);
}

void SubNetworkModeWidget::SlotCurrentTabChanged(int iCurrentIndex)
{
    Q_UNUSED(iCurrentIndex)

    UpdateActions();
}

void SubNetworkModeWidget::SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    UpdateActions();
}

void SubNetworkModeWidget::SlotViewDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    SlotActionEdit();
}

void SubNetworkModeWidget::SlotActionRefresh()
{
    Utils::WaitCursor waitcursor;
    Q_UNUSED(waitcursor)

    if(!m_pProjectVersion)
        return;

    QByteArray baCommunicationContent;
    if(!m_pProjectVersion->DbReadSclCommunication(baCommunicationContent))
        return;

    SclParser::SCLElement SCLElementCommunication("Communication", 0, 0);
    if(!baCommunicationContent.isNull() && !baCommunicationContent.isEmpty())
        SCLElementCommunication.ReadFromBinary(baCommunicationContent);

    SclParser::SCLElement *pSCLElementSubnetworkStationbus = SCLElementCommunication.FindChild("SubNetwork", "name", "Subnetwork_Stationbus", false);
    if(!pSCLElementSubnetworkStationbus)
    {
        QMap<QString, QString> mapAttValue;
        mapAttValue.insert("name", "Subnetwork_Stationbus");
        pSCLElementSubnetworkStationbus = SCLElementCommunication.CreateChild("SubNetwork", mapAttValue, -1, false);
    }

    SclParser::SCLElement *pSCLElementSubnetworkProcessbus = SCLElementCommunication.FindChild("SubNetwork", "name", "Subnetwork_Processbus", false);
    if(!pSCLElementSubnetworkProcessbus)
    {
        QMap<QString, QString> mapAttValue;
        mapAttValue.insert("name", "Subnetwork_Processbus");
        pSCLElementSubnetworkProcessbus = SCLElementCommunication.CreateChild("SubNetwork", mapAttValue, -1, false);
    }

    foreach(ProjectExplorer::PeDevice *pDevice, m_pProjectVersion->GetAllDevices())
    {
        QByteArray baDeviceContent;
        if(!m_pProjectVersion->DbReadDeviceModel(pDevice->GetId(), baDeviceContent))
            continue;

        if(baDeviceContent.isEmpty() || baDeviceContent.isNull())
            continue;

        SclParser::SCLElement SCLElementIED("IED", 0, 0);
        SCLElementIED.ReadFromBinary(baDeviceContent);

        foreach(SclParser::SCLElement *pSCLElementAccessPoint, SCLElementIED.FindChildByType("AccessPoint"))
        {
            const QString strAccessPointName = pSCLElementAccessPoint->GetAttribute("name");
            const bool bStation = strAccessPointName.startsWith(QLatin1String("S"), Qt::CaseInsensitive);

            QMap<QString, QString> mapAttValue;
            mapAttValue.insert("iedName", pDevice->GetName());
            mapAttValue.insert("apName", strAccessPointName);
            SclParser::SCLElement *pSCLElementConnectedAP = SCLElementCommunication.FindChild("ConnectedAP", mapAttValue, true);
            if(!pSCLElementConnectedAP)
            {
                if(bStation)
                    pSCLElementConnectedAP = pSCLElementSubnetworkStationbus->CreateChild("ConnectedAP", mapAttValue, -1, false);
                else
                    pSCLElementConnectedAP = pSCLElementSubnetworkProcessbus->CreateChild("ConnectedAP", mapAttValue, -1, false);
            }

            if(bStation)
            {
                SclParser::SCLElement *pSCLElementAddress = pSCLElementConnectedAP->GetFirstChild("Address");
                if(!pSCLElementAddress)
                {
                    if(pSCLElementAddress = pSCLElementConnectedAP->CreateChild("Address", -1, false))
                    {
                        SclParser::SCLElement *pSCLElementPIP = pSCLElementAddress->CreateChild("P", -1, false);
                        pSCLElementPIP->SetAttribute("type", "IP");
                        pSCLElementPIP->SetValue("0.0.0.0");

                        SclParser::SCLElement *pSCLElementPSUBNET = pSCLElementAddress->CreateChild("P", -1, false);
                        pSCLElementPSUBNET->SetAttribute("type", "IP-SUBNET");
                        pSCLElementPSUBNET->SetValue("255.255.0.0");
                    }
                }
            }

            SclParser::SCLElement *pSCLElementServer = pSCLElementAccessPoint->GetFirstChild("Server");
            if(!pSCLElementServer)
                continue;

            foreach(SclParser::SCLElement *pSCLElementLDevice, pSCLElementServer->FindChildByType("LDevice"))
            {
                const QString strLDInst = pSCLElementLDevice->GetAttribute("inst");

                SclParser::SCLElement *pSCLElementLN0 = pSCLElementLDevice->GetFirstChild("LN0");
                if(!pSCLElementLN0)
                    continue;

                foreach(SclParser::SCLElement *pSCLElementGSEControl, pSCLElementLN0->FindChildByType("GSEControl"))
                {
                    const QString strCBName = pSCLElementGSEControl->GetAttribute("name");

                    QMap<QString, QString> mapAttValue;
                    mapAttValue.insert("ldInst", strLDInst);
                    mapAttValue.insert("cbName", strCBName);

                    SclParser::SCLElement *pSCLElementGSE = pSCLElementConnectedAP->FindChild("GSE", mapAttValue, false);
                    if(!pSCLElementGSE)
                    {
                        if(pSCLElementGSE = pSCLElementConnectedAP->CreateChild("GSE", mapAttValue, -1, false))
                        {
                            if(SclParser::SCLElement *pSCLElementAddress = pSCLElementGSE->CreateChild("Address", -1, false))
                            {
                                SclParser::SCLElement *pSCLElementPMAC = pSCLElementAddress->CreateChild("P", -1, false);
                                pSCLElementPMAC->SetAttribute("type", "MAC-Address");
                                pSCLElementPMAC->SetValue("01-0C-CD-01-");

                                SclParser::SCLElement *pSCLElementPVLANID = pSCLElementAddress->CreateChild("P", -1, false);
                                pSCLElementPVLANID->SetAttribute("type", "VLAN-ID");
                                pSCLElementPVLANID->SetValue("000");

                                SclParser::SCLElement *pSCLElementPVLANPRIORITY = pSCLElementAddress->CreateChild("P", -1, false);
                                pSCLElementPVLANPRIORITY->SetAttribute("type", "VLAN-PRIORITY");
                                pSCLElementPVLANPRIORITY->SetValue("4");

                                SclParser::SCLElement *pSCLElementPAPPID = pSCLElementAddress->CreateChild("P", -1, false);
                                pSCLElementPAPPID->SetAttribute("type", "APPID");
                                pSCLElementPAPPID->SetValue("");
                            }

                            QMap<QString, QString> mapAttr;
                            mapAttr.insert("multiplier", "m");
                            mapAttr.insert("unit", "s");

                            if(SclParser::SCLElement *pSCLElementMinTime = pSCLElementGSE->CreateChild("MinTime", mapAttr, -1, false))
                                pSCLElementMinTime->SetValue("2");

                            if(SclParser::SCLElement *pSCLElementMaxTime = pSCLElementGSE->CreateChild("MaxTime", mapAttr, -1, false))
                                pSCLElementMaxTime->SetValue("5000");
                        }
                    }
                }

                foreach(SclParser::SCLElement *pSCLElementSampledValueControl, pSCLElementLN0->FindChildByType("SampledValueControl"))
                {
                    const QString strCBName = pSCLElementSampledValueControl->GetAttribute("name");

                    QMap<QString, QString> mapAttValue;
                    mapAttValue.insert("ldInst", strLDInst);
                    mapAttValue.insert("cbName", strCBName);

                    SclParser::SCLElement *pSCLElementSMV = pSCLElementConnectedAP->FindChild("SMV", mapAttValue, false);
                    if(!pSCLElementSMV)
                    {
                        if(pSCLElementSMV = pSCLElementConnectedAP->CreateChild("SMV", mapAttValue, -1, false))
                        {
                            if(SclParser::SCLElement *pSCLElementAddress = pSCLElementSMV->CreateChild("Address", -1, false))
                            {
                                SclParser::SCLElement *pSCLElementPMAC = pSCLElementAddress->CreateChild("P", -1, false);
                                pSCLElementPMAC->SetAttribute("type", "MAC-Address");
                                pSCLElementPMAC->SetValue("01-0C-CD-04-");

                                SclParser::SCLElement *pSCLElementPVLANID = pSCLElementAddress->CreateChild("P", -1, false);
                                pSCLElementPVLANID->SetAttribute("type", "VLAN-ID");
                                pSCLElementPVLANID->SetValue("000");

                                SclParser::SCLElement *pSCLElementPVLANPRIORITY = pSCLElementAddress->CreateChild("P", -1, false);
                                pSCLElementPVLANPRIORITY->SetAttribute("type", "VLAN-PRIORITY");
                                pSCLElementPVLANPRIORITY->SetValue("4");

                                SclParser::SCLElement *pSCLElementPAPPID = pSCLElementAddress->CreateChild("P", -1, false);
                                pSCLElementPAPPID->SetAttribute("type", "APPID");
                                pSCLElementPAPPID->SetValue("");
                            }
                        }
                    }
                }
            }
        }
    }

    foreach(SclParser::SCLElement *pSCLElementSubNetwork, SCLElementCommunication.FindChildByType("SubNetwork"))
    {
        foreach(SclParser::SCLElement *pSCLElementConnectedAP, pSCLElementSubNetwork->FindChildByType("ConnectedAP"))
        {
            if(pSCLElementConnectedAP->GetChildList().isEmpty())
                pSCLElementSubNetwork->RemoveChild(pSCLElementConnectedAP);
        }

        if(pSCLElementSubNetwork->GetChildList().isEmpty())
            SCLElementCommunication.RemoveChild(pSCLElementSubNetwork);
    }

    baCommunicationContent.clear();
    if(!SCLElementCommunication.GetChildList().isEmpty())
        baCommunicationContent = SCLElementCommunication.WriteToBinary();

    if(m_pProjectVersion->DbUpdateSclCommunication(baCommunicationContent, true))
    {
        ProjectExplorer::PeProjectVersion *pProjectVersion = m_pProjectVersion;
        SetProjectObject(0);
        SetProjectObject(pProjectVersion);
    }
}

void SubNetworkModeWidget::SlotActionBatchEdit()
{
    const int iCurrentTab = m_pTabBar->currentIndex();
    if(iCurrentTab == 0) // Addr
    {
        QItemSelectionModel *pSelectionModel = m_pViewAddr->selectionModel();
        QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
        if(lstSelectedIndex.isEmpty())
            return;
        qSort(lstSelectedIndex);

        QStandardItem *pItem = m_pModelAddr->itemFromIndex(lstSelectedIndex.first());
        if(!pItem)
            return;

        DialogBatchEditAddress dlg(this);
        dlg.SetIpAddress("0.0.0.0");
        dlg.SetIpSubnet("255.255.0.0");

        if(dlg.exec() != QDialog::Accepted)
            return;

        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        int iIpAddressLast = -1;
        const QStringList lstIpAddressSections = dlg.GetIpAddress().split(QLatin1String("."));
        if(lstIpAddressSections.size() == 4)
            iIpAddressLast = lstIpAddressSections.last().toInt();
        const QString strIpSubnet = dlg.GetIpSubnet();

        QByteArray baCommunicationContent;
        if(m_pProjectVersion->DbReadSclCommunication(baCommunicationContent) && !baCommunicationContent.isNull() && !baCommunicationContent.isEmpty())
        {
            SclParser::SCLElement ElementCommunication("Communication", 0, 0);
            ElementCommunication.ReadFromBinary(baCommunicationContent);

            foreach(const QModelIndex &index, lstSelectedIndex)
            {
                if(QStandardItem *pItem = m_pModelAddr->itemFromIndex(index))
                {
                    const QString strSubnetwork = m_pModelAddr->item(pItem->row(), 0)->text();
                    const QString strIED = m_pModelAddr->item(pItem->row(), 1)->data().toString();
                    const QString strAccessPoint = m_pModelAddr->item(pItem->row(), 2)->text();

                    if(SclParser::SCLElement *pSCLElementAddress = FindElementAddr(&ElementCommunication, strSubnetwork, strIED, strAccessPoint))
                    {
                        SclParser::SCLElement *pSCLElementPIP = 0, *pSCLElementPSUBNET = 0;
                        foreach(SclParser::SCLElement *pSCLElementP, pSCLElementAddress->FindChildByType("P"))
                        {
                            const QString strPType = pSCLElementP->GetAttribute("type");
                            if(strPType == QLatin1String("IP"))
                                pSCLElementPIP = pSCLElementP;
                            else if(strPType == QLatin1String("IP-SUBNET"))
                                pSCLElementPSUBNET = pSCLElementP;
                        }

                        if(iIpAddressLast != -1)
                        {
                            const QString strIpAddress = QString("%1.%2.%3.%4").arg(lstIpAddressSections.at(0))
                                                                               .arg(lstIpAddressSections.at(1))
                                                                               .arg(lstIpAddressSections.at(2))
                                                                               .arg(QString::number(iIpAddressLast++, 10));

                            if(!pSCLElementPIP)
                            {
                                pSCLElementPIP = pSCLElementAddress->CreateChild("P", false);
                                pSCLElementPIP->SetAttribute("type", "IP");
                            }
                            pSCLElementPIP->SetValue(strIpAddress);
                        }

                        if(!strIpSubnet.isEmpty())
                        {
                            if(!pSCLElementPSUBNET)
                            {
                                pSCLElementPSUBNET = pSCLElementAddress->CreateChild("P", false);
                                pSCLElementPSUBNET->SetAttribute("type", "IP-SUBNET");
                            }
                            pSCLElementPSUBNET->SetValue(strIpSubnet);
                        }
                    }
                }
            }

            if(m_pProjectVersion->DbUpdateSclCommunication(ElementCommunication.WriteToBinary(), true))
            {
                ProjectExplorer::PeProjectVersion *pProjectVersion = m_pProjectVersion;
                SetProjectObject(0);
                SetProjectObject(pProjectVersion);
            }
        }
    }
    else if(iCurrentTab == 1) // GSE
    {
        QItemSelectionModel *pSelectionModel = m_pViewGse->selectionModel();
        QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
        if(lstSelectedIndex.isEmpty())
            return;
        qSort(lstSelectedIndex);

        QStandardItem *pItem = m_pModelGse->itemFromIndex(lstSelectedIndex.first());
        if(!pItem)
            return;

        DialogBatchEditGSE dlg(this);
        dlg.SetMacAddress("01-0C-CD-01-");
        dlg.SetVLanId("000");
        dlg.SetVLanPriority("4");
        dlg.SetMinTime("2");
        dlg.SetMaxTime("5000");

        if(dlg.exec() != QDialog::Accepted)
            return;

        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        int iMacAddressLast = -1, iMacAddressPrev;
        const QStringList lstMacAddressSections = dlg.GetMacAddress().split(QLatin1String("-"));
        if(lstMacAddressSections.size() == 6)
        {
            iMacAddressLast = lstMacAddressSections.last().toInt(0, 16);
            iMacAddressPrev = lstMacAddressSections.at(4).toInt(0, 16);
        }

        const QString strVLanId = dlg.GetVLanId();
        const QString strVLanPriority = dlg.GetVLanPriority();

        const QString strAppId = dlg.GetAppId();
        int iAppId = -1;
        if(!strAppId.isEmpty())
            iAppId = strAppId.toInt(0, 16);

        const QString strMinTime = dlg.GetMinTime();
        const QString strMaxTime = dlg.GetMaxTime();

        QByteArray baCommunicationContent;
        if(m_pProjectVersion->DbReadSclCommunication(baCommunicationContent) && !baCommunicationContent.isNull() && !baCommunicationContent.isEmpty())
        {
            SclParser::SCLElement ElementCommunication("Communication", 0, 0);
            ElementCommunication.ReadFromBinary(baCommunicationContent);

            foreach(const QModelIndex &index, lstSelectedIndex)
            {
                if(QStandardItem *pItem = m_pModelGse->itemFromIndex(index))
                {
                    const QString strSubnetwork = m_pModelGse->item(pItem->row(), 0)->text();
                    const QString strIED = m_pModelGse->item(pItem->row(), 1)->data().toString();
                    const QString strAccessPoint = m_pModelGse->item(pItem->row(), 2)->text();
                    const QString strLDevice = m_pModelGse->item(pItem->row(), 3)->text();
                    const QString strControlBlock = m_pModelGse->item(pItem->row(), 4)->text();

                    if(SclParser::SCLElement *pSCLElementGSE = FindElementGse(&ElementCommunication, strSubnetwork, strIED, strAccessPoint, strLDevice, strControlBlock))
                    {
                        SclParser::SCLElement *pSCLElementAddress = pSCLElementGSE->GetFirstChild("Address");
                        if(!pSCLElementAddress)
                            pSCLElementAddress = pSCLElementGSE->CreateChild("Address", false);

                        SclParser::SCLElement *pSCLElementPMAC = 0, *pSCLElementPVLANID = 0, *pSCLElementPVLANPRIORITY = 0, *pSCLElementPAPPID = 0;
                        foreach(SclParser::SCLElement *pSCLElementP, pSCLElementAddress->FindChildByType("P"))
                        {
                            const QString strPType = pSCLElementP->GetAttribute("type");
                            if(strPType == QLatin1String("MAC-Address"))
                                pSCLElementPMAC = pSCLElementP;
                            else if(strPType == QLatin1String("VLAN-ID"))
                                pSCLElementPVLANID = pSCLElementP;
                            else if(strPType == QLatin1String("VLAN-PRIORITY"))
                                pSCLElementPVLANPRIORITY = pSCLElementP;
                            else if(strPType == QLatin1String("APPID"))
                                pSCLElementPAPPID = pSCLElementP;
                        }

                        if(iMacAddressLast != -1)
                        {
                            const QString strMacAddressLast = QString("%1").arg(iMacAddressLast, 2, 16, QLatin1Char('0'));
                            const QString strMacAddressPrev = QString("%1").arg(iMacAddressPrev, 2, 16, QLatin1Char('0'));
                            const QString strMacAddress = QString("%1-%2-%3-%4-%5-%6").arg(lstMacAddressSections.at(0))
                                                                                      .arg(lstMacAddressSections.at(1))
                                                                                      .arg(lstMacAddressSections.at(2))
                                                                                      .arg(lstMacAddressSections.at(3))
                                                                                      .arg(strMacAddressPrev.toUpper())
                                                                                      .arg(strMacAddressLast.toUpper());
                            if(++iMacAddressLast > 0xFF)
                            {
                                iMacAddressLast = 0;
                                iMacAddressPrev++;
                            }

                            if(!pSCLElementPMAC)
                            {
                                pSCLElementPMAC = pSCLElementAddress->CreateChild("P", false);
                                pSCLElementPMAC->SetAttribute("type", "MAC-Address");
                            }
                            pSCLElementPMAC->SetValue(strMacAddress);
                        }

                        if(!strVLanId.isEmpty())
                        {
                            if(!pSCLElementPVLANID)
                            {
                                pSCLElementPVLANID = pSCLElementAddress->CreateChild("P", false);
                                pSCLElementPVLANID->SetAttribute("type", "VLAN-ID");
                            }
                            pSCLElementPVLANID->SetValue(strVLanId);
                        }

                        if(!strVLanPriority.isEmpty())
                        {
                            if(!pSCLElementPVLANPRIORITY)
                            {
                                pSCLElementPVLANPRIORITY = pSCLElementAddress->CreateChild("P", false);
                                pSCLElementPVLANPRIORITY->SetAttribute("type", "VLAN-PRIORITY");
                            }
                            pSCLElementPVLANPRIORITY->SetValue(strVLanPriority);
                        }

                        if(iAppId != -1)
                        {
                            QString strAppIdValue = QString::number(iAppId++, 16);
                            if(strAppIdValue.length() < strAppId.length())
                                strAppIdValue.prepend(QByteArray(strAppId.length() - strAppIdValue.length(), '0'));
                            if(strAppIdValue.length() > strAppId.length())
                                strAppIdValue.left(strAppId.length());

                            if(!pSCLElementPAPPID)
                            {
                                pSCLElementPAPPID = pSCLElementAddress->CreateChild("P", false);
                                pSCLElementPAPPID->SetAttribute("type", "APPID");
                            }
                            pSCLElementPAPPID->SetValue(strAppIdValue.toUpper());
                        }

                        if(!strMinTime.isEmpty())
                        {
                            SclParser::SCLElement *pSCLElementMinTime = pSCLElementGSE->GetFirstChild("MinTime");
                            if(!pSCLElementMinTime)
                            {
                                QMap<QString, QString> mapAttr;
                                mapAttr.insert("multiplier", "m");
                                mapAttr.insert("unit", "s");
                                pSCLElementMinTime = pSCLElementGSE->CreateChild("MinTime", mapAttr, -1, false);
                            }
                            pSCLElementMinTime->SetValue(strMinTime);
                        }

                        if(!strMaxTime.isEmpty())
                        {
                            SclParser::SCLElement *pSCLElementMaxTime = pSCLElementGSE->GetFirstChild("MaxTime");
                            if(!pSCLElementMaxTime)
                            {
                                QMap<QString, QString> mapAttr;
                                mapAttr.insert("multiplier", "m");
                                mapAttr.insert("unit", "s");
                                pSCLElementMaxTime = pSCLElementGSE->CreateChild("MaxTime", mapAttr, -1, false);
                            }
                            pSCLElementMaxTime->SetValue(strMaxTime);
                        }
                    }
                }                
            }

            if(m_pProjectVersion->DbUpdateSclCommunication(ElementCommunication.WriteToBinary(), true))
            {
                ProjectExplorer::PeProjectVersion *pProjectVersion = m_pProjectVersion;
                SetProjectObject(0);
                SetProjectObject(pProjectVersion);
            }
        }
    }
    else if(iCurrentTab == 2) // SMV
    {
        QItemSelectionModel *pSelectionModel = m_pViewSmv->selectionModel();
        QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
        if(lstSelectedIndex.isEmpty())
            return;
        qSort(lstSelectedIndex);

        QStandardItem *pItem = m_pModelSmv->itemFromIndex(lstSelectedIndex.first());
        if(!pItem)
            return;

        DialogBatchEditSMV dlg(this);
        dlg.SetMacAddress("01-0C-CD-04-");
        dlg.SetVLanId("000");
        dlg.SetVLanPriority("4");

        if(dlg.exec() != QDialog::Accepted)
            return;

        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        int iMacAddressLast = -1, iMacAddressPrev;
        const QStringList lstMacAddressSections = dlg.GetMacAddress().split(QLatin1String("-"));
        if(lstMacAddressSections.size() == 6)
        {
            iMacAddressLast = lstMacAddressSections.last().toInt(0, 16);
            iMacAddressPrev = lstMacAddressSections.at(4).toInt(0, 16);
        }

        const QString strVLanId = dlg.GetVLanId();
        const QString strVLanPriority = dlg.GetVLanPriority();

        const QString strAppId = dlg.GetAppId();
        int iAppId = -1;
        if(!strAppId.isEmpty())
            iAppId = strAppId.toInt(0, 16);

        QByteArray baCommunicationContent;
        if(m_pProjectVersion->DbReadSclCommunication(baCommunicationContent) && !baCommunicationContent.isNull() && !baCommunicationContent.isEmpty())
        {
            SclParser::SCLElement ElementCommunication("Communication", 0, 0);
            ElementCommunication.ReadFromBinary(baCommunicationContent);

            foreach(const QModelIndex &index, lstSelectedIndex)
            {
                if(QStandardItem *pItem = m_pModelSmv->itemFromIndex(index))
                {
                    const QString strSubnetwork = m_pModelSmv->item(pItem->row(), 0)->text();
                    const QString strIED = m_pModelSmv->item(pItem->row(), 1)->data().toString();
                    const QString strAccessPoint = m_pModelSmv->item(pItem->row(), 2)->text();
                    const QString strLDevice = m_pModelSmv->item(pItem->row(), 3)->text();
                    const QString strControlBlock = m_pModelSmv->item(pItem->row(), 4)->text();

                    if(SclParser::SCLElement *pSCLElementSMV = FindElementSmv(&ElementCommunication, strSubnetwork, strIED, strAccessPoint, strLDevice, strControlBlock))
                    {
                        SclParser::SCLElement *pSCLElementAddress = pSCLElementSMV->GetFirstChild("Address");
                        if(!pSCLElementAddress)
                            pSCLElementAddress = pSCLElementSMV->CreateChild("Address", false);

                        SclParser::SCLElement *pSCLElementPMAC = 0, *pSCLElementPVLANID = 0, *pSCLElementPVLANPRIORITY = 0, *pSCLElementPAPPID = 0;
                        foreach(SclParser::SCLElement *pSCLElementP, pSCLElementAddress->FindChildByType("P"))
                        {
                            const QString strPType = pSCLElementP->GetAttribute("type");
                            if(strPType == QLatin1String("MAC-Address"))
                                pSCLElementPMAC = pSCLElementP;
                            else if(strPType == QLatin1String("VLAN-ID"))
                                pSCLElementPVLANID = pSCLElementP;
                            else if(strPType == QLatin1String("VLAN-PRIORITY"))
                                pSCLElementPVLANPRIORITY = pSCLElementP;
                            else if(strPType == QLatin1String("APPID"))
                                pSCLElementPAPPID = pSCLElementP;
                        }

                        if(iMacAddressLast != -1)
                        {
                            const QString strMacAddressLast = QString("%1").arg(iMacAddressLast, 2, 16, QLatin1Char('0'));
                            const QString strMacAddressPrev = QString("%1").arg(iMacAddressPrev, 2, 16, QLatin1Char('0'));
                            const QString strMacAddress = QString("%1-%2-%3-%4-%5-%6").arg(lstMacAddressSections.at(0))
                                                                                      .arg(lstMacAddressSections.at(1))
                                                                                      .arg(lstMacAddressSections.at(2))
                                                                                      .arg(lstMacAddressSections.at(3))
                                                                                      .arg(strMacAddressPrev.toUpper())
                                                                                      .arg(strMacAddressLast.toUpper());
                            if(++iMacAddressLast > 0xFF)
                            {
                                iMacAddressLast = 0;
                                iMacAddressPrev++;
                            }

                            if(!pSCLElementPMAC)
                            {
                                pSCLElementPMAC = pSCLElementAddress->CreateChild("P", false);
                                pSCLElementPMAC->SetAttribute("type", "MAC-Address");
                            }
                            pSCLElementPMAC->SetValue(strMacAddress);
                        }

                        if(!strVLanId.isEmpty())
                        {
                            if(!pSCLElementPVLANID)
                            {
                                pSCLElementPVLANID = pSCLElementAddress->CreateChild("P", false);
                                pSCLElementPVLANID->SetAttribute("type", "VLAN-ID");
                            }
                            pSCLElementPVLANID->SetValue(strVLanId);
                        }

                        if(!strVLanPriority.isEmpty())
                        {
                            if(!pSCLElementPVLANPRIORITY)
                            {
                                pSCLElementPVLANPRIORITY = pSCLElementAddress->CreateChild("P", false);
                                pSCLElementPVLANPRIORITY->SetAttribute("type", "VLAN-PRIORITY");
                            }
                            pSCLElementPVLANPRIORITY->SetValue(strVLanPriority);
                        }

                        if(iAppId != -1)
                        {
                            QString strAppIdValue = QString::number(iAppId++, 16);
                            if(strAppIdValue.length() < strAppId.length())
                                strAppIdValue.prepend(QByteArray(strAppId.length() - strAppIdValue.length(), '0'));
                            if(strAppIdValue.length() > strAppId.length())
                                strAppIdValue.left(strAppId.length());

                            if(!pSCLElementPAPPID)
                            {
                                pSCLElementPAPPID = pSCLElementAddress->CreateChild("P", false);
                                pSCLElementPAPPID->SetAttribute("type", "APPID");
                            }
                            pSCLElementPAPPID->SetValue(strAppIdValue.toUpper());
                        }
                    }
                }
            }

            if(m_pProjectVersion->DbUpdateSclCommunication(ElementCommunication.WriteToBinary(), true))
            {
                ProjectExplorer::PeProjectVersion *pProjectVersion = m_pProjectVersion;
                SetProjectObject(0);
                SetProjectObject(pProjectVersion);
            }
        }
    }
}

void SubNetworkModeWidget::SlotActionEdit()
{
    const int iCurrentTab = m_pTabBar->currentIndex();
    if(iCurrentTab == 0) // Addr
    {
        QItemSelectionModel *pSelectionModel = m_pViewAddr->selectionModel();
        QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
        if(lstSelectedIndex.size() != 1)
            return;

        QStandardItem *pItem = m_pModelAddr->itemFromIndex(lstSelectedIndex.first());
        if(!pItem)
            return;

        PropertyDlgAddress dlg(this);
        const QString strSubnetwork = m_pModelAddr->item(pItem->row(), 0)->text();
        const QString strDevice = m_pModelAddr->item(pItem->row(), 1)->text();
        const QString strIED = m_pModelAddr->item(pItem->row(), 1)->data().toString();
        const QString strAccessPoint = m_pModelAddr->item(pItem->row(), 2)->text();
        const QString strIp = m_pModelAddr->item(pItem->row(), 3)->text();
        const QString strIpSubnet = m_pModelAddr->item(pItem->row(), 4)->text();

        dlg.SetDevice(strDevice);
        dlg.SetAccessPoint(strAccessPoint);
        dlg.SetIp(strIp);
        dlg.SetIpSubnet(strIpSubnet);

        if(dlg.exec() != QDialog::Accepted)
            return;

        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        QByteArray baCommunicationContent;
        if(m_pProjectVersion->DbReadSclCommunication(baCommunicationContent) && !baCommunicationContent.isNull() && !baCommunicationContent.isEmpty())
        {
            SclParser::SCLElement ElementCommunication("Communication", 0, 0);
            ElementCommunication.ReadFromBinary(baCommunicationContent);

            if(SclParser::SCLElement *pSCLElementAddress = FindElementAddr(&ElementCommunication, strSubnetwork, strIED, strAccessPoint))
            {
                SclParser::SCLElement *pSCLElementPIP = 0, *pSCLElementPSUBNET = 0;
                foreach(SclParser::SCLElement *pSCLElementP, pSCLElementAddress->FindChildByType("P"))
                {
                    const QString strPType = pSCLElementP->GetAttribute("type");
                    if(strPType == QLatin1String("IP"))
                        pSCLElementPIP = pSCLElementP;
                    else if(strPType == QLatin1String("IP-SUBNET"))
                        pSCLElementPSUBNET = pSCLElementP;
                }

                if(!pSCLElementPIP)
                {
                    pSCLElementPIP = pSCLElementAddress->CreateChild("P", false);
                    pSCLElementPIP->SetAttribute("type", "IP");
                }
                pSCLElementPIP->SetValue(dlg.GetIp());

                if(!pSCLElementPSUBNET)
                {
                    pSCLElementPSUBNET = pSCLElementAddress->CreateChild("P", false);
                    pSCLElementPSUBNET->SetAttribute("type", "IP-SUBNET");
                }
                pSCLElementPSUBNET->SetValue(dlg.GetIpSubnet());

                if(m_pProjectVersion->DbUpdateSclCommunication(ElementCommunication.WriteToBinary(), true))
                {
                    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pProjectVersion;
                    SetProjectObject(0);
                    SetProjectObject(pProjectVersion);
                }
            }
        }
    }
    else if(iCurrentTab == 1) // GSE
    {
        QItemSelectionModel *pSelectionModel = m_pViewGse->selectionModel();
        QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
        if(lstSelectedIndex.size() != 1)
            return;

        QStandardItem *pItem = m_pModelGse->itemFromIndex(lstSelectedIndex.first());
        if(!pItem)
            return;

        PropertyDlgGse dlg(this);
        const QString strSubnetwork = m_pModelGse->item(pItem->row(), 0)->text();
        const QString strDevice = m_pModelGse->item(pItem->row(), 1)->text();
        const QString strIED = m_pModelGse->item(pItem->row(), 1)->data().toString();
        const QString strAccessPoint = m_pModelGse->item(pItem->row(), 2)->text();
        const QString strLDevice = m_pModelGse->item(pItem->row(), 3)->text();
        const QString strControlBlock = m_pModelGse->item(pItem->row(), 4)->text();
        const QString strMacAddress = m_pModelGse->item(pItem->row(), 5)->text();
        const QString strVLanId = m_pModelGse->item(pItem->row(), 6)->text();
        const QString strVLanPriority = m_pModelGse->item(pItem->row(), 7)->text();
        const QString strAppId = m_pModelGse->item(pItem->row(), 8)->text();
        const QString strMinTime = m_pModelGse->item(pItem->row(), 9)->text();
        const QString strMaxTime = m_pModelGse->item(pItem->row(), 10)->text();

        dlg.SetDevice(strDevice);
        dlg.SetAccessPoint(strAccessPoint);
        dlg.SetLDevice(strLDevice);
        dlg.SetControlBlock(strControlBlock);
        dlg.SetMacAddress(strMacAddress);
        dlg.SetVLanId(strVLanId);
        dlg.SetVLanPriority(strVLanPriority);
        dlg.SetAppId(strAppId);
        dlg.SetMinTime(strMinTime);
        dlg.SetMaxTime(strMaxTime);

        if(dlg.exec() != QDialog::Accepted)
            return;

        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        QByteArray baCommunicationContent;
        if(m_pProjectVersion->DbReadSclCommunication(baCommunicationContent) && !baCommunicationContent.isNull() && !baCommunicationContent.isEmpty())
        {
            SclParser::SCLElement ElementCommunication("Communication", 0, 0);
            ElementCommunication.ReadFromBinary(baCommunicationContent);

            if(SclParser::SCLElement *pSCLElementGSE = FindElementGse(&ElementCommunication, strSubnetwork, strIED, strAccessPoint, strLDevice, strControlBlock))
            {
                SclParser::SCLElement *pSCLElementAddress = pSCLElementGSE->GetFirstChild("Address");
                if(!pSCLElementAddress)
                    pSCLElementAddress = pSCLElementGSE->CreateChild("Address", false);

                SclParser::SCLElement *pSCLElementPMAC = 0, *pSCLElementPVLANID = 0, *pSCLElementPVLANPRIORITY = 0, *pSCLElementPAPPID = 0;
                foreach(SclParser::SCLElement *pSCLElementP, pSCLElementAddress->FindChildByType("P"))
                {
                    const QString strPType = pSCLElementP->GetAttribute("type");
                    if(strPType == QLatin1String("MAC-Address"))
                        pSCLElementPMAC = pSCLElementP;
                    else if(strPType == QLatin1String("VLAN-ID"))
                        pSCLElementPVLANID = pSCLElementP;
                    else if(strPType == QLatin1String("VLAN-PRIORITY"))
                        pSCLElementPVLANPRIORITY = pSCLElementP;
                    else if(strPType == QLatin1String("APPID"))
                        pSCLElementPAPPID = pSCLElementP;
                }

                if(!pSCLElementPMAC)
                {
                    pSCLElementPMAC = pSCLElementAddress->CreateChild("P", false);
                    pSCLElementPMAC->SetAttribute("type", "MAC-Address");
                }
                pSCLElementPMAC->SetValue(dlg.GetMacAddress());

                if(!pSCLElementPVLANID)
                {
                    pSCLElementPVLANID = pSCLElementAddress->CreateChild("P", false);
                    pSCLElementPVLANID->SetAttribute("type", "VLAN-ID");
                }
                pSCLElementPVLANID->SetValue(dlg.GetVLanId());

                if(!pSCLElementPVLANPRIORITY)
                {
                    pSCLElementPVLANPRIORITY = pSCLElementAddress->CreateChild("P", false);
                    pSCLElementPVLANPRIORITY->SetAttribute("type", "VLAN-PRIORITY");
                }
                pSCLElementPVLANPRIORITY->SetValue(dlg.GetVLanPriority());

                if(!pSCLElementPAPPID)
                {
                    pSCLElementPAPPID = pSCLElementAddress->CreateChild("P", false);
                    pSCLElementPAPPID->SetAttribute("type", "APPID");
                }
                pSCLElementPAPPID->SetValue(dlg.GetAppId());

                SclParser::SCLElement *pSCLElementMinTime = pSCLElementGSE->GetFirstChild("MinTime");
                if(!pSCLElementMinTime)
                {
                    QMap<QString, QString> mapAttr;
                    mapAttr.insert("multiplier", "m");
                    mapAttr.insert("unit", "s");
                    pSCLElementMinTime = pSCLElementGSE->CreateChild("MinTime", mapAttr, -1, false);
                }
                pSCLElementMinTime->SetValue(dlg.GetMinTime());

                SclParser::SCLElement *pSCLElementMaxTime = pSCLElementGSE->GetFirstChild("MaxTime");
                if(!pSCLElementMaxTime)
                {
                    QMap<QString, QString> mapAttr;
                    mapAttr.insert("multiplier", "m");
                    mapAttr.insert("unit", "s");
                    pSCLElementMaxTime = pSCLElementGSE->CreateChild("MaxTime", mapAttr, -1, false);
                }
                pSCLElementMaxTime->SetValue(dlg.GetMaxTime());

                if(m_pProjectVersion->DbUpdateSclCommunication(ElementCommunication.WriteToBinary(), true))
                {
                    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pProjectVersion;
                    SetProjectObject(0);
                    SetProjectObject(pProjectVersion);
                }
            }
        }
    }
    else if(iCurrentTab == 2) // SMV
    {
        QItemSelectionModel *pSelectionModel = m_pViewSmv->selectionModel();
        QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
        if(lstSelectedIndex.size() != 1)
            return;

        QStandardItem *pItem = m_pModelSmv->itemFromIndex(lstSelectedIndex.first());
        if(!pItem)
            return;

        PropertyDlgSmv dlg(this);
        const QString strSubnetwork = m_pModelSmv->item(pItem->row(), 0)->text();
        const QString strDevice = m_pModelSmv->item(pItem->row(), 1)->text();
        const QString strIED = m_pModelSmv->item(pItem->row(), 1)->data().toString();
        const QString strAccessPoint = m_pModelSmv->item(pItem->row(), 2)->text();
        const QString strLDevice = m_pModelSmv->item(pItem->row(), 3)->text();
        const QString strControlBlock = m_pModelSmv->item(pItem->row(), 4)->text();
        const QString strMacAddress = m_pModelSmv->item(pItem->row(), 5)->text();
        const QString strVLanId = m_pModelSmv->item(pItem->row(), 6)->text();
        const QString strVLanPriority = m_pModelSmv->item(pItem->row(), 7)->text();
        const QString strAppId = m_pModelSmv->item(pItem->row(), 8)->text();

        dlg.SetDevice(strDevice);
        dlg.SetAccessPoint(strAccessPoint);
        dlg.SetLDevice(strLDevice);
        dlg.SetControlBlock(strControlBlock);
        dlg.SetMacAddress(strMacAddress);
        dlg.SetVLanId(strVLanId);
        dlg.SetVLanPriority(strVLanPriority);
        dlg.SetAppId(strAppId);

        if(dlg.exec() != QDialog::Accepted)
            return;

        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        QByteArray baCommunicationContent;
        if(m_pProjectVersion->DbReadSclCommunication(baCommunicationContent) && !baCommunicationContent.isNull() && !baCommunicationContent.isEmpty())
        {
            SclParser::SCLElement ElementCommunication("Communication", 0, 0);
            ElementCommunication.ReadFromBinary(baCommunicationContent);

            if(SclParser::SCLElement *pSCLElementSMV = FindElementSmv(&ElementCommunication, strSubnetwork, strIED, strAccessPoint, strLDevice, strControlBlock))
            {
                SclParser::SCLElement *pSCLElementAddress = pSCLElementSMV->GetFirstChild("Address");
                if(!pSCLElementAddress)
                    pSCLElementAddress = pSCLElementSMV->CreateChild("Address", false);

                SclParser::SCLElement *pSCLElementPMAC = 0, *pSCLElementPVLANID = 0, *pSCLElementPVLANPRIORITY = 0, *pSCLElementPAPPID = 0;
                foreach(SclParser::SCLElement *pSCLElementP, pSCLElementAddress->FindChildByType("P"))
                {
                    const QString strPType = pSCLElementP->GetAttribute("type");
                    if(strPType == QLatin1String("MAC-Address"))
                        pSCLElementPMAC = pSCLElementP;
                    else if(strPType == QLatin1String("VLAN-ID"))
                        pSCLElementPVLANID = pSCLElementP;
                    else if(strPType == QLatin1String("VLAN-PRIORITY"))
                        pSCLElementPVLANPRIORITY = pSCLElementP;
                    else if(strPType == QLatin1String("APPID"))
                        pSCLElementPAPPID = pSCLElementP;
                }

                if(!pSCLElementPMAC)
                {
                    pSCLElementPMAC = pSCLElementAddress->CreateChild("P", false);
                    pSCLElementPMAC->SetAttribute("type", "MAC-Address");
                }
                pSCLElementPMAC->SetValue(dlg.GetMacAddress());

                if(!pSCLElementPVLANID)
                {
                    pSCLElementPVLANID = pSCLElementAddress->CreateChild("P", false);
                    pSCLElementPVLANID->SetAttribute("type", "VLAN-ID");
                }
                pSCLElementPVLANID->SetValue(dlg.GetVLanId());

                if(!pSCLElementPVLANPRIORITY)
                {
                    pSCLElementPVLANPRIORITY = pSCLElementAddress->CreateChild("P", false);
                    pSCLElementPVLANPRIORITY->SetAttribute("type", "VLAN-PRIORITY");
                }
                pSCLElementPVLANPRIORITY->SetValue(dlg.GetVLanPriority());

                if(!pSCLElementPAPPID)
                {
                    pSCLElementPAPPID = pSCLElementAddress->CreateChild("P", false);
                    pSCLElementPAPPID->SetAttribute("type", "APPID");
                }
                pSCLElementPAPPID->SetValue(dlg.GetAppId());

                if(m_pProjectVersion->DbUpdateSclCommunication(ElementCommunication.WriteToBinary(), true))
                {
                    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pProjectVersion;
                    SetProjectObject(0);
                    SetProjectObject(pProjectVersion);
                }
            }
        }
    }
}
