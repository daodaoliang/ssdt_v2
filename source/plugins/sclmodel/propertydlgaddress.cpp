#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "propertydlgaddress.h"

using namespace SclModel::Internal;

PropertyDlgAddress::PropertyDlgAddress(QWidget *pParent) :
    StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Address Property"));

    m_pLineEditDevice = new QLineEdit(this);
    m_pLineEditDevice->setMinimumWidth(200);
    m_pLineEditDevice->setEnabled(false);
    m_pLineEditAccessPoint = new QLineEdit(this);
    m_pLineEditAccessPoint->setEnabled(false);
    m_pLineEditIp = new QLineEdit(this);
    m_pLineEditIp->setInputMask("000.000.000.000;_");
    m_pLineEditIpSubnet = new QLineEdit(this);
    m_pLineEditIpSubnet->setInputMask("000.000.000.000;_");

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGroupBox *pGroupBox = new QGroupBox(this);
    QGridLayout *pGridLayout = new QGridLayout(pGroupBox);
    pGridLayout->addWidget(new QLabel(tr("Device:"), this), 0, 0);
    pGridLayout->addWidget(m_pLineEditDevice, 0, 1);
    pGridLayout->addWidget(new QLabel(tr("AccessPoint:"), this), 1, 0);
    pGridLayout->addWidget(m_pLineEditAccessPoint, 1, 1);
    pGridLayout->addWidget(pFrameLineBasic, 2, 0, 1, 2);
    pGridLayout->addWidget(new QLabel(tr("IP:"), this), 3, 0);
    pGridLayout->addWidget(m_pLineEditIp, 3, 1);
    pGridLayout->addWidget(new QLabel(tr("IP-SUBNET:"), this), 4, 0);
    pGridLayout->addWidget(m_pLineEditIpSubnet, 4, 1);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);
}

void PropertyDlgAddress::SetDevice(const QString &strDevice)
{
    m_pLineEditDevice->setText(strDevice);
}

QString PropertyDlgAddress::GetDevice() const
{
    return m_pLineEditDevice->text();
}

void PropertyDlgAddress::SetAccessPoint(const QString &strAccessPoint)
{
    m_pLineEditAccessPoint->setText(strAccessPoint);
}

QString PropertyDlgAddress::GetAccessPoint() const
{
    return m_pLineEditAccessPoint->text();
}

void PropertyDlgAddress::SetIp(const QString &strIp)
{
    m_pLineEditIp->setText(strIp);
}

QString PropertyDlgAddress::GetIp() const
{
    return m_pLineEditIp->text();
}

void PropertyDlgAddress::SetIpSubnet(const QString &strIpSubnet)
{
    m_pLineEditIpSubnet->setText(strIpSubnet);
}

QString PropertyDlgAddress::GetIpSubnet() const
{
    return m_pLineEditIpSubnet->text();
}

