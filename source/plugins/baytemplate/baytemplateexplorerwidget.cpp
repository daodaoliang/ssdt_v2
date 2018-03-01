#include <QApplication>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QFileIconProvider>
#include <QAction>
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>

#include "utils/waitcursor.h"
#include "utils/readonlyview.h"
#include "projectexplorer/basemanager.h"
#include "projectexplorer/pbtpbay.h"

#include "baytemplateexplorerwidget.h"
#include "baytemplateconfigdlg.h"
#include "propertydlgtpbay.h"

using namespace BayTemplate::Internal;

BayTemplateExplorerWidget::BayTemplateExplorerWidget(QWidget *pParent) : QWidget(pParent), m_pCurrentItem(0)
{
    m_pModel = new QStandardItemModel(0, 1, this);
    m_pView = new Utils::ReadOnlyTreeView(this);
    m_pView->setStyleSheet("QTreeView {background-color: rgba(255, 255, 255, 240); }");
    m_pView->setFrameStyle(QFrame::NoFrame);
    m_pView->setIndentation(m_pView->indentation() * 9/10);
    m_pView->setUniformRowHeights(true);
    m_pView->setTextElideMode(Qt::ElideNone);
    m_pView->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_pView->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pView->setDragEnabled(true);
    m_pView->setHeaderHidden(true);
#if QT_VERSION >= 0x050000
   m_pView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_pView->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    m_pView->header()->setStretchLastSection(false);
    m_pView->setModel(m_pModel);

    QAction *pActionSeparator1 = new QAction(this);
    pActionSeparator1->setSeparator(true);
    QAction *pActionSeparator2 = new QAction(this);
    pActionSeparator2->setSeparator(true);
    QAction *pActionExpandAll = new QAction(tr("E&xpand All"), this);
    QAction *pActionCollapseAll = new QAction(tr("Co&llapse All"), this);

    m_pActionNew = new QAction(QIcon(":/devlibrary/images/oper_add.png"), tr("New..."), this);
    m_pActionNew->setEnabled(true);
    m_pActionDelete = new QAction(QIcon(":/devlibrary/images/oper_remove.png"), tr("&Delete"), this);
    m_pActionDelete->setEnabled(false);
    m_pActionProperty = new QAction(QIcon(":/devlibrary/images/oper_edit.png"), tr("&Property..."), this);
    m_pActionProperty->setEnabled(false);
    m_pActionConfig = new QAction(tr("&Confg..."), this);
    m_pActionConfig->setEnabled(false);

    m_pView->addAction(m_pActionNew);
    m_pView->addAction(m_pActionDelete);
    m_pView->addAction(m_pActionProperty);
    m_pView->addAction(pActionSeparator1);
    m_pView->addAction(m_pActionConfig);
    m_pView->addAction(pActionSeparator2);
    m_pView->addAction(pActionExpandAll);
    m_pView->addAction(pActionCollapseAll);

    QVBoxLayout *pVBoxLayout = new QVBoxLayout(this);
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->addWidget(m_pView);

    connect(ProjectExplorer::BaseManager::Instance(), SIGNAL(sigInitialized()),
            this, SLOT(SlotBaseManagerInitialized()));
    connect(ProjectExplorer::BaseManager::Instance(), SIGNAL(sigUninitialized()),
            this, SLOT(SlotBaseManagerUninitialized()));
    connect(ProjectExplorer::BaseManager::Instance(), SIGNAL(sigObjectCreated(ProjectExplorer::PbBaseObject*)),
            this, SLOT(SlotBaseObjectCreated(ProjectExplorer::PbBaseObject*)));
    connect(ProjectExplorer::BaseManager::Instance(), SIGNAL(sigObjectPropertyChanged(ProjectExplorer::PbBaseObject*)),
            this, SLOT(SlotBaseObjectPropertyChanged(ProjectExplorer::PbBaseObject*)));
    connect(ProjectExplorer::BaseManager::Instance(), SIGNAL(sigObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject*)),
            this, SLOT(SlotBaseObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject*)));
    connect(m_pView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(m_pView, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewDoubleClicked(const QModelIndex&)));
    connect(m_pActionNew, SIGNAL(triggered()),
            this, SLOT(SlotActionNew()));
    connect(m_pActionDelete, SIGNAL(triggered()),
            this, SLOT(SlotActionDelete()));
    connect(m_pActionProperty, SIGNAL(triggered()),
            this, SLOT(SlotActionProperty()));
    connect(m_pActionConfig, SIGNAL(triggered()),
            this, SLOT(SlotActionConfig()));
    connect(pActionExpandAll, SIGNAL(triggered()),
            m_pView, SLOT(expandAll()));
    connect(pActionCollapseAll, SIGNAL(triggered()),
            m_pView, SLOT(collapseAll()));

    SlotBaseManagerInitialized();
}

