#ifndef STYLEDBAR_H
#define STYLEDBAR_H

#include <QWidget>
#include "styledui_global.h"

namespace StyledUi {

class STYLEDUI_EXPORT StyledBar : public QWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    StyledBar(QWidget *pParent);
    ~StyledBar();

// Operations
protected:
    virtual void paintEvent(QPaintEvent *event);
};

} // StyledUi

#endif // STYLEDBAR_H
