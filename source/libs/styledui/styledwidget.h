#ifndef STYLEDWIDGET_H
#define STYLEDWIDGET_H

#include <QWidget>
#include "styledui_global.h"

namespace StyledUi {

class STYLEDUI_EXPORT StyledWidget : public QWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    StyledWidget(QWidget *pParent);
    ~StyledWidget();

// Operations
protected:
    virtual void paintEvent(QPaintEvent *event);
};

} // StyledUi

#endif // STYLEDWIDGET_H
