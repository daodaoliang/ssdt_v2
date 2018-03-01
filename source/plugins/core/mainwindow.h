#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFuture>
#include "styledui/styledwindow.h"
#include "core_global.h"

namespace ProjectExplorer {

class PeProjectObject;
class PeProjectVersion;

} // namespace ProjectExplorer

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace Core {

class IMode;
class CActionManager;
class ExplorerWidget;
class MainWindowPrivate;
class CORE_EXPORT MainWindow : public StyledUi::StyledWindow
{
    Q_OBJECT

// Construction and Destruction
public:
    MainWindow(QWidget *pParent = 0);
    ~MainWindow();

// Operations
public:
    static  MainWindow*                 Instance();
    bool                                Initialize();

    void                                SwitchMode(const QString &strModeId);

    ProjectExplorer::PeProjectObject*   GetActiveObject(bool *pBayObject = 0) const;
    void                                SetActiveObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject, bool bSwitch = true);

    void                                SetContextMenuObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject);
    ProjectExplorer::PeProjectObject*   GetContextMenuObject(bool *pBayObject = 0) const;

    void                                SetContext(const QList<int> &lstContexts);

    void                                AddProgressTask(const QFuture<void> &future, const QString &strTitle);

    CActionManager*                     GetActionManager() const;
    QString                             GetResourcePath() const;
    QSettings*                          GetSettings() const;

private:
    void                                SetupToolBar();
    void                                SetupStatusBar();
    void                                SetupMenu();
    void                                SetupContextMenu();

// Properties
private:
    MainWindowPrivate   *m_d;

private slots:
    void SlotObjectAdded(QObject *pObject);
    void SlotAboutToRemoveObject(QObject *pObject);
    void SlotCurrentModeChanged(int iCurrentIndex);

    void SlotProjectVersionOpened(ProjectExplorer::PeProjectVersion *pProjectVersion);
    void SlotProjectVersionAboutToBeClosed(ProjectExplorer::PeProjectVersion *pProjectVersion);

    void SlotActionOpen();
    void SlotActionCloseAll();
    void SlotActionRefreshAll();
    void SlotActionCopyVConn();
    void SlotActionManageModel();
    void SlotActionOptions();
    void SlotContextMenuActionClose();
    void SlotContextMenuActionRefresh();
    void SlotViewInConfigMode();
    void SlotContextMenuActionExportStationModels();
};

} // namespace Core

#endif // MAINWINDOW_H
