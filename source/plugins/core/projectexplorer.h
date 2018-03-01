#ifndef PROJECTEXPLORER_H
#define PROJECTEXPLORER_H

#include "iexplorer.h"

QT_BEGIN_NAMESPACE
class QIcon;
QT_END_NAMESPACE

namespace Core {
namespace Internal {

class ProjectExplorerWidget;
class ProjectExplorer : public Core::IExplorer
{
    Q_OBJECT

// Construction and Destruction
public:
    ProjectExplorer(QObject *pParent = 0);
    virtual ~ProjectExplorer();

// Operations
public:
    virtual QString     GetName() const;
    virtual QString     GetPixmap() const;
    virtual int         GetPriority() const;
    virtual QWidget*    GetWidget() const;

// Properties
private:
    ProjectExplorerWidget   *m_pProjectExplorerWidget;
};

} // namespace Internal
} // namespace Core

#endif // PROJECTEXPLORER_H
