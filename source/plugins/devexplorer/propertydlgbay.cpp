#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/basemanager.h"
#include "projectexplorer/pbvlevel.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"

#include "propertydlgbay.h"

using namespace DevExplorer::Internal;

PropertyDlgBay::PropertyDlgBay(ProjectExplorer::PeBay *pBay, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pBay(pBay)
{
    m_pLineEditNumber = new QLineEdit(this);
    m_pLineEditNumber->setObjectName(tr("Number"));
    m_pLineEditNumber->setMinimumWidth(200);
    m_pLineEditName = new QLineEdit(this);
    m_pLineEditName->setObjectName(tr("Description"));
    m_pComboBoxVLevel = new QComboBox(this);
    m_pComboBoxVLevel->setObjectName(tr("Voltage Level"));

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGroupBox *pGroupBox = new QGroupBox(this);
    QFormLayout *pFormLayout = new QFormLayout(pGroupBox);
    pFormLayout->addRow(m_pLineEditNumber->objectName() + ":", m_pLineEditNumber);
    pFormLayout->addRow(pFrameLineBasic);
    pFormLayout->addRow(m_pLineEditName->objectName() + ":", m_pLineEditName);
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

    if(m_pBay->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        SetWindowTitle(tr("New %1").arg(m_pBay->GetObjectTypeName()));
    else
        SetWindowTitle(tr("%1 Property").arg(m_pBay->GetObjectTypeName()));

    UpdateData(false);
}

bool PropertyDlgBay::UpdateData(bool bSaveAndValidate)
{
    if(bSaveAndValidate)
    {
        const QString strNumber = m_pLineEditNumber->text().trimmed();
        if(strNumber.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pLineEditNumber->objectName()));

            m_pLineEditNumber->setFocus();
            return false;
        }

        foreach(ProjectExplorer::PeBay *pBay, m_pBay->GetProjectVersion()->GetAllBays())
        {
            if(pBay->GetId() == m_pBay->GetId())
                continue;

            if(strNumber == pBay->GetNumber())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The field '%1' can NOT be duplicate, please input a valid value.").arg(m_pLineEditNumber->objectName()));

                m_pLineEditNumber->setFocus();
                return false;
            }
        }

        int iVLevel = 0;
        if(m_pComboBoxVLevel->currentIndex() >= 0)
            iVLevel = m_pComboBoxVLevel->itemData(m_pComboBoxVLevel->currentIndex()).toInt();

        m_pBay->SetNumber(strNumber);
        m_pBay->SetName(m_pLineEditName->text().trimmed());
        m_pBay->SetVLevel(iVLevel);
    }
    else
    {
        m_pLineEditNumber->setText(m_pBay->GetNumber());
        m_pLineEditName->setText(m_pBay->GetName());

        int iCurrentIndex = m_pComboBoxVLevel->findData(m_pBay->GetVLevel());
        if(iCurrentIndex >= 0)
            m_pComboBoxVLevel->setCurrentIndex(iCurrentIndex);
    }

    return true;
}

void PropertyDlgBay::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}



