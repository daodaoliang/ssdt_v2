#include <QApplication>
#include <QDesktopWidget>
#include <QPushButton>
#include <QToolButton>
#include <QPainter>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>

#include "styledtitlebar.h"
#include "styledbutton.h"
#include "stylehelper.h"

namespace StyledUi {

class StyledTitleBarPrivate
{
public:
    StyledTitleBarPrivate() : m_pMenu(0)
    {
    }

    QLabel              *m_pLabelIcon;
    QLabel              *m_pLabelTitle;
    QMenu               *m_pMenu;
    int                 m_iButtonFlags;
    QByteArray          m_baGeometry;
    StyledButton        *m_pStyledButtonSkin;
    StyledButton        *m_pStyledButtonMenu;
    StyledButton        *m_pStyledButtonShowMin;
    StyledButton        *m_pStyledButtonShowMax;
    StyledButton        *m_pStyledButtonClose;
};

///////////////////////////////////////////////////////////////////
// Class StyledTitleBar
///////////////////////////////////////////////////////////////////
StyledTitleBar::StyledTitleBar(QWidget *pParent) :
    QWidget(pParent), m_d(new StyledTitleBarPrivate)
{
    pParent->installEventFilter(this);

    m_d->m_pLabelIcon = new QLabel(this);
    m_d->m_pLabelTitle = new QLabel(this);
    m_d->m_pLabelTitle->setStyleSheet("QLabel {background:transparent;border:0px;color:white;}");

    m_d->m_pStyledButtonSkin = new StyledButton(":/styledui/images/system/skin.png", tr("Change Skin"), this);
    m_d->m_pStyledButtonSkin->SetSize(QSize(23, 23));

    m_d->m_pStyledButtonMenu = new StyledButton(":/styledui/images/system/menu.png", tr("Main Menu"), this);
    m_d->m_pStyledButtonMenu->SetSize(QSize(23, 23));

    m_d->m_pStyledButtonShowMin = new StyledButton(":/styledui/images/system/min.png", tr("Minimize"), this);
    m_d->m_pStyledButtonShowMin->SetSize(QSize(23, 23));

    m_d->m_pStyledButtonShowMax = new StyledButton(":/styledui/images/system/max.png", tr("Maximize"), this);
    m_d->m_pStyledButtonShowMax->SetSize(QSize(23, 23));

    m_d->m_pStyledButtonClose = new StyledButton(":/styledui/images/system/close.png", tr("Close"), this);
    m_d->m_pStyledButtonClose->SetSize(QSize(25, 25));
    m_d->m_pStyledButtonClose->SetHoveredBKColor(Qt::red);

    connect(m_d->m_pStyledButtonSkin, SIGNAL(sigClicked()), this, SIGNAL(sigSkinClicked()));
    connect(m_d->m_pStyledButtonMenu, SIGNAL(sigClicked()), this, SLOT(SlotMenu()));
    connect(m_d->m_pStyledButtonShowMin, SIGNAL(sigClicked()), pParent, SLOT(showMinimized()));
    connect(m_d->m_pStyledButtonShowMax, SIGNAL(sigClicked()), this, SLOT(SlotShowMax()));
    connect(m_d->m_pStyledButtonClose, SIGNAL(sigClicked()), pParent, SLOT(close()));

    QHBoxLayout *pHBoxLayoutTitle = new QHBoxLayout;
    pHBoxLayoutTitle->setSpacing(0);
    pHBoxLayoutTitle->setContentsMargins(0, 0, 0, 0);

    pHBoxLayoutTitle->addWidget(m_d->m_pLabelIcon, 0, Qt::AlignVCenter);
    pHBoxLayoutTitle->addWidget(m_d->m_pLabelTitle, 0, Qt::AlignVCenter);
    pHBoxLayoutTitle->addStretch();
    pHBoxLayoutTitle->addWidget(m_d->m_pStyledButtonSkin, 0, Qt::AlignVCenter);
    pHBoxLayoutTitle->addSpacing(10);
    pHBoxLayoutTitle->addWidget(m_d->m_pStyledButtonMenu, 0, Qt::AlignVCenter);
    pHBoxLayoutTitle->addSpacing(10);
    pHBoxLayoutTitle->addWidget(m_d->m_pStyledButtonShowMin, 0, Qt::AlignVCenter);
    pHBoxLayoutTitle->addSpacing(10);
    pHBoxLayoutTitle->addWidget(m_d->m_pStyledButtonShowMax, 0, Qt::AlignVCenter);
    pHBoxLayoutTitle->addSpacing(10);
    pHBoxLayoutTitle->addWidget(m_d->m_pStyledButtonClose, 0, Qt::AlignVCenter);

    QVBoxLayout *pVBoxLayout = new QVBoxLayout(this);
    pVBoxLayout->setSpacing(0);
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->addLayout(pHBoxLayoutTitle);

    setFixedHeight(StyledUi::StyleHelper::GetTitleBarHeight());
    SetButtonFlags(tbShowMin | tbShowMax | tbClose);
}

StyledTitleBar::~StyledTitleBar()
{
}

void StyledTitleBar::SetIcon(const QIcon &icon)
{
    m_d->m_pLabelIcon->setPixmap(icon.pixmap(QSize(16, 16)));
}

void StyledTitleBar::SetTitle(const QString &strTitle)
{
    m_d->m_pLabelTitle->setText(strTitle);
}

void StyledTitleBar::SetMenu(QMenu *pMenu)
{
    m_d->m_pMenu = pMenu;
}

int StyledTitleBar::GetButtonFlags() const
{
    return m_d->m_iButtonFlags;
}

void StyledTitleBar::SetButtonFlags(int iButtonFlags)
{
    if(m_d->m_iButtonFlags == iButtonFlags)
        return;

    m_d->m_iButtonFlags = iButtonFlags;

    m_d->m_pStyledButtonSkin->setVisible(m_d->m_iButtonFlags & tbSkin);
    m_d->m_pStyledButtonMenu->setVisible(m_d->m_iButtonFlags & tbMenu);
    m_d->m_pStyledButtonShowMin->setVisible(m_d->m_iButtonFlags & tbShowMin);
    m_d->m_pStyledButtonShowMax->setVisible(m_d->m_iButtonFlags & tbShowMax);
    m_d->m_pStyledButtonClose->setVisible(m_d->m_iButtonFlags & tbClose);
}

bool StyledTitleBar::eventFilter(QObject *watched, QEvent *event)
{
    if(watched != parentWidget())
        return false;

    if(event->type() == QEvent::WindowStateChange)
    {
        Qt::WindowStates eWindowStates = parentWidget()->windowState();
        if(eWindowStates & Qt::WindowMaximized)
        {
            if(parentWidget()->layout())
                parentWidget()->layout()->setContentsMargins(0, 0, 0, 0);

            m_d->m_pStyledButtonShowMax->SetPixmap(":/styledui/images/system/normal.png");
            m_d->m_pStyledButtonShowMax->SetName(tr("Restore"));
        }
        else
        {
            const int iFrameWith = StyledUi::StyleHelper::GetWidgetFrameWidth();
            parentWidget()->layout()->setContentsMargins(iFrameWith, iFrameWith, iFrameWith, iFrameWith);

            m_d->m_pStyledButtonShowMax->SetPixmap(":/styledui/images/system/max.png");
            m_d->m_pStyledButtonShowMax->SetName(tr("Maximize"));
        }

        parentWidget()->repaint();
    }

    return false;
}

void StyledTitleBar::SlotShowMax()
{
    parentWidget()->setWindowState(parentWidget()->windowState() ^ Qt::WindowMaximized);
}

void StyledTitleBar::SlotMenu()
{
    if(m_d->m_iButtonFlags & tbMenu)
    {
        if(m_d->m_pMenu)
            m_d->m_pMenu->exec(mapToGlobal(m_d->m_pStyledButtonMenu->geometry().bottomLeft()));
    }
}

} // StyledUi
