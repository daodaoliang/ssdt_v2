#include <QVBoxLayout>
#include <QAction>
#include <QLabel>
#include <QStandardItemModel>
#include <QToolButton>
#include <QTabBar>
#include <QStackedWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QMimeData>
#include <QDragMoveEvent>
#include <QPainter>

#include "styledui/styledbar.h"
#include "utils/waitcursor.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pecable.h"
#include "projectexplorer/pefiber.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/peport.h"
#include "core/core_constants.h"
#include "core/actionmanager.h"
#include "core/actioncontainer.h"
#include "core/command.h"
#include "core/mainwindow.h"

#include "cablemodewidget.h"
#include "cablegenerator.h"
#include "propertydlgcable.h"
#include "propertydlgodf.h"
#include "propertydlgodflayer.h"
#include "renamecabledlg.h"
#include "phynetworksettings.h"

using namespace PhyNetwork::Internal;

const QString g_strMimeTypeForCable = "Cable";
const QString g_strMimeTypeForOdf = "Odf";

////////////////////////////////////////////////////////////////////////////////////////
// class StandardItemModelCable
////////////////////////////////////////////////////////////////////////////////////////
StandardItemModelCable::StandardItemModelCable(int rows, int columns, CableModeWidget *pCableModeWidget) : QStandardItemModel(rows, columns, pCableModeWidget), m_pCableModeWidget(pCableModeWidget)
{
}

QMimeData* StandardItemModelCable::mimeData(const QModelIndexList &indexes) const
{
    if(indexes.isEmpty())
        return 0;

    QModelIndex indexCable = indexes.first().parent();
    const int iRow = indexes.first().row();
    QModelIndex indexFiber;
    foreach(const QModelIndex &index, indexes)
    {
        if(index.column() == 0)
            indexFiber = index;

        if(iRow != index.row() || indexCable != index.parent())
            return 0;
    }

    if(!indexCable.isValid() || !indexFiber.isValid())
        return 0;

    ProjectExplorer::PeFiber *pFiber = 0;
    if(QStandardItem *pItemFiber = itemFromIndex(indexFiber))
        pFiber = reinterpret_cast<ProjectExplorer::PeFiber*>(pItemFiber->data().toInt());
    if(!pFiber)
        return 0;

    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    stream << reinterpret_cast<int>(pFiber);
    mimeData->setData(g_strMimeTypeForCable, encodedData);
    return mimeData;
}

QStringList StandardItemModelCable::mimeTypes() const
{
    return QStringList() << g_strMimeTypeForCable;
}

