#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/pblibport.h"
#include "projectexplorer/pblibboard.h"
#include "projectexplorer/pblibdevice.h"

#include "propertydlglibfiberport.h"

const int g_iMaxPortGroupIndex = 50;

using namespace DevLibrary::Internal;

const QString PropertyDlgLibFiberPort::m_strSpliter = "@spliter@";
PropertyDlgLibFiberPort::PropertyDlgLibFiberPort(ProjectExplorer::PbLibPort *pLibPort, bool bNew, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pLibPort(pLibPort), m_bNew(bNew)
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
    if(m_bNew)
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
    if(m_pLibPort->GetParentDevice()->GetDeviceType() == ProjectExplorer::PbLibDevice::dtSwitch)
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

    if(m_bNew)
    {
        SetWindowTitle(tr("New %1").arg(m_pLibPort->GetObjectTypeName()));
    }
    else
    {
        SetWindowTitle(tr("%1 Property").arg(m_pLibPort->GetObjectTypeName()));
        m_pComboBoxDirection->setEnabled(false);
    }

    FillComboBoxGroup();

    if(m_pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdTx)
    {
        m_pLineEditNameTx->setText(m_pLibPort->GetName());
    }
    else if(m_pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdRx)
    {
        m_pLineEditNameRx->setText(m_pLibPort->GetName());
    }
    else if(m_pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdRT)
    {
        if(m_bNew)
        {
            m_pLineEditNameTx->setText(ProjectExplorer::PbLibPort::GetPortDirectionName(ProjectExplorer::PbLibPort::pdTx) + QString("%1").arg(m_pComboBoxGroup->itemData(m_pComboBoxGroup->currentIndex()).toInt(), 2, 10, QLatin1Char('0')));
            m_pLineEditNameRx->setText(ProjectExplorer::PbLibPort::GetPortDirectionName(ProjectExplorer::PbLibPort::pdRx) + QString("%1").arg(m_pComboBoxGroup->itemData(m_pComboBoxGroup->currentIndex()).toInt(), 2, 10, QLatin1Char('0')));
        }
        else
        {
            QStringList lstNames = m_pLibPort->GetName().split(m_strSpliter);
            Q_ASSERT(lstNames.size() == 2);

            m_pLineEditNameTx->setText(lstNames.at(0));
            m_pLineEditNameRx->setText(lstNames.at(1));
        }
    }

    m_pComboBoxDirection->addItem(ProjectExplorer::PbLibPort::GetPortDirectionName(ProjectExplorer::PbLibPort::pdTx), int(ProjectExplorer::PbLibPort::pdTx));
    m_pComboBoxDirection->addItem(ProjectExplorer::PbLibPort::GetPortDirectionName(ProjectExplorer::PbLibPort::pdRx), int(ProjectExplorer::PbLibPort::pdRx));
    m_pComboBoxDirection->addItem(ProjectExplorer::PbLibPort::GetPortDirectionName(ProjectExplorer::PbLibPort::pdRT), int(ProjectExplorer::PbLibPort::pdRT));
    int iCurrentIndex = m_pComboBoxDirection->findData(int(m_pLibPort->GetPortDirection()));
    m_pComboBoxDirection->setCurrentIndex(iCurrentIndex == -1 ? 0 : iCurrentIndex);

    m_pComboBoxFiberPlug->addItem(ProjectExplorer::PbLibPort::GetFiberPlugName(ProjectExplorer::PbLibPort::fpST), int(ProjectExplorer::PbLibPort::fpST));
    m_pComboBoxFiberPlug->addItem(ProjectExplorer::PbLibPort::GetFiberPlugName(ProjectExplorer::PbLibPort::fpLC), int(ProjectExplorer::PbLibPort::fpLC));
    m_pComboBoxFiberPlug->addItem(ProjectExplorer::PbLibPort::GetFiberPlugName(ProjectExplorer::PbLibPort::fpSC), int(ProjectExplorer::PbLibPort::fpSC));
    m_pComboBoxFiberPlug->addItem(ProjectExplorer::PbLibPort::GetFiberPlugName(ProjectExplorer::PbLibPort::fpFC), int(ProjectExplorer::PbLibPort::fpFC));
    iCurrentIndex = m_pComboBoxFiberPlug->findData(int(m_pLibPort->GetFiberPlug()));
    m_pComboBoxFiberPlug->setCurrentIndex(iCurrentIndex == -1 ? 0 : iCurrentIndex);

    m_pComboBoxFiberMode->addItem(ProjectExplorer::PbLibPort::GetFiberModeName(ProjectExplorer::PbLibPort::fmSingle), int(ProjectExplorer::PbLibPort::fmSingle));
    m_pComboBoxFiberMode->addItem(ProjectExplorer::PbLibPort::GetFiberModeName(ProjectExplorer::PbLibPort::fmMultiple), int(ProjectExplorer::PbLibPort::fmMultiple));
    iCurrentIndex = m_pComboBoxFiberMode->findData(int(m_pLibPort->GetFiberMode()));
    m_pComboBoxFiberMode->setCurrentIndex(iCurrentIndex == -1 ? 0 : iCurrentIndex);

    m_pCheckBoxCascade->setChecked(m_pLibPort->GetCascade());
    m_pCheckBoxCascade->setEnabled(m_pLibPort->GetParentDevice()->GetDeviceType() == ProjectExplorer::PbLibDevice::dtSwitch);

    connect(m_pComboBoxDirection, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentDirectionIndexChanged(int)));
    SlotCurrentDirectionIndexChanged(m_pComboBoxDirection->currentIndex());
}

