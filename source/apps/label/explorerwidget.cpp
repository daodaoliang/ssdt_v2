#include <QApplication>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QVBoxLayout>

#include "utils/readonlyview.h"
#include "projectexplorer/peproject.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pecubicle.h"

#include "explorerwidget.h"
#include "mainwindow.h"

static QStandardItem* CreateItem(ProjectExplorer::PeProjectObject *pProjectObject)
{
    QStandardItem *pItem = new QStandardItem(pProjectObject->GetDisplayIcon(), pProjectObject->GetDisplayName());

    pItem->setData(reinterpret_cast<int>(pProjectObject));
    pItem->setDragEnabled(false);
    pItem->setDropEnabled(false);

    return pItem;
}

ExplorerWidget::ExplorerWidget(QWidget *pParent) : QWidget(pParent), m_pProjectVersion(0)
{
    setFixedWidth(300);
    setStyleSheet("QSplitter::handle:vertical {height: 0px;}");

    m_pModel = new QStandardItemModel(0, 1, this);
    m_pView = new Utils::ReadOnlyTreeView(this);
    m_pView->setStyleSheet("QTreeView {background-color: rgba(255, 255, 255, 240); }");
    m_pView->setFrameStyle(QFrame::NoFrame);
    m_pView->setIndentation(m_pView->indentation() * 9/10);
    m_pView->setUniformRowHeights(true);
    m_pView->setTextElideMode(Qt::ElideNone);
    m_pView->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_pView->setHeaderHidden(true);
#if QT_VERSION >= 0x050000
    m_pView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_pView->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    m_pView->header()->setStretchLastSection(false);

    m_pView->setModel(m_pModel);

    QVBoxLayout *pVBoxLayout = new QVBoxLayout(this);
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->addWidget(m_pView);

    connect(m_pView, SIGNAL(pressed(const QModelIndex&)),
            this, SLOT(SlotViewItemPressed(const QModelIndex&)));
}

ExplorerWidget::~ExplorerWidget()
{
}

void ExplorerWidget::SetProjectVersion(ProjectExplorer::PeProjectVersion *pProjectVersion)
{
    if(m_pProjectVersion == pProjectVersion)
        return;
    m_pProjectVersion = pProjectVersion;

    m_pModel->removeRows(0, m_pModel->rowCount());

    if(!m_pProjectVersion)
    {
        emit sigCubicleActivated(0);
        return;
    }

    QList<ProjectExplorer::PeCubicle*> lstCubicles = m_pProjectVersion->GetAllCubicles();
    QList<ProjectExplorer::PeRoom*> lstRooms = m_pProjectVersion->GetAllRooms();

    QStandardItem *pItemProjectVersion = CreateItem(m_pProjectVersion);
    m_pModel->appendRow(pItemProjectVersion);

    qSort(lstRooms.begin(), lstRooms.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    foreach(ProjectExplorer::PeRoom *pRoom, lstRooms)
    {
        QStandardItem *pItemRoom = CreateItem(pRoom);
        pItemProjectVersion->appendRow(pItemRoom);

        foreach(ProjectExplorer::PeCubicle *pCubicle, pRoom->GetChildCubicles())
        {
            QStandardItem *pItemCubicle = CreateItem(pCubicle);
            pItemRoom->appendRow(pItemCubicle);

            lstCubicles.removeOne(pCubicle);
        }

        m_pView->expand(pItemRoom->index());
    }

    qSort(lstCubicles.begin(), lstCubicles.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    foreach(ProjectExplorer::PeCubicle *pCubicle, lstCubicles)
    {
        QStandardItem *pItemCubicle = CreateItem(pCubicle);
        pItemProjectVersion->appendRow(pItemCubicle);
    }

    m_pView->expand(pItemProjectVersion->index());
}

ProjectExplorer::PeProjectVersion* ExplorerWidget::GetProjectVersion() const
{
    return m_pProjectVersion;
}

void ExplorerWidget::SlotViewItemPressed(const QModelIndex &index)
{
    if(QApplication::mouseButtons() != Qt::LeftButton)
        return;

    QStandardItem *pItem = m_pModel->itemFromIndex(index);
    if(!pItem)
        return;

    ProjectExplorer::PeProjectObject *pProjectObject = reinterpret_cast<ProjectExplorer::PeProjectObject*>(pItem->data().toInt());
    if(!pProjectObject)
        return;

    if(ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectObject))
        emit sigCubicleActivated(pCubicle);
}
