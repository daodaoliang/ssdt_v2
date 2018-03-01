#include <QApplication>
#include <QStandardItemModel>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QTabBar>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QToolButton>
#include <QMessageBox>

#include "utils/waitcursor.h"
#include "utils/readonlyview.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pecable.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pebay.h"

#include "renamecabledlg.h"
#include "propertydlgcable.h"

using namespace PhyNetwork::Internal;

RenameCableDlg::RenameCableDlg(QWidget *pParent) :
    StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Cable Rename"));

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

    // Create modle view for optical cable
    m_pModelOptical = new QStandardItemModel(0, 5, this);
    m_pModelOptical->setHeaderData(0, Qt::Horizontal, tr("Modified Name"));
    m_pModelOptical->setHeaderData(1, Qt::Horizontal, tr("Original Name"));
    m_pModelOptical->setHeaderData(2, Qt::Horizontal, tr("Start Cubicle"));
    m_pModelOptical->setHeaderData(3, Qt::Horizontal, tr("End Cubicle"));
    m_pModelOptical->setHeaderData(4, Qt::Horizontal, tr("Fiber Number"));

    m_pViewOptical = new Utils::ReadOnlyTableView(this);
    m_pViewOptical->setAlternatingRowColors(true);
    m_pViewOptical->setSortingEnabled(true);
    m_pViewOptical->setModel(m_pModelOptical);

    m_pViewOptical->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewOptical->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pViewOptical->horizontalHeader()->setHighlightSections(false);
    m_pViewOptical->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pViewOptical->horizontalHeader()->setStretchLastSection(true);
    m_pViewOptical->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewOptical->verticalHeader()->hide();
    m_pViewOptical->setShowGrid(false);

    m_pViewOptical->setColumnWidth(0, 155);
    m_pViewOptical->setColumnWidth(1, 155);
    m_pViewOptical->setColumnWidth(2, 200);
    m_pViewOptical->setColumnWidth(3, 200);
    m_pViewOptical->setColumnWidth(4, 100);

    // Create modle view for tail cable
    m_pModelTail = new QStandardItemModel(0, 5, this);
    m_pModelTail->setHeaderData(0, Qt::Horizontal, tr("Modified Name"));
    m_pModelTail->setHeaderData(1, Qt::Horizontal, tr("Original Name"));
    m_pModelTail->setHeaderData(2, Qt::Horizontal, tr("Start Cubicle"));
    m_pModelTail->setHeaderData(3, Qt::Horizontal, tr("End Cubicle"));
    m_pModelTail->setHeaderData(4, Qt::Horizontal, tr("Fiber Number"));

    // Create view
    m_pViewTail = new Utils::ReadOnlyTableView(this);
    m_pViewTail->setAlternatingRowColors(true);
    m_pViewTail->setSortingEnabled(true);
    m_pViewTail->setModel(m_pModelTail);

    m_pViewTail->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewTail->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pViewTail->horizontalHeader()->setHighlightSections(false);
    m_pViewTail->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pViewTail->horizontalHeader()->setStretchLastSection(true);
    m_pViewTail->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewTail->verticalHeader()->hide();
    m_pViewTail->setShowGrid(false);

    m_pViewTail->setColumnWidth(0, 155);
    m_pViewTail->setColumnWidth(1, 155);
    m_pViewTail->setColumnWidth(2, 200);
    m_pViewTail->setColumnWidth(3, 200);
    m_pViewTail->setColumnWidth(4, 100);

    QTabBar *pTabBar = new QTabBar(this);
    pTabBar->setDrawBase(false);
    pTabBar->setExpanding(false);
    pTabBar->addTab(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctOptical));
    pTabBar->addTab(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctTail));

    QStackedWidget *pStackedWidget = new QStackedWidget(this);
    pStackedWidget->addWidget(m_pViewOptical);
    pStackedWidget->addWidget(m_pViewTail);

    connect(pTabBar, SIGNAL(currentChanged(int)),
            pStackedWidget, SLOT(setCurrentIndex(int)));

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addLayout(pHBoxLayoutProjectVersion);
    pLayout->addSpacing(10);
    pLayout->addWidget(pTabBar);
    pLayout->addWidget(pStackedWidget);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    connect(m_pComboBoxProjectVersion, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentProjectVersionChanged(int)));
    connect(m_pViewOptical, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewDoubleClicked(const QModelIndex&)));
    connect(m_pViewTail, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewDoubleClicked(const QModelIndex&)));

    SlotCurrentProjectVersionChanged(m_pComboBoxProjectVersion->currentIndex());
}

