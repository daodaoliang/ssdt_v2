#ifndef PROPERTYDLGTPINFOSET_H
#define PROPERTYDLGTPINFOSET_H

#include <QTreeWidgetItem>
#include "styledui/styleddialog.h"
#include "projectexplorer/pevterminalconn.h"

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

class PbTpBay;
class PbTpDevice;
class PbTpPort;
class PbTpInfoSet;

} // namespace ProjectExplorer

namespace BayTemplate {
namespace Internal {

class SelectTpInfoSetDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    SelectTpInfoSetDlg(const QList<ProjectExplorer::PbTpInfoSet*> &lstTpInfoSets, QWidget *pParent);

// Operations
public:
    virtual QSize sizeHint() const { return QSize(800, 450); }
    ProjectExplorer::PbTpInfoSet* GetSelectedTpInfoSet() const;

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
    TreeWidgetItemPort(ProjectExplorer::PbTpPort *pPortTx, ProjectExplorer::PbTpPort *pPortRx, bool bSwitchPort);

// Operations
public:
    virtual void setData(int column, int role, const QVariant &value);

    ProjectExplorer::PbTpPort* GetPortTx() const;
    ProjectExplorer::PbTpPort* GetPortRx() const;

    QList<ProjectExplorer::PbTpInfoSet*> GetTpInfoSets() const;

    void UpdateConnectionInfo(const QList<ProjectExplorer::PbTpInfoSet*> &lstTpInfoSets);
    void CheckDirecPortConnection(const QList<ProjectExplorer::PbTpDevice*> &lstDevices);

private:
    void MarkConnectionInfo(bool bDirect);

// Property
private:
    ProjectExplorer::PbTpPort             *m_pPortTx;
    ProjectExplorer::PbTpPort             *m_pPortRx;
    bool                                    m_bSwitchPort;

    ProjectExplorer::PbTpPort             *m_pPortConnectedTx;
    ProjectExplorer::PbTpPort             *m_pPortConnectedRx;
    QList<ProjectExplorer::PbTpInfoSet*>  m_lstTpInfoSets;
};

class PropertyDlgTpInfoSet : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgTpInfoSet(ProjectExplorer::PbTpInfoSet *pTpInfoSetForward, ProjectExplorer::PbTpInfoSet *pTpInfoSetBackward, QWidget *pParent = 0);
    ~PropertyDlgTpInfoSet();

// Operations
public:
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
    bool                        SetCheckedPort(QTreeWidget *pTreeWidget, int iColumn, ProjectExplorer::PbTpPort *pPort);
    void                        MarkConnectedPorts();

    bool                        BlockSignals(bool bBlock);
    void                        SetUiData(ProjectExplorer::PbTpInfoSet *pTpInfoSetForward, ProjectExplorer::PbTpInfoSet *pTpInfoSetBackward);
    bool                        GetUiData(ProjectExplorer::PbTpInfoSet *pTpInfoSetForward, ProjectExplorer::PbTpInfoSet *pTpInfoSetBackward, QList<ProjectExplorer::PbTpInfoSet> &lstAffectTpInfoSets = QList<ProjectExplorer::PbTpInfoSet>());

// Properties
private:
    ProjectExplorer::PbTpBay      *m_pTpBay;
    ProjectExplorer::PbTpInfoSet  *m_pTpInfoSetForward;
    ProjectExplorer::PbTpInfoSet  *m_pTpInfoSetBackward;

    QLineEdit                   *m_pLineEditNameForward;
    QComboBox                   *m_pComboBoxTypeForward;
    QGroupBox                   *m_pGroupBoxForward;

    QLineEdit                   *m_pLineEditNameBackward;
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

public slots:
    virtual void accept();

private slots:
    void SlotSetTpInfoSet(ProjectExplorer::PbTpInfoSet *pTpInfoSet);
    void SlotCurrentForwardDeviceChanged(int iIndex);
    void SlotCurrentBackwardDeviceChanged(int iIndex);
    void SlotCurrentSwitchChanged(int iIndex, QComboBox *pComboBoxSwitch = 0);
    void SlotSwitchToggled(bool bToggled);
    void SlotPortDoubleClicked(QTreeWidgetItem *pItem, int iColumn);
    void SlotActionModify();
    void SlotActionCreate();
};

} // namespace Internal
} // namespace BayTemplate

#endif // PROPERTYDLGTPINFOSET_H
