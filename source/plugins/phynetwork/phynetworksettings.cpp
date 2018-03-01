#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QFormLayout>
#include <QListWidget>
#include <QComboBox>
#include <QAction>
#include <QToolButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QSpinBox>
#include <QDialogButtonBox>

#include "core/mainwindow.h"
#include "projectexplorer/pecable.h"
#include "projectexplorer/pedevice.h"

#include "phynetworksettings.h"
#include "cablegenerator.h"

using namespace PhyNetwork::Internal;

///////////////////////////////////////////////////////////////////////
// PhyNetworkCableSettingsDlg member functions
///////////////////////////////////////////////////////////////////////
PhyNetworkCableSettingsDlg::PhyNetworkCableSettingsDlg(QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Automatic Cabling Rule"));

    CableGenerator *pCableGenerator = CableGenerator::Instance();

    //////////////////////////////////////////////////
    // Optical
    //////////////////////////////////////////////////
    QGroupBox *pGroupBoxOptical = new QGroupBox(tr("Optical Rule"), this);
    pGroupBoxOptical->setMinimumWidth(300);
    QFormLayout *pFormLayoutOptical = new QFormLayout(pGroupBoxOptical);

    // Buttons
    m_pActionOpticalAdd = new QAction(QIcon(":/phynetwork/images/oper_add.png"), tr("Add..."), this);
    QToolButton *pToolButtonOpticalAdd = new QToolButton(this);
    pToolButtonOpticalAdd->setDefaultAction(m_pActionOpticalAdd);
    m_pActionOpticalEdit = new QAction(QIcon(":/phynetwork/images/oper_edit.png"), tr("Edit..."), this);
    QToolButton *pToolButtonOpticalEdit = new QToolButton(this);
    pToolButtonOpticalEdit->setDefaultAction(m_pActionOpticalEdit);
    m_pActionOpticalRemove = new QAction(QIcon(":/phynetwork/images/oper_remove.png"), tr("Remove"), this);
    QToolButton *pToolButtonOpticalRemove = new QToolButton(this);
    pToolButtonOpticalRemove->setDefaultAction(m_pActionOpticalRemove);

    // Reserve Type
    m_pComboBoxOpticalGroupType = new QComboBox((this));
    m_pComboBoxOpticalGroupType->addItem(tr("Group Cable By Custom"), CableGenerator::gtCustom);
    m_pComboBoxOpticalGroupType->addItem(tr("Group Cable By Port"), CableGenerator::gtPort);
    m_pComboBoxOpticalGroupType->setCurrentIndex(m_pComboBoxOpticalGroupType->findData(pCableGenerator->GetOpticalGroupType()));

    // Fiber Number
    m_pListWidgetOpticalFiberNumber = new QListWidget(this);
    m_pListWidgetOpticalFiberNumber->setMinimumHeight(150);
    m_pListWidgetOpticalFiberNumber->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pListWidgetOpticalFiberNumber->addAction(m_pActionOpticalAdd);
    m_pListWidgetOpticalFiberNumber->addAction(m_pActionOpticalEdit);
    m_pListWidgetOpticalFiberNumber->addAction(m_pActionOpticalRemove);
    foreach(int iNumber, pCableGenerator->GetOpticalFiberNumbers())
        m_pListWidgetOpticalFiberNumber->addItem(QString("%1").arg(iNumber, 2, 10, QLatin1Char('0')));
    m_pListWidgetOpticalFiberNumber->sortItems();

    // Reserve Type
    m_pComboBoxOpticalReserveType = new QComboBox((this));
    m_pComboBoxOpticalReserveType->addItem(tr("All Reserve"), CableGenerator::rtAll);
    m_pComboBoxOpticalReserveType->addItem(tr("Single Reserve"), CableGenerator::rtSingle);
    m_pComboBoxOpticalReserveType->setCurrentIndex(m_pComboBoxOpticalReserveType->findData(pCableGenerator->GetOpticalReserveType()));

    // Reserve Rate
    m_pLineEditOpticalReserveRate = new QLineEdit(QString::number(pCableGenerator->GetOpticalReserveRate()), this);
    m_pLineEditOpticalReserveRate->setValidator(new QDoubleValidator(0.0, 1.0, 5, this));

    QHBoxLayout *pHBoxLayoutOpticalButton = new QHBoxLayout;
    pHBoxLayoutOpticalButton->addStretch(1);
    pHBoxLayoutOpticalButton->addWidget(pToolButtonOpticalAdd);
    pHBoxLayoutOpticalButton->addWidget(pToolButtonOpticalEdit);
    pHBoxLayoutOpticalButton->addWidget(pToolButtonOpticalRemove);

    QFrame* pFrameLineOptical = new QFrame(this);
    pFrameLineOptical->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    pFormLayoutOptical->addRow(tr("Group Type:"), m_pComboBoxOpticalGroupType);
    pFormLayoutOptical->addRow(pFrameLineOptical);
    pFormLayoutOptical->addRow(tr("Fiber Numbers:"), m_pListWidgetOpticalFiberNumber);
    pFormLayoutOptical->addRow(pHBoxLayoutOpticalButton);
    pFormLayoutOptical->addRow(tr("Reserve Type:"), m_pComboBoxOpticalReserveType);
    pFormLayoutOptical->addRow(tr("Reserve Rate:"), m_pLineEditOpticalReserveRate);

    //////////////////////////////////////////////////
    // Tail
    //////////////////////////////////////////////////
    QGroupBox *pGroupBoxTail = new QGroupBox(tr("Tail Rule"), this);
    pGroupBoxTail->setMinimumWidth(300);
    QFormLayout *pFormLayoutTail = new QFormLayout(pGroupBoxTail);

    // Buttons
    m_pActionTailAdd = new QAction(QIcon(":/phynetwork/images/oper_add.png"), tr("Add..."), this);
    QToolButton *pToolButtonTailAdd = new QToolButton(this);
    pToolButtonTailAdd->setDefaultAction(m_pActionTailAdd);
    m_pActionTailEdit = new QAction(QIcon(":/phynetwork/images/oper_edit.png"), tr("Edit..."), this);
    QToolButton *pToolButtonTailEdit = new QToolButton(this);
    pToolButtonTailEdit->setDefaultAction(m_pActionTailEdit);
    m_pActionTailRemove = new QAction(QIcon(":/phynetwork/images/oper_remove.png"), tr("Remove"), this);
    QToolButton *pToolButtonTailRemove = new QToolButton(this);
    pToolButtonTailRemove->setDefaultAction(m_pActionTailRemove);

    // Reserve Type
    m_pComboBoxTailGroupType = new QComboBox((this));
    m_pComboBoxTailGroupType->addItem(tr("Group Cable By Custom"), CableGenerator::gtCustom);
    m_pComboBoxTailGroupType->addItem(tr("Group Cable By Port"), CableGenerator::gtPort);
    m_pComboBoxTailGroupType->setCurrentIndex(m_pComboBoxTailGroupType->findData(pCableGenerator->GetTailGroupType()));

    // Fiber Number
    m_pListWidgetTailFiberNumber = new QListWidget(this);
    m_pListWidgetTailFiberNumber->setMinimumHeight(150);
    m_pListWidgetTailFiberNumber->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pListWidgetTailFiberNumber->addAction(m_pActionOpticalAdd);
    m_pListWidgetTailFiberNumber->addAction(m_pActionOpticalEdit);
    m_pListWidgetTailFiberNumber->addAction(m_pActionOpticalRemove);
    foreach(int iNumber, pCableGenerator->GetTailFiberNumbers())
        m_pListWidgetTailFiberNumber->addItem(QString("%1").arg(iNumber, 2, 10, QLatin1Char('0')));
    m_pListWidgetTailFiberNumber->sortItems();

    // Reserve Type
    m_pComboBoxTailReserveType = new QComboBox((this));
    m_pComboBoxTailReserveType->addItem(tr("All Reserve"), CableGenerator::rtAll);
    m_pComboBoxTailReserveType->addItem(tr("Single Reserve"), CableGenerator::rtSingle);
    m_pComboBoxTailReserveType->setCurrentIndex(m_pComboBoxTailReserveType->findData(pCableGenerator->GetTailReserveType()));

    // Reserve Rate
    m_pLineEditTailReserveRate = new QLineEdit(QString::number(pCableGenerator->GetTailReserveRate()), this);
    m_pLineEditTailReserveRate->setValidator(new QDoubleValidator(0.0, 1.0, 5, this));

    QHBoxLayout *pHBoxLayoutTailButton = new QHBoxLayout;
    pHBoxLayoutTailButton->addStretch(1);
    pHBoxLayoutTailButton->addWidget(pToolButtonTailAdd);
    pHBoxLayoutTailButton->addWidget(pToolButtonTailEdit);
    pHBoxLayoutTailButton->addWidget(pToolButtonTailRemove);

    QFrame* pFrameLineTail = new QFrame(this);
    pFrameLineTail->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    pFormLayoutTail->addRow(tr("Group Type:"), m_pComboBoxTailGroupType);
    pFormLayoutTail->addRow(pFrameLineTail);
    pFormLayoutTail->addRow(tr("Fiber Numbers:"), m_pListWidgetTailFiberNumber);
    pFormLayoutTail->addRow(pHBoxLayoutTailButton);
    pFormLayoutTail->addRow(tr("Reserve Type:"), m_pComboBoxTailReserveType);
    pFormLayoutTail->addRow(tr("Reserve Rate:"), m_pLineEditTailReserveRate);

    //////////////////////////////////////////////////
    // Cable Naming Rule
    //////////////////////////////////////////////////
    m_pLineEditOpticalNameRule = new QLineEdit(pCableGenerator->GetOpticalNameRule(), this);
    m_pLineEditTailNameRule = new QLineEdit(pCableGenerator->GetTailNameRule(), this);
    m_pLineEditJumpNameRule = new QLineEdit(pCableGenerator->GetJumpNameRule(), this);

    QGroupBox *pGroupBoxCableNameRule = new QGroupBox(tr("Cable Naming Rule"), this);
    pGroupBoxCableNameRule->setMinimumWidth(300);
    QFormLayout *pFormLayoutCableNameRule = new QFormLayout(pGroupBoxCableNameRule);
    pFormLayoutCableNameRule->addRow(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctOptical) + ":", m_pLineEditOpticalNameRule);
    pFormLayoutCableNameRule->addRow(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctTail) + ":", m_pLineEditTailNameRule);
    pFormLayoutCableNameRule->addRow(ProjectExplorer::PeCable::GetCableTypeName(ProjectExplorer::PeCable::ctJump) + ":", m_pLineEditJumpNameRule);

    //////////////////////////////////////////////////
    // ODF
    //////////////////////////////////////////////////
    m_pLineEditOdfNameRule = new QLineEdit(pCableGenerator->GetOdfNameRule(), this);
    m_pSpinBoxPortNumber = new QSpinBox(this);
    m_pSpinBoxPortNumber->setRange(1, 50);
    m_pSpinBoxPortNumber->setValue(pCableGenerator->GetOdfLayerPortNumber());

    QGroupBox *pGroupBoxOdf = new QGroupBox(ProjectExplorer::PeDevice::GetDeviceTypeName(ProjectExplorer::PeDevice::dtODF), this);
    pGroupBoxCableNameRule->setMinimumWidth(300);
    QFormLayout *pFormLayoutOdf = new QFormLayout(pGroupBoxOdf);
    pFormLayoutOdf->addRow(tr("Naming Rule:"), m_pLineEditOdfNameRule);
    pFormLayoutOdf->addRow(tr("Layer Port Number:"), m_pSpinBoxPortNumber);

    QHBoxLayout *pHBoxLayout1 = new QHBoxLayout;
    pHBoxLayout1->addWidget(pGroupBoxOptical);
    pHBoxLayout1->addSpacing(5);
    pHBoxLayout1->addWidget(pGroupBoxTail);

    QHBoxLayout *pHBoxLayout2 = new QHBoxLayout;
    pHBoxLayout2->addWidget(pGroupBoxCableNameRule);
    pHBoxLayout2->addSpacing(5);
    pHBoxLayout2->addWidget(pGroupBoxOdf);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pVBoxLayout = GetClientLayout();
    pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    pVBoxLayout->addLayout(pHBoxLayout1);
    pVBoxLayout->addSpacing(5);
    pVBoxLayout->addLayout(pHBoxLayout2);
    pVBoxLayout->addSpacing(15);
    pVBoxLayout->addWidget(pDialogButtonBox);

    connect(m_pComboBoxOpticalGroupType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentGroupTypeChanged(int)));
    connect(m_pComboBoxTailGroupType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentGroupTypeChanged(int)));
    connect(m_pListWidgetOpticalFiberNumber, SIGNAL(currentRowChanged(int)),
            this, SLOT(SlotCurrentFiberNumberChanged(int)));
    connect(m_pListWidgetTailFiberNumber, SIGNAL(currentRowChanged(int)),
            this, SLOT(SlotCurrentFiberNumberChanged(int)));
    connect(m_pListWidgetOpticalFiberNumber, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(SlotCurrentFiberNumberDoubleClicked(QListWidgetItem*)));
    connect(m_pListWidgetTailFiberNumber, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(SlotCurrentFiberNumberDoubleClicked(QListWidgetItem*)));
    connect(m_pActionOpticalAdd, SIGNAL(triggered()),
            this, SLOT(SlotActionAdd()));
    connect(m_pActionTailAdd, SIGNAL(triggered()),
            this, SLOT(SlotActionAdd()));
    connect(m_pActionOpticalEdit, SIGNAL(triggered()),
            this, SLOT(SlotActionEdit()));
    connect(m_pActionTailEdit, SIGNAL(triggered()),
            this, SLOT(SlotActionEdit()));
    connect(m_pActionOpticalRemove, SIGNAL(triggered()),
            this, SLOT(SlotActionRemove()));
    connect(m_pActionTailRemove, SIGNAL(triggered()),
            this, SLOT(SlotActionRemove()));

    SlotCurrentFiberNumberChanged(-1, m_pListWidgetOpticalFiberNumber);
    SlotCurrentFiberNumberChanged(-1, m_pListWidgetTailFiberNumber);
    SlotCurrentGroupTypeChanged(-1, m_pComboBoxOpticalGroupType);
    SlotCurrentGroupTypeChanged(-1, m_pComboBoxTailGroupType);
}

