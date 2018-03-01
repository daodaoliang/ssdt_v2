#include <QHBoxLayout>
#include <QToolButton>

#include "projectexplorer/peprojectversion.h"

#include "core/configmodelview.h"

#include "configwidgetdeviceobject.h"

using namespace DevExplorer::Internal;

ConfigWidgetDeviceObject::ConfigWidgetDeviceObject(QWidget *pParent) :
    Core::ConfigWidget(pParent), m_pToolBarWidget(0)
{
}

ConfigWidgetDeviceObject::~ConfigWidgetDeviceObject()
{
    if(m_pToolBarWidget)
    {
        delete m_pToolBarWidget;
        m_pToolBarWidget = 0;
    }
}

QWidget* ConfigWidgetDeviceObject::GetToolBar()
{
    if(!m_pToolBarWidget)
    {
        m_pToolBarWidget = new QWidget;
        QHBoxLayout *pHBoxLayout = new QHBoxLayout(m_pToolBarWidget);
        pHBoxLayout->setContentsMargins(0, 0, 0, 0);
        pHBoxLayout->setSpacing(0);

        foreach(QAction *pAction, GetToolbarActions())
        {
            QToolButton *pToolButton = new QToolButton(m_pToolBarWidget);
            pToolButton->setFocusPolicy(Qt::NoFocus);
            pToolButton->setAutoRaise(true);
            pToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            pToolButton->setDefaultAction(pAction);
            pHBoxLayout->addWidget(pToolButton, 0, Qt::AlignRight);
        }
    }

    return m_pToolBarWidget;
}

QList<QAction*> ConfigWidgetDeviceObject::GetToolbarActions()
{
    return QList<QAction*>();
}