RenameCableDlg::~RenameCableDlg()
{
    qDeleteAll(m_lstOpticalCables);
    m_lstOpticalCables.clear();

    qDeleteAll(m_lstTailCables);
    m_lstTailCables.clear();
}

void RenameCableDlg::UpdateItemStatus(QStandardItemModel *pModel)
{
    if(!pModel)
        return;

    QMap<QString, QList<QStandardItem*> > mapNameToItems;
    for(int i = 0; i < pModel->rowCount(); i++)
    {
        QStandardItem *pItemModify = pModel->item(i, 0);
        QStandardItem *pItemOriginal = pModel->item(i, 1);

        QFont font(pItemModify->font());
        font.setBold(pItemModify->text() != pItemOriginal->text());
        pItemModify->setFont(font);

        mapNameToItems[pItemModify->text()].append(pItemModify);
    }

    foreach(const QString &strName, mapNameToItems.keys())
    {
        QList<QStandardItem*> lstItems = mapNameToItems.value(strName);

        QBrush brush(lstItems.size() > 1 ? Qt::red : Qt::black);
        foreach(QStandardItem *pItem, lstItems)
            pItem->setForeground(brush);
    }
}

void RenameCableDlg::SlotCurrentProjectVersionChanged(int iCurrentIndex)
{
    m_pModelOptical->removeRows(0, m_pModelOptical->rowCount());
    m_pModelTail->removeRows(0, m_pModelTail->rowCount());

    qDeleteAll(m_lstOpticalCables);
    m_lstOpticalCables.clear();

    qDeleteAll(m_lstTailCables);
    m_lstTailCables.clear();

    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxProjectVersion->itemData(iCurrentIndex).toInt());
    if(!pProjectVersion)
        return;

    // Build model for optical calbe
    foreach(ProjectExplorer::PeCable *pCable, pProjectVersion->GetAllCables())
    {
        if(pCable->GetCableType() == ProjectExplorer::PeCable::ctOptical)
            m_lstOpticalCables.append(new ProjectExplorer::PeCable(*pCable));
    }
    qSort(m_lstOpticalCables.begin(), m_lstOpticalCables.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    foreach(ProjectExplorer::PeCable *pCable, m_lstOpticalCables)
    {
        QList<QStandardItem*> lstItems;

        QStandardItem *pItem = new QStandardItem(pCable->GetDisplayIcon(), pCable->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pCable));
        lstItems.append(pItem);

        pItem = new QStandardItem(pCable->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pCable));
        lstItems.append(pItem);

        pItem = new QStandardItem(pCable->GetCubicle1()->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pCable));
        lstItems.append(pItem);

        pItem = new QStandardItem(pCable->GetCubicle2()->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pCable));
        lstItems.append(pItem);

        pItem = new QStandardItem(QString("%1").arg(pCable->GetChildFibers().size(), 2, 10, QLatin1Char('0')));
        pItem->setData(reinterpret_cast<int>(pCable));
        lstItems.append(pItem);

        m_pModelOptical->appendRow(lstItems);
    }

    // Build model for tail calbe
    foreach(ProjectExplorer::PeCable *pCable, pProjectVersion->GetAllCables())
    {
        if(pCable->GetCableType() == ProjectExplorer::PeCable::ctTail)
            m_lstTailCables.append(new ProjectExplorer::PeCable(*pCable));
    }
    qSort(m_lstTailCables.begin(), m_lstTailCables.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    foreach(ProjectExplorer::PeCable *pCable, m_lstTailCables)
    {
        QList<QStandardItem*> lstItems;

        QStandardItem *pItem = new QStandardItem(pCable->GetDisplayIcon(), pCable->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pCable));
        lstItems.append(pItem);

        pItem = new QStandardItem(pCable->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pCable));
        lstItems.append(pItem);

        pItem = new QStandardItem(pCable->GetCubicle1()->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pCable));
        lstItems.append(pItem);

        pItem = new QStandardItem(pCable->GetCubicle2()->GetDisplayName());
        pItem->setData(reinterpret_cast<int>(pCable));
        lstItems.append(pItem);

        pItem = new QStandardItem(QString("%1").arg(pCable->GetChildFibers().size(), 2, 10, QLatin1Char('0')));
        pItem->setData(reinterpret_cast<int>(pCable));
        lstItems.append(pItem);

        m_pModelTail->appendRow(lstItems);
    }
}

