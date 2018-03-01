#include <QFormLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QStandardItemModel>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "utils/waitcursor.h"
#include "utils/searchcombobox.h"
#include "utils/readonlyview.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peproject.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/pevterminal.h"
#include "projectexplorer/pevterminalconn.h"

#include "copyvconndlg.h"

using namespace Core::Internal;

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
        m_pComboBox->addItem(pProjectObject->GetDisplayIcon(), pProjectObject->GetDisplayName(), reinterpret_cast<int>(pProjectObject));

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
// CopyVConnDlg member functions
///////////////////////////////////////////////////////////////////////
CopyVConnDlg::CopyVConnDlg(QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Copy Connection"));

    m_pComboBoxSrcProject = new QComboBox(this);
    m_pComboBoxSrcProject->setObjectName(tr("Source Project"));
    m_pComboBoxSrcProject->setEditable(false);
    m_pComboBoxDstProject = new QComboBox(this);
    m_pComboBoxDstProject->setObjectName(tr("Destination Project"));
    m_pComboBoxDstProject->setEditable(false);

    m_pComboBoxSrcIed = new Utils::SearchComboBox(this);
    m_pComboBoxSrcIed->setObjectName(tr("Source IED"));
    m_pComboBoxDstIed = new Utils::SearchComboBox(this);
    m_pComboBoxDstIed->setObjectName(tr("Destination IED"));

    foreach(ProjectExplorer::PeProject *pProject, ProjectExplorer::ProjectManager::Instance()->GetAllProjects())
    {
        foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, pProject->GetAllProjectVersions())
        {
            if(pProjectVersion->IsOpend())
            {
                m_pComboBoxSrcProject->addItem(pProjectVersion->GetDisplayIcon(), pProjectVersion->GetDisplayName(), reinterpret_cast<int>(pProjectVersion));
                m_pComboBoxDstProject->addItem(pProjectVersion->GetDisplayIcon(), pProjectVersion->GetDisplayName(), reinterpret_cast<int>(pProjectVersion));
            }
        }
    }

    m_pModel = new QStandardItemModel(0, 6, this);
    m_pModel->setHorizontalHeaderLabels(QStringList() << tr("Source Current IED")
                                                      << tr("Direction")
                                                      << tr("Source Side IED")
                                                      << tr("Destination Current IED")
                                                      << tr("Direction")
                                                      << tr("Destination Side IED"));

    m_pView = new Utils::ReadOnlyTreeView(this);
    m_pView->setMinimumSize(1200, 400);
    m_pView->setIndentation(m_pView->indentation() * 8/10);
    m_pView->setUniformRowHeights(true);
    m_pView->setTextElideMode(Qt::ElideNone);
    m_pView->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_pView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pView->setModel(m_pModel);
    m_pView->setColumnWidth(0, 250);
    m_pView->setColumnWidth(1, 50);
    m_pView->setColumnWidth(2, 250);
    m_pView->setColumnWidth(3, 250);
    m_pView->setColumnWidth(4, 50);
    m_pView->setColumnWidth(5, 250);

    QGroupBox *pGroupBoxSrc = new QGroupBox(this);
    QFormLayout *pFormLayoutSrc = new QFormLayout(pGroupBoxSrc);
    pFormLayoutSrc->addRow(new QLabel(m_pComboBoxSrcProject->objectName() + ":", this), m_pComboBoxSrcProject);
    pFormLayoutSrc->addRow(new QLabel(m_pComboBoxSrcIed->objectName() + ":", this), m_pComboBoxSrcIed);

    QGroupBox *pGroupBoxDst = new QGroupBox(this);
    QFormLayout *pFormLayoutDst = new QFormLayout(pGroupBoxDst);
    pFormLayoutDst->addRow(new QLabel(m_pComboBoxDstProject->objectName() + ":", this), m_pComboBoxDstProject);
    pFormLayoutDst->addRow(new QLabel(m_pComboBoxDstIed->objectName() + ":", this), m_pComboBoxDstIed);

    QHBoxLayout *pHBoxLayout = new QHBoxLayout;
    pHBoxLayout->addWidget(pGroupBoxSrc);
    pHBoxLayout->addSpacing(5);
    pHBoxLayout->addWidget(pGroupBoxDst);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pVBoxLayout = GetClientLayout();
    pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    pVBoxLayout->addLayout(pHBoxLayout);
    pVBoxLayout->addSpacing(10);
    pVBoxLayout->addWidget(m_pView);
    pVBoxLayout->addSpacing(15);
    pVBoxLayout->addWidget(pDialogButtonBox);

    connect(m_pComboBoxSrcProject, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotSrcProjectChanged(int)));
    connect(m_pComboBoxDstProject, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotDstProjectChanged(int)));
    connect(m_pComboBoxSrcIed, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotSrcIedChanged(int)));
    connect(m_pComboBoxDstIed, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotDstIedChanged(int)));
    connect(m_pView, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotDoubleClicked(const QModelIndex&)));
    connect(m_pModel, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(SlotItemChanged(QStandardItem*)));

    SlotSrcProjectChanged(m_pComboBoxSrcProject->currentIndex());
    SlotDstProjectChanged(m_pComboBoxDstProject->currentIndex());
}

