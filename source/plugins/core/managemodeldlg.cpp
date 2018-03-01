#include <QApplication>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QFileInfo>

#include "utils/readonlyview.h"
#include "sclparser/scldocument.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peproject.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pedevice.h"

#include "mainwindow.h"
#include "configcoreoperation.h"

#include "managemodeldlg.h"

static const char * const g_szSettings_ScdFileDir       = "ScdFileDir";
static const char * const g_szSettings_IcdFilePath      = "IcdFilePath";

using namespace Core::Internal;

ManageModelDlg::ManageModelDlg(QWidget *pParent) :
    StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Model Management"));

    m_pComboBoxProjectVersion = new QComboBox(this);
    m_pComboBoxProjectVersion->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *pHBoxLayoutProjectVersion = new QHBoxLayout;
    pHBoxLayoutProjectVersion->addWidget(new QLabel(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otProject) + ":", this));
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

    // Create Model
    m_pModel = new QStandardItemModel(0, 5, this);
    m_pModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_pModel->setHeaderData(1, Qt::Horizontal, tr("Manufacture"));
    m_pModel->setHeaderData(2, Qt::Horizontal, tr("Type"));
    m_pModel->setHeaderData(3, Qt::Horizontal, tr("Config Version"));
    m_pModel->setHeaderData(4, Qt::Horizontal, tr("Icd File Name"));

    // Create View
    m_pView = new Utils::ReadOnlyTableView(this);
    m_pView->setAlternatingRowColors(true);
    m_pView->setSortingEnabled(true);
    m_pView->setModel(m_pModel);

    m_pView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pView->horizontalHeader()->setHighlightSections(false);
    m_pView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pView->horizontalHeader()->setStretchLastSection(true);
    m_pView->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pView->verticalHeader()->hide();
    m_pView->setShowGrid(false);

    m_pView->setColumnWidth(0, 300);
    m_pView->setColumnWidth(1, 150);
    m_pView->setColumnWidth(2, 150);
    m_pView->setColumnWidth(3, 100);
    m_pView->setColumnWidth(4, 159);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    m_pPushButtonImportScd = pDialogButtonBox->addButton(tr("Import Scd"), QDialogButtonBox::ActionRole);
    m_pPushButtonExportScd = pDialogButtonBox->addButton(tr("Export Scd"), QDialogButtonBox::ActionRole);
    m_pPushButtonExportSpcd = pDialogButtonBox->addButton(tr("Export Spcd"), QDialogButtonBox::ActionRole);
    m_pPushButtonExportIpcd = pDialogButtonBox->addButton(tr("Export Ipcd"), QDialogButtonBox::ActionRole);
    m_pPushButtonImportIpcd = pDialogButtonBox->addButton(tr("Import Ipcd"), QDialogButtonBox::ActionRole);
    m_pPushButtonAssociate = pDialogButtonBox->addButton(tr("Associate Model"), QDialogButtonBox::ActionRole);
    m_pPushButtonClear = pDialogButtonBox->addButton(tr("Clear Model"), QDialogButtonBox::ActionRole);
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(pDialogButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(SlotDialogButtonBoxClicked(QAbstractButton*)));
    m_pPushButtonExportIpcd->setEnabled(false);
    m_pPushButtonAssociate->setEnabled(false);
    m_pPushButtonClear->setEnabled(false);

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addLayout(pHBoxLayoutProjectVersion);
    pLayout->addSpacing(10);
    pLayout->addWidget(m_pView);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    connect(m_pComboBoxProjectVersion, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentProjectVersionChanged(int)));
    connect(m_pView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));

    SlotCurrentProjectVersionChanged(m_pComboBoxProjectVersion->currentIndex());

    ReadSettings();
}

ManageModelDlg::~ManageModelDlg()
{
    SaveSettings();
}

void ManageModelDlg::ReadSettings()
{
    QSettings *pSettings = Core::MainWindow::Instance()->GetSettings();

    m_strScdFileDir = pSettings->value(g_szSettings_ScdFileDir).toString();
    m_strIcdFileDir = pSettings->value(g_szSettings_IcdFilePath).toString();
}

void ManageModelDlg::SaveSettings()
{
    QSettings *pSettings = Core::MainWindow::Instance()->GetSettings();

    pSettings->setValue(g_szSettings_ScdFileDir, m_strScdFileDir);
    pSettings->setValue(g_szSettings_IcdFilePath, m_strIcdFileDir);
}

