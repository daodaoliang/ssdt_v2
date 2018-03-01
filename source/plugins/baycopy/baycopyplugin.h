#ifndef BAYCOPYPLUGIN_H
#define BAYCOPYPLUGIN_H

#include "extensionsystem/iplugin.h"

namespace BayCopy {
namespace Internal {

class BayCopyPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "bless.ssdt.plugin")
#endif

// Construction and Destruction
public:
    BayCopyPlugin();
    ~BayCopyPlugin();

// Operations
public:
    virtual bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    virtual void extensionsInitialized();
    virtual void aboutToShutdown();

private slots:
    void    SlotReadSettings();
    void    SlotWriteSettings();
    void    SlotActionCopyBay();
};

} // namespace Internal
} // namespace SclModel

#endif // BAYCOPYPLUGIN_H
