#include <QApplication>
#include <QSettings>
#include <QVBoxLayout>
#include <QDir>
#include <QStackedWidget>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QStatusBar>
#include <QFutureWatcher>
#include <QProgressBar>
#include <QLabel>
#include <QFontDatabase>

#include "extensionsystem/pluginmanager.h"
#include "styledui/styledbutton.h"
#include "styledui/stylehelper.h"
#include "utils/waitcursor.h"
#include "projectexplorer/basemanager.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peproject.h"
#include "projectexplorer/peprojectversion.h"

#include "mainwindow.h"
#include "core_constants.h"
#include "maintabbar.h"
#include "explorerwidget.h"
#include "imode.h"
#include "openprojectdlg.h"
#include "settingsdialog.h"
#include "uniqueidmanager.h"
#include "actionmanager_p.h"
#include "actioncontainer_p.h"
#include "command.h"
#include "copyvconndlg.h"
#include "managemodeldlg.h"
#include "configcoreoperation.h"

#include "QStyleFactory"
namespace Core {

class MainWindowPrivate
{
public:
    MainWindowPrivate() : m_pCurrentMode(0), m_pActiveObject(0), m_bActiveBayObject(false), m_pContextMenuObject(0), m_bContextMenuBayObject(false)
    {
    }

    static MainWindow*                  m_pInstance;
    QSettings                           *m_pSettings;
    QWidget                             *m_pToolBar;
    QStatusBar                          *m_pStatusBar;
    ExplorerWidget                      *m_pExplorerWidget;
    Internal::MainTabBar                *m_pMainTabBar;
    QStackedWidget*                     m_pStackedWidget;
    CUniqueIDManager                    *m_pUniqueIDManager;
    Internal::CActionManagerPrivate     *m_pActionManagerPrivate;

    QFutureWatcher<void>                *m_pFutureWatcher;
    QProgressBar                        *m_pProgressBar;
    QLabel                              *m_pLabelProcess;

    IMode                               *m_pCurrentMode;
    QList<IMode*>                       m_lstModes;

    ProjectExplorer::PeProjectObject    *m_pActiveObject;
    bool                                m_bActiveBayObject;

    ProjectExplorer::PeProjectObject    *m_pContextMenuObject;
    bool                                m_bContextMenuBayObject;

    QAction                             *m_pContextMenuActionProjectOpen;
    QAction                             *m_pContextMenuActionProjectClose;
    QAction                             *m_pContextMenuActionProjectRefresh;
    QAction                             *m_pContextMenuActionExportStationModels;

