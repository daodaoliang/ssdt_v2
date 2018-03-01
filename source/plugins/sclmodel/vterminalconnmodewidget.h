#ifndef VTERMINALCONNMODEWIDGET_H
#define VTERMINALCONNMODEWIDGET_H

#include "styledui/styledwidget.h"

QT_BEGIN_NAMESPACE
class QSplitter;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeProjectVersion;
class PeProjectObject;

} // namespace ProjectExplorer

namespace SclModel {
namespace Internal {

class ConfigWidgetVTerminalConn;
class WidgetVTerminalConn;
class VTerminalConnModeWidget : public StyledUi::StyledWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    VTerminalConnModeWidget(QWidget *pParent = 0);
    ~VTerminalConnModeWidget();

// Operations
public:
    void SetProjectObject(ProjectExplorer::PeProjectObject *pProjectObject);

// Properties
private:
    ProjectExplorer::PeProjectVersion   *m_pProjectVersion;
    QSplitter                           *m_pSplitter;
    ConfigWidgetVTerminalConn           *m_pConfigWidgetVTerminalConn;
    WidgetVTerminalConn                 *m_pWidgetVTerminalConn;
};

} // namespace Internal
} // namespace DevExplorer

#endif // VTERMINALCONNMODEWIDGET_H
