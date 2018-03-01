#ifndef SIDETABBAR_H
#define SIDETABBAR_H

#include <QWidget>

namespace Core {
namespace Internal {

class SideTabBar : public QWidget
{
    Q_OBJECT

// Structures and Enumerations
public:
    struct Tab
    {
        QString strPixmap;
        QString strText;
    };

// Construction and Destruction
public:
    SideTabBar(QWidget *pParent = 0);
    ~SideTabBar();

// Operations
public:
    void            AppendTab(const QString &strPixmap, const QString &strText);
    void            InsertTab(int iIndex, const QString &strPixmap, const QString &strText);
    void            RemoveTab(int iIndex);
    int             GetTabCount() const;

    int             GetCurrentIndex() const;
    void            SetCurrentIndex(int iIndex);

protected:
    virtual void    paintEvent(QPaintEvent *event);
    virtual void    mousePressEvent(QMouseEvent *event);
    virtual void    mouseMoveEvent(QMouseEvent *event);
    virtual void    leaveEvent(QEvent *event);
    virtual bool    event(QEvent *event);

private:
    int             HitTestTab(const QPoint &pt);
    void            UpdateMinimumHeight();

// Properties
private:
    QList<Tab>      m_lstTabs;
    int             m_iHoveredTab;
    int             m_iCheckedTab;

signals:
    void            sigCurrentIndexChanged(int iIndex);
};

} // namespace Internal
} // namespace Core

#endif // SIDETABBAR_H
