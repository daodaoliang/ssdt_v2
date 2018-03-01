#include <QApplication>
#include <QGroupBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "dialogbatchedit.h"

using namespace SclModel::Internal;

static bool ValidateIp(const QString &strIp)
{
    QStringList lstIpSections = strIp.split(QLatin1String("."));
    if(lstIpSections.size() != 4)
        return false;

    foreach(const QString strIpSection, lstIpSections)
    {
        bool bConvert = false;
        int iSectionValue = strIpSection.toInt(&bConvert, 10);
        if(!bConvert || iSectionValue < 0 || iSectionValue > 255)
            return false;
    }

    return true;
}

static bool ValidateMac(const QString &strMac)
{
    QStringList lstMacSections = strMac.split(QLatin1String("-"));
    if(lstMacSections.size() != 6)
        return false;

    foreach(const QString strMacSection, lstMacSections)
    {
        bool bConvert = false;
        int iSectionValue = strMacSection.toInt(&bConvert, 16);
        if(!bConvert || iSectionValue < 0 || iSectionValue > 255)
            return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////
// DialogBatchEditAddress member functions
///////////////////////////////////////////////////////////////////////
DialogBatchEditAddress::DialogBatchEditAddress(QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Batch Edit"));

    QGroupBox *pGroupBox = new QGroupBox(tr("Address Parameters"), this);
    QGridLayout *pGridLayoutGroupBox = new QGridLayout(pGroupBox);

    m_pCheckBoxIpAddress = new QCheckBox(tr("IP"), this);
    m_pLineEditIpAddress = new QLineEdit(this);
    m_pLineEditIpAddress->setInputMask("000.000.000.000;_");
    m_pCheckBoxIpSubnet = new QCheckBox(tr("IP-SUBNET"), this);
    m_pLineEditIpSubnet = new QLineEdit(this);
    m_pLineEditIpSubnet->setInputMask("000.000.000.000;_");

    pGridLayoutGroupBox->addWidget(m_pCheckBoxIpAddress, 0, 0);
    pGridLayoutGroupBox->addWidget(m_pLineEditIpAddress, 0, 1);
    pGridLayoutGroupBox->addWidget(m_pCheckBoxIpSubnet, 1, 0);
    pGridLayoutGroupBox->addWidget(m_pLineEditIpSubnet, 1, 1);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    connect(m_pCheckBoxIpAddress, SIGNAL(clicked()), this, SLOT(SlotCheckBoxClicked()));
    connect(m_pCheckBoxIpSubnet, SIGNAL(clicked()), this, SLOT(SlotCheckBoxClicked()));

    SlotCheckBoxClicked();
}

QString DialogBatchEditAddress::GetIpAddress() const
{
    if(m_pCheckBoxIpAddress->checkState() == Qt::Checked)
        return m_pLineEditIpAddress->text().trimmed();

    return "";
}

void DialogBatchEditAddress::SetIpAddress(const QString &strIpAddress)
{
    m_pLineEditIpAddress->setText(strIpAddress);
}

QString DialogBatchEditAddress::GetIpSubnet() const
{
    if(m_pCheckBoxIpSubnet->checkState() == Qt::Checked)
        return m_pLineEditIpSubnet->text().trimmed();

    return "";
}

void DialogBatchEditAddress::SetIpSubnet(const QString &strIpSubnet)
{
    m_pLineEditIpSubnet->setText(strIpSubnet);
}

void DialogBatchEditAddress::SlotCheckBoxClicked()
{
    m_pLineEditIpAddress->setEnabled(m_pCheckBoxIpAddress->checkState() == Qt::Checked);
    m_pLineEditIpSubnet->setEnabled(m_pCheckBoxIpSubnet->checkState() == Qt::Checked);
}

void DialogBatchEditAddress::accept()
{
    bool bHasParameter = (m_pCheckBoxIpAddress->checkState() == Qt::Checked) ||
                         (m_pCheckBoxIpSubnet->checkState() == Qt::Checked);

    if(!bHasParameter)
    {
        QMessageBox::critical(this, tr("Error"), tr("No parameter selected, please select at least one parameter you want to batch edit!"));
        return;
    }

    if(m_pCheckBoxIpAddress->checkState() == Qt::Checked)
    {
        if(!ValidateIp(m_pLineEditIpAddress->text()))
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid value for '%1'").arg(m_pCheckBoxIpAddress->text()));
            return;
        }
    }

    if(m_pCheckBoxIpSubnet->checkState() == Qt::Checked)
    {
        if(!ValidateIp(m_pLineEditIpSubnet->text()))
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid value for '%1'").arg(m_pCheckBoxIpSubnet->text()));
            return;
        }
    }

    return QDialog::accept();
}

