#include "cubicleconnmode.h"
#include "cubicleconnmodewidget.h"

using namespace PhyNetwork::Internal;

CubicleConnMode::CubicleConnMode(QObject *pParent) :
    Core::IMode(pParent), m_pCubicleConnModeWidget(new CubicleConnModeWidget)
{
}

CubicleConnMode::~CubicleConnMode()
{
    if(m_pCubicleConnModeWidget)
    {
        delete m_pCubicleConnModeWidget;
        m_pCubicleConnModeWidget = 0;
    }
}

QString CubicleConnMode::GetId() const
{
    return QLatin1String("Mode.CubicleConn");
}

QString CubicleConnMode::GetName() const
{
    return tr("Cubicle Connection");
}

QString CubicleConnMode::GetPixmap() const
{
    return ":/phynetwork/images/cubicleconnmode";
}

int CubicleConnMode::GetPriority() const
{
    return 35;
}

QWidget* CubicleConnMode::GetWidget() const
{
    return m_pCubicleConnModeWidget;
}

void CubicleConnMode::ActivateObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    Q_UNUSED(bBayObject)

    m_pCubicleConnModeWidget->SetProjectObject(pProjectObject);
}

void CubicleConnMode::SetContextMenuObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    Q_UNUSED(pProjectObject)
    Q_UNUSED(bBayObject)
}
