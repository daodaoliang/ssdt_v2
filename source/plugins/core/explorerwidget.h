#ifndef EXPLORERWIDGET_H
#define EXPLORERWIDGET_H

#include <QSplitter>
#include "core_global.h"

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

namespace Core {

class ExplorerWidgetPrivate;
class CORE_EXPORT ExplorerWidget : public QSplitter
{
    Q_OBJECT

// Construction and Destruction
public:
    ExplorerWidget(QWidget *pParent = 0);
    ~ExplorerWidget();

// Properties
private:
    ExplorerWidgetPrivate *m_d;

private slots:
    void SlotCurrentTabChanged(int iIndex);
    void SlotObjectAdded(QObject *pObject);
    void SlotAboutToRemoveObject(QObject *pObject);
};

} // namespace Core

#endif // EXPLORERWIDGET_H
