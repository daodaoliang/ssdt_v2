#ifndef PHYNETWORKPLUGIN_H
#define PHYNETWORKPLUGIN_H

#include "extensionsystem/iplugin.h"

namespace PhyNetwork {
namespace Internal {

class PhyNetworkPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "bless.ssdt.plugin")
#endif

// Construction and Destruction
public:
    PhyNetworkPlugin();
    ~PhyNetworkPlugin();

// Operations
public:
    virtual bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    virtual void extensionsInitialized();
    virtual void aboutToShutdown();

private slots:
    void    SlotReadSettings();
    void    SlotWriteSettings();
};

} // namespace Internal
} // namespace PhyNetwork

#endif // PHYNETWORKPLUGIN_H
