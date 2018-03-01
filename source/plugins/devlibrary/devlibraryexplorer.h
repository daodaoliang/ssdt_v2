#ifndef DEVLIBRARYEXPLORER_H
#define DEVLIBRARYEXPLORER_H

#include "core/iexplorer.h"

QT_BEGIN_NAMESPACE
class QIcon;
QT_END_NAMESPACE

namespace DevLibrary {
namespace Internal {

class DevLibraryExplorerWidget;
class DevLibraryExplorer : public Core::IExplorer
{
    Q_OBJECT

// Construction and Destruction
public:
    DevLibraryExplorer(QObject *pParent = 0);
    virtual ~DevLibraryExplorer();

// Operations
public:
    virtual QString     GetName() const;
    virtual QString     GetPixmap() const;
    virtual int         GetPriority() const;
    virtual QWidget*    GetWidget() const;

// Properties
private:
    DevLibraryExplorerWidget    *m_pDevLibraryExplorerWidget;
};

} // namespace Internal
} // namespace DevLibrary

#endif // DEVLIBRARYEXPLORER_H
