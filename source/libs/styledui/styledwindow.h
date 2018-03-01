#ifndef STYLEDWINDOW_H
#define STYLEDWINDOW_H

#include <QWidget>
#include "styledui_global.h"

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

namespace StyledUi {

class StyledWindowPrivate;
class STYLEDUI_EXPORT StyledWindow : public QWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    StyledWindow(QWidget *pParent = 0);
    ~StyledWindow();

// Operations
public:
    void            SetWindowIcon(const QIcon &icon);
    void            SetWindowTitle(const QString &strTitle);
    void            SetWindowMenu(QMenu *pMenu);

    void            ShowMaximized(bool bMaximized);
    bool            IsMaximized() const;

protected:
    virtual void    mousePressEvent(QMouseEvent *event);
    virtual void    mouseMoveEvent(QMouseEvent *event);
    virtual void    mouseReleaseEvent(QMouseEvent *event);
    virtual void    mouseDoubleClickEvent(QMouseEvent *event);
    virtual void    paintEvent(QPaintEvent *event);

// Properties
private:
    StyledWindowPrivate *m_d;
};

} // StyledUi

#endif // STYLEDWINDOW_H
