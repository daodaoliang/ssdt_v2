#include <QPainter>

#include "styledbar.h"
#include "stylehelper.h"

using namespace StyledUi;

StyledBar::StyledBar(QWidget *pParent) : QWidget(pParent)
{
    setFixedHeight(StyledUi::StyleHelper::GetBarHeight());
}

StyledBar::~StyledBar()
{
}

void StyledBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(StyledUi::StyleHelper::GetBarColor()));
    painter.drawRect(rect());

    painter.setPen(QPen(Qt::gray));
    painter.drawLine(rect().bottomLeft(), rect().bottomRight());
}
