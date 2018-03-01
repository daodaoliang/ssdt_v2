#ifndef OPENPROJECTDLG_H
#define OPENPROJECTDLG_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QItemSelection;
class QModelIndex;
class QDialogButtonBox;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTreeView;

} // namespace Utils

namespace ProjectExplorer {

class PeProjectVersion;

} // namespace ProjectExplorer

class OpenProjectDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    OpenProjectDlg(QWidget *pParent = 0);

// Operations
public:
    virtual QSize sizeHint() const { return QSize(650, 380); }

    ProjectExplorer::PeProjectVersion* GetSelectedProjectVersion() const;

// Properties
private:
    QStandardItemModel      *m_pModel;
    Utils::ReadOnlyTreeView *m_pView;
    QDialogButtonBox        *m_pDialogButtonBox;

    ProjectExplorer::PeProjectVersion   *m_pSelectedProjectVersion;

private slots:
    void    SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void    SlotViewDoubleClicked(const QModelIndex &index);
};

#endif // OPENPROJECTDLG_H
