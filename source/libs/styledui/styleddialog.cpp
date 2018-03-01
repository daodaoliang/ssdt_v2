#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QWindow>

#include "styleddialog.h"
#include "styledtitlebar.h"
#include "stylehelper.h"

namespace StyledUi {

class StyledDialogPrivate
{
public:
    StyledDialogPrivate() : m_pTitleBar(0), m_bMoveStated(false)
    {
    }

    StyledTitleBar  *m_pTitleBar;
    QVBoxLayout     *m_pVBoxLayoutClient;
    bool            m_bMoveStated;
    QPoint          m_ptPrevPosition;

};

} // StyledUi

using namespace StyledUi;

StyledDialog::StyledDialog(QWidget *pParent) :
    QDialog(pParent), m_d(new StyledDialogPrivate)
{
#if QT_VERSION >= 0x050000
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
#else
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
#endif
    setAttribute(Qt::WA_TranslucentBackground);
    //setStyleSheet(StyledUi::StyleHelper::GetStyleSheet());

    m_d->m_pTitleBar = new StyledTitleBar(this);
    m_d->m_pTitleBar->SetButtonFlags(StyledTitleBar::tbClose);
    m_d->m_pVBoxLayoutClient = new QVBoxLayout;

    QVBoxLayout *pVBoxLayout = new QVBoxLayout(this);
    pVBoxLayout->setSpacing(0);
    const int iFrameWith = StyledUi::StyleHelper::GetWidgetFrameWidth();
    pVBoxLayout->setContentsMargins(iFrameWith, iFrameWith, iFrameWith, iFrameWith);
    pVBoxLayout->addWidget(m_d->m_pTitleBar);
    pVBoxLayout->addLayout(m_d->m_pVBoxLayoutClient);
}

StyledDialog::~StyledDialog()
{
}

void StyledDialog::SetWindowTitle(const QString &strTitle)
{
    m_d->m_pTitleBar->SetTitle(strTitle);
    setWindowTitle(strTitle);
}

QVBoxLayout* StyledDialog::GetClientLayout() const
{
    return m_d->m_pVBoxLayoutClient;
}

void StyledDialog::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton)
        return;

    m_d->m_bMoveStated = true;
    m_d->m_ptPrevPosition = event->globalPos();
}

void StyledDialog::mouseMoveEvent(QMouseEvent *event)
{
    if(m_d->m_bMoveStated)
    {
        move(pos() + event->globalPos() - m_d->m_ptPrevPosition);
        m_d->m_ptPrevPosition = event->globalPos();
    }
}

void StyledDialog::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    m_d->m_bMoveStated = false;
}

void StyledDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    const int iFrameWith = StyledUi::StyleHelper::GetWidgetFrameWidth();

    {
        QPainter painter(this);
        StyledUi::StyleHelper::DrawWidgetFrame(&painter, rect());
    }

    {
        QPainter painter(this);
        StyledUi::StyleHelper::DrawWidgetPixmap(&painter, QRect(iFrameWith, iFrameWith, this->width() - 2 * iFrameWith, this->height() - 2 * iFrameWith));
    }

    {
        QPainter painter(this);
        StyledUi::StyleHelper::DrawDialogClient(&painter, QRect(iFrameWith, iFrameWith + m_d->m_pTitleBar->height(), this->width() - 2 * iFrameWith, this->height() - 2 * iFrameWith - m_d->m_pTitleBar->height()));
    }
}

void StyledDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    QWidget *pParent = parentWidget();
    if(pParent)
        pParent = pParent->window();

    int iScreen = 0;
    if(pParent)
        iScreen = QApplication::desktop()->screenNumber(pParent);
    else if (QApplication::desktop()->isVirtualDesktop())
        iScreen = QApplication::desktop()->screenNumber(QCursor::pos());
    else
        iScreen = QApplication::desktop()->screenNumber(this);
    QRect rcDesktop = QApplication::desktop()->availableGeometry(iScreen);

    int iExtraWidth = 0, iExtraHeight = 0;
    QWidgetList lstTopLevelWidgets = QApplication::topLevelWidgets();
    for(int i = 0; (iExtraWidth == 0 || iExtraHeight == 0) && i < lstTopLevelWidgets.size(); ++i)
    {
        QWidget *pWidget = lstTopLevelWidgets.at(i);
        if(pWidget->isVisible())
        {
            int iFrameWidth = pWidget->geometry().x() - pWidget->x();
            int iFrameHeight = pWidget->geometry().y() - pWidget->y();

            iExtraWidth = qMax(iExtraWidth, iFrameWidth);
            iExtraHeight = qMax(iExtraHeight, iFrameHeight);
        }
    }

    QPoint ptPos(0, 0);
    if(pParent)
    {
        QPoint ptParent = pParent->mapToGlobal(QPoint(0,0));

        // Use pos() if the widget is embedded into a native window
        if(pParent->windowHandle() && pParent->windowHandle()->property("_q_embedded_native_parent_handle").value<WId>())
            ptParent = pParent->pos();

        ptPos = QPoint(ptParent.x() + pParent->width() / 2, ptParent.y() + pParent->height() / 2);
    }
    else  // ptPos = middle of the desktop
    {
        ptPos = QPoint(rcDesktop.x() + rcDesktop.width() / 2, rcDesktop.y() + rcDesktop.height() /2 );
    }

    // ptParent = origin of this
    ptPos = QPoint(ptPos.x() - width() / 2 - iExtraWidth, ptPos.y() - height() / 2 - iExtraHeight);


    if(ptPos.x() + iExtraWidth + width() > rcDesktop.x() + rcDesktop.width())
        ptPos.setX(rcDesktop.x() + rcDesktop.width() - width() - iExtraWidth);
    if(ptPos.x() < rcDesktop.x())
        ptPos.setX(rcDesktop.x());

    if(ptPos.y() + iExtraHeight + height() > rcDesktop.y() + rcDesktop.height())
        ptPos.setY(rcDesktop.y() + rcDesktop.height() - height() - iExtraHeight);
    if(ptPos.y() < rcDesktop.y())
        ptPos.setY(rcDesktop.y());

    move(ptPos);
}
