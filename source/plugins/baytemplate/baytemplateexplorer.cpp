#include "baytemplateexplorer.h"
#include "baytemplateexplorerwidget.h"

using namespace BayTemplate;
using namespace BayTemplate::Internal;

BayTemplateExplorer::BayTemplateExplorer(QObject *pParent) :
    IExplorer(pParent), m_pBayTemplateExplorerWidget(new BayTemplateExplorerWidget)
{
}

BayTemplateExplorer::~BayTemplateExplorer()
{
}

QString BayTemplateExplorer::GetName() const
{
    return tr("Template");
}

QString BayTemplateExplorer::GetPixmap() const
{
    return QLatin1String(":/baytemplate/images/explorer_template.png");
}

int BayTemplateExplorer::GetPriority() const
{
    return 30;
}

QWidget* BayTemplateExplorer::GetWidget() const
{
    return m_pBayTemplateExplorerWidget;
}
