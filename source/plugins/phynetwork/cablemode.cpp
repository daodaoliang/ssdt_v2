#include "cablemode.h"
#include "cablemodewidget.h"

using namespace PhyNetwork::Internal;

CableMode::CableMode(QObject *pParent) :
    Core::IMode(pParent), m_pCableModeWidget(new CableModeWidget)
{
}

CableMode::~CableMode()
{
    if(m_pCableModeWidget)
    {
        delete m_pCableModeWidget;
        m_pCableModeWidget = 0;
    }
}

QString CableMode::GetId() const
{
    return QLatin1String("Mode.Cable");
}

QString CableMode::GetName() const
{
    return tr("Cable Management");
}

QString CableMode::GetPixmap() const
{
    return ":/phynetwork/images/cablemode";
}

int CableMode::GetPriority() const
{
    return 30;
}

QWidget* CableMode::GetWidget() const
{
    return m_pCableModeWidget;
}

void CableMode::ActivateObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    Q_UNUSED(bBayObject)

    m_pCableModeWidget->SetProjectObject(pProjectObject);
}

void CableMode::SetContextMenuObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    Q_UNUSED(pProjectObject)
    Q_UNUSED(bBayObject)
}
