#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QToolButton>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/peport.h"

#include "propertydlgodflayer.h"
#include "cablegenerator.h"

using namespace PhyNetwork::Internal;

PropertyDlgOdfLayer::PropertyDlgOdfLayer(ProjectExplorer::PeBoard *pLayer, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pLayer(pLayer)
{
    m_pComboBoxParentOdf = new QComboBox(this);
    m_pComboBoxParentOdf->setObjectName(tr("Parent ODF"));
    m_pComboBoxParentOdf->setEditable(false);
    m_pComboBoxPosition = new QComboBox(this);
    m_pComboBoxPosition->setObjectName(tr("Layer Number"));
    m_pComboBoxPosition->setMinimumWidth(200);
    m_pSpinBoxPortNumber = new QSpinBox(this);
    m_pSpinBoxPortNumber->setObjectName(tr("Port Number"));
    m_pSpinBoxPortNumber->setRange(1, 50);
    m_pComboBoxFiberPlug = new QComboBox(this);
    m_pComboBoxFiberPlug->setObjectName(tr("Fiber Plug"));
    m_pComboBoxFiberPlug->setEditable(false);

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGroupBox *pGroupBox = new QGroupBox(this);
    QFormLayout *pFormLayout = new QFormLayout(pGroupBox);
    pFormLayout->addRow(m_pComboBoxParentOdf->objectName() + ":", m_pComboBoxParentOdf);
    pFormLayout->addRow(m_pComboBoxPosition->objectName() + ":", m_pComboBoxPosition);
    pFormLayout->addRow(pFrameLineBasic);
    pFormLayout->addRow(m_pSpinBoxPortNumber->objectName() + ":", m_pSpinBoxPortNumber);
    pFormLayout->addRow(m_pComboBoxFiberPlug->objectName() + ":", m_pComboBoxFiberPlug);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    if(ProjectExplorer::PeDevice *pOdf = m_pLayer->GetParentDevice())
    {
        if(ProjectExplorer::PeCubicle *pCubicle = pOdf->GetParentCubicle())
        {
            foreach(ProjectExplorer::PeDevice *pO, pCubicle->GetChildDevices())
            {
                if(pO->GetDeviceType() == ProjectExplorer::PeDevice::dtODF)
                    m_pComboBoxParentOdf->addItem(pO->GetDisplayIcon(), pO->GetDisplayName(), reinterpret_cast<int>(pO));
            }
        }
    }
    SlotParentOdfChanged(-1);

    m_pComboBoxFiberPlug->addItem(ProjectExplorer::PePort::GetFiberPlugName(ProjectExplorer::PePort::fpST), int(ProjectExplorer::PePort::fpST));
    m_pComboBoxFiberPlug->addItem(ProjectExplorer::PePort::GetFiberPlugName(ProjectExplorer::PePort::fpLC), int(ProjectExplorer::PePort::fpLC));
    m_pComboBoxFiberPlug->addItem(ProjectExplorer::PePort::GetFiberPlugName(ProjectExplorer::PePort::fpSC), int(ProjectExplorer::PePort::fpSC));
    m_pComboBoxFiberPlug->addItem(ProjectExplorer::PePort::GetFiberPlugName(ProjectExplorer::PePort::fpFC), int(ProjectExplorer::PePort::fpFC));

    if(m_pLayer->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
    {
        SetWindowTitle(tr("New ODF Layer"));
    }
    else
    {
        SetWindowTitle(tr("ODF Layer Property"));
    }

    UpdateData(false);

    connect(m_pComboBoxParentOdf, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotParentOdfChanged(int)));
}

int PropertyDlgOdfLayer::GetPortNumber() const
{
    return m_pSpinBoxPortNumber->value();
}

int PropertyDlgOdfLayer::GetFiberPlug() const
{
    return m_pComboBoxFiberPlug->itemData(m_pComboBoxFiberPlug->currentIndex()).toInt();
}

bool PropertyDlgOdfLayer::UpdateData(bool bSaveAndValidate)
{
    if(bSaveAndValidate)
    {
        ProjectExplorer::PeDevice *pParentOdf = 0;
        if(m_pComboBoxParentOdf->currentIndex() >= 0)
            pParentOdf = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxParentOdf->itemData(m_pComboBoxParentOdf->currentIndex()).toInt());
        if(!pParentOdf)
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxParentOdf->objectName()));

            m_pComboBoxParentOdf->setFocus();
            return false;
        }

        const QString strPosition = m_pComboBoxPosition->currentText().trimmed();
        if(strPosition.isEmpty())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pComboBoxPosition->objectName()));

            m_pComboBoxPosition->setFocus();
            return false;
        }

        int iPortNumber = m_pSpinBoxPortNumber->value();
        foreach(ProjectExplorer::PeBoard *pLayer, pParentOdf->GetChildBoards())
        {
            QList<ProjectExplorer::PePort*> lstPorts = pLayer->GetChildPorts();
            qSort(lstPorts.begin(), lstPorts.end(), ProjectExplorer::PePort::CompareGroup);
            const int iOldLayerPortNumber = lstPorts.size();

            if(iOldLayerPortNumber > iPortNumber)
            {
                for(int i = iPortNumber; i < iOldLayerPortNumber; i++)
                {
                    ProjectExplorer::PePort *pPort = lstPorts.at(i);
                    if(!pPort->GetUsedFibers().isEmpty())
                    {
                        QMessageBox::critical(this,
                                              tr("Error"),
                                              tr("The field '%1' can NOT set to '%2' since the ODF layer '%3' contains used port, please input a valid value.").arg(m_pComboBoxPosition->objectName()).arg(iPortNumber).arg(pLayer->GetDisplayName()));

                        m_pComboBoxPosition->setFocus();
                        return false;
                    }
                }
            }
        }

        if(m_pLayer->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        {
            ProjectExplorer::PeDevice *pOdfSource = m_pLayer->GetParentDevice();
            ProjectExplorer::PeDevice *pOdfTarget = pParentOdf;
            if(pOdfSource->GetSet() != pOdfTarget->GetSet())
            {
                if(QMessageBox::question(this,
                                         tr("Confirmation"),
                                         tr("The source %1 '%2' and the target %1 '%3' have different set, are you sure you want to continue?").arg(pOdfSource->GetObjectTypeName()).arg(pOdfSource->GetDisplayName()).arg(pOdfTarget->GetDisplayName()),
                                         QMessageBox::Yes | QMessageBox::No,
                                         QMessageBox::No) != QMessageBox::Yes)
                {
                    return false;
                }
            }
        }

        m_pLayer->SetParentDevice(pParentOdf);
        m_pLayer->SetPosition(strPosition);
    }
    else
    {
        int iCurrentIndex = m_pComboBoxParentOdf->findData(reinterpret_cast<int>(m_pLayer->GetParentDevice()));
        if(iCurrentIndex >= 0)
            m_pComboBoxParentOdf->setCurrentIndex(iCurrentIndex);

        iCurrentIndex = m_pComboBoxPosition->findText(m_pLayer->GetPosition());
        if(iCurrentIndex >= 0)
            m_pComboBoxPosition->setCurrentIndex(iCurrentIndex);

        int iPortNumber = CableGenerator::Instance()->GetOdfLayerPortNumber();
        if(!m_pLayer->GetChildPorts().isEmpty())
            iPortNumber = m_pLayer->GetChildPorts().size();
        m_pSpinBoxPortNumber->setValue(iPortNumber);

        int iFiberPlug = int(ProjectExplorer::PePort::fpST);
        if(!m_pLayer->GetChildPorts().isEmpty())
            iFiberPlug = int(m_pLayer->GetChildPorts().first()->GetFiberPlug());

        iCurrentIndex = m_pComboBoxFiberPlug->findData(iFiberPlug);
        m_pComboBoxFiberPlug->setCurrentIndex(iCurrentIndex == -1 ? 0 : iCurrentIndex);
    }

    return true;
}

void PropertyDlgOdfLayer::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}

void PropertyDlgOdfLayer::SlotParentOdfChanged(int iCurrentIndex)
{
    m_pComboBoxPosition->clear();

    if(iCurrentIndex < 0)
        iCurrentIndex = m_pComboBoxParentOdf->currentIndex();
    if(iCurrentIndex < 0)
        return;

    if(ProjectExplorer::PeDevice *pOdf = reinterpret_cast<ProjectExplorer::PeDevice*>(m_pComboBoxParentOdf->itemData(iCurrentIndex).toInt()))
    {
        QStringList lstAvailablePositions;
        for(char ch = 'A'; ch <= 'Z'; ch++)
            lstAvailablePositions.append(QString(ch));

        foreach(ProjectExplorer::PeBoard *pL, pOdf->GetChildBoards())
        {
            if(pL->GetId() == m_pLayer->GetId())
                continue;

            lstAvailablePositions.removeAll(pL->GetPosition());
        }

        m_pComboBoxPosition->addItems(lstAvailablePositions);
    }
}
