#include <QApplication>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QAction>
#include <QMessageBox>
#include <QSettings>
#include <QToolButton>
#include <QFileDialog>
#include <QHeaderView>
#include <QSplitter>

#include "utils/waitcursor.h"
#include "utils/readonlyview.h"
#include "projectexplorer/basemanager.h"
#include "projectexplorer/pblibdevice.h"
#include "projectexplorer/pblibboard.h"
#include "projectexplorer/pblibport.h"
#include "projectexplorer/pbdevicecategory.h"
#include "projectexplorer/pbbaycategory.h"
#include "projectexplorer/pbmanufacture.h"
#include "projectexplorer/pevterminal.h"
#include "sclparser/scldocument.h"
#include "sclparser/sclelement.h"
#include "core/mainwindow.h"
#include "core/vterminaldefine.h"

#include "propertydlglibswitch.h"
#include "propertydlglibboard.h"
#include "propertydlglibfiberport.h"

static const char * const g_szSettings_IcdFilePath      = "IcdFilePath";

static QString GetICDFilePath()
{
    return Core::MainWindow::Instance()->GetSettings()->value(g_szSettings_IcdFilePath, QString()).toString();
}

static void SetICDFilePath(const QString strICDFilePath)
{
    Core::MainWindow::Instance()->GetSettings()->setValue(g_szSettings_IcdFilePath, strICDFilePath);
}

using namespace DevLibrary::Internal;

PropertyDlgLibSwitch::PropertyDlgLibSwitch(ProjectExplorer::PbLibDevice *pLibDevice, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pLibDevice(pLibDevice), m_pCurrentPortItem(0)
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

    m_pLineEditType = new QLineEdit(this);
    m_pLineEditType->setObjectName(tr("Type"));
    m_pComboBoxManufacture = new QComboBox(this);
    m_pComboBoxManufacture->setObjectName(tr("Manufacture"));
    m_pComboBoxManufacture->setEditable(true);
    m_pComboBoxManufacture->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QGroupBox *pGroupBoxGeneral = new QGroupBox(this);
    QGridLayout *pGridLayoutGeneral = new QGridLayout(pGroupBoxGeneral);
    pGridLayoutGeneral->addWidget(new QLabel(m_pLineEditType->objectName() + ":", this), 0, 0);
    pGridLayoutGeneral->addWidget(m_pLineEditType, 0, 1);
    pGridLayoutGeneral->addWidget(new QLabel(m_pComboBoxManufacture->objectName() + ":", this), 0, 2);
    pGridLayoutGeneral->addWidget(m_pComboBoxManufacture, 0, 3);

    m_pModelPort = new QStandardItemModel(0, 6, this);
    m_pModelPort->setHeaderData(0, Qt::Horizontal, tr("Port Name"));
    m_pModelPort->setHeaderData(1, Qt::Horizontal, tr("Port Index"));
    m_pModelPort->setHeaderData(2, Qt::Horizontal, tr("Port Type"));
    m_pModelPort->setHeaderData(3, Qt::Horizontal, tr("Port Direction"));
    m_pModelPort->setHeaderData(4, Qt::Horizontal, tr("Port Fiber Plug"));
    m_pModelPort->setHeaderData(5, Qt::Horizontal, tr("Port Fiber Mode"));
    m_pViewPort = new Utils::ReadOnlyTreeView(this);
    m_pViewPort->setIndentation(0);
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
    m_pViewPort->setColumnWidth(3, 130);
    m_pViewPort->setColumnWidth(4, 130);
    m_pViewPort->setColumnWidth(5, 130);
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

    QList<ProjectExplorer::PbManufacture*> lstManufactures = ProjectExplorer::BaseManager::Instance()->GetAllManufactures();
    qSort(lstManufactures.begin(), lstManufactures.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    foreach(ProjectExplorer::PbManufacture *pManufacture, lstManufactures)
        m_pComboBoxManufacture->addItem(pManufacture->GetDisplayName());

    if(pLibDevice->GetId() == ProjectExplorer::PbBaseObject::m_iInvalidObjectId)
        SetWindowTitle(tr("New %1").arg(pLibDevice->GetDeviceTypeName(pLibDevice->GetDeviceType())));
    else
        SetWindowTitle(tr("%1 Property").arg(pLibDevice->GetDeviceTypeName(pLibDevice->GetDeviceType())));

    UpdateData(false);
}

bool PropertyDlgLibSwitch::UpdateData(bool bSaveAndValidate)
{
    if(bSaveAndValidate)
    {
        // Type
        const QString strType = m_pLineEditType->text().trimmed();
        if(strType.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pLineEditType->objectName()));

            m_pLineEditType->setFocus();
            return false;
        }

        // Manufacture
        const QString strManufacture = m_pComboBoxManufacture->currentText().trimmed();
        if(strManufacture.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxManufacture->objectName()));

            m_pComboBoxManufacture->setFocus();
            return false;
        }

        foreach(ProjectExplorer::PbLibDevice *pLibDevice, ProjectExplorer::BaseManager::Instance()->GetAllLibDevices())
        {
            if(pLibDevice->GetId() == m_pLibDevice->GetId())
                continue;

            if(strType == pLibDevice->GetType() && strManufacture == pLibDevice->GetManufacture())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The product can NOT be duplicate, please input a valid value."));

                return false;
            }
        }

        m_pLibDevice->SetType(strType);
        m_pLibDevice->SetManufacture(strManufacture);
    }
    else
    {
        m_pLineEditType->setText(m_pLibDevice->GetType());

        m_pComboBoxManufacture->setEditText(m_pLibDevice->GetManufacture());

        BuildPortData();
    }

    return true;
}

