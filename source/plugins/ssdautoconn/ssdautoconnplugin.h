#ifndef SSDAUTOCONNPLUGIN_H
#define SSDAUTOCONNPLUGIN_H

#include "extensionsystem/iplugin.h"

namespace SsdAutoConn {
namespace Internal {

class SsdAutoConnPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "bless.ssdt.plugin")
#endif

// Construction and Destruction
public:
    SsdAutoConnPlugin();
    ~SsdAutoConnPlugin();

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

#endif // SSDAUTOCONNPLUGIN_H