///////////////////////////////////////////////////////////////////////
// DialogBatchEditGSE member functions
///////////////////////////////////////////////////////////////////////
DialogBatchEditGSE::DialogBatchEditGSE(QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Batch Edit"));

    QGroupBox *pGroupBox = new QGroupBox(tr("GSE Parameters"), this);
    QGridLayout *pGridLayoutGroupBox = new QGridLayout(pGroupBox);

    m_pCheckBoxMacAddress = new QCheckBox(tr("Mac Address"), this);
    m_pLineEditMacAddress = new QLineEdit(this);
    m_pLineEditMacAddress->setInputMask("HH-HH-HH-HH-HH-HH;_");
    m_pCheckBoxVLanId = new QCheckBox(tr("VLan ID"), this);
    m_pLineEditVLanId = new QLineEdit(this);
    m_pCheckBoxVLanPriority = new QCheckBox(tr("VLan Priority"), this);
    m_pLineEditVLanPriority = new QLineEdit(this);
    m_pCheckBoxAppId = new QCheckBox(tr("Application Id"), this);
    m_pLineEditAppId = new QLineEdit(this);
    m_pCheckBoxMinTime = new QCheckBox(tr("Minimum Time"), this);
    m_pLineEditMinTime = new QLineEdit(this);
    m_pCheckBoxMaxTime = new QCheckBox(tr("Maximum Time"), this);
    m_pLineEditMaxTime = new QLineEdit(this);

    pGridLayoutGroupBox->addWidget(m_pCheckBoxMacAddress, 0, 0);
    pGridLayoutGroupBox->addWidget(m_pLineEditMacAddress, 0, 1);
    pGridLayoutGroupBox->addWidget(m_pCheckBoxVLanId, 1, 0);
    pGridLayoutGroupBox->addWidget(m_pLineEditVLanId, 1, 1);
    pGridLayoutGroupBox->addWidget(m_pCheckBoxVLanPriority, 2, 0);
    pGridLayoutGroupBox->addWidget(m_pLineEditVLanPriority, 2, 1);
    pGridLayoutGroupBox->addWidget(m_pCheckBoxAppId, 3, 0);
    pGridLayoutGroupBox->addWidget(m_pLineEditAppId, 3, 1);
    pGridLayoutGroupBox->addWidget(m_pCheckBoxMinTime, 4, 0);
    pGridLayoutGroupBox->addWidget(m_pLineEditMinTime, 4, 1);
    pGridLayoutGroupBox->addWidget(m_pCheckBoxMaxTime, 5, 0);
    pGridLayoutGroupBox->addWidget(m_pLineEditMaxTime, 5, 1);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    connect(m_pCheckBoxMacAddress, SIGNAL(clicked()), this, SLOT(SlotCheckBoxClicked()));
    connect(m_pCheckBoxVLanId, SIGNAL(clicked()), this, SLOT(SlotCheckBoxClicked()));
    connect(m_pCheckBoxVLanPriority, SIGNAL(clicked()), this, SLOT(SlotCheckBoxClicked()));
    connect(m_pCheckBoxAppId, SIGNAL(clicked()), this, SLOT(SlotCheckBoxClicked()));
    connect(m_pCheckBoxMinTime, SIGNAL(clicked()), this, SLOT(SlotCheckBoxClicked()));
    connect(m_pCheckBoxMaxTime, SIGNAL(clicked()), this, SLOT(SlotCheckBoxClicked()));

    SlotCheckBoxClicked();
}

