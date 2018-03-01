#include <QPainter>

#include "styledwidget.h"
#include "stylehelper.h"

using namespace StyledUi;

StyledWidget::StyledWidget(QWidget *pParent) : QWidget(pParent)
{
}

StyledWidget::~StyledWidget()
{
}

void StyledWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(StyledUi::StyleHelper::GetWidgetColor()));
    painter.drawRect(rect());
}
