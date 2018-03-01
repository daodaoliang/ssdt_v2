#ifndef VTERMINALCONNMODE_H
#define VTERMINALCONNMODE_H

#include <QPointer>
#include "core/imode.h"

namespace SclModel {
namespace Internal {

class VTerminalConnModeWidget;
class VTerminalConnMode : public Core::IMode
{
    Q_OBJECT

// Construction and Destruction
public:
    VTerminalConnMode(QObject *pParent = 0);
    ~VTerminalConnMode();

// Operations
public:
    virtual QString     GetId() const;
    virtual QString     GetName() const;
    virtual QString     GetPixmap() const;
    virtual int         GetPriority() const;
    virtual QWidget*    GetWidget() const;
    virtual void        ActivateObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject);
    virtual void        SetContextMenuObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject);

// Properties
private:
    QPointer<VTerminalConnModeWidget>   m_pVTerminalConnModeWidget;
};

} // namespace Internal
} // namespace SclModel

#endif // VTERMINALCONNMODE_H