BayTemplateExplorerWidget::~BayTemplateExplorerWidget()
{
}

QStandardItem* BayTemplateExplorerWidget::FindChildItem(QStandardItem *pItemParent, const QString &strText)
{
    if(pItemParent)
    {
        for(int i = 0; i < pItemParent->rowCount(); i++)
        {
            if(pItemParent->child(i)->text() == strText)
                return pItemParent->child(i);
        }
    }
    else
    {
        for(int i = 0; i < m_pModel->rowCount(); i++)
        {
            if(m_pModel->item(i)->text() == strText)
                return m_pModel->item(i);
        }
    }

    return 0;
}

void BayTemplateExplorerWidget::InsertChildItem(QStandardItem *pItemParent, QStandardItem *pItemChild)
{
    if(!pItemChild)
        return;

    if(pItemParent)
    {
        int iPos = pItemParent->rowCount();
        for(int i = 0; i < pItemParent->rowCount(); i++)
        {
            if(pItemChild->text() < pItemParent->child(i)->text())
            {
                iPos = i;
                break;
            }
        }

        pItemParent->insertRow(iPos, pItemChild);
    }
    else
    {
        int iPos = m_pModel->rowCount();
        for(int i = 0; i < m_pModel->rowCount(); i++)
        {
            if(pItemChild->text() < m_pModel->item(i)->text())
            {
                iPos = i;
                break;
            }
        }

        m_pModel->insertRow(iPos, pItemChild);
    }
}

void BayTemplateExplorerWidget::SlotBaseManagerInitialized()
{
    m_pModel->clear();

    foreach(ProjectExplorer::PbTpBay *pTpBay, ProjectExplorer::BaseManager::Instance()->GetAllTpBays())
        SlotBaseObjectCreated(pTpBay);

    m_pView->expandAll();
}

void BayTemplateExplorerWidget::SlotBaseManagerUninitialized()
{
    m_pModel->clear();
}

void BayTemplateExplorerWidget::SlotBaseObjectCreated(ProjectExplorer::PbBaseObject *pBaseObject)
{
    ProjectExplorer::PbTpBay *pTpBay = qobject_cast<ProjectExplorer::PbTpBay*>(pBaseObject);
    if(!pTpBay)
        return;

    QFileIconProvider FileIconProvider;

    const QString strVLevel = QString("%1 kV").arg(pTpBay->GetVLevel());
    QStandardItem *pItemVLevel = FindChildItem(0, strVLevel);
    if(!pItemVLevel)
    {
        pItemVLevel = new QStandardItem(FileIconProvider.icon(QFileIconProvider::Folder), strVLevel);
        InsertChildItem(0, pItemVLevel);
    }

    QStandardItem *pItemBay = new QStandardItem(pTpBay->GetDisplayIcon(), pTpBay->GetDisplayName());
    pItemBay->setData(reinterpret_cast<int>(pTpBay));
    pItemBay->setDragEnabled(true);
    InsertChildItem(pItemVLevel, pItemBay);
    m_mapBayToItem.insert(pTpBay, pItemBay);

    m_pView->expand(pItemVLevel->index());
}

