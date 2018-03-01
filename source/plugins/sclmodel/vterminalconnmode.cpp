#include <QIcon>

#include "vterminalconnmode.h"
#include "vterminalconnmodewidget.h"

using namespace SclModel::Internal;

VTerminalConnMode::VTerminalConnMode(QObject *pParent) :
    Core::IMode(pParent), m_pVTerminalConnModeWidget(new VTerminalConnModeWidget)
{
}

VTerminalConnMode::~VTerminalConnMode()
{
    if(m_pVTerminalConnModeWidget)
    {
        delete m_pVTerminalConnModeWidget;
        m_pVTerminalConnModeWidget = 0;
    }
}

QString VTerminalConnMode::GetId() const
{
    return QLatin1String("Mode.VTerminalConn");
}

QString VTerminalConnMode::GetName() const
{
    return tr("Virtual Connection");
}

QString VTerminalConnMode::GetPixmap() const
{
    return ":/sclmodel/images/vterminalconnmode";
}

int VTerminalConnMode::GetPriority() const
{
    return 5;
}

QWidget* VTerminalConnMode::GetWidget() const
{
    return m_pVTerminalConnModeWidget;
}

void VTerminalConnMode::ActivateObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    Q_UNUSED(bBayObject)

    m_pVTerminalConnModeWidget->SetProjectObject(pProjectObject);
}

void VTerminalConnMode::SetContextMenuObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    Q_UNUSED(pProjectObject)
    Q_UNUSED(bBayObject)
}
