#include <QApplication>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QTabBar>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QAction>
#include <QMessageBox>
#include <QSettings>
#include <QToolButton>
#include <QFileDialog>
#include <QHeaderView>

#include "utils/waitcursor.h"
#include "utils/readonlyview.h"
#include "projectexplorer/basemanager.h"
#include "projectexplorer/pblibdevice.h"
#include "projectexplorer/pblibboard.h"
#include "projectexplorer/pblibport.h"
#include "projectexplorer/pbdevicecategory.h"
#include "projectexplorer/pbbaycategory.h"
#include "projectexplorer/pbmanufacture.h"
#include "projectexplorer/pevterminal.h"
#include "sclparser/scldocument.h"
#include "sclparser/sclelement.h"
#include "core/mainwindow.h"
#include "core/vterminaldefine.h"

#include "propertydlglibdevice.h"
#include "propertydlglibboard.h"
#include "propertydlglibfiberport.h"

static const char * const g_szSettings_IcdFilePath      = "IcdFilePath";

static QString GetICDFilePath()
{
    return Core::MainWindow::Instance()->GetSettings()->value(g_szSettings_IcdFilePath, QString()).toString();
}

static void SetICDFilePath(const QString strICDFilePath)
{
    Core::MainWindow::Instance()->GetSettings()->setValue(g_szSettings_IcdFilePath, strICDFilePath);
}

using namespace DevLibrary::Internal;

