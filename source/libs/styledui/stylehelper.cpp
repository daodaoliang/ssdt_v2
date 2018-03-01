#include <QPainter>
#include <QFile>
#include <qmath.h>

#include "stylehelper.h"

using namespace StyledUi;

QString StyleHelper::m_strWidgetPixmap = ":/styledui/images/skin/12_big";

QString StyleHelper::GetStyleSheet(const StyleType eStyleType)
{
    QString strQssFileName;

    if(eStyleType == stBlue)
        strQssFileName = ":/styledui/qss/blue.qss";

    QString strStyleSheet;
    if(!strQssFileName.isEmpty())
    {
        QFile file(strQssFileName);
        if(file.open(QFile::ReadOnly))
        {
            strStyleSheet = QLatin1String(file.readAll());
            file.close();
        }
    }

    return strStyleSheet;
}

int StyleHelper::GetBarHeight()
{
    return 28;
}

int StyleHelper::GetWidgetFrameWidth()
{
    return 1;
}

int StyleHelper::GetTitleBarHeight()
{
    return 28;
}

QColor StyleHelper::GetBarColor()
{
    return QColor(155, 180, 198);
}

QColor StyleHelper::GetWidgetColor()
{
    return QColor(220, 229, 237);
}

void StyleHelper::SetWidgetPixmap(const QString &strPixmap)
{
    m_strWidgetPixmap = strPixmap;
}

void StyleHelper::DrawWidgetFrame(QPainter *pPainter, const QRect &rect)
{
    QPen pen(Qt::black);
    pen.setWidth(GetWidgetFrameWidth());
    pPainter->setPen(pen);
    pPainter->drawRect(0, 0, rect.width() - GetWidgetFrameWidth(), rect.height() - GetWidgetFrameWidth());
}

void StyleHelper::DrawWidgetPixmap(QPainter *pPainter, const QRect &rect)
{
    pPainter->setPen(Qt::NoPen);
    pPainter->setBrush(Qt::NoBrush);
    pPainter->drawPixmap(rect, QPixmap(m_strWidgetPixmap));
}

void StyleHelper::DrawDialogClient(QPainter *pPainter, const QRect &rect)
{
    pPainter->setPen(Qt::NoPen);
    pPainter->setBrush(QColor(255, 255, 255, 245));
    pPainter->drawRect(rect);
}