QString DialogBatchEditGSE::GetMacAddress() const
{
    if(m_pCheckBoxMacAddress->checkState() == Qt::Checked)
        return m_pLineEditMacAddress->text().trimmed();

    return "";
}

void DialogBatchEditGSE::SetMacAddress(const QString &strMacAddress)
{
    m_pLineEditMacAddress->setText(strMacAddress);
}

QString DialogBatchEditGSE::GetVLanId() const
{
    if(m_pCheckBoxVLanId->checkState() == Qt::Checked)
        return m_pLineEditVLanId->text().trimmed();

    return "";
}

void DialogBatchEditGSE::SetVLanId(const QString &strVLanId)
{
    m_pLineEditVLanId->setText(strVLanId);
}

QString DialogBatchEditGSE::GetVLanPriority() const
{
    if(m_pCheckBoxVLanPriority->checkState() == Qt::Checked)
        return m_pLineEditVLanPriority->text().trimmed();

    return "";
}

void DialogBatchEditGSE::SetVLanPriority(const QString &strVLanPriority)
{
    m_pLineEditVLanPriority->setText(strVLanPriority);
}

QString DialogBatchEditGSE::GetAppId() const
{
    if(m_pCheckBoxAppId->checkState() == Qt::Checked)
        return m_pLineEditAppId->text().trimmed();

    return "";
}

void DialogBatchEditGSE::SetAppId(const QString &strAppId)
{
    m_pLineEditAppId->setText(strAppId);
}

QString DialogBatchEditGSE::GetMinTime() const
{
    if(m_pCheckBoxMinTime->checkState() == Qt::Checked)
        return m_pLineEditMinTime->text().trimmed();

    return "";
}

void DialogBatchEditGSE::SetMinTime(const QString &strMinTime)
{
    m_pLineEditMinTime->setText(strMinTime);
}

QString DialogBatchEditGSE::GetMaxTime() const
{
    if(m_pCheckBoxMaxTime->checkState() == Qt::Checked)
        return m_pLineEditMaxTime->text().trimmed();

    return "";
}

void DialogBatchEditGSE::SetMaxTime(const QString &strMaxTime)
{
    m_pLineEditMaxTime->setText(strMaxTime);
}

void DialogBatchEditGSE::SlotCheckBoxClicked()
{
    m_pLineEditMacAddress->setEnabled(m_pCheckBoxMacAddress->checkState() == Qt::Checked);
    m_pLineEditVLanId->setEnabled(m_pCheckBoxVLanId->checkState() == Qt::Checked);
    m_pLineEditVLanPriority->setEnabled(m_pCheckBoxVLanPriority->checkState() == Qt::Checked);
    m_pLineEditAppId->setEnabled(m_pCheckBoxAppId->checkState() == Qt::Checked);
    m_pLineEditMinTime->setEnabled(m_pCheckBoxMinTime->checkState() == Qt::Checked);
    m_pLineEditMaxTime->setEnabled(m_pCheckBoxMaxTime->checkState() == Qt::Checked);
}