void BayTemplateExplorerWidget::SlotBaseObjectPropertyChanged(ProjectExplorer::PbBaseObject *pBaseObject)
{
    ProjectExplorer::PbTpBay *pTpBay = qobject_cast<ProjectExplorer::PbTpBay*>(pBaseObject);
    if(!pTpBay)
        return;

    SlotBaseObjectAboutToBeDeleted(pTpBay);
    SlotBaseObjectCreated(pTpBay);
}

void BayTemplateExplorerWidget::SlotBaseObjectAboutToBeDeleted(ProjectExplorer::PbBaseObject *pBaseObject)
{
    ProjectExplorer::PbTpBay *pTpBay = qobject_cast<ProjectExplorer::PbTpBay*>(pBaseObject);
    if(!pTpBay)
        return;

    if(QStandardItem *pItemBay = m_mapBayToItem.value(pTpBay, 0))
    {
        if(QStandardItem *pItemVLevel = pItemBay->parent())
        {
            pItemVLevel->removeRow(pItemBay->row());
            m_mapBayToItem.remove(pTpBay);

            if(!pItemVLevel->hasChildren())
                m_pModel->removeRow(pItemVLevel->row());
        }
    }
}

void BayTemplateExplorerWidget::SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    m_pActionDelete->setEnabled(false);
    m_pActionProperty->setEnabled(false);
    m_pCurrentItem = 0;

    QItemSelectionModel *pSelectionModel = m_pView->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
    if(lstSelectedIndex.size() == 1)
    {
        m_pCurrentItem = m_pModel->itemFromIndex(lstSelectedIndex.first());
        if(!m_pCurrentItem)
            return;

        m_pActionDelete->setEnabled(!m_pCurrentItem->hasChildren());
        m_pActionProperty->setEnabled(!m_pCurrentItem->hasChildren());
        m_pActionConfig->setEnabled(!m_pCurrentItem->hasChildren());
    }
}

void BayTemplateExplorerWidget::SlotViewDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    SlotActionConfig();
}

void BayTemplateExplorerWidget::SlotActionNew()
{
    ProjectExplorer::PbTpBay TpBay;
    PropertyDlgTpBay dlg(&TpBay, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(ProjectExplorer::BaseManager::Instance()->DbCreateObject(TpBay))
        ProjectExplorer::BaseManager::Instance()->CreateObject(TpBay);
}

void BayTemplateExplorerWidget::SlotActionDelete()
{
    if(!m_pCurrentItem)
        return;

    ProjectExplorer::PbTpBay *pTpBay = reinterpret_cast<ProjectExplorer::PbTpBay*>(m_pCurrentItem->data().toInt());
    if(!pTpBay)
        return;

    if(QMessageBox::question(this,
                             tr("Delete Confirmation"),
                             tr("Are you sure you want to delete %1 '%2'?").arg(pTpBay->GetObjectTypeName(pTpBay->GetObjectType())).arg(pTpBay->GetDisplayName()),
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No) == QMessageBox::Yes)
    {
        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        if(ProjectExplorer::BaseManager::Instance()->DbDeleteObject(pTpBay->GetObjectType(), pTpBay->GetId()))
            ProjectExplorer::BaseManager::Instance()->DeleteObject(pTpBay);
    }
}

void BayTemplateExplorerWidget::SlotActionProperty()
{
    if(!m_pCurrentItem)
        return;

    ProjectExplorer::PbTpBay *pTpBay = reinterpret_cast<ProjectExplorer::PbTpBay*>(m_pCurrentItem->data().toInt());
    if(!pTpBay)
        return;

    ProjectExplorer::PbTpBay TpBay(*pTpBay);
    PropertyDlgTpBay dlg(&TpBay, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(ProjectExplorer::BaseManager::Instance()->DbUpdateObject(TpBay))
        ProjectExplorer::BaseManager::Instance()->UpdateObject(TpBay);
}

void BayTemplateExplorerWidget::SlotActionConfig()
{
    if(!m_pCurrentItem)
        return;

    ProjectExplorer::PbTpBay *pTpBay = reinterpret_cast<ProjectExplorer::PbTpBay*>(m_pCurrentItem->data().toInt());
    if(!pTpBay)
        return;

    BayTemplateConfigDlg dlg(pTpBay, this);
    dlg.exec();
}