    StyledUi::StyledButton              *m_pButtonOpen;
    StyledUi::StyledButton              *m_pButtonClose;
    StyledUi::StyledButton              *m_pButtonRefresh;
    StyledUi::StyledButton              *m_pButtonOptions;
};

MainWindow* MainWindowPrivate::m_pInstance = 0;

MainWindow::MainWindow(QWidget *pParent) :
    StyledUi::StyledWindow(pParent), m_d(new MainWindowPrivate)
{
    MainWindowPrivate::m_pInstance = this;

    qApp->setStyleSheet(StyledUi::StyleHelper::GetStyleSheet(StyledUi::StyleHelper::stBlue));
    SetWindowTitle(tr("Smart Designer"));

    int iId = QFontDatabase::addApplicationFont(GetResourcePath() + QLatin1String("/font/msyh.ttc"));
    if(iId != -1)
    {
        QStringList lstFamilies = QFontDatabase::applicationFontFamilies(iId);
        if(!lstFamilies.empty())
            qApp->setFont(QFont(lstFamilies.first(), 9));
    }

    m_d->m_pSettings = new QSettings(GetResourcePath() + QLatin1String("/settings/settings.ini"), QSettings::IniFormat, this);
    m_d->m_pUniqueIDManager = new CUniqueIDManager;
    m_d->m_pActionManagerPrivate = new Internal::CActionManagerPrivate(this);

    SetupToolBar();
    SetupStatusBar();
    SetupMenu();
    SetupContextMenu();

    m_d->m_pExplorerWidget = new ExplorerWidget(this);
    m_d->m_pMainTabBar = new Internal::MainTabBar(this);
    m_d->m_pStackedWidget = new QStackedWidget(this);

    QHBoxLayout *pHBoxLayoutTop = new QHBoxLayout;
    pHBoxLayoutTop->setSpacing(0);
    pHBoxLayoutTop->setContentsMargins(0, 0, 0, 0);
    pHBoxLayoutTop->addWidget(m_d->m_pToolBar);
    pHBoxLayoutTop->addWidget(m_d->m_pMainTabBar);
    pHBoxLayoutTop->addStretch(1);

    QHBoxLayout *pHBoxLayout = new QHBoxLayout;
    pHBoxLayout->setSpacing(0);
    pHBoxLayout->setContentsMargins(0, 0, 0, 0);
    pHBoxLayout->addWidget(m_d->m_pExplorerWidget, 0, Qt::AlignLeft);
    pHBoxLayout->addSpacing(1);
    pHBoxLayout->addWidget(m_d->m_pStackedWidget);

    QVBoxLayout *pVBoxLayout = static_cast<QVBoxLayout*>(layout());
    pVBoxLayout->addLayout(pHBoxLayoutTop);
    pVBoxLayout->addLayout(pHBoxLayout);
    pVBoxLayout->addWidget(m_d->m_pStatusBar);

    connect(ExtensionSystem::PluginManager::instance(), SIGNAL(objectAdded(QObject*)),
            this, SLOT(SlotObjectAdded(QObject*)));
    connect(ExtensionSystem::PluginManager::instance(), SIGNAL(aboutToRemoveObject(QObject*)),
            this, SLOT(SlotAboutToRemoveObject(QObject*)));
    connect(m_d->m_pMainTabBar, SIGNAL(sigCurrentIndexChanged(int)),
            this, SLOT(SlotCurrentModeChanged(int)));
    connect(ProjectExplorer::ProjectManager::Instance(), SIGNAL(sigProjectVersionOpened(ProjectExplorer::PeProjectVersion*)),
            this, SLOT(SlotProjectVersionOpened(ProjectExplorer::PeProjectVersion*)));
    connect(ProjectExplorer::ProjectManager::Instance(), SIGNAL(sigProjectVersionAboutToBeClosed(ProjectExplorer::PeProjectVersion*)),
            this, SLOT(SlotProjectVersionAboutToBeClosed(ProjectExplorer::PeProjectVersion*)));
}

MainWindow::~MainWindow()
{
}

MainWindow* MainWindow::Instance()
{
    return MainWindowPrivate::m_pInstance;
}

bool MainWindow::Initialize()
{
    setMinimumSize(QSize(1000, 600));
    ShowMaximized(true);
    return true;
}

void MainWindow::SwitchMode(const QString &strModeId)
{
    for(int i = 0;i < m_d->m_lstModes.size(); i++)
    {
        if(m_d->m_lstModes.at(i)->GetId() == strModeId)
            m_d->m_pMainTabBar->SetCurrentIndex(i);
    }
}

ProjectExplorer::PeProjectObject* MainWindow::GetActiveObject(bool *pBayObject) const
{
    if(pBayObject)
        *pBayObject = m_d->m_bActiveBayObject;

    return m_d->m_pActiveObject;
}

void MainWindow::SetActiveObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject, bool bSwitch)
{
    m_d->m_pActiveObject = pProjectObject;
    m_d->m_bActiveBayObject = bBayObject;
    if(m_d->m_pCurrentMode && bSwitch)
        m_d->m_pCurrentMode->ActivateObject(m_d->m_pActiveObject, m_d->m_bActiveBayObject);
}

