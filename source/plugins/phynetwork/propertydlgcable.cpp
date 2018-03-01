#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QToolButton>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/pecable.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pefiber.h"
#include "projectexplorer/pebay.h"

#include "propertydlgcable.h"
#include "cablegenerator.h"

using namespace PhyNetwork::Internal;

PropertyDlgCable::PropertyDlgCable(ProjectExplorer::PeCable *pCable, bool bBatchRename, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pCable(pCable), m_bBatchRename(bBatchRename)
{
    m_pLineEditName = new QLineEdit(this);
    m_pLineEditName->setObjectName(tr("Name"));
    m_pLineEditName->setMinimumWidth(200);
    m_pLineEditName->setEnabled(false);
    m_pComboBoxNameBay = new QComboBox(this);
    m_pComboBoxNameBay->setObjectName(tr("Bay Number"));
    m_pComboBoxNameBay->setEditable(false);
    m_pSpinBoxNameNumber = new QSpinBox(this);
    m_pSpinBoxNameNumber->setObjectName(tr("Number"));
    m_pSpinBoxNameNumber->setRange(1, 999);
    m_pComboBoxNameSet = new QComboBox(this);
    m_pComboBoxNameSet->setObjectName(tr("Set"));
    m_pComboBoxFiberNumber = new QComboBox(this);
    m_pComboBoxFiberNumber->setObjectName(tr("Fiber Number"));
    m_pComboBoxFiberNumber->setEditable(false);

    QToolButton *pToolButtonEdit = new QToolButton(this);
    pToolButtonEdit->setToolButtonStyle(Qt::ToolButtonIconOnly);
    pToolButtonEdit->setToolTip(tr("Customize"));
    pToolButtonEdit->setIcon(QIcon(":/phynetwork/images/oper_edit.png"));
    pToolButtonEdit->setCheckable(true);

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QFrame* pFrameLineFiberNumber = new QFrame(this);
    pFrameLineFiberNumber->setFrameStyle(QFrame::HLine | QFrame::Sunken);

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
    if(m_pCable->GetCableType() != ProjectExplorer::PeCable::ctJump)
        pFormLayout->addRow(m_pComboBoxNameBay->objectName() + ":", m_pComboBoxNameBay);
    else
        m_pComboBoxNameBay->hide();
    pFormLayout->addRow(m_pSpinBoxNameNumber->objectName() + ":", m_pSpinBoxNameNumber);
    if(m_pCable->GetCableType() != ProjectExplorer::PeCable::ctJump)
    {
        pFormLayout->addRow(m_pComboBoxNameSet->objectName() + ":", m_pComboBoxNameSet);

        if(!m_bBatchRename)
        {
            pFormLayout->addRow(pFrameLineFiberNumber);
            pFormLayout->addRow(m_pComboBoxFiberNumber->objectName() + ":", m_pComboBoxFiberNumber);
        }
        else
        {
            pFrameLineFiberNumber->hide();
            m_pComboBoxFiberNumber->hide();
        }
    }
    else
    {
        m_pComboBoxNameSet->hide();
        pFrameLineFiberNumber->hide();
        m_pComboBoxFiberNumber->hide();
    }

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    QStringList lstBayNumbers;
    if(!pCable->GetNameBay().isEmpty())
        lstBayNumbers.append(pCable->GetNameBay());
    foreach(ProjectExplorer::PeBay *pBay, m_pCable->GetProjectVersion()->GetAllBays())
    {
        if(!pBay->GetNumber().isEmpty() && !lstBayNumbers.contains(pBay->GetNumber()))
            lstBayNumbers.append(pBay->GetNumber());
    }
    qSort(lstBayNumbers);
    m_pComboBoxNameBay->addItems(lstBayNumbers);

    QStringList lstSets;
    if(!pCable->GetNameSet().isEmpty())
        lstSets.append(pCable->GetNameSet());
    for(char ch = 'A'; ch <= 'Z'; ch++)
        lstSets.append(QString(ch));
    qSort(lstSets);
    m_pComboBoxNameSet->addItems(lstSets);

    if(m_pCable->GetCableType() == ProjectExplorer::PeCable::ctOptical)
    {
        QList<int> lstFiberNumbers = CableGenerator::Instance()->GetOpticalFiberNumbers();
        const int iCurrentFiberNumber = m_pCable->GetChildFibers().size();
        if(iCurrentFiberNumber > 0 && !lstFiberNumbers.contains(iCurrentFiberNumber))
            lstFiberNumbers.append(iCurrentFiberNumber);
        qSort(lstFiberNumbers);

        foreach(int iNumber, lstFiberNumbers)
            m_pComboBoxFiberNumber->addItem(QString::number(iNumber));
    }
    else if(m_pCable->GetCableType() == ProjectExplorer::PeCable::ctTail)
    {
        QList<int> lstFiberNumbers = CableGenerator::Instance()->GetTailFiberNumbers();
        const int iCurrentFiberNumber = m_pCable->GetChildFibers().size();
        if(iCurrentFiberNumber > 0 && !lstFiberNumbers.contains(iCurrentFiberNumber))
            lstFiberNumbers.append(iCurrentFiberNumber);
        qSort(lstFiberNumbers);

        foreach(int iNumber, lstFiberNumbers)
            m_pComboBoxFiberNumber->addItem(QString::number(iNumber));
    }

    if(m_pCable->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
    {
        SetWindowTitle(tr("New %1").arg(m_pCable->GetCableTypeName(m_pCable->GetCableType())));

        m_pComboBoxNameSet->setEnabled(true);
    }
    else
    {
        SetWindowTitle(tr("%1 Property").arg(m_pCable->GetCableTypeName(m_pCable->GetCableType())));

        m_pComboBoxNameSet->setEnabled(false);
    }

    UpdateData(false);

    connect(m_pComboBoxNameBay, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(SlotUpdateName(QString)));
    connect(m_pComboBoxNameBay, SIGNAL(editTextChanged(QString)),
            this, SLOT(SlotUpdateName(QString)));
    connect(m_pSpinBoxNameNumber, SIGNAL(valueChanged(QString)),
            this, SLOT(SlotUpdateName(QString)));
    connect(m_pComboBoxNameSet, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(SlotUpdateName(QString)));
    connect(m_pComboBoxNameSet, SIGNAL(editTextChanged(QString)),
            this, SLOT(SlotUpdateName(QString)));
    connect(pToolButtonEdit, SIGNAL(toggled(bool)),
            m_pLineEditName, SLOT(setEnabled(bool)));
}

int PropertyDlgCable::GetFiberNumber() const
{
    return m_pComboBoxFiberNumber->currentText().trimmed().toInt();
}

bool PropertyDlgCable::UpdateData(bool bSaveAndValidate)
{
    if(bSaveAndValidate)
    {
        if(m_pCable->GetCableType() != ProjectExplorer::PeCable::ctJump)
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

            const QString strNameBay = m_pComboBoxNameBay->currentText().trimmed();
            if(strNameBay.isEmpty())
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxNameBay->objectName()));

                m_pComboBoxNameBay->setFocus();
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

            m_pCable->SetName(strName);
            m_pCable->SetNameBay(strNameBay);
            m_pCable->SetNameNumber(m_pSpinBoxNameNumber->value());
            m_pCable->SetNameSet(strNameSet);

            if(!m_bBatchRename)
            {
                foreach(ProjectExplorer::PeCable *pCable, m_pCable->GetProjectVersion()->GetAllCables())
                {
                    if(pCable->GetId() == m_pCable->GetId())
                        continue;

                    if(m_pCable->GetName() == pCable->GetName())
                    {
                        QMessageBox::critical(this,
                                              tr("Error"),
                                              tr("The field '%1' can NOT be duplicate, please input a valid value.").arg(m_pLineEditName->objectName()));

                        m_pLineEditName->setFocus();
                        return false;
                    }
                }
            }

            bool bOk;
            const int iFiberNumber = m_pComboBoxFiberNumber->currentText().trimmed().toInt(&bOk);
            if(!bOk)
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The field '%1' has an invalid value, please input a valid value.").arg(m_pComboBoxFiberNumber->objectName()));

                m_pComboBoxFiberNumber->setFocus();
                return false;
            }

            int iRealFiberNumber = 0;
            foreach(ProjectExplorer::PeFiber *pFiber, m_pCable->GetChildFibers())
            {
                if(!pFiber->GetReserve())
                    iRealFiberNumber++;
            }

            if(iFiberNumber < iRealFiberNumber)
            {
                QMessageBox::critical(this,
                                      tr("Error"),
                                      tr("The adjusted fiber number(%1) is less than the real fiber number(%2), please select a valid value.").arg(iFiberNumber).arg(iRealFiberNumber));

                m_pComboBoxFiberNumber->setFocus();
                return false;
            }
        }
        else
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

            m_pCable->SetName(strName);
            m_pCable->SetNameBay("");
            m_pCable->SetNameNumber(m_pSpinBoxNameNumber->value());
            m_pCable->SetNameSet("");
            foreach(ProjectExplorer::PeCable *pCable, m_pCable->GetProjectVersion()->GetAllCables())
            {
                if(pCable->GetId() == m_pCable->GetId())
                    continue;

                if((pCable->GetCubicle1() == m_pCable->GetCubicle1() && pCable->GetCubicle2() == m_pCable->GetCubicle2()) ||
                   (pCable->GetCubicle1() == m_pCable->GetCubicle2() && pCable->GetCubicle2() == m_pCable->GetCubicle1()))
                {
                    if(m_pCable->GetName() == pCable->GetName())
                    {
                        QMessageBox::critical(this,
                                              tr("Error"),
                                              tr("The field '%1' can NOT be duplicate, please input a valid value.").arg(m_pLineEditName->objectName()));

                        m_pLineEditName->setFocus();
                        return false;
                    }
                }
            }
        }
    }
    else
    {
        if(m_pCable->GetCableType() != ProjectExplorer::PeCable::ctJump)
        {
            m_pLineEditName->setText(m_pCable->GetName());

            int iCurrentIndex = m_pComboBoxNameBay->findText(m_pCable->GetNameBay());
            if(iCurrentIndex >= 0)
                m_pComboBoxNameBay->setCurrentIndex(iCurrentIndex);

            m_pSpinBoxNameNumber->setValue(m_pCable->GetNameNumber());

            iCurrentIndex = m_pComboBoxNameSet->findText(m_pCable->GetNameSet());
            if(iCurrentIndex >= 0)
                m_pComboBoxNameSet->setCurrentIndex(iCurrentIndex);

            iCurrentIndex = m_pComboBoxFiberNumber->findText(QString::number(m_pCable->GetChildFibers().size()));
            if(iCurrentIndex >= 0)
                m_pComboBoxFiberNumber->setCurrentIndex(iCurrentIndex);
        }
        else
        {
            m_pLineEditName->setText(m_pCable->GetName());
            m_pSpinBoxNameNumber->setValue(m_pCable->GetNameNumber());
        }
    }

    return true;
}

