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

#include "selectbaydlg.h"

using namespace BayCopy::Internal;

SelectBayDlg::SelectBayDlg(ProjectExplorer::PeBay *pBay, QWidget *pParent) : StyledUi::StyledDialog(pParent), m_pBay(pBay)
{
    SetWindowTitle(tr("Select %1").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otBay)));

    m_pComboBoxBay = new QComboBox(this);
    m_pComboBoxBay->setObjectName(tr("Select %1").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otBay)));
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
    pFormLayout->addRow(m_pComboBoxBay->objectName() + ":", m_pComboBoxBay);
    pFormLayout->addRow(pFrameLineBasic);
    pFormLayout->addRow(m_pLineEditNumber->objectName() + ":", m_pLineEditNumber);
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

    QList<ProjectExplorer::PeBay*> lstBays = m_pBay->GetProjectVersion()->GetAllBays();
    qSort(lstBays.begin(), lstBays.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    int iCurrentBay = m_pBay->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId ? 0 : -1;
    m_pComboBoxBay->addItem("<" + tr("New %1").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otBay)) + ">", 0);
    for(int i = 0; i < lstBays.size(); i++)
    {
        ProjectExplorer::PeBay *pBay = lstBays.at(i);

        m_pComboBoxBay->addItem(pBay->GetDisplayIcon(), pBay->GetDisplayName(), reinterpret_cast<int>(pBay));
        if(iCurrentBay < 0 && pBay->GetId() == m_pBay->GetId())
            iCurrentBay = i + 1;
    }

    QList<ProjectExplorer::PbVLevel*> lstVLevels = ProjectExplorer::BaseManager::Instance()->GetAllVLevels();
    qSort(lstVLevels.begin(), lstVLevels.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    m_pComboBoxVLevel->addItem(tr("None"), 0);
    foreach(ProjectExplorer::PbVLevel *pVLevel, lstVLevels)
        m_pComboBoxVLevel->addItem(pVLevel->GetDisplayName(), pVLevel->GetVLevel());

    m_pComboBoxBay->setCurrentIndex(iCurrentBay);
    SlotCurrentBayChanged(iCurrentBay);

    connect(m_pComboBoxBay, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentBayChanged(int)));
}

void SelectBayDlg::accept()
{
    ProjectExplorer::PeBay *pBay = m_pComboBoxBay->currentIndex() > 0 ? reinterpret_cast<ProjectExplorer::PeBay*>(m_pComboBoxBay->itemData(m_pComboBoxBay->currentIndex()).toInt()) : 0;
    if(pBay)
    {
        *m_pBay = *pBay;
    }
    else
    {
        m_pBay->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);

        const QString strNumber = m_pLineEditNumber->text().trimmed();
        if(strNumber.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pLineEditNumber->objectName()));

            m_pLineEditNumber->setFocus();
            return;
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
                return;
            }
        }

        int iVLevel = 0;
        if(m_pComboBoxVLevel->currentIndex() >= 0)
            iVLevel = m_pComboBoxVLevel->itemData(m_pComboBoxVLevel->currentIndex()).toInt();

        m_pBay->SetNumber(strNumber);
        m_pBay->SetName(m_pLineEditName->text().trimmed());
        m_pBay->SetVLevel(iVLevel);
    }

    return QDialog::accept();
}

void SelectBayDlg::SlotCurrentBayChanged(int iCurrentIndex)
{
    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeBay *pBay = (iCurrentIndex == 0 ? m_pBay : reinterpret_cast<ProjectExplorer::PeBay*>(m_pComboBoxBay->itemData(iCurrentIndex).toInt()));
    m_pLineEditName->setText(pBay->GetName());
    m_pLineEditNumber->setText(pBay->GetNumber());
    int iCurrentVLevel = m_pComboBoxVLevel->findData(pBay->GetVLevel());
    if(iCurrentVLevel >= 0)
        m_pComboBoxVLevel->setCurrentIndex(iCurrentVLevel);

    m_pLineEditName->setEnabled(iCurrentIndex == 0);
    m_pLineEditNumber->setEnabled(iCurrentIndex == 0);
    m_pComboBoxVLevel->setEnabled(iCurrentIndex == 0);
}
