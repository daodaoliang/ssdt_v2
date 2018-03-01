#ifndef SUBNETWORKMODEWIDGET_H
#define SUBNETWORKMODEWIDGET_H

#include "styledui/styledwidget.h"

QT_BEGIN_NAMESPACE
class QTabBar;
class QStackedWidget;
class QStandardItemModel;
class QItemSelection;
class QModelIndex;
QT_END_NAMESPACE

namespace SclParser {

class SCLElement;

} // namespace SclParser

namespace Utils {

class ReadOnlyTableView;

} // namespace Utils


namespace ProjectExplorer {

class PeProjectVersion;
class PeProjectObject;

} // namespace ProjectExplorer

namespace SclModel {
namespace Internal {

class SubNetworkModeWidget : public StyledUi::StyledWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    SubNetworkModeWidget(QWidget *pParent = 0);
    ~SubNetworkModeWidget();

// Operations
public:
    virtual void    SetProjectObject(ProjectExplorer::PeProjectObject *pProjectObject);

private:
    QWidget*                SetupToolBar();
    void                    SetupAddrModelView();
    void                    SetupGseModelView();
    void                    SetupSmvModelView();

    void                    BuildAddrModel(SclParser::SCLElement *pSCLElementCommunication);
    void                    BuildGseModel(SclParser::SCLElement *pSCLElementCommunication);
    void                    BuildSmvModel(SclParser::SCLElement *pSCLElementCommunication);

    SclParser::SCLElement*  FindElementConnectedAP(SclParser::SCLElement *pSCLElementCommunication, const QString &strIED, const QString &strAccessPoint);
    SclParser::SCLElement*  FindElementAddr(SclParser::SCLElement *pSCLElementCommunication, const QString &strSubnetwork, const QString &strIED, const QString &strAccessPoint);
    SclParser::SCLElement*  FindElementGse(SclParser::SCLElement *pSCLElementCommunication, const QString &strSubnetwork, const QString &strIED, const QString &strAccessPoint, const QString &strLDevice, const QString &strControlBlock);
    SclParser::SCLElement*  FindElementSmv(SclParser::SCLElement *pSCLElementCommunication, const QString &strSubnetwork, const QString &strIED, const QString &strAccessPoint, const QString &strLDevice, const QString &strControlBlock);

    void                    UpdateActions();

// Property
private:
    ProjectExplorer::PeProjectVersion   *m_pProjectVersion;
    QWidget                             *m_pStyleBar;
    QTabBar                             *m_pTabBar;
    QStackedWidget                      *m_pStackedWidget;

    QAction                             *m_pActionRefresh;
    QAction                             *m_pActionBatchEdit;
    QAction                             *m_pActionEdit;

    QStandardItemModel                  *m_pModelAddr;
    Utils::ReadOnlyTableView            *m_pViewAddr;
    QStandardItemModel                  *m_pModelGse;
    Utils::ReadOnlyTableView            *m_pViewGse;
    QStandardItemModel                  *m_pModelSmv;
    Utils::ReadOnlyTableView            *m_pViewSmv;

private slots:
    void SlotCurrentTabChanged(int iCurrentIndex);
    void SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void SlotViewDoubleClicked(const QModelIndex &index);
    void SlotActionRefresh();
    void SlotActionBatchEdit();
    void SlotActionEdit();
};

} // namespace Internal
} // namespace SclModel

#endif // SUBNETWORKMODEWIDGET_H
