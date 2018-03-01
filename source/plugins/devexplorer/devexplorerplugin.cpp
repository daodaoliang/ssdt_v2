#include <QStringList>
#include <QtPlugin>
#include <QSettings>

#include "devexplorerplugin.h"
#include "devexplorermode.h"

using namespace DevExplorer::Internal;

DevExplorerPlugin::DevExplorerPlugin()
{
}

DevExplorerPlugin::~DevExplorerPlugin()
{
}

bool DevExplorerPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    addAutoReleasedObject(new DevExplorerMode);
    return true;
}

void DevExplorerPlugin::extensionsInitialized()
{
    SlotReadSettings();

    //connect(Core::ICore::instance(), SIGNAL(sigCoreAboutToClose()),
    //        this, SLOT(SlotWriteSettings()));
}

void DevExplorerPlugin::aboutToShutdown()
{
}

void DevExplorerPlugin::SlotReadSettings()
{
    /*QSettings *pSettings = Core::ICore::instance()->GetSettings();

    pSettings->beginGroup(Config::Constants::g_szSettings_Group_Config);
    m_pProjectMode->ReadSettings(pSettings);
    pSettings->endGroup();*/
}

void DevExplorerPlugin::SlotWriteSettings()
{
    /*QSettings *pSettings = Core::ICore::instance()->GetSettings();

    pSettings->beginGroup(Config::Constants::g_szSettings_Group_Config);
    m_pProjectMode->WriteSettings(pSettings);
    pSettings->endGroup();*/
}

Q_EXPORT_PLUGIN(DevExplorerPlugin)