PropertyDlgLibDevice::PropertyDlgLibDevice(ProjectExplorer::PbLibDevice *pLibDevice, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pLibDevice(pLibDevice), m_pCurrentPortItem(0)
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    m_pActionNewBoard = new QAction(QIcon(":/devlibrary/images/oper_add.png"), tr("New %1...").arg(ProjectExplorer::PbBaseObject::GetObjectTypeName(ProjectExplorer::PbBaseObject::botLibBoard)), this);
    connect(m_pActionNewBoard, SIGNAL(triggered()), this, SLOT(SlotActionNewBoard()));
    m_pActionNewPort = new QAction(QIcon(":/devlibrary/images/oper_add.png"), tr("New %1...").arg(ProjectExplorer::PbBaseObject::GetObjectTypeName(ProjectExplorer::PbBaseObject::botLibPort)), this);
    connect(m_pActionNewPort, SIGNAL(triggered()), this, SLOT(SlotActionNewPort()));
    m_pActionDelete = new QAction(QIcon(":/devlibrary/images/oper_remove.png"), tr("&Delete"), this);
    connect(m_pActionDelete, SIGNAL(triggered()), this, SLOT(SlotActionDelete()));
    m_pActionProperty = new QAction(QIcon(":/devlibrary/images/oper_edit.png"), tr("&Property..."), this);
    connect(m_pActionProperty, SIGNAL(triggered()), this, SLOT(SlotActionProperty()));
    QAction *pActionSeparator = new QAction(this);
    pActionSeparator->setSeparator(true);

    m_pLineEditType = new QLineEdit(this);
    m_pLineEditType->setObjectName(tr("Type"));
    m_pComboBoxManufacture = new QComboBox(this);
    m_pComboBoxManufacture->setObjectName(tr("Manufacture"));
    m_pComboBoxManufacture->setEditable(true);
    m_pComboBoxManufacture->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pComboBoxDeviceCategory = new QComboBox(this);
    m_pComboBoxDeviceCategory->setObjectName(tr("Device Category"));
    m_pComboBoxDeviceCategory->setEditable(false);
    m_pComboBoxBayCategory = new QComboBox(this);
    m_pComboBoxBayCategory->setObjectName(tr("Bay Category"));
    m_pComboBoxBayCategory->setEditable(false);
    m_pLineEditIcdFileName = new QLineEdit(this);
    m_pLineEditIcdFileName->setObjectName(tr("Icd File Name"));
    m_pLineEditIcdFileName->setReadOnly(true);

    QToolButton *pToolButtonBrowse = new QToolButton(this);
    pToolButtonBrowse->setText(tr("Browse..."));
    connect(pToolButtonBrowse, SIGNAL(clicked()), this, SLOT(SlotBrowseClicked()));

    QHBoxLayout *pHBoxLayout = new QHBoxLayout;
    pHBoxLayout->setContentsMargins(0, 0, 0, 0);
    pHBoxLayout->addWidget(m_pLineEditIcdFileName);
    pHBoxLayout->addWidget(pToolButtonBrowse);

    QGroupBox *pGroupBoxGeneral = new QGroupBox(this);
    QGridLayout *pGridLayoutGeneral = new QGridLayout(pGroupBoxGeneral);
    pGridLayoutGeneral->addWidget(new QLabel(m_pLineEditType->objectName() + ":", this), 0, 0);
    pGridLayoutGeneral->addWidget(m_pLineEditType, 0, 1);
    pGridLayoutGeneral->addWidget(new QLabel(m_pComboBoxManufacture->objectName() + ":", this), 0, 2);
    pGridLayoutGeneral->addWidget(m_pComboBoxManufacture, 0, 3);
    pGridLayoutGeneral->addWidget(new QLabel(m_pComboBoxDeviceCategory->objectName() + ":", this), 1, 0);
    pGridLayoutGeneral->addWidget(m_pComboBoxDeviceCategory, 1, 1);
    pGridLayoutGeneral->addWidget(new QLabel(m_pComboBoxBayCategory->objectName() + ":", this), 1, 2);
    pGridLayoutGeneral->addWidget(m_pComboBoxBayCategory, 1, 3);
    pGridLayoutGeneral->addWidget(new QLabel(m_pLineEditIcdFileName->objectName() + ":", this), 2, 0);
    pGridLayoutGeneral->addLayout(pHBoxLayout, 2, 1, 1, 3);

    m_pModelPort = new QStandardItemModel(0, 6, this);
    m_pModelPort->setHeaderData(0, Qt::Horizontal, tr("Board Name/Port Name"));
    m_pModelPort->setHeaderData(1, Qt::Horizontal, tr("Board Type/Port Index"));
    m_pModelPort->setHeaderData(2, Qt::Horizontal, tr("Board Description/Port Type"));
    m_pModelPort->setHeaderData(3, Qt::Horizontal, tr("Port Direction"));
    m_pModelPort->setHeaderData(4, Qt::Horizontal, tr("Port Fiber Plug"));
    m_pModelPort->setHeaderData(5, Qt::Horizontal, tr("Port Fiber Mode"));
    m_pViewPort = new Utils::ReadOnlyTreeView(this);
    m_pViewPort->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewPort->setAlternatingRowColors(true);
    m_pViewPort->addAction(m_pActionNewBoard);
    m_pViewPort->addAction(m_pActionNewPort);
    m_pViewPort->addAction(pActionSeparator);
    m_pViewPort->addAction(m_pActionDelete);
    m_pViewPort->addAction(m_pActionProperty);
    m_pViewPort->setModel(m_pModelPort);
    m_pViewPort->setColumnWidth(0, 150);
    m_pViewPort->setColumnWidth(1, 150);
    m_pViewPort->setColumnWidth(2, 150);
    m_pViewPort->setColumnWidth(3, 130);
    m_pViewPort->setColumnWidth(4, 130);
    m_pViewPort->setColumnWidth(5, 130);
    connect(m_pViewPort->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(SlotViewPortSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(m_pViewPort, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewPortDoubleClicked(const QModelIndex&)));

    m_pModelTxTerminal = new QStandardItemModel(0, 2);
    m_pModelTxTerminal->setHeaderData(0, Qt::Horizontal, tr("Tx Terminal"));
    m_pModelTxTerminal->setHeaderData(1, Qt::Horizontal, tr("Terminal Type"));
    m_pViewTxTerminal = new Utils::ReadOnlyTableView(this);
    m_pViewTxTerminal->setShowGrid(false);
    m_pViewTxTerminal->setAlternatingRowColors(true);
    m_pViewTxTerminal->setSortingEnabled(true);
    m_pViewTxTerminal->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewTxTerminal->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pViewTxTerminal->horizontalHeader()->setHighlightSections(false);
    m_pViewTxTerminal->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_pViewTxTerminal->horizontalHeader()->setStretchLastSection(true);
    m_pViewTxTerminal->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewTxTerminal->setModel(m_pModelTxTerminal);
    m_pViewTxTerminal->setColumnWidth(0, 270);
    m_pViewTxTerminal->setColumnWidth(1, 100);

    m_pModelRxTerminal = new QStandardItemModel(0, 2);
    m_pModelRxTerminal->setHeaderData(0, Qt::Horizontal, tr("Rx Terminal"));
    m_pModelRxTerminal->setHeaderData(1, Qt::Horizontal, tr("Terminal Type"));
    m_pViewRxTerminal = new Utils::ReadOnlyTableView(this);
    m_pViewRxTerminal->setShowGrid(false);
    m_pViewRxTerminal->setAlternatingRowColors(true);
    m_pViewRxTerminal->setSortingEnabled(true);
    m_pViewRxTerminal->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewRxTerminal->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pViewRxTerminal->horizontalHeader()->setHighlightSections(false);
    m_pViewRxTerminal->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_pViewRxTerminal->horizontalHeader()->setStretchLastSection(true);
    m_pViewRxTerminal->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewRxTerminal->setModel(m_pModelRxTerminal);
    m_pViewRxTerminal->setColumnWidth(0, 270);
    m_pViewRxTerminal->setColumnWidth(1, 100);

    QWidget *pWidgetVTerminal = new QWidget(this);
    QHBoxLayout *pHBoxLayoutVTerminal = new QHBoxLayout(pWidgetVTerminal);
    pHBoxLayoutVTerminal->setContentsMargins(0, 0, 0, 0);
    pHBoxLayoutVTerminal->setSpacing(0);
    pHBoxLayoutVTerminal->addWidget(m_pViewTxTerminal);
    pHBoxLayoutVTerminal->addSpacing(2);
    pHBoxLayoutVTerminal->addWidget(m_pViewRxTerminal);

    QTabBar *pTabBar = new QTabBar(this);
    pTabBar->setDrawBase(false);
    pTabBar->setExpanding(false);
    pTabBar->addTab(tr("Physical Port"));
    pTabBar->addTab(tr("Virtual Terminal"));

    QStackedWidget *pStackedWidget = new QStackedWidget(this);
    pStackedWidget->addWidget(m_pViewPort);
    pStackedWidget->addWidget(pWidgetVTerminal);

    connect(pTabBar, SIGNAL(currentChanged(int)),
            pStackedWidget, SLOT(setCurrentIndex(int)));

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBoxGeneral);
    pLayout->addSpacing(5);
    pLayout->addWidget(pTabBar);
    pLayout->addWidget(pStackedWidget);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    QList<ProjectExplorer::PbManufacture*> lstManufactures = ProjectExplorer::BaseManager::Instance()->GetAllManufactures();
    qSort(lstManufactures.begin(), lstManufactures.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    foreach(ProjectExplorer::PbManufacture *pManufacture, lstManufactures)
        m_pComboBoxManufacture->addItem(pManufacture->GetDisplayName());

    QList<ProjectExplorer::PbDeviceCategory*> lstDeviceCategories = ProjectExplorer::BaseManager::Instance()->GetAllDeviceCategories();
    qSort(lstDeviceCategories.begin(), lstDeviceCategories.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    foreach(ProjectExplorer::PbDeviceCategory *pDeviceCategory, lstDeviceCategories)
        m_pComboBoxDeviceCategory->addItem(pDeviceCategory->GetDisplayName());

    QList<ProjectExplorer::PbBayCategory*> lstBayCategories = ProjectExplorer::BaseManager::Instance()->GetAllBayCategories();
    qSort(lstBayCategories.begin(), lstBayCategories.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    foreach(ProjectExplorer::PbBayCategory *pBayCategory, lstBayCategories)
        m_pComboBoxBayCategory->addItem(pBayCategory->GetDisplayName());

    if(pLibDevice->GetId() == ProjectExplorer::PbBaseObject::m_iInvalidObjectId)
        SetWindowTitle(tr("New %1").arg(pLibDevice->GetDeviceTypeName(pLibDevice->GetDeviceType())));
    else
        SetWindowTitle(tr("%1 Property").arg(pLibDevice->GetDeviceTypeName(pLibDevice->GetDeviceType())));

    UpdateData(false);
}

QByteArray PropertyDlgLibDevice::GetIcdContent() const
{
    return m_baIcdContent;
}

bool PropertyDlgLibDevice::UpdateData(bool bSaveAndValidate)
{
    if(bSaveAndValidate)
    {
        // Type
        const QString strType = m_pLineEditType->text().trimmed();
        if(strType.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pLineEditType->objectName()));

            m_pLineEditType->setFocus();
            return false;
        }

        // Manufacture
        const QString strManufacture = m_pComboBoxManufacture->currentText().trimmed();
        if(strManufacture.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxManufacture->objectName()));

            m_pComboBoxManufacture->setFocus();
            return false;
        }

        // DeviceCategory
        const QString strDeviceCategory = m_pComboBoxDeviceCategory->currentText().trimmed();
        if(strDeviceCategory.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxDeviceCategory->objectName()));

            m_pComboBoxDeviceCategory->setFocus();
            return false;
        }

        // BayCategory
        const QString strBayCategory = m_pComboBoxBayCategory->currentText().trimmed();
        if(strBayCategory.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxBayCategory->objectName()));

            m_pComboBoxBayCategory->setFocus();
            return false;
        }

        foreach(ProjectExplorer::PbLibDevice *pLibDevice, ProjectExplorer::BaseManager::Instance()->GetAllLibDevices())
        {
            if(pLibDevice->GetId() == m_pLibDevice->GetId())
                continue;

            if(strType == pLibDevice->GetType() && strManufacture == pLibDevice->GetManufacture() &&
               strDeviceCategory == pLibDevice->GetDeviceCategory() && strBayCategory == pLibDevice->GetBayCategory())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The product can NOT be duplicate, please input a valid value."));

                return false;
            }
        }

        m_pLibDevice->SetType(strType);
        m_pLibDevice->SetManufacture(strManufacture);
        m_pLibDevice->SetDeviceCategory(strDeviceCategory);
        m_pLibDevice->SetBayCategory(strBayCategory);
        m_pLibDevice->SetIcdFileName(QFileInfo(m_pLineEditIcdFileName->text()).fileName());
    }
    else
    {
        m_pLineEditType->setText(m_pLibDevice->GetType());

        m_pComboBoxManufacture->setEditText(m_pLibDevice->GetManufacture());

        int iIndex = m_pComboBoxDeviceCategory->findText(m_pLibDevice->GetDeviceCategory());
        if(iIndex >= 0)
            m_pComboBoxDeviceCategory->setCurrentIndex(iIndex);

        iIndex = m_pComboBoxBayCategory->findText(m_pLibDevice->GetBayCategory());
        if(iIndex >= 0)
            m_pComboBoxBayCategory->setCurrentIndex(iIndex);

        m_pLineEditIcdFileName->setText(m_pLibDevice->GetIcdFileName());

        ProjectExplorer::BaseManager::Instance()->DbReadLibDeviceModel(m_pLibDevice->GetId(), m_baIcdContent);

        BuildPortData();
        BuildTerminalData();
    }

    return true;
}

