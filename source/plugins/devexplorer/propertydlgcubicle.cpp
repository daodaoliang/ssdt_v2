#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/basemanager.h"
#include "projectexplorer/pbmanufacture.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pecubicle.h"

#include "propertydlgcubicle.h"

using namespace DevExplorer::Internal;

PropertyDlgCubicle::PropertyDlgCubicle(ProjectExplorer::PeCubicle *pCubicle, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pCubicle(pCubicle)
{
    m_pLineEditNumber = new QLineEdit(this);
    m_pLineEditNumber->setObjectName(tr("Number"));
    m_pLineEditNumber->setMinimumWidth(200);
    m_pLineEditName = new QLineEdit(this);
    m_pLineEditName->setObjectName(tr("Description"));
    m_pComboBoxManufacture = new QComboBox(this);
    m_pComboBoxManufacture->setObjectName(tr("Manufacture"));
    m_pComboBoxManufacture->setEditable(true);
    m_pComboBoxParentRoom = new QComboBox(this);
    m_pComboBoxParentRoom->setObjectName(tr("Parent Room"));
    m_pComboBoxParentRoom->setEditable(false);

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QFrame* pFrameLineRoom = new QFrame(this);
    pFrameLineRoom->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGroupBox *pGroupBox = new QGroupBox(this);
    QFormLayout *pFormLayout = new QFormLayout(pGroupBox);
    pFormLayout->addRow(m_pLineEditNumber->objectName() + ":", m_pLineEditNumber);
    pFormLayout->addRow(pFrameLineBasic);
    pFormLayout->addRow(m_pLineEditName->objectName() + ":", m_pLineEditName);
    pFormLayout->addRow(m_pComboBoxManufacture->objectName() + ":", m_pComboBoxManufacture);
    pFormLayout->addRow(pFrameLineRoom);
    pFormLayout->addRow(m_pComboBoxParentRoom->objectName() + ":", m_pComboBoxParentRoom);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    QList<ProjectExplorer::PbManufacture*> lstManufactures = ProjectExplorer::BaseManager::Instance()->GetAllManufactures();
    qSort(lstManufactures.begin(), lstManufactures.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    foreach(ProjectExplorer::PbManufacture *pManufacture, lstManufactures)
        m_pComboBoxManufacture->addItem(pManufacture->GetDisplayName());

    m_pComboBoxParentRoom->addItem(ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otRoom), tr("<Yard>"), 0);
    QList<ProjectExplorer::PeRoom*> lstRooms = m_pCubicle->GetProjectVersion()->GetAllRooms();
    qSort(lstRooms.begin(), lstRooms.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    foreach(ProjectExplorer::PeRoom *pRoom, lstRooms)
        m_pComboBoxParentRoom->addItem(pRoom->GetDisplayIcon(), pRoom->GetDisplayName(), reinterpret_cast<int>(pRoom));

    if(m_pCubicle->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        SetWindowTitle(tr("New %1").arg(m_pCubicle->GetObjectTypeName()));
    else
        SetWindowTitle(tr("%1 Property").arg(m_pCubicle->GetObjectTypeName()));

    UpdateData(false);
}

bool PropertyDlgCubicle::UpdateData(bool bSaveAndValidate)
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

        foreach(ProjectExplorer::PeCubicle *pCubicle, m_pCubicle->GetProjectVersion()->GetAllCubicles())
        {
            if(pCubicle->GetId() == m_pCubicle->GetId())
                continue;

            if(strNumber == pCubicle->GetNumber())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The field '%1' can NOT be duplicate, please input a valid value.").arg(m_pLineEditNumber->objectName()));

                m_pLineEditNumber->setFocus();
                return false;
            }
        }

        m_pCubicle->SetNumber(strNumber);
        m_pCubicle->SetName(m_pLineEditName->text().trimmed());
        m_pCubicle->SetManufacture(m_pComboBoxManufacture->currentText().trimmed());
        m_pCubicle->SetParentRoom(reinterpret_cast<ProjectExplorer::PeRoom*>(m_pComboBoxParentRoom->itemData(m_pComboBoxParentRoom->currentIndex()).toInt()));

        if(!m_pCubicle->GetManufacture().isEmpty())
        {
            ProjectExplorer::PbManufacture *pManufactureFound = 0;
            foreach(ProjectExplorer::PbManufacture *pManufacture, ProjectExplorer::BaseManager::Instance()->GetAllManufactures())
            {
                if(pManufacture->GetDisplayName() == m_pCubicle->GetManufacture())
                {
                    pManufactureFound = pManufacture;
                    break;
                }
            }
            if(!pManufactureFound)
            {
                ProjectExplorer::PbManufacture manufacture;
                manufacture.SetName(m_pCubicle->GetManufacture());

                if(ProjectExplorer::BaseManager::Instance()->DbCreateObject(manufacture))
                    ProjectExplorer::BaseManager::Instance()->CreateObject(manufacture, false);
            }
        }
    }
    else
    {
        m_pLineEditNumber->setText(m_pCubicle->GetNumber());
        m_pLineEditName->setText(m_pCubicle->GetName());
        m_pComboBoxManufacture->setEditText(m_pCubicle->GetManufacture());

        int iCurrentIndex = m_pComboBoxParentRoom->findData(reinterpret_cast<int>(m_pCubicle->GetParentRoom()));
        if(iCurrentIndex >= 0)
            m_pComboBoxParentRoom->setCurrentIndex(iCurrentIndex);
    }

    return true;
}

void PropertyDlgCubicle::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}
