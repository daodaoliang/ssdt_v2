#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include "core_global.h"

QT_BEGIN_NAMESPACE
class QAction;
class QShortcut;
class QKeySequence;
QT_END_NAMESPACE

namespace Core {

class CORE_EXPORT CCommand : public QObject
{
    Q_OBJECT

// Structure and enumeration
public:
    enum CommandAttribute
    {
        CA_Hide             = 0x0100,
        CA_UpdateText       = 0x0200,
        CA_UpdateIcon       = 0x0400,
        CA_NonConfigureable = 0x8000,
        CA_Mask             = 0xFF00
    };

// Construction and Destruction
public:
    virtual ~CCommand() {}

// Operations
public:
    virtual void            SetDefaultKeySequence(const QKeySequence &ks) = 0;
    virtual QKeySequence    GetDefaultKeySequence() const = 0;

    virtual void            SetKeySequence(const QKeySequence &ks) = 0;
    virtual QKeySequence    GetKeySequence() const = 0;

    virtual void            SetDefaultText(const QString &strText) = 0;
    virtual QString         GetDefaultText() const = 0;

    virtual int             GetID() const = 0;

    virtual QAction*        GetAction() const = 0;
    virtual QShortcut*      GetShortcut() const = 0;
    virtual QList<int>      GetContext() const = 0;

    virtual void            SetAttribute(CommandAttribute eCommandAttribute) = 0;
    virtual void            RemoveAttribute(CommandAttribute eCommandAttribute) = 0;
    virtual bool            HasAttribute(CommandAttribute eCommandAttribute) const = 0;

    virtual bool            IsActive() const = 0;

    virtual QString         GetStringWithAppendedShortcut(const QString &str) const = 0;

signals:
    void sigKeySequenceChanged();
    void sigActiveStateChanged();
};

} // namespace Core

#endif // COMMAND_H
