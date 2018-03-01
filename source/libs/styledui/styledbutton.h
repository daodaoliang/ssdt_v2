#ifndef STYLEDBUTTON_H
#define STYLEDBUTTON_H

#include <QWidget>
#include "styledui_global.h"

namespace StyledUi {

class StyledButtonPrivate;
class STYLEDUI_EXPORT StyledButton : public QWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    StyledButton(const QString &strPixmap, const QString &strName, QWidget *pParent = 0);
    ~StyledButton();

// Operations
public:
    void            SetPixmap(const QString &strPixmap);
    void            SetHoveredBKColor(const QColor &crHoveredBkGround);
    void            SetName(const QString &strName);
    void            SetSize(const QSize &size);
    void            SetEnabled(bool bEnabled);

protected:
    virtual void    enterEvent(QEvent *event);
    virtual void    leaveEvent(QEvent *event);
    virtual void    mousePressEvent(QMouseEvent *event);
    virtual void    mouseReleaseEvent(QMouseEvent *event);
    virtual void    paintEvent(QPaintEvent *event);

// Properties
private:
    StyledButtonPrivate *m_d;

signals:
    void sigClicked();
};

} // StyledUi

#endif // STYLEDBUTTON_H
