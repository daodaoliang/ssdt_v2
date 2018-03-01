#ifndef CABLEWIDGET_H
#define CABLEWIDGET_H

#include "styledui/styledwidget.h"

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QLabel;
class QTabBar;
class QStackedWidget;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTreeView;

} // namespace Utils

namespace ProjectExplorer {

class PeCubicle;

} // namespace ProjectExplorer

class CableWidget : public StyledUi::StyledWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    CableWidget(QWidget *pParent = 0);
    ~CableWidget();

// Operations
public:
    ProjectExplorer::PeCubicle* GetCubicle() const;

private:
    void    SetupOpticalModelView();
    void    SetupTailModelView();
    void    SetupJumpModelView();

    void    BuildOpticalModel(ProjectExplorer::PeCubicle *pCubicle);
    void    BuildTailModel(ProjectExplorer::PeCubicle *pCubicle);
    void    BuildJumpModel(ProjectExplorer::PeCubicle *pCubicle);

// Property
private:
    ProjectExplorer::PeCubicle  *m_pCubicle;
    QTabBar                     *m_pTabBar;
    QStackedWidget              *m_pStackedWidget;

    QStandardItemModel          *m_pModelOptical;
    Utils::ReadOnlyTreeView     *m_pViewOptical;
    QStandardItemModel          *m_pModelTail;
    Utils::ReadOnlyTreeView     *m_pViewTail;
    QStandardItemModel          *m_pModelJump;
    Utils::ReadOnlyTreeView     *m_pViewJump;

public slots:
    void    SlotSetCubicle(ProjectExplorer::PeCubicle *pCubicle);
};

#endif // CABLEWIDGET_H
