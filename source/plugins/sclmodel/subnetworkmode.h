#ifndef SUBNETWORKMODE_H
#define SUBNETWORKMODE_H

#include <QPointer>
#include "core/imode.h"

namespace SclModel {
namespace Internal {

class SubNetworkModeWidget;
class SubNetworkMode : public Core::IMode
{
    Q_OBJECT

// Construction and Destruction
public:
    SubNetworkMode(QObject *pParent = 0);
    ~SubNetworkMode();

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
    QPointer<SubNetworkModeWidget>   m_pSubNetworkModeWidget;
};

} // namespace Internal
} // namespace SclModel

#endif // SUBNETWORKMODE_H
