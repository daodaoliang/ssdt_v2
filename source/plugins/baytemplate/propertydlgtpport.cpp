#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/pbtpport.h"
#include "projectexplorer/pbtpdevice.h"

#include "propertydlgtpport.h"

const int g_iMaxPortGroupIndex = 50;

using namespace BayTemplate::Internal;

const QString PropertyDlgTpPort::m_strSpliter = "@spliter@";
PropertyDlgTpPort::PropertyDlgTpPort(ProjectExplorer::PbTpPort *pTpPort, bool bNew, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pTpPort(pTpPort), m_bNew(bNew)
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

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QFrame* pFrameLineGroup = new QFrame(this);
    pFrameLineGroup->setFrameStyle(QFrame::HLine | QFrame::Sunken);

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
        SetWindowTitle(tr("New %1").arg(m_pTpPort->GetObjectTypeName()));
    }
    else
    {
        SetWindowTitle(tr("%1 Property").arg(m_pTpPort->GetObjectTypeName()));
        m_pComboBoxDirection->setEnabled(false);
    }

    FillComboBoxGroup();

    if(m_pTpPort->GetPortDirection() == ProjectExplorer::PbTpPort::pdTx)
    {
        m_pLineEditNameTx->setText(m_pTpPort->GetName());
    }
    else if(m_pTpPort->GetPortDirection() == ProjectExplorer::PbTpPort::pdRx)
    {
        m_pLineEditNameRx->setText(m_pTpPort->GetName());
    }
    else if(m_pTpPort->GetPortDirection() == ProjectExplorer::PbTpPort::pdRT)
    {
        if(m_bNew)
        {
            m_pLineEditNameTx->setText(ProjectExplorer::PbTpPort::GetPortDirectionName(ProjectExplorer::PbTpPort::pdTx) + QString("%1").arg(m_pComboBoxGroup->itemData(m_pComboBoxGroup->currentIndex()).toInt(), 2, 10, QLatin1Char('0')));
            m_pLineEditNameRx->setText(ProjectExplorer::PbTpPort::GetPortDirectionName(ProjectExplorer::PbTpPort::pdRx) + QString("%1").arg(m_pComboBoxGroup->itemData(m_pComboBoxGroup->currentIndex()).toInt(), 2, 10, QLatin1Char('0')));
        }
        else
        {
            QStringList lstNames = m_pTpPort->GetName().split(m_strSpliter);
            Q_ASSERT(lstNames.size() == 2);

            m_pLineEditNameTx->setText(lstNames.at(0));
            m_pLineEditNameRx->setText(lstNames.at(1));
        }
    }

    m_pComboBoxDirection->addItem(ProjectExplorer::PbTpPort::GetPortDirectionName(ProjectExplorer::PbTpPort::pdTx), int(ProjectExplorer::PbTpPort::pdTx));
    m_pComboBoxDirection->addItem(ProjectExplorer::PbTpPort::GetPortDirectionName(ProjectExplorer::PbTpPort::pdRx), int(ProjectExplorer::PbTpPort::pdRx));
    m_pComboBoxDirection->addItem(ProjectExplorer::PbTpPort::GetPortDirectionName(ProjectExplorer::PbTpPort::pdRT), int(ProjectExplorer::PbTpPort::pdRT));
    int iCurrentIndex = m_pComboBoxDirection->findData(int(m_pTpPort->GetPortDirection()));
    m_pComboBoxDirection->setCurrentIndex(iCurrentIndex == -1 ? 0 : iCurrentIndex);

    connect(m_pComboBoxDirection, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentDirectionIndexChanged(int)));
    SlotCurrentDirectionIndexChanged(m_pComboBoxDirection->currentIndex());
}

int PropertyDlgTpPort::GetNewPortNumber() const
{
    return m_pSpinBoxNumber->value();
}

void PropertyDlgTpPort::FillComboBoxGroup()
{
    ProjectExplorer::PbTpDevice *pTpDevice = m_pTpPort->GetParentDevice();
    Q_ASSERT(pTpDevice);

    QList<int> lstExistGroups;
    foreach(ProjectExplorer::PbTpPort *pPort, pTpDevice->GetChildPorts())
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
        lstValidGroups.append(m_pTpPort->GetGroup());

    qSort(lstValidGroups);

    foreach(int iGroup, lstValidGroups)
        m_pComboBoxGroup->addItem(QString("%1").arg(iGroup, 2, 10, QLatin1Char('0')), iGroup);

    int iCurrentIndex = m_pComboBoxGroup->findText(QString("%1").arg(m_pTpPort->GetGroup(), 2, 10, QLatin1Char('0')));
    if(iCurrentIndex == -1)
        iCurrentIndex = 0;

    m_pComboBoxGroup->setCurrentIndex(iCurrentIndex);
}

void PropertyDlgTpPort::accept()
{
    m_pTpPort->SetGroup(m_pComboBoxGroup->itemData(m_pComboBoxGroup->currentIndex()).toInt());
    m_pTpPort->SetPortDirection(ProjectExplorer::PbTpPort::PortDirection(m_pComboBoxDirection->itemData(m_pComboBoxDirection->currentIndex()).toInt()));

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

    if(m_pTpPort->GetPortDirection() == ProjectExplorer::PbTpPort::pdTx)
        m_pTpPort->SetName(strTxName);
    else if(m_pTpPort->GetPortDirection() == ProjectExplorer::PbTpPort::pdRx)
        m_pTpPort->SetName(strRxName);
    else if(m_pTpPort->GetPortDirection() == ProjectExplorer::PbTpPort::pdRT)
        m_pTpPort->SetName(strTxName + m_strSpliter + strRxName);
    else
        Q_ASSERT(false);

    return QDialog::accept();
}

void PropertyDlgTpPort::SlotCurrentDirectionIndexChanged(int iIndex)
{
    ProjectExplorer::PbTpPort::PortDirection ePortDirection = ProjectExplorer::PbTpPort::PortDirection(m_pComboBoxDirection->itemData(iIndex).toInt());

    if(ePortDirection == ProjectExplorer::PbTpPort::pdTx)
    {
        m_pLineEditNameTx->setEnabled(true);
        m_pLineEditNameRx->setEnabled(false);
    }
    else if(ePortDirection == ProjectExplorer::PbTpPort::pdRx)
    {
        m_pLineEditNameTx->setEnabled(false);
        m_pLineEditNameRx->setEnabled(true);
    }
    else if(ePortDirection == ProjectExplorer::PbTpPort::pdRT)
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
