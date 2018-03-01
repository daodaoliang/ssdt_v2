#ifndef WIDGETVTERMINALCONN_H
#define WIDGETVTERMINALCONN_H

#include <QSortFilterProxyModel>
#include "styledui/styledwidget.h"
#include "projectexplorer/pevterminalconn.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QLineEdit;
class QButtonGroup;
class QToolButton;
class QPushButton;
class QStandardItemModel;
class QStandardItem;
class QModelIndex;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyListView;

} // namespace Utils

namespace ProjectExplorer {

class PeProjectVersion;
class PeDevice;
class PeVTerminalConn;

} // namespace ProjectExplorer

namespace SclModel {
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

class WidgetVTerminalConn : public StyledUi::StyledWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    WidgetVTerminalConn(QWidget *pParent = 0);
    ~WidgetVTerminalConn();

// Operations
public:
    void                                    SetProjectVersion(ProjectExplorer::PeProjectVersion *pProjectVersion, ProjectExplorer::PeDevice *pDevice);

private:
    QWidget*                                SetupCurrentIED();
    QWidget*                                SetupSideIED();
    QLayout*                                SetupCenterLayout();
    void                                    BlockSignals(bool bBlock);

    void                                    FillCurrentIED();
    void                                    FillSideIED();
    ProjectExplorer::PeDevice*              GetCurrentIED() const;
    ProjectExplorer::PeDevice*              GetSideIED() const;
    QList<ProjectExplorer::PeDevice*>       GetAllIEDs(ProjectExplorer::PeDevice *pIEDConnected);
    void                                    UpdateVTerminalStatus(QStandardItem *pItemVTerminal, const QList<ProjectExplorer::PeVTerminalConn*> &lstVTerminalConns);
    QList<ProjectExplorer::PeVTerminalConn> GetVTerminalConns();


// Properties
private:
    ProjectExplorer::PeProjectVersion   *m_pProjectVersion;

    QComboBox               *m_pComboBoxCurrentIED;
    QLineEdit               *m_pLineEditCurrentFilter;
    QLineEdit               *m_pLineEditCurrentIEDDesc;
    QLineEdit               *m_pLineEditCurrentProDesc;
    QComboBox               *m_pComboBoxCurrentStrap;
    Utils::ReadOnlyListView *m_pListViewCurrent;
    QStandardItemModel      *m_pModelCurrent;
    FilterProxyModel        *m_pFilterProxyModelCurrent;
    QToolButton             *m_pToolButtonProDescCurrent;
    QAction                 *m_pActionViewConnectionCurrent;

    QComboBox               *m_pComboBoxSideIED;
    QToolButton             *m_pToolButtonCheckConnect;
    QLineEdit               *m_pLineEditSideFilter;
    QLineEdit               *m_pLineEditSideIEDDesc;
    QLineEdit               *m_pLineEditSideProDesc;
    QComboBox               *m_pComboBoxSideStrap;
    Utils::ReadOnlyListView *m_pListViewSide;
    QStandardItemModel      *m_pModelSide;
    FilterProxyModel        *m_pFilterProxyModelSide;
    QToolButton             *m_pToolButtonProDescSide;
    QAction                 *m_pActionViewConnectionSide;

    QButtonGroup            *m_pButtonGroupDirection;
    QButtonGroup            *m_pButtonGroupType;
    QButtonGroup            *m_pButtonGroupLevel;
    QPushButton             *m_pPushButtonConnect;
    QPushButton             *m_pPushButtonConnectAuto;

    QMap<ProjectExplorer::PeVTerminal*, QStandardItem*> m_mapCurrentVTerminalToItem;
    QMap<ProjectExplorer::PeVTerminal*, QStandardItem*> m_mapSideVTerminalToItem;

private slots:
    void SlotSetVTerminalConn(ProjectExplorer::PeVTerminalConn *pVTerminalConn);
    void SlotCurrentIEDChanged(int iIndex);
    void SlotSideIEDChanged(int iIndex);
    void SlotSetDirection(int iDirection);
    void SlotSetType(int iType);
    void SlotViewCurrentChanged(const QModelIndex &current, const QModelIndex &previous);
    void SlotViewSideChanged(const QModelIndex &current, const QModelIndex &previous);
    void SlotViewDoubleClicked(const QModelIndex &index);
    void SlotActionViewConnectionTriggered();
    void SlotCheckConnectClicked();
    void SlotProDescClicked();
    void SlotConnectClicked();
    void SlotConnectAutoClicked();

    void SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject);
    void SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject);
    void SlotProjectObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject *pProjectObject);
    void SlotDeviceModelChanged(ProjectExplorer::PeDevice *pDevice);

signals:
    void sigCurrentDeviceChanged(ProjectExplorer::PeDevice *pCurrentDevice);
    void sigSideDeviceChanged(ProjectExplorer::PeDevice *pCurrentDevice);
    void sigViewConnection(ProjectExplorer::PeVTerminalConn *pVTerminalConn);
};

} // namespace Internal
} // namespace SclModel

#endif // WIDGETVTERMINALCONN_H
