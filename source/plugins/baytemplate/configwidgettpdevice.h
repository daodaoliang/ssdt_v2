#ifndef CONFIGWIDGETTPDEVICE_H
#define CONFIGWIDGETTPDEVICE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QItemSelection;
class QModelIndex;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PbBaseObject;
class PbTpBay;
class PbTpDevice;

} // namespace ProjectExplorer

namespace Utils {

class ReadOnlyTableView;

} // namespace Utils


namespace BayTemplate {
namespace Internal {

class ConfigWidgetTpDevice : public QWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigWidgetTpDevice(ProjectExplorer::PbTpBay *pTpBay, QWidget *pParent = 0);
    ~ConfigWidgetTpDevice();

// Operations
private:
    bool    CreateTpDevice(ProjectExplorer::PbTpDevice *pTpDeviceTemp);
    bool    UpdateTpDevice(ProjectExplorer::PbTpDevice *pTpDeviceTemp);
    bool    DeleteTpDevice(const QList<ProjectExplorer::PbTpDevice*> &lstTpDevices);

// Properties
private:
    ProjectExplorer::PbTpBay    *m_pTpBay;
    QStandardItemModel          *m_pModel;
    Utils::ReadOnlyTableView    *m_pView;

    QAction                     *m_pActionNewIED;
    QAction                     *m_pActionNewSwitch;
    QAction                     *m_pActionDelete;
    QAction                     *m_pActionProperty;

private slots:
    void SlotBaseObjectCreated(ProjectExplorer::PbBaseObject *pBaseObject);
    void SlotBaseObjectPropertyChanged(ProjectExplorer::PbBaseObject *pBaseObject);
    void SlotBaseObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject *pBaseObject);

    void SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void SlotViewDoubleClicked(const QModelIndex &index);

    void SlotActionNewIED();
    void SlotActionNewSwitch();
    void SlotActionDelete();
    void SlotActionProperty();
};

} // namespace Internal
} // namespace BayTemplate

#endif // CONFIGWIDGETTPDEVICE_H
