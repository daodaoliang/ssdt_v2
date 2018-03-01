#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/pblibboard.h"
#include "projectexplorer/pblibdevice.h"

#include "propertydlglibboard.h"

using namespace DevLibrary::Internal;

PropertyDlgLibBoard::PropertyDlgLibBoard(ProjectExplorer::PbLibBoard *pLibBoard, bool bNew, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pLibBoard(pLibBoard)
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

    if(bNew)
        SetWindowTitle(tr("New %1").arg(m_pLibBoard->GetObjectTypeName()));
    else
        SetWindowTitle(tr("%1 Property").arg(m_pLibBoard->GetObjectTypeName()));

    UpdateData(false);
}

bool PropertyDlgLibBoard::UpdateData(bool bSaveAndValidate)
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

        foreach(ProjectExplorer::PbLibBoard *pLibBoard, m_pLibBoard->GetParentDevice()->GetChildBoards())
        {
            if(pLibBoard->GetId() == m_pLibBoard->GetId())
                continue;

            if(strPosition == pLibBoard->GetPosition())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The field '%1' can NOT be duplicate, please input a valid value.").arg(m_pLineEditPosition->objectName()));

                m_pLineEditPosition->setFocus();
                return false;
            }
        }

        m_pLibBoard->SetPosition(strPosition);
        m_pLibBoard->SetType(m_pLineEditType->text().trimmed());
        m_pLibBoard->SetDescription(m_pLineEditDescription->text().trimmed());
    }
    else
    {
        m_pLineEditPosition->setText(m_pLibBoard->GetPosition());
        m_pLineEditType->setText(m_pLibBoard->GetType());
        m_pLineEditDescription->setText(m_pLibBoard->GetDescription());
    }

    return true;
}

void PropertyDlgLibBoard::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}
