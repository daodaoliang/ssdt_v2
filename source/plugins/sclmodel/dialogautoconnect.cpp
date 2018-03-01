#include <QApplication>
#include <QGroupBox>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QFileIconProvider>
#include <QDir>

#include "utils/waitcursor.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/pevterminal.h"
#include "projectexplorer/pevterminalconn.h"
#include "core/mainwindow.h"
#include "vlinkparse/vlinkdata.h"

#include "dialogautoconnect.h"

using namespace SclModel::Internal;

DialogAutoConnect::DialogAutoConnect(ProjectExplorer::PeDevice *pDeviceTx, ProjectExplorer::PeDevice *pDeviceRx, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pVTemplate(new VLink_Template), m_pDeviceTx(pDeviceTx), m_pDeviceRx(pDeviceRx)
{
    SetWindowTitle(tr("Auto Connect"));

    m_pListWidgetRxIed = new QListWidget(this);
    m_pListWidgetRxIed->setMinimumHeight(250);

    m_pTreeWidgetTxIed = new QTreeWidget(this);
    m_pTreeWidgetTxIed->setMinimumHeight(250);
    m_pTreeWidgetTxIed->setHeaderHidden(true);

    m_pTableWidgetVLink = new QTableWidget(0, 4, this);
    m_pTableWidgetVLink->setMinimumHeight(200);
    m_pTableWidgetVLink->setAlternatingRowColors(true);
    m_pTableWidgetVLink->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pTableWidgetVLink->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pTableWidgetVLink->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pTableWidgetVLink->horizontalHeader()->setHighlightSections(false);
    m_pTableWidgetVLink->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pTableWidgetVLink->horizontalHeader()->setStretchLastSection(true);
    m_pTableWidgetVLink->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pTableWidgetVLink->verticalHeader()->hide();

    m_pTableWidgetVLink->setHorizontalHeaderLabels(QStringList() << tr("Connection Type") << tr("Connection Name") << tr("Matched Rx Signal") << tr("Matched Tx Signal"));
    m_pTableWidgetVLink->setColumnWidth(0, 80);
    m_pTableWidgetVLink->setColumnWidth(1, 150);
    m_pTableWidgetVLink->setColumnWidth(2, 300);
    m_pTableWidgetVLink->setColumnWidth(3, 300);

    QGroupBox *pGroupBoxTxIed = new QGroupBox(tr("Template Tx IED"), this);
    QVBoxLayout *pVBoxLayoutGroupBoxTxIed = new QVBoxLayout(pGroupBoxTxIed);
    pVBoxLayoutGroupBoxTxIed->addWidget(m_pTreeWidgetTxIed);

    QGroupBox *pGroupBoxRxIed = new QGroupBox(tr("Template Rx IED"), this);
    QVBoxLayout *pVBoxLayoutGroupBoxRxIed = new QVBoxLayout(pGroupBoxRxIed);
    pVBoxLayoutGroupBoxRxIed->addWidget(m_pListWidgetRxIed);

    QHBoxLayout *pHBoxLayout = new QHBoxLayout;
    pHBoxLayout->addWidget(pGroupBoxRxIed);
    pHBoxLayout->addSpacing(5);
    pHBoxLayout->addWidget(pGroupBoxTxIed);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addLayout(pHBoxLayout);
    pLayout->addSpacing(5);
    pLayout->addWidget(m_pTableWidgetVLink);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    connect(m_pListWidgetRxIed, SIGNAL(itemChanged(QListWidgetItem*)),
            this, SLOT(SlotListWidgetItemChanged(QListWidgetItem*)));
    connect(m_pTreeWidgetTxIed, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this, SLOT(SlotTreeWidgetItemChanged(QTreeWidgetItem*, int)));

    QDir dirVLinkParse(Core::MainWindow::Instance()->GetResourcePath() + QLatin1String("/vlinkparse"));
    if(dirVLinkParse.exists())
    {
        QStringList lstFilesNames;
        foreach(const QFileInfo &FileInfo, dirVLinkParse.entryInfoList(QStringList() << "*.xml", QDir::Files))
            lstFilesNames.append(FileInfo.absoluteFilePath());

        if(!lstFilesNames.isEmpty())
            m_pVTemplate->read_vlink_template_list(lstFilesNames);
    }

    FillRxIed();
}

DialogAutoConnect::~DialogAutoConnect()
{
    delete m_pVTemplate;
}

void DialogAutoConnect::FillRxIed()
{
    m_pListWidgetRxIed->clear();

    QListWidgetItem *pItemChecked = 0;
    foreach(VLink_IED *pVLinkIed, m_pVTemplate->vlink_ied_list)
    {
        QListWidgetItem *pItem = new QListWidgetItem(ProjectExplorer::PeDevice::GetDeviceIcon(ProjectExplorer::PeDevice::dtIED), pVLinkIed->desc);
        pItem->setData(Qt::UserRole, reinterpret_cast<int>(pVLinkIed));
        pItem->setCheckState(Qt::Unchecked);
        m_pListWidgetRxIed->addItem(pItem);

        if(pVLinkIed->vlink_ied_desckey_match->check_match_string(m_pDeviceRx->GetDescription()))
            pItemChecked = pItem;
    }

    if(pItemChecked)
    {
        pItemChecked->setCheckState(Qt::Checked);
        pItemChecked->setSelected(true);
        m_pListWidgetRxIed->scrollToItem(pItemChecked);
    }
}

void DialogAutoConnect::FillTxIed()
{
    m_pTreeWidgetTxIed->clear();

    VLink_IED *pVLinkIed = GetSelectedRxIed();
    if(!pVLinkIed)
        return;

    QFileIconProvider FileIconProvider;

    QTreeWidgetItem *pItemChecked = 0;
    if(!pVLinkIed->vlink_gorx_list.isEmpty())
    {
        QTreeWidgetItem *pItemGo = new QTreeWidgetItem;
        pItemGo->setIcon(0, FileIconProvider.icon(QFileIconProvider::Folder));
        pItemGo->setText(0, "GOOSE");
        m_pTreeWidgetTxIed->addTopLevelItem(pItemGo);

        foreach(VLink_TxIed *pVLinkTxIed, pVLinkIed->vlink_gorx_list)
        {
            QTreeWidgetItem *pItem = new QTreeWidgetItem;
            pItem->setIcon(0, ProjectExplorer::PeDevice::GetDeviceIcon(ProjectExplorer::PeDevice::dtIED));
            pItem->setText(0, pVLinkTxIed->desc);
            pItem->setData(0, Qt::UserRole, reinterpret_cast<int>(pVLinkTxIed));
            pItem->setCheckState(0, Qt::Unchecked);
            pItemGo->addChild(pItem);

            if(pVLinkTxIed->vlink_txied_match->check_match_string(m_pDeviceTx->GetDescription()))
                pItemChecked = pItem;
        }
    }
    if(pItemChecked)
    {
        pItemChecked->setCheckState(0, Qt::Checked);
        pItemChecked->setSelected(true);
        m_pTreeWidgetTxIed->scrollToItem(pItemChecked);
    }

    pItemChecked = 0;
    if(!pVLinkIed->vlink_svrx_list.isEmpty())
    {
        QTreeWidgetItem *pItemSv = new QTreeWidgetItem;
        pItemSv->setIcon(0, FileIconProvider.icon(QFileIconProvider::Folder));
        pItemSv->setText(0, "SV");
        m_pTreeWidgetTxIed->addTopLevelItem(pItemSv);

        foreach(VLink_TxIed *pVLinkTxIed, pVLinkIed->vlink_svrx_list)
        {
            QTreeWidgetItem *pItem = new QTreeWidgetItem;
            pItem->setIcon(0, ProjectExplorer::PeDevice::GetDeviceIcon(ProjectExplorer::PeDevice::dtIED));
            pItem->setText(0, pVLinkTxIed->desc);
            pItem->setData(0, Qt::UserRole, reinterpret_cast<int>(pVLinkTxIed));
            pItem->setCheckState(0, Qt::Unchecked);
            pItemSv->addChild(pItem);

            if(pVLinkTxIed->vlink_txied_match->check_match_string(m_pDeviceTx->GetDescription()))
                pItemChecked = pItem;
        }
    }
    if(pItemChecked)
    {
        pItemChecked->setCheckState(0, Qt::Checked);
        pItemChecked->setSelected(true);
        m_pTreeWidgetTxIed->scrollToItem(pItemChecked);
    }

    m_pTreeWidgetTxIed->expandAll();
}

void DialogAutoConnect::FillVLink()
{
    while(m_pTableWidgetVLink->rowCount() > 0)
        m_pTableWidgetVLink->removeRow(0);

    QList<ProjectExplorer::PeVTerminal*> lstTxGoVTerminals, lstTxSvVTerminals;
    foreach(ProjectExplorer::PeVTerminal *pVTerminal, m_pDeviceTx->GetVTerminals())
    {
        if(pVTerminal->GetDirection() != ProjectExplorer::PeVTerminal::tdOut)
            continue;

        if(pVTerminal->GetType() == ProjectExplorer::PeVTerminal::ttGoose && !pVTerminal->GetDAName().isEmpty())
            lstTxGoVTerminals.append(pVTerminal);
        else if(pVTerminal->GetType() == ProjectExplorer::PeVTerminal::ttSv && pVTerminal->GetDAName().isEmpty())
            lstTxSvVTerminals.append(pVTerminal);
    }

    QList<ProjectExplorer::PeVTerminal*> lstRxGoVTerminals, lstRxSvVTerminals;
    foreach(ProjectExplorer::PeVTerminal *pVTerminal, m_pDeviceRx->GetVTerminals())
    {
        if(pVTerminal->GetDirection() != ProjectExplorer::PeVTerminal::tdIn)
            continue;

        if(pVTerminal->GetType() == ProjectExplorer::PeVTerminal::ttGoose && !pVTerminal->GetDAName().isEmpty())
            lstRxGoVTerminals.append(pVTerminal);
        else if(pVTerminal->GetType() == ProjectExplorer::PeVTerminal::ttSv && pVTerminal->GetDAName().isEmpty())
            lstRxSvVTerminals.append(pVTerminal);
    }

    if(GetSelectedRxIed())
    {
        if(VLink_TxIed *pVLinkGoTxIed = GetSelectedGoTxIed())
        {
            foreach(VLink_VLink *pVlink, pVLinkGoTxIed->vlink_vlink_list)
            {
                int iRow = m_pTableWidgetVLink->rowCount();
                m_pTableWidgetVLink->insertRow(iRow);

                QTableWidgetItem *pItem = new QTableWidgetItem("GOOSE");
                pItem->setCheckState(Qt::Checked);
                pItem->setData(Qt::UserRole, reinterpret_cast<int>(pVlink));
                m_pTableWidgetVLink->setItem(iRow, 0, pItem);

                pItem = new QTableWidgetItem(pVlink->desc);
                pItem->setData(Qt::UserRole, reinterpret_cast<int>(pVlink));
                m_pTableWidgetVLink->setItem(iRow, 1, pItem);

                ProjectExplorer::PeVTerminal *pRxVTerminal = 0;
                foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstRxGoVTerminals)
                {
                    if(pVlink->vlink_rx->vlink_terminal_match->check_match_string(pVTerminal->GetProDesc()))
                    {
                        pRxVTerminal = pVTerminal;
                        break;
                    }
                }
                pItem = new QTableWidgetItem(pRxVTerminal ? pRxVTerminal->GetDisplayName() : "");
                pItem->setData(Qt::UserRole, reinterpret_cast<int>(pRxVTerminal));
                m_pTableWidgetVLink->setItem(iRow, 2, pItem);

                ProjectExplorer::PeVTerminal *pTxVTerminal = 0;
                foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstTxGoVTerminals)
                {
                    if(pVlink->vlink_tx->vlink_terminal_match->check_match_string(pVTerminal->GetProDesc()))
                    {
                        pTxVTerminal = pVTerminal;
                        break;
                    }
                }
                pItem = new QTableWidgetItem(pTxVTerminal ? pTxVTerminal->GetDisplayName() : "");
                pItem->setData(Qt::UserRole, reinterpret_cast<int>(pTxVTerminal));
                m_pTableWidgetVLink->setItem(iRow, 3, pItem);
            }
        }

        if(VLink_TxIed *pVLinkSvTxIed = GetSelectedSvTxIed())
        {
            foreach(VLink_VLink *pVlink, pVLinkSvTxIed->vlink_vlink_list)
            {
                int iRow = m_pTableWidgetVLink->rowCount();
                m_pTableWidgetVLink->insertRow(iRow);

                QTableWidgetItem *pItem = new QTableWidgetItem("SV");
                pItem->setCheckState(Qt::Checked);
                pItem->setData(Qt::UserRole, reinterpret_cast<int>(pVlink));
                m_pTableWidgetVLink->setItem(iRow, 0, pItem);

                pItem = new QTableWidgetItem(pVlink->desc);
                pItem->setData(Qt::UserRole, reinterpret_cast<int>(pVlink));
                m_pTableWidgetVLink->setItem(iRow, 1, pItem);

                ProjectExplorer::PeVTerminal *pRxVTerminal = 0;
                foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstRxSvVTerminals)
                {
                    if(pVlink->vlink_rx->vlink_terminal_match->check_match_string(pVTerminal->GetProDesc()))
                    {
                        pRxVTerminal = pVTerminal;
                        break;
                    }
                }
                pItem = new QTableWidgetItem(pRxVTerminal ? pRxVTerminal->GetDisplayName() : "");
                pItem->setData(Qt::UserRole, reinterpret_cast<int>(pRxVTerminal));
                m_pTableWidgetVLink->setItem(iRow, 2, pItem);

                ProjectExplorer::PeVTerminal *pTxVTerminal = 0;
                foreach(ProjectExplorer::PeVTerminal *pVTerminal, lstTxSvVTerminals)
                {
                    if(pVlink->vlink_tx->vlink_terminal_match->check_match_string(pVTerminal->GetProDesc()))
                    {
                        pTxVTerminal = pVTerminal;
                        break;
                    }
                }
                pItem = new QTableWidgetItem(pTxVTerminal ? pTxVTerminal->GetDisplayName() : "");
                pItem->setData(Qt::UserRole, reinterpret_cast<int>(pTxVTerminal));
                m_pTableWidgetVLink->setItem(iRow, 3, pItem);
            }
        }
    }
}