void ManageModelDlg::SlotDialogButtonBoxClicked(QAbstractButton* pButton)
{
    int iCurrentIndex = m_pComboBoxProjectVersion->currentIndex();
    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxProjectVersion->itemData(iCurrentIndex).toInt());
    if(!pProjectVersion)
        return;

    QList<ProjectExplorer::PeDevice*> lstDevices;
    const QModelIndexList lstIndex = m_pView->selectionModel()->selectedRows();
    foreach(const QModelIndex &index, lstIndex)
    {
        QStandardItem *pItem = m_pModel->itemFromIndex(index);
        if(!pItem)
            continue;

        if(ProjectExplorer::PeDevice *pDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(pItem->data().toInt()))
            lstDevices.append(pDevice);
    }

    if(pButton == m_pPushButtonImportScd)
    {
        if(!pProjectVersion->ValidateVersionPwd(this))
            return;

        const QString strFileName = QFileDialog::getOpenFileName(this,
                                                                 tr("Import SCD File"),
                                                                 m_strScdFileDir,
                                                                 tr("Substation Configuration Description File (*.scd)"));

        if(!strFileName.isEmpty())
        {
            qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
            bool bResult = Core::ConfigCoreOperation::Instance()->ImportFromScd(pProjectVersion, strFileName);
            qApp->restoreOverrideCursor();

            if(bResult)
            {
                SlotCurrentProjectVersionChanged(iCurrentIndex);
                QMessageBox::information(this, tr("Import Success"), tr("Succeed to import SCD file!"));
            }
            else
            {
                QMessageBox::critical(this, tr("Import Failure"), tr("Failed to import SCD file!"));
            }

            m_strScdFileDir = QFileInfo(strFileName).absolutePath();
        }
    }
    else if(pButton == m_pPushButtonExportScd)
    {
        const QString strFileName = QFileDialog::getSaveFileName(this,
                                                                 tr("Export SCD File"),
                                                                 QDir(m_strScdFileDir).absoluteFilePath(pProjectVersion->GetProject()->GetName() + ".scd"),
                                                                 tr("Substation Configuration Description File (*.scd)"));
        if(!strFileName.isEmpty())
        {
            QMessageBox::StandardButton sb = QMessageBox::question(this,
                                                                   tr("Export SCD File"),
                                                                   tr("Do you want to export SCD file with port connection information?"),
                                                                   QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            if(sb == QMessageBox::Cancel)
                return;

            qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
            bool bResult = Core::ConfigCoreOperation::Instance()->ExportToScd(pProjectVersion, strFileName, sb == QMessageBox::Yes);
            qApp->restoreOverrideCursor();

            if(bResult)
                QMessageBox::information(this, tr("Export Success"), tr("Succeed to export SCD file!"));
            else
                QMessageBox::critical(this, tr("Export Failure"), tr("Failed to export SCD file!"));

            m_strScdFileDir = QFileInfo(strFileName).absolutePath();
        }
    }
    else if(pButton == m_pPushButtonExportSpcd)
    {
        const QString strFileName = QFileDialog::getSaveFileName(this,
                                                                 tr("Export SPCD File"),
                                                                 QDir(m_strScdFileDir).absoluteFilePath(pProjectVersion->GetProject()->GetName() + ".spcd"),
                                                                 tr("Substation Physical Configuration Description File (*.spcd)"));

        if(!strFileName.isEmpty())
        {
            qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
            bool bResult = Core::ConfigCoreOperation::Instance()->ExportSpcd(pProjectVersion, strFileName);
            qApp->restoreOverrideCursor();

            if(bResult)
                QMessageBox::information(this, tr("Export Success"), tr("Succeed to export SPCD file!"));
            else
                QMessageBox::critical(this, tr("Export Failure"), tr("Failed to export SPCD file!"));

            m_strScdFileDir = QFileInfo(strFileName).absolutePath();
        }
    }
    else if(pButton == m_pPushButtonExportIpcd)
    {
        const QString strFileName = QFileDialog::getSaveFileName(this,
                                                                 tr("Export IPCD File"),
                                                                 QDir(m_strScdFileDir).absoluteFilePath(lstDevices.first()->GetName() + ".ipcd"),
                                                                 tr("IED Physical Configuration Description File (*.ipcd)"));

        if(!strFileName.isEmpty())
        {
            qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
            bool bResult = Core::ConfigCoreOperation::Instance()->ExportIpcd(lstDevices.first(), strFileName);
            qApp->restoreOverrideCursor();

            if(bResult)
                QMessageBox::information(this, tr("Export Success"), tr("Succeed to export IPCD file!"));
            else
                QMessageBox::critical(this, tr("Export Failure"), tr("Failed to export IPCD file!"));

            m_strScdFileDir = QFileInfo(strFileName).absolutePath();
        }
    }
    else if(pButton == m_pPushButtonImportIpcd)
    {
        const QString strFileName = QFileDialog::getOpenFileName(this,
                                                                 tr("Import IPCD File"),
                                                                 m_strScdFileDir,
                                                                 tr("IED Physical Configuration Description File (*.ipcd)"));

        if(!strFileName.isEmpty())
        {
            qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
            bool bResult = Core::ConfigCoreOperation::Instance()->ImportIpcd(lstDevices, strFileName);
            qApp->restoreOverrideCursor();

            if(bResult)
                QMessageBox::information(this, tr("Export Success"), tr("Succeed to import IPCD file!"));
            else
                QMessageBox::critical(this, tr("Export Failure"), tr("Failed to import IPCD file!"));

            m_strScdFileDir = QFileInfo(strFileName).absolutePath();
        }
    }
    else if(pButton == m_pPushButtonAssociate)
    {
        const QString strFileName = QFileDialog::getOpenFileName(this,
                                                                 tr("Open ICD File"),
                                                                 m_strIcdFileDir,
                                                                 tr("IED Capability Description File (*.icd);;All Files (*.*)"));

        if(!strFileName.isEmpty())
        {
            m_strIcdFileDir = QFileInfo(strFileName).absolutePath();

            qApp->setOverrideCursor(QCursor(Qt::WaitCursor));

            SclParser::SCLDocument document;
            if(!document.Open(strFileName))
            {
                qApp->restoreOverrideCursor();

                QMessageBox::critical(this, tr("Error"), tr("Failed to load ICD file '%1'.").arg(strFileName));
                return;
            }

            Core::ConfigCoreOperation::Instance()->UpdateDeviceModel(&document, "", lstDevices);
            SlotCurrentProjectVersionChanged(iCurrentIndex);

            qApp->restoreOverrideCursor();
        }
    }
    else if(pButton == m_pPushButtonClear)
    {
        if(QMessageBox::question(this,
                                 tr("Clear Confirmation"),
                                 tr("Are you sure you want to clear the models for these devices?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No) == QMessageBox::Yes)
        {
            qApp->setOverrideCursor(QCursor(Qt::WaitCursor));

            Core::ConfigCoreOperation::Instance()->ClearDeviceModel(lstDevices);
            SlotCurrentProjectVersionChanged(iCurrentIndex);

            qApp->restoreOverrideCursor();
        }
    }
}

void ManageModelDlg::SlotCurrentProjectVersionChanged(int iCurrentIndex)
{
    m_pModel->removeRows(0, m_pModel->rowCount());

    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxProjectVersion->itemData(iCurrentIndex).toInt());
    if(!pProjectVersion)
        return;

    foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
    {
        if(pDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtIED)
            continue;

        const bool bHasModel = pDevice->GetHasModel();
        QFont font;
        font.setItalic(!bHasModel);
        QBrush brush = QBrush(bHasModel ? Qt::black : Qt::gray);

        QList<QStandardItem*> lstItems;

        QStandardItem *pItem = new QStandardItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pDevice));
        pItem->setFont(font);
        pItem->setForeground(brush);
        lstItems.append(pItem);

        pItem = new QStandardItem(pDevice->GetManufacture());
        pItem->setData(reinterpret_cast<int>(pDevice));
        pItem->setFont(font);
        pItem->setForeground(brush);
        lstItems.append(pItem);

        pItem = new QStandardItem(pDevice->GetType());
        pItem->setData(reinterpret_cast<int>(pDevice));
        pItem->setFont(font);
        pItem->setForeground(brush);
        lstItems.append(pItem);

        pItem = new QStandardItem(pDevice->GetConfigVersion());
        pItem->setData(reinterpret_cast<int>(pDevice));
        pItem->setFont(font);
        pItem->setForeground(brush);
        lstItems.append(pItem);

        pItem = new QStandardItem(pDevice->GetIcdFileName());
        pItem->setData(reinterpret_cast<int>(pDevice));
        pItem->setFont(font);
        pItem->setForeground(brush);
        lstItems.append(pItem);

        m_pModel->appendRow(lstItems);
    }
}

void ManageModelDlg::SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    QItemSelectionModel *pSelectionModel = m_pView->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();

    m_pPushButtonClear->setEnabled(!lstSelectedIndex.isEmpty());
    m_pPushButtonAssociate->setEnabled(!lstSelectedIndex.isEmpty());
    m_pPushButtonExportIpcd->setEnabled(lstSelectedIndex.size() == 1);
}
