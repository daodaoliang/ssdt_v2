#include <QPainter>
#include <QMouseEvent>
#include <QToolTip>

#include "sidetabbar.h"

const int g_iTabHeight = 70;

using namespace Core::Internal;

SideTabBar::SideTabBar(QWidget *pParent) :
    QWidget(pParent), m_iHoveredTab(-1), m_iCheckedTab(-1)
{
    setMouseTracking(true);
    setFixedWidth(45);
}

SideTabBar::~SideTabBar()
{
}

void SideTabBar::AppendTab(const QString &strPixmap, const QString &strText)
{
    InsertTab(m_lstTabs.size(), strPixmap, strText);
}

void SideTabBar::InsertTab(int iIndex, const QString &strPixmap, const QString &strText)
{
    if(m_lstTabs.isEmpty() || iIndex <= m_iCheckedTab)
        m_iCheckedTab++;

    Tab tab;
    tab.strPixmap = strPixmap;
    tab.strText = strText;

    m_lstTabs.insert(iIndex, tab);
    UpdateMinimumHeight();
}

void SideTabBar::RemoveTab(int iIndex)
{
    m_lstTabs.removeAt(iIndex);
    UpdateMinimumHeight();
}

int SideTabBar::GetTabCount() const
{
    return m_lstTabs.size();
}

int SideTabBar::GetCurrentIndex() const
{
    return m_iCheckedTab;
}

void SideTabBar::SetCurrentIndex(int iIndex)
{
    if(iIndex >= m_lstTabs.size())
        return;

    if(m_iCheckedTab != iIndex)
    {
        m_iCheckedTab = iIndex;
        emit sigCurrentIndexChanged(m_iCheckedTab);

        update();
    }
}

void SideTabBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(255, 255, 255, 60)));

    const QRect rc = rect();
    painter.drawRect(rc);

    QRect rcTab(0, 0, rc.width(), g_iTabHeight);
    for(int i = 0; i < m_lstTabs.size(); i++)
    {
        Tab tab = m_lstTabs.at(i);

        rcTab.moveTop(i * g_iTabHeight);

        if(i == m_iCheckedTab)
            painter.setBrush(QBrush(QColor(255, 255, 255, 100)));
        else if(i == m_iHoveredTab)
            painter.setBrush(QBrush(QColor(0, 0, 0, 100)));
        else
            painter.setBrush(Qt::NoBrush);

        painter.drawRect(rcTab);

        QPixmap pixmap(tab.strPixmap);
        QRect rcPixmap = QRect(0, 0, 32, 32);
        rcPixmap.moveCenter(rcTab.center());
        painter.drawPixmap(rcPixmap, pixmap);
    }
}

void SideTabBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;

    int iCheckedTab = HitTestTab(event->pos());
    if(iCheckedTab == -1)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    if(m_iCheckedTab == iCheckedTab)
        iCheckedTab = -1;

    SetCurrentIndex(iCheckedTab);
}

void SideTabBar::mouseMoveEvent(QMouseEvent *event)
{
    int iHoveredTab = HitTestTab(event->pos());
    if(m_iHoveredTab != iHoveredTab)
    {
        m_iHoveredTab = iHoveredTab;
        update();
    }

    QWidget::mouseMoveEvent(event);
}

void SideTabBar::leaveEvent(QEvent *event)
{
    if(m_iHoveredTab != -1)
    {
        m_iHoveredTab = -1;
        update();
    }

    QWidget::leaveEvent(event);
}

bool SideTabBar::event(QEvent *event)
{
    if(event->type() == QEvent::ToolTip)
    {
        if(m_iHoveredTab >= 0)
        {
            QString strToolTip = m_lstTabs.at(m_iHoveredTab).strText;
            if(!strToolTip.isEmpty())
            {
                QToolTip::showText(static_cast<QHelpEvent*>(event)->globalPos(), strToolTip, this);
                return true;
            }
        }
    }

    return QWidget::event(event);
}

int SideTabBar::HitTestTab(const QPoint &pt)
{
    QRect rcTab(0, 0, rect().width(), g_iTabHeight);
    for(int i = 0; i < m_lstTabs.size(); i++)
    {
        rcTab.moveTop(g_iTabHeight * i);
        if(rcTab.contains(pt))
            return i;
    }

    return -1;
}

void SideTabBar::UpdateMinimumHeight()
{
    setMinimumHeight(g_iTabHeight * (m_lstTabs.size() + 1));
}
