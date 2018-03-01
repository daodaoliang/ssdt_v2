#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peport.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/pedevice.h"

#include "propertydlgfiberport.h"

const int g_iMaxPortGroupIndex = 50;

using namespace DevExplorer::Internal;

const QString PropertyDlgFiberPort::m_strSpliter = "@spliter@";
PropertyDlgFiberPort::PropertyDlgFiberPort(ProjectExplorer::PePort *pPort, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pPort(pPort)
{
    m_pSpinBoxNumber = new QSpinBox(this);
    m_pSpinBoxNumber->setObjectName(tr("New Port Number"));
    m_pSpinBoxNumber->setRange(1, 100);
    m_pComboBoxGroup = new QComboBox(this);
    m_pComboBoxGroup->setEditable(false);
    m_pComboBoxGroup->setObjectName(tr("Index"));
    m_pComboBoxDirection = new QComboBox(this);
    m_pComboBoxDirection->setObjectName(tr("Direction"));
    m_pComboBoxGroup->setEditable(false);
    m_pLineEditNameTx = new QLineEdit(this);
    m_pLineEditNameTx->setObjectName(tr("Tx Port Name"));
    m_pLineEditNameTx->setMinimumWidth(200);
    m_pLineEditNameRx = new QLineEdit(this);
    m_pLineEditNameRx->setMinimumWidth(200);
    m_pLineEditNameRx->setObjectName(tr("Rx Port Name"));
    m_pComboBoxFiberPlug = new QComboBox(this);
    m_pComboBoxFiberPlug->setObjectName(tr("Fiber Plug"));
    m_pComboBoxFiberPlug->setEditable(false);
    m_pComboBoxFiberMode = new QComboBox(this);
    m_pComboBoxFiberMode->setObjectName(tr("Fiber Mode"));
    m_pComboBoxFiberMode->setEditable(false);
    m_pCheckBoxCascade = new QCheckBox(this);
    m_pCheckBoxCascade->setObjectName(tr("Cascade"));
    m_pCheckBoxCascade->setEnabled(false);

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QFrame* pFrameLineGroup = new QFrame(this);
    pFrameLineGroup->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QFrame* pFrameLineType = new QFrame(this);
    pFrameLineType->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QFrame* pFrameLineCascade = new QFrame(this);
    pFrameLineCascade->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGroupBox *pGroupBox = new QGroupBox(this);
    QFormLayout *pFormLayout = new QFormLayout(pGroupBox);
    if(m_pPort->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
    {
        pFormLayout->addRow(m_pSpinBoxNumber->objectName() + ":", m_pSpinBoxNumber);
        pFormLayout->addRow(pFrameLineBasic);
    }
    else
    {
        m_pSpinBoxNumber->hide();
        pFrameLineBasic->hide();
    }
    pFormLayout->addRow(m_pComboBoxGroup->objectName() + ":", m_pComboBoxGroup);
    pFormLayout->addRow(m_pComboBoxDirection->objectName() + ":", m_pComboBoxDirection);
    pFormLayout->addRow(pFrameLineGroup);
    pFormLayout->addRow(m_pLineEditNameTx->objectName() + ":", m_pLineEditNameTx);
    pFormLayout->addRow(m_pLineEditNameRx->objectName() + ":", m_pLineEditNameRx);
    pFormLayout->addRow(pFrameLineType);
    pFormLayout->addRow(m_pComboBoxFiberPlug->objectName() + ":", m_pComboBoxFiberPlug);
    pFormLayout->addRow(m_pComboBoxFiberMode->objectName() + ":", m_pComboBoxFiberMode);
    if(m_pPort->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
    {
        pFormLayout->addRow(pFrameLineCascade);
        pFormLayout->addRow(m_pCheckBoxCascade->objectName() + ":", m_pCheckBoxCascade);
    }
    else
    {
        m_pCheckBoxCascade->hide();
        pFrameLineCascade->hide();
    }

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    if(m_pPort->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
    {
        SetWindowTitle(tr("New %1").arg(m_pPort->GetObjectTypeName()));
    }
    else
    {
        SetWindowTitle(tr("%1 Property").arg(m_pPort->GetObjectTypeName()));
        m_pComboBoxDirection->setEnabled(false);
    }

    FillComboBoxGroup();

    if(m_pPort->GetPortDirection() == ProjectExplorer::PePort::pdTx)
    {
        m_pLineEditNameTx->setText(m_pPort->GetName());
    }
    else if(m_pPort->GetPortDirection() == ProjectExplorer::PePort::pdRx)
    {
        m_pLineEditNameRx->setText(m_pPort->GetName());
    }
    else if(m_pPort->GetPortDirection() == ProjectExplorer::PePort::pdRT)
    {
        if(m_pPort->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        {
            m_pLineEditNameTx->setText(ProjectExplorer::PePort::GetPortDirectionName(ProjectExplorer::PePort::pdTx) + QString("%1").arg(m_pComboBoxGroup->itemData(m_pComboBoxGroup->currentIndex()).toInt(), 2, 10, QLatin1Char('0')));
            m_pLineEditNameRx->setText(ProjectExplorer::PePort::GetPortDirectionName(ProjectExplorer::PePort::pdRx) + QString("%1").arg(m_pComboBoxGroup->itemData(m_pComboBoxGroup->currentIndex()).toInt(), 2, 10, QLatin1Char('0')));
        }
        else
        {
            QStringList lstNames = m_pPort->GetName().split(m_strSpliter);
            Q_ASSERT(lstNames.size() == 2);

            m_pLineEditNameTx->setText(lstNames.at(0));
            m_pLineEditNameRx->setText(lstNames.at(1));
        }
    }

    m_pComboBoxDirection->addItem(ProjectExplorer::PePort::GetPortDirectionName(ProjectExplorer::PePort::pdTx), int(ProjectExplorer::PePort::pdTx));
    m_pComboBoxDirection->addItem(ProjectExplorer::PePort::GetPortDirectionName(ProjectExplorer::PePort::pdRx), int(ProjectExplorer::PePort::pdRx));
    m_pComboBoxDirection->addItem(ProjectExplorer::PePort::GetPortDirectionName(ProjectExplorer::PePort::pdRT), int(ProjectExplorer::PePort::pdRT));
    int iCurrentIndex = m_pComboBoxDirection->findData(int(m_pPort->GetPortDirection()));
    m_pComboBoxDirection->setCurrentIndex(iCurrentIndex == -1 ? 0 : iCurrentIndex);

    m_pComboBoxFiberPlug->addItem(ProjectExplorer::PePort::GetFiberPlugName(ProjectExplorer::PePort::fpST), int(ProjectExplorer::PePort::fpST));
    m_pComboBoxFiberPlug->addItem(ProjectExplorer::PePort::GetFiberPlugName(ProjectExplorer::PePort::fpLC), int(ProjectExplorer::PePort::fpLC));
    m_pComboBoxFiberPlug->addItem(ProjectExplorer::PePort::GetFiberPlugName(ProjectExplorer::PePort::fpSC), int(ProjectExplorer::PePort::fpSC));
    m_pComboBoxFiberPlug->addItem(ProjectExplorer::PePort::GetFiberPlugName(ProjectExplorer::PePort::fpFC), int(ProjectExplorer::PePort::fpFC));
    iCurrentIndex = m_pComboBoxFiberPlug->findData(int(m_pPort->GetFiberPlug()));
    m_pComboBoxFiberPlug->setCurrentIndex(iCurrentIndex == -1 ? 0 : iCurrentIndex);

    m_pComboBoxFiberMode->addItem(ProjectExplorer::PePort::GetFiberModeName(ProjectExplorer::PePort::fmSingle), int(ProjectExplorer::PePort::fmSingle));
    m_pComboBoxFiberMode->addItem(ProjectExplorer::PePort::GetFiberModeName(ProjectExplorer::PePort::fmMultiple), int(ProjectExplorer::PePort::fmMultiple));
    iCurrentIndex = m_pComboBoxFiberMode->findData(int(m_pPort->GetFiberMode()));
    m_pComboBoxFiberMode->setCurrentIndex(iCurrentIndex == -1 ? 0 : iCurrentIndex);

    m_pCheckBoxCascade->setChecked(m_pPort->GetCascade());
    m_pCheckBoxCascade->setEnabled(m_pPort->GetParentDevice()->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch);

    connect(m_pComboBoxDirection, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentDirectionIndexChanged(int)));
    SlotCurrentDirectionIndexChanged(m_pComboBoxDirection->currentIndex());
}

int PropertyDlgFiberPort::GetNewPortNumber() const
{
    return m_pSpinBoxNumber->value();
}

void PropertyDlgFiberPort::FillComboBoxGroup()
{
    ProjectExplorer::PeBoard *pBoard = m_pPort->GetParentBoard();
    Q_ASSERT(pBoard);

    QList<int> lstExistGroups;
    foreach(ProjectExplorer::PePort *pPort, pBoard->GetChildPorts())
    {
        if(!lstExistGroups.contains(pPort->GetGroup()))
            lstExistGroups.append(pPort->GetGroup());
    }

    QList<int> lstValidGroups;
    for(int i = 1; i <= g_iMaxPortGroupIndex; i++)
    {
        if(lstExistGroups.contains(i))
            continue;

        lstValidGroups.append(i);
    }

    if(m_pPort->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        lstValidGroups.append(m_pPort->GetGroup());

    qSort(lstValidGroups);

    foreach(int iGroup, lstValidGroups)
        m_pComboBoxGroup->addItem(QString("%1").arg(iGroup, 2, 10, QLatin1Char('0')), iGroup);

    int iCurrentIndex = m_pComboBoxGroup->findText(QString("%1").arg(m_pPort->GetGroup(), 2, 10, QLatin1Char('0')));
    if(iCurrentIndex == -1)
        iCurrentIndex = 0;

    m_pComboBoxGroup->setCurrentIndex(iCurrentIndex);
}

void PropertyDlgFiberPort::accept()
{
    m_pPort->SetGroup(m_pComboBoxGroup->itemData(m_pComboBoxGroup->currentIndex()).toInt());
    m_pPort->SetPortDirection(ProjectExplorer::PePort::PortDirection(m_pComboBoxDirection->itemData(m_pComboBoxDirection->currentIndex()).toInt()));

    const QString strTxName = m_pLineEditNameTx->text().trimmed();
    if(strTxName.isEmpty() && m_pLineEditNameTx->isEnabled())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pLineEditNameTx->objectName()));

        m_pLineEditNameTx->setFocus();
        return;
    }
    const QString strRxName = m_pLineEditNameRx->text().trimmed();
    if(strRxName.isEmpty() && m_pLineEditNameRx->isEnabled())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pLineEditNameRx->objectName()));

        m_pLineEditNameRx->setFocus();
        return;
    }

    if(m_pPort->GetPortDirection() == ProjectExplorer::PePort::pdTx)
        m_pPort->SetName(strTxName);
    else if(m_pPort->GetPortDirection() == ProjectExplorer::PePort::pdRx)
        m_pPort->SetName(strRxName);
    else if(m_pPort->GetPortDirection() == ProjectExplorer::PePort::pdRT)
        m_pPort->SetName(strTxName + m_strSpliter + strRxName);
    else
        Q_ASSERT(false);

    m_pPort->SetFiberPlug(ProjectExplorer::PePort::FiberPlug(m_pComboBoxFiberPlug->itemData(m_pComboBoxFiberPlug->currentIndex()).toInt()));
    m_pPort->SetFiberMode(ProjectExplorer::PePort::FiberMode(m_pComboBoxFiberMode->itemData(m_pComboBoxFiberMode->currentIndex()).toInt()));
    m_pPort->SetCascade(m_pCheckBoxCascade->isChecked());

    return QDialog::accept();
}

void PropertyDlgFiberPort::SlotCurrentDirectionIndexChanged(int iIndex)
{
    ProjectExplorer::PePort::PortDirection ePortDirection = ProjectExplorer::PePort::PortDirection(m_pComboBoxDirection->itemData(iIndex).toInt());

    if(ePortDirection == ProjectExplorer::PePort::pdTx)
    {
        m_pLineEditNameTx->setEnabled(true);
        m_pLineEditNameRx->setEnabled(false);
    }
    else if(ePortDirection == ProjectExplorer::PePort::pdRx)
    {
        m_pLineEditNameTx->setEnabled(false);
        m_pLineEditNameRx->setEnabled(true);
    }
    else if(ePortDirection == ProjectExplorer::PePort::pdRT)
    {
        m_pLineEditNameTx->setEnabled(true);
        m_pLineEditNameRx->setEnabled(true);
    }
    else
    {
        m_pLineEditNameTx->setEnabled(false);
        m_pLineEditNameRx->setEnabled(false);
    }
}

