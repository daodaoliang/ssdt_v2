#ifndef CONFIGWIDGETINFOSET_H
#define CONFIGWIDGETINFOSET_H

#include "core/configwidget.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QItemSelection;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeInfoSet;

} // namespace ProjectExplorer

namespace PhyNetwork {
namespace Internal {

class ConfigWidgetInfoSet : public Core::ConfigWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigWidgetInfoSet(QWidget *pParent = 0);
    ~ConfigWidgetInfoSet();

// Operations
public:
    virtual void            SetupConfigWidget();
    virtual bool            BuildModel(ProjectExplorer::PeProjectObject *pProjectObject);

private:
    virtual bool            BuildModelData();
    virtual QStringList     RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual QList<QAction*> GetContextMenuActions();
    virtual QList<QAction*> GetToolbarActions();
    virtual void            UpdateActions(const QModelIndexList &lstSelectedIndex);
    virtual void            PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject);

    bool                    CheckVInfoSet(ProjectExplorer::PeInfoSet *pInfoSet) const;
    void                    UpdateCompleteStatus(ProjectExplorer::PeInfoSet *pInfoSet);
    void                    UpdateFilberIEDs();

// Properties
private:
    QComboBox               *m_pComboBoxTypeFilter;
    QComboBox               *m_pComboBoxSendIEDFilter;
    QComboBox               *m_pComboBoxReceiveIEDFilter;
    QComboBox               *m_pComboBoxSwitchFilter;
    QAction                 *m_pActionDelete;

private slots:
    virtual void            SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual void            SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual void            SlotProjectObjectDeleted();

private slots:
    void SlotFilterChanged(int iIndex);
    void SlotActionDelete();

signals:
    void sigCurrentInfoSetChanged(ProjectExplorer::PeInfoSet *pInfoSet);
};

} // namespace Internal
} // namespace PhyNetwork

#endif // CONFIGWIDGETINFOSET_H