VLink_IED* DialogAutoConnect::GetSelectedRxIed() const
{
    for(int i = 0; i < m_pListWidgetRxIed->count(); i++)
    {
        QListWidgetItem *pItem = m_pListWidgetRxIed->item(i);
        if(pItem->checkState() == Qt::Checked)
            return reinterpret_cast<VLink_IED*>(pItem->data(Qt::UserRole).toInt());
    }

    return 0;
}

VLink_TxIed* DialogAutoConnect::GetSelectedGoTxIed() const
{
    for(int i = 0; i < m_pTreeWidgetTxIed->topLevelItemCount(); i++)
    {
        if(m_pTreeWidgetTxIed->topLevelItem(i)->text(0) == "GOOSE")
        {
            QTreeWidgetItem *pItemGo = m_pTreeWidgetTxIed->topLevelItem(i);
            for(int j = 0; j < pItemGo->childCount(); j++)
            {
                QTreeWidgetItem *pItem = pItemGo->child(j);
                if(pItem->checkState(0) == Qt::Checked)
                    return reinterpret_cast<VLink_TxIed*>(pItem->data(0, Qt::UserRole).toInt());
            }

            break;
        }
    }

    return 0;
}

VLink_TxIed* DialogAutoConnect::GetSelectedSvTxIed() const
{
    for(int i = 0; i < m_pTreeWidgetTxIed->topLevelItemCount(); i++)
    {
        if(m_pTreeWidgetTxIed->topLevelItem(i)->text(0) == "SV")
        {
            QTreeWidgetItem *pItemSv = m_pTreeWidgetTxIed->topLevelItem(i);
            for(int j = 0; j < pItemSv->childCount(); j++)
            {
                QTreeWidgetItem *pItem = pItemSv->child(j);
                if(pItem->checkState(0) == Qt::Checked)
                    return reinterpret_cast<VLink_TxIed*>(pItem->data(0, Qt::UserRole).toInt());
            }

            break;
        }
    }

    return 0;
}

