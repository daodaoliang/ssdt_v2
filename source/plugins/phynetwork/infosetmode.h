#ifndef INFOSETMODE_H
#define INFOSETMODE_H

#include <QPointer>
#include "core/imode.h"

namespace PhyNetwork {
namespace Internal {

class InfoSetModeWidget;
class InfoSetMode : public Core::IMode
{
    Q_OBJECT

// Construction and Destruction
public:
    InfoSetMode(QObject *pParent = 0);
    ~InfoSetMode();

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
    QPointer<InfoSetModeWidget> m_pInfoSetModeWidget;
};

} // namespace Internal
} // namespace PhyNetwork

#endif // INFOSETMODE_H