ProjectExplorer::PeProjectObject* MainWindow::GetContextMenuObject(bool *pBayObject) const
{
    if(pBayObject)
        *pBayObject = m_d->m_bContextMenuBayObject;

    return m_d->m_pContextMenuObject;
}

void MainWindow::SetContextMenuObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    m_d->m_pContextMenuObject = pProjectObject;
    m_d->m_bContextMenuBayObject = bBayObject;

    const bool bProjectVersionObject = (m_d->m_pContextMenuObject && m_d->m_pContextMenuObject->GetObjectType() == ProjectExplorer::PeProjectObject::otProjectVersion);
    m_d->m_pContextMenuActionProjectClose->setEnabled(bProjectVersionObject);
    m_d->m_pContextMenuActionProjectClose->setVisible(bProjectVersionObject);
    m_d->m_pContextMenuActionProjectRefresh->setEnabled(bProjectVersionObject);
    m_d->m_pContextMenuActionProjectRefresh->setVisible(bProjectVersionObject);
    m_d->m_pContextMenuActionExportStationModels->setEnabled(bProjectVersionObject);
    m_d->m_pContextMenuActionExportStationModels->setVisible(bProjectVersionObject);

    CActionContainer *pActionContainerViewInConfigModes = m_d->m_pActionManagerPrivate->GetActionContainer(Constants::g_szUID_ViewInModesMenu);
    pActionContainerViewInConfigModes->GetMenu()->clear();
    pActionContainerViewInConfigModes->GetMenu()->setEnabled(m_d->m_pContextMenuObject && !m_d->m_lstModes.isEmpty());
    if(m_d->m_pContextMenuObject)
    {
        foreach(IMode *pMode, m_d->m_lstModes)
        {
            QAction *pAction = pActionContainerViewInConfigModes->GetMenu()->addAction(pMode->GetName());
            pAction->setData(reinterpret_cast<int>(pMode));
            connect(pAction, SIGNAL(triggered()), this, SLOT(SlotViewInConfigMode()));
        }
    }

    foreach(IMode *pMode, m_d->m_lstModes)
        pMode->SetContextMenuObject(m_d->m_pContextMenuObject, m_d->m_bContextMenuBayObject);
}

void MainWindow::SetContext(const QList<int> &lstContexts)
{
    m_d->m_pActionManagerPrivate->SetContext(lstContexts);
}

void MainWindow::AddProgressTask(const QFuture<void> &future, const QString &strTitle)
{
    m_d->m_pLabelProcess->setText(strTitle);
    m_d->m_pFutureWatcher->setFuture(future);
}

CActionManager* MainWindow::GetActionManager() const
{
    return m_d->m_pActionManagerPrivate;
}

QString MainWindow::GetResourcePath() const
{
    return QDir::cleanPath(QCoreApplication::applicationDirPath() + QLatin1String("/../share"));
}

QSettings* MainWindow::GetSettings() const
{
    return m_d->m_pSettings;
}