ProjectExplorer::PbLibBoard* PropertyDlgLibSwitch::GetSwitchBoard() const
{
    if(!m_pLibDevice || m_pLibDevice->GetDeviceType() != ProjectExplorer::PbLibDevice::dtSwitch)
        return 0;

    if(m_pLibDevice->GetChildBoards().isEmpty())
    {
        ProjectExplorer::PbLibBoard *pLibBoard = new ProjectExplorer::PbLibBoard;
        pLibBoard->SetPosition("B01");
        pLibBoard->SetType("SWITCH");
        m_pLibDevice->AddChildBoard(pLibBoard);
        pLibBoard->SetParentDevice(m_pLibDevice);
    }

    return m_pLibDevice->GetChildBoards().size() == 1 ? m_pLibDevice->GetChildBoards().first() : 0;
}

void PropertyDlgLibSwitch::BuildPortData()
{
    if(ProjectExplorer::PbLibBoard *pLibBoard = GetSwitchBoard())
    {
        foreach(ProjectExplorer::PbLibPort *pLibPort, pLibBoard->GetChildPorts())
            InsertPortItem(pLibPort);
    }

    m_pViewPort->expandAll();
}

QStandardItem* PropertyDlgLibSwitch::InsertPortItem(ProjectExplorer::PbLibPort *pLibPort)
{
    if(!pLibPort)
        return 0;

    QList<QStandardItem*> lstPortItems;

    QStandardItem *pItemPortName = new QStandardItem(pLibPort->GetDisplayIcon(), pLibPort->GetName());
    pItemPortName->setData(reinterpret_cast<int>(pLibPort));
    lstPortItems.append(pItemPortName);

    QStandardItem *pItemPortIndex = new QStandardItem(QString::number(pLibPort->GetGroup()));
    pItemPortIndex->setData(reinterpret_cast<int>(pLibPort));
    lstPortItems.append(pItemPortIndex);

    QStandardItem *pItemPortType = new QStandardItem(ProjectExplorer::PbLibPort::GetPortTypeName(pLibPort->GetPortType()));
    pItemPortType->setData(reinterpret_cast<int>(pLibPort));
    lstPortItems.append(pItemPortType);

    QStandardItem *pItemPortDirection = new QStandardItem(ProjectExplorer::PbLibPort::GetPortDirectionName(pLibPort->GetPortDirection()));
    pItemPortDirection->setData(reinterpret_cast<int>(pLibPort));
    lstPortItems.append(pItemPortDirection);

    QStandardItem *pItemPortFiberPlug = new QStandardItem(ProjectExplorer::PbLibPort::GetFiberPlugName(pLibPort->GetFiberPlug()));
    pItemPortFiberPlug->setData(reinterpret_cast<int>(pLibPort));
    lstPortItems.append(pItemPortFiberPlug);

    QStandardItem *pItemPortFiberMode = new QStandardItem(ProjectExplorer::PbLibPort::GetFiberModeName(pLibPort->GetFiberMode()));
    pItemPortFiberMode->setData(reinterpret_cast<int>(pLibPort));
    lstPortItems.append(pItemPortFiberMode);

    m_pModelPort->appendRow(lstPortItems);
    return pItemPortName;
}

