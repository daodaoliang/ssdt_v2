#include <QTimer>
#include <QAction>
#include <QMenuBar>

#include "actioncontainer_p.h"
#include "core_constants.h"
#include "uniqueidmanager.h"
#include "actionmanager_p.h"
#include "command_p.h"

Q_DECLARE_METATYPE(Core::Internal::CMenuActionContainer*)

using namespace Core;
using namespace Core::Internal;

///////////////////////////////////////////////////////////////////////
// CActionContainerPrivate member functions
///////////////////////////////////////////////////////////////////////
CActionContainerPrivate::CActionContainerPrivate(int nID) : m_iEmptyAction(0), m_nID(nID), m_bUpdateRequested(false)
{
}

void CActionContainerPrivate::SetEmptyAction(EmptyAction eEmptyAction)
{
    m_iEmptyAction = ((m_iEmptyAction & ~EA_Mask) | eEmptyAction);
}

bool CActionContainerPrivate::HasEmptyAction(EmptyAction eEmptyAction) const
{
    return ((m_iEmptyAction & EA_Mask) == eEmptyAction);
}

void CActionContainerPrivate::AppendGroup(const QString &strGroupID)
{
    int nGroupID = CUniqueIDManager::Instance()->GenerateUniqueIdentifier(strGroupID);
    m_lstGroupIDs << nGroupID;
}

QAction* CActionContainerPrivate::InsertLocation(const QString &strGroup) const
{
    int nGroupID = CUniqueIDManager::Instance()->GenerateUniqueIdentifier(strGroup);
    int iPrevKey = 0;
    int iPos = ((nGroupID << 16) | 0xFFFF);
    return GetBeforeAction(iPos, &iPrevKey);
}

void CActionContainerPrivate::AddAction(Core::CCommand *pAction, const QString &strGroupID)
{
    if(!CanAddAction(pAction))
        return;

    CActionManagerPrivate *pActionManagerPrivate = CActionManagerPrivate::Instance();
    CUniqueIDManager *pUniqueIDManager = CUniqueIDManager::Instance();
    int nGroupID = pUniqueIDManager->GenerateUniqueIdentifier(Constants::g_szUID_GroupDefaultTwo);

    if(!strGroupID.isEmpty())
        nGroupID = pUniqueIDManager->GenerateUniqueIdentifier(strGroupID);

    if(!m_lstGroupIDs.contains(nGroupID) && !pActionManagerPrivate->GetDefaultGroups().contains(nGroupID))
        qWarning("CActionContainerPrivate::AddAction(): Unknown group ID: %s", strGroupID);

    int iPos = ((nGroupID << 16) | 0xFFFF);
    AddAction(pAction, iPos, true);
}

void CActionContainerPrivate::AddMenu(CActionContainer *pMenu, const QString &strGroupID)
{
    CActionContainerPrivate *pActionContainerPrivate = static_cast<CActionContainerPrivate*>(pMenu);
    if(!pActionContainerPrivate->CanBeAddedToMenu())
        return;

    CActionManagerPrivate *pActionManagerPrivate = CActionManagerPrivate::Instance();
    CUniqueIDManager *pUniqueIDManager = CUniqueIDManager::Instance();
    int nGroupID = pUniqueIDManager->GenerateUniqueIdentifier(Constants::g_szUID_GroupDefaultTwo);

    if(!strGroupID.isEmpty())
        nGroupID = pUniqueIDManager->GenerateUniqueIdentifier(strGroupID);

    if(!m_lstGroupIDs.contains(nGroupID) && !pActionManagerPrivate->GetDefaultGroups().contains(nGroupID))
        qWarning("CActionContainerPrivate::AddMenu(): Unknown group ID: %s", strGroupID);

    int iPos = ((nGroupID << 16) | 0xFFFF);
    AddMenu(pMenu, iPos, true);
}

int CActionContainerPrivate::GetID() const
{
    return m_nID;
}

QMenu *CActionContainerPrivate::GetMenu() const
{
    return 0;
}

QMenuBar *CActionContainerPrivate::GetMenuBar() const
{
    return 0;
}

bool CActionContainerPrivate::CanAddAction(CCommand *pAction) const
{
    return (pAction->GetAction() != 0);
}

void CActionContainerPrivate::AddAction(CCommand *pCommand, int iPos, bool bSetPos)
{
    CAction *pAction = static_cast<CAction*>(pCommand);

    int iPrevKey = 0;
    QAction *pActionBefore = GetBeforeAction(iPos, &iPrevKey);

    if(bSetPos)
    {
        iPos = CalcPosition(iPos, iPrevKey);
        CommandLocation cl;
        cl.m_container = m_nID;
        cl.m_position = iPos;

        QList<CommandLocation> lstLocations = pAction->GetLocations();
        lstLocations.append(cl);
        pAction->SetLocations(lstLocations);
    }

    m_lstCommands.append(pCommand);
    m_mapActionPosID.insert(iPos, pCommand->GetID());

    connect(pCommand, SIGNAL(sigActiveStateChanged()), this, SLOT(SlotScheduleUpdate()));
    InsertAction(pActionBefore, pAction->GetAction());
}

void CActionContainerPrivate::AddMenu(CActionContainer *pMenu, int iPos, bool bSetPos)
{
    CMenuActionContainer *pMenuActionContainer = static_cast<CMenuActionContainer*>(pMenu);

    int iPrevKey = 0;
    QAction *pAction = GetBeforeAction(iPos, &iPrevKey);

    if(bSetPos)
    {
        iPos = CalcPosition(iPos, iPrevKey);
        CommandLocation cl;
        cl.m_container = m_nID;
        cl.m_position = iPos;
        pMenuActionContainer->SetLocation(cl);
    }

    m_lstSubContainers.append(pMenu);
    m_mapActionPosID.insert(iPos, pMenu->GetID());
    InsertMenu(pAction, pMenuActionContainer->GetMenu());
}

