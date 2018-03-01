#ifndef CONFIGWIDGETROOM_H
#define CONFIGWIDGETROOM_H

#include "configwidgetdeviceobject.h"

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace DevExplorer {
namespace Internal {

class ConfigWidgetRoom : public ConfigWidgetDeviceObject
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigWidgetRoom(QWidget *pParent = 0);
    ~ConfigWidgetRoom();

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
    QAction                 *m_pActionNewCubicle;
    QAction                 *m_pActionDelete;
    QAction                 *m_pActionProperty;

private slots:
    virtual void            SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual void            SlotProjectObjectParentChanged(ProjectExplorer::PeProjectObject *pProjectObject, ProjectExplorer::PeProjectObject *pOldParentProjectObject);

private slots:
    void SlotActionNewCubicle();
    void SlotActionDelete();
    void SlotActionProperty();
};

} // namespace Internal
} // namespace DevExplorer

#endif // CONFIGWIDGETROOM_H