void PropertyDlgLibSwitch::UpdatePortItem(QStandardItem *pItemLibPort)
{
    if(!pItemLibPort)
        return;

    ProjectExplorer::PbLibPort *pLibPort = reinterpret_cast<ProjectExplorer::PbLibPort*>(pItemLibPort->data().toInt());
    if(!pLibPort)
        return;

    QStandardItem *pItemPortName = m_pModelPort->item(pItemLibPort->row(), 0);
    pItemPortName->setText(pLibPort->GetName());

    QStandardItem *pItemPortIndex = m_pModelPort->item(pItemLibPort->row(), 1);
    pItemPortIndex->setText(QString::number(pLibPort->GetGroup()));

    QStandardItem *pItemPortType = m_pModelPort->item(pItemLibPort->row(), 2);
    pItemPortType->setText(ProjectExplorer::PbLibPort::GetPortTypeName(pLibPort->GetPortType()));

    QStandardItem *pItemPortDirection = m_pModelPort->item(pItemLibPort->row(), 3);
    pItemPortDirection->setText(ProjectExplorer::PbLibPort::GetPortDirectionName(pLibPort->GetPortDirection()));

    QStandardItem *pItemPortFiberPlug = m_pModelPort->item(pItemLibPort->row(), 4);
    pItemPortFiberPlug->setText(ProjectExplorer::PbLibPort::GetFiberPlugName(pLibPort->GetFiberPlug()));

    QStandardItem *pItemPortFiberMode = m_pModelPort->item(pItemLibPort->row(), 5);
    pItemPortFiberMode->setText(ProjectExplorer::PbLibPort::GetFiberModeName(pLibPort->GetFiberMode()));
}

void PropertyDlgLibSwitch::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}

void PropertyDlgLibSwitch::SlotViewPortSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    QItemSelectionModel *pSelectionModel = m_pViewPort->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
    if(lstSelectedIndex.size() == 1)
        m_pCurrentPortItem = m_pModelPort->itemFromIndex(lstSelectedIndex.first());
    else
        m_pCurrentPortItem = 0;

    m_pActionNewPort->setEnabled(m_pCurrentPortItem);
    m_pActionDelete->setEnabled(m_pCurrentPortItem);
    m_pActionProperty->setEnabled(m_pCurrentPortItem);
}

void PropertyDlgLibSwitch::SlotViewPortDoubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    SlotActionProperty();
}