void MainWindow::SetupToolBar()
{
    m_d->m_pToolBar = new QWidget(this);
    m_d->m_pToolBar->setFixedWidth(360);

    m_d->m_pButtonOpen = new StyledUi::StyledButton(":/core/images/open.png", tr("Open Project..."), m_d->m_pToolBar);
    m_d->m_pButtonOpen->SetSize(QSize(28, 28));
    m_d->m_pButtonClose = new StyledUi::StyledButton(":/core/images/close.png", tr("Close All Projects"), m_d->m_pToolBar);
    m_d->m_pButtonClose->SetSize(QSize(28, 28));
    m_d->m_pButtonRefresh = new StyledUi::StyledButton(":/core/images/refresh.png", tr("Refresh All Projects"), m_d->m_pToolBar);
    m_d->m_pButtonRefresh->SetSize(QSize(28, 28));
    m_d->m_pButtonOptions = new StyledUi::StyledButton(":/core/images/options.png", tr("Options..."), m_d->m_pToolBar);
    m_d->m_pButtonOptions->SetSize(QSize(32, 32));

    QHBoxLayout *pHBoxLayout = new QHBoxLayout(m_d->m_pToolBar);
    pHBoxLayout->addSpacing(30);
    pHBoxLayout->addWidget(m_d->m_pButtonOpen);
    pHBoxLayout->addSpacing(20);
    pHBoxLayout->addWidget(m_d->m_pButtonClose);
    pHBoxLayout->addSpacing(20);
    pHBoxLayout->addWidget(m_d->m_pButtonRefresh);
    pHBoxLayout->addSpacing(20);
    pHBoxLayout->addWidget(m_d->m_pButtonOptions);

    connect(m_d->m_pButtonOpen, SIGNAL(sigClicked()), this, SLOT(SlotActionOpen()));
    connect(m_d->m_pButtonClose, SIGNAL(sigClicked()), this, SLOT(SlotActionCloseAll()));
    connect(m_d->m_pButtonRefresh, SIGNAL(sigClicked()), this, SLOT(SlotActionRefreshAll()));
    connect(m_d->m_pButtonOptions, SIGNAL(sigClicked()), this, SLOT(SlotActionOptions()));
}

void MainWindow::SetupStatusBar()
{
    m_d->m_pStatusBar = new QStatusBar(this);
    m_d->m_pStatusBar->setFixedHeight(30);
    m_d->m_pStatusBar->setAutoFillBackground(true);
    QPalette palette = m_d->m_pStatusBar->palette();
    palette.setColor(QPalette::Background, QColor(255, 255, 255, 60));
    m_d->m_pStatusBar->setPalette(palette);

    m_d->m_pFutureWatcher = new QFutureWatcher<void>(this);
    m_d->m_pProgressBar = new QProgressBar(this);
    m_d->m_pProgressBar->hide();
    m_d->m_pProgressBar->setStyleSheet("color: white");
    m_d->m_pProgressBar->setFixedSize(300, 15);
    m_d->m_pLabelProcess = new QLabel(this);
    m_d->m_pLabelProcess->hide();
    m_d->m_pLabelProcess->setStyleSheet("color: white");

    m_d->m_pStatusBar->addPermanentWidget(m_d->m_pLabelProcess);
    m_d->m_pStatusBar->addPermanentWidget(m_d->m_pProgressBar);

    connect(m_d->m_pFutureWatcher, SIGNAL(started()), m_d->m_pProgressBar, SLOT(show()));
    connect(m_d->m_pFutureWatcher, SIGNAL(started()), m_d->m_pLabelProcess, SLOT(show()));
    connect(m_d->m_pFutureWatcher, SIGNAL(finished()), m_d->m_pProgressBar, SLOT(hide()));
    connect(m_d->m_pFutureWatcher, SIGNAL(finished()), m_d->m_pLabelProcess, SLOT(hide()));

    connect(m_d->m_pFutureWatcher, SIGNAL(progressRangeChanged(int,int)), m_d->m_pProgressBar, SLOT(setRange(int,int)));
    connect(m_d->m_pFutureWatcher, SIGNAL(progressValueChanged(int)), m_d->m_pProgressBar, SLOT(setValue(int)));
}