void RenameCableDlg::SlotViewDoubleClicked(const QModelIndex &index)
{
    Utils::ReadOnlyTableView *pView = qobject_cast<Utils::ReadOnlyTableView*>(sender());
    if(!pView)
        return;

    QStandardItemModel *pModel = qobject_cast<QStandardItemModel*>(pView->model());
    if(!pModel)
        return;

    QStandardItem *pItemModify = pModel->itemFromIndex(pModel->index(index.row(), 0, index.parent()));
    if(!pItemModify)
        return;

    QStandardItem *pItemOriginal = pModel->itemFromIndex(pModel->index(index.row(), 1, index.parent()));
    if(!pItemOriginal)
        return;

    ProjectExplorer::PeCable *pCable = reinterpret_cast<ProjectExplorer::PeCable*>(pItemModify->data().toInt());
    if(!pCable)
        return;

    PropertyDlgCable dlg(pCable, true, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    pItemModify->setText(pCable->GetDisplayName());

    UpdateItemStatus(pModel);
}

void RenameCableDlg::accept()
{
    int iIndex = m_pComboBoxProjectVersion->currentIndex();
    if(iIndex < 0)
        return;
        ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxProjectVersion->itemData(iIndex).toInt());
    if(!pProjectVersion)
        return;

    QList<ProjectExplorer::PeCable*> lstCables;

    QStringList lstCableNames;
    for(int i = 0; i < m_pModelOptical->rowCount(); i++)
    {
        if(ProjectExplorer::PeCable *pCable = reinterpret_cast<ProjectExplorer::PeCable*>(m_pModelOptical->item(i, 0)->data().toInt()))
        {
            if(lstCableNames.contains(pCable->GetDisplayName()))
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The name of %1 '%2' is duplicated.").arg(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctOptical)).arg(pCable->GetDisplayName()));
                return;
            }

            lstCableNames.append(pCable->GetDisplayName());
            lstCables.append(pCable);
        }
    }

    lstCableNames.clear();
    for(int i = 0; i < m_pModelTail->rowCount(); i++)
    {
        if(ProjectExplorer::PeCable *pCable = reinterpret_cast<ProjectExplorer::PeCable*>(m_pModelTail->item(i, 0)->data().toInt()))
        {
            if(lstCableNames.contains(pCable->GetDisplayName()))
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The name of %1 '%2' is duplicated.").arg(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctTail)).arg(pCable->GetDisplayName()));
                return;
            }

            lstCableNames.append(pCable->GetDisplayName());
            lstCables.append(pCable);
        }
    }

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    // Handle Database
    ProjectExplorer::DbTransaction DbTrans(pProjectVersion, true);
    foreach(ProjectExplorer::PeCable *pCable, lstCables)
    {
        if(!pProjectVersion->DbUpdateObject(*pCable, false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    if(!DbTrans.Commit())
        return;

    // Handle Ojbect
    foreach(ProjectExplorer::PeCable *pCable, lstCables)
        pProjectVersion->UpdateObject(*pCable);

    QDialog::accept();
}
