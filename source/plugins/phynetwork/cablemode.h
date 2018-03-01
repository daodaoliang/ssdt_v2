#ifndef CABLEMODE_H
#define CABLEMODE_H

#include <QPointer>
#include "core/imode.h"

namespace PhyNetwork {
namespace Internal {

class CableModeWidget;
class CableMode : public Core::IMode
{
    Q_OBJECT

// Construction and Destruction
public:
    CableMode(QObject *pParent = 0);
    ~CableMode();

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
    QPointer<CableModeWidget> m_pCableModeWidget;
};

} // namespace Internal
} // namespace PhyNetwork

#endif // CABLEMODE_H
