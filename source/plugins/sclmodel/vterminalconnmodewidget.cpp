#include <QVBoxLayout>
#include <QSplitter>

#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"

#include "vterminalconnmodewidget.h"
#include "widgetvterminalconn.h"
#include "configwidgetvterminalconn.h"

using namespace SclModel::Internal;

VTerminalConnModeWidget::VTerminalConnModeWidget(QWidget *pParent)
    : StyledUi::StyledWidget(pParent), m_pProjectVersion(0)
{
    m_pConfigWidgetVTerminalConn = new ConfigWidgetVTerminalConn(this);
    m_pConfigWidgetVTerminalConn->SetupConfigWidget();

    m_pWidgetVTerminalConn = new WidgetVTerminalConn(this);
    connect(m_pWidgetVTerminalConn, SIGNAL(sigCurrentDeviceChanged(ProjectExplorer::PeDevice*)),
            m_pConfigWidgetVTerminalConn, SLOT(SlotSetCurrentDevice(ProjectExplorer::PeDevice*)));
    connect(m_pWidgetVTerminalConn, SIGNAL(sigSideDeviceChanged(ProjectExplorer::PeDevice*)),
            m_pConfigWidgetVTerminalConn, SLOT(SlotSetSideDevice(ProjectExplorer::PeDevice*)));
    connect(m_pWidgetVTerminalConn, SIGNAL(sigViewConnection(ProjectExplorer::PeVTerminalConn*)),
            m_pConfigWidgetVTerminalConn, SLOT(SlotActivateVTerminalConn(ProjectExplorer::PeVTerminalConn*)));

    m_pSplitter = new QSplitter(this);
    m_pSplitter->setOrientation(Qt::Vertical);
    m_pSplitter->addWidget(m_pWidgetVTerminalConn);
    m_pSplitter->addWidget(m_pConfigWidgetVTerminalConn);

    QVBoxLayout *pVBoxLayoutMain = new QVBoxLayout(this);
    pVBoxLayoutMain->setContentsMargins(0, 0, 0, 0);
    pVBoxLayoutMain->addWidget(m_pSplitter);

    connect(m_pConfigWidgetVTerminalConn, SIGNAL(sigCurrentVTerminalConnChanged(ProjectExplorer::PeVTerminalConn*)),
            m_pWidgetVTerminalConn, SLOT(SlotSetVTerminalConn(ProjectExplorer::PeVTerminalConn*)));
}

VTerminalConnModeWidget::~VTerminalConnModeWidget()
{
}

void VTerminalConnModeWidget::SetProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    ProjectExplorer::PeDevice *pDeviceSelected = 0;

    if(pProjectObject)
    {
        m_pProjectVersion = pProjectObject->GetProjectVersion();

        if(ProjectExplorer::PeBay *pBay = qobject_cast<ProjectExplorer::PeBay*>(pProjectObject))
        {
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
                pDeviceSelected = lstDevices.first();
        }
        else if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                pDeviceSelected = pDevice;
        }
        else if(ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pProjectObject))
        {
            pDeviceSelected = pBoard->GetParentDevice();
        }
    }
    else
    {
        m_pProjectVersion = 0;
        pDeviceSelected = 0;
    }

    m_pWidgetVTerminalConn->SetProjectVersion(m_pProjectVersion, pDeviceSelected);
    if(!m_pProjectVersion)
        m_pConfigWidgetVTerminalConn->CleanModel();

     m_pSplitter->setVisible(pProjectObject);
}
