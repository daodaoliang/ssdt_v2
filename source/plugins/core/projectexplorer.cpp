#include "projectexplorer.h"
#include "projectexplorerwidget.h"

using namespace Core;
using namespace Core::Internal;

ProjectExplorer::ProjectExplorer(QObject *pParent) :
    IExplorer(pParent), m_pProjectExplorerWidget(new ProjectExplorerWidget)
{
}

ProjectExplorer::~ProjectExplorer()
{
}

QString ProjectExplorer::GetName() const
{
    return tr("Project");
}

QString ProjectExplorer::GetPixmap() const
{
    return QLatin1String(":/core/images/explorer_project.png");
}

int ProjectExplorer::GetPriority() const
{
    return 100;
}

QWidget* ProjectExplorer::GetWidget() const
{
    return m_pProjectExplorerWidget;
}
