#ifndef STYLEDTITLEBAR_H
#define STYLEDTITLEBAR_H

#include <QWidget>
#include "styledui_global.h"

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

namespace StyledUi {

class StyledTitleBarPrivate;
class STYLEDUI_EXPORT StyledTitleBar : public QWidget
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum TitleButtons { tbNoButton = 0, tbSkin = 1, tbMenu = 2, tbShowMin = 4, tbShowMax = 8, tbClose = 16 };

// Construction and Destruction
public:
    StyledTitleBar(QWidget *pParent);
    ~StyledTitleBar();

// Operations
public:
    void            SetIcon(const QIcon &icon);
    void            SetTitle(const QString &strTitle);
    void            SetMenu(QMenu *pMenu);

    int             GetButtonFlags() const;
    void            SetButtonFlags(int iButtonFlags);

protected:
    virtual bool    eventFilter(QObject *watched, QEvent *event);

// Properties
private:
    StyledTitleBarPrivate *m_d;

private slots:
    void SlotShowMax();
    void SlotMenu();

signals:
    void sigSkinClicked();
};

} // StyledUi

#endif // STYLEDTITLEBAR_H