void DialogAutoConnect::SlotListWidgetItemChanged(QListWidgetItem *pItem)
{
    m_pListWidgetRxIed->blockSignals(true);

    if(pItem->checkState() == Qt::Checked)
    {
        for(int i = 0; i < m_pListWidgetRxIed->count(); i++)
        {
            if(m_pListWidgetRxIed->item(i) != pItem)
                m_pListWidgetRxIed->item(i)->setCheckState(Qt::Unchecked);
        }        
    }

    m_pListWidgetRxIed->blockSignals(false);

    FillTxIed();
    FillVLink();
}

void DialogAutoConnect::SlotTreeWidgetItemChanged(QTreeWidgetItem *pItem, int iColumn)
{
    if(iColumn != 0)
        return;

    m_pTreeWidgetTxIed->blockSignals(true);

    QTreeWidgetItem *pItemParent = pItem->parent();
    if(!pItemParent)
        return;

    if(pItem->checkState(0) == Qt::Checked)
    {
        for(int i = 0; i < pItemParent->childCount(); i++)
        {
            if(pItemParent->child(i) != pItem)
                pItemParent->child(i)->setCheckState(0, Qt::Unchecked);
        }
    }

    m_pTreeWidgetTxIed->blockSignals(false);

    FillVLink();
}

