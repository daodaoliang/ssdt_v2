#ifndef CONFIGWIDGETVTERMINALCONN_H
#define CONFIGWIDGETVTERMINALCONN_H

#include "core/configwidget.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QItemSelection;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeDevice;
class PeVTerminalConn;

}

namespace SclModel {
namespace Internal {

class ConfigWidgetVTerminalConn : public Core::ConfigWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigWidgetVTerminalConn(QWidget *pParent = 0);
    ~ConfigWidgetVTerminalConn();

// Operations
public:
    virtual void                SetupConfigWidget();

private:
    virtual bool                BuildModelData();
    virtual QStringList         RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual QList<QAction*>     GetContextMenuActions();
    virtual void                UpdateActions(const QModelIndexList &lstSelectedIndex);
    virtual void                PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject);

    bool                        CheckVTerminalConn(ProjectExplorer::PeVTerminalConn *pVTerminalConn) const;
    void                        UpdateMarkStatus(ProjectExplorer::PeVTerminalConn *pVTerminalConn);

// Properties
private:
    ProjectExplorer::PeDevice   *m_pCurrentDevice;
    ProjectExplorer::PeDevice   *m_pSideDevice;

    QCheckBox                   *m_pCheckBoxShowAll;
    QComboBox                   *m_pComboBoxConnectionType;
    QAction                     *m_pActionDelete;
    QAction                     *m_pActionMark;

private slots:
    virtual void                SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual void                SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject);

public slots:
    void SlotSetCurrentDevice(ProjectExplorer::PeDevice *pCurrentDevice);
    void SlotSetSideDevice(ProjectExplorer::PeDevice *pSideDevice);
    void SlotActivateVTerminalConn(ProjectExplorer::PeVTerminalConn *pVTerminalConn);

private slots:
    void SlotActionDelete();
    void SlotActionMark();
    void SlotShowAllConnectionChanged(bool bShow);
    void SlotConnectionTypeChanged(int iIndex);

signals:
    void sigCurrentVTerminalConnChanged(ProjectExplorer::PeVTerminalConn *pVTerminalConn);
};

} // namespace Internal
} // namespace PhyNetwork

#endif // CONFIGWIDGETVTERMINALCONN_H
