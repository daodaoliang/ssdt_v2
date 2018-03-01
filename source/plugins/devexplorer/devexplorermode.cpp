#include "devexplorermode.h"
#include "devexplorermodewidget.h"

using namespace DevExplorer::Internal;

DevExplorerMode::DevExplorerMode(QObject *pParent) :
    Core::IMode(pParent), m_pDevExplorerModeWidget(new DevExplorerModeWidget)
{
}

DevExplorerMode::~DevExplorerMode()
{
    if(m_pDevExplorerModeWidget)
    {
        delete m_pDevExplorerModeWidget;
        m_pDevExplorerModeWidget = 0;
    }
}

QString DevExplorerMode::GetId() const
{
    return QLatin1String("Mode.DevExplorer");
}

QString DevExplorerMode::GetName() const
{
    return tr("Device Object");
}

QString DevExplorerMode::GetPixmap() const
{
    return ":/devexplorer/images/devmode";
}

int DevExplorerMode::GetPriority() const
{
    return 87;
}

QWidget* DevExplorerMode::GetWidget() const
{
    return m_pDevExplorerModeWidget;
}

void DevExplorerMode::ActivateObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    m_pDevExplorerModeWidget->ActivateObject(pProjectObject, bBayObject);
}

void DevExplorerMode::SetContextMenuObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject)
{
    m_pDevExplorerModeWidget->SetContextMenuObject(pProjectObject, bBayObject);
}
