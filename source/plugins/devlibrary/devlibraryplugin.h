#ifndef DEVLIBRARYPLUGIN_H
#define DEVLIBRARYPLUGIN_H

#include "extensionsystem/iplugin.h"

namespace DevLibrary {
namespace Internal {

class DevLibraryPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "bless.ssdt.plugin")
#endif

// Construction and Destruction
public:
    DevLibraryPlugin();
    ~DevLibraryPlugin();

// Operations
public:
    virtual bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    virtual void extensionsInitialized();
    virtual void aboutToShutdown();
};

} // namespace Internal
} // namespace DevLibrary

#endif // DEVLIBRARYPLUGIN_H