CopyVConnDlg::~CopyVConnDlg()
{
}

QSize CopyVConnDlg::sizeHint() const
{
    return QSize(1050, 600);
}

void CopyVConnDlg::accept()
{
    ProjectExplorer::PeDevice *pSrcCurrentIed = 0;
    int iCurrentIndex = m_pComboBoxSrcIed->currentIndex();
    if(iCurrentIndex >= 0)
        pSrcCurrentIed = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSrcIed->itemData(iCurrentIndex).toInt());
    if(!pSrcCurrentIed)
    {
        QMessageBox::critical(this, tr("Error"), tr("No %1 Specified").arg(m_pComboBoxSrcIed->objectName()));
        m_pComboBoxSrcIed->setFocus();

        return;
    }

    ProjectExplorer::PeDevice *pDstCurrentIed = 0;
    iCurrentIndex = m_pComboBoxDstIed->currentIndex();
    if(iCurrentIndex >= 0)
        pDstCurrentIed = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxDstIed->itemData(iCurrentIndex).toInt());
    if(!pDstCurrentIed)
    {
        QMessageBox::critical(this, tr("Error"), tr("No %1 Specified").arg(m_pComboBoxDstIed->objectName()));
        m_pComboBoxDstIed->setFocus();

        return;
    }

    if(pSrcCurrentIed == pDstCurrentIed)
    {
        QMessageBox::critical(this, tr("Error"), tr("%1 and %2 can NOT be the same").arg(m_pComboBoxSrcIed->objectName()).arg(m_pComboBoxDstIed->objectName()));
        m_pComboBoxDstIed->setFocus();

        return;
    }

    ProjectExplorer::PeProjectVersion *pProjectVersion = pDstCurrentIed->GetProjectVersion();
    if(!pProjectVersion)
        return;

    QList<ProjectExplorer::PeDevice*> lstDstSideIeds;
    QList<ProjectExplorer::PeVTerminalConn> lstCreatedVTerminalConns;
    QMap<ProjectExplorer::PeVTerminal*, QStandardItem*> mapRxVTerminalToItem;
    for(int i = 0; i < m_pModel->rowCount(); i++)
    {
        if(m_pModel->item(i, 0)->checkState() == Qt::Unchecked)
            continue;

        QStandardItem *pItemDstCurrentIed = m_pModel->item(i, 3);
        ProjectExplorer::PeDevice *pDstCurrentIed = reinterpret_cast<ProjectExplorer::PeDevice*>(pItemDstCurrentIed->data().toInt());

        QStandardItem *pItemDstSideIed = m_pModel->item(i, 5);
        ProjectExplorer::PeDevice *pDstSideIed  = reinterpret_cast<ProjectExplorer::PeDevice*>(pItemDstSideIed->data().toInt());

        if(!pDstSideIed)
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 NOT specified").arg(m_pModel->horizontalHeaderItem(5)->text()));
            m_pView->selectionModel()->select(pItemDstSideIed->index(), QItemSelectionModel::Select);
            m_pView->scrollTo(pItemDstSideIed->index());

            return;
        }

        if(pDstCurrentIed == pDstSideIed)
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 and %2 can NOT be the same").arg(m_pModel->horizontalHeaderItem(3)->text()).arg(m_pModel->horizontalHeaderItem(5)->text()));
            m_pView->selectionModel()->select(pItemDstCurrentIed->index(), QItemSelectionModel::Select);
            m_pView->selectionModel()->select(pItemDstSideIed->index(), QItemSelectionModel::Select);
            m_pView->scrollTo(pItemDstSideIed->index());

            return;
        }

        if(lstDstSideIeds.contains(pDstSideIed))
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 can NOT be duplicate").arg(m_pModel->horizontalHeaderItem(5)->text()));
            m_pView->selectionModel()->select(pItemDstSideIed->index(), QItemSelectionModel::Select);
            m_pView->scrollTo(pItemDstSideIed->index());

            return;
        }
        lstDstSideIeds.append(pDstSideIed);

        QStandardItem *pItemSrcCurrentIed = m_pModel->item(i, 0);
        for(int j = 0; j < pItemSrcCurrentIed->rowCount(); j++)
        {
            if(pItemSrcCurrentIed->child(j, 0)->checkState() == Qt::Unchecked)
                continue;

            QStandardItem *pItemDstCurrentVTerminal = pItemSrcCurrentIed->child(j, 3);
            ProjectExplorer::PeVTerminal *pDstCurrentVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItemDstCurrentVTerminal->data().toInt());
            if(!pDstCurrentVTerminal)
            {
                QMessageBox::critical(this, tr("Error"), tr("%1 NOT specified").arg(tr("Destination Current Signal")));
                m_pView->selectionModel()->select(pItemDstCurrentVTerminal->index(), QItemSelectionModel::Select);
                m_pView->scrollTo(pItemDstCurrentVTerminal->index());

                return;
            }

            QStandardItem *pItemDstSideVTerminal = pItemSrcCurrentIed->child(j, 5);
            ProjectExplorer::PeVTerminal *pDstSideVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItemDstSideVTerminal->data().toInt());
            if(!pDstSideVTerminal)
            {
                QMessageBox::critical(this, tr("Error"), tr("%1 NOT specified").arg(tr("Destination Side Signal")));
                m_pView->selectionModel()->select(pItemDstSideVTerminal->index(), QItemSelectionModel::Select);
                m_pView->scrollTo(pItemDstSideVTerminal->index());

                return;
            }

            QStandardItem *pItemSrcDirection = pItemSrcCurrentIed->child(j, 1);
            ProjectExplorer::PeVTerminalConn *pSrcVTerminalConn = reinterpret_cast<ProjectExplorer::PeVTerminalConn*>(pItemSrcDirection->data().toInt());

            ProjectExplorer::PeVTerminalConn VTerminalConn(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            VTerminalConn.SetStraight(pSrcVTerminalConn->GetStraight());
            if(pDstCurrentVTerminal->GetDirection() == ProjectExplorer::PeVTerminal::tdOut)
            {
                VTerminalConn.SetTxVTerminal(pDstCurrentVTerminal);
                VTerminalConn.SetRxVTerminal(pDstSideVTerminal);

                if(mapRxVTerminalToItem.contains(pDstSideVTerminal))
                {
                    QMessageBox::critical(this, tr("Error"), tr("%1 can NOT be duplicate").arg(tr("Destination Side Signal")));
                    m_pView->selectionModel()->select(pItemDstSideVTerminal->index(), QItemSelectionModel::Select);
                    m_pView->selectionModel()->select(mapRxVTerminalToItem.value(pDstSideVTerminal)->index(), QItemSelectionModel::Select);
                    m_pView->scrollTo(pItemDstSideVTerminal->index());

                    return;
                }
                {
                    mapRxVTerminalToItem.insert(pDstSideVTerminal, pItemDstSideVTerminal);
                }
            }
            else
            {
                VTerminalConn.SetTxVTerminal(pDstSideVTerminal);
                VTerminalConn.SetRxVTerminal(pDstCurrentVTerminal);

                if(mapRxVTerminalToItem.contains(pDstCurrentVTerminal))
                {
                    QMessageBox::critical(this, tr("Error"), tr("%1 can NOT be duplicate").arg(tr("Destination Current Signal")));
                    m_pView->selectionModel()->select(pItemDstCurrentVTerminal->index(), QItemSelectionModel::Select);
                    m_pView->selectionModel()->select(mapRxVTerminalToItem.value(pDstCurrentVTerminal)->index(), QItemSelectionModel::Select);
                    m_pView->scrollTo(pItemDstCurrentVTerminal->index());

                    return;
                }
                else
                {
                    mapRxVTerminalToItem.insert(pDstCurrentVTerminal, pItemDstCurrentVTerminal);
                }
            }
            lstCreatedVTerminalConns.append(VTerminalConn);
        }
    }

    if(lstCreatedVTerminalConns.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No copy connections specified"));
        return;
    }

    QList<ProjectExplorer::PeVTerminalConn*> lstDeletedVTerminalConns;
    QList<ProjectExplorer::PeVTerminalConn*> lstRestVTerminalConns;
    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, pProjectVersion->GetAllVTerminalConns())
    {
        ProjectExplorer::PeVTerminal *pTxVTerminal = pVTerminalConn->GetTxVTerminal();
        ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();

        if(pTxVTerminal->GetParentDevice() == pDstCurrentIed && lstDstSideIeds.contains(pRxVTerminal->GetParentDevice()) ||
           pRxVTerminal->GetParentDevice() == pDstCurrentIed && lstDstSideIeds.contains(pTxVTerminal->GetParentDevice()))
        {
            lstDeletedVTerminalConns.append(pVTerminalConn);
        }
        else
        {
            lstRestVTerminalConns.append(pVTerminalConn);
        }
    }

    foreach(ProjectExplorer::PeVTerminal *pVTerminal, mapRxVTerminalToItem.keys())
    {
        foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, lstRestVTerminalConns)
        {
            if(pVTerminalConn->GetRxVTerminal() == pVTerminal)
            {
                QMessageBox::critical(this, tr("Error"), tr("The signal '%1' has been used").arg(pVTerminal->GetDisplayName()));
                m_pView->selectionModel()->select(mapRxVTerminalToItem.value(pVTerminal)->index(), QItemSelectionModel::Select);
                m_pView->scrollTo(mapRxVTerminalToItem.value(pVTerminal)->index());

                return;
            }
        }
    }

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    ///////////////////////////////////////////////////////////////////////
    // Handle Database
    ///////////////////////////////////////////////////////////////////////
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);

    // Delete VTerminalConn
    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, lstDeletedVTerminalConns)
    {
        if(!pProjectVersion->DbDeleteObject(pVTerminalConn->GetObjectType(), pVTerminalConn->GetId(), false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    // Create VTerminalConn
    for(int i = 0; i < lstCreatedVTerminalConns.size(); i++)
    {
        ProjectExplorer::PeVTerminalConn &VTerminalConn = lstCreatedVTerminalConns[i];
        if(!pProjectVersion->DbCreateObject(VTerminalConn, false))
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

    // Delete VTerminalConn
    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, lstDeletedVTerminalConns)
        pProjectVersion->DeleteObject(pVTerminalConn);

    // Create VTerminalConn
    foreach(const ProjectExplorer::PeVTerminalConn &VTerminalConn, lstCreatedVTerminalConns)
        pProjectVersion->CreateObject(VTerminalConn);

    QDialog::accept();
}

void CopyVConnDlg::SlotSrcProjectChanged(int iCurrentIndex)
{
    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxSrcProject->itemData(iCurrentIndex).toInt());
    if(!pProjectVersion)
        return;

    m_pComboBoxSrcIed->blockSignals(true);

    m_pComboBoxSrcIed->clear();
    QList<ProjectExplorer::PeDevice*> lstDevices;
    foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
    {
        if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
            lstDevices.append(pDevice);
    }
    qSort(lstDevices.begin(), lstDevices.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    foreach(ProjectExplorer::PeDevice *pDevice, lstDevices)
        m_pComboBoxSrcIed->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));

    m_pComboBoxSrcIed->blockSignals(false);

    SlotSrcIedChanged(m_pComboBoxSrcProject->currentIndex());
}