void PhyNetworkCableSettingsDlg::accept()
{
    if(!m_pLineEditOpticalReserveRate->hasAcceptableInput() || !m_pLineEditTailReserveRate->hasAcceptableInput())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The range of 'Reserve Rate' must be between 0.0 and 1.0."));

        if(!m_pLineEditOpticalReserveRate->hasAcceptableInput())
            m_pLineEditOpticalReserveRate->setFocus();
        else if(!m_pLineEditTailReserveRate->hasAcceptableInput())
            m_pLineEditTailReserveRate->setFocus();

        return;
    }

    CableGenerator *pCableGenerator = CableGenerator::Instance();

    pCableGenerator->SetOpticalGroupType(CableGenerator::GroupType(m_pComboBoxOpticalGroupType->itemData(m_pComboBoxOpticalGroupType->currentIndex()).toInt()));
    QList<int> lstOpticalFiberNumbers;
    for(int i = 0; i < m_pListWidgetOpticalFiberNumber->count(); i++)
        lstOpticalFiberNumbers.append(m_pListWidgetOpticalFiberNumber->item(i)->text().toInt());
    pCableGenerator->SetOpticalFiberNumbers(lstOpticalFiberNumbers);
    pCableGenerator->SetOpticalReserveType(CableGenerator::ReserveType(m_pComboBoxOpticalReserveType->itemData(m_pComboBoxOpticalReserveType->currentIndex()).toInt()));
    pCableGenerator->SetOpticalReserveRate(m_pLineEditOpticalReserveRate->text().toDouble());

    pCableGenerator->SetTailGroupType(CableGenerator::GroupType(m_pComboBoxTailGroupType->itemData(m_pComboBoxTailGroupType->currentIndex()).toInt()));
    QList<int> lstTailFiberNumbers;
    for(int i = 0; i < m_pListWidgetTailFiberNumber->count(); i++)
        lstTailFiberNumbers.append(m_pListWidgetTailFiberNumber->item(i)->text().toInt());
    pCableGenerator->SetTailFiberNumbers(lstTailFiberNumbers);
    pCableGenerator->SetTailReserveType(CableGenerator::ReserveType(m_pComboBoxTailReserveType->itemData(m_pComboBoxTailReserveType->currentIndex()).toInt()));
    pCableGenerator->SetTailReserveRate(m_pLineEditTailReserveRate->text().toDouble());

    pCableGenerator->SetOpticalNameRule(m_pLineEditOpticalNameRule->text().trimmed());
    pCableGenerator->SetTailNameRule(m_pLineEditTailNameRule->text().trimmed());
    pCableGenerator->SetJumpNameRule(m_pLineEditJumpNameRule->text().trimmed());

    pCableGenerator->SetOdfNameRule(m_pLineEditOdfNameRule->text().trimmed());
    pCableGenerator->SetOdfLayerPortNumber(m_pSpinBoxPortNumber->value());

    QDialog::accept();
}

