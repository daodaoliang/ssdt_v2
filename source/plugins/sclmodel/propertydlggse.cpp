#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QDialogButtonBox>

#include "propertydlggse.h"

using namespace SclModel::Internal;

PropertyDlgGse::PropertyDlgGse(QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("GSE Property"));

    m_pLineEditDevice = new QLineEdit(this);
    m_pLineEditDevice->setMinimumWidth(200);
    m_pLineEditDevice->setEnabled(false);
    m_pLineEditAccessPoint = new QLineEdit(this);
    m_pLineEditAccessPoint->setEnabled(false);
    m_pLineEditLDevice = new QLineEdit(this);
    m_pLineEditLDevice->setEnabled(false);
    m_pLineEditControlBlock = new QLineEdit(this);
    m_pLineEditControlBlock->setEnabled(false);
    m_pLineEditMacAddress = new QLineEdit(this);
    m_pLineEditMacAddress->setInputMask("HH-HH-HH-HH-HH-HH;_");
    m_pLineEditVLanId = new QLineEdit(this);
    m_pLineEditVLanPriority = new QLineEdit(this);
    m_pLineEditAppId = new QLineEdit(this);
    m_pLineEditMinTime = new QLineEdit(this);
    m_pLineEditMaxTime = new QLineEdit(this);

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGroupBox *pGroupBox = new QGroupBox(this);
    QGridLayout *pGridLayout = new QGridLayout(pGroupBox);
    pGridLayout->addWidget(new QLabel(tr("Device:"), this), 0, 0);
    pGridLayout->addWidget(m_pLineEditDevice, 0, 1);
    pGridLayout->addWidget(new QLabel(tr("AccessPoint:"), this), 1, 0);
    pGridLayout->addWidget(m_pLineEditAccessPoint, 1, 1);
    pGridLayout->addWidget(new QLabel(tr("LDevice:"), this), 2, 0);
    pGridLayout->addWidget(m_pLineEditLDevice, 2, 1);
    pGridLayout->addWidget(new QLabel(tr("Control Block:"), this), 3, 0);
    pGridLayout->addWidget(m_pLineEditControlBlock, 3, 1);
    pGridLayout->addWidget(pFrameLineBasic, 4, 0, 1, 2);
    pGridLayout->addWidget(new QLabel(tr("Mac Address:"), this), 5, 0);
    pGridLayout->addWidget(m_pLineEditMacAddress, 5, 1);
    pGridLayout->addWidget(new QLabel(tr("VLan ID:"), this), 6, 0);
    pGridLayout->addWidget(m_pLineEditVLanId, 6, 1);
    pGridLayout->addWidget(new QLabel(tr("VLan Priority:"), this), 7, 0);
    pGridLayout->addWidget(m_pLineEditVLanPriority, 7, 1);
    pGridLayout->addWidget(new QLabel(tr("Application Id:"), this), 8, 0);
    pGridLayout->addWidget(m_pLineEditAppId, 8, 1);
    pGridLayout->addWidget(new QLabel(tr("Minimum Time:"), this), 9, 0);
    pGridLayout->addWidget(m_pLineEditMinTime, 9, 1);
    pGridLayout->addWidget(new QLabel(tr("Maximum Time:"), this), 10, 0);
    pGridLayout->addWidget(m_pLineEditMaxTime, 10, 1);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);
}

void PropertyDlgGse::SetDevice(const QString &strDevice)
{
    m_pLineEditDevice->setText(strDevice);
}

QString PropertyDlgGse::GetDevice() const
{
    return m_pLineEditDevice->text();
}

void PropertyDlgGse::SetAccessPoint(const QString &strAccessPoint)
{
    m_pLineEditAccessPoint->setText(strAccessPoint);
}

QString PropertyDlgGse::GetAccessPoint() const
{
    return m_pLineEditAccessPoint->text();
}

void PropertyDlgGse::SetLDevice(const QString &strLDevice)
{
    m_pLineEditLDevice->setText(strLDevice);
}

QString PropertyDlgGse::GetLDevice() const
{
    return m_pLineEditLDevice->text();
}

void PropertyDlgGse::SetControlBlock(const QString &strControlBlock)
{
    m_pLineEditControlBlock->setText(strControlBlock);
}

QString PropertyDlgGse::GetControlBlock() const
{
    return m_pLineEditControlBlock->text();
}

void PropertyDlgGse::SetMacAddress(const QString &strMacAddress)
{
    m_pLineEditMacAddress->setText(strMacAddress);
}

QString PropertyDlgGse::GetMacAddress() const
{
    return m_pLineEditMacAddress->text();
}

void PropertyDlgGse::SetVLanId(const QString &strVLanId)
{
    m_pLineEditVLanId->setText(strVLanId);
}

QString PropertyDlgGse::GetVLanId() const
{
    return m_pLineEditVLanId->text();
}

void PropertyDlgGse::SetVLanPriority(const QString &strVLanPriority)
{
    m_pLineEditVLanPriority->setText(strVLanPriority);
}

QString PropertyDlgGse::GetVLanPriority() const
{
    return m_pLineEditVLanPriority->text();
}

void PropertyDlgGse::SetAppId(const QString &strAppId)
{
    m_pLineEditAppId->setText(strAppId);
}

QString PropertyDlgGse::GetAppId() const
{
    return m_pLineEditAppId->text();
}

void PropertyDlgGse::SetMinTime(const QString &strMinTime)
{
    m_pLineEditMinTime->setText(strMinTime);
}

QString PropertyDlgGse::GetMinTime() const
{
    return m_pLineEditMinTime->text();
}

void PropertyDlgGse::SetMaxTime(const QString &strMaxTime)
{
    m_pLineEditMaxTime->setText(strMaxTime);
}

QString PropertyDlgGse::GetMaxTime() const
{
    return m_pLineEditMaxTime->text();
}
