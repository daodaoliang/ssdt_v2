#ifndef CUBICLECONNMODEWIDGET_H
#define CUBICLECONNMODEWIDGET_H

#include "styledui/styledwidget.h"

QT_BEGIN_NAMESPACE
class QSplitter;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeProjectVersion;
class PeProjectObject;

} // namespace ProjectExplorer

namespace PhyNetwork {
namespace Internal {

class ConfigWidgetCubicleConn;
class WidgetCubicleConn;
class CubicleConnModeWidget : public StyledUi::StyledWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    CubicleConnModeWidget(QWidget *pParent = 0);
    ~CubicleConnModeWidget();

// Operations
public:
    virtual void    SetProjectObject(ProjectExplorer::PeProjectObject *pProjectObject);

// Properties
private:
    ProjectExplorer::PeProjectVersion   *m_pProjectVersion;
    QSplitter                           *m_pSplitter;
    QWidget                             *m_pStyleBar;
    ConfigWidgetCubicleConn             *m_pConfigWidgetCubicleConn;
    WidgetCubicleConn                   *m_pWidgetCubicleConn;
};

} // namespace Internal
} // namespace PhyNetwork

#endif // CUBICLECONNMODEWIDGET_H