void CopyVConnDlg::SlotDstProjectChanged(int iCurrentIndex)
{
    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxDstProject->itemData(iCurrentIndex).toInt());
    if(!pProjectVersion)
        return;

    m_pComboBoxDstIed->blockSignals(true);

    m_pComboBoxDstIed->clear();
    QList<ProjectExplorer::PeDevice*> lstDevices;
    foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
    {
        if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
            lstDevices.append(pDevice);
    }
    qSort(lstDevices.begin(), lstDevices.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    foreach(ProjectExplorer::PeDevice *pDevice, lstDevices)
        m_pComboBoxDstIed->addItem(pDevice->GetDisplayIcon(), pDevice->GetDisplayName(), reinterpret_cast<int>(pDevice));

    m_pComboBoxDstIed->blockSignals(false);

    SlotDstIedChanged(m_pComboBoxDstProject->currentIndex());
}

void CopyVConnDlg::SlotSrcIedChanged(int iCurrentIndex)
{
    m_pModel->removeRows(0, m_pModel->rowCount());

    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeDevice *pSrcCurrentIed = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxSrcIed->itemData(iCurrentIndex).toInt());
    if(!pSrcCurrentIed)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pSrcCurrentIed->GetProjectVersion();
    if(!pProjectVersion)
        return;

    QMap<ProjectExplorer::PeDevice*, QList<ProjectExplorer::PeVTerminalConn*> > mapSideDeviceToVTerminalConns;
    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, pProjectVersion->GetAllVTerminalConns())
    {
        ProjectExplorer::PeVTerminal *pTxVTerminal = pVTerminalConn->GetTxVTerminal();
        ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();
        if(!pTxVTerminal || !pRxVTerminal)
            continue;

        ProjectExplorer::PeDevice *pTxIed = pTxVTerminal->GetParentDevice();
        ProjectExplorer::PeDevice *pRxIed = pRxVTerminal->GetParentDevice();
        if(!pTxIed || !pRxIed)
            continue;

        if(pSrcCurrentIed == pTxIed)
        {
            QList<ProjectExplorer::PeVTerminalConn*> &lstVTerminalConns = mapSideDeviceToVTerminalConns[pRxIed];
            lstVTerminalConns.append(pVTerminalConn);
        }
        else if(pSrcCurrentIed == pRxIed)
        {
            QList<ProjectExplorer::PeVTerminalConn*> &lstVTerminalConns = mapSideDeviceToVTerminalConns[pTxIed];
            lstVTerminalConns.append(pVTerminalConn);
        }
    }

    foreach(ProjectExplorer::PeDevice *pSrcSideIed, mapSideDeviceToVTerminalConns.keys())
    {
        QList<QStandardItem*> lstIedItems;

        QStandardItem *pItemSrcCurrentIed = new QStandardItem(pSrcCurrentIed->GetDisplayIcon(), pSrcCurrentIed->GetDisplayName());
        pItemSrcCurrentIed->setData(reinterpret_cast<int>(pSrcCurrentIed));
        pItemSrcCurrentIed->setCheckable(true);
        pItemSrcCurrentIed->setCheckState(Qt::Checked);
        lstIedItems.append(pItemSrcCurrentIed);

        lstIedItems.append(new QStandardItem);

        QStandardItem *pSrcItemSideIed = new QStandardItem(pSrcSideIed->GetDisplayIcon(), pSrcSideIed->GetDisplayName());
        pSrcItemSideIed->setData(reinterpret_cast<int>(pSrcSideIed));
        lstIedItems.append(pSrcItemSideIed);

        lstIedItems.append(new QStandardItem);
        lstIedItems.append(new QStandardItem);
        lstIedItems.append(new QStandardItem);

        foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, mapSideDeviceToVTerminalConns.value(pSrcSideIed))
        {
            ProjectExplorer::PeVTerminal *pTxVTerminal = pVTerminalConn->GetTxVTerminal();
            ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();

            ProjectExplorer::PeVTerminal *pCurrentVTerminal = 0, *pSideVTerminal = 0;
            QString strDirection;

            if(pTxVTerminal->GetParentDevice() == pSrcCurrentIed)
            {
                pCurrentVTerminal = pTxVTerminal;
                pSideVTerminal = pRxVTerminal;
                strDirection = ">>>";
            }
            else
            {
                pCurrentVTerminal = pRxVTerminal;
                pSideVTerminal = pTxVTerminal;
                strDirection = "<<<";
            }

            QList<QStandardItem*> lstConnItems;

            QStandardItem *pItemCurrentVTerminal = new QStandardItem(pCurrentVTerminal->GetDisplayIcon(), pCurrentVTerminal->GetDisplayName());
            pItemCurrentVTerminal->setData(reinterpret_cast<int>(pCurrentVTerminal));
            pItemCurrentVTerminal->setCheckable(true);
            pItemCurrentVTerminal->setCheckState(Qt::Checked);
            lstConnItems.append(pItemCurrentVTerminal);

            QStandardItem *pItemDirection = new QStandardItem(strDirection);
            pItemDirection->setData(reinterpret_cast<int>(pVTerminalConn));
            pItemDirection->setTextAlignment(Qt::AlignCenter);
            lstConnItems.append(pItemDirection);

            QStandardItem *pItemSideVTerminal = new QStandardItem(pSideVTerminal->GetDisplayIcon(), pSideVTerminal->GetDisplayName());
            pItemSideVTerminal->setData(reinterpret_cast<int>(pSideVTerminal));
            lstConnItems.append(pItemSideVTerminal);

            lstConnItems.append(new QStandardItem);

            pItemDirection = new QStandardItem(strDirection);
            pItemDirection->setTextAlignment(Qt::AlignCenter);
            lstConnItems.append(pItemDirection);

            lstConnItems.append(new QStandardItem);

            pItemSrcCurrentIed->appendRow(lstConnItems);
        }

        m_pModel->appendRow(lstIedItems);
    }

    SlotDstIedChanged(m_pComboBoxDstIed->currentIndex());
}

