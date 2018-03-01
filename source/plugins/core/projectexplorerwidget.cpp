#include <QApplication>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QComboBox>
#include <QMenu>

#include "projectexplorer/peprojectobject.h"
#include "utils/fancylineedit.h"
#include "utils/readonlyview.h"
#include "styledui/styledbar.h"

#include "projectexplorerwidget.h"
#include "projectexplorermodel.h"
#include "mainwindow.h"
#include "core_constants.h"
#include "actionmanager.h"
#include "actioncontainer.h"
#include "command.h"

namespace Core {
namespace Internal {

ProjectExplorerWidget::ProjectExplorerWidget(QWidget *pParent) : QWidget(pParent)
{
    m_pComboBoxFilterPhysical = new QComboBox(this);
    m_pComboBoxFilterPhysical->setObjectName("RoundComboBox");
    m_pComboBoxFilterPhysical->addItem(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otRoom), ProjectExplorer::PeProjectObject::otRoom);
    m_pComboBoxFilterPhysical->addItem(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otCubicle), ProjectExplorer::PeProjectObject::otCubicle);
    m_pComboBoxFilterPhysical->addItem(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otDevice), ProjectExplorer::PeProjectObject::otDevice);
    m_pComboBoxFilterPhysical->addItem(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otBoard), ProjectExplorer::PeProjectObject::otBoard);

    m_pLineEditFilterPhysical = new Utils::FancyLineEdit(this);
    m_pLineEditFilterPhysical->setObjectName("RoundLineEdit");
    m_pLineEditFilterPhysical->setFiltering(true);

    m_pModelPhysical = new ProjectExplorerModel(ProjectExplorerModel::mtPhysical, this);
    m_pFilterModelPhysical = new ProjectExplorerFilterModel(this);
    m_pFilterModelPhysical->setSourceModel(m_pModelPhysical);
    m_pViewPhysical = new Utils::ReadOnlyTreeView(this);
    m_pViewPhysical->setObjectName("Styled_TreeView");
    m_pViewPhysical->setFrameStyle(QFrame::NoFrame);
    m_pViewPhysical->setIndentation(m_pViewPhysical->indentation() * 9/10);
    m_pViewPhysical->setUniformRowHeights(true);
    m_pViewPhysical->setTextElideMode(Qt::ElideNone);
    m_pViewPhysical->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_pViewPhysical->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pViewPhysical->setHeaderHidden(true);
#if QT_VERSION >= 0x050000
    m_pViewPhysical->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_pViewPhysical->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    m_pViewPhysical->header()->setStretchLastSection(false);
    m_pViewPhysical->setModel(m_pFilterModelPhysical);

    m_pComboBoxFilterElectrical = new QComboBox(this);
    m_pComboBoxFilterElectrical->setObjectName("RoundComboBox");
    m_pComboBoxFilterElectrical->addItem(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otBay), ProjectExplorer::PeProjectObject::otBay);
    m_pComboBoxFilterElectrical->addItem(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otDevice), ProjectExplorer::PeProjectObject::otDevice);
    m_pComboBoxFilterElectrical->addItem(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otBoard), ProjectExplorer::PeProjectObject::otBoard);

    m_pLineEditFilterElectrical = new Utils::FancyLineEdit(this);
    m_pLineEditFilterElectrical->setObjectName("RoundLineEdit");
    m_pLineEditFilterElectrical->setFiltering(true);

    m_pModelElectrical = new ProjectExplorerModel(ProjectExplorerModel::mtElectrical, this);
    m_pFilterModelElectrical = new ProjectExplorerFilterModel(this);
    m_pFilterModelElectrical->setSourceModel(m_pModelElectrical);
    m_pViewElectrical = new Utils::ReadOnlyTreeView(this);
    m_pViewElectrical->setObjectName("Styled_TreeView");
    m_pViewElectrical->setFrameStyle(QFrame::NoFrame);
    m_pViewElectrical->setIndentation(m_pViewElectrical->indentation() * 9/10);
    m_pViewElectrical->setUniformRowHeights(true);
    m_pViewElectrical->setTextElideMode(Qt::ElideNone);
    m_pViewElectrical->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_pViewElectrical->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pViewElectrical->setHeaderHidden(true);
