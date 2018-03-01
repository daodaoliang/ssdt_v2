#include <QVBoxLayout>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

#include "styledwindow.h"
#include "styledtitlebar.h"
#include "stylehelper.h"

namespace StyledUi {

class StyledWindowPrivate
{
public:
    StyledWindowPrivate() : m_pTitleBar(0), m_bMoveStated(false)
    {
    }

    StyledTitleBar  *m_pTitleBar;
    bool            m_bMoveStated;
    QPoint          m_ptPrevPosition;
};

} // StyledUi

using namespace StyledUi;

StyledWindow::StyledWindow(QWidget *pParent) :
    QWidget(pParent), m_d(new StyledWindowPrivate)
{
#if QT_VERSION >= 0x050000
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#else
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#endif
    setAttribute(Qt::WA_TranslucentBackground);

    m_d->m_pTitleBar = new StyledTitleBar(this);
    m_d->m_pTitleBar->SetButtonFlags(StyledTitleBar::tbMenu | StyledTitleBar::tbShowMin | StyledTitleBar::tbShowMax | StyledTitleBar::tbClose);

    QVBoxLayout *pVBoxLayout = new QVBoxLayout(this);
    pVBoxLayout->setSpacing(0);
    pVBoxLayout->addWidget(m_d->m_pTitleBar);
}

StyledWindow::~StyledWindow()
{
}

void StyledWindow::SetWindowIcon(const QIcon &icon)
{
    m_d->m_pTitleBar->SetIcon(icon);
}

void StyledWindow::SetWindowTitle(const QString &strTitle)
{
    m_d->m_pTitleBar->SetTitle(strTitle);
    setWindowTitle(strTitle);
}

void StyledWindow::SetWindowMenu(QMenu *pMenu)
{
    m_d->m_pTitleBar->SetMenu(pMenu);
}

void StyledWindow::ShowMaximized(bool bMaximized)
{
    show();

    if(bMaximized)
        setWindowState(windowState() | Qt::WindowMaximized);
    else
        setWindowState(windowState() & ~Qt::WindowMaximized);
}

bool StyledWindow::IsMaximized() const
{
    return (windowState() & Qt::WindowMaximized);
}

void StyledWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() != Qt::LeftButton || IsMaximized())
        return;

    m_d->m_bMoveStated = true;
    m_d->m_ptPrevPosition = event->globalPos();
}

void StyledWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(m_d->m_bMoveStated)
    {
        move(pos() + event->globalPos() - m_d->m_ptPrevPosition);
        m_d->m_ptPrevPosition = event->globalPos();
    }
}

void StyledWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    m_d->m_bMoveStated = false;
}

void StyledWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    ShowMaximized(!IsMaximized());
}

void StyledWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    const int iFrameWith = IsMaximized() ? 0 : StyledUi::StyleHelper::GetWidgetFrameWidth();
    if(iFrameWith)
    {
        QPainter painter(this);
        StyledUi::StyleHelper::DrawWidgetFrame(&painter, rect());
    }

    QPainter painter(this);
    StyledUi::StyleHelper::DrawWidgetPixmap(&painter, QRect(iFrameWith, iFrameWith, width() - 2 * iFrameWith, height() - 2 * iFrameWith));
}
