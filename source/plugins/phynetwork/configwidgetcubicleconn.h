#ifndef CONFIGWIDGETCUBICLECONN_H
#define CONFIGWIDGETCUBICLECONN_H

#include "core/configwidget.h"

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeCubicle;
class PeCubicleConn;

} // namespace ProjectExplorer

namespace PhyNetwork {
namespace Internal {

class ConfigWidgetCubicleConn : public Core::ConfigWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigWidgetCubicleConn(QWidget *pParent = 0);
    ~ConfigWidgetCubicleConn();

// Operations
private:
    virtual bool                BuildModelData();
    virtual QStringList         RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual void                UpdateActions(const QModelIndexList &lstSelectedIndex);

// Properties
private:
    ProjectExplorer::PeCubicle  *m_pCurrentCubicle;

private slots:
    virtual void                SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual void                SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject);

public slots:
    void SlotSetCurrentCubicle(ProjectExplorer::PeCubicle *pCurrentCubicle);

signals:
    void sigCurrentCubicleConnChanged(ProjectExplorer::PeCubicleConn *pCubicleConn);
};

} // namespace Internal
} // namespace PhyNetwork

#endif // CONFIGWIDGETCUBICLECONN_H
