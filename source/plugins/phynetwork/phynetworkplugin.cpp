#include <QStringList>
#include <QtPlugin>
#include <QSettings>

#include "phynetworkplugin.h"
#include "phynetwork_constants.h"
#include "infosetmode.h"
#include "cubicleconnmode.h"
#include "cablemode.h"
#include "cablegenerator.h"

using namespace PhyNetwork::Internal;

PhyNetworkPlugin::PhyNetworkPlugin()
{
}

PhyNetworkPlugin::~PhyNetworkPlugin()
{
}

bool PhyNetworkPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    new CableGenerator(this);
    addAutoReleasedObject(new InfoSetMode);
    addAutoReleasedObject(new CubicleConnMode);
    addAutoReleasedObject(new CableMode);

    return true;
}

void PhyNetworkPlugin::extensionsInitialized()
{
    SlotReadSettings();

    //connect(Core::ICore::instance(), SIGNAL(sigCoreAboutToClose()),
    //        this, SLOT(SlotWriteSettings()));
}

void PhyNetworkPlugin::aboutToShutdown()
{
}

void PhyNetworkPlugin::SlotReadSettings()
{
    /*QSettings *pSettings = Core::ICore::instance()->GetSettings();

    pSettings->beginGroup(Config::Constants::g_szSettings_Group_Config);
    m_pProjectMode->ReadSettings(pSettings);
    pSettings->endGroup();*/
}

void PhyNetworkPlugin::SlotWriteSettings()
{
    /*QSettings *pSettings = Core::ICore::instance()->GetSettings();

    pSettings->beginGroup(Config::Constants::g_szSettings_Group_Config);
    m_pProjectMode->WriteSettings(pSettings);
    pSettings->endGroup();*/
}

Q_EXPORT_PLUGIN(PhyNetworkPlugin)