void DialogBatchEditGSE::accept()
{
    bool bHasParameter = (m_pCheckBoxMacAddress->checkState() == Qt::Checked) ||
                         (m_pCheckBoxVLanId->checkState() == Qt::Checked) ||
                         (m_pCheckBoxVLanPriority->checkState() == Qt::Checked) ||
                         (m_pCheckBoxAppId->checkState() == Qt::Checked) ||
                         (m_pCheckBoxMinTime->checkState() == Qt::Checked) ||
                         (m_pCheckBoxMaxTime->checkState() == Qt::Checked);

    if(!bHasParameter)
    {
        QMessageBox::critical(this, tr("Error"), tr("No parameter selected, please select at least one parameter you want to batch edit!"));
        return;
    }

    if(m_pCheckBoxMacAddress->checkState() == Qt::Checked)
    {
        if(!ValidateMac(m_pLineEditMacAddress->text()))
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid value for '%1'").arg(m_pCheckBoxMacAddress->text()));
            return;
        }
    }

    if(m_pCheckBoxVLanId->checkState() == Qt::Checked)
    {
        if(m_pLineEditVLanId->text().trimmed().isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid value for '%1'").arg(m_pCheckBoxVLanId->text()));
            return;
        }
    }

    if(m_pCheckBoxVLanPriority->checkState() == Qt::Checked)
    {
        if(m_pLineEditVLanPriority->text().trimmed().isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid value for '%1'").arg(m_pCheckBoxVLanPriority->text()));
            return;
        }
    }

    if(m_pCheckBoxAppId->checkState() == Qt::Checked)
    {
        bool bConvert;
        m_pLineEditAppId->text().trimmed().toInt(&bConvert, 16);
        if(!bConvert)
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid value for '%1'").arg(m_pCheckBoxAppId->text()));
            return;
        }
    }

    if(m_pCheckBoxMinTime->checkState() == Qt::Checked)
    {
        if(m_pLineEditMinTime->text().trimmed().isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid value for '%1'").arg(m_pCheckBoxMinTime->text()));
            return;
        }
    }

    if(m_pCheckBoxMaxTime->checkState() == Qt::Checked)
    {
        if(m_pLineEditMaxTime->text().trimmed().isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid value for '%1'").arg(m_pCheckBoxMaxTime->text()));
            return;
        }
    }

    return QDialog::accept();
}

///////////////////////////////////////////////////////////////////////
// DialogBatchEditSMV member functions
///////////////////////////////////////////////////////////////////////
DialogBatchEditSMV::DialogBatchEditSMV(QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Batch Edit"));

    QGroupBox *pGroupBox = new QGroupBox(tr("SMV Parameters"), this);
    QGridLayout *pGridLayoutGroupBox = new QGridLayout(pGroupBox);

    m_pCheckBoxMacAddress = new QCheckBox(tr("Mac Address"), this);
    m_pLineEditMacAddress = new QLineEdit(this);
    m_pLineEditMacAddress->setInputMask("HH-HH-HH-HH-HH-HH;_");
    m_pCheckBoxVLanId = new QCheckBox(tr("VLan ID"), this);
    m_pLineEditVLanId = new QLineEdit(this);
    m_pCheckBoxVLanPriority = new QCheckBox(tr("VLan Priority"), this);
    m_pLineEditVLanPriority = new QLineEdit(this);
    m_pCheckBoxAppId = new QCheckBox(tr("Application Id"), this);
    m_pLineEditAppId = new QLineEdit(this);

    pGridLayoutGroupBox->addWidget(m_pCheckBoxMacAddress, 0, 0);
    pGridLayoutGroupBox->addWidget(m_pLineEditMacAddress, 0, 1);
    pGridLayoutGroupBox->addWidget(m_pCheckBoxVLanId, 1, 0);
    pGridLayoutGroupBox->addWidget(m_pLineEditVLanId, 1, 1);
    pGridLayoutGroupBox->addWidget(m_pCheckBoxVLanPriority, 2, 0);
    pGridLayoutGroupBox->addWidget(m_pLineEditVLanPriority, 2, 1);
    pGridLayoutGroupBox->addWidget(m_pCheckBoxAppId, 3, 0);
    pGridLayoutGroupBox->addWidget(m_pLineEditAppId, 3, 1);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    connect(m_pCheckBoxMacAddress, SIGNAL(clicked()), this, SLOT(SlotCheckBoxClicked()));
    connect(m_pCheckBoxVLanId, SIGNAL(clicked()), this, SLOT(SlotCheckBoxClicked()));
    connect(m_pCheckBoxVLanPriority, SIGNAL(clicked()), this, SLOT(SlotCheckBoxClicked()));
    connect(m_pCheckBoxAppId, SIGNAL(clicked()), this, SLOT(SlotCheckBoxClicked()));

    SlotCheckBoxClicked();
}

