#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/peroom.h"
#include "projectexplorer/peprojectversion.h"

#include "propertydlgroom.h"

using namespace DevExplorer::Internal;

PropertyDlgRoom::PropertyDlgRoom(ProjectExplorer::PeRoom *pRoom, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pRoom(pRoom)
{
    m_pLineEditNumber = new QLineEdit(this);
    m_pLineEditNumber->setObjectName(tr("Number"));
    m_pLineEditNumber->setMinimumWidth(200);
    m_pLineEditName = new QLineEdit(this);
    m_pLineEditName->setObjectName(tr("Description"));
    m_pCheckBoxYard = new QCheckBox(this);
    m_pCheckBoxYard->setObjectName(tr("Yard"));

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGroupBox *pGroupBox = new QGroupBox(this);
    QFormLayout *pFormLayout = new QFormLayout(pGroupBox);
    pFormLayout->addRow(m_pLineEditNumber->objectName() + ":", m_pLineEditNumber);
    pFormLayout->addRow(pFrameLineBasic);
    pFormLayout->addRow(m_pLineEditName->objectName() + ":", m_pLineEditName);
    pFormLayout->addRow(m_pCheckBoxYard->objectName() + ":", m_pCheckBoxYard);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    if(m_pRoom->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        SetWindowTitle(tr("New %1").arg(m_pRoom->GetObjectTypeName()));
    else
        SetWindowTitle(tr("%1 Property").arg(m_pRoom->GetObjectTypeName()));

    UpdateData(false);
}

bool PropertyDlgRoom::UpdateData(bool bSaveAndValidate)
{
    if(bSaveAndValidate)
    {
        const QString strNumber = m_pLineEditNumber->text().trimmed();
        if(strNumber.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pLineEditName->objectName()));

            m_pLineEditNumber->setFocus();
            return false;
        }

        foreach(ProjectExplorer::PeRoom *pRoom, m_pRoom->GetProjectVersion()->GetAllRooms())
        {
            if(pRoom->GetId() == m_pRoom->GetId())
                continue;

            if(strNumber == pRoom->GetNumber())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The field '%1' can NOT be duplicate, please input a valid value.").arg(m_pLineEditNumber->objectName()));

                m_pLineEditNumber->setFocus();
                return false;
            }
        }

        m_pRoom->SetNumber(strNumber);
        m_pRoom->SetName(m_pLineEditName->text().trimmed());
        m_pRoom->SetYard(m_pCheckBoxYard->isChecked());
    }
    else
    {
        m_pLineEditNumber->setText(m_pRoom->GetNumber());
        m_pLineEditName->setText(m_pRoom->GetName());
        m_pCheckBoxYard->setChecked(m_pRoom->GetYard());
    }

    return true;
}

void PropertyDlgRoom::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}
