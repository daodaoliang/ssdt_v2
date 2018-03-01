#include <QApplication>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QTabBar>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QAction>
#include <QMessageBox>
#include <QSettings>
#include <QToolButton>
#include <QFileDialog>
#include <QHeaderView>

#include "utils/waitcursor.h"
#include "utils/readonlyview.h"
#include "projectexplorer/pbtpdevice.h"
#include "projectexplorer/pbtpbay.h"
#include "projectexplorer/pbtpport.h"

#include "propertydlgtpdevice.h"
#include "propertydlgtpport.h"

using namespace BayTemplate::Internal;

PropertyDlgTpDevice::PropertyDlgTpDevice(ProjectExplorer::PbTpDevice *pTpDevice, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pTpDevice(pTpDevice)
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    m_pActionNewPort = new QAction(QIcon(":/devlibrary/images/oper_add.png"), tr("New %1...").arg(ProjectExplorer::PbBaseObject::GetObjectTypeName(ProjectExplorer::PbBaseObject::botLibPort)), this);
    connect(m_pActionNewPort, SIGNAL(triggered()), this, SLOT(SlotActionNewPort()));
    m_pActionDelete = new QAction(QIcon(":/devlibrary/images/oper_remove.png"), tr("&Delete"), this);
    connect(m_pActionDelete, SIGNAL(triggered()), this, SLOT(SlotActionDelete()));
    m_pActionProperty = new QAction(QIcon(":/devlibrary/images/oper_edit.png"), tr("&Property..."), this);
    connect(m_pActionProperty, SIGNAL(triggered()), this, SLOT(SlotActionProperty()));
    QAction *pActionSeparator = new QAction(this);
    pActionSeparator->setSeparator(true);

    m_pLineEditName = new QLineEdit(this);
    m_pLineEditName->setObjectName(tr("Name"));

    QGroupBox *pGroupBoxGeneral = new QGroupBox(this);
    QGridLayout *pGridLayoutGeneral = new QGridLayout(pGroupBoxGeneral);
    pGridLayoutGeneral->addWidget(new QLabel(m_pLineEditName->objectName() + ":", this), 0, 0);
    pGridLayoutGeneral->addWidget(m_pLineEditName, 0, 1);

    m_pModelPort = new QStandardItemModel(0, 3, this);
    m_pModelPort->setHeaderData(0, Qt::Horizontal, tr("Port Name"));
    m_pModelPort->setHeaderData(1, Qt::Horizontal, tr("Port Index"));
    m_pModelPort->setHeaderData(2, Qt::Horizontal, tr("Port Direction"));

    m_pViewPort = new Utils::ReadOnlyTableView(this);
    m_pViewPort->setShowGrid(false);
    m_pViewPort->setAlternatingRowColors(true);
    m_pViewPort->setSortingEnabled(true);
    m_pViewPort->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewPort->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_pViewPort->horizontalHeader()->setHighlightSections(false);
    m_pViewPort->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_pViewPort->horizontalHeader()->setStretchLastSection(true);
    m_pViewPort->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewPort->setShowGrid(false);
    m_pViewPort->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewPort->setAlternatingRowColors(true);
    m_pViewPort->addAction(m_pActionNewPort);
    m_pViewPort->addAction(pActionSeparator);
    m_pViewPort->addAction(m_pActionDelete);
    m_pViewPort->addAction(m_pActionProperty);
    m_pViewPort->setModel(m_pModelPort);
    m_pViewPort->setColumnWidth(0, 150);
    m_pViewPort->setColumnWidth(1, 150);
    m_pViewPort->setColumnWidth(2, 150);
    connect(m_pViewPort->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(SlotViewPortSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(m_pViewPort, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewPortDoubleClicked(const QModelIndex&)));

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBoxGeneral);
    pLayout->addSpacing(5);
    pLayout->addWidget(m_pViewPort);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    if(m_pTpDevice->GetId() == ProjectExplorer::PbBaseObject::m_iInvalidObjectId)
        SetWindowTitle(tr("New %1").arg(m_pTpDevice->GetDeviceTypeName(m_pTpDevice->GetDeviceType())));
    else
        SetWindowTitle(tr("%1 Property").arg(m_pTpDevice->GetDeviceTypeName(m_pTpDevice->GetDeviceType())));

    UpdateData(false);
}