void PropertyDlgLibDevice::BuildPortData()
{
    foreach(ProjectExplorer::PbLibBoard *pLibBoard, m_pLibDevice->GetChildBoards())
    {
        if(QStandardItem *pItemLibBoard = InsertBoardItem(pLibBoard))
        {
            foreach(ProjectExplorer::PbLibPort *pLibPort, pLibBoard->GetChildPorts())
                InsertPortItem(pItemLibBoard, pLibPort);
        }
    }

    m_pViewPort->expandAll();
}

void PropertyDlgLibDevice::BuildTerminalData()
{
    m_pModelTxTerminal->removeRows(0, m_pModelTxTerminal->rowCount());
    m_pModelRxTerminal->removeRows(0, m_pModelRxTerminal->rowCount());

    if(m_baIcdContent.isEmpty() || m_baIcdContent.isNull())
        return;

    SclParser::SCLDocument document;
    document.ReadBinary(m_baIcdContent);

    SclParser::SCLElement *pSCLElementRoot = document.Root();
    if(!pSCLElementRoot)
        return;

    SclParser::SCLElement *pSCLElementIED = pSCLElementRoot->GetFirstChild("IED");
    if(!pSCLElementIED)
        return;

    int iCount = 0;
    foreach(const QStringList lstTerminalData, Core::VTerminalDefine::Instance()->PickupGooseTx(pSCLElementIED))
    {
        QString strRefName = QString("%1/%2%3%4.%5").arg(lstTerminalData.at(0))
                                                    .arg(lstTerminalData.at(1))
                                                    .arg(lstTerminalData.at(2))
                                                    .arg(lstTerminalData.at(3))
                                                    .arg(lstTerminalData.at(5));
        if(!lstTerminalData.at(6).isEmpty())
            strRefName = QString("%1.%2").arg(strRefName).arg(lstTerminalData.at(6));

        if(!lstTerminalData.at(7).isEmpty())
            strRefName = QString("%1:%2").arg(strRefName).arg(lstTerminalData.at(7));

        QStandardItem *pItem = new QStandardItem(QIcon(ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otVTerminal)), strRefName);
        m_pModelTxTerminal->setItem(iCount, 0, pItem);

        pItem = new QStandardItem("GOOSE");
        m_pModelTxTerminal->setItem(iCount, 1, pItem);

        iCount++;
    }
    foreach(const QStringList lstTerminalData, Core::VTerminalDefine::Instance()->PickupSvTx(pSCLElementIED))
    {
        QString strRefName = QString("%1/%2%3%4.%5").arg(lstTerminalData.at(0))
                                                    .arg(lstTerminalData.at(1))
                                                    .arg(lstTerminalData.at(2))
                                                    .arg(lstTerminalData.at(3))
                                                    .arg(lstTerminalData.at(5));
        if(!lstTerminalData.at(6).isEmpty())
            strRefName = QString("%1.%2").arg(strRefName).arg(lstTerminalData.at(6));

        if(!lstTerminalData.at(7).isEmpty())
            strRefName = QString("%1:%2").arg(strRefName).arg(lstTerminalData.at(7));

        QStandardItem *pItem = new QStandardItem(QIcon(ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otVTerminal)), strRefName);
        m_pModelTxTerminal->setItem(iCount, 0, pItem);

        pItem = new QStandardItem("SV");
        m_pModelTxTerminal->setItem(iCount, 1, pItem);

        iCount++;
    }

    iCount = 0;
    foreach(const QStringList lstTerminalData, Core::VTerminalDefine::Instance()->PickupGooseRx(pSCLElementIED))
    {
        QString strRefName = QString("%1/%2%3%4.%5").arg(lstTerminalData.at(0))
                                                    .arg(lstTerminalData.at(1))
                                                    .arg(lstTerminalData.at(2))
                                                    .arg(lstTerminalData.at(3))
                                                    .arg(lstTerminalData.at(5));
        if(!lstTerminalData.at(6).isEmpty())
            strRefName = QString("%1.%2").arg(strRefName).arg(lstTerminalData.at(6));

        if(!lstTerminalData.at(7).isEmpty())
            strRefName = QString("%1:%2").arg(strRefName).arg(lstTerminalData.at(7));

        QStandardItem *pItem = new QStandardItem(QIcon(ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otVTerminal)), strRefName);
        m_pModelRxTerminal->setItem(iCount, 0, pItem);

        pItem = new QStandardItem("GOOSE");
        m_pModelRxTerminal->setItem(iCount, 1, pItem);

        iCount++;
    }
    foreach(const QStringList lstTerminalData, Core::VTerminalDefine::Instance()->PickupSvRx(pSCLElementIED))
    {
        QString strRefName = QString("%1/%2%3%4.%5").arg(lstTerminalData.at(0))
                                                    .arg(lstTerminalData.at(1))
                                                    .arg(lstTerminalData.at(2))
                                                    .arg(lstTerminalData.at(3))
                                                    .arg(lstTerminalData.at(5));
        if(!lstTerminalData.at(6).isEmpty())
            strRefName = QString("%1.%2").arg(strRefName).arg(lstTerminalData.at(6));

        if(!lstTerminalData.at(7).isEmpty())
            strRefName = QString("%1:%2").arg(strRefName).arg(lstTerminalData.at(7));

        QStandardItem *pItem = new QStandardItem(QIcon(ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otVTerminal)), strRefName);
        m_pModelRxTerminal->setItem(iCount, 0, pItem);

        pItem = new QStandardItem("SV");
        m_pModelRxTerminal->setItem(iCount, 1, pItem);

        iCount++;
    }
}