QString DialogBatchEditSMV::GetMacAddress() const
{
    if(m_pCheckBoxMacAddress->checkState() == Qt::Checked)
        return m_pLineEditMacAddress->text().trimmed();

    return "";
}

void DialogBatchEditSMV::SetMacAddress(const QString &strMacAddress)
{
    m_pLineEditMacAddress->setText(strMacAddress);
}

QString DialogBatchEditSMV::GetVLanId() const
{
    if(m_pCheckBoxVLanId->checkState() == Qt::Checked)
        return m_pLineEditVLanId->text().trimmed();

    return "";
}

void DialogBatchEditSMV::SetVLanId(const QString &strVLanId)
{
    m_pLineEditVLanId->setText(strVLanId);
}

QString DialogBatchEditSMV::GetVLanPriority() const
{
    if(m_pCheckBoxVLanPriority->checkState() == Qt::Checked)
        return m_pLineEditVLanPriority->text().trimmed();

    return "";
}

void DialogBatchEditSMV::SetVLanPriority(const QString &strVLanPriority)
{
    m_pLineEditVLanPriority->setText(strVLanPriority);
}

QString DialogBatchEditSMV::GetAppId() const
{
    if(m_pCheckBoxAppId->checkState() == Qt::Checked)
        return m_pLineEditAppId->text().trimmed();

    return "";
}

void DialogBatchEditSMV::SetAppId(const QString &strAppId)
{
    m_pLineEditAppId->setText(strAppId);
}

void DialogBatchEditSMV::SlotCheckBoxClicked()
{
    m_pLineEditMacAddress->setEnabled(m_pCheckBoxMacAddress->checkState() == Qt::Checked);
    m_pLineEditVLanId->setEnabled(m_pCheckBoxVLanId->checkState() == Qt::Checked);
    m_pLineEditVLanPriority->setEnabled(m_pCheckBoxVLanPriority->checkState() == Qt::Checked);
    m_pLineEditAppId->setEnabled(m_pCheckBoxAppId->checkState() == Qt::Checked);
}

void DialogBatchEditSMV::accept()
{
    bool bHasParameter = (m_pCheckBoxMacAddress->checkState() == Qt::Checked) ||
                         (m_pCheckBoxVLanId->checkState() == Qt::Checked) ||
                         (m_pCheckBoxVLanPriority->checkState() == Qt::Checked) ||
                         (m_pCheckBoxAppId->checkState() == Qt::Checked);

    if(!bHasParameter)
    {
        QMessageBox::critical(this, tr("Error"), tr("No parameter selected, please select at least one parameter you want to batch edit!"));
        return;
    }

    if(m_pCheckBoxMacAddress->checkState() == Qt::Checked)
    {
        if(!ValidateMac(m_pLineEditMacAddress->text()))
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid value for '%1'").arg(m_pCheckBoxMacAddress->text()));
            return;
        }
    }

    if(m_pCheckBoxVLanId->checkState() == Qt::Checked)
    {
        if(m_pLineEditVLanId->text().trimmed().isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid value for '%1'").arg(m_pCheckBoxVLanId->text()));
            return;
        }
    }

    if(m_pCheckBoxVLanPriority->checkState() == Qt::Checked)
    {
        if(m_pLineEditVLanPriority->text().trimmed().isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid value for '%1'").arg(m_pCheckBoxVLanPriority->text()));
            return;
        }
    }

    if(m_pCheckBoxAppId->checkState() == Qt::Checked)
    {
        bool bConvert;
        m_pLineEditAppId->text().trimmed().toInt(&bConvert, 16);
        if(!bConvert)
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid value for '%1'").arg(m_pCheckBoxAppId->text()));
            return;
        }
    }

    return QDialog::accept();
}
