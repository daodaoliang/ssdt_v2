#include <QComboBox>
#include <QToolButton>
#include <QLabel>
#include <QSplitter>
#include <QVBoxLayout>

#include "styledui/styledbar.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/peinfoset.h"

#include "styledui/stylehelper.h"
#include "core/configwidget.h"

#include "infosetmodewidget.h"
#include "widgetinfoset.h"
#include "configwidgetinfoset.h"

using namespace PhyNetwork::Internal;

InfoSetModeWidget::InfoSetModeWidget(QWidget *pParent) :
    StyledUi::StyledWidget(pParent), m_pBay(0)
{
    m_pConfigWidgetInfoSet = new ConfigWidgetInfoSet(this);
    m_pConfigWidgetInfoSet->SetupConfigWidget();
    m_pWidgetInfoSet = new WidgetInfoSet(this);

    m_pSplitter = new QSplitter(this);
    m_pSplitter->setOrientation(Qt::Vertical);
    m_pSplitter->addWidget(m_pWidgetInfoSet);
    m_pSplitter->addWidget(m_pConfigWidgetInfoSet);

    m_pStyleBar = new StyledUi::StyledBar(this);
    m_pLabelDisplayIcon = new QLabel(m_pStyleBar);
    m_pLabelDisplayName = new QLabel(m_pStyleBar);

    QHBoxLayout *pHBoxLayout = new QHBoxLayout(m_pStyleBar);
    pHBoxLayout->setContentsMargins(10, 0, 0, 0);
    pHBoxLayout->addWidget(m_pLabelDisplayIcon);
    pHBoxLayout->addWidget(m_pLabelDisplayName);
    pHBoxLayout->addStretch(1);

    foreach(QAction *pAction, m_pWidgetInfoSet->GetActions())
    {
        QToolButton *pToolButton = new QToolButton(m_pStyleBar);
        pToolButton->setFocusPolicy(Qt::NoFocus);
        pToolButton->setAutoRaise(true);
        pToolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        pToolButton->setDefaultAction(pAction);
        pHBoxLayout->addWidget(pToolButton);
    }

    QVBoxLayout *pVBoxLayoutMain = new QVBoxLayout(this);
    pVBoxLayoutMain->setSpacing(0);
    pVBoxLayoutMain->setContentsMargins(0, 0, 0, 0);
    pVBoxLayoutMain->addWidget(m_pStyleBar);
    pVBoxLayoutMain->addWidget(m_pSplitter);

    connect(m_pConfigWidgetInfoSet, SIGNAL(sigCurrentInfoSetChanged(ProjectExplorer::PeInfoSet*)),
            m_pWidgetInfoSet, SLOT(SlotSetInfoSet(ProjectExplorer::PeInfoSet*)));
}

InfoSetModeWidget::~InfoSetModeWidget()
{
}

void InfoSetModeWidget::SetProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    ProjectExplorer::PeDevice *pDeviceSelected = 0;

    if(pProjectObject)
    {
        if(ProjectExplorer::PeBay *pBay = qobject_cast<ProjectExplorer::PeBay*>(pProjectObject))
        {
            m_pBay = pBay;

            QList<ProjectExplorer::PeDevice*> lstDevices;
            foreach(ProjectExplorer::PeDevice *pDevice ,pBay->GetChildDevices())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    lstDevices.append(pDevice);
            }
            qSort(lstDevices.begin(), lstDevices.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

            if(!lstDevices.isEmpty())
                pDeviceSelected = lstDevices.first();
        }
        else if(ProjectExplorer::PeRoom *pRoom = qobject_cast<ProjectExplorer::PeRoom*>(pProjectObject))
        {
            QList<ProjectExplorer::PeCubicle*> lstCubicles = pRoom->GetChildCubicles();
            qSort(lstCubicles.begin(), lstCubicles.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

            if(!lstCubicles.isEmpty())
            {
                SetProjectObject(lstCubicles.first());
                return;
            }
        }
        else if(ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectObject))
        {
            QList<ProjectExplorer::PeDevice*> lstDevices;
            foreach(ProjectExplorer::PeDevice *pDevice, pCubicle->GetChildDevices())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    lstDevices.append(pDevice);
            }
            qSort(lstDevices.begin(), lstDevices.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

            if(!lstDevices.isEmpty())
            {
                pDeviceSelected = lstDevices.first();
                m_pBay = pDeviceSelected->GetParentBay();
            }
        }
        else if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
            {
                pDeviceSelected = pDevice;
                m_pBay = pDeviceSelected->GetParentBay();
            }
        }
        else if(ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pProjectObject))
        {
            if(pDeviceSelected = pBoard->GetParentDevice())
                m_pBay = pDeviceSelected->GetParentBay();
        }
    }
    else
    {
        m_pBay = 0;
        pDeviceSelected = 0;
    }

    if(m_pBay)
    {
        m_pLabelDisplayIcon->setPixmap(m_pBay->GetDisplayIcon().pixmap(QSize(24, 24)));
        m_pLabelDisplayName->setText("<font><B>" + m_pBay->GetDisplayName() + "</B></font>");
    }
    else
    {
        m_pLabelDisplayIcon->clear();
        m_pLabelDisplayName->clear();
    }

    m_pWidgetInfoSet->SetBay(m_pBay, pDeviceSelected);
    if(m_pBay)
        m_pConfigWidgetInfoSet->BuildModel(m_pBay);
    else
        m_pConfigWidgetInfoSet->CleanModel();

    m_pStyleBar->setVisible(pProjectObject);
    m_pSplitter->setVisible(pProjectObject);
}
