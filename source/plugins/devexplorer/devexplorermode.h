#ifndef DEVEXPLORERMODE_H
#define DEVEXPLORERMODE_H

#include <QPointer>
#include "core/imode.h"

namespace DevExplorer {
namespace Internal {

class DevExplorerModeWidget;
class DevExplorerMode : public Core::IMode
{
    Q_OBJECT

// Construction and Destruction
public:
    DevExplorerMode(QObject *pParent = 0);
    ~DevExplorerMode();

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
    QPointer<DevExplorerModeWidget> m_pDevExplorerModeWidget;
};

} // namespace Internal
} // namespace DevExplorer

#endif // DEVEXPLORERMODE_H
