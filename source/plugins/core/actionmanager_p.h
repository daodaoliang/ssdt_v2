#ifndef ACTIONMANAGERPRIVATE_H
#define ACTIONMANAGERPRIVATE_H

#include <QMap>
#include <QHash>
#include <QMultiHash>
#include "actionmanager.h"

struct CommandLocation
{
    int m_container;
    int m_position;
};

namespace Core {

class CUniqueIDManager;
class MainWindow;

namespace Internal {

class CActionContainerPrivate;
class CCommandPrivate;

class CActionManagerPrivate : public Core::CActionManager
{
    Q_OBJECT

// Construction and Destruction
public:
    explicit CActionManagerPrivate(Core::MainWindow *pMainWindow);
    ~CActionManagerPrivate();

// Operations
public:
    static CActionManagerPrivate*   Instance();

    void                            SetContext(const QList<int> &lstContext);
    QList<int>                      GetDefaultGroups() const;
    QList<CActionContainerPrivate*> GetContainers() const;
    bool                            HasContext(int iContext) const;
    CCommand*                       GetCommand(int nID) const;
    CActionContainer*               GetActionContainer(int nID) const;

    virtual CActionContainer*       CreateMenu(const QString &strMenuID);
    virtual CActionContainer*       CreateMenuBar(const QString &strID);

    virtual CCommand*               RegisterAction(QAction *pAction, const QString &strActionID, const QList<int> &lstContext);
    virtual CCommand*               RegisterShortcut(QShortcut *pShortcut, const QString &strID, const QList<int> &lstContext);

    virtual Core::CCommand*         GetCommand(const QString &strID) const;
    virtual Core::CActionContainer* GetActionContainer(const QString &strID) const;

    virtual QList<CCommand*>        GetCommands() const;

private:
    bool        HasContext(QList<int> lstContext) const;
    CCommand*   RegisterOverridableAction(QAction *pAction, const QString &strActionID, bool bCheckUnique);

// Properties
private:
    static CActionManagerPrivate *m_pInstance;

    Core::MainWindow    *m_pMainWindow;
    QList<int>          m_lstDefaultGroups;
    QList<int>          m_lstContext;

    QHash<int, CCommandPrivate*>            m_hashIDCommand;
    QHash<int, CActionContainerPrivate*>    m_hashIDContainer;
};

} // namespace Internal
} // namespace Core

#endif // ACTIONMANAGERPRIVATE_H