bool PropertyDlgTpDevice::UpdateData(bool bSaveAndValidate)
{
    if(bSaveAndValidate)
    {
        // Type
        const QString strName = m_pLineEditName->text().trimmed();
        if(strName.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pLineEditName->objectName()));

            m_pLineEditName->setFocus();
            return false;
        }

        foreach(ProjectExplorer::PbTpDevice *pTpDevice, m_pTpDevice->GetParentBay()->GetChildDevices())
        {
            if(pTpDevice->GetId() == m_pTpDevice->GetId())
                continue;

            if(strName == pTpDevice->GetName())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The field '%1' can NOT be duplicate, please input a valid value.").arg(m_pLineEditName->objectName()));

                return false;
            }
        }

        m_pTpDevice->SetName(strName);
    }
    else
    {
        m_pLineEditName->setText(m_pTpDevice->GetName());

        BuildPortData();
    }

    return true;
}

void PropertyDlgTpDevice::BuildPortData()
{
    foreach(ProjectExplorer::PbTpPort *pTpPort, m_pTpDevice->GetChildPorts())
        InsertPortItem(pTpPort);
}

QStandardItem* PropertyDlgTpDevice::InsertPortItem(ProjectExplorer::PbTpPort *pTpPort)
{
    if(!pTpPort)
        return 0;

    QList<QStandardItem*> lstPortItems;

    QStandardItem *pItemPortName = new QStandardItem(pTpPort->GetDisplayIcon(), pTpPort->GetName());
    pItemPortName->setData(reinterpret_cast<int>(pTpPort));
    lstPortItems.append(pItemPortName);

    QStandardItem *pItemPortIndex = new QStandardItem(QString::number(pTpPort->GetGroup()));
    pItemPortIndex->setData(reinterpret_cast<int>(pTpPort));
    lstPortItems.append(pItemPortIndex);

    QStandardItem *pItemPortDirection = new QStandardItem(ProjectExplorer::PbTpPort::GetPortDirectionName(pTpPort->GetPortDirection()));
    pItemPortDirection->setData(reinterpret_cast<int>(pTpPort));
    lstPortItems.append(pItemPortDirection);

    m_pModelPort->appendRow(lstPortItems);
    return pItemPortName;
}

void PropertyDlgTpDevice::UpdatePortItem(QStandardItem *pItemTpPort)
{
    if(!pItemTpPort)
        return;

    ProjectExplorer::PbTpPort *pTpPort = reinterpret_cast<ProjectExplorer::PbTpPort*>(pItemTpPort->data().toInt());
    if(!pTpPort)
        return;

    QStandardItem *pItemPortName = m_pModelPort->item(pItemTpPort->row(), 0);
    pItemPortName->setText(pTpPort->GetName());

    QStandardItem *pItemPortIndex = m_pModelPort->item(pItemTpPort->row(), 1);
    pItemPortIndex->setText(QString::number(pTpPort->GetGroup()));

    QStandardItem *pItemPortDirection = m_pModelPort->item(pItemTpPort->row(), 2);
    pItemPortDirection->setText(ProjectExplorer::PbTpPort::GetPortDirectionName(pTpPort->GetPortDirection()));
}

void PropertyDlgTpDevice::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}

void PropertyDlgTpDevice::SlotViewPortSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    QItemSelectionModel *pSelectionModel = m_pViewPort->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();

    m_pActionNewPort->setEnabled(true);
    m_pActionDelete->setEnabled(lstSelectedIndex.size() == 1);
    m_pActionProperty->setEnabled(lstSelectedIndex.size() == 1);
}

void PropertyDlgTpDevice::SlotViewPortDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    SlotActionProperty();
}

