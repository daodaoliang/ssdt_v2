#ifndef WIDGETCUBICLECONN_H
#define WIDGETCUBICLECONN_H

#include "styledui/styledwidget.h"
#include "projectexplorer/pevterminalconn.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QCheckBox;
class QGroupBox;
class QAbstractButton;
class QMenu;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeProjectVersion;
class PeCubicle;
class PeCubicleConn;
class PeDevice;

} // namespace ProjectExplorer

namespace PhyNetwork {
namespace Internal {

class WidgetCubicleConn : public StyledUi::StyledWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    WidgetCubicleConn(QWidget *pParent = 0);
    ~WidgetCubicleConn();

// Operations
public:
    void                                SetProjectVersion(ProjectExplorer::PeProjectVersion *pProjectVersion, ProjectExplorer::PeCubicle *pCubicle);
    QList<QAction*>                     GetActions() const;

private:
    QWidget*                            SetupCurrentCubicle();
    QWidget*                            SetupCurrentPassCubicle();
    QWidget*                            SetupSideCubicle();
    QWidget*                            SetupSidePassCubicle();

    void                                FillCurrentCubicle();
    void                                FillCurrentPassCubicle();
    void                                FillSideCubicle();
    void                                FillSidePassCubicle();

    ProjectExplorer::PeCubicle*         GetCubicle(QComboBox *pComboxCubicle) const;
    QList<ProjectExplorer::PeCubicle*>  GetAllCubicles(ProjectExplorer::PeProjectObject *pProjectObject);

    bool                                BlockSignals(bool bBlock);
    void                                SetUiData(ProjectExplorer::PeCubicleConn *pCubicleConn);
    bool                                GetUiData(ProjectExplorer::PeCubicleConn *pCubicleConn);

    void                                UpdateAction();
    bool                                UpdateCubicleConn(ProjectExplorer::PeCubicle *pCubicle);
    bool                                ClearInvalidInfoSet();

// Properties
private:
    ProjectExplorer::PeProjectVersion   *m_pProjectVersion;
    ProjectExplorer::PeCubicleConn      *m_pCubicleConn;

    QComboBox                           *m_pComboBoxCurrentCubicle;
    QCheckBox                           *m_pCheckBoxUseCurrentOdf;
    QGroupBox                           *m_pGroupBoxCurrentPassCubicle;
    QComboBox                           *m_pComboBoxCurrentPassCubicle;
    QComboBox                           *m_pComboBoxSideCubicle;
    QCheckBox                           *m_pCheckBoxUseSideOdf;
    QGroupBox                           *m_pGroupBoxSidePassCubicle;
    QComboBox                           *m_pComboBoxSidePassCubicle;

    QMenu                               *m_pMenuUpdate;
    QMenu                               *m_pMenuClear;
    QAction                             *m_pActionModify;
    QAction                             *m_pActionUpdate;
    QAction                             *m_pActionUpdateAll;
    QAction                             *m_pActionClear;
    QAction                             *m_pActionClearAll;

private slots:
    void SlotSetCubicleConn(ProjectExplorer::PeCubicleConn *pCubicleConn);
    void SlotCurrentCubicleChanged(int iIndex);
    void SlotSideCubicleChanged(int iIndex);
    void SlotPassCubicleToggled(bool bToggled);
    void SlotUpdateUseOdfStatus(bool bToggled);
    void SlotActionModify();
    void SlotActionUpdate();
    void SlotActionUpdateAll();
    void SlotActionClear();
    void SlotActionClearAll();

signals:
    void sigCurrentCubicleChanged(ProjectExplorer::PeCubicle *pCurrentCubicle);
};

} // namespace Internal
} // namespace PhyNetwork

#endif // WIDGETCUBICLECONN_H
