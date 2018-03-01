#ifndef CONFIGWIDGETTPINFOSET_H
#define CONFIGWIDGETTPINFOSET_H

#include <QWidget>
#include <QMap>

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QStandardItem;
class QItemSelection;
class QModelIndex;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PbBaseObject;
class PbTpBay;
class PbTpDevice;
class PbTpInfoSet;

} // namespace ProjectExplorer

namespace Utils {

class ReadOnlyTableView;

} // namespace Utils


namespace BayTemplate {
namespace Internal {

class ConfigWidgetTpInfoSet : public QWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigWidgetTpInfoSet(ProjectExplorer::PbTpBay *pTpBay, QWidget *pParent = 0);
    ~ConfigWidgetTpInfoSet();

// Operations
private:
    void    UpdateCompleteStatus(ProjectExplorer::PbTpInfoSet *pTpInfoSet);

//    bool    CreateTpDevice(ProjectExplorer::PbTpDevice *pTpDeviceTemp);
//    bool    UpdateTpDevice(ProjectExplorer::PbTpDevice *pTpDeviceTemp);
//    bool    DeleteTpDevice(const QList<ProjectExplorer::PbTpDevice*> &lstTpDevices);

// Properties
private:
    ProjectExplorer::PbTpBay    *m_pTpBay;
    QStandardItemModel          *m_pModel;
    Utils::ReadOnlyTableView    *m_pView;

    QAction                     *m_pActionNew;
    QAction                     *m_pActionDelete;
    QAction                     *m_pActionProperty;

    QMap<ProjectExplorer::PbTpInfoSet*, QStandardItem*> m_mapTpInfoSetToItem;

private slots:
    void SlotBaseObjectCreated(ProjectExplorer::PbBaseObject *pBaseObject);
    void SlotBaseObjectPropertyChanged(ProjectExplorer::PbBaseObject *pBaseObject);
    void SlotBaseObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject *pBaseObject);

    void SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void SlotViewDoubleClicked(const QModelIndex &index);

    void SlotActionNew();
    void SlotActionDelete();
    void SlotActionProperty();
};

} // namespace Internal
} // namespace BayTemplate

#endif // CONFIGWIDGETTPINFOSET_H