void PhyNetworkCableSettingsDlg::SlotCurrentGroupTypeChanged(int iIndex, QObject *pSender)
{
    if(!pSender)
        pSender = sender();

    if(pSender == m_pComboBoxOpticalGroupType)
    {
        if(iIndex < 0)
            iIndex = m_pComboBoxOpticalGroupType->currentIndex();

        CableGenerator::GroupType eGroupType = CableGenerator::GroupType(m_pComboBoxOpticalGroupType->itemData(iIndex).toInt());
        if(eGroupType == CableGenerator::gtPort)
        {
            m_pListWidgetOpticalFiberNumber->setEnabled(false);
            m_pComboBoxOpticalReserveType->setEnabled(false);
            m_pLineEditOpticalReserveRate->setEnabled(false);
            m_pActionOpticalAdd->setEnabled(false);
            m_pActionOpticalEdit->setEnabled(false);
            m_pActionOpticalRemove->setEnabled(false);
        }
        else if(eGroupType == CableGenerator::gtCustom)
        {
            m_pListWidgetOpticalFiberNumber->setEnabled(true);
            m_pComboBoxOpticalReserveType->setEnabled(true);
            m_pLineEditOpticalReserveRate->setEnabled(true);

            SlotCurrentFiberNumberChanged(-1, m_pListWidgetOpticalFiberNumber);
        }
    }
    else if(pSender == m_pComboBoxTailGroupType)
    {
        if(iIndex < 0)
            iIndex = m_pComboBoxTailGroupType->currentIndex();

        CableGenerator::GroupType eGroupType = CableGenerator::GroupType(m_pComboBoxTailGroupType->itemData(iIndex).toInt());
        if(eGroupType == CableGenerator::gtPort)
        {
            m_pListWidgetTailFiberNumber->setEnabled(false);
            m_pComboBoxTailReserveType->setEnabled(false);
            m_pLineEditTailReserveRate->setEnabled(false);
            m_pActionTailAdd->setEnabled(false);
            m_pActionTailEdit->setEnabled(false);
            m_pActionTailRemove->setEnabled(false);
        }
        else if(eGroupType == CableGenerator::gtCustom)
        {
            m_pListWidgetTailFiberNumber->setEnabled(true);
            m_pComboBoxTailReserveType->setEnabled(true);
            m_pLineEditTailReserveRate->setEnabled(true);

            SlotCurrentFiberNumberChanged(-1, m_pListWidgetTailFiberNumber);
        }
    }
}

