#ifndef COMMAND_P_H
#define COMMAND_P_H

#include "command.h"
#include "actionmanager_p.h"

#include <QList>
#include <QPointer>
#include <QKeySequence>

namespace Core {
namespace Internal {

class CCommandPrivate : public Core::CCommand
{
    Q_OBJECT

// Construction and Destruction
public:
    CCommandPrivate(int nID);
    virtual ~CCommandPrivate() {}

// Operations
public:
    virtual QString         GetName() const = 0;

    virtual void            SetDefaultKeySequence(const QKeySequence &ks);
    virtual QKeySequence    GetDefaultKeySequence() const;

    virtual void            SetDefaultText(const QString &strText);
    virtual QString         GetDefaultText() const;

    virtual int             GetID() const;

    virtual QAction*        GetAction() const;
    virtual QShortcut*      GetShortcut() const;
    virtual QList<int>      GetContext() const;


    virtual void            SetAttribute(CommandAttribute eCommandAttribute);
    virtual void            RemoveAttribute(CommandAttribute eCommandAttribute);
    virtual bool            HasAttribute(CommandAttribute eCommandAttribute) const;

    virtual bool            SetCurrentContext(const QList<int> &lstContext) = 0;

    virtual QString         GetStringWithAppendedShortcut(const QString &str) const;

// Properties
protected:
    QList<int>      m_lstContext;
    int             m_iAttributes;
    int             m_nID;
    QKeySequence    m_ksDefault;
    QString         m_strDefaultText;
};

class CShortcut : public CCommandPrivate
{
    Q_OBJECT

// Construction and Destruction
public:
    CShortcut(int nID);

// Operations
public:
    virtual QString         GetName() const;

    virtual void            SetDefaultKeySequence(const QKeySequence &ks);
    virtual void            SetKeySequence(const QKeySequence &ks);
    virtual QKeySequence    GetKeySequence() const;

    void                    SetShortcut(QShortcut *pShortcut);
    virtual QShortcut*      GetShortcut() const;

    void                    SetContext(const QList<int> &lstContext);
    virtual QList<int>      GetContext() const;
    virtual bool            SetCurrentContext(const QList<int> &lstContext);

    virtual bool            IsActive() const;

// Properties
private:
    QShortcut *m_pShortcut;
};

class CAction : public CCommandPrivate
{
    Q_OBJECT

// Construction and Destruction
public:
    CAction(int nID);

// Operations
public:
    virtual QString         GetName() const;

    virtual void            SetDefaultKeySequence(const QKeySequence &ks);
    virtual void            SetKeySequence(const QKeySequence &ks);
    virtual QKeySequence    GetKeySequence() const;

    virtual void            setAction(QAction *pAction);
    virtual QAction*        GetAction() const;

    void                    SetLocations(const QList<CommandLocation> &lstLocations);
    QList<CommandLocation>  GetLocations() const;

    virtual bool            SetCurrentContext(const QList<int> &lstContext);
    virtual bool            IsActive() const;
    void                    AddOverrideAction(QAction *pAction, const QList<int> &lstContext);

private:
    void                    UpdateToolTipWithKeySequence();
    void                    SetActive(bool bActive);

// Properties
private:
    QAction *m_pAction;
    QString m_strToolTip;
    bool    m_bActive;
    bool    m_bContextInitialized;

    QList<CommandLocation>          m_lstLocations;
    QPointer<QAction>               m_pCurrentAction;
    QMap<int, QPointer<QAction> >   m_mapContextAction;

private slots:
    void SlotActionChanged();
};

} // namespace Internal
} // namespace Core

#endif // COMMAND_P_H
