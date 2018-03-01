#ifndef VTERMINALCONNDLG_H
#define VTERMINALCONNDLG_H

#include <QSortFilterProxyModel>
#include "styledui/styleddialog.h"
#include "projectexplorer/pevterminalconn.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QLineEdit;
class QButtonGroup;
class QToolButton;
class QStandardItemModel;
class QStandardItem;
class QModelIndex;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyListView;

} // namespace Utils

namespace ProjectExplorer {

class PeDevice;
class PeVTerminalConn;

} // namespace ProjectExplorer

namespace BayCopy {
namespace Internal {

class FilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

// Construction
public:
    FilterProxyModel(QObject *pParent = 0);

// Operations
private:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

// Properties
private:
    int     m_iType;
    int     m_iDirection;
    int     m_iLevel;
    QString m_strFilter;

public slots:
    void SlotSetType(int iType);
    void SlotSetDirection(int iDirection);
    void SlotSetLevel(int iLevel);
    void SlotSetFilter(const QString &strFilter);
};

class VTerminalConnDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    VTerminalConnDlg(ProjectExplorer::PeVTerminalConn *pVTerminalConn, QWidget *pParent = 0);
    ~VTerminalConnDlg();

// Operations
private:
    QWidget*                                SetupCurrentIED();
    QWidget*                                SetupSideIED();
    QLayout*                                SetupCenterLayout();

    ProjectExplorer::PeDevice*              GetCurrentIED() const;
    ProjectExplorer::PeDevice*              GetSideIED() const;
    void                                    UpdateVTerminalStatus(QStandardItem *pItemVTerminal, const QList<ProjectExplorer::PeVTerminalConn*> &lstVTerminalConns);

// Properties
private:
    ProjectExplorer::PeVTerminalConn   *m_pVTerminalConn;

    QComboBox               *m_pComboBoxCurrentIED;
    QLineEdit               *m_pLineEditCurrentFilter;
    QLineEdit               *m_pLineEditCurrentIEDDesc;
    QLineEdit               *m_pLineEditCurrentProDesc;
    QComboBox               *m_pComboBoxCurrentStrap;
    Utils::ReadOnlyListView *m_pListViewCurrent;
    QStandardItemModel      *m_pModelCurrent;
    FilterProxyModel        *m_pFilterProxyModelCurrent;
    QToolButton             *m_pToolButtonProDescCurrent;

    QComboBox               *m_pComboBoxSideIED;
    QLineEdit               *m_pLineEditSideFilter;
    QLineEdit               *m_pLineEditSideIEDDesc;
    QLineEdit               *m_pLineEditSideProDesc;
    QComboBox               *m_pComboBoxSideStrap;
    Utils::ReadOnlyListView *m_pListViewSide;
    QStandardItemModel      *m_pModelSide;
    FilterProxyModel        *m_pFilterProxyModelSide;
    QToolButton             *m_pToolButtonProDescSide;

    QButtonGroup            *m_pButtonGroupDirection;
    QButtonGroup            *m_pButtonGroupType;
    QButtonGroup            *m_pButtonGroupLevel;

    QMap<ProjectExplorer::PeVTerminal*, QStandardItem*> m_mapCurrentVTerminalToItem;
    QMap<ProjectExplorer::PeVTerminal*, QStandardItem*> m_mapSideVTerminalToItem;

public slots:
    virtual void accept();

private slots:
    void SlotViewCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
    void SlotViewSideChanged(const QModelIndex &current, const QModelIndex &previous);
    void SlotProDescClicked();
    void SlotConnectClicked();
};

} // namespace Internal
} // namespace BayCopy

#endif // VTERMINALCONNDLG_H
