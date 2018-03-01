#include <QApplication>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QHeaderView>

#include "utils/readonlyview.h"
#include "extensionsystem/pluginmanager.h"

#include "explorerwidget.h"
#include "sidetabbar.h"
#include "iexplorer.h"

class ExplorerTreeView : public Utils::ReadOnlyTreeView
{
public:
    ExplorerTreeView(QWidget *pParent = 0) : Utils::ReadOnlyTreeView(pParent)
    {
        setStyleSheet("QTreeView {background-color: rgba(255, 255, 255, 240); }");
        setFrameStyle(QFrame::NoFrame);
        setIndentation(indentation() * 9/10);
        setUniformRowHeights(true);
        setTextElideMode(Qt::ElideNone);
        setAttribute(Qt::WA_MacShowFocusRect, false);
        setHeaderHidden(true);
#if QT_VERSION >= 0x050000
        header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
        header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
        header()->setStretchLastSection(false);
    }
};

namespace Core {

class ExplorerWidgetPrivate
{
public:
    ExplorerWidgetPrivate() {}

    Internal::SideTabBar    *m_pTabBar;
    QStackedWidget          *m_pStackedWidget;
    QList<IExplorer*>       m_lstExplorers;
};

ExplorerWidget::ExplorerWidget(QWidget *pParent) :
    QSplitter(pParent), m_d(new ExplorerWidgetPrivate)
{
    setStyleSheet("QSplitter::handle:vertical {height: 0px;}");

    m_d->m_pTabBar = new Internal::SideTabBar(this);
    m_d->m_pStackedWidget = new QStackedWidget(this);
    m_d->m_pStackedWidget->setFixedWidth(300);

    addWidget(m_d->m_pTabBar);
    addWidget(m_d->m_pStackedWidget);

    connect(m_d->m_pTabBar, SIGNAL(sigCurrentIndexChanged(int)),
            this, SLOT(SlotCurrentTabChanged(int)));
    connect(ExtensionSystem::PluginManager::instance(), SIGNAL(objectAdded(QObject*)),
            this, SLOT(SlotObjectAdded(QObject*)));
    connect(ExtensionSystem::PluginManager::instance(), SIGNAL(aboutToRemoveObject(QObject*)),
            this, SLOT(SlotAboutToRemoveObject(QObject*)));
}

ExplorerWidget::~ExplorerWidget()
{
}

void ExplorerWidget::SlotCurrentTabChanged(int iIndex)
{
    if(iIndex >= 0)
        m_d->m_pStackedWidget->setCurrentIndex(iIndex);

    m_d->m_pStackedWidget->setVisible(iIndex >= 0);
}

void ExplorerWidget::SlotObjectAdded(QObject *pObject)
{
    if(IExplorer *pExplorer = ExtensionSystem::query<IExplorer>(pObject))
    {
        int iIndex = 0;
        foreach(const IExplorer *pE, m_d->m_lstExplorers)
        {
            if(pE->GetPriority() > pExplorer->GetPriority())
                ++iIndex;
        }

        m_d->m_lstExplorers.insert(iIndex, pExplorer);
        m_d->m_pTabBar->InsertTab(iIndex, pExplorer->GetPixmap(), pExplorer->GetName());
        m_d->m_pStackedWidget->insertWidget(iIndex, pExplorer->GetWidget());

        if(m_d->m_pTabBar->GetCurrentIndex() >= iIndex)
            m_d->m_pTabBar->SetCurrentIndex(iIndex);
    }
}

void ExplorerWidget::SlotAboutToRemoveObject(QObject *pObject)
{
    if(IExplorer *pExplorer = ExtensionSystem::query<IExplorer>(pObject))
    {
        const int iIndex = m_d->m_lstExplorers.indexOf(pExplorer);
        m_d->m_lstExplorers.removeAt(iIndex);
        m_d->m_pTabBar->RemoveTab(iIndex);
        m_d->m_pStackedWidget->removeWidget(m_d->m_pStackedWidget->widget(iIndex));
    }
}

} // namespace Core
