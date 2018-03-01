#include <QVBoxLayout>
#include <QAction>
#include <QLabel>
#include <QStandardItemModel>
#include <QToolButton>
#include <QTabBar>
#include <QStackedWidget>

#include "utils/readonlyview.h"
#include "styledui/styledbar.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pecable.h"
#include "projectexplorer/pefiber.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/peport.h"

#include "cablewidget.h"

CableWidget::CableWidget(QWidget *pParent) :
    StyledUi::StyledWidget(pParent), m_pCubicle(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(QSize(700, 500));

    SetupOpticalModelView();
    SetupTailModelView();
    SetupJumpModelView();

    m_pTabBar = new QTabBar(this);
    m_pTabBar->setExpanding(false);
    m_pTabBar->addTab(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctOptical));
    m_pTabBar->addTab(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctTail));
    m_pTabBar->addTab(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctJump));

    m_pStackedWidget = new QStackedWidget(this);
    m_pStackedWidget->addWidget(m_pViewOptical);
    m_pStackedWidget->addWidget(m_pViewTail);
    m_pStackedWidget->addWidget(m_pViewJump);

    QVBoxLayout *pVBoxLayoutMain = new QVBoxLayout(this);
    pVBoxLayoutMain->setContentsMargins(0, 0, 0, 0);
    pVBoxLayoutMain->setSpacing(0);
    pVBoxLayoutMain->addWidget(m_pStackedWidget);
    pVBoxLayoutMain->addWidget(m_pTabBar);

    connect(m_pTabBar, SIGNAL(currentChanged(int)),
            m_pStackedWidget, SLOT(setCurrentIndex(int)));

    m_pTabBar->setVisible(false);
    m_pStackedWidget->setVisible(false);
}

CableWidget::~CableWidget()
{
}

ProjectExplorer::PeCubicle* CableWidget::GetCubicle() const
{
    return m_pCubicle;
}

void CableWidget::SetupOpticalModelView()
{
    m_pViewOptical = new Utils::ReadOnlyTreeView(this);
    m_pViewOptical->setFrameStyle(QFrame::NoFrame);

    m_pModelOptical = new QStandardItemModel(0, 7, this);
    m_pModelOptical->setHeaderData(0, Qt::Horizontal, ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctOptical));
    m_pModelOptical->setHeaderData(1, Qt::Horizontal, tr("Pipe Color"));
    m_pModelOptical->setHeaderData(2, Qt::Horizontal, tr("Fiber Color"));
    m_pModelOptical->setHeaderData(3, Qt::Horizontal, tr("ODF Port"));
    m_pModelOptical->setHeaderData(4, Qt::Horizontal, ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctJump));
    m_pModelOptical->setHeaderData(5, Qt::Horizontal, tr("Current Port"));
    m_pModelOptical->setHeaderData(6, Qt::Horizontal, tr("Side Port"));
    m_pViewOptical->setModel(m_pModelOptical);    

    m_pViewOptical->setColumnWidth(0, 200);
    m_pViewOptical->setColumnWidth(1, 80);
    m_pViewOptical->setColumnWidth(2, 80);
    m_pViewOptical->setColumnWidth(3, 80);
    m_pViewOptical->setColumnWidth(4, 80);
    m_pViewOptical->setColumnWidth(5, 290);
    m_pViewOptical->setColumnWidth(6, 290);
}

void CableWidget::SetupTailModelView()
{
    m_pViewTail = new Utils::ReadOnlyTreeView(this);
    m_pViewTail->setFrameStyle(QFrame::NoFrame);

    m_pModelTail = new QStandardItemModel(0, 5, this);
    m_pModelTail->setHeaderData(0, Qt::Horizontal, ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctTail));
    m_pModelTail->setHeaderData(1, Qt::Horizontal, tr("Current Port"));
    m_pModelTail->setHeaderData(2, Qt::Horizontal, tr("Current Fiber Plug"));
    m_pModelTail->setHeaderData(3, Qt::Horizontal, tr("Side Port"));
    m_pModelTail->setHeaderData(4, Qt::Horizontal, tr("Side Fiber Plug"));
    m_pViewTail->setModel(m_pModelTail);

    m_pViewTail->setColumnWidth(0, 200);
    m_pViewTail->setColumnWidth(1, 290);
    m_pViewTail->setColumnWidth(2, 120);
    m_pViewTail->setColumnWidth(3, 290);
    m_pViewTail->setColumnWidth(4, 120);
}

