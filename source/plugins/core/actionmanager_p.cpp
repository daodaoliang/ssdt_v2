#include <QMenu>
#include <QAction>
#include <QShortcut>
#include <QMenuBar>

#include "actionmanager_p.h"
#include "core_constants.h"
#include "mainwindow.h"
#include "uniqueidmanager.h"
#include "actioncontainer_p.h"
#include "command_p.h"

using namespace Core;
using namespace Core::Internal;

CActionManagerPrivate* CActionManagerPrivate::m_pInstance = 0;

CActionManagerPrivate::CActionManagerPrivate(Core::MainWindow *pMainWindow) : CActionManager(pMainWindow), m_pMainWindow(pMainWindow)
{
    CUniqueIDManager *pUniqueIDManager = CUniqueIDManager::Instance();
    m_lstDefaultGroups << pUniqueIDManager->GenerateUniqueIdentifier(Constants::g_szUID_GroupDefaultOne);
    m_lstDefaultGroups << pUniqueIDManager->GenerateUniqueIdentifier(Constants::g_szUID_GroupDefaultTwo);
    m_lstDefaultGroups << pUniqueIDManager->GenerateUniqueIdentifier(Constants::g_szUID_GroupDefaultThree);
    m_pInstance = this;
}

CActionManagerPrivate::~CActionManagerPrivate()
{
    qDeleteAll(m_hashIDCommand.values());
    qDeleteAll(m_hashIDContainer.values());
}

CActionManagerPrivate* CActionManagerPrivate::Instance()
{
    return m_pInstance;
}

QList<int> CActionManagerPrivate::GetDefaultGroups() const
{
    return m_lstDefaultGroups;
}

QList<CCommand*> CActionManagerPrivate::GetCommands() const
{
    QList<CCommand*> lstCommands;

    foreach(CCommand *pCommand, m_hashIDCommand.values())
        lstCommands << pCommand;

    return lstCommands;
}

QList<CActionContainerPrivate*> CActionManagerPrivate::GetContainers() const
{
    return m_hashIDContainer.values();
}

bool CActionManagerPrivate::HasContext(int iContext) const
{
    return m_lstContext.contains(iContext);
}

void CActionManagerPrivate::SetContext(const QList<int> &lstContext)
{
    m_lstContext = lstContext;

    for(QHash<int, CCommandPrivate*>::const_iterator it = m_hashIDCommand.constBegin(); it != m_hashIDCommand.constEnd(); ++it)
        it.value()->SetCurrentContext(m_lstContext);
}

bool CActionManagerPrivate::HasContext(QList<int> lstContext) const
{
    foreach(int iContext, m_lstContext)
    {
        if(lstContext.contains(iContext))
            return true;
    }

    return false;
}

CActionContainer* CActionManagerPrivate::CreateMenu(const QString &strMenuID)
{
    const int nID = CUniqueIDManager::Instance()->GenerateUniqueIdentifier(strMenuID);
    const QHash<int, CActionContainerPrivate*>::const_iterator it = m_hashIDContainer.constFind(nID);
    if(it != m_hashIDContainer.constEnd())
        return it.value();

    QMenu *pMenu = new QMenu(m_pMainWindow);
    pMenu->setObjectName(strMenuID);

    CMenuActionContainer *pMenuActionContainer = new CMenuActionContainer(nID);
    pMenuActionContainer->SetMenu(pMenu);
    m_hashIDContainer.insert(nID, pMenuActionContainer);

    return pMenuActionContainer;
}

CActionContainer* CActionManagerPrivate::CreateMenuBar(const QString &strID)
{
    const int nID = CUniqueIDManager::Instance()->GenerateUniqueIdentifier(strID);
    const QHash<int, CActionContainerPrivate*>::const_iterator it = m_hashIDContainer.constFind(nID);
    if (it !=  m_hashIDContainer.constEnd())
        return it.value();

    QMenuBar *pMenuBar = new QMenuBar;
    pMenuBar->setObjectName(strID);

    CMenuBarActionContainer *pMenuBarActionContainer = new CMenuBarActionContainer(nID);
    pMenuBarActionContainer->SetMenuBar(pMenuBar);
    m_hashIDContainer.insert(nID, pMenuBarActionContainer);

    return pMenuBarActionContainer;
}

CCommand* CActionManagerPrivate::RegisterAction(QAction *pAction, const QString &strActionID, const QList<int> &lstContext)
{
    CCommand *pCommand = RegisterOverridableAction(pAction, strActionID, false);

    CAction *pActionReturned = static_cast<CAction*>(pCommand);
    if(pActionReturned)
        pActionReturned->AddOverrideAction(pAction, lstContext);

    return pActionReturned;
}

