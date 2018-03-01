#ifndef CABLEMODEWIDGET_H
#define CABLEMODEWIDGET_H

#include "styledui/styledwidget.h"
#include "utils/readonlyview.h"
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QLabel;
class QTabBar;
class QStackedWidget;
class QItemSelection;
class QModelIndex;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTreeView;

} // namespace Utils

namespace ProjectExplorer {

class PeProjectObject;
class PeCubicle;
class PeCable;
class PeFiber;
class PeDevice;
class PeBoard;

} // namespace ProjectExplorer


namespace PhyNetwork {
namespace Internal {

class CableModeWidget;
class StandardItemModelCable : public QStandardItemModel
{
    Q_OBJECT

// Construction and Destruction
public:
    StandardItemModelCable(int rows, int columns, CableModeWidget *pCableModeWidget);

// Operations
public:
    virtual QMimeData*  mimeData(const QModelIndexList &indexes) const;
    virtual QStringList mimeTypes() const;
    virtual bool        dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int /*column*/, const QModelIndex &parent);

// Properties
private:
    CableModeWidget *m_pCableModeWidget;
};

class TreeViewCable : public Utils::ReadOnlyTreeView
{
    Q_OBJECT

// Construction and Destruction
public:
    TreeViewCable(QWidget *pParent = 0);

// Operations
protected:
    virtual void dragMoveEvent(QDragMoveEvent * event);
    virtual void paintEvent (QPaintEvent * event);

// Properties
protected:
    QRect m_DropIndicatorRect;
};

class StandardItemModelOdf : public QStandardItemModel
{
    Q_OBJECT

// Construction and Destruction
public:
    StandardItemModelOdf(int rows, int columns, CableModeWidget *pCableModeWidget);

// Operations
public:
    virtual QMimeData*  mimeData(const QModelIndexList &indexes) const;
    virtual QStringList mimeTypes() const;
    virtual bool        dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int /*column*/, const QModelIndex &parent);

// Properties
private:
    CableModeWidget *m_pCableModeWidget;
};

class TreeViewOdf : public Utils::ReadOnlyTreeView
{
// Construction and Destruction
public:
    TreeViewOdf(QWidget *pParent = 0);

// Operations
protected:
    virtual void dragMoveEvent(QDragMoveEvent * event);
    virtual void paintEvent (QPaintEvent * event);

// Properties
protected:
    QRect m_DropIndicatorRect;
};

class CableModeWidget : public StyledUi::StyledWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    CableModeWidget(QWidget *pParent = 0);
    ~CableModeWidget();

// Operations
public:
    virtual void    SetProjectObject(ProjectExplorer::PeProjectObject *pProjectObject);
    void            Refresh();

private:
    QWidget*        SetupToolBar();
    void            SetupOpticalModelView();
    void            SetupTailModelView();
    void            SetupJumpModelView();
    void            SetupOdfModelView();

    void            BuildOpticalModel(ProjectExplorer::PeCubicle *pCubicle);
    void            BuildTailModel(ProjectExplorer::PeCubicle *pCubicle);
    void            BuildJumpModel(ProjectExplorer::PeCubicle *pCubicle);
    void            BuildOdfModel(ProjectExplorer::PeCubicle *pCubicle);

    bool            GetSelectedCable(Utils::ReadOnlyTreeView *pView, QList<ProjectExplorer::PeCable*> &lstSelctedCables);
    bool            GetSelectedFiber(Utils::ReadOnlyTreeView *pView, QList<ProjectExplorer::PeFiber*> &lstSelctedFibers);
    bool            GetSelectedOdf(QList<ProjectExplorer::PeDevice*> &lstSelctedOdfs);
    bool            GetSelectedLayer(QList<ProjectExplorer::PeBoard*> &lstSelctedLayers);

    void            UpdateActions();

// Property
private:
    ProjectExplorer::PeCubicle  *m_pCubicle;
    QWidget                     *m_pStyleBar;
    QTabBar                     *m_pTabBar;
    QStackedWidget              *m_pStackedWidget;

    QLabel                      *m_pLabelDisplayIcon;
    QLabel                      *m_pLabelDisplayName;
    QAction                     *m_pActionSettings;
    QAction                     *m_pActionRefresh;
    QAction                     *m_pActionClear;
    QAction                     *m_pActionNewOdf;
    QAction                     *m_pActionNewOdfLayer;
    QAction                     *m_pActionDelete;
    QAction                     *m_pActionProperty;
    QAction                     *m_pActionMerge;
    QAction                     *m_pActionSplit;

    QStandardItemModel          *m_pModelOptical;
    Utils::ReadOnlyTreeView     *m_pViewOptical;
    QStandardItemModel          *m_pModelTail;
    Utils::ReadOnlyTreeView     *m_pViewTail;
    QStandardItemModel          *m_pModelJump;
    Utils::ReadOnlyTreeView     *m_pViewJump;
    QStandardItemModel          *m_pModelOdf;
    Utils::ReadOnlyTreeView     *m_pViewOdf;

private slots:
    void SlotCurrentTabChanged(int iCurrentIndex);
    void SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void SlotActionSettings();
    void SlotActionRefresh();
    void SlotActionClear();
    void SlotActionNewOdf();
    void SlotActionNewOdfLayer();
    void SlotActionDelete();
    void SlotActionProperty();
    void SlotActionMerge();
    void SlotActionSplit();
    void SlotActionRenameCable();
};

} // namespace Internal
} // namespace PhyNetwork

#endif // CABLEMODEWIDGET_H
