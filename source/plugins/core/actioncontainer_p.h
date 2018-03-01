#ifndef ACTIONCONTAINER_P_H
#define ACTIONCONTAINER_P_H

#include "actioncontainer.h"
#include "actionmanager_p.h"

namespace Core {

class CCommand;

namespace Internal {

class CActionContainerPrivate : public Core::CActionContainer
{
    Q_OBJECT

// Construction and Destruction
public:
    CActionContainerPrivate(int nID);
    virtual ~CActionContainerPrivate() {}

// Operations
public:
    virtual void        SetEmptyAction(EmptyAction eEmptyAction);
    bool                HasEmptyAction(EmptyAction eEmptyAction) const;

    virtual QAction*    InsertLocation(const QString &strGroup) const;
    virtual void        AppendGroup(const QString &strGroupID);
    virtual void        AddAction(CCommand *pAction, const QString &strGroupID = QString());
    virtual void        AddMenu(CActionContainer *pMenu, const QString &strGroupID = QString());

    virtual int         GetID() const;

    virtual QMenu*      GetMenu() const;
    virtual QMenuBar*   GetMenuBar() const;

    virtual void        InsertAction(QAction *pActionBefore, QAction *pAction) = 0;
    virtual void        InsertMenu(QAction *pActionBefore, QMenu *pMenu) = 0;

    virtual bool        UpdateInternal() = 0;

    QList<CCommand*>            GetCommands() const { return m_lstCommands; }
    QList<CActionContainer*>    GetSubContainers() const { return m_lstSubContainers; }

protected:
    bool            CanAddAction(CCommand *pAction) const;
    virtual bool    CanBeAddedToMenu() const = 0;

    void            AddAction(CCommand *pCommand, int iPos, bool bSetPos);
    void            AddMenu(CActionContainer *pMenu, int iPos, bool bSetPos);

private:
    QAction*    GetBeforeAction(int iPos, int *pPrevKey) const;
    int         CalcPosition(int iPos, int iPrevKey) const;

// Properties
private:
    int                         m_nID;
    QList<int>                  m_lstGroupIDs;
    int                         m_iEmptyAction;
    QMap<int, int>              m_mapActionPosID;
    QList<CActionContainer*>    m_lstSubContainers;
    QList<CCommand*>            m_lstCommands;
    bool                        m_bUpdateRequested;

private slots:
    void SlotScheduleUpdate();
    void SlotUpdate();
};

class CMenuActionContainer : public CActionContainerPrivate
{
// Construction and Destruction
public:
    CMenuActionContainer(int nID);

// Operations
public:
    void            SetMenu(QMenu *pMenu);
    virtual QMenu*  GetMenu() const;

    void            SetLocation(const CommandLocation &location);
    CommandLocation GetLocation() const;

    virtual void    InsertAction(QAction *pActionBefore, QAction *pAction);
    virtual void    InsertMenu(QAction *pActionBefore, QMenu *pMenu);

protected:
    virtual bool    CanBeAddedToMenu() const;
    virtual bool    UpdateInternal();

// Properties
private:
    QMenu           *m_pMenu;
    CommandLocation m_location;
};

class CMenuBarActionContainer : public CActionContainerPrivate
{
// Construction and Destruction
public:
    CMenuBarActionContainer(int nID);

// Operations
public:
    void                SetMenuBar(QMenuBar *pMenuBar);
    virtual QMenuBar*   GetMenuBar() const;

    virtual void        InsertAction(QAction *pActionBefore, QAction *pAction);
    virtual void        InsertMenu(QAction *pActionBefore, QMenu *pMenu);

protected:
    virtual bool        CanBeAddedToMenu() const;
    virtual bool        UpdateInternal();

// Properties
private:
    QMenuBar *m_pMenuBar;
};

} // namespace Internal
} // namespace Core

#endif // ACTIONCONTAINER_P_H