void CopyVConnDlg::SlotDstIedChanged(int iCurrentIndex)
{
    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeDevice *pDstCurrentIed = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxDstIed->itemData(iCurrentIndex).toInt());
    if(!pDstCurrentIed)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pDstCurrentIed->GetProjectVersion();
    if(!pProjectVersion)
        return;

    for(int i = 0; i < m_pModel->rowCount(); i++)
    {
        QStandardItem *pItemDstCurrentIed = m_pModel->item(i, 3);
        pItemDstCurrentIed->setIcon(pDstCurrentIed->GetDisplayIcon());
        pItemDstCurrentIed->setText(pDstCurrentIed->GetDisplayName());

        QStandardItem *pItemDstSideIed = m_pModel->item(i, 5);
        pItemDstSideIed->setIcon(QIcon());
        pItemDstSideIed->setText(tr("No IED Specified"));
        pItemDstSideIed->setForeground(Qt::red);
        pItemDstSideIed->setData(0);

        QMap<QString, ProjectExplorer::PeVTerminal*> mapRefNameToVTerminal;
        foreach(ProjectExplorer::PeVTerminal *pVTerminal, pDstCurrentIed->GetVTerminals())
            mapRefNameToVTerminal.insert(pVTerminal->GetRefName(), pVTerminal);

        QStandardItem *pItemSrcCurrentIed = m_pModel->item(i, 0);
        for(int j = 0; j < pItemSrcCurrentIed->rowCount(); j++)
        {
            QStandardItem *pItemSrcCurrentVTerminal = pItemSrcCurrentIed->child(j, 0);
            ProjectExplorer::PeVTerminal *pSrcCurrentVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItemSrcCurrentVTerminal->data().toInt());

            ProjectExplorer::PeVTerminal *pDstCurrentVTerminal = mapRefNameToVTerminal.value(pSrcCurrentVTerminal->GetRefName(), 0);
            QStandardItem *pItemDstCurrentVTerminal = pItemSrcCurrentIed->child(j, 3);
            pItemDstCurrentVTerminal->setIcon(pDstCurrentVTerminal ? pDstCurrentVTerminal->GetDisplayIcon() : QIcon());
            pItemDstCurrentVTerminal->setText(pDstCurrentVTerminal ? pDstCurrentVTerminal->GetDisplayName() : tr("No Matched Signal"));
            pItemDstCurrentVTerminal->setForeground(pDstCurrentVTerminal ? Qt::black : Qt::red);
            pItemDstCurrentVTerminal->setData(reinterpret_cast<int>(pDstCurrentVTerminal));

            pItemSrcCurrentIed->child(j, 5)->setIcon(QIcon());
            pItemSrcCurrentIed->child(j, 5)->setText(tr("No Signal Specified"));
            pItemSrcCurrentIed->child(j, 5)->setData(0);
            pItemSrcCurrentIed->child(j, 5)->setForeground(Qt::red);
        }
    }
}