QAction* CActionContainerPrivate::GetBeforeAction(int iPos, int *pPrevKey) const
{
    CActionManagerPrivate *pActionManagerPrivate = CActionManagerPrivate::Instance();

    int iBeforeActionID = -1;
    (*pPrevKey) = -1;

    QMap<int, int>::const_iterator it = m_mapActionPosID.constBegin();
    while(it != m_mapActionPosID.constEnd())
    {
        if(it.key() > iPos)
        {
            iBeforeActionID = it.value();
            break;
        }

        (*pPrevKey) = it.key();

        ++it;
    }

    if(iBeforeActionID == -1)
        return 0;

    if(CCommand *pCommand = pActionManagerPrivate->GetCommand(iBeforeActionID))
        return pCommand->GetAction();

    if(CActionContainer *pActionContainer = pActionManagerPrivate->GetActionContainer(iBeforeActionID))
    {
        if(QMenu *pMenu = pActionContainer->GetMenu())
            return pMenu->menuAction();
    }

    return 0;
}

int CActionContainerPrivate::CalcPosition(int iPos, int iPrevKey) const
{
    int grp = (iPos & 0xFFFF0000);
    if (iPrevKey == -1)
        return grp;

    int prevgrp = (iPrevKey & 0xFFFF0000);

    if (grp != prevgrp)
        return grp;

    return grp + (iPrevKey & 0xFFFF) + 10;
}

void CActionContainerPrivate::SlotScheduleUpdate()
{
    if(m_bUpdateRequested)
        return;

    m_bUpdateRequested = true;
    QTimer::singleShot(0, this, SLOT(SlotUpdate()));
}

void CActionContainerPrivate::SlotUpdate()
{
    UpdateInternal();
    m_bUpdateRequested = false;
}

///////////////////////////////////////////////////////////////////////
// CMenuActionContainer member functions
///////////////////////////////////////////////////////////////////////
CMenuActionContainer::CMenuActionContainer(int nID) : CActionContainerPrivate(nID), m_pMenu(0)
{
    SetEmptyAction(EA_Disable);
}

void CMenuActionContainer::SetMenu(QMenu *pMenu)
{
    m_pMenu = pMenu;

    QVariant var;
    qVariantSetValue<CMenuActionContainer*>(var, this);

    m_pMenu->menuAction()->setData(var);
}

QMenu *CMenuActionContainer::GetMenu() const
{
    return m_pMenu;
}

void CMenuActionContainer::InsertAction(QAction *pActionBefore, QAction *pAction)
{
    m_pMenu->insertAction(pActionBefore, pAction);
}

void CMenuActionContainer::InsertMenu(QAction *pActionBefore, QMenu *pMenu)
{
    m_pMenu->insertMenu(pActionBefore, pMenu);
}

void CMenuActionContainer::SetLocation(const CommandLocation &location)
{
    m_location = location;
}

CommandLocation CMenuActionContainer::GetLocation() const
{
    return m_location;
}

bool CMenuActionContainer::UpdateInternal()
{
    if(HasEmptyAction(EA_None))
        return true;

    bool bHasItems = false;

    foreach(CActionContainer *pActionContainer, GetSubContainers())
    {
        if(pActionContainer == this)
        {
            qWarning("CActionContainer %s contains itself as subcontainer", (this->GetMenu() ? this->GetMenu()->title() : ""));
            continue;
        }

        if(qobject_cast<CActionContainerPrivate*>(pActionContainer)->UpdateInternal())
        {
            bHasItems = true;
            break;
        }
    }
    if(!bHasItems)
    {
        foreach(CCommand *pCommand, GetCommands())
        {
            if(pCommand->IsActive())
            {
                bHasItems = true;
                break;
            }
        }
    }

    if(HasEmptyAction(EA_Hide))
        m_pMenu->setVisible(bHasItems);
    else if(HasEmptyAction(EA_Disable))
        m_pMenu->setEnabled(bHasItems);

    return bHasItems;
}

bool CMenuActionContainer::CanBeAddedToMenu() const
{
    return true;
}

///////////////////////////////////////////////////////////////////////
// CMenuBarActionContainer member functions
///////////////////////////////////////////////////////////////////////
CMenuBarActionContainer::CMenuBarActionContainer(int nID) : CActionContainerPrivate(nID), m_pMenuBar(0)
{
    SetEmptyAction(EA_None);
}

void CMenuBarActionContainer::SetMenuBar(QMenuBar *pMenuBar)
{
    m_pMenuBar = pMenuBar;
}

QMenuBar *CMenuBarActionContainer::GetMenuBar() const
{
    return m_pMenuBar;
}

void CMenuBarActionContainer::InsertAction(QAction *pActionBefore, QAction *pAction)
{
    m_pMenuBar->insertAction(pActionBefore, pAction);
}

void CMenuBarActionContainer::InsertMenu(QAction *pActionBefore, QMenu *pMenu)
{
    m_pMenuBar->insertMenu(pActionBefore, pMenu);
}

bool CMenuBarActionContainer::UpdateInternal()
{
    if(HasEmptyAction(EA_None))
        return true;

    bool bHasItems = false;
    foreach(QAction *pAction, m_pMenuBar->actions())
    {
        if(pAction->isVisible())
        {
            bHasItems = true;
            break;
        }
    }

    if(HasEmptyAction(EA_Hide))
        m_pMenuBar->setVisible(bHasItems);
    else if(HasEmptyAction(EA_Disable))
        m_pMenuBar->setEnabled(bHasItems);

    return bHasItems;
}

bool CMenuBarActionContainer::CanBeAddedToMenu() const
{
    return false;
}

