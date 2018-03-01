#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/peboard.h"
#include "projectexplorer/pedevice.h"

#include "propertydlgboard.h"

using namespace DevExplorer::Internal;

PropertyDlgBoard::PropertyDlgBoard(ProjectExplorer::PeBoard *pBoard, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pBoard(pBoard)
{
    m_pLineEditPosition = new QLineEdit(this);
    m_pLineEditPosition->setObjectName(tr("Position"));
    m_pLineEditPosition->setMinimumWidth(200);
    m_pLineEditType = new QLineEdit(this);
    m_pLineEditType->setObjectName(tr("Type"));
    m_pLineEditDescription = new QLineEdit(this);
    m_pLineEditDescription->setObjectName(tr("Description"));

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGroupBox *pGroupBox = new QGroupBox(this);
    QFormLayout *pFormLayout = new QFormLayout(pGroupBox);
    pFormLayout->addRow(m_pLineEditPosition->objectName() + ":", m_pLineEditPosition);
    pFormLayout->addRow(pFrameLineBasic);
    pFormLayout->addRow(m_pLineEditType->objectName(), m_pLineEditType);
    pFormLayout->addRow(m_pLineEditDescription->objectName() + ":", m_pLineEditDescription);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    if(m_pBoard->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        SetWindowTitle(tr("New %1").arg(m_pBoard->GetObjectTypeName()));
    else
        SetWindowTitle(tr("%1 Property").arg(m_pBoard->GetObjectTypeName()));

    UpdateData(false);
}

bool PropertyDlgBoard::UpdateData(bool bSaveAndValidate)
{
    if(bSaveAndValidate)
    {
        const QString strPosition = m_pLineEditPosition->text().trimmed();
        if(strPosition.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pLineEditPosition->objectName()));

            m_pLineEditPosition->setFocus();
            return false;
        }

        foreach(ProjectExplorer::PeBoard *pBoard, m_pBoard->GetParentDevice()->GetChildBoards())
        {
            if(pBoard->GetId() == m_pBoard->GetId())
                continue;

            if(strPosition == pBoard->GetPosition())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The field '%1' can NOT be duplicate, please input a valid value.").arg(m_pLineEditPosition->objectName()));

                m_pLineEditPosition->setFocus();
                return false;
            }
        }

        m_pBoard->SetPosition(strPosition);
        m_pBoard->SetType(m_pLineEditType->text().trimmed());
        m_pBoard->SetDescription(m_pLineEditDescription->text().trimmed());
    }
    else
    {
        m_pLineEditPosition->setText(m_pBoard->GetPosition());
        m_pLineEditType->setText(m_pBoard->GetType());
        m_pLineEditDescription->setText(m_pBoard->GetDescription());
    }

    return true;
}

void PropertyDlgBoard::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}