#if QT_VERSION >= 0x050000
    m_pViewElectrical->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_pViewElectrical->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    m_pViewElectrical->header()->setStretchLastSection(false);
    m_pViewElectrical->setModel(m_pFilterModelElectrical);

    QWidget *pWidgetFilterPhysical = new StyledUi::StyledBar(this);
    QHBoxLayout *pHBoxLayoutFilterPhysical = new QHBoxLayout(pWidgetFilterPhysical);
    pHBoxLayoutFilterPhysical->setContentsMargins(2, 2, 2, 2);
    pHBoxLayoutFilterPhysical->setSpacing(0);
    pHBoxLayoutFilterPhysical->addWidget(m_pComboBoxFilterPhysical);
    pHBoxLayoutFilterPhysical->addSpacing(1);
    pHBoxLayoutFilterPhysical->addWidget(m_pLineEditFilterPhysical);

    QWidget *pWidgetFilterElectrical = new StyledUi::StyledBar(this);
    QHBoxLayout *pHBoxLayoutFilterElectrical = new QHBoxLayout(pWidgetFilterElectrical);
    pHBoxLayoutFilterElectrical->setContentsMargins(2, 2, 2, 2);
    pHBoxLayoutFilterElectrical->setSpacing(0);
    pHBoxLayoutFilterElectrical->addWidget(m_pComboBoxFilterElectrical);
    pHBoxLayoutFilterElectrical->addSpacing(1);
    pHBoxLayoutFilterElectrical->addWidget(m_pLineEditFilterElectrical);

    QVBoxLayout *pVBoxLayout = new QVBoxLayout(this);
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->setSpacing(0);
    pVBoxLayout->addWidget(pWidgetFilterPhysical);
    pVBoxLayout->addWidget(m_pViewPhysical);
    pVBoxLayout->addSpacing(1);
    pVBoxLayout->addWidget(pWidgetFilterElectrical);
    pVBoxLayout->addWidget(m_pViewElectrical);

    QList<int> lstGlobalContexts = QList<int>() << Constants::g_iUID_Context_Global;

    Core::CActionManager *pActionManager = MainWindow::Instance()->GetActionManager();
    Core::CActionContainer *pActionContainerContextMenu = pActionManager->GetActionContainer(Core::Constants::g_szUID_ContextMenu);

    m_pContextMenuActionExpandAllPhysical = new QAction(tr("E&xpand All"), this);
    CCommand *pCommand = pActionManager->RegisterAction(m_pContextMenuActionExpandAllPhysical, QLatin1String("Core.ContextMenuAction.ExpandAllPhysical"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Constants::g_szUID_ContextMenuGroup_TreeView);
    connect(m_pContextMenuActionExpandAllPhysical, SIGNAL(triggered()), m_pViewPhysical, SLOT(expandAll()));

    m_pContextMenuActionCollapseAllPhysical = new QAction(tr("Co&llapse All"), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionCollapseAllPhysical, QLatin1String("Core.ContextMenuAction.CollapseAllPhysical"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Constants::g_szUID_ContextMenuGroup_TreeView);
    connect(m_pContextMenuActionCollapseAllPhysical, SIGNAL(triggered()), m_pViewPhysical, SLOT(collapseAll()));

    m_pContextMenuActionExpandAllElectrical = new QAction(tr("E&xpand All"), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionExpandAllElectrical, QLatin1String("Core.ContextMenuAction.ExpandAllElectrical"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Constants::g_szUID_ContextMenuGroup_TreeView);
    connect(m_pContextMenuActionExpandAllElectrical, SIGNAL(triggered()), m_pViewElectrical, SLOT(expandAll()));

    m_pContextMenuActionCollapseAllElectrical = new QAction(tr("Co&llapse All"), this);
    pCommand = pActionManager->RegisterAction(m_pContextMenuActionCollapseAllElectrical, QLatin1String("Core.ContextMenuAction.CollapseAllElectrical"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Constants::g_szUID_ContextMenuGroup_TreeView);
    connect(m_pContextMenuActionCollapseAllElectrical, SIGNAL(triggered()), m_pViewElectrical, SLOT(collapseAll()));

    Core::MainWindow::Instance()->SetContext(lstGlobalContexts);

    connect(m_pViewPhysical, SIGNAL(pressed(const QModelIndex&)),
            this, SLOT(SlotViewItemPressed(const QModelIndex&)));
    connect(m_pViewElectrical, SIGNAL(pressed(const QModelIndex&)),
            this, SLOT(SlotViewItemPressed(const QModelIndex&)));
    connect(m_pViewPhysical->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&,const QModelIndex&)),
            this, SLOT(SlotViewCurrentChanged(const QModelIndex&,const QModelIndex&)));
    connect(m_pViewElectrical->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&,const QModelIndex&)),
            this, SLOT(SlotViewCurrentChanged(const QModelIndex&,const QModelIndex&)));
    connect(m_pModelPhysical, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            this, SLOT(SlotModelRowsInserted(QModelIndex,int,int)));
    connect(m_pModelElectrical, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
            this, SLOT(SlotModelRowsInserted(QModelIndex,int,int)));
    connect(m_pViewPhysical, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(SlotViewCustomContextMenuRequested(const QPoint&)));
    connect(m_pViewElectrical, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(SlotViewCustomContextMenuRequested(const QPoint&)));
    connect(m_pLineEditFilterPhysical, SIGNAL(textChanged(const QString&)),
            this, SLOT(SlotFilterTextChanged(const QString&)));
    connect(m_pLineEditFilterElectrical, SIGNAL(textChanged(const QString&)),
            this, SLOT(SlotFilterTextChanged(const QString&)));
    connect(m_pComboBoxFilterPhysical, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotFilterTypeChanged(int)));
    connect(m_pComboBoxFilterElectrical, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotFilterTypeChanged(int)));

    m_pComboBoxFilterPhysical->setCurrentIndex(2);
    m_pComboBoxFilterElectrical->setCurrentIndex(1);
}

