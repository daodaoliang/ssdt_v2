#ifndef STYLEHELPER_H
#define STYLEHELPER_H

#include <QColor>
#include "styledui_global.h"

QT_BEGIN_NAMESPACE
class QPainter;
QT_END_NAMESPACE

namespace StyledUi{

class STYLEDUI_EXPORT StyleHelper
{
// Structures and Enumerations
public:
    enum StyleType { stBlue };

// Operations
public:
    static QString  GetStyleSheet(const StyleType eStyleType);

    static int      GetBarHeight();
    static int      GetWidgetFrameWidth();
    static int      GetTitleBarHeight();

    static QColor   GetBarColor();
    static QColor   GetWidgetColor();

    static void     SetWidgetPixmap(const QString &strPixmap);

    static void     DrawWidgetFrame(QPainter *pPainter, const QRect &rect);
    static void     DrawWidgetPixmap(QPainter *pPainter, const QRect &rect);
    static void     DrawDialogClient(QPainter *pPainter, const QRect &rect);

private:
    static QString  m_strWidgetPixmap;
};

} // namespace StyledUi

#endif // STYLEHELPER_H