int PropertyDlgLibFiberPort::GetNewPortNumber() const
{
    return m_pSpinBoxNumber->value();
}

void PropertyDlgLibFiberPort::FillComboBoxGroup()
{
    ProjectExplorer::PbLibBoard *pBoard = m_pLibPort->GetParentBoard();
    Q_ASSERT(pBoard);

    QList<int> lstExistGroups;
    foreach(ProjectExplorer::PbLibPort *pPort, pBoard->GetChildPorts())
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

    if(!m_bNew)
        lstValidGroups.append(m_pLibPort->GetGroup());

    qSort(lstValidGroups);

    foreach(int iGroup, lstValidGroups)
        m_pComboBoxGroup->addItem(QString("%1").arg(iGroup, 2, 10, QLatin1Char('0')), iGroup);

    int iCurrentIndex = m_pComboBoxGroup->findText(QString("%1").arg(m_pLibPort->GetGroup(), 2, 10, QLatin1Char('0')));
    if(iCurrentIndex == -1)
        iCurrentIndex = 0;

    m_pComboBoxGroup->setCurrentIndex(iCurrentIndex);
}

void PropertyDlgLibFiberPort::accept()
{
    m_pLibPort->SetGroup(m_pComboBoxGroup->itemData(m_pComboBoxGroup->currentIndex()).toInt());
    m_pLibPort->SetPortDirection(ProjectExplorer::PbLibPort::PortDirection(m_pComboBoxDirection->itemData(m_pComboBoxDirection->currentIndex()).toInt()));

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

    if(m_pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdTx)
        m_pLibPort->SetName(strTxName);
    else if(m_pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdRx)
        m_pLibPort->SetName(strRxName);
    else if(m_pLibPort->GetPortDirection() == ProjectExplorer::PbLibPort::pdRT)
        m_pLibPort->SetName(strTxName + m_strSpliter + strRxName);
    else
        Q_ASSERT(false);

    m_pLibPort->SetFiberPlug(ProjectExplorer::PbLibPort::FiberPlug(m_pComboBoxFiberPlug->itemData(m_pComboBoxFiberPlug->currentIndex()).toInt()));
    m_pLibPort->SetFiberMode(ProjectExplorer::PbLibPort::FiberMode(m_pComboBoxFiberMode->itemData(m_pComboBoxFiberMode->currentIndex()).toInt()));
    m_pLibPort->SetCascade(m_pCheckBoxCascade->isChecked());

    return QDialog::accept();
}

void PropertyDlgLibFiberPort::SlotCurrentDirectionIndexChanged(int iIndex)
{
    ProjectExplorer::PbLibPort::PortDirection ePortDirection = ProjectExplorer::PbLibPort::PortDirection(m_pComboBoxDirection->itemData(iIndex).toInt());

    if(ePortDirection == ProjectExplorer::PbLibPort::pdTx)
    {
        m_pLineEditNameTx->setEnabled(true);
        m_pLineEditNameRx->setEnabled(false);
    }
    else if(ePortDirection == ProjectExplorer::PbLibPort::pdRx)
    {
        m_pLineEditNameTx->setEnabled(false);
        m_pLineEditNameRx->setEnabled(true);
    }
    else if(ePortDirection == ProjectExplorer::PbLibPort::pdRT)
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
