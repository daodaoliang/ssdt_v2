#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QToolButton>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"

#include "propertydlgodf.h"
#include "cablegenerator.h"

using namespace PhyNetwork::Internal;

PropertyDlgOdf::PropertyDlgOdf(ProjectExplorer::PeDevice *pOdf, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pOdf(pOdf)
{
    m_pLineEditName = new QLineEdit(this);
    m_pLineEditName->setObjectName(tr("Name"));
    m_pLineEditName->setMinimumWidth(200);
    m_pLineEditName->setEnabled(false);
    m_pComboBoxNameSet = new QComboBox(this);
    m_pComboBoxNameSet->setObjectName(tr("Set"));

    QToolButton *pToolButtonEdit = new QToolButton(this);
    pToolButtonEdit->setToolButtonStyle(Qt::ToolButtonIconOnly);
    pToolButtonEdit->setToolTip(tr("Customize"));
    pToolButtonEdit->setIcon(QIcon(":/phynetwork/images/oper_edit.png"));
    pToolButtonEdit->setCheckable(true);

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QHBoxLayout *pHBoxLayoutName = new QHBoxLayout;
    pHBoxLayoutName->setContentsMargins(0, 0, 0, 0);
    pHBoxLayoutName->setSpacing(0);
    pHBoxLayoutName->addWidget(m_pLineEditName);
    pHBoxLayoutName->addSpacing(3);
    pHBoxLayoutName->addWidget(pToolButtonEdit);

    QGroupBox *pGroupBox = new QGroupBox(this);
    QFormLayout *pFormLayout = new QFormLayout(pGroupBox);
    pFormLayout->addRow(m_pLineEditName->objectName() + ":", pHBoxLayoutName);
    pFormLayout->addRow(pFrameLineBasic);
    pFormLayout->addRow(m_pComboBoxNameSet->objectName() + ":", m_pComboBoxNameSet);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    QStringList lstSets;
    for(char ch = 'A'; ch <= 'Z'; ch++)
        lstSets.append(QString(ch));
    if(!m_pOdf->GetSet().isEmpty() && !lstSets.contains(m_pOdf->GetSet()))
        lstSets.append(m_pOdf->GetSet());
    qSort(lstSets);
    m_pComboBoxNameSet->addItems(lstSets);

    if(pOdf->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
    {
        SetWindowTitle(tr("New %1").arg(m_pOdf->GetDeviceTypeName(pOdf->GetDeviceType())));
        SlotUpdateName("");
    }
    else
    {
        SetWindowTitle(tr("%1 Property").arg(m_pOdf->GetDeviceTypeName(pOdf->GetDeviceType())));

        m_pComboBoxNameSet->setEnabled(false);
    }

    UpdateData(false);

    connect(m_pComboBoxNameSet, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(SlotUpdateName(QString)));
    connect(m_pComboBoxNameSet, SIGNAL(editTextChanged(QString)),
            this, SLOT(SlotUpdateName(QString)));
    connect(pToolButtonEdit, SIGNAL(toggled(bool)),
            m_pLineEditName, SLOT(setEnabled(bool)));
}

bool PropertyDlgOdf::UpdateData(bool bSaveAndValidate)
{
    if(bSaveAndValidate)
    {
        const QString strName = m_pLineEditName->text().trimmed();
        if(strName.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pLineEditName->objectName()));

            m_pLineEditName->setFocus();
            return false;
        }

        const QString strNameSet = m_pComboBoxNameSet->currentText().trimmed();
        if(strNameSet.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxNameSet->objectName()));

            m_pComboBoxNameSet->setFocus();
            return false;
        }

        m_pOdf->SetName(strName);
        m_pOdf->SetSet(strNameSet);
        foreach(ProjectExplorer::PeDevice *pOdf, m_pOdf->GetParentCubicle()->GetChildDevices())
        {
            if(pOdf == m_pOdf || pOdf->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
                continue;

            if(pOdf->GetName() == m_pOdf->GetName())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The field '%1' can NOT be duplicate, please input a valid value.").arg(m_pLineEditName->objectName()));

                m_pLineEditName->setFocus();
                return false;
            }
        }
    }
    else
    {
        m_pLineEditName->setText(m_pOdf->GetName());
        m_pComboBoxNameSet->setCurrentIndex(m_pComboBoxNameSet->findText(m_pOdf->GetSet()));
    }

    return true;
}

void PropertyDlgOdf::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}

void PropertyDlgOdf::SlotUpdateName(const QString &strText)
{
    Q_UNUSED(strText)

    if(!m_pLineEditName->isEnabled())
    {
        ProjectExplorer::PeCubicle *pCubicle = m_pOdf->GetParentCubicle();
        if(!pCubicle)
            return;

        QList<ProjectExplorer::PeDevice*> lstOdfs;
        foreach(ProjectExplorer::PeDevice *pOdf, pCubicle->GetChildDevices())
        {
            if(pOdf != m_pOdf && pOdf->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                lstOdfs.append(pOdf);
        }

        const QString strNameSet = m_pComboBoxNameSet->currentText().trimmed();
        if(strNameSet.isEmpty())
            return;

        QString strOdfName = CableGenerator::Instance()->GetOdfNameRule();
        strOdfName.replace("%{Set}", strNameSet, Qt::CaseInsensitive);

        ProjectExplorer::PeDevice *pOdfFound = 0;
        foreach(ProjectExplorer::PeDevice *pOdf, lstOdfs)
        {
            if(pOdf->GetName() == strOdfName)
            {
                pOdfFound = pOdf;
                break;
            }
        }

        int iIndex = 1;
        while(pOdfFound)
        {
            pOdfFound = 0;
            QString strNewOdfName = QString("%1%2").arg(strOdfName).arg(iIndex++);

            foreach(ProjectExplorer::PeDevice *pOdf, lstOdfs)
            {
                if(pOdf->GetName() == strNewOdfName)
                {
                    pOdfFound = pOdf;
                    break;
                }
            }

            if(!pOdfFound)
                strOdfName = strNewOdfName;
        }

        m_pOdf->SetSet(strNameSet);
        m_pOdf->SetName(strOdfName);
        m_pLineEditName->setText(m_pOdf->GetName());
    }
}


