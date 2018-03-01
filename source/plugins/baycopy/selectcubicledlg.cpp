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

#include "selectcubicledlg.h"

using namespace BayCopy::Internal;

SelectCubicleDlg::SelectCubicleDlg(ProjectExplorer::PeCubicle *pCubicle, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pCubicle(pCubicle)
{
    SetWindowTitle(tr("Select %1").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otCubicle)));

    m_pComboBoxCubicle = new QComboBox(this);
    m_pComboBoxCubicle->setObjectName(tr("Select %1").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otCubicle)));
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
    QFormLayout *pFormLayou = new QFormLayout(pGroupBox);
    pFormLayou->addRow(m_pComboBoxCubicle->objectName() + ":", m_pComboBoxCubicle);
    pFormLayou->addRow(pFrameLineBasic);
    pFormLayou->addRow(m_pLineEditNumber->objectName() + ":", m_pLineEditNumber);
    pFormLayou->addRow(m_pLineEditName->objectName() + ":", m_pLineEditName);
    pFormLayou->addRow(m_pComboBoxManufacture->objectName() + ":", m_pComboBoxManufacture);
    pFormLayou->addWidget(pFrameLineRoom);
    pFormLayou->addRow(m_pComboBoxParentRoom->objectName() + ":", m_pComboBoxParentRoom);

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

    QList<ProjectExplorer::PeCubicle*> lstCubicles = m_pCubicle->GetProjectVersion()->GetAllCubicles();
    qSort(lstCubicles.begin(), lstCubicles.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    int iCurrentCubicle = m_pCubicle->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId ? 0 : -1;
    m_pComboBoxCubicle->addItem("<" + tr("New %1").arg(ProjectExplorer::PeProjectObject::GetObjectTypeName(ProjectExplorer::PeProjectObject::otCubicle)) + ">", 0);
    for(int i = 0; i < lstCubicles.size(); i++)
    {
        ProjectExplorer::PeCubicle *pCubicle = lstCubicles.at(i);

        m_pComboBoxCubicle->addItem(pCubicle->GetDisplayIcon(), pCubicle->GetDisplayName(), reinterpret_cast<int>(pCubicle));
        if(iCurrentCubicle < 0 && pCubicle->GetId() == m_pCubicle->GetId())
            iCurrentCubicle = i + 1;
    }

    m_pComboBoxCubicle->setCurrentIndex(iCurrentCubicle);
    SlotCurrentCubicleChanged(iCurrentCubicle);

    connect(m_pComboBoxCubicle, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentCubicleChanged(int)));
}

void SelectCubicleDlg::accept()
{
    ProjectExplorer::PeCubicle *pCubicle = m_pComboBoxCubicle->currentIndex() > 0 ? reinterpret_cast<ProjectExplorer::PeCubicle*>(m_pComboBoxCubicle->itemData(m_pComboBoxCubicle->currentIndex()).toInt()) : 0;
    if(pCubicle)
    {
        *m_pCubicle = *pCubicle;
    }
    else
    {
        m_pCubicle->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);

        const QString strNumber = m_pLineEditNumber->text().trimmed();
        if(strNumber.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pLineEditNumber->objectName()));

            m_pLineEditNumber->setFocus();
            return;
        }

        foreach(ProjectExplorer::PeCubicle *pCubicle, m_pCubicle->GetProjectVersion()->GetAllCubicles())
        {
            if(pCubicle->GetId() == m_pCubicle->GetId())
                continue;

            if(strNumber == pCubicle->GetName())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The field '%1' can NOT be duplicate, please input a valid value.").arg(m_pLineEditNumber->objectName()));

                m_pLineEditNumber->setFocus();
                return;
            }
        }

        m_pCubicle->SetNumber(strNumber);
        m_pCubicle->SetName(m_pLineEditName->text().trimmed());
        m_pCubicle->SetManufacture(m_pComboBoxManufacture->currentText().trimmed());
        m_pCubicle->SetParentRoom(reinterpret_cast<ProjectExplorer::PeRoom*>(m_pComboBoxParentRoom->itemData(m_pComboBoxParentRoom->currentIndex()).toInt()));
    }

    return QDialog::accept();
}

void SelectCubicleDlg::SlotCurrentCubicleChanged(int iCurrentIndex)
{
    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeCubicle *pCubicle = (iCurrentIndex == 0 ? m_pCubicle : reinterpret_cast<ProjectExplorer::PeCubicle*>(m_pComboBoxCubicle->itemData(iCurrentIndex).toInt()));
    m_pLineEditNumber->setText(pCubicle->GetNumber());
    m_pLineEditName->setText(pCubicle->GetName());
    m_pComboBoxManufacture->setEditText(pCubicle->GetManufacture());
    int iCurrentRoom = m_pComboBoxParentRoom->findData(reinterpret_cast<int>(pCubicle->GetParentRoom()));
    if(iCurrentRoom >= 0)
        m_pComboBoxParentRoom->setCurrentIndex(iCurrentRoom);
    else
        m_pComboBoxParentRoom->setCurrentIndex(0);

    m_pLineEditNumber->setEnabled(iCurrentIndex == 0);
    m_pLineEditName->setEnabled(iCurrentIndex == 0);
    m_pComboBoxManufacture->setEnabled(iCurrentIndex == 0);
    m_pComboBoxParentRoom->setEnabled(iCurrentIndex == 0);
}