void PropertyDlgLibSwitch::SlotActionNewPort()
{
    ProjectExplorer::PbLibBoard *pLibBoard = GetSwitchBoard();
    if(!pLibBoard)
        return;

    ProjectExplorer::PbLibPort libport;
    libport.SetParentBoard(pLibBoard);
    libport.SetPortType(ProjectExplorer::PbLibPort::ptFiber);
    libport.SetPortDirection(ProjectExplorer::PbLibPort::pdRT);

    PropertyDlgLibFiberPort dlg(&libport, true, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    const int iNewPortNumber = dlg.GetNewPortNumber();
    int iGroup = libport.GetGroup();
    QList<int> lstExistGroups;
    foreach(ProjectExplorer::PbLibPort *pPort, pLibBoard->GetChildPorts())
    {
        if(!lstExistGroups.contains(pPort->GetGroup()))
            lstExistGroups.append(pPort->GetGroup());
    }

    if(libport.GetPortDirection() == ProjectExplorer::PbLibPort::pdRT)
    {
        const QStringList lstPortName = libport.GetName().split(PropertyDlgLibFiberPort::m_strSpliter);
        Q_ASSERT(lstPortName.size() == 2);

        for(int i = 0; i < iNewPortNumber; i++)
        {
            while(lstExistGroups.contains(iGroup))
                iGroup++;
            lstExistGroups.append(iGroup);

            ProjectExplorer::PbLibPort *pLibPortTx = new ProjectExplorer::PbLibPort(libport);
            pLibPortTx->SetGroup(iGroup);
            pLibPortTx->SetPortDirection(ProjectExplorer::PbLibPort::pdTx);
            pLibPortTx->SetName(iNewPortNumber == 1 ? lstPortName.at(0) : QString("%1%2").arg(lstPortName.at(0)).arg(iGroup, 2, 10, QLatin1Char('0')));
            pLibPortTx->SetParentBoard(pLibBoard);
            pLibBoard->AddChildPort(pLibPortTx);
            InsertPortItem(pLibPortTx);

            ProjectExplorer::PbLibPort *pLibPortRx = new ProjectExplorer::PbLibPort(libport);
            pLibPortRx->SetGroup(iGroup);
            pLibPortRx->SetPortDirection(ProjectExplorer::PbLibPort::pdRx);
            pLibPortRx->SetName(iNewPortNumber == 1 ? lstPortName.at(1) : QString("%1%2").arg(lstPortName.at(1)).arg(iGroup, 2, 10, QLatin1Char('0')));
            pLibPortRx->SetParentBoard(pLibBoard);
            pLibBoard->AddChildPort(pLibPortRx);
            InsertPortItem(pLibPortRx);
        }
    }
    else
    {
        for(int i = 0; i < iNewPortNumber; i++)
        {
            while(lstExistGroups.contains(iGroup))
                iGroup++;
            lstExistGroups.append(iGroup);

            ProjectExplorer::PbLibPort *pLibPort = new ProjectExplorer::PbLibPort(libport);
            pLibPort->SetGroup(iGroup);
            pLibPort->SetName(iNewPortNumber == 1 ? libport.GetName() : QString("%1%2").arg(libport.GetName()).arg(iGroup, 2, 10, QLatin1Char('0')));
            pLibPort->SetParentBoard(pLibBoard);
            pLibBoard->AddChildPort(pLibPort);
            InsertPortItem(pLibPort);
        }
    }
}

void PropertyDlgLibSwitch::SlotActionDelete()
{
    if(!m_pCurrentPortItem)
        return;

    ProjectExplorer::PbLibPort *pLibPort = reinterpret_cast<ProjectExplorer::PbLibPort*>(m_pCurrentPortItem->data().toInt());
    if(!pLibPort)
        return;

    ProjectExplorer::PbLibBoard *pLibBoard = pLibPort->GetParentBoard();
    if(!pLibBoard)
        return;

    ProjectExplorer::PbLibPort *pLibPortPair = pLibPort->GetPairPort();

    pLibBoard->RemoveChildPort(pLibPort);
    delete pLibPort;
    m_pModelPort->removeRow(m_pCurrentPortItem->row());

    if(pLibPortPair)
    {
        for(int i = 0; i < m_pModelPort->rowCount(); i++)
        {
            if(QStandardItem *pItemLibPortPair = m_pModelPort->item(i, 0))
            {
                if(reinterpret_cast<ProjectExplorer::PbLibPort*>(pItemLibPortPair->data().toInt()) == pLibPortPair)
                {
                    m_pModelPort->removeRow(pItemLibPortPair->row());
                    break;
                }
            }
        }

        pLibBoard->RemoveChildPort(pLibPortPair);
        delete pLibPortPair;
    }


}

void PropertyDlgLibSwitch::SlotActionProperty()
{
    if(!m_pCurrentPortItem)
        return;

    ProjectExplorer::PbLibPort *pLibPort = reinterpret_cast<ProjectExplorer::PbLibPort*>(m_pCurrentPortItem->data().toInt());
    if(!pLibPort)
        return;

    if(pLibPort->GetPortType() == ProjectExplorer::PbLibPort::ptFiber)
    {
        ProjectExplorer::PbLibPort libport(*pLibPort);
        ProjectExplorer::PbLibPort *pLibPortPair = pLibPort->GetPairPort();
        if(pLibPortPair)
        {
            libport.SetPortDirection(ProjectExplorer::PbLibPort::pdRT);
            if(pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdTx)
                libport.SetName(pLibPort->GetName() + PropertyDlgLibFiberPort::m_strSpliter + pLibPortPair->GetName());
            else if(pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdRx)
                libport.SetName(pLibPortPair->GetName() + PropertyDlgLibFiberPort::m_strSpliter + pLibPort->GetName());
        }

        PropertyDlgLibFiberPort dlg(&libport, false, this);
        if(dlg.exec() != QDialog::Accepted)
            return;

        if(libport.GetPortDirection() == ProjectExplorer::PbLibPort::pdRT)
        {
            const QStringList lstPortName = libport.GetName().split(PropertyDlgLibFiberPort::m_strSpliter);
            Q_ASSERT(lstPortName.size() == 2);

            if(pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdTx)
            {
                *pLibPort = libport;
                pLibPort->SetName(lstPortName.at(0));
                pLibPort->SetPortDirection(ProjectExplorer::PbLibPort::pdTx);

                *pLibPortPair = libport;
                pLibPortPair->SetName(lstPortName.at(1));
                pLibPortPair->SetPortDirection(ProjectExplorer::PbLibPort::pdRx);
            }
            else if(pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdRx)
            {
                *pLibPort = libport;
                pLibPort->SetName(lstPortName.at(1));
                pLibPort->SetPortDirection(ProjectExplorer::PbLibPort::pdRx);

                *pLibPortPair = libport;
                pLibPortPair->SetName(lstPortName.at(0));
                pLibPortPair->SetPortDirection(ProjectExplorer::PbLibPort::pdTx);
            }

            UpdatePortItem(m_pCurrentPortItem);

            for(int i = 0; i < m_pModelPort->rowCount(); i++)
            {
                if(QStandardItem *pItemLibPortPair = m_pModelPort->item(i, 0))
                {
                    if(reinterpret_cast<ProjectExplorer::PbLibPort*>(pItemLibPortPair->data().toInt()) == pLibPortPair)
                    {
                        UpdatePortItem(pItemLibPortPair);
                        break;
                    }
                }
            }

        }
        else
        {
            *pLibPort = libport;
            UpdatePortItem(m_pCurrentPortItem);
        }
    }
}