QStandardItem* PropertyDlgLibDevice::InsertBoardItem(ProjectExplorer::PbLibBoard *pLibBoard)
{
    if(!pLibBoard)
        return 0;

    QList<QStandardItem*> lstBoardItems;

    QStandardItem *pItemBoardPosition = new QStandardItem(pLibBoard->GetDisplayIcon(), pLibBoard->GetPosition());
    pItemBoardPosition->setData(reinterpret_cast<int>(pLibBoard));
    lstBoardItems.append(pItemBoardPosition);

    QStandardItem *pItemBoardType = new QStandardItem(pLibBoard->GetType());
    pItemBoardType->setData(reinterpret_cast<int>(pLibBoard));
    lstBoardItems.append(pItemBoardType);

    QStandardItem *pItemBoardDesc = new QStandardItem(pLibBoard->GetDescription());
    pItemBoardDesc->setData(reinterpret_cast<int>(pLibBoard));
    lstBoardItems.append(pItemBoardDesc);

    m_pModelPort->appendRow(lstBoardItems);
    return pItemBoardPosition;
}

void PropertyDlgLibDevice::UpdateBoardItem(QStandardItem *pItemLibBoard)
{
    if(!pItemLibBoard)
        return;

    ProjectExplorer::PbLibBoard *pLibBoard = reinterpret_cast<ProjectExplorer::PbLibBoard*>(pItemLibBoard->data().toInt());
    if(!pLibBoard)
        return;

    QStandardItem *pItemBoardPosition = m_pModelPort->item(pItemLibBoard->row(), 0);
    pItemBoardPosition->setText(pLibBoard->GetPosition());

    QStandardItem *pItemBoardType = m_pModelPort->item(pItemLibBoard->row(), 1);
    pItemBoardType->setText(pLibBoard->GetType());

    QStandardItem *pItemBoardDesc = m_pModelPort->item(pItemLibBoard->row(), 2);
    pItemBoardDesc->setText(pLibBoard->GetDescription());
}

