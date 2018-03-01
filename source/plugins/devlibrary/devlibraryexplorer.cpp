#include "devlibraryexplorer.h"
#include "devlibraryexplorerwidget.h"

using namespace DevLibrary;
using namespace DevLibrary::Internal;

DevLibraryExplorer::DevLibraryExplorer(QObject *pParent) :
    IExplorer(pParent), m_pDevLibraryExplorerWidget(new DevLibraryExplorerWidget)
{
}

DevLibraryExplorer::~DevLibraryExplorer()
{
}

QString DevLibraryExplorer::GetName() const
{
    return tr("Product");
}

QString DevLibraryExplorer::GetPixmap() const
{
    return QLatin1String(":/devlibrary/images/explorer_product.png");
}

int DevLibraryExplorer::GetPriority() const
{
    return 50;
}

QWidget* DevLibraryExplorer::GetWidget() const
{
    return m_pDevLibraryExplorerWidget;
}