void MainWindow::SetupMenu()
{
    QList<int> lstGlobalContexts = QList<int>() << Constants::g_iUID_Context_Global;

    CActionContainer *pActionContainerMenu = m_d->m_pActionManagerPrivate->CreateMenu(Constants::g_szUID_Menu);
    SetWindowMenu(pActionContainerMenu->GetMenu());

    pActionContainerMenu->AppendGroup(Constants::g_szUID_MenuGroup_Project);
    pActionContainerMenu->AppendGroup(Constants::g_szUID_MenuGroup_Tools);
    pActionContainerMenu->AppendGroup(Constants::g_szUID_MenuGroup_Options);
    pActionContainerMenu->AppendGroup(Constants::g_szUID_MenuGroup_Exit);

    QAction *pActionProjectOpen = new QAction(tr("&Open Project..."), this);
    CCommand *pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionProjectOpen, QLatin1String("Core.Action.ProjectOpen"), lstGlobalContexts);
    pCommand->SetDefaultKeySequence(QKeySequence::Open);
    pActionContainerMenu->AddAction(pCommand, Constants::g_szUID_MenuGroup_Project);
    connect(pActionProjectOpen, SIGNAL(triggered()), this, SLOT(SlotActionOpen()));

    QAction *pActionProjectClose = new QAction(tr("&Close All Projects"), this);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionProjectClose, QLatin1String("Core.Action.ProjectClose"), lstGlobalContexts);
    pCommand->SetDefaultKeySequence(QKeySequence("Ctrl+Alt+W"));
    pActionContainerMenu->AddAction(pCommand, Constants::g_szUID_MenuGroup_Project);
    connect(pActionProjectClose, SIGNAL(triggered()), this, SLOT(SlotActionCloseAll()));

    QAction *pActionProjectRefresh = new QAction(tr("&Refresh All Projects"), this);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionProjectRefresh, QLatin1String("Core.Action.ProjectRefresh"), lstGlobalContexts);
    pCommand->SetDefaultKeySequence(QKeySequence("Ctrl+Alt+R"));
    pActionContainerMenu->AddAction(pCommand, Constants::g_szUID_MenuGroup_Project);
    connect(pActionProjectRefresh, SIGNAL(triggered()), this, SLOT(SlotActionRefreshAll()));

    QAction *pActionSeparator = new QAction(this);
    pActionSeparator->setSeparator(true);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionSeparator, QLatin1String("Core.Action.Separator.Tools"), lstGlobalContexts);
    pActionContainerMenu->AddAction(pCommand, Constants::g_szUID_MenuGroup_Tools);

    QAction *pActionToolsCopyVConn = new QAction(tr("&Connection Copy..."), this);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionToolsCopyVConn, QLatin1String("Core.Action.ToolsCopyVConn"), lstGlobalContexts);
    pCommand->SetDefaultKeySequence(QKeySequence("Ctrl+E"));
    pActionContainerMenu->AddAction(pCommand, Constants::g_szUID_MenuGroup_Tools);
    connect(pActionToolsCopyVConn, SIGNAL(triggered()), this, SLOT(SlotActionCopyVConn()));

    QAction *pActionToolsManageModel = new QAction(tr("&Model Management..."), this);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionToolsManageModel, QLatin1String("Core.Action.ToolsManageModel"), lstGlobalContexts);
    pCommand->SetDefaultKeySequence(QKeySequence("Ctrl+M"));
    pActionContainerMenu->AddAction(pCommand, Core::Constants::g_szUID_MenuGroup_Tools);
    connect(pActionToolsManageModel, SIGNAL(triggered()), this, SLOT(SlotActionManageModel()));

    Core::MainWindow::Instance()->SetContext(lstGlobalContexts);

    pActionSeparator = new QAction(this);
    pActionSeparator->setSeparator(true);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionSeparator, QLatin1String("Core.Action.Separator.Options"), lstGlobalContexts);
    pActionContainerMenu->AddAction(pCommand, Constants::g_szUID_MenuGroup_Options);

    QAction *pActionOptions = new QAction(tr("Op&tioins..."), this);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionOptions, QLatin1String("Core.Action.Options"), lstGlobalContexts);
    pCommand->SetDefaultKeySequence(QKeySequence("Ctrl+T"));
    pActionContainerMenu->AddAction(pCommand, Constants::g_szUID_MenuGroup_Options);
    connect(pActionOptions, SIGNAL(triggered()), this, SLOT(SlotActionOptions()));

    pActionSeparator = new QAction(this);
    pActionSeparator->setSeparator(true);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionSeparator, QLatin1String("Core.Action.Separator.Exit"), lstGlobalContexts);
    pActionContainerMenu->AddAction(pCommand, Constants::g_szUID_MenuGroup_Exit);

    QAction *pActionProjectExit = new QAction(tr("&Exit"), this);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionProjectExit, QLatin1String("Core.Action.Exit"), lstGlobalContexts);
    pCommand->SetDefaultKeySequence(QKeySequence::Quit);
    pActionContainerMenu->AddAction(pCommand, Constants::g_szUID_MenuGroup_Exit);
    connect(pActionProjectExit, SIGNAL(triggered()), this, SLOT(close()));

    SetContext(lstGlobalContexts);
}