CCommand* CActionManagerPrivate::RegisterOverridableAction(QAction *pAction, const QString &strActionID, bool bCheckUnique)
{
    CAction *pActionReturned = 0;
    const int nID = CUniqueIDManager::Instance()->GenerateUniqueIdentifier(strActionID);

    if(CCommandPrivate *pCommandPrivate = m_hashIDCommand.value(nID, 0))
    {
        pActionReturned = qobject_cast<CAction*>(pCommandPrivate);
        if(!pActionReturned)
        {
            qWarning("CActionManagerPrivate::RegisterOverridableAction: %s is registered with a different command type.", strActionID);
            return pCommandPrivate;
        }
    }
    else
    {
        pActionReturned = new CAction(nID);
        m_hashIDCommand.insert(nID, pActionReturned);
    }

    if(!pActionReturned->GetAction())
    {
        QAction *pBaseAction = new QAction(m_pMainWindow);
        pBaseAction->setObjectName(strActionID);
        pBaseAction->setCheckable(pAction->isCheckable());
        pBaseAction->setIcon(pAction->icon());
        pBaseAction->setIconText(pAction->iconText());
        pBaseAction->setText(pAction->text());
        pBaseAction->setToolTip(pAction->toolTip());
        pBaseAction->setStatusTip(pAction->statusTip());
        pBaseAction->setWhatsThis(pAction->whatsThis());
        pBaseAction->setChecked(pAction->isChecked());
        pBaseAction->setSeparator(pAction->isSeparator());
        pBaseAction->setShortcutContext(Qt::ApplicationShortcut);
        pBaseAction->setEnabled(false);
        pBaseAction->setParent(m_pMainWindow);
        pBaseAction->setIconVisibleInMenu(pAction->isIconVisibleInMenu());

        pActionReturned->setAction(pBaseAction);
        m_pMainWindow->addAction(pBaseAction);
        pActionReturned->SetKeySequence(pActionReturned->GetKeySequence());
        pActionReturned->SetDefaultKeySequence(QKeySequence());
    }
    else if(bCheckUnique)
    {
        qWarning("CActionManagerPrivate::RegisterOverridableAction: id %s is already registered.", strActionID);
    }

    return pActionReturned;
}

CCommand* CActionManagerPrivate::RegisterShortcut(QShortcut *pShortcut, const QString &strID, const QList<int> &lstContext)
{
    CShortcut *pShortcutReturned = 0;
    const int nID = CUniqueIDManager::Instance()->GenerateUniqueIdentifier(strID);

    if(CCommandPrivate *pCommandPrivate = m_hashIDCommand.value(nID, 0))
    {
        pShortcutReturned = qobject_cast<CShortcut *>(pCommandPrivate);
        if(!pShortcutReturned)
        {
            qWarning("RegisterShortcut: id %s is registered with a different command type.", strID);
            return pCommandPrivate;
        }
    }
    else
    {
        pShortcutReturned = new CShortcut(nID);
        m_hashIDCommand.insert(nID, pShortcutReturned);
    }

    if(pShortcutReturned->GetShortcut())
    {
        qWarning("RegisterShortcut: action already registered (id %s).", strID);
        return pShortcutReturned;
    }

    if (!HasContext(lstContext))
        pShortcut->setEnabled(false);

    pShortcut->setObjectName(strID);
    pShortcut->setParent(m_pMainWindow);
    pShortcutReturned->SetShortcut(pShortcut);

    if(lstContext.isEmpty())
        pShortcutReturned->SetContext(QList<int>() << 0);
    else
        pShortcutReturned->SetContext(lstContext);

    pShortcutReturned->SetKeySequence(pShortcut->key());
    pShortcutReturned->SetDefaultKeySequence(QKeySequence());

    return pShortcutReturned;
}

CCommand* CActionManagerPrivate::GetCommand(const QString &strID) const
{
    const int nID = CUniqueIDManager::Instance()->GenerateUniqueIdentifier(strID);
    const QHash<int, CCommandPrivate*>::const_iterator it = m_hashIDCommand.constFind(nID);
    if(it == m_hashIDCommand.constEnd())
        return 0;

    return it.value();
}

CActionContainer* CActionManagerPrivate::GetActionContainer(const QString &strID) const
{
    const int nID = CUniqueIDManager::Instance()->GenerateUniqueIdentifier(strID);
    const QHash<int, CActionContainerPrivate*>::const_iterator it = m_hashIDContainer.constFind(nID);
    if(it == m_hashIDContainer.constEnd())
        return 0;

    return it.value();
}

CCommand* CActionManagerPrivate::GetCommand(int nID) const
{
    const QHash<int, CCommandPrivate*>::const_iterator it = m_hashIDCommand.constFind(nID);
    if(it == m_hashIDCommand.constEnd())
        return 0;

    return it.value();
}

CActionContainer *CActionManagerPrivate::GetActionContainer(int nID) const
{
    const QHash<int, CActionContainerPrivate*>::const_iterator it = m_hashIDContainer.constFind(nID);
    if(it == m_hashIDContainer.constEnd())
        return 0;

    return it.value();
}