void PhyNetworkCableSettingsDlg::SlotCurrentFiberNumberChanged(int iIndex, QObject *pSender)
{
    if(!pSender)
        pSender = sender();

    if(pSender == m_pListWidgetOpticalFiberNumber)
    {
        if(iIndex < 0)
            iIndex = m_pListWidgetOpticalFiberNumber->currentRow();

        m_pActionOpticalAdd->setEnabled(true);
        m_pActionOpticalEdit->setEnabled(iIndex >= 0);
        m_pActionOpticalRemove->setEnabled(iIndex >= 0);
    }
    else if(pSender == m_pListWidgetTailFiberNumber)
    {
        if(iIndex < 0)
            iIndex = m_pListWidgetTailFiberNumber->currentRow();

        m_pActionTailAdd->setEnabled(true);
        m_pActionTailEdit->setEnabled(iIndex >= 0);
        m_pActionTailRemove->setEnabled(iIndex >= 0);
    }
}

void PhyNetworkCableSettingsDlg::SlotCurrentFiberNumberDoubleClicked(QListWidgetItem *pItem)
{
    if(!pItem)
        return;

    if(pItem->listWidget() == m_pListWidgetOpticalFiberNumber)
        SlotActionEdit(m_pActionOpticalEdit);
    else if(pItem->listWidget() == m_pListWidgetTailFiberNumber)
        SlotActionEdit(m_pActionTailEdit);
}

