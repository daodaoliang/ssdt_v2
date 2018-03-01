#include <QtPlugin>
#include <QStringList>

#include "devlibraryplugin.h"
#include "devlibraryexplorer.h"

using namespace DevLibrary::Internal;

DevLibraryPlugin::DevLibraryPlugin()
{
}

DevLibraryPlugin::~DevLibraryPlugin()
{
}

bool DevLibraryPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    return true;
}

void DevLibraryPlugin::extensionsInitialized()
{
    addAutoReleasedObject(new DevLibraryExplorer);
}

void DevLibraryPlugin::aboutToShutdown()
{
}

Q_EXPORT_PLUGIN(DevLibraryPlugin)