void CopyVConnDlg::SlotDoubleClicked(const QModelIndex &index)
{
    int iCurrentIndex = m_pComboBoxDstProject->currentIndex();
    if(iCurrentIndex < 0)
        return;
    ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxDstProject->itemData(iCurrentIndex).toInt());
    if(!pProjectVersion)
        return;

    iCurrentIndex = m_pComboBoxDstIed->currentIndex();
    if(iCurrentIndex < 0)
        return;
    ProjectExplorer::PeDevice *pCurrentIed = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxDstIed->itemData(iCurrentIndex).toInt());
    if(!pCurrentIed)
        return;

    QStandardItem *pItem = m_pModel->itemFromIndex(index);
    if(pItem->column() != 5)
        return;

    if(!pItem->parent()) // IED
    {
        QList<ProjectExplorer::PeProjectObject*> lstDevices;
        for(int i = 0; i < m_pComboBoxDstIed->count(); i++)
        {
            if(ProjectExplorer::PeDevice *pDevice = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxDstIed->itemData(i).toInt()))
            {
                if(pDevice != pCurrentIed)
                    lstDevices.append(pDevice);
            }
        }

        ProjectExplorer::PeDevice *pDstSideIed = reinterpret_cast<ProjectExplorer::PeDevice*>(pItem->data().toInt());
        SelectProjectObjectDlg dlg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otDevice),
                                   lstDevices,
                                   pDstSideIed,
                                   this);
        if(dlg.exec() != QDialog::Accepted)
            return;

        pDstSideIed = qobject_cast<ProjectExplorer::PeDevice*>(dlg.GetSelectedProjectObject());
        if(!pDstSideIed)
            return;

        pItem->setIcon(pDstSideIed->GetDisplayIcon());
        pItem->setText(pDstSideIed->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pDstSideIed));
        pItem->setForeground(Qt::black);

        QMap<QString, ProjectExplorer::PeVTerminal*> mapRefNameToVTerminal;
        foreach(ProjectExplorer::PeVTerminal *pVTerminal, pDstSideIed->GetVTerminals())
            mapRefNameToVTerminal.insert(pVTerminal->GetRefName(), pVTerminal);

        QStandardItem *pItemSrcCurrentIed = m_pModel->item(pItem->row(), 0);
        for(int i = 0; i < pItemSrcCurrentIed->rowCount(); i++)
        {
            QStandardItem *pItemSrcSideVTerminal = pItemSrcCurrentIed->child(i, 2);
            ProjectExplorer::PeVTerminal *pSrcSideVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItemSrcSideVTerminal->data().toInt());

            ProjectExplorer::PeVTerminal *pDstSideVTerminal = mapRefNameToVTerminal.value(pSrcSideVTerminal->GetRefName(), 0);
            QStandardItem *pItemDstSideVTerminal = pItemSrcCurrentIed->child(i, 5);
            pItemDstSideVTerminal->setIcon(pDstSideVTerminal ? pDstSideVTerminal->GetDisplayIcon() : QIcon());
            pItemDstSideVTerminal->setText(pDstSideVTerminal ? pDstSideVTerminal->GetDisplayName() : tr("No Matched Signal"));
            pItemDstSideVTerminal->setForeground(pDstSideVTerminal ? Qt::black : Qt::red);
            pItemDstSideVTerminal->setData(reinterpret_cast<int>(pDstSideVTerminal));
        }
    }
    else // VTerminal
    {
        QStandardItem *pItemSrcCurrentIed = pItem->parent();
        QStandardItem *pItemSrcSideVTerminal = pItemSrcCurrentIed->child(pItem->row(), 2);
        ProjectExplorer::PeVTerminal *pSrcSideVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItemSrcSideVTerminal->data().toInt());

        QStandardItem *pItemDstSideIed = m_pModel->item(pItemSrcCurrentIed->row(), 5);
        ProjectExplorer::PeDevice *pDstSideIed = reinterpret_cast<ProjectExplorer::PeDevice*>(pItemDstSideIed->data().toInt());
        if(!pDstSideIed)
            return;
        QStandardItem *pItemDstSideVTerminal = pItemSrcCurrentIed->child(pItem->row(), 5);
        ProjectExplorer::PeVTerminal *pDstSideVTerminal = reinterpret_cast<ProjectExplorer::PeVTerminal*>(pItemDstSideVTerminal->data().toInt());

        QList<ProjectExplorer::PeProjectObject*> lstVTerminals;
        foreach(ProjectExplorer::PeVTerminal *pVTerminal, pDstSideIed->GetVTerminals())
        {
            if(pVTerminal->GetType() == pSrcSideVTerminal->GetType() &&
               pVTerminal->GetDirection() == pSrcSideVTerminal->GetDirection() &&
               pVTerminal->GetDAName().isEmpty() == pSrcSideVTerminal->GetDAName().isEmpty())
            {
                lstVTerminals.append(pVTerminal);
            }
        }
        qSort(lstVTerminals.begin(), lstVTerminals.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

        SelectProjectObjectDlg dlg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otVTerminal),
                                   lstVTerminals,
                                   pDstSideVTerminal,
                                   this);
        if(dlg.exec() != QDialog::Accepted)
            return;

        pDstSideVTerminal = qobject_cast<ProjectExplorer::PeVTerminal*>(dlg.GetSelectedProjectObject());
        if(!pDstSideVTerminal)
            return;

        pItem->setIcon(pDstSideVTerminal->GetDisplayIcon());
        pItem->setText(pDstSideVTerminal->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pDstSideVTerminal));
        pItem->setForeground(Qt::black);
    }
}

