#include <QAction>
#include <QShortcut>
#include <QTextStream>

#include "command_p.h"

using namespace Core::Internal;

///////////////////////////////////////////////////////////////////////
// CCommandPrivate member functions
///////////////////////////////////////////////////////////////////////
CCommandPrivate::CCommandPrivate(int nID) : m_iAttributes(0), m_nID(nID)
{
}

void CCommandPrivate::SetDefaultKeySequence(const QKeySequence &ks)
{
    m_ksDefault = ks;
}

QKeySequence CCommandPrivate::GetDefaultKeySequence() const
{
    return m_ksDefault;
}

void CCommandPrivate::SetDefaultText(const QString &strText)
{
    m_strDefaultText = strText;
}

QString CCommandPrivate::GetDefaultText() const
{
    return m_strDefaultText;
}

int CCommandPrivate::GetID() const
{
    return m_nID;
}

QAction* CCommandPrivate::GetAction() const
{
    return 0;
}

QShortcut* CCommandPrivate::GetShortcut() const
{
    return 0;
}

QList<int> CCommandPrivate::GetContext() const
{
    return m_lstContext;
}

void CCommandPrivate::SetAttribute(CommandAttribute eCommandAttribute)
{
    m_iAttributes |= eCommandAttribute;
}

void CCommandPrivate::RemoveAttribute(CommandAttribute eCommandAttribute)
{
    m_iAttributes &= ~eCommandAttribute;
}

bool CCommandPrivate::HasAttribute(CommandAttribute eCommandAttribute) const
{
    return (m_iAttributes & eCommandAttribute);
}

QString CCommandPrivate::GetStringWithAppendedShortcut(const QString &str) const
{
    return QString("%1 <span style=\"color: gray; font-size: small\">%2</span>").arg(str).arg(
            GetKeySequence().toString(QKeySequence::NativeText));
}

///////////////////////////////////////////////////////////////////////
// CShortcut member functions
///////////////////////////////////////////////////////////////////////
CShortcut::CShortcut(int nID) : CCommandPrivate(nID), m_pShortcut(0)
{
}

QString CShortcut::GetName() const
{
    return (m_pShortcut ? m_pShortcut->whatsThis() : QString());
}

void CShortcut::SetShortcut(QShortcut *pShortcut)
{
    m_pShortcut = pShortcut;
}

QShortcut* CShortcut::GetShortcut() const
{
    return m_pShortcut;
}

void CShortcut::SetContext(const QList<int> &lstContext)
{
    m_lstContext = lstContext;
}

QList<int> CShortcut::GetContext() const
{
    return m_lstContext;
}

void CShortcut::SetDefaultKeySequence(const QKeySequence &ks)
{
    if(m_pShortcut->key().isEmpty())
        SetKeySequence(ks);

    CCommandPrivate::SetDefaultKeySequence(ks);
}

void CShortcut::SetKeySequence(const QKeySequence &ks)
{
    m_pShortcut->setKey(ks);
    emit sigKeySequenceChanged();
}

QKeySequence CShortcut::GetKeySequence() const
{
    return m_pShortcut->key();
}

bool CShortcut::SetCurrentContext(const QList<int> &lstContext)
{
    foreach(int iContext, m_lstContext)
    {
        if(lstContext.contains(iContext))
        {
            if(!m_pShortcut->isEnabled())
            {
                m_pShortcut->setEnabled(true);
                emit sigActiveStateChanged();
            }
            return true;
        }
    }

    if (m_pShortcut->isEnabled())
    {
        m_pShortcut->setEnabled(false);
        emit sigActiveStateChanged();
    }

    return false;
}

bool CShortcut::IsActive() const
{
    return m_pShortcut->isEnabled();
}

///////////////////////////////////////////////////////////////////////
// CAction member functions
///////////////////////////////////////////////////////////////////////
CAction::CAction(int nID) : CCommandPrivate(nID),
                            m_pAction(0),
                            m_pCurrentAction(0),
                            m_bActive(false),
                            m_bContextInitialized(false)
{
}

QString CAction::GetName() const
{
    return (m_pAction ? m_pAction->text() : QString());
}

void CAction::setAction(QAction *pAction)
{
    m_pAction = pAction;

    if(m_pAction)
    {
        m_pAction->setParent(this);
        m_strToolTip = m_pAction->toolTip();
    }
}

QAction* CAction::GetAction() const
{
    return m_pAction;
}

void CAction::SetLocations(const QList<CommandLocation> &lstLocations)
{
    m_lstLocations = lstLocations;
}

