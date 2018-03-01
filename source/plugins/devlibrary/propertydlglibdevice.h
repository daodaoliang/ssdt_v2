#ifndef PROPERTYMDLGLIBDEVICE_H
#define PROPERTYMDLGLIBDEVICE_H

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
class ReadOnlyTableView;

} // namespace Utils

namespace ProjectExplorer {

class PbLibDevice;
class PbLibBoard;
class PbLibPort;

} // namespace ProjectExplorer

namespace DevLibrary {
namespace Internal {

class PropertyDlgLibDevice : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgLibDevice(ProjectExplorer::PbLibDevice *pLibDevice, QWidget *pParent = 0);

// Operations
public:
    virtual QSize sizeHint() const { return QSize(1200, 600); }
    QByteArray GetIcdContent() const;

private:
    bool            UpdateData(bool bSaveAndValidate);
    void            BuildPortData();
    void            BuildTerminalData();
    QStandardItem*  InsertBoardItem(ProjectExplorer::PbLibBoard *pLibBoard);
    void            UpdateBoardItem(QStandardItem *pItemLibBoard);
    QStandardItem*  InsertPortItem(QStandardItem *pItemLibBoard, ProjectExplorer::PbLibPort *pLibPort);
    void            UpdatePortItem(QStandardItem *pItemLibPort);

// Properties
private:
    ProjectExplorer::PbLibDevice    *m_pLibDevice;

    QLineEdit                       *m_pLineEditType;
    QComboBox                       *m_pComboBoxManufacture;
    QComboBox                       *m_pComboBoxDeviceCategory;
    QComboBox                       *m_pComboBoxBayCategory;
    QLineEdit                       *m_pLineEditIcdFileName;

    QStandardItemModel              *m_pModelPort;
    Utils::ReadOnlyTreeView         *m_pViewPort;
    QStandardItemModel              *m_pModelTxTerminal;
    Utils::ReadOnlyTableView        *m_pViewTxTerminal;
    QStandardItemModel              *m_pModelRxTerminal;
    Utils::ReadOnlyTableView        *m_pViewRxTerminal;

    QAction                         *m_pActionNewBoard;
    QAction                         *m_pActionNewPort;
    QAction                         *m_pActionDelete;
    QAction                         *m_pActionProperty;

    QStandardItem                   *m_pCurrentPortItem;
    QByteArray                      m_baIcdContent;

public slots:
    virtual void accept();

private slots:
    void SlotViewPortSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void SlotViewPortDoubleClicked(const QModelIndex &index);
    void SlotBrowseClicked();
    void SlotActionNewBoard();
    void SlotActionNewPort();
    void SlotActionDelete();
    void SlotActionProperty();
};

} // namespace Internal
} // namespace DevLibrary

#endif // PROPERTYMDLGLIBDEVICE_H
