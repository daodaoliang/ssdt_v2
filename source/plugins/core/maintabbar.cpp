#include <QPainter>
#include <QMouseEvent>

#include "maintabbar.h"

const int g_iTabGap = 30;

using namespace Core::Internal;

MainTabBar::MainTabBar(QWidget *pParent) :
    QWidget(pParent), m_iHoveredTab(-1), m_iCheckedTab(-1)
{
    setMouseTracking(true);
    setFixedHeight(60);
}

MainTabBar::~MainTabBar()
{
}

void MainTabBar::AppendTab(const QString &strPixmap)
{
    InsertTab(m_lstTabs.size(), strPixmap);
}

void MainTabBar::InsertTab(int iIndex, const QString &strPixmap)
{
    Tab tab;
    tab.strPixmapNormal = strPixmap + "_normal.png";
    tab.strPixmapHovered = strPixmap + "_hovered.png";
    tab.strPixmapChecked = strPixmap + "_checked.png";

    QPixmap pixmap;
    pixmap.load(tab.strPixmapNormal);
    tab.rcTab = QRect(QPoint(0, 0), pixmap.size());

    m_lstTabs.insert(iIndex, tab);
    UpdateMinimumWidth();
}

void MainTabBar::RemoveTab(int iIndex)
{
    m_lstTabs.removeAt(iIndex);
    UpdateMinimumWidth();
}

int MainTabBar::GetTabCount() const
{
    return m_lstTabs.size();
}

int MainTabBar::GetCurrentIndex() const
{
    return m_iCheckedTab;
}

void MainTabBar::SetCurrentIndex(int iIndex)
{
    if(m_iCheckedTab != iIndex)
    {
        m_iCheckedTab = iIndex;
        emit sigCurrentIndexChanged(m_iCheckedTab);

        update();
    }
}

void MainTabBar::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    const QRect rc = rect();

    int iCurrentX = 0;
    for(int i = 0; i < m_lstTabs.size(); i++)
    {
        Tab &tab = m_lstTabs[i];
        tab.rcTab.moveTopLeft(QPoint(iCurrentX, (rc.height() - tab.rcTab.height()) / 2));

        iCurrentX += g_iTabGap;
        iCurrentX += tab.rcTab.width();
    }
}

void MainTabBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);

    for(int i = 0; i < m_lstTabs.size(); i++)
    {
        Tab tab = m_lstTabs.at(i);

        QPixmap pixmap;
        if(i == m_iCheckedTab)
            pixmap.load(tab.strPixmapChecked);
        else if(i == m_iHoveredTab)
            pixmap.load(tab.strPixmapHovered);
        else
            pixmap.load(tab.strPixmapNormal);

        painter.drawPixmap(tab.rcTab, pixmap);
    }
}

void MainTabBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;

    int iCheckedTab = HitTestTab(event->pos());
    if(iCheckedTab == -1)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    SetCurrentIndex(iCheckedTab);
}

void MainTabBar::mouseMoveEvent(QMouseEvent *event)
{
    int iHoveredTab = HitTestTab(event->pos());
    if(m_iHoveredTab != iHoveredTab)
    {
        m_iHoveredTab = iHoveredTab;
        update();
    }

    QWidget::mouseMoveEvent(event);
}

void MainTabBar::leaveEvent(QEvent *event)
{
    if(m_iHoveredTab != -1)
    {
        m_iHoveredTab = -1;
        update();
    }

    QWidget::leaveEvent(event);
}

int MainTabBar::HitTestTab(const QPoint &pt)
{
    for(int i = 0; i < m_lstTabs.size(); i++)
    {
        QRect rcTab = m_lstTabs.at(i).rcTab;
        rcTab.adjust(-g_iTabGap / 2, 0, g_iTabGap / 2, 0);
        if(rcTab.contains(pt))
            return i;
    }

    return -1;
}

void MainTabBar::UpdateMinimumWidth()
{
    int iMinimumWidth = g_iTabGap * (m_lstTabs.size() + 1);
    foreach(const Tab &tab, m_lstTabs)
        iMinimumWidth += tab.rcTab.width();

    setMinimumWidth(iMinimumWidth);
}
