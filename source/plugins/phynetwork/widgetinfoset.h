#ifndef WIDGETINFOSET_H
#define WIDGETINFOSET_H

#include <QTreeWidgetItem>
#include "styledui/styledwidget.h"
#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QTreeWidget;
class QGroupBox;
class QHBoxLayout;
class QAbstractButton;
class QStandardItemModel;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTableView;

} // namespace Utils


namespace ProjectExplorer {

class PeBay;
class PeDevice;
class PePort;
class PeInfoSet;

} // namespace ProjectExplorer

namespace PhyNetwork {
namespace Internal {

class SelectInfoSetDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    SelectInfoSetDlg(const QList<ProjectExplorer::PeInfoSet*> &lstInfoSets, QWidget *pParent);

// Operations
public:
    virtual QSize sizeHint() const { return QSize(800, 450); }
    ProjectExplorer::PeInfoSet* GetSelectedInfoSet() const;

// Properties
private:
    QStandardItemModel          *m_pModel;
    Utils::ReadOnlyTableView    *m_pView;
    QPushButton                 *m_pPushButtonOk;

private slots:
    void SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void SlotViewDoubleClicked(const QModelIndex &index);
};

class TreeWidgetItemPort : public QTreeWidgetItem
{
// Construction and Destruction
public:
    TreeWidgetItemPort(ProjectExplorer::PePort *pPortTx, ProjectExplorer::PePort *pPortRx, bool bSwitchPort);

// Operations
public:
    virtual void setData(int column, int role, const QVariant &value);

    ProjectExplorer::PePort* GetPortTx() const;
    ProjectExplorer::PePort* GetPortRx() const;

    QList<ProjectExplorer::PeInfoSet*> GetInfoSets() const;

    void UpdateConnectionInfo(const QList<ProjectExplorer::PeInfoSet*> &lstInfoSets);
    void CheckDirecPortConnection(const QList<ProjectExplorer::PeDevice*> &lstDevices);

private:
    void MarkConnectionInfo(bool bDirect);

// Property
private:
    ProjectExplorer::PePort             *m_pPortTx;
    ProjectExplorer::PePort             *m_pPortRx;
    bool                                m_bSwitchPort;

    ProjectExplorer::PePort             *m_pPortConnectedTx;
    ProjectExplorer::PePort             *m_pPortConnectedRx;
    QList<ProjectExplorer::PeInfoSet*>  m_lstInfoSets;
};

class WidgetInfoSet : public StyledUi::StyledWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    WidgetInfoSet(QWidget *pParent = 0);
    ~WidgetInfoSet();

// Operations
public:
    void                        SetBay(ProjectExplorer::PeBay *pBay, ProjectExplorer::PeDevice *pDevice);
    QList<QAction*>             GetActions() const;

private:
    QWidget*                    SetupGeneralForward();
    QWidget*                    SetupGeneralBackward();
    QWidget*                    SetupDeviceForward();
    QWidget*                    SetupDeviceBackward();
    QWidget*                    SetupSwitch1();
    QWidget*                    SetupSwitch2();
    QWidget*                    SetupSwitch3();
    QWidget*                    SetupSwitch4();

    TreeWidgetItemPort*         GetCheckedItem(QTreeWidget *pTreeWidget, int iColumn);
    bool                        SetCheckedPort(QTreeWidget *pTreeWidget, int iColumn, ProjectExplorer::PePort *pPort);
    void                        MarkConnectedPorts();

    void                        AdjustComboBoxContentWidth(QComboBox *pComboBox);
    bool                        BlockSignals(bool bBlock);
    void                        SetUiData(ProjectExplorer::PeInfoSet *pInfoSetForward, ProjectExplorer::PeInfoSet *pInfoSetBackward);
    bool                        GetUiData(ProjectExplorer::PeInfoSet *pInfoSetForward, ProjectExplorer::PeInfoSet *pInfoSetBackward, QList<ProjectExplorer::PeInfoSet> &lstAffectInfoSets = QList<ProjectExplorer::PeInfoSet>());

// Properties
private:
    ProjectExplorer::PeBay      *m_pBay;
    ProjectExplorer::PeInfoSet  *m_pInfoSetForward;
    ProjectExplorer::PeInfoSet  *m_pInfoSetBackward;

    QLineEdit                   *m_pLineEditNameForward;
    QLineEdit                   *m_pLineEditDescForward;
    QComboBox                   *m_pComboBoxTypeForward;
    QGroupBox                   *m_pGroupBoxForward;

    QLineEdit                   *m_pLineEditNameBackward;
    QLineEdit                   *m_pLineEditDescBackward;
    QComboBox                   *m_pComboBoxTypeBackward;
    QGroupBox                   *m_pGroupBoxBackward;

    QGroupBox                   *m_pGroupBoxDeviceForward;
    QComboBox                   *m_pComboBoxDeviceForward;
    QTreeWidget                 *m_pTreeWidgetPortForward;

    QGroupBox                   *m_pGroupBoxDeviceBackward;
    QComboBox                   *m_pComboBoxDeviceBackward;
    QTreeWidget                 *m_pTreeWidgetPortBackward;

    QGroupBox                   *m_pGroupBoxSwitch1;
    QComboBox                   *m_pComboBoxSwitch1;
    QTreeWidget                 *m_pTreeWidgetPortSwitch1;

    QGroupBox                   *m_pGroupBoxSwitch2;
    QComboBox                   *m_pComboBoxSwitch2;
    QTreeWidget                 *m_pTreeWidgetPortSwitch2;

    QGroupBox                   *m_pGroupBoxSwitch3;
    QComboBox                   *m_pComboBoxSwitch3;
    QTreeWidget                 *m_pTreeWidgetPortSwitch3;

    QGroupBox                   *m_pGroupBoxSwitch4;
    QComboBox                   *m_pComboBoxSwitch4;
    QTreeWidget                 *m_pTreeWidgetPortSwitch4;

    QAction                     *m_pActionModify;
    QAction                     *m_pActionCreate;

    QHBoxLayout                 *m_pHBoxLayoutDetails;

private slots:
    void SlotSetInfoSet(ProjectExplorer::PeInfoSet *pInfoSet);
    void SlotCurrentForwardDeviceChanged(int iIndex);
    void SlotCurrentBackwardDeviceChanged(int iIndex);
    void SlotCurrentSwitchChanged(int iIndex, QComboBox *pComboBoxSwitch = 0);
    void SlotSwitchToggled(bool bToggled);
    void SlotPortDoubleClicked(QTreeWidgetItem *pItem, int iColumn);
    void SlotActionModify();
    void SlotActionCreate();
};

} // namespace Internal
} // namespace PhyNetwork

#endif // WIDGETINFOSET_H
