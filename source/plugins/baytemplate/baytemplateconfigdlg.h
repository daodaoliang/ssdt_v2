#ifndef BAYTEMPLATECONFIGDLG_H
#define BAYTEMPLATECONFIGDLG_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QTabBar;
class QStackedWidget;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PbTpBay;

} // namespace ProjectExplorer

namespace BayTemplate {
namespace Internal {

class ConfigWidgetTpDevice;
class BayTemplateConfigDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    BayTemplateConfigDlg(ProjectExplorer::PbTpBay *pBay, QWidget *pParent = 0);

// Operations
public:
    virtual QSize       sizeHint() const;

// Properties
private:
    ProjectExplorer::PbTpBay    *m_pBay;

    QTabBar                     *m_pTabBar;
    QStackedWidget              *m_pStackedWidget;
    ConfigWidgetTpDevice        *m_pConfigWidgetTpDevice;

private slots:
    void SlotCurrentTabChanged(int iCurrentIndex);
};

} // namespace Internal
} // namespace BayTemplate

#endif // BAYTEMPLATECONFIGDLG_H
