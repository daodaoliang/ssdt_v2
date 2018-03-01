#ifndef BAYTEMPLATEEXPLORERWIDGET_H
#define BAYTEMPLATEEXPLORERWIDGET_H

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
class PbTpBay;

} // namespace ProjectExplorer

namespace BayTemplate {
namespace Internal {

class BayTemplateExplorerWidget : public QWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    BayTemplateExplorerWidget(QWidget *pParent = 0);
    ~BayTemplateExplorerWidget();

// Operations
private:
    QStandardItem*      FindChildItem(QStandardItem *pItemParent, const QString &strText);
    void                InsertChildItem(QStandardItem *pItemParent, QStandardItem *pItemChild);

// Properties
private:
    QStandardItemModel      *m_pModel;
    Utils::ReadOnlyTreeView *m_pView;
    QStandardItem           *m_pCurrentItem;

    QAction                 *m_pActionNew;
    QAction                 *m_pActionDelete;
    QAction                 *m_pActionProperty;
    QAction                 *m_pActionConfig;

    QMap<ProjectExplorer::PbTpBay*, QStandardItem*> m_mapBayToItem;

private slots:
    void SlotBaseManagerInitialized();
    void SlotBaseManagerUninitialized();
    void SlotBaseObjectCreated(ProjectExplorer::PbBaseObject *pBaseObject);
    void SlotBaseObjectPropertyChanged(ProjectExplorer::PbBaseObject *pBaseObject);
    void SlotBaseObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject *pBaseObject);
    void SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void SlotViewDoubleClicked(const QModelIndex &index);
    void SlotActionNew();
    void SlotActionDelete();
    void SlotActionProperty();
    void SlotActionConfig();
};

} // namespace Internal
} // namespace BayTemplate

#endif // BAYTEMPLATEEXPLORERWIDGET_H
