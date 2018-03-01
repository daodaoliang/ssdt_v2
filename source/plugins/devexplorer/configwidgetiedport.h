#ifndef CONFIGWIDGETIEDPORT_H
#define CONFIGWIDGETIEDPORT_H

#include "configwidgetdeviceobject.h"

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeBoard;

} // namespace ProjectExplorer

namespace DevExplorer {
namespace Internal {

class ConfigWidgetIEDPort : public ConfigWidgetDeviceObject
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigWidgetIEDPort(QWidget *pParent = 0);
    ~ConfigWidgetIEDPort();

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
    QAction                 *m_pActionNewPort;
    QAction                 *m_pActionDelete;
    QAction                 *m_pActionProperty;

private slots:
    virtual void            SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject);

private slots:
    void SlotActionNewPort();
    void SlotActionDelete();
    void SlotActionProperty();
};

} // namespace Internal
} // namespace DevExplorer

#endif // CONFIGWIDGETIEDPORT_H