void CopyVConnDlg::SlotItemChanged(QStandardItem *pItem)
{
    if(!pItem)
        return;

    pItem = m_pModel->itemFromIndex(m_pModel->index(pItem->index().row(), 0, pItem->index().parent()));
    if(!pItem)
        return;

    if(!pItem->parent()) // IED
    {
        if(pItem->checkState() == Qt::Checked)
        {
            for(int i = 0; i < pItem->rowCount(); i++)
            {
                pItem->child(i, 0)->setCheckState(Qt::Checked);

                for(int j = 0; j < m_pModel->columnCount(); j++)
                {
                    if(QStandardItem *pItemColumn = pItem->child(i, j))
                    {
                        if(pItemColumn->foreground() != Qt::red)
                            pItemColumn->setForeground(Qt::black);
                    }
                }
            }

            for(int i = 0; i < m_pModel->columnCount(); i++)
            {
                if(QStandardItem *pItemColumn = m_pModel->item(pItem->row(), i))
                {
                    if(pItemColumn->foreground() != Qt::red)
                        pItemColumn->setForeground(Qt::black);
                }
            }

        }
        else if(pItem->checkState() == Qt::Unchecked)
        {
            for(int i = 0; i < pItem->rowCount(); i++)
            {
                pItem->child(i, 0)->setCheckState(Qt::Unchecked);

                for(int j = 0; j < m_pModel->columnCount(); j++)
                {
                    if(QStandardItem *pItemColumn = pItem->child(i, j))
                    {
                        if(pItemColumn->foreground() != Qt::red)
                            pItemColumn->setForeground(Qt::gray);
                    }
                }
            }

            for(int i = 0; i < m_pModel->columnCount(); i++)
            {
                if(QStandardItem *pItemColumn = m_pModel->item(pItem->row(), i))
                {
                    if(pItemColumn->foreground() != Qt::red)
                        pItemColumn->setForeground(Qt::gray);
                }
            }
        }
    }
    else
    {
        QStandardItem *pItemParent = pItem->parent();
        for(int i = 0; i < m_pModel->columnCount(); i++)
        {
            if(QStandardItem *pItemColumn = pItemParent->child(pItem->row(), i))
            {
                if(pItemColumn->foreground() != Qt::red)
                    pItemColumn->setForeground(pItem->checkState() == Qt::Checked ? Qt::black : Qt::gray);
            }
        }

        bool bChecked = true, bUnchecked = true;
        for(int i = 0; i < pItemParent->rowCount(); i++)
        {
            bChecked = (bChecked && pItemParent->child(i, 0)->checkState() == Qt::Checked);
            bUnchecked = (bUnchecked && pItemParent->child(i, 0)->checkState() == Qt::Unchecked);

            if(!bChecked && !bUnchecked)
                break;
        }

        if(bChecked)
            pItemParent->setCheckState(Qt::Checked);
        else if(bUnchecked)
            pItemParent->setCheckState(Qt::Unchecked);
        else
            pItemParent->setCheckState(Qt::PartiallyChecked);
    }
}
