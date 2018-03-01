#include <QVBoxLayout>
#include <QToolButton>
#include <QSplitter>

#include "styledui/styledbar.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"

#include "cubicleconnmodewidget.h"
#include "widgetcubicleconn.h"
#include "configwidgetcubicleconn.h"

using namespace PhyNetwork::Internal;

CubicleConnModeWidget::CubicleConnModeWidget(QWidget *pParent)
    : StyledUi::StyledWidget(pParent), m_pProjectVersion(0)
{
    m_pConfigWidgetCubicleConn = new ConfigWidgetCubicleConn(this);
    m_pConfigWidgetCubicleConn->SetupConfigWidget();
    m_pWidgetCubicleConn = new WidgetCubicleConn(this);

    m_pSplitter = new QSplitter(this);
    m_pSplitter->setOrientation(Qt::Vertical);
    m_pSplitter->addWidget(m_pWidgetCubicleConn);
    m_pSplitter->addWidget(m_pConfigWidgetCubicleConn);
    m_pSplitter->setStretchFactor(1, 1);

    m_pStyleBar = new StyledUi::StyledBar(this);
    QHBoxLayout *pHBoxLayout = new QHBoxLayout(m_pStyleBar);
    pHBoxLayout->setContentsMargins(0, 0, 0, 0);
    pHBoxLayout->setSpacing(0);
    pHBoxLayout->addStretch(1);
    foreach(QAction *pAction, m_pWidgetCubicleConn->GetActions())
    {
        QToolButton *pToolButton = new QToolButton(m_pStyleBar);
        pToolButton->setFocusPolicy(Qt::NoFocus);
        pToolButton->setAutoRaise(true);
        pToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        pToolButton->setDefaultAction(pAction);
        pHBoxLayout->addWidget(pToolButton);
    }

    QVBoxLayout *pVBoxLayoutMain = new QVBoxLayout(this);
    pVBoxLayoutMain->setContentsMargins(0, 0, 0, 0);
    pVBoxLayoutMain->addWidget(m_pStyleBar);
    pVBoxLayoutMain->addWidget(m_pSplitter);

    connect(m_pWidgetCubicleConn, SIGNAL(sigCurrentCubicleChanged(ProjectExplorer::PeCubicle*)),
            m_pConfigWidgetCubicleConn, SLOT(SlotSetCurrentCubicle(ProjectExplorer::PeCubicle*)));
    connect(m_pConfigWidgetCubicleConn, SIGNAL(sigCurrentCubicleConnChanged(ProjectExplorer::PeCubicleConn*)),
            m_pWidgetCubicleConn, SLOT(SlotSetCubicleConn(ProjectExplorer::PeCubicleConn*)));
}

CubicleConnModeWidget::~CubicleConnModeWidget()
{
}

void CubicleConnModeWidget::SetProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    ProjectExplorer::PeCubicle *pCubicleSelected = 0;

    if(pProjectObject)
    {
        m_pProjectVersion = pProjectObject->GetProjectVersion();

        if(ProjectExplorer::PeBay *pBay = qobject_cast<ProjectExplorer::PeBay*>(pProjectObject))
        {
            QList<ProjectExplorer::PeDevice*> lstDevices = pBay->GetChildDevices();
            qSort(lstDevices.begin(), lstDevices.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

            if(!lstDevices.isEmpty())
                pCubicleSelected = lstDevices.first()->GetParentCubicle();
        }
        else if(ProjectExplorer::PeRoom *pRoom = qobject_cast<ProjectExplorer::PeRoom*>(pProjectObject))
        {
            QList<ProjectExplorer::PeCubicle*> lstCubicles = pRoom->GetChildCubicles();
            qSort(lstCubicles.begin(), lstCubicles.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

            if(!lstCubicles.isEmpty())
                pCubicleSelected = lstCubicles.first();
        }
        else if(ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectObject))
        {
            pCubicleSelected = pCubicle;
        }
        else if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
        {
            pCubicleSelected = pDevice->GetParentCubicle();
        }
        else if(ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pProjectObject))
        {
            pCubicleSelected = pBoard->GetParentDevice()->GetParentCubicle();
        }
    }
    else
    {
        m_pProjectVersion = 0;
        pCubicleSelected = 0;
    }

    m_pWidgetCubicleConn->SetProjectVersion(m_pProjectVersion, pCubicleSelected);
    if(m_pProjectVersion)
        m_pConfigWidgetCubicleConn->BuildModel(m_pProjectVersion);
    else
        m_pConfigWidgetCubicleConn->CleanModel();

    m_pStyleBar->setVisible(pProjectObject);
    m_pSplitter->setVisible(pProjectObject);
}