void MainWindow::SetupContextMenu()
{
    QList<int> lstGlobalContexts = QList<int>() << Constants::g_iUID_Context_Global;

    CActionContainer *pActionContainerContextMenu = m_d->m_pActionManagerPrivate->CreateMenu(Constants::g_szUID_ContextMenu);
    pActionContainerContextMenu->AppendGroup(Constants::g_szUID_ContextMenuGroup_Project);
    pActionContainerContextMenu->AppendGroup(Constants::g_szUID_ContextMenuGroup_DevObject);
    pActionContainerContextMenu->AppendGroup(Constants::g_szUID_ContextMenuGroup_View);
    pActionContainerContextMenu->AppendGroup(Constants::g_szUID_ContextMenuGroup_Export);
    pActionContainerContextMenu->AppendGroup(Constants::g_szUID_ContextMenuGroup_TreeView);

    m_d->m_pContextMenuActionProjectOpen = new QAction(tr("&Open Project..."), this);
    CCommand *pCommand = m_d->m_pActionManagerPrivate->RegisterAction(m_d->m_pContextMenuActionProjectOpen, QLatin1String("Core.ContextMenuAction.ProjectOpen"), lstGlobalContexts);
    pCommand->SetDefaultKeySequence(QKeySequence::Open);
    pActionContainerContextMenu->AddAction(pCommand, Constants::g_szUID_ContextMenuGroup_Project);
    connect(m_d->m_pContextMenuActionProjectOpen, SIGNAL(triggered()), this, SLOT(SlotActionOpen()));

    m_d->m_pContextMenuActionProjectClose = new QAction(tr("&Close Project"), this);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(m_d->m_pContextMenuActionProjectClose, QLatin1String("Core.ContextMenuAction.ProjectClose"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Constants::g_szUID_MenuGroup_Project);
    connect(m_d->m_pContextMenuActionProjectClose, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionClose()));

    m_d->m_pContextMenuActionProjectRefresh = new QAction(tr("&Refresh Project"), this);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(m_d->m_pContextMenuActionProjectRefresh, QLatin1String("Core.ContextMenuAction.ProjectRefresh"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Constants::g_szUID_MenuGroup_Project);
    connect(m_d->m_pContextMenuActionProjectRefresh, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionRefresh()));

    QAction *pActionSeparator = new QAction(this);
    pActionSeparator->setSeparator(true);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionSeparator, QLatin1String("Core.Action.Separator.DevObject"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Constants::g_szUID_ContextMenuGroup_DevObject);

    pActionSeparator = new QAction(this);
    pActionSeparator->setSeparator(true);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionSeparator, QLatin1String("Core.Action.Separator.View"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Constants::g_szUID_ContextMenuGroup_View);

    CActionContainer *pActionContainerViewInConfigModes = m_d->m_pActionManagerPrivate->CreateMenu(Constants::g_szUID_ViewInModesMenu);
    pActionContainerContextMenu->SetEmptyAction(CActionContainer::EA_None);
    pActionContainerViewInConfigModes->GetMenu()->setTitle(tr("&View in Configuration Mode"));
    pActionContainerContextMenu->AddMenu(pActionContainerViewInConfigModes, Constants::g_szUID_ContextMenuGroup_View);

    pActionSeparator = new QAction(this);
    pActionSeparator->setSeparator(true);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionSeparator, QLatin1String("Core.Action.Separator.Export"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Constants::g_szUID_ContextMenuGroup_Export);

    m_d->m_pContextMenuActionExportStationModels = new QAction(tr("&Export Station Models"), this);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(m_d->m_pContextMenuActionExportStationModels, QLatin1String("Core.ContextMenuAction.ToolsExportModel"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Constants::g_szUID_ContextMenuGroup_Export);
    connect(m_d->m_pContextMenuActionExportStationModels, SIGNAL(triggered()), this, SLOT(SlotContextMenuActionExportStationModels()));

    pActionSeparator = new QAction(this);
    pActionSeparator->setSeparator(true);
    pCommand = m_d->m_pActionManagerPrivate->RegisterAction(pActionSeparator, QLatin1String("Core.Action.Separator.TreeView"), lstGlobalContexts);
    pActionContainerContextMenu->AddAction(pCommand, Constants::g_szUID_ContextMenuGroup_TreeView);

    SetContext(lstGlobalContexts);
}

void MainWindow::SlotObjectAdded(QObject *pObject)
{
    if(IMode *pMode = ExtensionSystem::query<IMode>(pObject))
    {
        int iIndex = 0;
        foreach(const IMode *pM, m_d->m_lstModes)
        {
            if(pM->GetPriority() > pMode->GetPriority())
                ++iIndex;
        }

        m_d->m_lstModes.insert(iIndex, pMode);
        m_d->m_pMainTabBar->InsertTab(iIndex, pMode->GetPixmap());
        m_d->m_pStackedWidget->insertWidget(iIndex, pMode->GetWidget());

        if(m_d->m_pMainTabBar->GetCurrentIndex() < 0)
            m_d->m_pMainTabBar->SetCurrentIndex(iIndex);
    }
}

void MainWindow::SlotAboutToRemoveObject(QObject *pObject)
{
    if(IMode *pMode = ExtensionSystem::query<IMode>(pObject))
    {
        const int iIndex = m_d->m_lstModes.indexOf(pMode);
        m_d->m_lstModes.removeAt(iIndex);
        m_d->m_pMainTabBar->RemoveTab(iIndex);
        m_d->m_pStackedWidget->removeWidget(m_d->m_pStackedWidget->widget(iIndex));
    }
}

void MainWindow::SlotCurrentModeChanged(int iCurrentIndex)
{
    if(iCurrentIndex < 0 || iCurrentIndex >= m_d->m_lstModes.size())
        return;

    IMode *pMode = m_d->m_lstModes.at(iCurrentIndex);
    if(m_d->m_pCurrentMode == pMode)
        return;

    if(m_d->m_pCurrentMode)
        m_d->m_pCurrentMode->ActivateObject(0, false);

    if(pMode)
        pMode->ActivateObject(m_d->m_pActiveObject, m_d->m_bActiveBayObject);

    m_d->m_pCurrentMode = pMode;
    m_d->m_pStackedWidget->setCurrentIndex(iCurrentIndex);
}

void MainWindow::SlotProjectVersionOpened(ProjectExplorer::PeProjectVersion *pProjectVersion)
{
    if(!m_d->m_pActiveObject)
        SetActiveObject(pProjectVersion, false);
}

void MainWindow::SlotProjectVersionAboutToBeClosed(ProjectExplorer::PeProjectVersion *pProjectVersion)
{
    if(m_d->m_pActiveObject && m_d->m_pActiveObject->GetProjectVersion() == pProjectVersion)
        SetActiveObject(0, false);

    if(m_d->m_pContextMenuObject && m_d->m_pContextMenuObject->GetProjectVersion() == pProjectVersion)
        SetContextMenuObject(0, false);
}

void MainWindow::SlotActionOpen()
{
    Internal::OpenProjectDlg dlg(this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(ProjectExplorer::PeProjectVersion *pProjectVersion = dlg.GetSelectedProjectVersion())
        pProjectVersion->Open();
}

void MainWindow::SlotActionCloseAll()
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, ProjectExplorer::ProjectManager::Instance()->GetAllProjectVersions())
    {
        if(pProjectVersion->IsOpend())
            pProjectVersion->Close();
    }
}

