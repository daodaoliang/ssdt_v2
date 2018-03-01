#ifndef EXPLORERWIDGET_H
#define EXPLORERWIDGET_H

#include <QWidget>

namespace Utils {

class ReadOnlyTreeView;

} // namespace Utils


namespace ProjectExplorer {

class PeProjectVersion;
class PeCubicle;

} // namespace ProjectExplorer

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QModelIndex;
QT_END_NAMESPACE

class ExplorerWidget : public QWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    ExplorerWidget(QWidget *pParent = 0);
    ~ExplorerWidget();

// Operations
public:
    void SetProjectVersion(ProjectExplorer::PeProjectVersion *pProjectVersion);
    ProjectExplorer::PeProjectVersion* GetProjectVersion() const;

// Properties
private:
    ProjectExplorer::PeProjectVersion   *m_pProjectVersion;
    QStandardItemModel                  *m_pModel;
    Utils::ReadOnlyTreeView             *m_pView;

signals:
    void sigCubicleActivated(ProjectExplorer::PeCubicle *pCubicle);

private slots:
    void SlotViewItemPressed(const QModelIndex &index);
};

#endif // EXPLORERWIDGET_H
