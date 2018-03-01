#ifndef MAINTABBAR_H
#define MAINTABBAR_H

#include <QWidget>

namespace Core {
namespace Internal {

class MainTabBar : public QWidget
{
    Q_OBJECT

// Structures and Enumerations
public:
    struct Tab
    {
        QString strPixmapNormal;
        QString strPixmapHovered;
        QString strPixmapChecked;
        QRect   rcTab;
    };

// Construction and Destruction
public:
    MainTabBar(QWidget *pParent = 0);
    ~MainTabBar();

// Operations
public:
    void            AppendTab(const QString &strPixmap);
    void            InsertTab(int iIndex, const QString &strPixmap);
    void            RemoveTab(int iIndex);
    int             GetTabCount() const;

    int             GetCurrentIndex() const;
    void            SetCurrentIndex(int iIndex);

protected:
    virtual void    resizeEvent(QResizeEvent *event);
    virtual void    paintEvent(QPaintEvent *event);
    virtual void    mousePressEvent(QMouseEvent *event);
    virtual void    mouseMoveEvent(QMouseEvent *event);
    virtual void    leaveEvent(QEvent *event);

private:
    int             HitTestTab(const QPoint &pt);
    void            UpdateMinimumWidth();

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

#endif // MAINTABBAR_H