void MainWindow::SlotActionRefreshAll()
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    QList<int> lstOpenedProjectVersionIds;
    foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, ProjectExplorer::ProjectManager::Instance()->GetAllProjectVersions())
    {
        if(pProjectVersion->IsOpend())
            lstOpenedProjectVersionIds.append(pProjectVersion->GetId());
    }

    ProjectExplorer::BaseManager::Instance()->Reinitialize();
    ProjectExplorer::ProjectManager::Instance()->Reinitialize();

    foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, ProjectExplorer::ProjectManager::Instance()->GetAllProjectVersions())
    {
        if(lstOpenedProjectVersionIds.contains(pProjectVersion->GetId()))
            pProjectVersion->Open();
    }
}

void MainWindow::SlotActionCopyVConn()
{
    Internal::CopyVConnDlg dlg(this);
    dlg.exec();
}

void MainWindow::SlotActionManageModel()
{
    Internal::ManageModelDlg dlg(this);
    dlg.exec();
}

void MainWindow::SlotActionOptions()
{
    Internal::CSettingsDialog dlg(this);
    dlg.ExecDialog();
}

void MainWindow::SlotContextMenuActionClose()
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(ProjectExplorer::PeProjectVersion *pProjectVersion = qobject_cast<ProjectExplorer::PeProjectVersion*>(m_d->m_pContextMenuObject))
    {
        if(pProjectVersion->IsOpend())
            pProjectVersion->Close();
    }
}