QStandardItem* PropertyDlgLibDevice::InsertPortItem(QStandardItem *pItemLibBoard, ProjectExplorer::PbLibPort *pLibPort)
{
    if(!pItemLibBoard || !pLibPort)
        return 0;

    QList<QStandardItem*> lstPortItems;

    QStandardItem *pItemPortName = new QStandardItem(pLibPort->GetDisplayIcon(), pLibPort->GetName());
    pItemPortName->setData(reinterpret_cast<int>(pLibPort));
    lstPortItems.append(pItemPortName);

    QStandardItem *pItemPortIndex = new QStandardItem(QString::number(pLibPort->GetGroup()));
    pItemPortIndex->setData(reinterpret_cast<int>(pLibPort));
    lstPortItems.append(pItemPortIndex);

    QStandardItem *pItemPortType = new QStandardItem(ProjectExplorer::PbLibPort::GetPortTypeName(pLibPort->GetPortType()));
    pItemPortType->setData(reinterpret_cast<int>(pLibPort));
    lstPortItems.append(pItemPortType);

    QStandardItem *pItemPortDirection = new QStandardItem(ProjectExplorer::PbLibPort::GetPortDirectionName(pLibPort->GetPortDirection()));
    pItemPortDirection->setData(reinterpret_cast<int>(pLibPort));
    lstPortItems.append(pItemPortDirection);

    QStandardItem *pItemPortFiberPlug = new QStandardItem(ProjectExplorer::PbLibPort::GetFiberPlugName(pLibPort->GetFiberPlug()));
    pItemPortFiberPlug->setData(reinterpret_cast<int>(pLibPort));
    lstPortItems.append(pItemPortFiberPlug);

    QStandardItem *pItemPortFiberMode = new QStandardItem(ProjectExplorer::PbLibPort::GetFiberModeName(pLibPort->GetFiberMode()));
    pItemPortFiberMode->setData(reinterpret_cast<int>(pLibPort));
    lstPortItems.append(pItemPortFiberMode);

    pItemLibBoard->appendRow(lstPortItems);
    return pItemPortName;
}

