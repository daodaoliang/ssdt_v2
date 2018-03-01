#ifndef RENAMECABLEDLG_H
#define RENAMECABLEDLG_H

#include <QMap>
#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QStandardItem;
class QModelIndex;
class QComboBox;
class QLineEdit;
class QSpinBox;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTableView;

} // namespace Utils

namespace ProjectExplorer {

class PeProjectVersion;
class PeCable;

} // namespace ProjectExplorer

namespace PhyNetwork {
namespace Internal {

class RenameCableDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    RenameCableDlg(QWidget *pParent = 0);
    ~RenameCableDlg();

// Operations
public:
    virtual QSize       sizeHint() const { return QSize(900, 600); }

private:
    void                UpdateItemStatus(QStandardItemModel *pModel);

// Properties
private:
    QComboBox                   *m_pComboBoxProjectVersion;
    QStandardItemModel          *m_pModelOptical;
    Utils::ReadOnlyTableView    *m_pViewOptical;
    QStandardItemModel          *m_pModelTail;
    Utils::ReadOnlyTableView    *m_pViewTail;

    QList<ProjectExplorer::PeCable*> m_lstOpticalCables;
    QList<ProjectExplorer::PeCable*> m_lstTailCables;

private slots:
    void    SlotCurrentProjectVersionChanged(int iCurrentIndex);
    void    SlotViewDoubleClicked(const QModelIndex &index);

public slots:
    virtual void accept();
};

} // namespace Internal
} // namespace PhyNetwork

#endif // RENAMECABLEDLG_H
