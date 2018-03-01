#ifndef SCLMODELPLUGIN_H
#define SCLMODELPLUGIN_H

#include "extensionsystem/iplugin.h"

namespace SclModel {
namespace Internal {

class SclModelPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "bless.ssdt.plugin")
#endif

// Construction and Destruction
public:
    SclModelPlugin();
    ~SclModelPlugin();

// Operations
public:
    virtual bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    virtual void extensionsInitialized();
    virtual void aboutToShutdown();

private slots:
    void    SlotReadSettings();
    void    SlotWriteSettings();
    void    SlotActionDescSync();
};

} // namespace Internal
} // namespace SclModel

#endif // SCLMODELPLUGIN_H