QList<CommandLocation> CAction::GetLocations() const
{
    return m_lstLocations;
}

void CAction::SetDefaultKeySequence(const QKeySequence &ks)
{
    if(m_pAction->shortcut().isEmpty())
        SetKeySequence(ks);

    CCommandPrivate::SetDefaultKeySequence(ks);
}

void CAction::SetKeySequence(const QKeySequence &ks)
{
    m_pAction->setShortcut(ks);
    UpdateToolTipWithKeySequence();
    emit sigKeySequenceChanged();
}

void CAction::UpdateToolTipWithKeySequence()
{
    if (m_pAction->shortcut().isEmpty())
        m_pAction->setToolTip(m_strToolTip);
    else
        m_pAction->setToolTip(GetStringWithAppendedShortcut(m_strToolTip));
}

QKeySequence CAction::GetKeySequence() const
{
    return m_pAction->shortcut();
}

bool CAction::SetCurrentContext(const QList<int> &lstContext)
{
    m_lstContext = lstContext;

    QAction *pOldAction = m_pCurrentAction;
    m_pCurrentAction = 0;

    for(int i = 0; i < m_lstContext.size(); ++i)
    {
        if(QAction *pAction = m_mapContextAction.value(m_lstContext.at(i), 0))
        {
            m_pCurrentAction = pAction;
            break;
        }
    }

    if(m_pCurrentAction == pOldAction && m_bContextInitialized)
        return true;
    m_bContextInitialized = true;

    if(pOldAction)
    {
        disconnect(pOldAction, SIGNAL(changed()), this, SLOT(SlotActionChanged()));
        disconnect(m_pAction, SIGNAL(triggered(bool)), pOldAction, SIGNAL(triggered(bool)));
        disconnect(m_pAction, SIGNAL(toggled(bool)), pOldAction, SLOT(setChecked(bool)));
    }

    if(m_pCurrentAction)
    {
        connect(m_pCurrentAction, SIGNAL(changed()), this, SLOT(SlotActionChanged()));
        // we want to avoid the toggling semantic on slot trigger(), so we just connect the signals
        connect(m_pAction, SIGNAL(triggered(bool)), m_pCurrentAction, SIGNAL(triggered(bool)));
        // we need to update the checked state, so we connect to setChecked slot, which also fires a toggled signal
        connect(m_pAction, SIGNAL(toggled(bool)), m_pCurrentAction, SLOT(setChecked(bool)));

        SlotActionChanged();
        return true;
    }
    // no active/delegate action, "visible" action is not enabled/visible
    if(HasAttribute(CA_Hide))
        m_pAction->setVisible(false);

    m_pAction->setEnabled(false);
    SetActive(false);

    return false;
}

void CAction::AddOverrideAction(QAction *pAction, const QList<int> &lstContext)
{
    if(lstContext.isEmpty())
    {
        m_mapContextAction.insert(0, pAction);
    }
    else
    {
        foreach(int iContext, lstContext)
        {
            if(m_mapContextAction.contains(iContext))
                qWarning("CAction is already registered for context");

            m_mapContextAction.insert(iContext, pAction);
        }
    }
}

void CAction::SlotActionChanged()
{
    if(HasAttribute(CA_UpdateIcon))
    {
        m_pAction->setIcon(m_pCurrentAction->icon());
        m_pAction->setIconText(m_pCurrentAction->iconText());
        m_pAction->setIconVisibleInMenu(m_pCurrentAction->isIconVisibleInMenu());
    }
    if(HasAttribute(CA_UpdateText))
    {
        m_pAction->setText(m_pCurrentAction->text());
        m_strToolTip = m_pCurrentAction->toolTip();
        UpdateToolTipWithKeySequence();
        m_pAction->setStatusTip(m_pCurrentAction->statusTip());
        m_pAction->setWhatsThis(m_pCurrentAction->whatsThis());
    }

    m_pAction->setCheckable(m_pCurrentAction->isCheckable());
    bool bBlock = m_pAction->blockSignals(true);
    m_pAction->setChecked(m_pCurrentAction->isChecked());
    m_pAction->blockSignals(bBlock);

    m_pAction->setEnabled(m_pCurrentAction->isEnabled());
    m_pAction->setVisible(m_pCurrentAction->isVisible());
    SetActive(m_pAction->isEnabled() && m_pAction->isVisible() && !m_pAction->isSeparator());
}

bool CAction::IsActive() const
{
    return m_bActive;
}

void CAction::SetActive(bool bActive)
{
    if(bActive != m_bActive)
    {
        m_bActive = bActive;
        emit sigActiveStateChanged();
    }
}
