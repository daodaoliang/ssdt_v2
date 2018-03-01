#include <QtPlugin>

#include "extensionsystem/pluginmanager.h"

#include "coreplugin.h"
#include "mainwindow.h"
#include "projectexplorer.h"
#include "vterminalsettings.h"
#include "vterminaldefine.h"

using namespace Core;
using namespace Core::Internal;

CorePlugin::CorePlugin() : m_pMainWindow(new MainWindow), m_pVTerminalDefine(new VTerminalDefine)
{
}

CorePlugin::~CorePlugin()
{
    delete m_pVTerminalDefine;
    delete m_pMainWindow;
}

bool CorePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    return true;
}

void CorePlugin::extensionsInitialized()
{
    addAutoReleasedObject(new ProjectExplorer);
    addAutoReleasedObject(new VTerminalSettings);

    m_pMainWindow->Initialize();
}

void CorePlugin::aboutToShutdown()
{
}

Q_EXPORT_PLUGIN(CorePlugin)
