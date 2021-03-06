#ifndef CUBICLECONNMODE_H
#define CUBICLECONNMODE_H

#include <QPointer>
#include "core/imode.h"

namespace PhyNetwork {
namespace Internal {

class CubicleConnModeWidget;
class CubicleConnMode : public Core::IMode
{
    Q_OBJECT

// Construction and Destruction
public:
    CubicleConnMode(QObject *pParent = 0);
    ~CubicleConnMode();

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
    QPointer<CubicleConnModeWidget> m_pCubicleConnModeWidget;
};

} // namespace Internal
} // namespace PhyNetwork

#endif // CUBICLECONNMODE_H
