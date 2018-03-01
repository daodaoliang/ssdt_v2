#include "infosetmode.h"
#include "infosetmodewidget.h"

using namespace PhyNetwork::Internal;

InfoSetMode::InfoSetMode(QObject *pParent) :
    Core::IMode(pParent), m_pInfoSetModeWidget(new InfoSetModeWidget)
{
}

InfoSetMode::~InfoSetMode()
{
    if(m_pInfoSetModeWidget)
    {
        delete m_pInfoSetModeWidget;
        m_pInfoSetModeWidget = 0;
    }
}

QString InfoSetMode::GetId() const
{
    return QLatin1String("Mode.InfoSet");
}

QString InfoSetMode::GetName() const
{
    return tr("Information Logic");
}

QString InfoSetMode::GetPixmap() const
{
    return ":/phynetwork/images/infosetmode";
}

int InfoSetMode::GetPriority() const
{
    return 40;
}

QWidget* InfoSetMode::GetWidget() const
{
    return m_pInfoSetModeWidget;
}

void InfoSetMode::ActivateObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    Q_UNUSED(bBayObject)

    m_pInfoSetModeWidget->SetProjectObject(pProjectObject);
}

void InfoSetMode::SetContextMenuObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    Q_UNUSED(pProjectObject)
    Q_UNUSED(bBayObject)
}
