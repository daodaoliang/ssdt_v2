#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QList>
#include "core/core_global.h"

QT_BEGIN_NAMESPACE
class QAction;
class QShortcut;
class QString;
QT_END_NAMESPACE

namespace Core {

class CActionContainer;
class CCommand;

class CORE_EXPORT CActionManager : public QObject
{
    Q_OBJECT

// Construction and Destruction
public:
    CActionManager(QObject *pParent = 0) : QObject(pParent) {}
    virtual ~CActionManager() {}

// Operations
public:
    virtual CActionContainer*   CreateMenu(const QString &strMenuID) = 0;
    virtual CActionContainer*   CreateMenuBar(const QString &strID) = 0;

    virtual CCommand*           RegisterAction(QAction *pAction, const QString &strActionID, const QList<int> &lstContext) = 0;
    virtual CCommand*           RegisterShortcut(QShortcut *pShortcut, const QString &strID, const QList<int> &lstContext) = 0;

    virtual CCommand*           GetCommand(const QString &strID) const = 0;
    virtual CActionContainer*   GetActionContainer(const QString &strID) const = 0;

    virtual QList<CCommand*>    GetCommands() const = 0;
};

} // namespace Core

#endif // ACTIONMANAGER_H
