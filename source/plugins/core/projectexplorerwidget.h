#ifndef PROJECTEXPLORERWIDGET_H
#define PROJECTEXPLORERWIDGET_H

#include <QWidget>
#include "core_global.h"

QT_BEGIN_NAMESPACE
class QModelIndex;
class QComboBox;
QT_END_NAMESPACE

namespace Utils {

class FancyLineEdit;
class ReadOnlyTreeView;

}

namespace Core {

class ProjectExplorerModel;
class ProjectExplorerFilterModel;

namespace Internal {

class CORE_EXPORT ProjectExplorerWidget : public QWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    ProjectExplorerWidget(QWidget *pParent = 0);
    ~ProjectExplorerWidget();

// Properties
private:
    QComboBox                   *m_pComboBoxFilterPhysical;
    Utils::FancyLineEdit        *m_pLineEditFilterPhysical;
    ProjectExplorerModel        *m_pModelPhysical;
    ProjectExplorerFilterModel  *m_pFilterModelPhysical;
    Utils::ReadOnlyTreeView     *m_pViewPhysical;

    QComboBox                   *m_pComboBoxFilterElectrical;
    Utils::FancyLineEdit        *m_pLineEditFilterElectrical;
    ProjectExplorerModel        *m_pModelElectrical;
    ProjectExplorerFilterModel  *m_pFilterModelElectrical;
    Utils::ReadOnlyTreeView     *m_pViewElectrical;

    QAction                     *m_pContextMenuActionExpandAllPhysical;
    QAction                     *m_pContextMenuActionCollapseAllPhysical;
    QAction                     *m_pContextMenuActionExpandAllElectrical;
    QAction                     *m_pContextMenuActionCollapseAllElectrical;

private slots:
    void SlotViewItemPressed(const QModelIndex &index);
    void SlotViewCurrentChanged(const QModelIndex & current, const QModelIndex & previous);
    void SlotModelRowsInserted(const QModelIndex &parent, int first, int last);
    void SlotViewCustomContextMenuRequested(const QPoint &point);
    void SlotFilterTextChanged(const QString &strFilter);
    void SlotFilterTypeChanged(int iCurrentIndex);
};

} // namespace Internal
} // namespace Core

#endif // PROJECTEXPLORERWIDGET_H
