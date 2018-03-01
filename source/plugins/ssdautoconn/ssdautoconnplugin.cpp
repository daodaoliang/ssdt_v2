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

#include "ssdautoconnplugin.h"
#include "ssdautoconn_constants.h"
#include "ssdautoconndlg.h"

using namespace SsdAutoConn::Internal;

SsdAutoConnPlugin::SsdAutoConnPlugin()
{
}

SsdAutoConnPlugin::~SsdAutoConnPlugin()
{
}

bool SsdAutoConnPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    return true;
}

void SsdAutoConnPlugin::extensionsInitialized()
{
    SlotReadSettings();

    //connect(Core::ICore::instance(), SIGNAL(sigCoreAboutToClose()),
    //        this, SLOT(SlotWriteSettings()));    

    QList<int> lstGlobalContexts = QList<int>() << Core::Constants::g_iUID_Context_Global;

    Core::CActionManager *pActionManager = Core::MainWindow::Instance()->GetActionManager();
    Core::CActionContainer *pActionContainerMenu = pActionManager->GetActionContainer(Core::Constants::g_szUID_Menu);

    QAction *pActionToolsSsdAutoConn = new QAction(tr("SSD based auto connection"), this);
    Core::CCommand *pCommand = pActionManager->RegisterAction(pActionToolsSsdAutoConn, QLatin1String("SsdAutoConn.Action.ToolsSsdAutoConn"), lstGlobalContexts);
    pCommand->SetDefaultKeySequence(QKeySequence("Ctrl+K"));
    pActionContainerMenu->AddAction(pCommand, Core::Constants::g_szUID_MenuGroup_Tools);
    connect(pActionToolsSsdAutoConn, SIGNAL(triggered()), this, SLOT(SlotActionAutoConn()));
}

void SsdAutoConnPlugin::aboutToShutdown()
{
}

void SsdAutoConnPlugin::SlotReadSettings()
{
    /*QSettings *pSettings = Core::ICore::instance()->GetSettings();

    pSettings->beginGroup(Config::Constants::g_szSettings_Group_Config);
    m_pProjectMode->ReadSettings(pSettings);
    pSettings->endGroup();*/
}

void SsdAutoConnPlugin::SlotWriteSettings()
{
    /*QSettings *pSettings = Core::ICore::instance()->GetSettings();

    pSettings->beginGroup(Config::Constants::g_szSettings_Group_Config);
    m_pProjectMode->WriteSettings(pSettings);
    pSettings->endGroup();*/
}

void SsdAutoConnPlugin::SlotActionAutoConn()
{
    SsdAutoConnDlg dlg(Core::MainWindow::Instance());
    dlg.exec();
}

Q_EXPORT_PLUGIN(SsdAutoConnPlugin)
