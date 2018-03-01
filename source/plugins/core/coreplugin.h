#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include "extensionsystem/iplugin.h"

namespace Core {

class VTerminalDefine;
class MainWindow;

namespace Internal {

class CorePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "bless.ssdt.plugin")
#endif

// Construction and Destruction
public:
    CorePlugin();
    ~CorePlugin();

// Operations
public:
    virtual bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    virtual void extensionsInitialized();
    virtual void aboutToShutdown();

// Properties
private:
    MainWindow      *m_pMainWindow;
    VTerminalDefine *m_pVTerminalDefine;
};

} // namespace Internal
} // namespace Core

#endif // COREPLUGIN_H
