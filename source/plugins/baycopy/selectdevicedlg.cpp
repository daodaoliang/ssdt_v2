#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QToolButton>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "projectexplorer/basemanager.h"
#include "projectexplorer/pbmanufacture.h"
#include "projectexplorer/pbdevicecategory.h"
#include "projectexplorer/pbbaycategory.h"
#include "projectexplorer/pbvlevel.h"
#include "projectexplorer/pbdevicetype.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/pecubicle.h"

#include "selectdevicedlg.h"

using namespace BayCopy::Internal;

SelectDeviceDlg::SelectDeviceDlg(ProjectExplorer::PeDevice *pDevice, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pDevice(pDevice)
{
    SetWindowTitle(tr("New %1").arg(ProjectExplorer::PeDevice::GetDeviceTypeName(pDevice->GetDeviceType())));

    m_pLineEditName = new QLineEdit(this);
    m_pLineEditName->setObjectName(tr("Name"));
    m_pLineEditName->setMinimumWidth(200);
    m_pLineEditName->setEnabled(false);
    m_pComboBoxDeviceCategory = new QComboBox(this);
    m_pComboBoxDeviceCategory->setObjectName(tr("Categroy"));
    m_pComboBoxBayCategory = new QComboBox(this);
    m_pComboBoxBayCategory->setObjectName(tr("Object"));
    m_pComboBoxVLevel = new QComboBox(this);
    m_pComboBoxVLevel->setObjectName(tr("Voltage Level"));
    m_pComboBoxCircuitIndex = new QComboBox(this);
    m_pComboBoxCircuitIndex->setObjectName(tr("Circuit Index"));
    m_pComboBoxSetIndex = new QComboBox(this);
    m_pComboBoxSetIndex->setObjectName(tr("Set Index"));
    m_pLineEditDescription = new QLineEdit(this);
    m_pLineEditDescription->setObjectName(tr("Description"));
    m_pComboBoxManufacture = new QComboBox(this);
    m_pComboBoxManufacture->setObjectName(tr("Manufacture"));
    m_pComboBoxManufacture->setEditable(true);
    m_pComboBoxType = new QComboBox(this);
    m_pComboBoxType->setObjectName(tr("Type"));
    m_pComboBoxType->setEditable(true);
    m_pLineEditParentBay = new QLineEdit(this);
    m_pLineEditParentBay->setObjectName(tr("Parent Bay"));
    m_pLineEditParentBay->setEnabled(false);
    m_pLineEditParentCubicle = new QLineEdit(this);
    m_pLineEditParentCubicle->setObjectName(tr("Parent Cubicle"));
    m_pLineEditParentCubicle->setEnabled(false);
    m_pLineEditCubiclePos = new QLineEdit(this);
    m_pLineEditCubiclePos->setObjectName(tr("Cubicle Position"));
    m_pLineEditCubicleNum = new QLineEdit(this);
    m_pLineEditCubicleNum->setObjectName(tr("Cubicle Number"));

    QToolButton *pToolButtonEdit = new QToolButton(this);
    pToolButtonEdit->setToolButtonStyle(Qt::ToolButtonIconOnly);
    pToolButtonEdit->setToolTip(tr("Customize"));
    pToolButtonEdit->setIcon(QIcon(":/devexplorer/images/oper_edit.png"));
    pToolButtonEdit->setCheckable(true);

    QFrame* pFrameLineBasic = new QFrame(this);
    pFrameLineBasic->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QFrame* pFrameLineNameRelated = new QFrame(this);
    pFrameLineNameRelated->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QFrame* pFrameLineBay = new QFrame(this);
    pFrameLineBay->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QFrame* pFrameLineCubicle = new QFrame(this);
    pFrameLineCubicle->setFrameStyle(QFrame::HLine | QFrame::Sunken);

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
    if(m_pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
        pFormLayout->addRow(m_pComboBoxDeviceCategory->objectName() + ":", m_pComboBoxDeviceCategory);
    else
        m_pComboBoxDeviceCategory->hide();
    pFormLayout->addRow(m_pComboBoxBayCategory->objectName() + ":", m_pComboBoxBayCategory);
    pFormLayout->addRow(m_pComboBoxVLevel->objectName() + ":", m_pComboBoxVLevel);
    pFormLayout->addRow(m_pComboBoxCircuitIndex->objectName() + ":", m_pComboBoxCircuitIndex);
    pFormLayout->addRow(m_pComboBoxSetIndex->objectName() + ":", m_pComboBoxSetIndex);
    pFormLayout->addRow(pFrameLineNameRelated);
    pFormLayout->addRow(m_pLineEditDescription->objectName() + ":", m_pLineEditDescription);
    pFormLayout->addRow(m_pComboBoxManufacture->objectName() + ":", m_pComboBoxManufacture);
    pFormLayout->addRow(m_pComboBoxType->objectName() + ":", m_pComboBoxType);
    pFormLayout->addRow(pFrameLineBay);
    pFormLayout->addRow(m_pLineEditParentBay->objectName() + ":", m_pLineEditParentBay);
    pFormLayout->addRow(pFrameLineCubicle);
    pFormLayout->addRow(m_pLineEditParentCubicle->objectName() + ":", m_pLineEditParentCubicle);
    pFormLayout->addRow(m_pLineEditCubiclePos->objectName() + ":", m_pLineEditCubiclePos);
    pFormLayout->addRow(m_pLineEditCubicleNum->objectName() + ":", m_pLineEditCubicleNum);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(pGroupBox);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    QList<ProjectExplorer::PbDeviceCategory*> lstDeviceCategories = ProjectExplorer::BaseManager::Instance()->GetAllDeviceCategories();
    qSort(lstDeviceCategories.begin(), lstDeviceCategories.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    foreach(ProjectExplorer::PbDeviceCategory *pDeviceCategory, lstDeviceCategories)
        m_pComboBoxDeviceCategory->addItem(pDeviceCategory->GetDisplayName(), pDeviceCategory->GetCode());

    QList<ProjectExplorer::PbBayCategory*> lstBayCategories = ProjectExplorer::BaseManager::Instance()->GetAllBayCategories();
    qSort(lstBayCategories.begin(), lstBayCategories.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    foreach(ProjectExplorer::PbBayCategory *pBayCategory, lstBayCategories)
        m_pComboBoxBayCategory->addItem(pBayCategory->GetDisplayName(), pBayCategory->GetCode());

    QList<ProjectExplorer::PbVLevel*> lstVLevels = ProjectExplorer::BaseManager::Instance()->GetAllVLevels();
    qSort(lstVLevels.begin(), lstVLevels.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    m_pComboBoxVLevel->addItem(tr("None"), "");
    foreach(ProjectExplorer::PbVLevel *pVLevel, lstVLevels)
        m_pComboBoxVLevel->addItem(pVLevel->GetDisplayName(), pVLevel->GetCode());

    QStringList lstCircuitIndexes;
    for(int i = 0; i < 99; i++)
        lstCircuitIndexes.append(QString("%1").arg(i, 2, 10, QLatin1Char('0')));
    qSort(lstCircuitIndexes);
    m_pComboBoxCircuitIndex->addItems(lstCircuitIndexes);

    QStringList lstSetIndexes = QStringList() << "";
    char chA = 'A';
    for(int i = 0; i < 26; i++)
        lstSetIndexes.append(QString(chA + i));
    qSort(lstSetIndexes);
    m_pComboBoxSetIndex->addItems(lstSetIndexes);

    QList<ProjectExplorer::PbManufacture*> lstManufactures = ProjectExplorer::BaseManager::Instance()->GetAllManufactures();
    qSort(lstManufactures.begin(), lstManufactures.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    foreach(ProjectExplorer::PbManufacture *pManufacture, lstManufactures)
        m_pComboBoxManufacture->addItem(pManufacture->GetDisplayName());

    QList<ProjectExplorer::PbDeviceType*> lstDeviceTypes = ProjectExplorer::BaseManager::Instance()->GetAllDeviceTypes();
    qSort(lstDeviceTypes.begin(), lstDeviceTypes.end(), ProjectExplorer::PbBaseObject::CompareDisplayName);
    foreach(ProjectExplorer::PbDeviceType *pDeviceType, lstDeviceTypes)
        m_pComboBoxType->addItem(pDeviceType->GetDisplayName());

    m_pLineEditName->setText(m_pDevice->GetName());
    m_pLineEditDescription->setText(m_pDevice->GetDescription());
    m_pComboBoxManufacture->setEditText(m_pDevice->GetManufacture());
    m_pComboBoxType->setEditText(m_pDevice->GetType());
    m_pLineEditParentBay->setText(m_pDevice->GetParentBay() ? m_pDevice->GetParentBay()->GetDisplayName() : "");
    m_pLineEditParentCubicle->setText(m_pDevice->GetParentCubicle() ? m_pDevice->GetParentCubicle()->GetDisplayName() : "");
    m_pLineEditCubiclePos->setText(m_pDevice->GetCubiclePos());
    m_pLineEditCubiclePos->setEnabled(m_pDevice->GetParentCubicle());
    m_pLineEditCubicleNum->setText(m_pDevice->GetCubicleNum());
    m_pLineEditCubicleNum->setEnabled(m_pDevice->GetParentCubicle());

    int iCurrentIndex = m_pComboBoxDeviceCategory->findData(m_pDevice->GetDeviceCategory());
    if(iCurrentIndex >= 0)
        m_pComboBoxDeviceCategory->setCurrentIndex(iCurrentIndex);

    iCurrentIndex = m_pComboBoxBayCategory->findData(m_pDevice->GetBayCategory());
    if(iCurrentIndex >= 0)
        m_pComboBoxBayCategory->setCurrentIndex(iCurrentIndex);

    QString strVLevel;
    foreach(ProjectExplorer::PbVLevel *pVLevel, ProjectExplorer::BaseManager::Instance()->GetAllVLevels())
    {
        if(m_pDevice->GetVLevel() == pVLevel->GetVLevel())
            strVLevel = pVLevel->GetCode();
    }
    iCurrentIndex = m_pComboBoxVLevel->findData(strVLevel);
    if(iCurrentIndex >= 0)
        m_pComboBoxVLevel->setCurrentIndex(iCurrentIndex);

    iCurrentIndex = m_pComboBoxCircuitIndex->findText(m_pDevice->GetCircuitIndex());
    if(iCurrentIndex >= 0)
        m_pComboBoxCircuitIndex->setCurrentIndex(iCurrentIndex);

    iCurrentIndex = m_pComboBoxSetIndex->findText(m_pDevice->GetSet());
    if(iCurrentIndex >= 0)
        m_pComboBoxSetIndex->setCurrentIndex(iCurrentIndex);

    connect(m_pComboBoxDeviceCategory, SIGNAL(activated(int)),
            this, SLOT(SlotUpdateName(int)));
    connect(m_pComboBoxBayCategory, SIGNAL(activated(int)),
            this, SLOT(SlotUpdateName(int)));
    connect(m_pComboBoxVLevel, SIGNAL(activated(int)),
            this, SLOT(SlotUpdateName(int)));
    connect(m_pComboBoxCircuitIndex, SIGNAL(activated(int)),
            this, SLOT(SlotUpdateName(int)));
    connect(m_pComboBoxSetIndex, SIGNAL(activated(int)),
            this, SLOT(SlotUpdateName(int)));
    connect(pToolButtonEdit, SIGNAL(toggled(bool)),
            m_pLineEditName, SLOT(setEnabled(bool)));
}

void SelectDeviceDlg::accept()
{
    const QString strName = m_pLineEditName->text().trimmed();
    if(strName.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The field '%1' can NOT be empty, please input a valid value.").arg(m_pLineEditName->objectName()));

        m_pLineEditName->setFocus();
        return;
    }

    foreach(ProjectExplorer::PeDevice *pDevice, m_pDevice->GetProjectVersion()->GetAllDevices())
    {
        if(pDevice->GetId() == m_pDevice->GetId())
            continue;

        if(strName == pDevice->GetName())
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("The field '%1' can NOT be duplicate, please input a valid value.").arg(m_pLineEditName->objectName()));

            m_pLineEditName->setFocus();
            return;
        }
    }

    m_pDevice->SetName(strName);
    m_pDevice->SetDeviceCategory((m_pComboBoxDeviceCategory->isVisible() && m_pComboBoxDeviceCategory->currentIndex() >= 0) ? m_pComboBoxDeviceCategory->itemData(m_pComboBoxDeviceCategory->currentIndex()).toString() : "W");
    m_pDevice->SetBayCategory(m_pComboBoxBayCategory->currentIndex() >= 0 ? m_pComboBoxBayCategory->itemData(m_pComboBoxBayCategory->currentIndex()).toString() : "");
    m_pDevice->SetCircuitIndex(m_pComboBoxCircuitIndex->currentIndex() >= 0 ? m_pComboBoxCircuitIndex->itemText(m_pComboBoxCircuitIndex->currentIndex()) : "");
    m_pDevice->SetSet(m_pComboBoxSetIndex->currentIndex() >= 0 ? m_pComboBoxSetIndex->itemText(m_pComboBoxSetIndex->currentIndex()) : "");
    m_pDevice->SetDescription(m_pLineEditDescription->text().trimmed());
    m_pDevice->SetManufacture(m_pComboBoxManufacture->currentText().trimmed());
    m_pDevice->SetType(m_pComboBoxType->currentText().trimmed());

    int iVLevel = 0;
    int iCurrentIndex = m_pComboBoxVLevel->currentIndex();
    if(iCurrentIndex >= 0)
    {
        QString strCode = m_pComboBoxVLevel->itemData(iCurrentIndex).toString();
        foreach(ProjectExplorer::PbVLevel *pVLevel, ProjectExplorer::BaseManager::Instance()->GetAllVLevels())
        {
            if(strCode == pVLevel->GetCode())
            {
                iVLevel = pVLevel->GetVLevel();
                break;
            }
        }
    }
    m_pDevice->SetVLevel(iVLevel);

    m_pDevice->SetCubiclePos(m_pLineEditCubiclePos->text().trimmed());
    m_pDevice->SetCubicleNum(m_pLineEditCubicleNum->text().trimmed());

    return QDialog::accept();
}

void SelectDeviceDlg::SlotUpdateName(int iIndex)
{
    Q_UNUSED(iIndex)

    QString strCategroyCode = "W";
    if(m_pComboBoxDeviceCategory->currentIndex() >= 0)
        strCategroyCode = m_pComboBoxDeviceCategory->itemData(m_pComboBoxDeviceCategory->currentIndex()).toString();

    QString strObjectCode;
    if(m_pComboBoxBayCategory->currentIndex() >= 0)
        strObjectCode = m_pComboBoxBayCategory->itemData(m_pComboBoxBayCategory->currentIndex()).toString();

    QString strVLevelCode;
    if(m_pComboBoxVLevel->currentIndex() >= 0)
        strVLevelCode = m_pComboBoxVLevel->itemData(m_pComboBoxVLevel->currentIndex()).toString();

    QString strCircuitIndex;
    if(m_pComboBoxCircuitIndex->currentIndex() >= 0)
        strCircuitIndex = m_pComboBoxCircuitIndex->itemText(m_pComboBoxCircuitIndex->currentIndex());

    QString strSetIndex;
    if(m_pComboBoxSetIndex->currentIndex() >= 0)
        strSetIndex = m_pComboBoxSetIndex->itemText(m_pComboBoxSetIndex->currentIndex());

    m_pLineEditName->setText(strCategroyCode + strObjectCode + strVLevelCode + strCircuitIndex + strSetIndex);
}