void DialogAutoConnect::accept()
{
    if(!m_pDeviceRx || !m_pDeviceTx)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pDeviceRx->GetProjectVersion();
    if(!pProjectVersion)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    QMap<ProjectExplorer::PeVTerminal*, QList<ProjectExplorer::PeVTerminalConn*> > mapUsedVTerminalsToVTerminalConns;
    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, pProjectVersion->GetAllVTerminalConns())
    {
        ProjectExplorer::PeVTerminal *pVTerminalRx = pVTerminalConn->GetRxVTerminal();
        if(pVTerminalRx)
        {
            QList<ProjectExplorer::PeVTerminalConn*> &lstVTerminalConns = mapUsedVTerminalsToVTerminalConns[pVTerminalRx];
            if(!lstVTerminalConns.contains(pVTerminalConn))
                lstVTerminalConns.append(pVTerminalConn);
        }
    }

    QList<ProjectExplorer::PeVTerminalConn*> lstDeletedVTerminalConns;
    QList<ProjectExplorer::PeVTerminalConn> lstCreatedVTerminalConns;
    for(int i = 0; i < m_pTableWidgetVLink->rowCount(); i++)
    {
        if(m_pTableWidgetVLink->item(i, 0)->checkState() != Qt::Checked)
            continue;

        ProjectExplorer::PeVTerminal *pVTerminalRx = reinterpret_cast<ProjectExplorer::PeVTerminal*>(m_pTableWidgetVLink->item(i, 2)->data(Qt::UserRole).toInt());
        ProjectExplorer::PeVTerminal *pVTerminalTx = reinterpret_cast<ProjectExplorer::PeVTerminal*>(m_pTableWidgetVLink->item(i, 3)->data(Qt::UserRole).toInt());
        if(!pVTerminalRx || !pVTerminalTx)
            continue;

        if(mapUsedVTerminalsToVTerminalConns.contains(pVTerminalRx))
        {
            foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, mapUsedVTerminalsToVTerminalConns.value(pVTerminalRx))
            {
                if(!lstDeletedVTerminalConns.contains(pVTerminalConn))
                    lstDeletedVTerminalConns.append(pVTerminalConn);
            }
        }

        ProjectExplorer::PeVTerminalConn VTerminalConn(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        VTerminalConn.SetTxVTerminal(pVTerminalTx);
        VTerminalConn.SetRxVTerminal(pVTerminalRx);
        lstCreatedVTerminalConns.append(VTerminalConn);
    }

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
    foreach(ProjectExplorer::PeVTerminalConn VTerminalConn, lstCreatedVTerminalConns)
        pProjectVersion->CreateObject(VTerminalConn);

    return QDialog::accept();
}
