#ifndef CONFIGWIDGETIEDBOARD_H
#define CONFIGWIDGETIEDBOARD_H

#include "configwidgetdeviceobject.h"

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace DevExplorer {
namespace Internal {

class ConfigWidgetIEDBoard : public ConfigWidgetDeviceObject
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigWidgetIEDBoard(QWidget *pParent = 0);
    ~ConfigWidgetIEDBoard();

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
    QAction                 *m_pActionNewBoard;
    QAction                 *m_pActionDelete;
    QAction                 *m_pActionProperty;

private slots:
    virtual void            SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject);

private slots:
    void SlotActionNewBoard();
    void SlotActionDelete();
    void SlotActionProperty();
};

} // namespace Internal
} // namespace DevExplorer

#endif // CONFIGWIDGETIEDBOARD_H