void PropertyDlgLibDevice::UpdatePortItem(QStandardItem *pItemLibPort)
{
    if(!pItemLibPort)
        return;

    QStandardItem *pItemLibBoard = pItemLibPort->parent();
    if(!pItemLibBoard)
        return;

    ProjectExplorer::PbLibPort *pLibPort = reinterpret_cast<ProjectExplorer::PbLibPort*>(pItemLibPort->data().toInt());
    if(!pLibPort)
        return;

    QStandardItem *pItemPortName = pItemLibBoard->child(pItemLibPort->row(), 0);
    pItemPortName->setText(pLibPort->GetName());

    QStandardItem *pItemPortIndex = pItemLibBoard->child(pItemLibPort->row(), 1);
    pItemPortIndex->setText(QString::number(pLibPort->GetGroup()));

    QStandardItem *pItemPortType = pItemLibBoard->child(pItemLibPort->row(), 2);
    pItemPortType->setText(ProjectExplorer::PbLibPort::GetPortTypeName(pLibPort->GetPortType()));

    QStandardItem *pItemPortDirection = pItemLibBoard->child(pItemLibPort->row(), 3);
    pItemPortDirection->setText(ProjectExplorer::PbLibPort::GetPortDirectionName(pLibPort->GetPortDirection()));

    QStandardItem *pItemPortFiberPlug = pItemLibBoard->child(pItemLibPort->row(), 4);
    pItemPortFiberPlug->setText(ProjectExplorer::PbLibPort::GetFiberPlugName(pLibPort->GetFiberPlug()));

    QStandardItem *pItemPortFiberMode = pItemLibBoard->child(pItemLibPort->row(), 5);
    pItemPortFiberMode->setText(ProjectExplorer::PbLibPort::GetFiberModeName(pLibPort->GetFiberMode()));
}

void PropertyDlgLibDevice::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}

void PropertyDlgLibDevice::SlotViewPortSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    QItemSelectionModel *pSelectionModel = m_pViewPort->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
    if(lstSelectedIndex.size() == 1)
        m_pCurrentPortItem = m_pModelPort->itemFromIndex(lstSelectedIndex.first());
    else
        m_pCurrentPortItem = 0;

    m_pActionNewBoard->setEnabled(true);
    m_pActionNewPort->setEnabled(m_pCurrentPortItem);
    m_pActionDelete->setEnabled(m_pCurrentPortItem);
    m_pActionProperty->setEnabled(m_pCurrentPortItem);
}

void PropertyDlgLibDevice::SlotViewPortDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    SlotActionProperty();
}

void PropertyDlgLibDevice::SlotBrowseClicked()
{
    const QString strIcdFileName = QFileDialog::getOpenFileName(this,
                                                                tr("Open ICD File"),
                                                                GetICDFilePath(),
                                                                tr("IED Capability Description File (*.icd);;All Files (*.*)"));

    if(!strIcdFileName.isEmpty())
    {
        SclParser::SCLDocument document;
        if(!document.Open(strIcdFileName))
            return;

        m_pLineEditIcdFileName->setText(QDir::toNativeSeparators(strIcdFileName));
        m_baIcdContent = document.WriteBinary();
        BuildTerminalData();
    }
}

