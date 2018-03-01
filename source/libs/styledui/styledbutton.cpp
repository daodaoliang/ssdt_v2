#include <QPainter>
#include <QMouseEvent>
#include <QIcon>

#include "styledbutton.h"

namespace StyledUi {

class StyledButtonPrivate
{
public:
    StyledButtonPrivate() : m_eMode(QIcon::Normal)
    {
    }

    QIcon           m_icon;
    QIcon::Mode     m_eMode;
    QSize           m_sz;
    QColor          m_crHoveredBkGround;
};

StyledButton::StyledButton(const QString &strPixmap, const QString &strName, QWidget *pParent) : QWidget(pParent), m_d(new StyledButtonPrivate)
{
    SetPixmap(strPixmap);
    SetName(strName);
}

StyledButton::~StyledButton()
{
}

void StyledButton::SetPixmap(const QString &strPixmap)
{
    QPixmap pxpActive(strPixmap);
    if(pxpActive.isNull())
        return;

    QPixmap pxpNormal(pxpActive.size());
    pxpNormal.fill(Qt::transparent);
    QPainter painterNormal(&pxpNormal);
    painterNormal.setCompositionMode(QPainter::CompositionMode_Source);
    painterNormal.drawPixmap(0, 0, pxpActive);
    painterNormal.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painterNormal.fillRect(pxpNormal.rect(), QColor(0, 0, 0, 210));

    QPixmap pxpDisable(pxpActive.size());
    pxpDisable.fill(Qt::transparent);
    QPainter painterDisable(&pxpDisable);
    painterDisable.setCompositionMode(QPainter::CompositionMode_Source);
    painterDisable.drawPixmap(0, 0, pxpActive);
    painterDisable.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painterDisable.fillRect(pxpDisable.rect(), QColor(200, 200, 200, 150));

    m_d->m_icon.addPixmap(pxpNormal, QIcon::Normal, QIcon::Off);
    m_d->m_icon.addPixmap(pxpActive, QIcon::Active, QIcon::Off);
    m_d->m_icon.addPixmap(pxpDisable, QIcon::Disabled, QIcon::Off);

    if(m_d->m_sz.isNull())
        SetSize(pxpActive.size());
}

void StyledButton::SetHoveredBKColor(const QColor &crHoveredBkGround)
{
    m_d->m_crHoveredBkGround = crHoveredBkGround;
}

void StyledButton::SetName(const QString &strName)
{
    setToolTip(strName);
}

void StyledButton::SetSize(const QSize &size)
{
    if(m_d->m_sz != size)
    {
        m_d->m_sz = size;
        setFixedSize(size);
    }
}

void StyledButton::SetEnabled(bool bEnabled)
{
    m_d->m_eMode = (bEnabled ? QIcon::Normal : QIcon::Disabled);
    setEnabled(bEnabled);
}

void StyledButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);

    if(m_d->m_eMode != QIcon::Disabled)
    {
        m_d->m_eMode = QIcon::Active;
        update();
    }
}

void StyledButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);

    if(m_d->m_eMode != QIcon::Disabled)
    {
        m_d->m_eMode = QIcon::Normal;
        update();
    }
}

void StyledButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && m_d->m_eMode != QIcon::Disabled)
    {
        m_d->m_eMode = QIcon::Selected;
        update();
    }
}

void StyledButton::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_d->m_eMode == QIcon::Selected)
    {
        m_d->m_eMode = QIcon::Normal;

        if(rect().contains(event->pos()))
        {
            update();
            emit sigClicked();
        }
    }
}

void StyledButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    if(m_d->m_crHoveredBkGround.isValid() && (m_d->m_eMode == QIcon::Active || m_d->m_eMode == QIcon::Selected))
    {
        painter.setBrush(m_d->m_crHoveredBkGround);
        painter.setPen(Qt::NoPen);
        painter.drawRect(rect());
    }

    m_d->m_icon.paint(&painter, rect(), Qt::AlignCenter, m_d->m_eMode, QIcon::Off);
}

} // StyledUi
