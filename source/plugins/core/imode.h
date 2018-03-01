#ifndef IMODE_H
#define IMODE_H

#include <QWidget>
#include "core_global.h"

namespace ProjectExplorer {

class PeProjectObject;

} // namespace ProjectExplorer

QT_BEGIN_NAMESPACE
class QIcon;
QT_END_NAMESPACE

namespace Core {

class CORE_EXPORT IMode : public QObject
{
    Q_OBJECT

// Construction and Destruction
public:
    IMode(QObject *pParent = 0) : QObject(pParent) {}
    virtual ~IMode() {}

// Operations
public:
    virtual QString     GetId() const = 0;
    virtual QString     GetName() const = 0;
    virtual QString     GetPixmap() const = 0;
    virtual int         GetPriority() const = 0;
    virtual QWidget*    GetWidget() const = 0;
    virtual void        ActivateObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject) = 0;
    virtual void        SetContextMenuObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject) = 0;
};

} // namespace Core

#endif // IMODE_H
