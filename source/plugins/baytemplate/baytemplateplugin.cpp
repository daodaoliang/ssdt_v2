#include <QtPlugin>
#include <QStringList>
#include <QAction>

#include "core/core_constants.h"
#include "core/actionmanager.h"
#include "core/actioncontainer.h"
#include "core/command.h"
#include "core/mainwindow.h"

#include "baytemplateplugin.h"
#include "baytemplateexplorer.h"
#include "baytemplatedlg.h"

using namespace BayTemplate::Internal;

BayTemplatePlugin::BayTemplatePlugin()
{
}

BayTemplatePlugin::~BayTemplatePlugin()
{
}

bool BayTemplatePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    return true;
}

void BayTemplatePlugin::extensionsInitialized()
{
    addAutoReleasedObject(new BayTemplateExplorer);

    QList<int> lstGlobalContexts = QList<int>() << Core::Constants::g_iUID_Context_Global;

    Core::CActionManager *pActionManager = Core::MainWindow::Instance()->GetActionManager();
    Core::CActionContainer *pActionContainerMenu = pActionManager->GetActionContainer(Core::Constants::g_szUID_Menu);

    QAction *pActionToolsBayTemplate = new QAction(tr("&Bay Template..."), this);
    Core::CCommand *pCommand = pActionManager->RegisterAction(pActionToolsBayTemplate, QLatin1String("BayTemplate.Action.ToolsBayTemplate"), lstGlobalContexts);
    pCommand->SetDefaultKeySequence(QKeySequence("Ctrl+P"));
    pActionContainerMenu->AddAction(pCommand, Core::Constants::g_szUID_MenuGroup_Tools);
    connect(pActionToolsBayTemplate, SIGNAL(triggered()), this, SLOT(SlotActionBayTemplate()));
}

void BayTemplatePlugin::aboutToShutdown()
{
}

void BayTemplatePlugin::SlotActionBayTemplate()
{
    BayTemplateDlg dlg(Core::MainWindow::Instance());
    dlg.exec();
}

Q_EXPORT_PLUGIN(BayTemplatePlugin)
