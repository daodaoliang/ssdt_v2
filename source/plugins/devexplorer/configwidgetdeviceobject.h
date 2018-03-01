#ifndef CONFIGWIDGETDEVICEOBJECT_H
#define CONFIGWIDGETDEVICEOBJECT_H

#include <QPointer>
#include "core/configwidget.h"

namespace DevExplorer {
namespace Internal {

class ConfigWidgetDeviceObject : public Core::ConfigWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigWidgetDeviceObject(QWidget *pParent = 0);
    ~ConfigWidgetDeviceObject();

// Operations
public:
    virtual QWidget*        GetToolBar();

protected:
    virtual QList<QAction*> GetToolbarActions();

// Properties
protected:
    QPointer<QWidget>       m_pToolBarWidget;
};

} // namespace Internal
} // namespace DevExplorer

#endif // CONFIGWIDGETDEVICEOBJECT_H