void MainWindow::SlotContextMenuActionRefresh()
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(ProjectExplorer::PeProjectVersion *pProjectVersion = qobject_cast<ProjectExplorer::PeProjectVersion*>(m_d->m_pContextMenuObject))
    {
        if(pProjectVersion->IsOpend())
            pProjectVersion->Reopen();
    }
}

void MainWindow::SlotViewInConfigMode()
{
    if(QAction *pAction = qobject_cast<QAction*>(sender()))
    {
        if(IMode *pMode = reinterpret_cast<IMode*>(pAction->data().toInt()))
        {
            SetActiveObject(m_d->m_pContextMenuObject, m_d->m_bContextMenuBayObject);
            SwitchMode(pMode->GetId());
        }
    }
}

void MainWindow::SlotContextMenuActionExportStationModels()
{
    if(ProjectExplorer::PeProjectVersion *pProjectVersion = qobject_cast<ProjectExplorer::PeProjectVersion*>(m_d->m_pContextMenuObject))
    {
        if(ProjectExplorer::PeProject *pProject = pProjectVersion->GetProject())
        {
            QDir dirExport(QCoreApplication::applicationDirPath() + QString("/../../dwg/%1").arg(pProject->GetDescription()));
            if(!dirExport.exists())
                dirExport.mkpath(dirExport.absolutePath());

            if(dirExport.exists())
            {
                qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
                const bool bExportScd = ConfigCoreOperation::Instance()->ExportToScd(pProjectVersion, dirExport.absoluteFilePath(pProject->GetName() + QLatin1String(".scd")));
                const bool bExportSpcd = ConfigCoreOperation::Instance()->ExportSpcd(pProjectVersion, dirExport.absoluteFilePath(pProject->GetName() + QLatin1String(".spcd")));
                qApp->restoreOverrideCursor();

                if(bExportScd && bExportSpcd)
                {
                    QMessageBox::information(this, tr("Export Success"), tr("Succeed to export station model files!"));
                    QDesktopServices::openUrl(QUrl(QLatin1String("file:///") + dirExport.absolutePath(), QUrl::TolerantMode));
                }
            }
        }
    }
}

} // namespace Core
