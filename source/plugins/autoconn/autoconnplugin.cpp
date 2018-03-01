#include <QStringList>
#include <QtPlugin>
#include <QSettings>
#include <QAction>

#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peprojectversion.h"
#include "core/core_constants.h"
#include "core/actionmanager.h"
#include "core/actioncontainer.h"
#include "core/command.h"
#include "core/mainwindow.h"

#include "autoconnplugin.h"
#include "autoconn_constants.h"
#include "autoconndlg.h"

using namespace AutoConn::Internal;

AutoConnPlugin::AutoConnPlugin()
{
}

AutoConnPlugin::~AutoConnPlugin()
{
}

bool AutoConnPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    return true;
}

void AutoConnPlugin::extensionsInitialized()
{
    SlotReadSettings();

    //connect(Core::ICore::instance(), SIGNAL(sigCoreAboutToClose()),
    //        this, SLOT(SlotWriteSettings()));    

    QList<int> lstGlobalContexts = QList<int>() << Core::Constants::g_iUID_Context_Global;

    Core::CActionManager *pActionManager = Core::MainWindow::Instance()->GetActionManager();
    Core::CActionContainer *pActionContainerMenu = pActionManager->GetActionContainer(Core::Constants::g_szUID_Menu);

    QAction *pActionToolsAutoConn = new QAction(tr("Automatical connection"), this);
    Core::CCommand *pCommand = pActionManager->RegisterAction(pActionToolsAutoConn, QLatin1String("AutoConn.Action.ToolsAutoConn"), lstGlobalContexts);
    pActionContainerMenu->AddAction(pCommand, Core::Constants::g_szUID_MenuGroup_Tools);
    connect(pActionToolsAutoConn, SIGNAL(triggered()), this, SLOT(SlotActionAutoConn()));
}

void AutoConnPlugin::aboutToShutdown()
{
}

void AutoConnPlugin::SlotReadSettings()
{
    /*QSettings *pSettings = Core::ICore::instance()->GetSettings();

    pSettings->beginGroup(Config::Constants::g_szSettings_Group_Config);
    m_pProjectMode->ReadSettings(pSettings);
    pSettings->endGroup();*/
}

void AutoConnPlugin::SlotWriteSettings()
{
    /*QSettings *pSettings = Core::ICore::instance()->GetSettings();

    pSettings->beginGroup(Config::Constants::g_szSettings_Group_Config);
    m_pProjectMode->WriteSettings(pSettings);
    pSettings->endGroup();*/
}

void AutoConnPlugin::SlotActionAutoConn()
{
    AutoConnDlg dlg(Core::MainWindow::Instance());
    dlg.exec();
}

Q_EXPORT_PLUGIN(AutoConnPlugin)