void PropertyDlgCable::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}

void PropertyDlgCable::SlotUpdateName(const QString &strText)
{
    Q_UNUSED(strText)

    if(!m_pLineEditName->isEnabled())
    {
        if(m_pCable->GetCableType() == ProjectExplorer::PeCable::ctOptical)
        {
            const QString strNameBay = m_pComboBoxNameBay->currentText().trimmed();
            if(strNameBay.isEmpty())
                return;

            const QString strNameSet = m_pComboBoxNameSet->currentText().trimmed();
            if(strNameSet.isEmpty())
                return;

            m_pCable->SetNameBay(strNameBay);
            m_pCable->SetNameNumber(m_pSpinBoxNameNumber->value());
            m_pCable->SetNameSet(strNameSet);
            m_pCable->SetName(m_pCable->CalculateNameByRule(CableGenerator::Instance()->GetOpticalNameRule()));
        }
        else if(m_pCable->GetCableType() == ProjectExplorer::PeCable::ctTail)
        {
            const QString strNameBay = m_pComboBoxNameBay->currentText().trimmed();
            if(strNameBay.isEmpty())
                return;

            const QString strNameSet = m_pComboBoxNameSet->currentText().trimmed();
            if(strNameSet.isEmpty())
                return;

            m_pCable->SetNameBay(strNameBay);
            m_pCable->SetNameNumber(m_pSpinBoxNameNumber->value());
            m_pCable->SetNameSet(strNameSet);
            m_pCable->SetName(m_pCable->CalculateNameByRule(CableGenerator::Instance()->GetTailNameRule()));
        }
        else
        {
            m_pCable->SetNameBay("");
            m_pCable->SetNameNumber(m_pSpinBoxNameNumber->value());
            m_pCable->SetNameSet("");
            m_pCable->SetName(m_pCable->CalculateNameByRule(CableGenerator::Instance()->GetJumpNameRule()));
        }

        m_pLineEditName->setText(m_pCable->GetName());
    }
}


