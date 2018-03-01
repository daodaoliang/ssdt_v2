#ifndef PROPERTYMDLGTPDEVICE_H
#define PROPERTYMDLGTPDEVICE_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QStandardItemModel;
class QStandardItem;
class QItemSelection;
class QModelIndex;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTableView;

} // namespace Utils

namespace ProjectExplorer {

class PbTpDevice;
class PbTpPort;

} // namespace ProjectExplorer

namespace BayTemplate {
namespace Internal {

class PropertyDlgTpDevice : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgTpDevice(ProjectExplorer::PbTpDevice *pTpDevice, QWidget *pParent = 0);

// Operations
public:
    virtual QSize sizeHint() const { return QSize(800, 600); }

private:
    bool            UpdateData(bool bSaveAndValidate);
    void            BuildPortData();
    QStandardItem*  InsertPortItem(ProjectExplorer::PbTpPort *pTpPort);
    void            UpdatePortItem(QStandardItem *pItemTpPort);

// Properties
private:
    ProjectExplorer::PbTpDevice     *m_pTpDevice;

    QLineEdit                       *m_pLineEditName;
    QStandardItemModel              *m_pModelPort;
    Utils::ReadOnlyTableView        *m_pViewPort;

    QAction                         *m_pActionNewPort;
    QAction                         *m_pActionDelete;
    QAction                         *m_pActionProperty;

public slots:
    virtual void accept();

private slots:
    void SlotViewPortSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void SlotViewPortDoubleClicked(const QModelIndex &index);
    void SlotActionNewPort();
    void SlotActionDelete();
    void SlotActionProperty();
};

} // namespace Internal
} // namespace BayTemplate

#endif // PROPERTYMDLGTPDEVICE_H
