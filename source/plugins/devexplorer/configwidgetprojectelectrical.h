#ifndef CONFIGWIDGETPROJECTELECTRICAL_H
#define CONFIGWIDGETPROJECTELECTRICAL_H

#include "configwidgetdeviceobject.h"

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace DevExplorer {
namespace Internal {

class ConfigWidgetProjectElectrical : public ConfigWidgetDeviceObject
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigWidgetProjectElectrical(QWidget *pParent = 0);
    ~ConfigWidgetProjectElectrical();

// Operations
private:
    virtual bool            BuildModelData();
    virtual QStringList     RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual QList<QAction*> GetContextMenuActions();
    virtual QList<QAction*> GetToolbarActions();
    virtual void            UpdateActions(const QModelIndexList &lstSelectedIndex);
    virtual void            PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject);

// Properties
private:
    QAction                 *m_pActionNewBay;
    QAction                 *m_pActionNewIED;
    QAction                 *m_pActionNewSwitch;
    QAction                 *m_pActionDelete;
    QAction                 *m_pActionProperty;

private slots:
    virtual void            SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual void            SlotProjectDeviceBayChanged(ProjectExplorer::PeDevice *pDevice, ProjectExplorer::PeBay *pOldBay);

private slots:
    void SlotActionNewBay();
    void SlotActionNewIED();
    void SlotActionNewSwitch();
    void SlotActionDelete();
    void SlotActionProperty();
};

} // namespace Internal
} // namespace DevExplorer

#endif // CONFIGWIDGETPROJECTELECTRICAL_H