bool StandardItemModelCable::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int /*column*/, const QModelIndex &parent)
{
    if(action == Qt::IgnoreAction)
        return true;

    // Get source fiber
    ProjectExplorer::PeFiber *pFiberSource = 0;
    if(data->hasFormat(g_strMimeTypeForCable))
    {
        QByteArray encodedData = data->data(g_strMimeTypeForCable);
        QDataStream stream(&encodedData, QIODevice::ReadOnly);

        int iFiber = 0;
        stream >> iFiber;
        pFiberSource = reinterpret_cast<ProjectExplorer::PeFiber*>(iFiber);
    }
    if(!pFiberSource)
        return true;

    ProjectExplorer::PeCable *pCableSource = pFiberSource->GetParentCable();
    if(!pCableSource)
        return true;

    if(!parent.isValid())
        return true;
    QModelIndex indexParent = parent.sibling(parent.row(), 0);

    QStandardItem *pItemParent = itemFromIndex(indexParent);
    if(!pItemParent)
        return true;

    if(pItemParent->parent()) // Fiber item
    {
        ProjectExplorer::PeFiber *pFiberTarget = reinterpret_cast<ProjectExplorer::PeFiber*>(pItemParent->data().toInt());
        if(!pFiberTarget)
            return true;

        ProjectExplorer::PeCable *pCableTarget = pFiberTarget->GetParentCable();
        if(!pCableTarget)
            return true;

        if(pCableSource == pCableTarget)
        {
//            QList<ProjectExplorer::PeFiber*> lstSourceFibers = pCableSource->GetChildFibers();
//            qSort(lstSourceFibers.begin(), lstSourceFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

//            int iIndex = row;
//            if(iIndex < 0)
//                iIndex = lstSourceFibers.indexOf(pFiberTarget) + 1;

//            ProjectExplorer::PeFiber tempFiber;
//            lstSourceFibers.insert(iIndex, &tempFiber);
//            lstSourceFibers.removeAll(pFiberSource);

//            Utils::WaitCursor waitcursor;
//            Q_UNUSED(waitcursor)

//            if(CableGenerator::Instance()->AdjustFiberIndex(pFiberSource, lstSourceFibers.indexOf(&tempFiber)))
//                m_pCableModeWidget->Refresh();

            QList<ProjectExplorer::PeFiber*> lstSourceFibers = pCableSource->GetChildFibers();
            qSort(lstSourceFibers.begin(), lstSourceFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

            int iIndex = row - 1;
            if(iIndex < 0)
                iIndex = lstSourceFibers.indexOf(pFiberTarget);

            Utils::WaitCursor waitcursor;
            Q_UNUSED(waitcursor)

            int iCurrentIndex = lstSourceFibers.indexOf(pFiberSource);
            if(iCurrentIndex < iIndex)
            {
                while(iCurrentIndex < iIndex)
                {
                    CableGenerator::Instance()->ExchangeFiber(lstSourceFibers.at(iCurrentIndex), lstSourceFibers.at(iCurrentIndex + 1));
                    iCurrentIndex++;
                }
            }
            else
            {
                while(iCurrentIndex > iIndex)
                {
                    CableGenerator::Instance()->ExchangeFiber(lstSourceFibers.at(iCurrentIndex), lstSourceFibers.at(iCurrentIndex - 1));
                    iCurrentIndex--;
                }
            }

            m_pCableModeWidget->Refresh();
        }
        else
        {
            if(pCableSource->GetNameSet() != pCableTarget->GetNameSet())
            {
                if(QMessageBox::question(m_pCableModeWidget,
                                         tr("Adjust Confirmation"),
                                         tr("The source %1 '%2' and the target %1 '%3' have different set, are you sure you want to adjust %4?").arg(pCableSource->GetCableTypeName(pCableSource->GetCableType())).arg(pCableSource->GetDisplayName()).arg(pCableTarget->GetDisplayName()).arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otFiber)),
                                         QMessageBox::Yes | QMessageBox::No,
                                         QMessageBox::No) != QMessageBox::Yes)
                {
                    return true;
                }
            }

            if(!((pCableSource->GetCubicle1() == pCableTarget->GetCubicle1() && pCableSource->GetCubicle2() == pCableTarget->GetCubicle2()) ||
                 (pCableSource->GetCubicle1() == pCableTarget->GetCubicle2() && pCableSource->GetCubicle2() == pCableTarget->GetCubicle1())))
            {
                return true;
            }

            Utils::WaitCursor waitcursor;
            Q_UNUSED(waitcursor)

            if(CableGenerator::Instance()->ExchangeFiber(pFiberSource, pFiberTarget))
                m_pCableModeWidget->Refresh();
        }
    }
    else if(row >= 0) // Cable item
    {
//        ProjectExplorer::PeCable *pCableTarget = reinterpret_cast<ProjectExplorer::PeCable*>(pItemParent->data().toInt());
//        if(!pCableTarget)
//            return true;

//        if(pCableSource != pCableTarget)
//            return true;

//        QList<ProjectExplorer::PeFiber*> lstSourceFibers = pCableSource->GetChildFibers();
//        qSort(lstSourceFibers.begin(), lstSourceFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

//        ProjectExplorer::PeFiber tempFiber;
//        lstSourceFibers.insert(row, &tempFiber);
//        lstSourceFibers.removeAll(pFiberSource);

//        Utils::WaitCursor waitcursor;
//        Q_UNUSED(waitcursor)

//        if(CableGenerator::Instance()->AdjustFiberIndex(pFiberSource, lstSourceFibers.indexOf(&tempFiber)))
//            m_pCableModeWidget->Refresh();

        ProjectExplorer::PeCable *pCableTarget = reinterpret_cast<ProjectExplorer::PeCable*>(pItemParent->data().toInt());
        if(!pCableTarget)
            return true;

        if(pCableSource != pCableTarget)
            return true;

        QList<ProjectExplorer::PeFiber*> lstSourceFibers = pCableSource->GetChildFibers();
        qSort(lstSourceFibers.begin(), lstSourceFibers.end(), ProjectExplorer::PeFiber::CompareIndex);

        int iIndex = row;
        int iCurrentIndex = lstSourceFibers.indexOf(pFiberSource);
        if(iCurrentIndex < iIndex)
        {
            while(iCurrentIndex < iIndex)
            {
                CableGenerator::Instance()->ExchangeFiber(lstSourceFibers.at(iCurrentIndex), lstSourceFibers.at(iCurrentIndex + 1));
                iCurrentIndex++;
            }
        }
        else
        {
            while(iCurrentIndex > iIndex)
            {
                CableGenerator::Instance()->ExchangeFiber(lstSourceFibers.at(iCurrentIndex), lstSourceFibers.at(iCurrentIndex - 1));
                iCurrentIndex--;
            }
        }

        m_pCableModeWidget->Refresh();
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////
// class TreeViewCable
////////////////////////////////////////////////////////////////////////////////////////
TreeViewCable::TreeViewCable(QWidget *pParent) : Utils::ReadOnlyTreeView(pParent)
{
}

void TreeViewCable::dragMoveEvent(QDragMoveEvent * event)
{
    Utils::ReadOnlyTreeView::dragMoveEvent(event);
    event->ignore();

    m_DropIndicatorRect = QRect();

    // Get source fiber
    ProjectExplorer::PeFiber *pFiberSource = 0;
    if(event->mimeData()->hasFormat(g_strMimeTypeForCable))
    {
        QByteArray encodedData = event->mimeData()->data(g_strMimeTypeForCable);
        QDataStream stream(&encodedData, QIODevice::ReadOnly);

        int iFiber = 0;
        stream >> iFiber;
        pFiberSource = reinterpret_cast<ProjectExplorer::PeFiber*>(iFiber);
    }
    if(!pFiberSource)
        return;

    ProjectExplorer::PeCable *pCableSource = pFiberSource->GetParentCable();
    if(!pCableSource)
        return;

    // Get target fiber
    QModelIndex indexTarget = indexAt(event->pos());
    if(!indexTarget.isValid())
        return;
    indexTarget = indexTarget.sibling(indexTarget.row(), 0);

    QStandardItemModel *pModel = qobject_cast<QStandardItemModel*>(model());
    if(!pModel)
        return;
    QStandardItem *pItem = pModel->itemFromIndex(indexTarget);
    if(!pItem || !pItem->parent())
        return;
    ProjectExplorer::PeFiber *pFiberTarget = reinterpret_cast<ProjectExplorer::PeFiber*>(pItem->data().toInt());
    if(!pFiberTarget)
        return;

    ProjectExplorer::PeCable *pCableTarget = pFiberTarget->GetParentCable();
    if(!pCableTarget)
        return;

    //if(pCableSource->GetNameSet() != pCableTarget->GetNameSet())
    //  return;

    if(!((pCableSource->GetCubicle1() == pCableTarget->GetCubicle1() && pCableSource->GetCubicle2() == pCableTarget->GetCubicle2()) ||
         (pCableSource->GetCubicle1() == pCableTarget->GetCubicle2() && pCableSource->GetCubicle2() == pCableTarget->GetCubicle1())))
    {
        return;
    }

    if(pFiberSource->GetParentCable() == pFiberTarget->GetParentCable())
    {
        switch(dropIndicatorPosition())
        {
            case QAbstractItemView::OnItem:
            case QAbstractItemView::BelowItem:
            {
                QRect rect;
                int iColumnCount = pModel->columnCount();
                for(int i = 0; i < iColumnCount; i++)
                {
                    QModelIndex s = indexTarget.sibling(indexTarget.row(), i);
                    if(s.isValid())
                        rect |= visualRect(s);
                }
                m_DropIndicatorRect = QRect(rect.left(), rect.bottom(), rect.width(), 0);
                event->accept();
            }
            break;

            case QAbstractItemView::AboveItem:
            {
                QRect rect;
                int iColumnCount = pModel->columnCount();
                for(int i = 0; i < iColumnCount; i++)
                {
                    QModelIndex s = indexTarget.sibling(indexTarget.row(), i);
                    if(s.isValid())
                        rect |= visualRect(s);
                }
                m_DropIndicatorRect = QRect(rect.left(), rect.top(), rect.width(), 0);
                event->accept();
            }
            break;
        }
    }
    else
    {
        switch(dropIndicatorPosition())
        {
            case QAbstractItemView::OnItem:
            {
                QRect rect;
                int iColumnCount = pModel->columnCount();
                for(int i = 0; i < iColumnCount; i++)
                {
                    QModelIndex s = indexTarget.sibling(indexTarget.row(), i);
                    if(s.isValid())
                        rect |= visualRect(s);
                }
                m_DropIndicatorRect = rect;
                event->accept();
            }
            break;
        }
    }
}

void TreeViewCable::paintEvent (QPaintEvent * event)
{
    setDropIndicatorShown(false);
    Utils::ReadOnlyTreeView::paintEvent(event);
    setDropIndicatorShown(true);

    QPainter painter(viewport());
    if(showDropIndicator() && state() == QAbstractItemView::DraggingState)
    {
        QStyleOption opt;
        opt.init(this);
        opt.rect = m_DropIndicatorRect;
        style()->drawPrimitive(QStyle::PE_IndicatorItemViewItemDrop, &opt, &painter, this);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// class StandardItemModelOdf
////////////////////////////////////////////////////////////////////////////////////////
StandardItemModelOdf::StandardItemModelOdf(int rows, int columns, CableModeWidget *pCableModeWidget) : QStandardItemModel(rows, columns, pCableModeWidget), m_pCableModeWidget(pCableModeWidget)
{
}

QMimeData* StandardItemModelOdf::mimeData(const QModelIndexList &indexes) const
{
    if(indexes.isEmpty())
        return 0;

    QModelIndex indexLayer = indexes.first().parent();
    const int iRow = indexes.first().row();
    QModelIndex indexPort;
    foreach(const QModelIndex &index, indexes)
    {
        if(index.column() == 0)
            indexPort = index;

        if(iRow != index.row() || indexLayer != index.parent())
            return 0;
    }

    if(!indexLayer.isValid() || !indexPort.isValid())
        return 0;

    ProjectExplorer::PePort *pPort = 0;
    if(QStandardItem *pItemPort = itemFromIndex(indexPort))
        pPort = reinterpret_cast<ProjectExplorer::PePort*>(pItemPort->data().toInt());
    if(!pPort)
        return 0;

    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    stream << reinterpret_cast<int>(pPort);
    mimeData->setData(g_strMimeTypeForOdf, encodedData);
    return mimeData;
}

QStringList StandardItemModelOdf::mimeTypes() const
{
    return QStringList() << g_strMimeTypeForOdf;
}

bool StandardItemModelOdf::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int /*column*/, const QModelIndex &parent)
{
    if(action == Qt::IgnoreAction)
        return true;

    // Get source port
    ProjectExplorer::PePort *pPortSource = 0;
    if(data->hasFormat(g_strMimeTypeForOdf))
    {
        QByteArray encodedData = data->data(g_strMimeTypeForOdf);
        QDataStream stream(&encodedData, QIODevice::ReadOnly);

        int iPort = 0;
        stream >> iPort;
        pPortSource = reinterpret_cast<ProjectExplorer::PePort*>(iPort);
    }
    if(!pPortSource)
        return true;

    ProjectExplorer::PeBoard *pLayerSource = pPortSource->GetParentBoard();
    if(!pLayerSource)
        return true;

    if(!parent.isValid())
        return true;
    QModelIndex indexParent = parent.sibling(parent.row(), 0);

    QStandardItem *pItemParent = itemFromIndex(indexParent);
    if(!pItemParent)
        return true;

    if(!pItemParent->hasChildren()) // Port item
    {
        ProjectExplorer::PePort *pPortTarget = reinterpret_cast<ProjectExplorer::PePort*>(pItemParent->data().toInt());
        if(!pPortTarget)
            return true;

        ProjectExplorer::PeBoard *pLayerTarget = pPortTarget->GetParentBoard();
        if(!pLayerTarget)
            return true;

        if(pLayerSource == pLayerTarget)
        {
            QList<ProjectExplorer::PePort*> lstSourcePorts = pLayerSource->GetChildPorts();
            qSort(lstSourcePorts.begin(), lstSourcePorts.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

            int iIndex = row;
            if(iIndex < 0)
                iIndex = lstSourcePorts.indexOf(pPortTarget) + 1;

            ProjectExplorer::PePort tempPort;
            lstSourcePorts.insert(iIndex, &tempPort);
            lstSourcePorts.removeAll(pPortSource);

            Utils::WaitCursor waitcursor;
            Q_UNUSED(waitcursor)

            if(CableGenerator::Instance()->AdjustOdfPortIndex(pPortSource, lstSourcePorts.indexOf(&tempPort)))
                m_pCableModeWidget->Refresh();
        }
        else
        {
            ProjectExplorer::PeDevice *pOdfSource = pPortSource->GetParentDevice();
            ProjectExplorer::PeDevice *pOdfTarget = pPortTarget->GetParentDevice();
            if(pOdfSource->GetSet() != pOdfTarget->GetSet())
            {
                if(QMessageBox::question(m_pCableModeWidget,
                                         tr("Adjust Confirmation"),
                                         tr("The source %1 '%2' and the target %1 '%3' have different set, are you sure you want to adjust %4?").arg(pOdfSource->GetObjectTypeName()).arg(pOdfSource->GetDisplayName()).arg(pOdfTarget->GetDisplayName()).arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otPort)),
                                         QMessageBox::Yes | QMessageBox::No,
                                         QMessageBox::No) != QMessageBox::Yes)
                {
                    return true;
                }
            }

            Utils::WaitCursor waitcursor;
            Q_UNUSED(waitcursor)

            if(CableGenerator::Instance()->ExchangeOdfPort(pPortSource, pPortTarget))
                m_pCableModeWidget->Refresh();
        }
    }
    else if(row >= 0) // Layer item
    {
        ProjectExplorer::PeBoard *pLayerTarget = reinterpret_cast<ProjectExplorer::PeBoard*>(pItemParent->data().toInt());
        if(!pLayerTarget)
            return true;

        if(pLayerSource != pLayerTarget)
            return true;

        QList<ProjectExplorer::PePort*> lstSourcePorts = pLayerSource->GetChildPorts();
        qSort(lstSourcePorts.begin(), lstSourcePorts.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

        ProjectExplorer::PePort tempPort;
        lstSourcePorts.insert(row, &tempPort);
        lstSourcePorts.removeAll(pPortSource);

        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        if(CableGenerator::Instance()->AdjustOdfPortIndex(pPortSource, lstSourcePorts.indexOf(&tempPort)))
            m_pCableModeWidget->Refresh();
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////
// class TreeViewOdf
////////////////////////////////////////////////////////////////////////////////////////
TreeViewOdf::TreeViewOdf(QWidget *pParent) : Utils::ReadOnlyTreeView(pParent)
{
}

void TreeViewOdf::dragMoveEvent(QDragMoveEvent * event)
{
    Utils::ReadOnlyTreeView::dragMoveEvent(event);
    event->ignore();

    m_DropIndicatorRect = QRect();

    // Get source port
    ProjectExplorer::PePort *pPortSource = 0;
    if(event->mimeData()->hasFormat(g_strMimeTypeForOdf))
    {
        QByteArray encodedData = event->mimeData()->data(g_strMimeTypeForOdf);
        QDataStream stream(&encodedData, QIODevice::ReadOnly);

        int iPort = 0;
        stream >> iPort;
        pPortSource = reinterpret_cast<ProjectExplorer::PePort*>(iPort);
    }
    if(!pPortSource)
        return;

    // Get target port
    QModelIndex indexTarget = indexAt(event->pos());
    if(!indexTarget.isValid())
        return;
    indexTarget = indexTarget.sibling(indexTarget.row(), 0);

    QStandardItemModel *pModel = qobject_cast<QStandardItemModel*>(model());
    if(!pModel)
        return;
    QStandardItem *pItem = pModel->itemFromIndex(indexTarget);
    if(!pItem || pItem->hasChildren())
        return;
    ProjectExplorer::PePort *pPortTarget = reinterpret_cast<ProjectExplorer::PePort*>(pItem->data().toInt());
    if(!pPortTarget)
        return;

    //if(pPortSource->GetParentDevice()->GetSet() != pPortTarget->GetParentDevice()->GetSet())
    //  return;

    if(pPortSource->GetParentBoard() == pPortTarget->GetParentBoard())
    {
        switch(dropIndicatorPosition())
        {
            case QAbstractItemView::OnItem:
            case QAbstractItemView::BelowItem:
            {
                QRect rect;
                int iColumnCount = pModel->columnCount();
                for(int i = 0; i < iColumnCount; i++)
                {
                    QModelIndex s = indexTarget.sibling(indexTarget.row(), i);
                    if(s.isValid())
                        rect |= visualRect(s);
                }
                m_DropIndicatorRect = QRect(rect.left(), rect.bottom(), rect.width(), 0);
                event->accept();
            }
            break;

            case QAbstractItemView::AboveItem:
            {
                QRect rect;
                int iColumnCount = pModel->columnCount();
                for(int i = 0; i < iColumnCount; i++)
                {
                    QModelIndex s = indexTarget.sibling(indexTarget.row(), i);
                    if(s.isValid())
                        rect |= visualRect(s);
                }
                m_DropIndicatorRect = QRect(rect.left(), rect.top(), rect.width(), 0);
                event->accept();
            }
            break;
        }
    }
    else
    {
        switch(dropIndicatorPosition())
        {
        case QAbstractItemView::OnItem:
        {
            QRect rect;
            int iColumnCount = pModel->columnCount();
            for(int i = 0; i < iColumnCount; i++)
            {
                QModelIndex s = indexTarget.sibling(indexTarget.row(), i);
                if(s.isValid())
                    rect |= visualRect(s);
            }
            m_DropIndicatorRect = rect;
            event->accept();
        }
            break;
        }
    }
}

void TreeViewOdf::paintEvent (QPaintEvent * event)
{
    setDropIndicatorShown(false);
    Utils::ReadOnlyTreeView::paintEvent(event);
    setDropIndicatorShown(true);

    QPainter painter(viewport());
    if(showDropIndicator() && state() == QAbstractItemView::DraggingState)
    {
        QStyleOption opt;
        opt.init(this);
        opt.rect = m_DropIndicatorRect;
        style()->drawPrimitive(QStyle::PE_IndicatorItemViewItemDrop, &opt, &painter, this);
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// class CableModeWidget
////////////////////////////////////////////////////////////////////////////////////////
CableModeWidget::CableModeWidget(QWidget *pParent) :
    StyledUi::StyledWidget(pParent), m_pCubicle(0)
{
    SetupOpticalModelView();
    SetupTailModelView();
    SetupJumpModelView();
    SetupOdfModelView();

    m_pTabBar = new QTabBar(this);
    m_pTabBar->setShape(QTabBar::RoundedSouth);
    m_pTabBar->setExpanding(false);
    m_pTabBar->addTab(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctOptical));
    m_pTabBar->addTab(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctTail));
    m_pTabBar->addTab(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctJump));
    m_pTabBar->addTab(ProjectExplorer::PeDevice::GetDeviceTypeName(ProjectExplorer::PeDevice::dtODF));

    m_pStackedWidget = new QStackedWidget(this);
    m_pStackedWidget->addWidget(m_pViewOptical);
    m_pStackedWidget->addWidget(m_pViewTail);
    m_pStackedWidget->addWidget(m_pViewJump);
    m_pStackedWidget->addWidget(m_pViewOdf);

    QVBoxLayout *pVBoxLayoutMain = new QVBoxLayout(this);
    pVBoxLayoutMain->setContentsMargins(0, 0, 0, 0);
    pVBoxLayoutMain->setSpacing(0);
    pVBoxLayoutMain->addWidget(SetupToolBar());
    pVBoxLayoutMain->addWidget(m_pStackedWidget);
    pVBoxLayoutMain->addWidget(m_pTabBar);

    connect(m_pTabBar, SIGNAL(currentChanged(int)),
            m_pStackedWidget, SLOT(setCurrentIndex(int)));
    connect(m_pTabBar, SIGNAL(currentChanged(int)),
            this, SLOT(SlotCurrentTabChanged(int)));

    QList<int> lstGlobalContexts = QList<int>() << Core::Constants::g_iUID_Context_Global;

    Core::CActionManager *pActionManager = Core::MainWindow::Instance()->GetActionManager();
    Core::CActionContainer *pActionContainerMenu = pActionManager->GetActionContainer(Core::Constants::g_szUID_Menu);

    QAction *pActionToolsRenameCable = new QAction(tr("&Cable Rename..."), this);
    Core::CCommand *pCommand = pActionManager->RegisterAction(pActionToolsRenameCable, QLatin1String("PhyNetwork.Action.ToolsRenameCable"), lstGlobalContexts);
    pCommand->SetDefaultKeySequence(QKeySequence("Ctrl+R"));
    pActionContainerMenu->AddAction(pCommand, Core::Constants::g_szUID_MenuGroup_Tools);
    connect(pActionToolsRenameCable, SIGNAL(triggered()), this, SLOT(SlotActionRenameCable()));

    SetProjectObject(0);
}

CableModeWidget::~CableModeWidget()
{
}

void CableModeWidget::SetProjectObject(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(pProjectObject)
    {
        if(ProjectExplorer::PeBay *pBay = qobject_cast<ProjectExplorer::PeBay*>(pProjectObject))
        {
            QList<ProjectExplorer::PeDevice*> lstDevices = pBay->GetChildDevices();
            qSort(lstDevices.begin(), lstDevices.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

            if(!lstDevices.isEmpty())
                m_pCubicle = lstDevices.first()->GetParentCubicle();
        }
        else if(ProjectExplorer::PeRoom *pRoom = qobject_cast<ProjectExplorer::PeRoom*>(pProjectObject))
        {
            QList<ProjectExplorer::PeCubicle*> lstCubicles = pRoom->GetChildCubicles();
            qSort(lstCubicles.begin(), lstCubicles.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

            if(!lstCubicles.isEmpty())
                m_pCubicle = lstCubicles.first();
        }
        else if(ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectObject))
        {
            m_pCubicle = pCubicle;
        }
        else if(ProjectExplorer::PeDevice *pDevice = qobject_cast<ProjectExplorer::PeDevice*>(pProjectObject))
        {
            m_pCubicle = pDevice->GetParentCubicle();
        }
        else if(ProjectExplorer::PeBoard *pBoard = qobject_cast<ProjectExplorer::PeBoard*>(pProjectObject))
        {
            m_pCubicle = pBoard->GetParentDevice()->GetParentCubicle();
        }
    }
    else
    {
        m_pCubicle = 0;
    }

    if(m_pCubicle)
    {
        m_pLabelDisplayIcon->setPixmap(m_pCubicle->GetDisplayIcon().pixmap(QSize(24, 24)));
        m_pLabelDisplayName->setText("<font><B>" + m_pCubicle->GetDisplayName() + "</B></font>");

        CableGenerator::Instance()->ReadSettings(m_pCubicle->GetProjectVersion());
    }
    else
    {
        m_pLabelDisplayIcon->clear();
        m_pLabelDisplayName->clear();
    }

    BuildOpticalModel(m_pCubicle);
    BuildTailModel(m_pCubicle);
    BuildJumpModel(m_pCubicle);
    BuildOdfModel(m_pCubicle);

    m_pStyleBar->setVisible(pProjectObject);
    m_pTabBar->setVisible(pProjectObject);
    m_pStackedWidget->setVisible(pProjectObject);

    UpdateActions();
}

void CableModeWidget::Refresh()
{
    SetProjectObject(m_pCubicle);
}

QWidget* CableModeWidget::SetupToolBar()
{
    m_pStyleBar = new StyledUi::StyledBar(this);

    m_pLabelDisplayIcon = new QLabel(m_pStyleBar);
    m_pLabelDisplayName = new QLabel(m_pStyleBar);

    m_pActionSettings = new QAction(QIcon(":/phynetwork/images/oper_settings.png"), tr("Settings"), m_pStyleBar);
    connect(m_pActionSettings, SIGNAL(triggered()), this, SLOT(SlotActionSettings()));
    QToolButton *pToolButtonSettings = new QToolButton(m_pStyleBar);
    pToolButtonSettings->setFocusPolicy(Qt::NoFocus);
    pToolButtonSettings->setAutoRaise(true);
    pToolButtonSettings->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonSettings->setDefaultAction(m_pActionSettings);

    m_pActionRefresh = new QAction(QIcon(":/phynetwork/images/oper_refreshall.png"), tr("Refresh"), m_pStyleBar);
    connect(m_pActionRefresh, SIGNAL(triggered()), this, SLOT(SlotActionRefresh()));
    QToolButton *pToolButtonRefresh = new QToolButton(m_pStyleBar);
    pToolButtonRefresh->setFocusPolicy(Qt::NoFocus);
    pToolButtonRefresh->setAutoRaise(true);
    pToolButtonRefresh->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonRefresh->setDefaultAction(m_pActionRefresh);

    m_pActionClear = new QAction(QIcon(":/phynetwork/images/oper_remove.png"), tr("Clear"), this);
    connect(m_pActionClear, SIGNAL(triggered()), this, SLOT(SlotActionClear()));
    QToolButton *pToolButtonClear = new QToolButton(m_pStyleBar);
    pToolButtonClear->setFocusPolicy(Qt::NoFocus);
    pToolButtonClear->setAutoRaise(true);
    pToolButtonClear->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonClear->setDefaultAction(m_pActionClear);

    m_pActionNewOdf = new QAction(QIcon(":/phynetwork/images/oper_add.png"), tr("New %1").arg(ProjectExplorer::PeDevice::GetDeviceTypeName(ProjectExplorer::PeDevice::dtODF)), this);
    connect(m_pActionNewOdf, SIGNAL(triggered()), this, SLOT(SlotActionNewOdf()));

    m_pActionNewOdfLayer = new QAction(QIcon(":/phynetwork/images/oper_add.png"), tr("New ODF Layer"), this);
    connect(m_pActionNewOdfLayer, SIGNAL(triggered()), this, SLOT(SlotActionNewOdfLayer()));

    m_pActionDelete = new QAction(QIcon(":/phynetwork/images/oper_remove.png"), tr("Delete"), this);
    connect(m_pActionDelete, SIGNAL(triggered()), this, SLOT(SlotActionDelete()));
    QToolButton *pToolButtonDelete = new QToolButton(m_pStyleBar);
    pToolButtonDelete->setFocusPolicy(Qt::NoFocus);
    pToolButtonDelete->setAutoRaise(true);
    pToolButtonDelete->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonDelete->setDefaultAction(m_pActionDelete);

    m_pActionProperty = new QAction(QIcon(":/phynetwork/images/oper_edit.png"), tr("Property"), this);
    connect(m_pActionProperty, SIGNAL(triggered()), this, SLOT(SlotActionProperty()));
    QToolButton *pToolButtonProperty = new QToolButton(m_pStyleBar);
    pToolButtonProperty->setFocusPolicy(Qt::NoFocus);
    pToolButtonProperty->setAutoRaise(true);
    pToolButtonProperty->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonProperty->setDefaultAction(m_pActionProperty);

    m_pActionMerge = new QAction(QIcon(":/phynetwork/images/oper_merge.png"), tr("Merge"), this);
    connect(m_pActionMerge, SIGNAL(triggered()), this, SLOT(SlotActionMerge()));
    QToolButton *pToolButtonMerge = new QToolButton(m_pStyleBar);
    pToolButtonMerge->setFocusPolicy(Qt::NoFocus);
    pToolButtonMerge->setAutoRaise(true);
    pToolButtonMerge->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonMerge->setDefaultAction(m_pActionMerge);

    m_pActionSplit = new QAction(QIcon(":/phynetwork/images/oper_split.png"), tr("Split"), this);
    connect(m_pActionSplit, SIGNAL(triggered()), this, SLOT(SlotActionSplit()));
    QToolButton *pToolButtonSplit = new QToolButton(m_pStyleBar);
    pToolButtonSplit->setFocusPolicy(Qt::NoFocus);
    pToolButtonSplit->setAutoRaise(true);
    pToolButtonSplit->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    pToolButtonSplit->setDefaultAction(m_pActionSplit);

    QHBoxLayout *pHBoxLayout = new QHBoxLayout(m_pStyleBar);
    pHBoxLayout->setContentsMargins(10, 0, 0, 0);
    pHBoxLayout->addWidget(m_pLabelDisplayIcon);
    pHBoxLayout->addWidget(m_pLabelDisplayName);
    pHBoxLayout->addStretch(1);
    pHBoxLayout->addWidget(pToolButtonDelete);
    pHBoxLayout->addWidget(pToolButtonProperty);
    pHBoxLayout->addWidget(pToolButtonMerge);
    pHBoxLayout->addWidget(pToolButtonSplit);
    pHBoxLayout->addWidget(pToolButtonClear);
    pHBoxLayout->addWidget(pToolButtonRefresh);
    pHBoxLayout->addWidget(pToolButtonSettings);

    QAction *pActionSeparatorOptical = new QAction(this);
    pActionSeparatorOptical->setSeparator(true);

    QAction *pActionSeparatorTail = new QAction(this);
    pActionSeparatorTail->setSeparator(true);

    QAction *pActionSeparatorOdf = new QAction(this);
    pActionSeparatorOdf->setSeparator(true);

    m_pViewOptical->addAction(m_pActionMerge);
    m_pViewOptical->addAction(m_pActionSplit);
    m_pViewOptical->addAction(pActionSeparatorOptical);
    m_pViewOptical->addAction(m_pActionDelete);
    m_pViewOptical->addAction(m_pActionProperty);

    m_pViewTail->addAction(m_pActionMerge);
    m_pViewTail->addAction(m_pActionSplit);
    m_pViewTail->addAction(pActionSeparatorTail);
    m_pViewTail->addAction(m_pActionDelete);
    m_pViewTail->addAction(m_pActionProperty);

    m_pViewJump->addAction(m_pActionProperty);

    m_pViewOdf->addAction(m_pActionNewOdf);
    m_pViewOdf->addAction(m_pActionNewOdfLayer);
    m_pViewOdf->addAction(pActionSeparatorOdf);
    m_pViewOdf->addAction(m_pActionDelete);
    m_pViewOdf->addAction(m_pActionProperty);

    return m_pStyleBar;
}

void CableModeWidget::SetupOpticalModelView()
{
    m_pViewOptical = new TreeViewCable(this);
    m_pViewOptical->setObjectName("Styled_TreeView");
    m_pViewOptical->setFrameStyle(QFrame::NoFrame);
    m_pViewOptical->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewOptical->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pViewOptical->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewOptical->setDragEnabled(true);
    m_pViewOptical->setAcceptDrops(true);
    m_pViewOptical->setDropIndicatorShown(true);

    m_pModelOptical = new StandardItemModelCable(0, 9, this);
    m_pModelOptical->setHeaderData(0, Qt::Horizontal, ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctOptical));
    m_pModelOptical->setHeaderData(1, Qt::Horizontal, tr("Pipe Color"));
    m_pModelOptical->setHeaderData(2, Qt::Horizontal, tr("Fiber Color"));
    m_pModelOptical->setHeaderData(3, Qt::Horizontal, tr("Current Port"));
    m_pModelOptical->setHeaderData(4, Qt::Horizontal, tr("Current Jump Fiber/Tail Fiber"));
    m_pModelOptical->setHeaderData(5, Qt::Horizontal, tr("Current ODF Port"));
    m_pModelOptical->setHeaderData(6, Qt::Horizontal, tr("Side ODF Port"));
    m_pModelOptical->setHeaderData(7, Qt::Horizontal, tr("Side Jump Fiber/Tail Fiber"));
    m_pModelOptical->setHeaderData(8, Qt::Horizontal, tr("Side Port"));
    m_pViewOptical->setModel(m_pModelOptical);    

    m_pViewOptical->setColumnWidth(0, 200);
    m_pViewOptical->setColumnWidth(1, 80);
    m_pViewOptical->setColumnWidth(2, 80);
    m_pViewOptical->setColumnWidth(3, 290);
    m_pViewOptical->setColumnWidth(4, 290);
    m_pViewOptical->setColumnWidth(5, 80);
    m_pViewOptical->setColumnWidth(6, 80);
    m_pViewOptical->setColumnWidth(7, 290);
    m_pViewOptical->setColumnWidth(8, 290);

    connect(m_pViewOptical->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
}

void CableModeWidget::SetupTailModelView()
{
    m_pViewTail = new TreeViewCable(this);
    m_pViewTail->setObjectName("Styled_TreeView");
    m_pViewTail->setFrameStyle(QFrame::NoFrame);
    m_pViewTail->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewTail->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pViewTail->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewTail->setDragEnabled(true);
    m_pViewTail->setAcceptDrops(true);
    m_pViewTail->setDropIndicatorShown(true);

    m_pModelTail = new StandardItemModelCable(0, 5, this);
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

    connect(m_pViewTail->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
}

void CableModeWidget::SetupJumpModelView()
{
    m_pViewJump = new Utils::ReadOnlyTreeView(this);
    m_pViewJump->setObjectName("Styled_TreeView");
    m_pViewJump->setFrameStyle(QFrame::NoFrame);
    m_pViewJump->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewJump->setContextMenuPolicy(Qt::ActionsContextMenu);

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

    connect(m_pViewJump->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
}

void CableModeWidget::SetupOdfModelView()
{
    m_pViewOdf = new TreeViewOdf(this);
    m_pViewOdf->setObjectName("Styled_TreeView");
    m_pViewOdf->setFrameStyle(QFrame::NoFrame);
    m_pViewOdf->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pViewOdf->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewOdf->setDragEnabled(true);
    m_pViewOdf->setAcceptDrops(true);
    m_pViewOdf->setDropIndicatorShown(true);

    m_pModelOdf = new StandardItemModelOdf(0, 7, this);
    m_pModelOdf->setHeaderData(0, Qt::Horizontal, tr("Current ODF"));
    m_pModelOdf->setHeaderData(1, Qt::Horizontal, tr("Current Jump Fiber/Tail Fiber"));
    m_pModelOdf->setHeaderData(2, Qt::Horizontal, tr("Current Jump Fiber/Tail Fiber Port"));
    m_pModelOdf->setHeaderData(3, Qt::Horizontal, tr("Optical Fiber"));
    m_pModelOdf->setHeaderData(4, Qt::Horizontal, tr("Side ODF"));
    m_pModelOdf->setHeaderData(5, Qt::Horizontal, tr("Side Jump Fiber/Tail Fiber"));
    m_pModelOdf->setHeaderData(6, Qt::Horizontal, tr("Side Jump Fiber/Tail Fiber Port"));
    m_pViewOdf->setModel(m_pModelOdf);

    m_pViewOdf->setColumnWidth(0, 140);
    m_pViewOdf->setColumnWidth(1, 140);
    m_pViewOdf->setColumnWidth(2, 300);
    m_pViewOdf->setColumnWidth(3, 140);
    m_pViewOdf->setColumnWidth(4, 140);
    m_pViewOdf->setColumnWidth(5, 140);
    m_pViewOdf->setColumnWidth(6, 300);

    connect(m_pViewOdf->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
}

void CableModeWidget::BuildOpticalModel(ProjectExplorer::PeCubicle *pCubicle)
{
    m_pModelOptical->removeRows(0, m_pModelOptical->rowCount());

    if(!pCubicle)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pCubicle->GetProjectVersion();
    if(!pProjectVersion)
        return;

    QMap<ProjectExplorer::PeCubicle*, QList<ProjectExplorer::PeCable*> > mapSideCubicleToCables;
    QList<ProjectExplorer::PeCable*> lstJumpAndTailCables;
    foreach(ProjectExplorer::PeCable *pCable, pProjectVersion->GetAllCables())
    {
        if(pCable->GetCubicle1() == pCubicle || pCable->GetCubicle2() == pCubicle)
        {
            if(pCable->GetCableType() == ProjectExplorer::PeCable::ctOptical)
            {
                ProjectExplorer::PeCubicle *pCubicleSide = pCable->GetCubicle1() == pCubicle ? pCable->GetCubicle2() : pCable->GetCubicle1();
                QList<ProjectExplorer::PeCable*> &lstOpticalCables = mapSideCubicleToCables[pCubicleSide];
                lstOpticalCables.append(pCable);
            }
            else if(pCable->GetCableType() == ProjectExplorer::PeCable::ctJump || pCable->GetCableType() == ProjectExplorer::PeCable::ctTail)
            {
                lstJumpAndTailCables.append(pCable);
            }
        }
    }

    QList<ProjectExplorer::PeCubicle*> lstSideCubicles = mapSideCubicleToCables.keys();
    qSort(lstSideCubicles.begin(), lstSideCubicles.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    foreach(ProjectExplorer::PeCubicle *pCubicleSide, lstSideCubicles)
    {
        QList<ProjectExplorer::PeCable*> lstOpticalCables = mapSideCubicleToCables.value(pCubicleSide);
        qSort(lstOpticalCables.begin(), lstOpticalCables.end(), ProjectExplorer::PeCable::ComparCableName);

        foreach(ProjectExplorer::PeCable *pCable, lstOpticalCables)
        {
            QList<QStandardItem*> lstOpticalItems;

            QStandardItem *pItemCable = new QStandardItem(pCable->GetDisplayIcon(), pCable->GetDisplayName() + " : " + pCubicleSide->GetDisplayName());
            pItemCable->setData(reinterpret_cast<int>(pCable));
            pItemCable->setDragEnabled(false);
            pItemCable->setDropEnabled(false);
            lstOpticalItems.append(pItemCable);

            for(int i = 0; i < m_pModelOptical->columnCount() - 1; i++)
            {
                QStandardItem *pItem = new QStandardItem("");
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstOpticalItems.append(pItem);
            }
            m_pModelOptical->appendRow(lstOpticalItems);

            QList<ProjectExplorer::PeFiber*> lstFibers = pCable->GetChildFibers();
            qSort(lstFibers.begin(), lstFibers.end(), ProjectExplorer::PeFiber::CompareIndex);
            foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
            {
                QList<QStandardItem*> lstFiberItems;

                // Name
                QStandardItem *pItemName = new QStandardItem(pFiber->GetDisplayIcon(), pFiber->GetDisplayName());
                pItemName->setData(reinterpret_cast<int>(pFiber));
                pItemName->setDragEnabled(true);
                pItemName->setDropEnabled(true);
                lstFiberItems.append(pItemName);

                // Pipe Color
                QStandardItem *pItemPipeColor = new QStandardItem(ProjectExplorer::PeFiber::GetFiberColorName(pFiber->GetPipeColor()));
                pItemPipeColor->setForeground(QBrush(ProjectExplorer::PeFiber::GetFiberColor(pFiber->GetPipeColor())));
                pItemPipeColor->setDragEnabled(true);
                pItemPipeColor->setDropEnabled(true);
                lstFiberItems.append(pItemPipeColor);

                // Fiber Color
                QStandardItem *pItemFiberColor = new QStandardItem(ProjectExplorer::PeFiber::GetFiberColorName(pFiber->GetFiberColor()));
                pItemFiberColor->setForeground(QBrush(ProjectExplorer::PeFiber::GetFiberColor(pFiber->GetFiberColor())));
                pItemFiberColor->setDragEnabled(true);
                pItemFiberColor->setDropEnabled(true);
                lstFiberItems.append(pItemFiberColor);

                //////////////////////////////////////////////////////////////////
                // Calculate current port, current fiber, current odf port
                //////////////////////////////////////////////////////////////////
                ProjectExplorer::PePort *pCurrentOdfPort = (pCable->GetCubicle1() == pCubicle ? pFiber->GetPort1() : pFiber->GetPort2());
                if(!pCurrentOdfPort)
                    continue;
                ProjectExplorer::PeDevice *pCurrentOdf = pCurrentOdfPort->GetParentDevice();
                if(!pCurrentOdf)
                    continue;

                ProjectExplorer::PeFiber *pCurrentFiber = 0;
                foreach(ProjectExplorer::PeCable *pJumpAndTailCable, lstJumpAndTailCables)
                {
                    foreach(ProjectExplorer::PeFiber *pJumpAndTailFiber, pJumpAndTailCable->GetChildFibers())
                    {
                        if(pJumpAndTailFiber->GetPort1() == pCurrentOdfPort || pJumpAndTailFiber->GetPort2() == pCurrentOdfPort)
                        {
                            pCurrentFiber = pJumpAndTailFiber;
                            break;
                        }
                    }

                    if(pCurrentFiber)
                        break;
                }

                QString strCurrentFiber, strCurrentPort;
                if(pCurrentFiber)
                {
                    ProjectExplorer::PeCable *pCurrentCable = pCurrentFiber->GetParentCable();
                    if(!pCurrentCable)
                        continue;

                    ProjectExplorer::PePort *pCurrentDevicePort = pCurrentFiber->GetPort1() == pCurrentOdfPort ? pCurrentFiber->GetPort2() : pCurrentFiber->GetPort1();
                    if(!pCurrentDevicePort)
                        continue;

                    ProjectExplorer::PeBoard *pCurrentDeviceBoard = pCurrentDevicePort->GetParentBoard();
                    if(!pCurrentDeviceBoard)
                        continue;

                    ProjectExplorer::PeDevice *pCurrentDevic = pCurrentDeviceBoard->GetParentDevice();
                    if(!pCurrentDevic)
                        continue;

                    if(pCurrentCable->GetCableType() == ProjectExplorer::PeCable::ctJump)
                        strCurrentFiber = pCurrentCable->GetDisplayName();
                    else if(pCurrentCable->GetCableType() == ProjectExplorer::PeCable::ctTail)
                        strCurrentFiber = QString("%1(%2)").arg(pCurrentCable->GetDisplayName()).arg(pCurrentFiber->GetDisplayName());

                    if(pCurrentDevic->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                        strCurrentPort = QString("%1 <%2/%3>").arg(pCurrentDevic->GetDisplayName()).arg(pCurrentDeviceBoard->GetDisplayName()).arg(pCurrentDevicePort->GetDisplayName());
                    else if(pCurrentDevic->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                        strCurrentPort = QString("%1 <%2>").arg(pCurrentDevic->GetDisplayName()).arg(pCurrentDevicePort->GetDisplayName());

                }

                // Current Port
                QStandardItem *pItemCurrentPort = new QStandardItem(strCurrentPort);
                pItemCurrentPort->setDragEnabled(true);
                pItemCurrentPort->setDropEnabled(true);
                lstFiberItems.append(pItemCurrentPort);

                // Current Jump Fiber/Tail Fiber
                QStandardItem *pItemCurrentJumpTailFiber = new QStandardItem(strCurrentFiber);
                pItemCurrentJumpTailFiber->setDragEnabled(true);
                pItemCurrentJumpTailFiber->setDropEnabled(true);
                lstFiberItems.append(pItemCurrentJumpTailFiber);

                // Current Odf Port
                QStandardItem *pItemCurrentOdfPort = new QStandardItem(QString("%1: %2").arg(pCurrentOdf->GetDisplayName()).arg(pCurrentOdfPort->GetDisplayName()));
                pItemCurrentOdfPort->setDragEnabled(true);
                pItemCurrentOdfPort->setDropEnabled(true);
                lstFiberItems.append(pItemCurrentOdfPort);

                //////////////////////////////////////////////////////////////////
                // Calculate side port, side fiber, side odf port
                //////////////////////////////////////////////////////////////////
                ProjectExplorer::PePort *pSideOdfPort = (pCable->GetCubicle1() == pCubicleSide ? pFiber->GetPort1() : pFiber->GetPort2());
                if(!pSideOdfPort)
                    continue;
                ProjectExplorer::PeDevice *pSideOdf = pSideOdfPort->GetParentDevice();
                if(!pSideOdf)
                    continue;

                ProjectExplorer::PeFiber *pSideFiber = 0;
                foreach(ProjectExplorer::PeCable *pJumpAndTailCable, pProjectVersion->GetAllCables())
                {
                    if((pJumpAndTailCable->GetCubicle1() != pCubicleSide && pJumpAndTailCable->GetCubicle2() != pCubicleSide) ||
                       (pJumpAndTailCable->GetCableType() != ProjectExplorer::PeCable::ctJump && pJumpAndTailCable->GetCableType() != ProjectExplorer::PeCable::ctTail))
                    {
                        continue;
                    }

                    foreach(ProjectExplorer::PeFiber *pJumpAndTailFiber, pJumpAndTailCable->GetChildFibers())
                    {
                        if(pJumpAndTailFiber->GetPort1() == pSideOdfPort || pJumpAndTailFiber->GetPort2() == pSideOdfPort)
                        {
                            pSideFiber = pJumpAndTailFiber;
                            break;
                        }
                    }

                    if(pSideFiber)
                        break;
                }

                QString strSideFiber, strSidePort;
                if(pSideFiber)
                {
                    ProjectExplorer::PeCable *pSideCable = pSideFiber->GetParentCable();
                    if(!pSideCable)
                        continue;

                    ProjectExplorer::PePort *pSideDevicePort = pSideFiber->GetPort1() == pSideOdfPort ? pSideFiber->GetPort2() : pSideFiber->GetPort1();
                    if(!pSideDevicePort)
                        continue;

                    ProjectExplorer::PeBoard *pSideDeviceBoard = pSideDevicePort->GetParentBoard();
                    if(!pSideDeviceBoard)
                        continue;

                    ProjectExplorer::PeDevice *pSideDevic = pSideDeviceBoard->GetParentDevice();
                    if(!pSideDevic)
                        continue;

                    if(pSideCable->GetCableType() == ProjectExplorer::PeCable::ctJump)
                        strSideFiber = pSideCable->GetDisplayName();
                    else if(pSideCable->GetCableType() == ProjectExplorer::PeCable::ctTail)
                        strSideFiber = QString("%1(%2)").arg(pSideCable->GetDisplayName()).arg(pSideFiber->GetDisplayName());

                    if(pSideDevic->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                        strSidePort = QString("%1 <%2/%3>").arg(pSideDevic->GetDisplayName()).arg(pSideDeviceBoard->GetDisplayName()).arg(pSideDevicePort->GetDisplayName());
                    else if(pSideDevic->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                        strSidePort = QString("%1 <%2>").arg(pSideDevic->GetDisplayName()).arg(pSideDevicePort->GetDisplayName());

                }

                // Side Odf Port
                QStandardItem *pItemSideOdfPort = new QStandardItem(QString("%1: %2").arg(pSideOdf->GetDisplayName()).arg(pSideOdfPort->GetDisplayName()));
                pItemSideOdfPort->setDragEnabled(true);
                pItemSideOdfPort->setDropEnabled(true);
                lstFiberItems.append(pItemSideOdfPort);

                // Side Jump Fiber/Tail Fiber
                QStandardItem *pItemSideJumpTailFiber = new QStandardItem(strSideFiber);
                pItemSideJumpTailFiber->setDragEnabled(true);
                pItemSideJumpTailFiber->setDropEnabled(true);
                lstFiberItems.append(pItemSideJumpTailFiber);

                // Side Port
                QStandardItem *pItemSidePort = new QStandardItem(strSidePort);
                pItemSidePort->setDragEnabled(true);
                pItemSidePort->setDropEnabled(true);
                lstFiberItems.append(pItemSidePort);

                pItemCable->appendRow(lstFiberItems);
            }
        }
    }

    m_pViewOptical->expandAll();
}

void CableModeWidget::BuildTailModel(ProjectExplorer::PeCubicle *pCubicle)
{
    m_pModelTail->removeRows(0, m_pModelTail->rowCount());

    if(!pCubicle)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pCubicle->GetProjectVersion();
    if(!pProjectVersion)
        return;

    QMap<ProjectExplorer::PeCubicle*, QList<ProjectExplorer::PeCable*> > mapSideCubicleToCables;
    foreach(ProjectExplorer::PeCable *pCable, pProjectVersion->GetAllCables())
    {
        if(pCable->GetCableType() == ProjectExplorer::PeCable::ctTail && (pCable->GetCubicle1() == pCubicle || pCable->GetCubicle2() == pCubicle))
        {
            ProjectExplorer::PeCubicle *pCubicleSide = pCable->GetCubicle1() == pCubicle ? pCable->GetCubicle2() : pCable->GetCubicle1();
            QList<ProjectExplorer::PeCable*> &lstTailCables = mapSideCubicleToCables[pCubicleSide];
            lstTailCables.append(pCable);
        }
    }

    QList<ProjectExplorer::PeCubicle*> lstSideCubicles = mapSideCubicleToCables.keys();
    qSort(lstSideCubicles.begin(), lstSideCubicles.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    foreach(ProjectExplorer::PeCubicle *pCubicleSide, lstSideCubicles)
    {
        QList<ProjectExplorer::PeCable*> lstTailCables = mapSideCubicleToCables.value(pCubicleSide);
        qSort(lstTailCables.begin(), lstTailCables.end(), ProjectExplorer::PeCable::ComparCableName);

        foreach(ProjectExplorer::PeCable *pCable, lstTailCables)
        {
            QList<QStandardItem*> lstTailItems;

            QStandardItem *pItemCable = new QStandardItem(pCable->GetDisplayIcon(), pCable->GetDisplayName() + " : " + pCubicleSide->GetDisplayName());
            pItemCable->setData(reinterpret_cast<int>(pCable));
            pItemCable->setDragEnabled(false);
            pItemCable->setDropEnabled(false);
            lstTailItems.append(pItemCable);

            for(int i = 0; i < m_pModelTail->columnCount() - 1; i++)
            {
                QStandardItem *pItem = new QStandardItem("");
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstTailItems.append(pItem);
            }
            m_pModelTail->appendRow(lstTailItems);

            QList<ProjectExplorer::PeFiber*> lstFibers = pCable->GetChildFibers();
            qSort(lstFibers.begin(), lstFibers.end(), ProjectExplorer::PeFiber::CompareIndex);
            foreach(ProjectExplorer::PeFiber *pFiber, lstFibers)
            {
                QList<QStandardItem*> lstFiberItems;

                // Name
                QStandardItem *pItemName = new QStandardItem(pFiber->GetDisplayIcon(), pFiber->GetDisplayName());
                pItemName->setData(reinterpret_cast<int>(pFiber));
                pItemName->setDragEnabled(true);
                pItemName->setDropEnabled(true);
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
                pItemCurrentPort->setDragEnabled(true);
                pItemCurrentPort->setDropEnabled(true);
                lstFiberItems.append(pItemCurrentPort);

                // Current Fiber Plug
                QStandardItem *pItemCurrentFiberPlug = new QStandardItem(strCurrentFiberPlug);
                pItemCurrentFiberPlug->setDragEnabled(true);
                pItemCurrentFiberPlug->setDropEnabled(true);
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
                pItemSidePort->setDragEnabled(true);
                pItemSidePort->setDropEnabled(true);
                lstFiberItems.append(pItemSidePort);

                // Side Fiber Plug
                QStandardItem *pItemSideFiberPlug = new QStandardItem(strSideFiberPlug);
                pItemSideFiberPlug->setDragEnabled(true);
                pItemSideFiberPlug->setDropEnabled(true);
                lstFiberItems.append(pItemSideFiberPlug);

                pItemCable->appendRow(lstFiberItems);
            }
        }
    }

    m_pViewTail->expandAll();
}

void CableModeWidget::BuildJumpModel(ProjectExplorer::PeCubicle *pCubicle)
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
    qSort(lstJumpCables.begin(), lstJumpCables.end(), ProjectExplorer::PeCable::ComparCableName);

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
        if(!pDevice1)
            continue;

        ProjectExplorer::PePort *pPort2 = pFiber->GetPort2();
        if(!pPort2)
            continue;

        ProjectExplorer::PeBoard *pBoard2 = pPort2->GetParentBoard();
        if(!pBoard2)
            continue;

        ProjectExplorer::PeDevice *pDevice2 = pBoard2->GetParentDevice();
        if(!pDevice2)
            continue;

        ProjectExplorer::PePort *pPortCurrent = pPort1, *pPortSide = pPort2;
        ProjectExplorer::PeBoard *pBoardCurrent = pBoard1, *pBoardSide = pBoard2;
        ProjectExplorer::PeDevice *pDeviceCurrent = pDevice1, *pDeviceSide = pDevice2;
        if(pDevice1->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
        {
            pPortCurrent = pPort1;
            pBoardCurrent = pBoard1;
            pDeviceCurrent = pDevice1;
            pPortSide = pPort2;
            pBoardSide = pBoard2;
            pDeviceSide = pDevice2;
        }
        else if(pDevice2->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
        {
            pPortCurrent = pPort2;
            pBoardCurrent = pBoard2;
            pDeviceCurrent = pDevice2;
            pPortSide = pPort1;
            pBoardSide = pBoard1;
            pDeviceSide = pDevice1;
        }

        QList<QStandardItem*> lstJumpItems;

        QStandardItem *pItemName = new QStandardItem(pCable->GetDisplayIcon(), pCable->GetDisplayName());
        pItemName->setData(reinterpret_cast<int>(pCable));
        lstJumpItems.append(pItemName);

        // Current Port
        QString strPortCurrent;
        if(pDeviceCurrent->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
            strPortCurrent = QString("%1 <%2/%3>").arg(pDeviceCurrent->GetDisplayName()).arg(pBoardCurrent->GetDisplayName()).arg(pPortCurrent->GetDisplayName());
        else if(pDeviceCurrent->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch || pDeviceCurrent->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
            strPortCurrent = QString("%1 <%2>").arg(pDeviceCurrent->GetDisplayName()).arg(pPortCurrent->GetDisplayName());
        QStandardItem *pItemCurrentPort = new QStandardItem(strPortCurrent);
        lstJumpItems.append(pItemCurrentPort);

        // Current Fiber Plug
        QStandardItem *pItemCurrentFiberPlug = new QStandardItem(ProjectExplorer::PePort::GetFiberPlugName(pPortCurrent->GetFiberPlug()));
        lstJumpItems.append(pItemCurrentFiberPlug);

        // Side Port
        QString strPortSide;
        if(pDeviceSide->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
            strPortSide = QString("%1 <%2/%3>").arg(pDeviceSide->GetDisplayName()).arg(pBoardSide->GetDisplayName()).arg(pPortSide->GetDisplayName());
        else if(pDeviceSide->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch || pDeviceSide->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
            strPortSide = QString("%1 <%2>").arg(pDeviceSide->GetDisplayName()).arg(pPortSide->GetDisplayName());
        QStandardItem *pItemSidePort = new QStandardItem(strPortSide);
        lstJumpItems.append(pItemSidePort);

        // Side Fiber Plug
        QStandardItem *pItemSideFiberPlug = new QStandardItem(ProjectExplorer::PePort::GetFiberPlugName(pPortSide->GetFiberPlug()));
        lstJumpItems.append(pItemSideFiberPlug);

        m_pModelJump->appendRow(lstJumpItems);
    }

    m_pViewJump->expandAll();
}

void CableModeWidget::BuildOdfModel(ProjectExplorer::PeCubicle *pCubicle)
{
    m_pModelOdf->removeRows(0, m_pModelOdf->rowCount());

    if(!pCubicle)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = pCubicle->GetProjectVersion();
    if(!pProjectVersion)
        return;

    QList<ProjectExplorer::PeDevice*> lstOdfs;
    foreach(ProjectExplorer::PeDevice *pDevice, pCubicle->GetChildDevices())
    {
        if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
            lstOdfs.append(pDevice);
    }
    qSort(lstOdfs.begin(), lstOdfs.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

    foreach(ProjectExplorer::PeDevice *pOdf, lstOdfs)
    {
        QList<QStandardItem*> lstOdfItems;

        QStandardItem *pItemOdf = new QStandardItem(pOdf->GetDisplayIcon(), pOdf->GetDisplayName());
        pItemOdf->setData(reinterpret_cast<int>(pOdf));
        pItemOdf->setDragEnabled(false);
        pItemOdf->setDropEnabled(false);
        lstOdfItems.append(pItemOdf);

        for(int i = 0; i < m_pModelOdf->columnCount() - 1; i++)
        {
            QStandardItem *pItem = new QStandardItem("");
            pItem->setDragEnabled(false);
            pItem->setDropEnabled(false);
            lstOdfItems.append(pItem);
        }
        m_pModelOdf->appendRow(lstOdfItems);

        QList<ProjectExplorer::PeBoard*> lstLayers = pOdf->GetChildBoards();
        qSort(lstLayers.begin(), lstLayers.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
        foreach(ProjectExplorer::PeBoard *pLayer, lstLayers)
        {
            QList<QStandardItem*> lstLayerItems;

            QStandardItem *pItemLayer = new QStandardItem(pLayer->GetDisplayIcon(), pLayer->GetDisplayName());
            pItemLayer->setData(reinterpret_cast<int>(pLayer));
            pItemLayer->setDragEnabled(false);
            pItemLayer->setDropEnabled(false);
            lstLayerItems.append(pItemLayer);

            for(int i = 0; i < m_pModelOdf->columnCount() - 1; i++)
            {
                QStandardItem *pItem = new QStandardItem("");
                pItem->setDragEnabled(false);
                pItem->setDropEnabled(false);
                lstLayerItems.append(pItem);
            }
            pItemOdf->appendRow(lstLayerItems);

            QList<ProjectExplorer::PePort*> lstPorts = pLayer->GetChildPorts();
            qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
            foreach(ProjectExplorer::PePort *pCurrentOdfPort, lstPorts)
            {
                ProjectExplorer::PeFiber *pFiberOptical = 0, *pFiberCurrentJumpTail = 0;
                foreach(ProjectExplorer::PeFiber *pFiber, pCurrentOdfPort->GetUsedFibers())
                {
                    ProjectExplorer::PeCable *pCable = pFiber->GetParentCable();
                    ProjectExplorer::PeCable::CableType eCableType = pCable->GetCableType();

                    if(eCableType == ProjectExplorer::PeCable::ctOptical)
                        pFiberOptical = pFiber;
                    else
                        pFiberCurrentJumpTail = pFiber;

                    if(!pFiberOptical && !pFiberCurrentJumpTail)
                        break;
                }

                QString strCurrentFiberTailJump = "", strCurrentFiberTailJumpPort = "";
                if(pFiberCurrentJumpTail)
                {
                    ProjectExplorer::PeCable *pCableCurrentJumpTail = pFiberCurrentJumpTail->GetParentCable();
                    ProjectExplorer::PeCable::CableType eCableType = pCableCurrentJumpTail->GetCableType();

                    if(eCableType == ProjectExplorer::PeCable::ctTail)
                    {
                        strCurrentFiberTailJump =  QString("%1(%2)").arg(pCableCurrentJumpTail->GetDisplayName()).arg(pFiberCurrentJumpTail->GetDisplayName());

                        if(ProjectExplorer::PePort *pPortCurrentFiberTailJump = (pFiberCurrentJumpTail->GetPort1() == pCurrentOdfPort ? pFiberCurrentJumpTail->GetPort2() : pFiberCurrentJumpTail->GetPort1()))
                        {
                            if(ProjectExplorer::PeBoard *pBoardCurrentFiberTailJump = pPortCurrentFiberTailJump->GetParentBoard())
                            {
                                if(ProjectExplorer::PeDevice *pDeviceCurrentFiberTailJump = pBoardCurrentFiberTailJump->GetParentDevice())
                                {
                                    if(pDeviceCurrentFiberTailJump->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                                        strCurrentFiberTailJumpPort = QString("%1 <%2/%3>").arg(pDeviceCurrentFiberTailJump->GetDisplayName()).arg(pBoardCurrentFiberTailJump->GetDisplayName()).arg(pPortCurrentFiberTailJump->GetDisplayName());
                                    else if(pDeviceCurrentFiberTailJump->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch || pDeviceCurrentFiberTailJump->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                                        strCurrentFiberTailJumpPort = QString("%1 <%2>").arg(pDeviceCurrentFiberTailJump->GetDisplayName()).arg(pPortCurrentFiberTailJump->GetDisplayName());
                                }
                            }
                        }
                    }
                    else if(eCableType == ProjectExplorer::PeCable::ctJump)
                    {
                        strCurrentFiberTailJump = pCableCurrentJumpTail->GetDisplayName();

                        if(ProjectExplorer::PePort *pPortCurrentFiberTailJump = (pFiberCurrentJumpTail->GetPort1() == pCurrentOdfPort ? pFiberCurrentJumpTail->GetPort2() : pFiberCurrentJumpTail->GetPort1()))
                        {
                            if(ProjectExplorer::PeBoard *pBoardCurrentFiberTailJump = pPortCurrentFiberTailJump->GetParentBoard())
                            {
                                if(ProjectExplorer::PeDevice *pDeviceCurrentFiberTailJump = pBoardCurrentFiberTailJump->GetParentDevice())
                                {
                                    if(pDeviceCurrentFiberTailJump->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                                        strCurrentFiberTailJumpPort = QString("%1 <%2/%3>").arg(pDeviceCurrentFiberTailJump->GetDisplayName()).arg(pBoardCurrentFiberTailJump->GetDisplayName()).arg(pPortCurrentFiberTailJump->GetDisplayName());
                                    else if(pDeviceCurrentFiberTailJump->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch || pDeviceCurrentFiberTailJump->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                                        strCurrentFiberTailJumpPort = QString("%1 <%2>").arg(pDeviceCurrentFiberTailJump->GetDisplayName()).arg(pPortCurrentFiberTailJump->GetDisplayName());
                                }
                            }
                        }
                    }
                }

                QString strFiberOptical = "", strSideOdfPort = "", strSideFiberTailJump = "", strSideFiberTailJumpPort = "";
                if(pFiberOptical)
                {
                    ProjectExplorer::PeCable *pCableOptical = pFiberOptical->GetParentCable();
                    strFiberOptical = QString("%1(%2)").arg(pCableOptical->GetDisplayName()).arg(pFiberOptical->GetDisplayName());

                    if(ProjectExplorer::PePort *pSideOdfPort = (pFiberOptical->GetPort1() == pCurrentOdfPort ? pFiberOptical->GetPort2() : pFiberOptical->GetPort1()))
                    {
                        if(ProjectExplorer::PeBoard *pSideOdfLayer = pSideOdfPort->GetParentBoard())
                        {
                            if(ProjectExplorer::PeDevice *pSideOdf = pSideOdfLayer->GetParentDevice())
                            {
                                if(pSideOdf->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                                    strSideOdfPort = QString("%1 <%2/%3>").arg(pSideOdf->GetDisplayName()).arg(pSideOdfLayer->GetDisplayName()).arg(pSideOdfPort->GetDisplayName());
                                else if(pSideOdf->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch || pSideOdf->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                                    strSideOdfPort = QString("%1 <%2>").arg(pSideOdf->GetDisplayName()).arg(pSideOdfPort->GetDisplayName());
                            }
                        }

                        ProjectExplorer::PeFiber *pFiberSideJumpTail = 0;
                        foreach(ProjectExplorer::PeFiber *pFiber, pSideOdfPort->GetUsedFibers())
                        {
                            if(pFiber != pFiberOptical && pFiber->GetParentCable()->GetCableType() != ProjectExplorer::PeCable::ctTail)
                            {
                                pFiberSideJumpTail = pFiber;
                                break;
                            }
                        }

                        if(pFiberSideJumpTail)
                        {
                            ProjectExplorer::PeCable *pCableSideJumpTail = pFiberSideJumpTail->GetParentCable();
                            ProjectExplorer::PeCable::CableType eCableType = pCableSideJumpTail->GetCableType();

                            if(eCableType == ProjectExplorer::PeCable::ctTail)
                            {
                                strSideFiberTailJump =  QString("%1(%2)").arg(pCableSideJumpTail->GetDisplayName()).arg(pFiberSideJumpTail->GetDisplayName());

                                if(ProjectExplorer::PePort *pPortSideFiberTailJump = (pFiberSideJumpTail->GetPort1() == pSideOdfPort ? pFiberSideJumpTail->GetPort2() : pFiberSideJumpTail->GetPort1()))
                                {
                                    if(ProjectExplorer::PeBoard *pBoardSideFiberTailJump = pPortSideFiberTailJump->GetParentBoard())
                                    {
                                        if(ProjectExplorer::PeDevice *pDeviceSideFiberTailJump = pBoardSideFiberTailJump->GetParentDevice())
                                        {
                                            if(pDeviceSideFiberTailJump->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                                                strSideFiberTailJumpPort = QString("%1 <%2/%3>").arg(pDeviceSideFiberTailJump->GetDisplayName()).arg(pBoardSideFiberTailJump->GetDisplayName()).arg(pPortSideFiberTailJump->GetDisplayName());
                                            else if(pDeviceSideFiberTailJump->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch || pDeviceSideFiberTailJump->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                                                strSideFiberTailJumpPort = QString("%1 <%2>").arg(pDeviceSideFiberTailJump->GetDisplayName()).arg(pPortSideFiberTailJump->GetDisplayName());
                                        }
                                    }
                                }
                            }
                            else if(eCableType == ProjectExplorer::PeCable::ctJump)
                            {
                                strSideFiberTailJump = pCableSideJumpTail->GetDisplayName();

                                if(ProjectExplorer::PePort *pPortSideFiberTailJump = (pFiberSideJumpTail->GetPort1() == pSideOdfPort ? pFiberSideJumpTail->GetPort2() : pFiberSideJumpTail->GetPort1()))
                                {
                                    if(ProjectExplorer::PeBoard *pBoardSideFiberTailJump = pPortSideFiberTailJump->GetParentBoard())
                                    {
                                        if(ProjectExplorer::PeDevice *pDeviceSideFiberTailJump = pBoardSideFiberTailJump->GetParentDevice())
                                        {
                                            if(pDeviceSideFiberTailJump->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                                                strSideFiberTailJumpPort = QString("%1 <%2/%3>").arg(pDeviceSideFiberTailJump->GetDisplayName()).arg(pBoardSideFiberTailJump->GetDisplayName()).arg(pPortSideFiberTailJump->GetDisplayName());
                                            else if(pDeviceSideFiberTailJump->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch || pDeviceSideFiberTailJump->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                                                strSideFiberTailJumpPort = QString("%1 <%2>").arg(pDeviceSideFiberTailJump->GetDisplayName()).arg(pPortSideFiberTailJump->GetDisplayName());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                QList<QStandardItem*> lstPortItems;

                QStandardItem *pItemCurrentOdfPort = new QStandardItem(pCurrentOdfPort->GetDisplayIcon(), pCurrentOdfPort->GetDisplayName());
                pItemCurrentOdfPort->setData(reinterpret_cast<int>(pCurrentOdfPort));
                pItemCurrentOdfPort->setDragEnabled(true);
                pItemCurrentOdfPort->setDropEnabled(true);
                lstPortItems.append(pItemCurrentOdfPort);

                QStandardItem *pItemCurrentFiberTailJump = new QStandardItem(strCurrentFiberTailJump);
                pItemCurrentFiberTailJump->setData(reinterpret_cast<int>(pCurrentOdfPort));
                pItemCurrentFiberTailJump->setDragEnabled(true);
                pItemCurrentFiberTailJump->setDropEnabled(true);
                lstPortItems.append(pItemCurrentFiberTailJump);

                QStandardItem *pItemCurrentFiberTailJumpPort = new QStandardItem(strCurrentFiberTailJumpPort);
                pItemCurrentFiberTailJumpPort->setData(reinterpret_cast<int>(pCurrentOdfPort));
                pItemCurrentFiberTailJumpPort->setDragEnabled(true);
                pItemCurrentFiberTailJumpPort->setDropEnabled(true);
                lstPortItems.append(pItemCurrentFiberTailJumpPort);

                QStandardItem *pItemFiberOptical = new QStandardItem(strFiberOptical);
                pItemFiberOptical->setData(reinterpret_cast<int>(pCurrentOdfPort));
                pItemFiberOptical->setDragEnabled(true);
                pItemFiberOptical->setDropEnabled(true);
                lstPortItems.append(pItemFiberOptical);

                QStandardItem *pItemSideOdfPort = new QStandardItem(strSideOdfPort);
                pItemSideOdfPort->setData(reinterpret_cast<int>(pCurrentOdfPort));
                pItemSideOdfPort->setDragEnabled(true);
                pItemSideOdfPort->setDropEnabled(true);
                lstPortItems.append(pItemSideOdfPort);

                QStandardItem *pItemSideFiberTailJump = new QStandardItem(strSideFiberTailJump);
                pItemSideFiberTailJump->setData(reinterpret_cast<int>(pCurrentOdfPort));
                pItemSideFiberTailJump->setDragEnabled(true);
                pItemSideFiberTailJump->setDropEnabled(true);
                lstPortItems.append(pItemSideFiberTailJump);

                QStandardItem *pItemSideFiberTailJumpPort = new QStandardItem(strSideFiberTailJumpPort);
                pItemSideFiberTailJumpPort->setData(reinterpret_cast<int>(pCurrentOdfPort));
                pItemSideFiberTailJumpPort->setDragEnabled(true);
                pItemSideFiberTailJumpPort->setDropEnabled(true);
                lstPortItems.append(pItemSideFiberTailJumpPort);

                pItemLayer->appendRow(lstPortItems);
            }
        }
    }

    m_pViewOdf->expandAll();
}

bool CableModeWidget::GetSelectedCable(Utils::ReadOnlyTreeView *pView, QList<ProjectExplorer::PeCable*> &lstSelctedCables)
{
    if(!pView)
        return false;

    QStandardItemModel *pModel = qobject_cast<QStandardItemModel*>(pView->model());
    if(!pModel)
        return false;

    lstSelctedCables.clear();
    foreach(const QModelIndex &index, pView->selectionModel()->selectedRows())
    {
        QStandardItem *pItem = pModel->itemFromIndex(index);
        if(pItem->parent())
            return false;

        ProjectExplorer::PeCable *pCable = reinterpret_cast<ProjectExplorer::PeCable*>(pItem->data().toInt());
        if(pCable && !lstSelctedCables.contains(pCable))
            lstSelctedCables.append(pCable);
    }

    return true;
}

bool CableModeWidget::GetSelectedFiber(Utils::ReadOnlyTreeView *pView, QList<ProjectExplorer::PeFiber*> &lstSelctedFibers)
{
    if(!pView)
        return false;

    QStandardItemModel *pModel = qobject_cast<QStandardItemModel*>(pView->model());
    if(!pModel)
        return false;

    lstSelctedFibers.clear();
    foreach(const QModelIndex &index, pView->selectionModel()->selectedRows())
    {
        QStandardItem *pItem = pModel->itemFromIndex(index);
        if(!pItem->parent())
            return false;

        ProjectExplorer::PeFiber *pFiber = reinterpret_cast<ProjectExplorer::PeFiber*>(pItem->data().toInt());
        if(pFiber && !lstSelctedFibers.contains(pFiber))
            lstSelctedFibers.append(pFiber);
    }

    return true;
}

bool CableModeWidget::GetSelectedOdf(QList<ProjectExplorer::PeDevice*> &lstSelctedOdfs)
{
    lstSelctedOdfs.clear();
    foreach(const QModelIndex &index, m_pViewOdf->selectionModel()->selectedRows())
    {
        QStandardItem *pItem = m_pModelOdf->itemFromIndex(index);
        if(pItem->parent())
            return false;

        ProjectExplorer::PeDevice *pOdf = reinterpret_cast<ProjectExplorer::PeDevice*>(pItem->data().toInt());
        if(pOdf && !lstSelctedOdfs.contains(pOdf))
            lstSelctedOdfs.append(pOdf);
    }

    return true;
}

bool CableModeWidget::GetSelectedLayer(QList<ProjectExplorer::PeBoard*> &lstSelctedLayers)
{
    lstSelctedLayers.clear();
    foreach(const QModelIndex &index, m_pViewOdf->selectionModel()->selectedRows())
    {
        QStandardItem *pItem = m_pModelOdf->itemFromIndex(index);
        if(!pItem->parent() || !pItem->hasChildren())
            return false;

        ProjectExplorer::PeBoard *pBoard = reinterpret_cast<ProjectExplorer::PeBoard*>(pItem->data().toInt());
        if(pBoard && !lstSelctedLayers.contains(pBoard))
            lstSelctedLayers.append(pBoard);
    }

    return true;
}

void CableModeWidget::UpdateActions()
{
    m_pActionSettings->setEnabled(m_pCubicle);
    m_pActionRefresh->setEnabled(m_pCubicle);
    m_pActionClear->setEnabled(m_pCubicle);
    m_pActionNewOdf->setEnabled(false);
    m_pActionNewOdfLayer->setEnabled(false);
    m_pActionDelete->setEnabled(false);
    m_pActionProperty->setEnabled(false);
    m_pActionMerge->setEnabled(false);
    m_pActionSplit->setEnabled(false);

    const int iCurrentTab = m_pTabBar->currentIndex();
    if(iCurrentTab == 0) // Optical
    {
        QList<ProjectExplorer::PeCable*> lstSelectedCables;
        if(GetSelectedCable(m_pViewOptical, lstSelectedCables))
        {
            m_pActionDelete->setEnabled(lstSelectedCables.size() == 1);
            m_pActionProperty->setEnabled(lstSelectedCables.size() == 1);
            m_pActionMerge->setEnabled(lstSelectedCables.size() > 1);
        }

        QList<ProjectExplorer::PeFiber*> lstSelectedFibers;
        if(GetSelectedFiber(m_pViewOptical, lstSelectedFibers))
        {
            m_pActionSplit->setEnabled(lstSelectedFibers.size() > 0);
        }
    }
    else if(iCurrentTab == 1) // Tail
    {
        QList<ProjectExplorer::PeCable*> lstSelectedCables;
        if(GetSelectedCable(m_pViewTail, lstSelectedCables))
        {
            m_pActionDelete->setEnabled(lstSelectedCables.size() == 1);
            m_pActionProperty->setEnabled(lstSelectedCables.size() == 1);
            m_pActionMerge->setEnabled(lstSelectedCables.size() > 1);
        }

        QList<ProjectExplorer::PeFiber*> lstSelectedFibers;
        if(GetSelectedFiber(m_pViewTail, lstSelectedFibers))
        {
            m_pActionSplit->setEnabled(lstSelectedFibers.size() > 0);
        }
    }
    else if(iCurrentTab == 2) // Jump
    {
        QList<ProjectExplorer::PeCable*> lstSelectedCables;
        if(GetSelectedCable(m_pViewJump, lstSelectedCables))
            m_pActionProperty->setEnabled(lstSelectedCables.size() == 1);
    }
    else if(iCurrentTab == 3) // ODF
    {
        m_pActionNewOdf->setEnabled(true);

        QList<ProjectExplorer::PeDevice*> lstSelectedOdfs;
        if(GetSelectedOdf(lstSelectedOdfs))
        {
            m_pActionNewOdfLayer->setEnabled(lstSelectedOdfs.size() == 1);
            m_pActionDelete->setEnabled(lstSelectedOdfs.size() == 1);
            m_pActionProperty->setEnabled(lstSelectedOdfs.size() == 1);
        }
        else
        {
            QList<ProjectExplorer::PeBoard*> lstSelectedLayers;
            if(GetSelectedLayer(lstSelectedLayers))
            {
                m_pActionDelete->setEnabled(lstSelectedLayers.size() == 1);
                m_pActionProperty->setEnabled(lstSelectedLayers.size() == 1);
            }
        }
    }
}

void CableModeWidget::SlotCurrentTabChanged(int iCurrentIndex)
{
    Q_UNUSED(iCurrentIndex)

    UpdateActions();
}

void CableModeWidget::SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    UpdateActions();
}

void CableModeWidget::SlotActionSettings()
{
    if(!m_pCubicle)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pCubicle->GetProjectVersion();
    if(!pProjectVersion)
        return;

    PhyNetworkCableSettingsDlg dlg(this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    CableGenerator::Instance()->SaveSettings(pProjectVersion);
}

void CableModeWidget::SlotActionRefresh()
{
    if(!m_pCubicle)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pCubicle->GetProjectVersion();
    if(!pProjectVersion)
        return;

    if(QMessageBox::question(this,
                             tr("Refresh Confirmation"),
                             tr("Are you sure you want to refresh all cables?"),
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No) == QMessageBox::Yes)
    {
        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        if(CableGenerator::Instance()->Generator(pProjectVersion))
            pProjectVersion->UpdateModifyTime();

        SetProjectObject(m_pCubicle);
    }
}

void CableModeWidget::SlotActionClear()
{
    if(!m_pCubicle)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pCubicle->GetProjectVersion();
    if(!pProjectVersion)
        return;

    if(!pProjectVersion->ValidateVersionPwd(this))
        return;

    if(QMessageBox::question(this,
                             tr("Clear Confirmation"),
                             tr("Are you sure you want to clear all cables?"),
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No) == QMessageBox::Yes)
    {
        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        if(CableGenerator::Instance()->Clear(pProjectVersion))
            pProjectVersion->UpdateModifyTime();

        SetProjectObject(m_pCubicle);
    }
}

void CableModeWidget::SlotActionNewOdf()
{
    if(m_pTabBar->currentIndex() != 3)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = m_pCubicle->GetProjectVersion();
    if(!pProjectVersion)
        return;

    ProjectExplorer::PeDevice odf(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    odf.SetParentCubicle(m_pCubicle);
    odf.SetDeviceType(ProjectExplorer::PeDevice::dtODF);
    odf.SetSet("A");

    PropertyDlgOdf dlg(&odf, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Utils::WaitCursor waitcursor;
    Q_UNUSED(waitcursor)

    if(pProjectVersion->DbCreateObject(odf, true))
    {
        pProjectVersion->CreateObject(odf);
        SetProjectObject(m_pCubicle);
    }
}

void CableModeWidget::SlotActionNewOdfLayer()
{
    if(m_pTabBar->currentIndex() != 3)
        return;

    QList<ProjectExplorer::PeDevice*> lstSelectedOdfs;
    if(!GetSelectedOdf(lstSelectedOdfs) || lstSelectedOdfs.size() != 1)
        return;

    ProjectExplorer::PeDevice *pOdf = lstSelectedOdfs.first();
    ProjectExplorer::PeProjectVersion *pProjectVersion = pOdf->GetProjectVersion();
    if(!pProjectVersion)
        return;

    ProjectExplorer::PeBoard layer(pProjectVersion, ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    layer.SetParentDevice(pOdf);

    PropertyDlgOdfLayer dlg(&layer, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Utils::WaitCursor waitcursor;
    Q_UNUSED(waitcursor)

    if(CableGenerator::Instance()->CreateOdfLayer(layer, dlg.GetPortNumber(), dlg.GetFiberPlug()))
    {
        if(CableGenerator::Instance()->AdjustOdfLayerPortNumber(layer.GetParentDevice(), dlg.GetPortNumber()))
            Refresh();
    }
}

void CableModeWidget::SlotActionDelete()
{
    const int iCurrentTab = m_pTabBar->currentIndex();
    if(iCurrentTab == 0 || iCurrentTab == 1)
    {
        Utils::ReadOnlyTreeView *pView = (iCurrentTab == 0 ? m_pViewOptical : m_pViewTail);
        QList<ProjectExplorer::PeCable*> lstSelectedCables;
        if(!GetSelectedCable(pView, lstSelectedCables) || lstSelectedCables.size() != 1)
            return;

        ProjectExplorer::PeCable *pCable = lstSelectedCables.first();
        ProjectExplorer::PeProjectVersion *pProjectVersion = pCable->GetProjectVersion();
        if(!pProjectVersion)
            return;

        bool bAllReserve = true;
        foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
        {
            if(!pFiber->GetReserve())
            {
                bAllReserve = false;
                break;
            }
        }

        if(!bAllReserve)
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Can NOT delete cable '%1' since it contains the non-reserved fibers.").arg(pCable->GetDisplayName()));
            return;
        }

        if(QMessageBox::question(this,
                                 tr("Delete Confirmation"),
                                 tr("Are you sure you want to delete cable '%1' ?").arg(pCable->GetDisplayName()),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No) == QMessageBox::No)
        {
            return;
        }

        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        if(pProjectVersion->DbDeleteObject(pCable->GetObjectType(), pCable->GetId(), true))
        {
            pProjectVersion->DeleteObject(pCable);
            Refresh();
        }
    }
    else if(iCurrentTab == 3)
    {
        QList<ProjectExplorer::PeDevice*> lstSelectedOdfs;
        if(GetSelectedOdf(lstSelectedOdfs) || lstSelectedOdfs.size() == 1)
        {
            ProjectExplorer::PeDevice *pOdf = lstSelectedOdfs.first();
            ProjectExplorer::PeProjectVersion *pProjectVersion = pOdf->GetProjectVersion();
            if(!pProjectVersion)
                return;

            bool bOdfUsed = false;
            foreach(ProjectExplorer::PeBoard *pLayer, pOdf->GetChildBoards())
            {
                if(pLayer->IsBoardUsed())
                {
                    bOdfUsed = true;
                    break;
                }
            }

            if(bOdfUsed)
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("Can NOT delete ODF '%1' since it contains the used layer.").arg(pOdf->GetDisplayName()));
                return;
            }

            if(QMessageBox::question(this,
                                     tr("Delete Confirmation"),
                                     tr("Are you sure you want to delete ODF '%1' ?").arg(pOdf->GetDisplayName()),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No) == QMessageBox::No)
            {
                return;
            }

            Utils::WaitCursor waitcursor;
            Q_UNUSED(waitcursor)

            if(pProjectVersion->DbDeleteObject(pOdf->GetObjectType(), pOdf->GetId(), true))
            {
                pProjectVersion->DeleteObject(pOdf);
                Refresh();
            }
        }
        else
        {
            QList<ProjectExplorer::PeBoard*> lstSelectedLayers;
            if(!GetSelectedLayer(lstSelectedLayers) || lstSelectedLayers.size() != 1)
                return;

            ProjectExplorer::PeBoard *pLayer = lstSelectedLayers.first();
            ProjectExplorer::PeProjectVersion *pProjectVersion = pLayer->GetProjectVersion();
            if(!pProjectVersion)
                return;

            if(pLayer->IsBoardUsed())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("Can NOT delete ODF layer '%1' since it contains the used port.").arg(pLayer->GetDisplayName()));
                return;
            }

            if(QMessageBox::question(this,
                                     tr("Delete Confirmation"),
                                     tr("Are you sure you want to delete ODF layer '%1' ?").arg(pLayer->GetDisplayName()),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No) == QMessageBox::No)
            {
                return;
            }

            Utils::WaitCursor waitcursor;
            Q_UNUSED(waitcursor)

            if(pProjectVersion->DbDeleteObject(pLayer->GetObjectType(), pLayer->GetId(), true))
            {
                pProjectVersion->DeleteObject(pLayer);
                Refresh();
            }
        }
    }
}

void CableModeWidget::SlotActionProperty()
{
    const int iCurrentTab = m_pTabBar->currentIndex();
    if(iCurrentTab == 0 || iCurrentTab == 1 || iCurrentTab == 2)
    {
        Utils::ReadOnlyTreeView *pView = 0;
        if(iCurrentTab == 0) // Optical
            pView = m_pViewOptical;
        else if(iCurrentTab == 1) // Tail
            pView = m_pViewTail;
        else if(iCurrentTab == 2) // Jump
            pView = m_pViewJump;

        if(!pView)
            return;

        QList<ProjectExplorer::PeCable*> lstSelectedCables;
        if(!GetSelectedCable(pView, lstSelectedCables) || lstSelectedCables.size() != 1)
            return;

        ProjectExplorer::PeCable *pCable = lstSelectedCables.first();
        ProjectExplorer::PeProjectVersion *pProjectVersion = pCable->GetProjectVersion();
        if(!pCable || !pProjectVersion)
            return;

        ProjectExplorer::PeCable cable(*pCable);
        PropertyDlgCable dlg(&cable, false, this);
        if(dlg.exec() != QDialog::Accepted)
            return;

        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        if(pProjectVersion->DbUpdateObject(cable, true))
        {
            pProjectVersion->UpdateObject(cable);

            const int iNewFiberNumber = dlg.GetFiberNumber();
            if(m_pTabBar->currentIndex() == 0) // Optical
                CableGenerator::Instance()->AdjustOpticalCableSize(pCable, iNewFiberNumber);
            else if(m_pTabBar->currentIndex() == 1) // Tail
                CableGenerator::Instance()->AdjustTailCableSize(pCable, iNewFiberNumber);

            SetProjectObject(m_pCubicle);
        }
    }
    else if(iCurrentTab == 3)
    {
        QList<ProjectExplorer::PeDevice*> lstSelectedOdfs;
        if(GetSelectedOdf(lstSelectedOdfs) || lstSelectedOdfs.size() == 1)
        {
            ProjectExplorer::PeDevice *pOdf = lstSelectedOdfs.first();
            ProjectExplorer::PeProjectVersion *pProjectVersion = pOdf->GetProjectVersion();
            if(!pOdf || !pProjectVersion)
                return;

            ProjectExplorer::PeDevice odf(*pOdf);
            PropertyDlgOdf dlg(&odf, this);
            if(dlg.exec() != QDialog::Accepted)
                return;

            Utils::WaitCursor waitcursor;
            Q_UNUSED(waitcursor)

            if(pProjectVersion->DbUpdateObject(odf, true))
            {
                pProjectVersion->UpdateObject(odf);
                SetProjectObject(m_pCubicle);
            }
        }
        else
        {
            QList<ProjectExplorer::PeBoard*> lstSelectedLayers;
            if(!GetSelectedLayer(lstSelectedLayers) || lstSelectedLayers.size() != 1)
                return;

            ProjectExplorer::PeBoard *pLayer = lstSelectedLayers.first();
            ProjectExplorer::PeProjectVersion *pProjectVersion = pLayer->GetProjectVersion();
            if(!pLayer || !pProjectVersion)
                return;

            ProjectExplorer::PeBoard layer(*pLayer);
            PropertyDlgOdfLayer dlg(&layer, this);
            if(dlg.exec() != QDialog::Accepted)
                return;

            Utils::WaitCursor waitcursor;
            Q_UNUSED(waitcursor)

            if(CableGenerator::Instance()->UpdateOdfLayer(layer, dlg.GetFiberPlug()))
            {
                if(CableGenerator::Instance()->AdjustOdfLayerPortNumber(layer.GetParentDevice(), dlg.GetPortNumber()))
                    Refresh();
            }
        }
    }
}

void CableModeWidget::SlotActionMerge()
{
    Utils::ReadOnlyTreeView *pView = 0;
    int iMaxFiberNumber = 0;
    if(m_pTabBar->currentIndex() == 0) // Optical
    {
        pView = m_pViewOptical;
        iMaxFiberNumber = CableGenerator::Instance()->GetOpticalFiberNumbers().last();
    }
    else if(m_pTabBar->currentIndex() == 1) // Tail
    {
        pView = m_pViewTail;
        iMaxFiberNumber = CableGenerator::Instance()->GetTailFiberNumbers().last();
    }

    if(!pView)
        return;

    QList<ProjectExplorer::PeCable*> lstSelectedCables;
    if(!GetSelectedCable(pView, lstSelectedCables) || lstSelectedCables.size() < 2)
        return;

    QString strSetOfCable = lstSelectedCables.first()->GetNameSet();
    ProjectExplorer::PeCubicle *pCubicle1 = lstSelectedCables.first()->GetCubicle1();
    ProjectExplorer::PeCubicle *pCubicle2 = lstSelectedCables.first()->GetCubicle2();
    for(int i = 1; i < lstSelectedCables.size(); i++)
    {
        ProjectExplorer::PeCable *pCable = lstSelectedCables.at(i);
        if(strSetOfCable != pCable->GetNameSet())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Can NOT merge selected cables since the sets of cable are not matched."));
            return;
        }


        if(!((pCable->GetCubicle1() == pCubicle1 && pCable->GetCubicle2() == pCubicle2) ||
             (pCable->GetCubicle1() == pCubicle2 && pCable->GetCubicle2() == pCubicle1)))
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Can NOT merge selected cables since the cubicle of cable are not matched."));
            return;
        }
    }

    int iTotalRealFiberNumber = 0;
    foreach(ProjectExplorer::PeCable *pCable, lstSelectedCables)
    {
        foreach(ProjectExplorer::PeFiber *pFiber, pCable->GetChildFibers())
        {
            if(!pFiber->GetReserve())
                iTotalRealFiberNumber++;
        }
    }
    if(iTotalRealFiberNumber > iMaxFiberNumber)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Can NOT merge selected cables since the fiber number of merged cable exceeds the maximum fiber number of available cable."));
        return;
    }

    qSort(lstSelectedCables.begin(), lstSelectedCables.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    QStringList lstItems;
    foreach(ProjectExplorer::PeCable *pCable, lstSelectedCables)
        lstItems.append(pCable->GetDisplayName());

    bool bOk;
    QString strValue = QInputDialog::getItem(this,
                                             tr("Select target cable"),
                                             tr("Target Cable:"),
                                             lstItems,
                                             0,
                                             false,
                                             &bOk);
    if(!bOk || strValue.isEmpty())
        return;

    int iTargetCableIndex = -1;
    for(int i = 0;i < lstSelectedCables.size(); i++)
    {
        if(lstSelectedCables.at(i)->GetDisplayName() == strValue)
        {
            iTargetCableIndex = i;
            break;
        }
    }
    if(iTargetCableIndex < 0)
        return;
    lstSelectedCables.prepend(lstSelectedCables.takeAt(iTargetCableIndex));

    Utils::WaitCursor waitcursor;
    Q_UNUSED(waitcursor)

    if(m_pTabBar->currentIndex() == 0) // Optical
    {
        CableGenerator::Instance()->MergeOpticalCables(lstSelectedCables);
        SetProjectObject(m_pCubicle);
    }
    else if(m_pTabBar->currentIndex() == 1) // Tail
    {
        CableGenerator::Instance()->MergeTailCables(lstSelectedCables);
        SetProjectObject(m_pCubicle);
    }
}

void CableModeWidget::SlotActionSplit()
{
    Utils::ReadOnlyTreeView *pView = 0;
    int iMaxFiberNumber = 0;
    if(m_pTabBar->currentIndex() == 0) // Optical
    {
        pView = m_pViewOptical;
        iMaxFiberNumber = CableGenerator::Instance()->GetOpticalFiberNumbers().last();
    }
    else if(m_pTabBar->currentIndex() == 1) // Tail
    {
        pView = m_pViewTail;
        iMaxFiberNumber = CableGenerator::Instance()->GetTailFiberNumbers().last();
    }

    if(!pView)
        return;

    QList<ProjectExplorer::PeFiber*> lstSelectedFibers;
    if(!GetSelectedFiber(pView, lstSelectedFibers) || lstSelectedFibers.isEmpty())
        return;

    ProjectExplorer::PeCable *pCable = lstSelectedFibers.first()->GetParentCable();
    foreach(ProjectExplorer::PeFiber *pFiber, lstSelectedFibers)
    {
        if(pFiber->GetReserve())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Can NOT split reserved fibers."));
            return;
        }
        if(pCable != pFiber->GetParentCable())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Can NOT split fibers belonging to different cables."));
            return;
        }
    }

    if(lstSelectedFibers.size() > iMaxFiberNumber)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Can NOT splite selected fibers since the number of splitted fibers exceeds the maximum fiber number of available cable."));
        return;
    }

    Utils::WaitCursor waitcursor;
    Q_UNUSED(waitcursor)

    if(m_pTabBar->currentIndex() == 0) // Optical
    {
        CableGenerator::Instance()->SplitOpticalCable(lstSelectedFibers);
        SetProjectObject(m_pCubicle);
    }
    else if(m_pTabBar->currentIndex() == 1) // Tail
    {
        CableGenerator::Instance()->SplitTailCable(lstSelectedFibers);
        SetProjectObject(m_pCubicle);
    }
}

void CableModeWidget::SlotActionRenameCable()
{
    RenameCableDlg dlg(this);
    dlg.exec();
}