ProjectExplorerWidget::~ProjectExplorerWidget()
{
}

void ProjectExplorerWidget::SlotViewItemPressed(const QModelIndex &index)
{
    if(QApplication::mouseButtons() != Qt::LeftButton)
        return;

    Utils::ReadOnlyTreeView *pView = qobject_cast<Utils::ReadOnlyTreeView*>(sender());
    if(!pView)
        return;

    ProjectExplorerFilterModel *pFilterModel = qobject_cast<ProjectExplorerFilterModel*>(pView->model());
    if(!pFilterModel)
        return;

    ProjectExplorerModel *pSourceModel = qobject_cast<ProjectExplorerModel*>(pFilterModel->sourceModel());
    if(!pSourceModel)
        return;

    ProjectExplorerItem *pItem = pSourceModel->ItemFromIndex(pFilterModel->mapToSource(index));
    if(!pItem)
        return;

    if(ProjectExplorer::PeProjectObject *pProjectObject = pItem->GetProjectObject())
        MainWindow::Instance()->SetActiveObject(pProjectObject, pSourceModel->GetModelType() == ProjectExplorerModel::mtElectrical);
}

void ProjectExplorerWidget::SlotViewCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(current)
    Q_UNUSED(previous)

    QItemSelectionModel *pItemSelectionModel = qobject_cast<QItemSelectionModel*>(sender());
    if(!pItemSelectionModel)
        return;

    Utils::ReadOnlyTreeView *pView = 0;
    if(m_pViewElectrical->selectionModel() == pItemSelectionModel)
        pView = m_pViewElectrical;
    else if(m_pViewPhysical->selectionModel() == pItemSelectionModel)
        pView = m_pViewPhysical;
    if(!pView)
        return;

    ProjectExplorerFilterModel *pFilterModel = qobject_cast<ProjectExplorerFilterModel*>(pView->model());
    if(!pFilterModel)
        return;

    ProjectExplorerModel *pSourceModel = qobject_cast<ProjectExplorerModel*>(pFilterModel->sourceModel());
    if(!pSourceModel)
        return;

    if(ProjectExplorerItem *pItem = pSourceModel->ItemFromIndex(pFilterModel->mapToSource(pItemSelectionModel->currentIndex())))
        MainWindow::Instance()->SetActiveObject(pItem->GetProjectObject(), pSourceModel->GetModelType() == ProjectExplorerModel::mtElectrical, false);
    else
        MainWindow::Instance()->SetActiveObject(0, pSourceModel->GetModelType() == ProjectExplorerModel::mtElectrical, false);
}

