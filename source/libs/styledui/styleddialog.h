#ifndef STYLEDDIALOG_H
#define STYLEDDIALOG_H

#include <QDialog>
#include "styledui_global.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace StyledUi {

class StyledDialogPrivate;
class STYLEDUI_EXPORT StyledDialog : public QDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    StyledDialog(QWidget *pParent = 0);
    ~StyledDialog();

// Operations
public:
    void            SetWindowTitle(const QString &strTitle);
    QVBoxLayout*    GetClientLayout() const;

protected:
    virtual void    mousePressEvent(QMouseEvent *event);
    virtual void    mouseMoveEvent(QMouseEvent *event);
    virtual void    mouseReleaseEvent(QMouseEvent *event);
    virtual void    paintEvent(QPaintEvent *event);
    virtual void    showEvent(QShowEvent *event);

// Properties
private:
    StyledDialogPrivate *m_d;
};

} // StyledUi

#endif // STYLEDDIALOG_H