void PhyNetworkCableSettingsDlg::SlotActionAdd(QObject *pSender)
{
    if(!pSender)
        pSender = sender();

    QListWidget *pListWidget = 0;
    if(pSender == m_pActionOpticalAdd)
        pListWidget = m_pListWidgetOpticalFiberNumber;
    else if(pSender == m_pActionTailAdd)
        pListWidget = m_pListWidgetTailFiberNumber;

    if(!pListWidget)
        return;

    bool bOk;
    int iValue = QInputDialog::getInt(Core::MainWindow::Instance(),
                                      tr("Add Fiber Number"),
                                      tr("Fiber Number:"), 8, 1, 99, 1, &bOk);
    if(!bOk)
        return;

    if(!pListWidget->findItems(QString("%1").arg(iValue, 2, 10, QLatin1Char('0')), Qt::MatchExactly).isEmpty())
    {
        QMessageBox::critical(Core::MainWindow::Instance(),
                              tr("Error"),
                              tr("Fiber Number '%1' has existed, please input a valid value.").arg(iValue));
        return;
    }

    pListWidget->addItem(QString("%1").arg(iValue, 2, 10, QLatin1Char('0')));
    pListWidget->sortItems();
}

void PhyNetworkCableSettingsDlg::SlotActionEdit(QObject *pSender)
{
    if(!pSender)
        pSender = sender();

    QListWidget *pListWidget = 0;
    if(pSender == m_pActionOpticalEdit)
        pListWidget = m_pListWidgetOpticalFiberNumber;
    else if(pSender == m_pActionTailEdit)
        pListWidget = m_pListWidgetTailFiberNumber;

    if(!pListWidget)
        return;

    QListWidgetItem *pItem = pListWidget->currentItem();
    if(!pItem)
        return;

    bool bOk;
    int iValue = QInputDialog::getInt(Core::MainWindow::Instance(),
                                      tr("Edit Fiber Number"),
                                      tr("Fiber Number:"), pItem->text().toInt(), 1, 99, 1, &bOk);
    if(!bOk)
        return;

    QList<QListWidgetItem*> lstItems = pListWidget->findItems(QString("%1").arg(iValue, 2, 10, QLatin1Char('0')), Qt::MatchExactly);
    lstItems.removeAll(pItem);
    if(!lstItems.isEmpty())
    {
        QMessageBox::critical(Core::MainWindow::Instance(),
                              tr("Error"),
                              tr("Fiber Number '%1' has existed, please input a valid value.").arg(iValue));
        return;
    }

    pItem->setText(QString("%1").arg(iValue, 2, 10, QLatin1Char('0')));
    pListWidget->sortItems();
}

void PhyNetworkCableSettingsDlg::SlotActionRemove(QObject *pSender)
{
    if(!pSender)
        pSender = sender();

    QListWidget *pListWidget = 0;
    if(pSender == m_pActionOpticalRemove)
        pListWidget = m_pListWidgetOpticalFiberNumber;
    else if(pSender == m_pActionTailRemove)
        pListWidget = m_pListWidgetTailFiberNumber;

    if(!pListWidget)
        return;

    QListWidgetItem *pItem = pListWidget->currentItem();
    if(!pItem)
        return;

    if(pListWidget->count() <= 1)
    {
        QMessageBox::critical(Core::MainWindow::Instance(),
                              tr("Error"),
                              tr("There must be at least one fiber number."));
        return;
    }

    pListWidget->takeItem(pListWidget->row(pItem));
    delete pItem;
}
