#ifndef DEVLIBRARYEXPLORERWIDGET_H
#define DEVLIBRARYEXPLORERWIDGET_H

#include <QWidget>
#include <QMap>

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QStandardItem;
class QItemSelection;
class QModelIndex;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTreeView;

} // namespace Utils

namespace ProjectExplorer {

class PbBaseObject;
class PbLibDevice;

} // namespace ProjectExplorer

namespace DevLibrary {
namespace Internal {

class DevLibraryExplorerWidget : public QWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    DevLibraryExplorerWidget(QWidget *pParent = 0);
    ~DevLibraryExplorerWidget();

// Operations
private:
    QStandardItem*      FindChildItem(QStandardItem *pItemParent, const QString &strText);
    void                InsertChildItem(QStandardItem *pItemParent, QStandardItem *pItemChild);
    bool                CreateLibDevice(ProjectExplorer::PbLibDevice *pLibDeviceTemp, const QByteArray &baIcdContent);
    bool                UpdateLibDevice(ProjectExplorer::PbLibDevice *pLibDeviceTemp, const QByteArray &baIcdContent);

// Properties
private:
    QStandardItemModel      *m_pModel;
    Utils::ReadOnlyTreeView *m_pView;
    QStandardItem           *m_pCurrentItem;

    QAction                 *m_pActionNewDevice;
    QAction                 *m_pActionNewSwitch;
    QAction                 *m_pActionDelete;
    QAction                 *m_pActionProperty;
    QAction                 *m_pActionExportIpcd;

    QMap<ProjectExplorer::PbLibDevice*, QStandardItem*> m_mapDeviceToItem;

private slots:
    void SlotBaseManagerInitialized();
    void SlotBaseManagerUninitialized();
    void SlotBaseObjectCreated(ProjectExplorer::PbBaseObject *pBaseObject);
    void SlotBaseObjectPropertyChanged(ProjectExplorer::PbBaseObject *pBaseObject);
    void SlotBaseObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject *pBaseObject);
    void SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void SlotViewDoubleClicked(const QModelIndex &index);
    void SlotActionNewDevice();
    void SlotActionNewSwitch();
    void SlotActionDelete();
    void SlotActionProperty();
    void SlotActionExportIpcd();
};

} // namespace Internal
} // namespace DevLibrary

#endif // DEVLIBRARYEXPLORERWIDGET_H
