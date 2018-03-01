#include "subnetworkmode.h"
#include "subnetworkmodewidget.h"

using namespace SclModel::Internal;

SubNetworkMode::SubNetworkMode(QObject *pParent) :
    Core::IMode(pParent), m_pSubNetworkModeWidget(new SubNetworkModeWidget)
{
}

SubNetworkMode::~SubNetworkMode()
{
    if(m_pSubNetworkModeWidget)
    {
        delete m_pSubNetworkModeWidget;
        m_pSubNetworkModeWidget = 0;
    }
}

QString SubNetworkMode::GetId() const
{
    return QLatin1String("Mode.SubNetwork");
}

QString SubNetworkMode::GetName() const
{
    return tr("Logic SubNetwork");
}

QString SubNetworkMode::GetPixmap() const
{
    return ":/sclmodel/images/subnetworkmode";
}

int SubNetworkMode::GetPriority() const
{
    return 4;
}

QWidget* SubNetworkMode::GetWidget() const
{
    return m_pSubNetworkModeWidget;
}

void SubNetworkMode::ActivateObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    Q_UNUSED(bBayObject)

    m_pSubNetworkModeWidget->SetProjectObject(pProjectObject);
}

void SubNetworkMode::SetContextMenuObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    Q_UNUSED(pProjectObject)
    Q_UNUSED(bBayObject)
}
