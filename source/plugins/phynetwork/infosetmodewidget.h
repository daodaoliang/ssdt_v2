#ifndef INFOSETMODEWIDGET_H
#define INFOSETMODEWIDGET_H

#include "styledui/styledwidget.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QSplitter;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeBay;
class PeProjectObject;

} // namespace ProjectExplorer

namespace PhyNetwork {
namespace Internal {

class ConfigWidgetInfoSet;
class WidgetInfoSet;

class InfoSetModeWidget : public StyledUi::StyledWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    InfoSetModeWidget(QWidget *pParent = 0);
    ~InfoSetModeWidget();

// Operations
public:
    virtual void    SetProjectObject(ProjectExplorer::PeProjectObject *pProjectObject);

// Properties
private:
    ProjectExplorer::PeBay  *m_pBay;
    QSplitter               *m_pSplitter;
    QWidget                 *m_pStyleBar;
    QLabel                  *m_pLabelDisplayIcon;
    QLabel                  *m_pLabelDisplayName;
    ConfigWidgetInfoSet     *m_pConfigWidgetInfoSet;
    WidgetInfoSet           *m_pWidgetInfoSet;
};

} // namespace Internal
} // namespace PhyNetwork

#endif // INFOSETMODEWIDGET_H
