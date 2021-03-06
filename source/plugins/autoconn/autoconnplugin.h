#ifndef AUTOCONNPLUGIN_H
#define AUTOCONNPLUGIN_H

#include "extensionsystem/iplugin.h"

namespace AutoConn {
namespace Internal {

class AutoConnPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "bless.ssdt.plugin")
#endif

// Construction and Destruction
public:
    AutoConnPlugin();
    ~AutoConnPlugin();

// Operations
public:
    virtual bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    virtual void extensionsInitialized();
    virtual void aboutToShutdown();

private slots:
    void    SlotReadSettings();
    void    SlotWriteSettings();
    void    SlotActionAutoConn();
};

} // namespace Internal
} // namespace SclModel

#endif // AUTOCONNPLUGIN_H