void PropertyDlgTpDevice::SlotActionNewPort()
{
    ProjectExplorer::PbTpPort tpport;
    tpport.SetParentDevice(m_pTpDevice);
    tpport.SetPortDirection(ProjectExplorer::PbTpPort::pdRT);

    PropertyDlgTpPort dlg(&tpport, true, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    const int iNewPortNumber = dlg.GetNewPortNumber();
    int iGroup = tpport.GetGroup();
    QList<int> lstExistGroups;
    foreach(ProjectExplorer::PbTpPort *pPort, m_pTpDevice->GetChildPorts())
    {
        if(!lstExistGroups.contains(pPort->GetGroup()))
            lstExistGroups.append(pPort->GetGroup());
    }

    if(tpport.GetPortDirection() == ProjectExplorer::PbTpPort::pdRT)
    {
        const QStringList lstPortName = tpport.GetName().split(PropertyDlgTpPort::m_strSpliter);
        Q_ASSERT(lstPortName.size() == 2);

        for(int i = 0; i < iNewPortNumber; i++)
        {
            while(lstExistGroups.contains(iGroup))
                iGroup++;
            lstExistGroups.append(iGroup);

            ProjectExplorer::PbTpPort *pTpPortTx = new ProjectExplorer::PbTpPort(tpport);
            pTpPortTx->SetGroup(iGroup);
            pTpPortTx->SetPortDirection(ProjectExplorer::PbTpPort::pdTx);
            pTpPortTx->SetName(iNewPortNumber == 1 ? lstPortName.at(0) : QString("%1%2").arg(lstPortName.at(0)).arg(iGroup, 2, 10, QLatin1Char('0')));
            pTpPortTx->SetParentDevice(m_pTpDevice);
            m_pTpDevice->AddChildPort(pTpPortTx);
            InsertPortItem(pTpPortTx);

            ProjectExplorer::PbTpPort *pTpPortRx = new ProjectExplorer::PbTpPort(tpport);
            pTpPortRx->SetGroup(iGroup);
            pTpPortRx->SetPortDirection(ProjectExplorer::PbTpPort::pdRx);
            pTpPortRx->SetName(iNewPortNumber == 1 ? lstPortName.at(1) : QString("%1%2").arg(lstPortName.at(1)).arg(iGroup, 2, 10, QLatin1Char('0')));
            pTpPortRx->SetParentDevice(m_pTpDevice);
            m_pTpDevice->AddChildPort(pTpPortRx);
            InsertPortItem(pTpPortRx);
        }
    }
    else
    {
        for(int i = 0; i < iNewPortNumber; i++)
        {
            while(lstExistGroups.contains(iGroup))
                iGroup++;
            lstExistGroups.append(iGroup);

            ProjectExplorer::PbTpPort *pTpPort = new ProjectExplorer::PbTpPort(tpport);
            pTpPort->SetGroup(iGroup);
            pTpPort->SetName(iNewPortNumber == 1 ? tpport.GetName() : QString("%1%2").arg(tpport.GetName()).arg(iGroup, 2, 10, QLatin1Char('0')));
            pTpPort->SetParentDevice(m_pTpDevice);
            m_pTpDevice->AddChildPort(pTpPort);
            InsertPortItem(pTpPort);
        }
    }
}

void PropertyDlgTpDevice::SlotActionDelete()
{
    QModelIndexList lstSelectedIndex = m_pViewPort->selectionModel()->selectedRows();
    if(lstSelectedIndex.size() != 1)
        return;

    QStandardItem *pItemTpPort = m_pModelPort->itemFromIndex(lstSelectedIndex.first());
    if(!pItemTpPort)
        return;

    ProjectExplorer::PbTpPort *pTpPort = reinterpret_cast<ProjectExplorer::PbTpPort*>(pItemTpPort->data().toInt());
    if(!pTpPort)
        return;

    ProjectExplorer::PbTpPort *pTpPortPair = pTpPort->GetPairPort();

    m_pTpDevice->RemoveChildPort(pTpPort);
    delete pTpPort;
    m_pModelPort->removeRow(pItemTpPort->row());

    if(pTpPortPair)
    {
        for(int i = 0; i < m_pModelPort->rowCount(); i++)
        {
            if(QStandardItem *pItemTpPortPair = m_pModelPort->item(i, 0))
            {
                if(reinterpret_cast<ProjectExplorer::PbTpPort*>(pItemTpPortPair->data().toInt()) == pTpPortPair)
                {
                    m_pModelPort->removeRow(pItemTpPortPair->row());
                    break;
                }
            }
        }

        m_pTpDevice->RemoveChildPort(pTpPortPair);
        delete pTpPortPair;
    }
}

void PropertyDlgTpDevice::SlotActionProperty()
{
    QModelIndexList lstSelectedIndex = m_pViewPort->selectionModel()->selectedRows();
    if(lstSelectedIndex.size() != 1)
        return;

    QStandardItem *pItemTpPort = m_pModelPort->itemFromIndex(lstSelectedIndex.first());
    if(!pItemTpPort)
        return;

    ProjectExplorer::PbTpPort *pTpPort = reinterpret_cast<ProjectExplorer::PbTpPort*>(pItemTpPort->data().toInt());
    if(!pTpPort)
        return;

    ProjectExplorer::PbTpPort libport(*pTpPort);
    ProjectExplorer::PbTpPort *pTpPortPair = pTpPort->GetPairPort();
    if(pTpPortPair)
    {
        libport.SetPortDirection(ProjectExplorer::PbTpPort::pdRT);
        if(pTpPort->GetPortDirection() == ProjectExplorer::PbTpPort::pdTx)
            libport.SetName(pTpPort->GetName() + PropertyDlgTpPort::m_strSpliter + pTpPortPair->GetName());
        else if(pTpPort->GetPortDirection() == ProjectExplorer::PbTpPort::pdRx)
            libport.SetName(pTpPortPair->GetName() + PropertyDlgTpPort::m_strSpliter + pTpPort->GetName());
    }

    PropertyDlgTpPort dlg(&libport, false, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    if(libport.GetPortDirection() == ProjectExplorer::PbTpPort::pdRT)
    {
        const QStringList lstPortName = libport.GetName().split(PropertyDlgTpPort::m_strSpliter);
        Q_ASSERT(lstPortName.size() == 2);

        if(pTpPort->GetPortDirection() == ProjectExplorer::PbTpPort::pdTx)
        {
            *pTpPort = libport;
            pTpPort->SetName(lstPortName.at(0));
            pTpPort->SetPortDirection(ProjectExplorer::PbTpPort::pdTx);

            int iOldTpPortPairId = pTpPortPair->GetId();
            *pTpPortPair = libport;
            pTpPortPair->SetId(iOldTpPortPairId);
            pTpPortPair->SetName(lstPortName.at(1));
            pTpPortPair->SetPortDirection(ProjectExplorer::PbTpPort::pdRx);
        }
        else if(pTpPort->GetPortDirection() == ProjectExplorer::PbTpPort::pdRx)
        {
            *pTpPort = libport;
            pTpPort->SetName(lstPortName.at(1));
            pTpPort->SetPortDirection(ProjectExplorer::PbTpPort::pdRx);

            int iOldTpPortPairId = pTpPortPair->GetId();
            *pTpPortPair = libport;
            pTpPortPair->SetId(iOldTpPortPairId);
            pTpPortPair->SetName(lstPortName.at(0));
            pTpPortPair->SetPortDirection(ProjectExplorer::PbTpPort::pdTx);
        }

        UpdatePortItem(pItemTpPort);

        for(int i = 0; i < m_pModelPort->rowCount(); i++)
        {
            if(QStandardItem *pItemTpPortPair = m_pModelPort->item(i, 0))
            {
                if(reinterpret_cast<ProjectExplorer::PbTpPort*>(pItemTpPortPair->data().toInt()) == pTpPortPair)
                {
                    UpdatePortItem(pItemTpPortPair);
                    break;
                }
            }
        }

    }
    else
    {
        *pTpPort = libport;
        UpdatePortItem(pItemTpPort);
    }
}