void ProjectExplorerWidget::SlotModelRowsInserted(const QModelIndex &parent, int first, int last)
{
    ProjectExplorerModel *pModel = qobject_cast<ProjectExplorerModel*>(sender());
    if(!pModel)
        return;

    Utils::ReadOnlyTreeView *pView = 0;
    ProjectExplorerFilterModel *pFilterModel = 0;
    if(pModel == m_pModelPhysical)
    {
        pFilterModel = m_pFilterModelPhysical;
        pView = m_pViewPhysical;
    }
    else if(pModel == m_pModelElectrical)
    {
        pFilterModel = m_pFilterModelElectrical;
        pView = m_pViewElectrical;
    }
    else
        return;

    if(parent.isValid())
        return;

    for(int i = first; i <= last; i++)
    {
        QModelIndex index = pModel->index(i, 0, parent);
        if(!index.isValid())
            continue;

        ProjectExplorerItem *pItem = pModel->ItemFromIndex(index);
        if(!pItem)
            continue;

        if(pItem->GetChildItems().isEmpty())
            continue;

        pView->expand(pFilterModel->mapFromSource(index));
    }
}

void ProjectExplorerWidget::SlotViewCustomContextMenuRequested(const QPoint &point)
{
    Utils::ReadOnlyTreeView *pView = qobject_cast<Utils::ReadOnlyTreeView*>(sender());
    if(!pView)
        return;

    ProjectExplorerFilterModel *pFilterModel = qobject_cast<ProjectExplorerFilterModel*>(pView->model());
    if(!pFilterModel)
        return;

    ProjectExplorerModel *pSourceModel = qobject_cast<ProjectExplorerModel*>(pFilterModel->sourceModel());
    if(!pSourceModel)
        return;

    m_pContextMenuActionExpandAllPhysical->setEnabled(pView == m_pViewPhysical);
    m_pContextMenuActionExpandAllPhysical->setVisible(pView == m_pViewPhysical);
    m_pContextMenuActionCollapseAllPhysical->setEnabled(pView == m_pViewPhysical);
    m_pContextMenuActionCollapseAllPhysical->setVisible(pView == m_pViewPhysical);
    m_pContextMenuActionExpandAllElectrical->setEnabled(pView == m_pViewElectrical);
    m_pContextMenuActionExpandAllElectrical->setVisible(pView == m_pViewElectrical);
    m_pContextMenuActionCollapseAllElectrical->setEnabled(pView == m_pViewElectrical);
    m_pContextMenuActionCollapseAllElectrical->setVisible(pView == m_pViewElectrical);

    ProjectExplorerItem *pItem = dynamic_cast<ProjectExplorerItem*>(pSourceModel->ItemFromIndex(pFilterModel->mapToSource(pView->indexAt(point))));
    MainWindow::Instance()->SetContextMenuObject(pItem ? pItem->GetProjectObject() : 0, (pSourceModel == m_pModelElectrical));

    Core::CActionContainer *pActionContainerContextMenu = Core::MainWindow::Instance()->GetActionManager()->GetActionContainer(Core::Constants::g_szUID_ContextMenu);
    if(pActionContainerContextMenu)
         pActionContainerContextMenu->GetMenu()->popup(pView->mapToGlobal(point));
}

