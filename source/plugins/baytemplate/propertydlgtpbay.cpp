#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/basemanager.h"
#include "projectexplorer/pbtpbay.h"
#include "projectexplorer/pbvlevel.h"

#include "propertydlgtpbay.h"

using namespace BayTemplate::Internal;

PropertyDlgTpBay::PropertyDlgTpBay(ProjectExplorer::PbTpBay *pBay, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pBay(pBay)
{
    m_pLineEditName = new QLineEdit(this);
    m_pLineEditName->setObjectName(tr("Name"));
    m_pLineEditName->setMinimumWidth(200);
    m_pComboBoxVLevel = new QComboBox(this);
    m_pComboBoxVLevel->setObjectName(tr("Voltage Level"));

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGroupBox *pGroupBox = new QGroupBox(this);
    QFormLayout *pFormLayout = new QFormLayout(pGroupBox);
    pFormLayout->addRow(m_pLineEditName->objectName() + ":", m_pLineEditName);
    pFormLayout->addRow(pFrameLineBasic);
    pFormLayout->addRow(m_pComboBoxVLevel->objectName() + ":", m_pComboBoxVLevel);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    QList<ProjectExplorer::PbVLevel*> lstVLevels = ProjectExplorer::BaseManager::Instance()->GetAllVLevels();
    qSort(lstVLevels.begin(), lstVLevels.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    m_pComboBoxVLevel->addItem(tr("None"), 0);
    foreach(ProjectExplorer::PbVLevel *pVLevel, lstVLevels)
        m_pComboBoxVLevel->addItem(pVLevel->GetDisplayName(), pVLevel->GetVLevel());

    if(m_pBay->GetId() == ProjectExplorer::PbBaseObject::m_iInvalidObjectId)
        SetWindowTitle(tr("New %1").arg(m_pBay->GetObjectTypeName()));
    else
        SetWindowTitle(tr("%1 Property").arg(m_pBay->GetObjectTypeName()));

    UpdateData(false);
}

bool PropertyDlgTpBay::UpdateData(bool bSaveAndValidate)
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

        foreach(ProjectExplorer::PbTpBay *pBay, ProjectExplorer::BaseManager::Instance()->GetAllTpBays())
        {
            if(pBay->GetId() == m_pBay->GetId())
                continue;

            if(strName == pBay->GetName())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The field '%1' can NOT be duplicate, please input a valid value.").arg(m_pLineEditName->objectName()));

                m_pLineEditName->setFocus();
                return false;
            }
        }

        int iVLevel = 0;
        if(m_pComboBoxVLevel->currentIndex() >= 0)
            iVLevel = m_pComboBoxVLevel->itemData(m_pComboBoxVLevel->currentIndex()).toInt();

        m_pBay->SetName(m_pLineEditName->text().trimmed());
        m_pBay->SetVLevel(iVLevel);
    }
    else
    {
        m_pLineEditName->setText(m_pBay->GetName());

        int iCurrentIndex = m_pComboBoxVLevel->findData(m_pBay->GetVLevel());
        if(iCurrentIndex >= 0)
            m_pComboBoxVLevel->setCurrentIndex(iCurrentIndex);
    }

    return true;
}

void PropertyDlgTpBay::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}



