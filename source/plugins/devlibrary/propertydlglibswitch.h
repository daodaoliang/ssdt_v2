#ifndef PROPERTYMDLGLIBSWITCH_H
#define PROPERTYMDLGLIBSWITCH_H

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

class ReadOnlyTreeView;

} // namespace Utils

namespace ProjectExplorer {

class PbLibDevice;
class PbLibBoard;
class PbLibPort;

} // namespace ProjectExplorer

namespace DevLibrary {
namespace Internal {

class PropertyDlgLibSwitch : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgLibSwitch(ProjectExplorer::PbLibDevice *pLibDevice, QWidget *pParent = 0);

// Operations
public:
    virtual QSize sizeHint() const { return QSize(1200, 600); }

private:
    bool            UpdateData(bool bSaveAndValidate);
    ProjectExplorer::PbLibBoard* GetSwitchBoard() const;
    void            BuildPortData();
    QStandardItem*  InsertPortItem(ProjectExplorer::PbLibPort *pLibPort);
    void            UpdatePortItem(QStandardItem *pItemLibPort);

// Properties
private:
    ProjectExplorer::PbLibDevice *m_pLibDevice;

    QLineEdit               *m_pLineEditType;
    QComboBox               *m_pComboBoxManufacture;

    QStandardItemModel      *m_pModelPort;
    Utils::ReadOnlyTreeView *m_pViewPort;

    QAction                 *m_pActionNewPort;
    QAction                 *m_pActionDelete;
    QAction                 *m_pActionProperty;

    QStandardItem           *m_pCurrentPortItem;

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
} // namespace DevLibrary

#endif // PROPERTYMDLGLIBSWITCH_H