void ProjectExplorerWidget::SlotFilterTextChanged(const QString &strFilter)
{
    Utils::FancyLineEdit *pLineEditFilter = qobject_cast<Utils::FancyLineEdit*>(sender());

    if(pLineEditFilter == m_pLineEditFilterPhysical)
    {
        int iCurrentIndex = m_pComboBoxFilterPhysical->currentIndex();
        if(iCurrentIndex >= 0)
        {
            m_pFilterModelPhysical->SlotSetFilter(ProjectExplorer::PeProjectObject::ObjectType(m_pComboBoxFilterPhysical->itemData(iCurrentIndex).toInt()), strFilter);
            if(!strFilter.isEmpty())
            {
                m_pViewPhysical->expandAll();
                foreach(ProjectExplorerItem *pItem, m_pFilterModelPhysical->GetFilterItems())
                {
                    QModelIndex index = m_pFilterModelPhysical->mapFromSource(m_pModelPhysical->IndexForItem(pItem));
                    m_pViewPhysical->collapse(index);
                }
            }
        }
    }
    else if(pLineEditFilter == m_pLineEditFilterElectrical)
    {
        int iCurrentIndex = m_pComboBoxFilterElectrical->currentIndex();
        if(iCurrentIndex >= 0)
        {
            m_pFilterModelElectrical->SlotSetFilter(ProjectExplorer::PeProjectObject::ObjectType(m_pComboBoxFilterElectrical->itemData(iCurrentIndex).toInt()), strFilter);
            if(!strFilter.isEmpty())
            {
                m_pViewElectrical->expandAll();
                foreach(ProjectExplorerItem *pItem, m_pFilterModelElectrical->GetFilterItems())
                {
                    QModelIndex index = m_pFilterModelElectrical->mapFromSource(m_pModelElectrical->IndexForItem(pItem));
                    m_pViewElectrical->collapse(index);
                }
            }
        }
    }
}

void ProjectExplorerWidget::SlotFilterTypeChanged(int iCurrentIndex)
{
    if(iCurrentIndex < 0)
        return;

    QComboBox *pComboBox = qobject_cast<QComboBox*>(sender());
    if(pComboBox == m_pComboBoxFilterPhysical)
    {
        m_pLineEditFilterPhysical->setPlaceholderText(m_pComboBoxFilterPhysical->itemText(iCurrentIndex) + tr("Filter"));

        const QString strFilter = m_pLineEditFilterPhysical->text().trimmed();
        m_pFilterModelPhysical->SlotSetFilter(ProjectExplorer::PeProjectObject::ObjectType(m_pComboBoxFilterPhysical->itemData(iCurrentIndex).toInt()), strFilter);
        if(!strFilter.isEmpty())
        {
            m_pViewPhysical->expandAll();
            foreach(ProjectExplorerItem *pItem, m_pFilterModelPhysical->GetFilterItems())
            {
                QModelIndex index = m_pFilterModelPhysical->mapFromSource(m_pModelPhysical->IndexForItem(pItem));
                m_pViewPhysical->collapse(index);
            }
        }
    }
    else if(pComboBox == m_pComboBoxFilterElectrical)
    {
        m_pLineEditFilterElectrical->setPlaceholderText(m_pComboBoxFilterElectrical->itemText(iCurrentIndex) + tr("Filter"));

        const QString strFilter = m_pLineEditFilterElectrical->text().trimmed();
        m_pFilterModelElectrical->SlotSetFilter(ProjectExplorer::PeProjectObject::ObjectType(m_pComboBoxFilterElectrical->itemData(iCurrentIndex).toInt()), strFilter);
        if(!strFilter.isEmpty())
        {
            m_pViewElectrical->expandAll();
            foreach(ProjectExplorerItem *pItem, m_pFilterModelElectrical->GetFilterItems())
            {
                QModelIndex index = m_pFilterModelElectrical->mapFromSource(m_pModelElectrical->IndexForItem(pItem));
                m_pViewElectrical->collapse(index);
            }
        }
    }
}

} // namespace Internal
} // namespace Core
