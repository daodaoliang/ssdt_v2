#ifndef INFOSETDLG_H
#define INFOSETDLG_H

#include <QTreeWidgetItem>
#include <QDialog>
#include "styledui/styleddialog.h"
#include "projectexplorer/pevterminalconn.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QCheckBox;
class QTreeWidget;
class QGroupBox;
class QHBoxLayout;
class QStandardItemModel;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTableView;

} // namespace Utils


namespace ProjectExplorer {

class PeBay;
class PeInfoSet;
class PePort;

} // namespace ProjectExplorer

namespace BayCopy {
namespace Internal {

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

class InfoSetDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    InfoSetDlg(ProjectExplorer::PeInfoSet *pInfoSetForward, ProjectExplorer::PeInfoSet *pInfoSetBackward, const QList<ProjectExplorer::PeInfoSet*> &lstAllInfoSets, QWidget *pParent = 0);
    ~InfoSetDlg();

// Operations
public:
    QList<ProjectExplorer::PeInfoSet*>  GetAffectedInfoSets() const;

private:
    QWidget*                            SetupGeneralForward();
    QWidget*                            SetupGeneralBackward();
    QWidget*                            SetupDeviceForward();
    QWidget*                            SetupDeviceBackward();
    QWidget*                            SetupSwitch1();
    QWidget*                            SetupSwitch2();
    QWidget*                            SetupSwitch3();
    QWidget*                            SetupSwitch4();

    TreeWidgetItemPort*                 GetCheckedItem(QTreeWidget *pTreeWidget, int iColumn);
    bool                                SetCheckedPort(QTreeWidget *pTreeWidget, int iColumn, ProjectExplorer::PePort *pPort);
    void                                MarkConnectedPorts();

    void                                SetUiData();
    bool                                GetUiData();

// Properties
private:
    ProjectExplorer::PeInfoSet          *m_pInfoSetForward;
    ProjectExplorer::PeInfoSet          *m_pInfoSetBackward;
    QList<ProjectExplorer::PeInfoSet*>  m_lstAllInfoSets;
    QList<ProjectExplorer::PeInfoSet*>  m_lstAffectedInfoSets;

    QLineEdit                           *m_pLineEditNameForward;
    QLineEdit                           *m_pLineEditDescForward;
    QComboBox                           *m_pComboBoxTypeForward;
    QGroupBox                           *m_pGroupBoxForward;

    QLineEdit                           *m_pLineEditNameBackward;
    QLineEdit                           *m_pLineEditDescBackward;
    QComboBox                           *m_pComboBoxTypeBackward;
    QGroupBox                           *m_pGroupBoxBackward;

    QGroupBox                           *m_pGroupBoxDeviceForward;
    QComboBox                           *m_pComboBoxDeviceForward;
    QTreeWidget                         *m_pTreeWidgetPortForward;

    QGroupBox                           *m_pGroupBoxDeviceBackward;
    QComboBox                           *m_pComboBoxDeviceBackward;
    QTreeWidget                         *m_pTreeWidgetPortBackward;

    QGroupBox                           *m_pGroupBoxSwitch1;
    QComboBox                           *m_pComboBoxSwitch1;
    QTreeWidget                         *m_pTreeWidgetPortSwitch1;

    QGroupBox                           *m_pGroupBoxSwitch2;
    QComboBox                           *m_pComboBoxSwitch2;
    QTreeWidget                         *m_pTreeWidgetPortSwitch2;

    QGroupBox                           *m_pGroupBoxSwitch3;
    QComboBox                           *m_pComboBoxSwitch3;
    QTreeWidget                         *m_pTreeWidgetPortSwitch3;

    QGroupBox                           *m_pGroupBoxSwitch4;
    QComboBox                           *m_pComboBoxSwitch4;
    QTreeWidget                         *m_pTreeWidgetPortSwitch4;

    QCheckBox                           *m_pCheckBoxUpdateAffectedInfoSets;

    QAction                             *m_pActionModify;
    QAction                             *m_pActionCreate;

    QHBoxLayout                         *m_pHBoxLayoutDetails;

public slots:
    virtual void accept();

private slots:
    void SlotCurrentForwardDeviceChanged(int iIndex);
    void SlotCurrentBackwardDeviceChanged(int iIndex);
    void SlotCurrentSwitchChanged(int iIndex, QComboBox *pComboBoxSwitch = 0);
};

} // namespace Internal
} // namespace BayCopy

#endif // INFOSETDLG_H
