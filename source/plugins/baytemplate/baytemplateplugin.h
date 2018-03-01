#ifndef BAYTEMPLATEPLUGIN_H
#define BAYTEMPLATEPLUGIN_H

#include "extensionsystem/iplugin.h"

namespace BayTemplate {
namespace Internal {

class BayTemplatePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "bless.ssdt.plugin")
#endif

// Construction and Destruction
public:
    BayTemplatePlugin();
    ~BayTemplatePlugin();

// Operations
public:
    virtual bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    virtual void extensionsInitialized();
    virtual void aboutToShutdown();

private slots:
    void SlotActionBayTemplate();
};

} // namespace Internal
} // namespace BayTemplate

#endif // BAYTEMPLATEPLUGIN_H
