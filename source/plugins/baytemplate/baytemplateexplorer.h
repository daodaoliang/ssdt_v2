#ifndef BAYTEMPLATEEXPLORER_H
#define BAYTEMPLATEEXPLORER_H

#include "core/iexplorer.h"

QT_BEGIN_NAMESPACE
class QIcon;
QT_END_NAMESPACE

namespace BayTemplate {
namespace Internal {

class BayTemplateExplorerWidget;
class BayTemplateExplorer : public Core::IExplorer
{
    Q_OBJECT

// Construction and Destruction
public:
    BayTemplateExplorer(QObject *pParent = 0);
    virtual ~BayTemplateExplorer();

// Operations
public:
    virtual QString     GetName() const;
    virtual QString     GetPixmap() const;
    virtual int         GetPriority() const;
    virtual QWidget*    GetWidget() const;

// Properties
private:
    BayTemplateExplorerWidget    *m_pBayTemplateExplorerWidget;
};

} // namespace Internal
} // namespace BayTemplate

#endif // BAYTEMPLATEEXPLORER_H