void CableWidget::SetupJumpModelView()
{
    m_pViewJump = new Utils::ReadOnlyTreeView(this);
    m_pViewJump->setFrameStyle(QFrame::NoFrame);

    m_pModelJump = new QStandardItemModel(0, 5, this);
    m_pModelJump->setHeaderData(0, Qt::Horizontal, ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctJump));
    m_pModelJump->setHeaderData(1, Qt::Horizontal, tr("Current Port"));
    m_pModelJump->setHeaderData(2, Qt::Horizontal, tr("Current Fiber Plug"));
    m_pModelJump->setHeaderData(3, Qt::Horizontal, tr("Side Port"));
    m_pModelJump->setHeaderData(4, Qt::Horizontal, tr("Side Fiber Plug"));
    m_pViewJump->setModel(m_pModelJump);

    m_pViewJump->setColumnWidth(0, 200);
    m_pViewJump->setColumnWidth(1, 290);
    m_pViewJump->setColumnWidth(2, 120);
    m_pViewJump->setColumnWidth(3, 290);
    m_pViewJump->setColumnWidth(4, 120);
}

void CableWidget::BuildOpticalModel(ProjectExplorer::PeCubicle *pCubicle)
{
    m_pModelOptical->removeRows(0, m_pModelOptical->rowCount());

    if(!pCubicle)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pCubicle->GetProjectVersion();
    if(!pProjectVersion)
        return;

    QList<ProjectExplorer::PeCable*> lstOpticalCables, lstJumpCables;
    foreach(ProjectExplorer::PeCable *pCable, pProjectVersion->GetAllCables())
    {
        if(pCable->GetCubicle1() == pCubicle || pCable->GetCubicle2() == pCubicle)
        {
            if(pCable->GetCableType() == ProjectExplorer::PeCable::ctOptical)
                lstOpticalCables.append(pCable);
            else if(pCable->GetCableType() == ProjectExplorer::PeCable::ctJump)
                lstJumpCables.append(pCable);
        }
    }
    qSort(lstOpticalCables.begin(), lstOpticalCables.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    foreach(ProjectExplorer::PeCable *pCable, lstOpticalCables)
    {
        ProjectExplorer::PeCubicle *pCubicleSide = (pCable->GetCubicle1() == pCubicle ? pCable->GetCubicle2() : pCable->GetCubicle1());
        if(!pCubicleSide)
            continue;

        QList<QStandardItem*> lstOpticalItems;

        QStandardItem *pItemCable = new QStandardItem(pCable->GetDisplayIcon(), pCable->GetDisplayName() + " : " + pCubicleSide->GetDisplayName());
        lstOpticalItems.append(pItemCable);

        for(int i = 0; i < m_pModelOptical->columnCount() - 1; i++)
        {
            QStandardItem *pItem = new QStandardItem("");
            lstOpticalItems.append(pItem);
        }
        m_pModelOptical->appendRow(lstOpticalItems);

        foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
        {
            QList<QStandardItem*> lstFiberItems;

            // Name
            QStandardItem *pItemName = new QStandardItem(pFiber->GetDisplayIcon(), pFiber->GetDisplayName());
            lstFiberItems.append(pItemName);

            // Pipe Color
            QStandardItem *pItemPipeColor = new QStandardItem(ProjectExplorer::PeFiber::GetFiberColorName(pFiber->GetPipeColor()));
            pItemPipeColor->setForeground(QBrush(ProjectExplorer::PeFiber::GetFiberColor(pFiber->GetPipeColor())));
            lstFiberItems.append(pItemPipeColor);

            // Fiber Color
            QStandardItem *pItemFiberColor = new QStandardItem(ProjectExplorer::PeFiber::GetFiberColorName(pFiber->GetFiberColor()));
            pItemFiberColor->setForeground(QBrush(ProjectExplorer::PeFiber::GetFiberColor(pFiber->GetFiberColor())));
            lstFiberItems.append(pItemFiberColor);

            QString strOdfPort, strJumpFiber, strCurrentPort;
            if(ProjectExplorer::PePort *pPortOdf = (pCable->GetCubicle1() == pCubicle ? pFiber->GetPort1() : pFiber->GetPort2()))
            {
                ProjectExplorer::PeBoard *pLayer = pPortOdf->GetParentBoard();
                ProjectExplorer::PeDevice *pOdf = (pLayer ? pLayer->GetParentDevice() : 0);

                if(pOdf)
                    strOdfPort = QString("%1: %2").arg(pOdf->GetName()).arg(pPortOdf->GetName());

                foreach(ProjectExplorer::PeCable *pJumpCable, lstJumpCables)
                {
                    if(ProjectExplorer::PeFiber *pFiber = pJumpCable->GetChildFibers().first())
                    {
                        if(pFiber->GetPort1() == pPortOdf)
                        {
                            strJumpFiber = pJumpCable->GetDisplayName();

                            ProjectExplorer::PePort *pPortDevice = pFiber->GetPort2();
                            ProjectExplorer::PeBoard *pBoardDevice = pPortDevice->GetParentBoard();
                            ProjectExplorer::PeDevice *pDevice = (pBoardDevice ? pBoardDevice->GetParentDevice() : 0);
                            if(pDevice)
                            {
                                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                                    strCurrentPort = QString("%1 <%2/%3>").arg(pDevice->GetDisplayName()).arg(pBoardDevice->GetDisplayName()).arg(pPortDevice->GetDisplayName());
                                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                                    strCurrentPort = QString("%1 <%2>").arg(pDevice->GetDisplayName()).arg(pPortDevice->GetDisplayName());
                            }
                        }
                        else if(pFiber->GetPort2() == pPortOdf)
                        {
                            strJumpFiber = pJumpCable->GetDisplayName();

                            ProjectExplorer::PePort *pPortDevice = pFiber->GetPort1();
                            ProjectExplorer::PeBoard *pBoardDevice = pPortDevice->GetParentBoard();
                            ProjectExplorer::PeDevice *pDevice = (pBoardDevice ? pBoardDevice->GetParentDevice() : 0);
                            if(pDevice)
                            {
                                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                                    strCurrentPort = QString("%1 <%2/%3>").arg(pDevice->GetDisplayName()).arg(pBoardDevice->GetDisplayName()).arg(pPortDevice->GetDisplayName());
                                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                                    strCurrentPort = QString("%1 <%2>").arg(pDevice->GetDisplayName()).arg(pPortDevice->GetDisplayName());
                            }
                        }
                    }
                }
            }
            // ODF Port
            QStandardItem *pItemOdfPort = new QStandardItem(strOdfPort);
            lstFiberItems.append(pItemOdfPort);

            // Jump Fiber
            QStandardItem *pItemJumpFiber = new QStandardItem(strJumpFiber);
            lstFiberItems.append(pItemJumpFiber);

            // Current Port
            QStandardItem *pItemCurrentPort = new QStandardItem(strCurrentPort);
            lstFiberItems.append(pItemCurrentPort);

            // Side Port
            QString strSidePort;
            if(ProjectExplorer::PePort *pPortSideOdf = (pCable->GetCubicle1() == pCubicleSide ? pFiber->GetPort1() : pFiber->GetPort2()))
            {
                ProjectExplorer::PeBoard *pLayer = pPortSideOdf->GetParentBoard();
                ProjectExplorer::PeDevice *pOdf = (pLayer ? pLayer->GetParentDevice() : 0);

                if(pOdf)
                    strSidePort = QString("%1: %2").arg(pOdf->GetName()).arg(pPortSideOdf->GetName());

                foreach(ProjectExplorer::PeCable *pJumpCable, pProjectVersion->GetAllCables())
                {
                    if(pJumpCable->GetCubicle1() == pCubicleSide && pJumpCable->GetCubicle2() == pCubicleSide)
                    {
                        if(pJumpCable->GetCableType() == ProjectExplorer::PeCable::ctJump)
                        {
                            if(ProjectExplorer::PeFiber *pFiber = pJumpCable->GetChildFibers().first())
                            {
                                if(pFiber->GetPort1() == pPortSideOdf)
                                {
                                    ProjectExplorer::PePort *pPortDevice = pFiber->GetPort2();
                                    ProjectExplorer::PeBoard *pBoardDevice = pPortDevice->GetParentBoard();
                                    ProjectExplorer::PeDevice *pDevice = (pBoardDevice ? pBoardDevice->GetParentDevice() : 0);
                                    if(pDevice)
                                    {
                                        if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                                            strCurrentPort = QString("%1 <%2/%3>").arg(pDevice->GetDisplayName()).arg(pBoardDevice->GetDisplayName()).arg(pPortDevice->GetDisplayName());
                                        else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                                            strCurrentPort = QString("%1 <%2>").arg(pDevice->GetDisplayName()).arg(pPortDevice->GetDisplayName());
                                    }
                                }
                                else if(pFiber->GetPort2() == pPortSideOdf)
                                {
                                    ProjectExplorer::PePort *pPortDevice = pFiber->GetPort1();
                                    ProjectExplorer::PeBoard *pBoardDevice = pPortDevice->GetParentBoard();
                                    ProjectExplorer::PeDevice *pDevice = (pBoardDevice ? pBoardDevice->GetParentDevice() : 0);
                                    if(pDevice)
                                    {
                                        if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                                            strCurrentPort = QString("%1 <%2/%3>").arg(pDevice->GetDisplayName()).arg(pBoardDevice->GetDisplayName()).arg(pPortDevice->GetDisplayName());
                                        else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                                            strCurrentPort = QString("%1 <%2>").arg(pDevice->GetDisplayName()).arg(pPortDevice->GetDisplayName());
                                    }
                                }
                            }
                        }
                    }
                }
            }

            QStandardItem *pItemSidePort = new QStandardItem(strSidePort);
            lstFiberItems.append(pItemSidePort);

            pItemCable->appendRow(lstFiberItems);
        }
    }

    m_pViewOptical->expandAll();
}

void CableWidget::BuildTailModel(ProjectExplorer::PeCubicle *pCubicle)
{
    m_pModelTail->removeRows(0, m_pModelTail->rowCount());

    if(!pCubicle)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pCubicle->GetProjectVersion();
    if(!pProjectVersion)
        return;

    QList<ProjectExplorer::PeCable*> lstTailCables;
    foreach(ProjectExplorer::PeCable *pCable, pProjectVersion->GetAllCables())
    {
        if(pCable->GetCubicle1() == pCubicle || pCable->GetCubicle2() == pCubicle)
        {
            if(pCable->GetCableType() == ProjectExplorer::PeCable::ctTail)
                lstTailCables.append(pCable);
        }
    }
    qSort(lstTailCables.begin(), lstTailCables.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    foreach(ProjectExplorer::PeCable *pCable, lstTailCables)
    {
        ProjectExplorer::PeCubicle *pCubicleSide = (pCable->GetCubicle1() == pCubicle ? pCable->GetCubicle2() : pCable->GetCubicle1());
        if(!pCubicleSide)
            continue;

        QList<QStandardItem*> lstTailItems;

        QStandardItem *pItemCable = new QStandardItem(pCable->GetDisplayIcon(), pCable->GetDisplayName() + " : " + pCubicleSide->GetDisplayName());
        lstTailItems.append(pItemCable);

        for(int i = 0; i < m_pModelTail->columnCount() - 1; i++)
        {
            QStandardItem *pItem = new QStandardItem("");
            lstTailItems.append(pItem);
        }
        m_pModelTail->appendRow(lstTailItems);

        foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
        {
            QList<QStandardItem*> lstFiberItems;

            // Name
            QStandardItem *pItemName = new QStandardItem(pFiber->GetDisplayIcon(), pFiber->GetDisplayName());
            lstFiberItems.append(pItemName);

            ProjectExplorer::PePort *pPortCurrent, *pPortSide;
            if(pCable->GetCubicle1() == pCubicle)
            {
                pPortCurrent = pFiber->GetPort1();
                pPortSide = pFiber->GetPort2();
            }
            else
            {
                pPortCurrent = pFiber->GetPort2();
                pPortSide = pFiber->GetPort1();
            }

            QString strCurrentPort, strCurrentFiberPlug;
            if(pPortCurrent)
            {
                ProjectExplorer::PeBoard *pBoardCurrent = pPortCurrent->GetParentBoard();
                ProjectExplorer::PeDevice *pDeviceCurrent = (pBoardCurrent ? pBoardCurrent->GetParentDevice() : 0);

                if(pDeviceCurrent)
                {
                    if(pDeviceCurrent->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                        strCurrentPort = QString("%1 <%2/%3>").arg(pDeviceCurrent->GetDisplayName()).arg(pBoardCurrent->GetDisplayName()).arg(pPortCurrent->GetDisplayName());
                    else if(pDeviceCurrent->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                        strCurrentPort = QString("%1 <%2>").arg(pDeviceCurrent->GetDisplayName()).arg(pPortCurrent->GetDisplayName());
                    else if(pDeviceCurrent->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                        strCurrentPort = QString("%1: %2").arg(pDeviceCurrent->GetDisplayName()).arg(pPortCurrent->GetDisplayName());
                }

                strCurrentFiberPlug = ProjectExplorer::PePort::GetFiberPlugName(pPortCurrent->GetFiberPlug());
            }

            // Current Port
            QStandardItem *pItemCurrentPort = new QStandardItem(strCurrentPort);
            lstFiberItems.append(pItemCurrentPort);

            // Current Fiber Plug
            QStandardItem *pItemCurrentFiberPlug = new QStandardItem(strCurrentFiberPlug);
            lstFiberItems.append(pItemCurrentFiberPlug);

            QString strSidePort, strSideFiberPlug;
            if(pPortSide)
            {
                ProjectExplorer::PeBoard *pBoardSide = pPortSide->GetParentBoard();
                ProjectExplorer::PeDevice *pDeviceSide = (pBoardSide ? pBoardSide->GetParentDevice() : 0);

                if(pDeviceSide)
                {
                    if(pDeviceSide->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                        strSidePort = QString("%1 <%2/%3>").arg(pDeviceSide->GetDisplayName()).arg(pBoardSide->GetDisplayName()).arg(pPortSide->GetDisplayName());
                    else if(pDeviceSide->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                        strSidePort = QString("%1 <%2>").arg(pDeviceSide->GetDisplayName()).arg(pPortSide->GetDisplayName());
                    else if(pDeviceSide->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                        strSidePort = QString("%1: %2").arg(pDeviceSide->GetDisplayName()).arg(pPortSide->GetDisplayName());
                }

                strSideFiberPlug = ProjectExplorer::PePort::GetFiberPlugName(pPortSide->GetFiberPlug());
            }

            // Side Port
            QStandardItem *pItemSidePort = new QStandardItem(strSidePort);
            lstFiberItems.append(pItemSidePort);

            // Side Fiber Plug
            QStandardItem *pItemSideFiberPlug = new QStandardItem(strSideFiberPlug);
            lstFiberItems.append(pItemSideFiberPlug);

            pItemCable->appendRow(lstFiberItems);
        }
    }

    m_pViewTail->expandAll();
}

void CableWidget::BuildJumpModel(ProjectExplorer::PeCubicle *pCubicle)
{
    m_pModelJump->removeRows(0, m_pModelJump->rowCount());

    if(!pCubicle)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pCubicle->GetProjectVersion();
    if(!pProjectVersion)
        return;

    QList<ProjectExplorer::PeCable*> lstJumpCables;
    foreach(ProjectExplorer::PeCable *pCable, pProjectVersion->GetAllCables())
    {
        if(pCable->GetCubicle1() == pCubicle || pCable->GetCubicle2() == pCubicle)
        {
            if(pCable->GetCableType() == ProjectExplorer::PeCable::ctJump)
                lstJumpCables.append(pCable);
        }
    }
    qSort(lstJumpCables.begin(), lstJumpCables.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    foreach(ProjectExplorer::PeCable *pCable, lstJumpCables)
    {
        ProjectExplorer::PeFiber *pFiber = pCable->GetChildFibers().first();
        if(!pFiber)
            continue;

        ProjectExplorer::PePort *pPort1 = pFiber->GetPort1();
        if(!pPort1)
            continue;

        ProjectExplorer::PeBoard *pBoard1 = pPort1->GetParentBoard();
        if(!pBoard1)
            continue;

        ProjectExplorer::PeDevice *pDevice1 = pBoard1->GetParentDevice();
        if(!pDevice1 || pDevice1->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
            continue;

        ProjectExplorer::PePort *pPort2 = pFiber->GetPort2();
        if(!pPort2)
            continue;

        ProjectExplorer::PeBoard *pBoard2 = pPort2->GetParentBoard();
        if(!pBoard2)
            continue;

        ProjectExplorer::PeDevice *pDevice2 = pBoard2->GetParentDevice();
        if(!pDevice2 || pDevice2->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
            continue;

        QList<QStandardItem*> lstJumpItems;

        QStandardItem *pItemName = new QStandardItem(pCable->GetDisplayIcon(), pCable->GetDisplayName());
        lstJumpItems.append(pItemName);

        // Current Port
        QString strPort1;
        if(pDevice1->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
            strPort1 = QString("%1 <%2/%3>").arg(pDevice1->GetDisplayName()).arg(pBoard1->GetDisplayName()).arg(pPort1->GetDisplayName());
        else if(pDevice1->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
            strPort1 = QString("%1 <%2>").arg(pDevice1->GetDisplayName()).arg(pPort1->GetDisplayName());
        QStandardItem *pItemCurrentPort = new QStandardItem(strPort1);
        lstJumpItems.append(pItemCurrentPort);

        // Current Fiber Plug
        QStandardItem *pItemCurrentFiberPlug = new QStandardItem(ProjectExplorer::PePort::GetFiberPlugName(pPort1->GetFiberPlug()));
        lstJumpItems.append(pItemCurrentFiberPlug);

        // Side Port
        QString strPort2;
        if(pDevice2->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
            strPort2 = QString("%1 <%2/%3>").arg(pDevice2->GetDisplayName()).arg(pBoard2->GetDisplayName()).arg(pPort2->GetDisplayName());
        else if(pDevice2->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
            strPort2 = QString("%1 <%2>").arg(pDevice2->GetDisplayName()).arg(pPort2->GetDisplayName());
        QStandardItem *pItemSidePort = new QStandardItem(strPort2);
        lstJumpItems.append(pItemSidePort);

        // Side Fiber Plug
        QStandardItem *pItemSideFiberPlug = new QStandardItem(ProjectExplorer::PePort::GetFiberPlugName(pPort2->GetFiberPlug()));
        lstJumpItems.append(pItemSideFiberPlug);

        m_pModelJump->appendRow(lstJumpItems);
    }

    m_pViewJump->expandAll();
}

void CableWidget::SlotSetCubicle(ProjectExplorer::PeCubicle *pCubicle)
{
    if(m_pCubicle == pCubicle)
        return;

    m_pCubicle = pCubicle;

    BuildOpticalModel(m_pCubicle);
    BuildTailModel(m_pCubicle);
    BuildJumpModel(m_pCubicle);

    m_pTabBar->setVisible(m_pCubicle);
    m_pStackedWidget->setVisible(m_pCubicle);
}
