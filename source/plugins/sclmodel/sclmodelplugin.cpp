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

#include "sclmodelplugin.h"
#include "sclmodel_constants.h"
#include "vterminalconnmode.h"
#include "subnetworkmode.h"
#include "dialogdescsync.h"

using namespace SclModel::Internal;

SclModelPlugin::SclModelPlugin()
{
}

SclModelPlugin::~SclModelPlugin()
{
}

bool SclModelPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    addAutoReleasedObject(new VTerminalConnMode);
    addAutoReleasedObject(new SubNetworkMode);

    return true;
}

void SclModelPlugin::extensionsInitialized()
{
    SlotReadSettings();

    //connect(Core::ICore::instance(), SIGNAL(sigCoreAboutToClose()),
    //        this, SLOT(SlotWriteSettings()));

    QList<int> lstGlobalContexts = QList<int>() << Core::Constants::g_iUID_Context_Global;

    Core::CActionManager *pActionManager = Core::MainWindow::Instance()->GetActionManager();
    Core::CActionContainer *pActionContainerMenu = pActionManager->GetActionContainer(Core::Constants::g_szUID_Menu);

    QAction *pActionToolsAutoConn = new QAction(tr("Description Synchronization..."), this);
    Core::CCommand *pCommand = pActionManager->RegisterAction(pActionToolsAutoConn, QLatin1String("SclModel.Action.ToolsDescSync"), lstGlobalContexts);
    pCommand->SetDefaultKeySequence(QKeySequence("Ctrl+D"));
    pActionContainerMenu->AddAction(pCommand, Core::Constants::g_szUID_MenuGroup_Tools);
    connect(pActionToolsAutoConn, SIGNAL(triggered()), this, SLOT(SlotActionDescSync()));
}

void SclModelPlugin::aboutToShutdown()
{
}

void SclModelPlugin::SlotReadSettings()
{
    /*QSettings *pSettings = Core::ICore::instance()->GetSettings();

    pSettings->beginGroup(Config::Constants::g_szSettings_Group_Config);
    m_pProjectMode->ReadSettings(pSettings);
    pSettings->endGroup();*/
}

void SclModelPlugin::SlotWriteSettings()
{
    /*QSettings *pSettings = Core::ICore::instance()->GetSettings();

    pSettings->beginGroup(Config::Constants::g_szSettings_Group_Config);
    m_pProjectMode->WriteSettings(pSettings);
    pSettings->endGroup();*/
}

void SclModelPlugin::SlotActionDescSync()
{
    DialogDescSync dialog(Core::MainWindow::Instance());
    dialog.exec();
}

Q_EXPORT_PLUGIN(SclModelPlugin)
