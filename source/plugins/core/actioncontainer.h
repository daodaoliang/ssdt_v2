#ifndef ACTIONCONTAINER_H
#define ACTIONCONTAINER_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QMenu;
class QMenuBar;
class QAction;
QT_END_NAMESPACE

namespace Core {

class CCommand;

class CActionContainer : public QObject
{
    Q_OBJECT

// Structure and enumeration
public:
    enum EmptyAction
    {
        EA_Mask             = 0xFF00,
        EA_None             = 0x0100,
        EA_Hide             = 0x0200,
        EA_Disable          = 0x0300
    };

// Construction and Destruction
public:
    virtual ~CActionContainer() {}

// Operations
public:
    virtual void        SetEmptyAction(EmptyAction eEmptyAction) = 0;
    virtual int         GetID() const = 0;

    virtual QMenu*      GetMenu() const = 0;
    virtual QMenuBar*   GetMenuBar() const = 0;

    virtual QAction*    InsertLocation(const QString &strGroup) const = 0;
    virtual void        AppendGroup(const QString &strGroupID) = 0;
    virtual void        AddAction(Core::CCommand *pAction, const QString &strGroupID = QString()) = 0;
    virtual void        AddMenu(CActionContainer *pMenu, const QString &strGroupID = QString()) = 0;
};

} // namespace Core

#endif // ACTIONCONTAINER_H
