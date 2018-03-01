#ifndef IOPTIONSPAGE_H
#define IOPTIONSPAGE_H

#include <QIcon>
#include <QObject>
#include "core/core_global.h"

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace Core {

class CORE_EXPORT IOptionsPage : public QObject
{
    Q_OBJECT

// Construction and Destruction
public:
    IOptionsPage(QObject *pParent = 0) : QObject(pParent) {}
    virtual ~IOptionsPage() {}

// Operations
public:
    virtual QString     GetID() const = 0;
    virtual QString     GetDisplayName() const = 0;
    virtual QIcon       GetDisplayIcon() const = 0;

    virtual QWidget*    CreatePage(QWidget *pParent) = 0;
    virtual void        Apply() = 0;
    virtual void        Finish() = 0;
};

} // namespace Core

#endif // IOPTIONSPAGE_H