void PropertyDlgLibDevice::SlotActionNewBoard()
{
    ProjectExplorer::PbLibBoard libboard;
    libboard.SetParentDevice(m_pLibDevice);

    PropertyDlgLibBoard dlg(&libboard, true, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    ProjectExplorer::PbLibBoard *pLibBoard = new ProjectExplorer::PbLibBoard(libboard);
    m_pLibDevice->AddChildBoard(pLibBoard);
    pLibBoard->SetParentDevice(m_pLibDevice);

    InsertBoardItem(pLibBoard);
}

void PropertyDlgLibDevice::SlotActionNewPort()
{
    if(!m_pCurrentPortItem || m_pCurrentPortItem->parent())
        return;

    ProjectExplorer::PbLibBoard *pLibBoard = 0;
    if(m_pCurrentPortItem->parent()) // Port Item
    {
        if(ProjectExplorer::PbLibPort *pLibPort = reinterpret_cast<ProjectExplorer::PbLibPort*>(m_pCurrentPortItem->data().toInt()))
            pLibBoard = pLibPort->GetParentBoard();
    }
    else // Board Item
    {
        pLibBoard = reinterpret_cast<ProjectExplorer::PbLibBoard*>(m_pCurrentPortItem->data().toInt());
    }

    if(!pLibBoard)
        return;

    ProjectExplorer::PbLibPort libport;
    libport.SetParentBoard(pLibBoard);
    libport.SetPortType(ProjectExplorer::PbLibPort::ptFiber);
    libport.SetPortDirection(ProjectExplorer::PbLibPort::pdRT);

    PropertyDlgLibFiberPort dlg(&libport, true, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    const int iNewPortNumber = dlg.GetNewPortNumber();
    int iGroup = libport.GetGroup();
    QList<int> lstExistGroups;
    foreach(ProjectExplorer::PbLibPort *pPort, pLibBoard->GetChildPorts())
    {
        if(!lstExistGroups.contains(pPort->GetGroup()))
            lstExistGroups.append(pPort->GetGroup());
    }

    if(libport.GetPortDirection() == ProjectExplorer::PbLibPort::pdRT)
    {
        const QStringList lstPortName = libport.GetName().split(PropertyDlgLibFiberPort::m_strSpliter);
        Q_ASSERT(lstPortName.size() == 2);

        for(int i = 0; i < iNewPortNumber; i++)
        {
            while(lstExistGroups.contains(iGroup))
                iGroup++;
            lstExistGroups.append(iGroup);

            ProjectExplorer::PbLibPort *pLibPortTx = new ProjectExplorer::PbLibPort(libport);
            pLibPortTx->SetGroup(iGroup);
            pLibPortTx->SetPortDirection(ProjectExplorer::PbLibPort::pdTx);
            pLibPortTx->SetName(iNewPortNumber == 1 ? lstPortName.at(0) : QString("%1%2").arg(lstPortName.at(0)).arg(iGroup, 2, 10, QLatin1Char('0')));
            pLibPortTx->SetParentBoard(pLibBoard);
            pLibBoard->AddChildPort(pLibPortTx);
            InsertPortItem(m_pCurrentPortItem, pLibPortTx);

            ProjectExplorer::PbLibPort *pLibPortRx = new ProjectExplorer::PbLibPort(libport);
            pLibPortRx->SetGroup(iGroup);
            pLibPortRx->SetPortDirection(ProjectExplorer::PbLibPort::pdRx);
            pLibPortRx->SetName(iNewPortNumber == 1 ? lstPortName.at(1) : QString("%1%2").arg(lstPortName.at(1)).arg(iGroup, 2, 10, QLatin1Char('0')));
            pLibPortRx->SetParentBoard(pLibBoard);
            pLibBoard->AddChildPort(pLibPortRx);
            InsertPortItem(m_pCurrentPortItem, pLibPortRx);
        }
    }
    else
    {
        for(int i = 0; i < iNewPortNumber; i++)
        {
            while(lstExistGroups.contains(iGroup))
                iGroup++;
            lstExistGroups.append(iGroup);

            ProjectExplorer::PbLibPort *pLibPort = new ProjectExplorer::PbLibPort(libport);
            pLibPort->SetGroup(iGroup);
            pLibPort->SetName(iNewPortNumber == 1 ? libport.GetName() : QString("%1%2").arg(libport.GetName()).arg(iGroup, 2, 10, QLatin1Char('0')));
            pLibPort->SetParentBoard(pLibBoard);
            pLibBoard->AddChildPort(pLibPort);
            InsertPortItem(m_pCurrentPortItem, pLibPort);
        }
    }
}

void PropertyDlgLibDevice::SlotActionDelete()
{
    if(!m_pCurrentPortItem)
        return;

    if(QStandardItem *pItemLibBoard = m_pCurrentPortItem->parent()) // Port Item
    {
        ProjectExplorer::PbLibPort *pLibPort = reinterpret_cast<ProjectExplorer::PbLibPort*>(m_pCurrentPortItem->data().toInt());
        if(!pLibPort)
            return;

        ProjectExplorer::PbLibBoard *pLibBoard = pLibPort->GetParentBoard();
        if(!pLibBoard)
            return;

        ProjectExplorer::PbLibPort *pLibPortPair = pLibPort->GetPairPort();

        pLibBoard->RemoveChildPort(pLibPort);
        delete pLibPort;
        pItemLibBoard->removeRow(m_pCurrentPortItem->row());

        if(pLibPortPair)
        {
            for(int i = 0; i < pItemLibBoard->rowCount(); i++)
            {
                if(QStandardItem *pItemLibPortPair = pItemLibBoard->child(i, 0))
                {
                    if(reinterpret_cast<ProjectExplorer::PbLibPort*>(pItemLibPortPair->data().toInt()) == pLibPortPair)
                    {
                        pItemLibBoard->removeRow(pItemLibPortPair->row());
                        break;
                    }
                }
            }

            pLibBoard->RemoveChildPort(pLibPortPair);
            delete pLibPortPair;
        }
    }
    else // Board Item
    {
        ProjectExplorer::PbLibBoard *pLibBoard = reinterpret_cast<ProjectExplorer::PbLibBoard*>(m_pCurrentPortItem->data().toInt());
        if(!pLibBoard)
            return;

        foreach(ProjectExplorer::PbLibPort *pLibPort, pLibBoard->GetChildPorts())
        {
            pLibBoard->RemoveChildPort(pLibPort);
            delete pLibPort;
        }

        m_pLibDevice->RemoveChildBoard(pLibBoard);
        delete pLibBoard;
        m_pModelPort->removeRow(m_pCurrentPortItem->row());
    }
}

void PropertyDlgLibDevice::SlotActionProperty()
{
    if(!m_pCurrentPortItem)
        return;

    if(m_pCurrentPortItem->parent()) // Port Item
    {
        ProjectExplorer::PbLibPort *pLibPort = reinterpret_cast<ProjectExplorer::PbLibPort*>(m_pCurrentPortItem->data().toInt());
        if(!pLibPort)
            return;

        if(pLibPort->GetPortType() == ProjectExplorer::PbLibPort::ptFiber)
        {
            ProjectExplorer::PbLibPort libport(*pLibPort);
            ProjectExplorer::PbLibPort *pLibPortPair = pLibPort->GetPairPort();
            if(pLibPortPair)
            {
                libport.SetPortDirection(ProjectExplorer::PbLibPort::pdRT);
                if(pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdTx)
                    libport.SetName(pLibPort->GetName() + PropertyDlgLibFiberPort::m_strSpliter + pLibPortPair->GetName());
                else if(pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdRx)
                    libport.SetName(pLibPortPair->GetName() + PropertyDlgLibFiberPort::m_strSpliter + pLibPort->GetName());
            }

            PropertyDlgLibFiberPort dlg(&libport, false, this);
            if(dlg.exec() != QDialog::Accepted)
                return;

            if(libport.GetPortDirection() == ProjectExplorer::PbLibPort::pdRT)
            {
                const QStringList lstPortName = libport.GetName().split(PropertyDlgLibFiberPort::m_strSpliter);
                Q_ASSERT(lstPortName.size() == 2);

                if(pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdTx)
                {
                    *pLibPort = libport;
                    pLibPort->SetName(lstPortName.at(0));
                    pLibPort->SetPortDirection(ProjectExplorer::PbLibPort::pdTx);

                    int iOldLibPortPairId = pLibPortPair->GetId();
                    *pLibPortPair = libport;
                    pLibPortPair->SetId(iOldLibPortPairId);
                    pLibPortPair->SetName(lstPortName.at(1));
                    pLibPortPair->SetPortDirection(ProjectExplorer::PbLibPort::pdRx);
                }
                else if(pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdRx)
                {
                    *pLibPort = libport;
                    pLibPort->SetName(lstPortName.at(1));
                    pLibPort->SetPortDirection(ProjectExplorer::PbLibPort::pdRx);

                    int iOldLibPortPairId = pLibPortPair->GetId();
                    *pLibPortPair = libport;
                    pLibPortPair->SetId(iOldLibPortPairId);
                    pLibPortPair->SetName(lstPortName.at(0));
                    pLibPortPair->SetPortDirection(ProjectExplorer::PbLibPort::pdTx);
                }

                UpdatePortItem(m_pCurrentPortItem);

                QStandardItem *pItemLibBoard = m_pCurrentPortItem->parent();
                for(int i = 0; i < pItemLibBoard->rowCount(); i++)
                {
                    if(QStandardItem *pItemLibPortPair = pItemLibBoard->child(i, 0))
                    {
                        if(reinterpret_cast<ProjectExplorer::PbLibPort*>(pItemLibPortPair->data().toInt()) == pLibPortPair)
                        {
                            UpdatePortItem(pItemLibPortPair);
                            break;
                        }
                    }
                }

            }
            else
            {
                *pLibPort = libport;
                UpdatePortItem(m_pCurrentPortItem);
            }
        }
    }
    else // Board Item
    {
        ProjectExplorer::PbLibBoard *pLibBoard = reinterpret_cast<ProjectExplorer::PbLibBoard*>(m_pCurrentPortItem->data().toInt());
        if(!pLibBoard)
            return;

        PropertyDlgLibBoard dlg(pLibBoard, false, this);
        if(dlg.exec() == QDialog::Accepted)
            UpdateBoardItem(m_pCurrentPortItem);
    }
}
