#ifndef IEXPLORER_H
#define IEXPLORER_H

#include <QObject>
#include "core_global.h"

namespace Core {

class CORE_EXPORT IExplorer : public QObject
{
    Q_OBJECT

// Construction and Destruction
public:
    IExplorer(QObject *pParent = 0) : QObject(pParent) {}
    virtual ~IExplorer() {}

// Operations
public:
    virtual QString     GetName() const = 0;
    virtual QString     GetPixmap() const = 0;
    virtual int         GetPriority() const = 0;
    virtual QWidget*    GetWidget() const = 0;
};

} // namespace Core

#endif // IEXPLORER_H
