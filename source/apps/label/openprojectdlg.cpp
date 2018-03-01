#include <QApplication>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDateTime>

#include "utils/readonlyview.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peproject.h"
#include "projectexplorer/peprojectversion.h"

#include "openprojectdlg.h"

OpenProjectDlg::OpenProjectDlg(QWidget *pParent) : StyledUi::StyledDialog(pParent), m_pSelectedProjectVersion(0)
{
    SetWindowTitle(tr("Open Project"));

    // Create Model
    m_pModel = new QStandardItemModel(0, 5, this);
    m_pModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_pModel->setHeaderData(1, Qt::Horizontal, tr("Modifier"));
    m_pModel->setHeaderData(2, Qt::Horizontal, tr("Modification Time"));
    m_pModel->setHeaderData(3, Qt::Horizontal, tr("What"));
    m_pModel->setHeaderData(4, Qt::Horizontal, tr("Why"));

    QList<ProjectExplorer::PeProject*> lstProjects = ProjectExplorer::ProjectManager::Instance()->GetAllProjects();
    qSort(lstProjects.begin(), lstProjects.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    foreach(ProjectExplorer::PeProject *pProject, lstProjects)
    {
        QStandardItem *pItemProject = new QStandardItem(pProject->GetDisplayIcon(), pProject->GetDisplayName());
        pItemProject->setData(reinterpret_cast<int>(pProject));
        m_pModel->appendRow(pItemProject);

        QList<ProjectExplorer::PeProjectVersion*> lstProjectVersions = pProject->GetAllProjectVersions();
        qSort(lstProjectVersions.begin(), lstProjectVersions.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
        foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, lstProjectVersions)
        {
            QList<QStandardItem*> lstItems;

            QStandardItem *pItem = new QStandardItem(pProjectVersion->GetDisplayIcon(), QString("V%1").arg(pProjectVersion->GetVersionNumber()));
            pItem->setData(reinterpret_cast<int>(pProjectVersion));
            lstItems.append(pItem);

            pItem = new QStandardItem(pProjectVersion->GetModifier());
            pItem->setData(reinterpret_cast<int>(pProjectVersion));
            lstItems.append(pItem);

            pItem = new QStandardItem(pProjectVersion->GetModificationTime().toString("yyyy-MM-dd hh:mm::ss"));
            pItem->setData(reinterpret_cast<int>(pProjectVersion));
            lstItems.append(pItem);

            pItem = new QStandardItem(pProjectVersion->GetWhat());
            pItem->setData(reinterpret_cast<int>(pProjectVersion));
            lstItems.append(pItem);

            pItem = new QStandardItem(pProjectVersion->GetWhy());
            pItem->setData(reinterpret_cast<int>(pProjectVersion));
            lstItems.append(pItem);

            pItemProject->appendRow(lstItems);
        }
    }

    // Create View
    m_pView = new Utils::ReadOnlyTreeView(this);
    m_pView->setAlternatingRowColors(true);
    m_pView->setSortingEnabled(true);

    m_pView->setModel(m_pModel);
    m_pView->expandAll();

    m_pView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pView->setColumnWidth(0, 170);
    m_pView->setColumnWidth(1, 70);
    m_pView->setColumnWidth(2, 140);
    m_pView->setColumnWidth(3, 115);
    m_pView->setColumnWidth(4, 115);

    m_pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(m_pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    m_pDialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(m_pView);
    pLayout->addSpacing(15);
    pLayout->addWidget(m_pDialogButtonBox);

    connect(m_pView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(m_pView, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewDoubleClicked(const QModelIndex&)));
}

ProjectExplorer::PeProjectVersion* OpenProjectDlg::GetSelectedProjectVersion() const
{
    return m_pSelectedProjectVersion;
}

void OpenProjectDlg::SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    m_pSelectedProjectVersion = 0;

    QItemSelectionModel *pSelectionModel = m_pView->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
    if(lstSelectedIndex.size() == 1)
    {
        QStandardItem *pItem = m_pModel->itemFromIndex(lstSelectedIndex.first());
        if(pItem && pItem->parent())
            m_pSelectedProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(pItem->data().toInt());
    }

    m_pDialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(m_pSelectedProjectVersion);
}

void OpenProjectDlg::SlotViewDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    if(m_pSelectedProjectVersion)
        QDialog::accept();
}
