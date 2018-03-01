#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QFileIconProvider>
#include <QToolButton>

#include "utils/readonlyview.h"
#include "projectexplorer/basemanager.h"
#include "projectexplorer/pbvlevel.h"
#include "projectexplorer/pbmanufacture.h"
#include "projectexplorer/pbdevicetype.h"
#include "projectexplorer/pbdevicecategory.h"
#include "projectexplorer/pbbaycategory.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/basemanager.h"
#include "projectexplorer/pbmanufacture.h"
#include "projectexplorer/pbdevicecategory.h"
#include "projectexplorer/pbbaycategory.h"
#include "projectexplorer/pblibdevice.h"

#include "propertydlgdevice.h"

namespace DevExplorer {
namespace Internal {

class ProductModel : public QStandardItemModel
{
public:
    ProductModel(int rows, int columns, PropertyDlgDevice *pPropertyDlgDevice) :
        QStandardItemModel(rows, columns, pPropertyDlgDevice), m_pPropertyDlgDevice(pPropertyDlgDevice), m_bHandleData(false)
    {
    }

public:
    void SetLibDevice(ProjectExplorer::PbLibDevice *pLibDevice)
    {
        if(!pLibDevice)
            return;

        for(int i = 0; i < rowCount(); i++)
        {
            QStandardItem *pItemManufacture = item(i);
            for(int j = 0; j < pItemManufacture->rowCount(); j++)
            {
                QStandardItem *pItemCategory = pItemManufacture->child(j);
                for(int k = 0; k < pItemCategory->rowCount(); k++)
                {
                    QStandardItem *pItemType = pItemCategory->child(k);
                    if(reinterpret_cast<ProjectExplorer::PbLibDevice*>(pItemType->data().toInt()) == pLibDevice)
                    {
                        pItemType->setCheckState(Qt::Checked);
                        return;
                    }
                }
            }
        }
    }

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)
    {
        if(data(index, role) == value)
            return true;

        QStandardItemModel::setData(index, value, role);

        if(role == Qt::CheckStateRole && !m_bHandleData)
        {
            QStandardItem *pItem = itemFromIndex(index);
            if(!pItem)
                return false;

            ProjectExplorer::PbLibDevice *pLibDevice = reinterpret_cast<ProjectExplorer::PbLibDevice*>(pItem->data().toInt());
            if(!pLibDevice)
                return false;

            m_bHandleData = true;

            const bool bChecked = pItem->checkState() == Qt::Checked;
            if(bChecked)
            {
                for(int i = 0; i < rowCount(); i++)
                {
                    QStandardItem *pItemManufacture = item(i);
                    for(int j = 0; j < pItemManufacture->rowCount(); j++)
                    {
                        QStandardItem *pItemCategory = pItemManufacture->child(j);
                        for(int k = 0; k < pItemCategory->rowCount(); k++)
                        {
                            QStandardItem *pItemType = pItemCategory->child(k);
                            if(pItemType != pItem && pItemType->checkState() == Qt::Checked)
                            {
                                pItemType->setCheckState(Qt::Unchecked);

                                QFont ft = pItemType->font();
                                ft.setBold(false);
                                pItemType->setFont(ft);
                            }
                        }
                    }
                }
            }

            QFont ft = pItem->font();
            ft.setBold(bChecked);
            pItem->setFont(ft);

            m_bHandleData = false;

            m_pPropertyDlgDevice->SetLibDevice(bChecked ? pLibDevice : 0);
        }

        return true;
    }

private:
    PropertyDlgDevice   *m_pPropertyDlgDevice;
    bool                m_bHandleData;
};

} // namespace DevExplorer
} // namespace Internal

using namespace DevExplorer::Internal;

PropertyDlgDevice::PropertyDlgDevice(ProjectExplorer::PeDevice *pDevice, ProjectExplorer::PbLibDevice *pLibDevice, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pDevice(pDevice), m_pLibDevice(0)
{
    m_pModelProduct = new ProductModel(0, 0, this);
    m_pViewProduct = new Utils::ReadOnlyTreeView(this);
    m_pViewProduct->setIndentation(m_pViewProduct->indentation() * 9/10);
    m_pViewProduct->setUniformRowHeights(true);
    m_pViewProduct->setTextElideMode(Qt::ElideNone);
    m_pViewProduct->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_pViewProduct->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewProduct->setHeaderHidden(true);
#if QT_VERSION >= 0x050000
   m_pViewProduct->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    m_pViewProduct->header()->setResizeMode(QHeaderView::ResizeToContents);
#endif
    m_pViewProduct->header()->setStretchLastSection(false);
    m_pViewProduct->setModel(m_pModelProduct);

    m_pLineEditName = new QLineEdit(this);
    m_pLineEditName->setObjectName(tr("Name"));
    m_pLineEditName->setEnabled(false);
    m_pLineEditName->setMinimumWidth(200);
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
    m_pComboBoxParentBay = new QComboBox(this);
    m_pComboBoxParentBay->setObjectName(tr("Parent Bay"));
    m_pComboBoxParentCubicle = new QComboBox(this);
    m_pComboBoxParentCubicle->setObjectName(tr("Parent Cubicle"));
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

    QFormLayout *pFormLayout = new QFormLayout;
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
    pFormLayout->addRow(m_pComboBoxParentBay->objectName() + ":", m_pComboBoxParentBay);
    pFormLayout->addRow(pFrameLineCubicle);
    pFormLayout->addRow(m_pComboBoxParentCubicle->objectName() + ":", m_pComboBoxParentCubicle);
    pFormLayout->addRow(m_pLineEditCubiclePos->objectName() + ":", m_pLineEditCubiclePos);
    pFormLayout->addRow(m_pLineEditCubicleNum->objectName() + ":", m_pLineEditCubicleNum);

    QGroupBox *pGroupBox = new QGroupBox(this);
    QHBoxLayout *pHBoxLayout = new QHBoxLayout(pGroupBox);
    if(m_pDevice->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
    {
        pHBoxLayout->addWidget(m_pViewProduct);
        pHBoxLayout->addSpacing(10);
    }
    else
    {
        m_pViewProduct->hide();
    }
    pHBoxLayout->addLayout(pFormLayout);

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

    m_pComboBoxParentBay->addItem("", 0);
    QList<ProjectExplorer::PeBay*> lstBays = m_pDevice->GetProjectVersion()->GetAllBays();
    qSort(lstBays.begin(), lstBays.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    foreach(ProjectExplorer::PeBay *pBay, lstBays)
        m_pComboBoxParentBay->addItem(pBay->GetDisplayIcon(), pBay->GetDisplayName(), reinterpret_cast<int>(pBay));

    m_pComboBoxParentCubicle->addItem("" ,0);
    QList<ProjectExplorer::PeCubicle*> lstCubicles = m_pDevice->GetProjectVersion()->GetAllCubicles();
    qSort(lstCubicles.begin(), lstCubicles.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    foreach(ProjectExplorer::PeCubicle *pCubicle, lstCubicles)
        m_pComboBoxParentCubicle->addItem(pCubicle->GetDisplayIcon(), pCubicle->GetDisplayName(), reinterpret_cast<int>(pCubicle));

    if(m_pDevice->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
    {
        SetWindowTitle(tr("New %1").arg(m_pDevice->GetObjectTypeName()));
        SlotUpdateName(0);
    }
    else
    {
        SetWindowTitle(tr("%1 Property").arg(m_pDevice->GetObjectTypeName()));
    }

    UpdateData(false);

    BuildProductModel();
    m_pModelProduct->SetLibDevice(pLibDevice);

    connect(m_pComboBoxDeviceCategory, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotUpdateName(int)));
    connect(m_pComboBoxBayCategory, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotUpdateName(int)));
    connect(m_pComboBoxVLevel, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotUpdateName(int)));
    connect(m_pComboBoxCircuitIndex, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotUpdateName(int)));
    connect(m_pComboBoxSetIndex, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotUpdateName(int)));
    connect(m_pComboBoxParentCubicle, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentCubicleIndexChanged(int)));
    connect(pToolButtonEdit, SIGNAL(toggled(bool)),
            m_pLineEditName, SLOT(setEnabled(bool)));

    SlotCurrentCubicleIndexChanged(m_pComboBoxParentCubicle->currentIndex());
}

ProjectExplorer::PbLibDevice* PropertyDlgDevice::GetLibDevice() const
{
    return m_pLibDevice;
}

void PropertyDlgDevice::BuildProductModel()
{
    QFileIconProvider FileIconProvider;
    foreach(ProjectExplorer::PbLibDevice *pLibDevice, ProjectExplorer::BaseManager::Instance()->GetAllLibDevices())
    {
        if(pLibDevice->GetDeviceType() != m_pDevice->GetDeviceType())
            continue;

        const QString strManufacture = pLibDevice->GetManufacture();

        QStandardItem *pItemManufacture = 0;
        for(int i = 0; i < m_pModelProduct->rowCount(); i++)
        {
            if(m_pModelProduct->item(i)->text() == strManufacture)
            {
                pItemManufacture = m_pModelProduct->item(i);
                break;
            }
        }

        if(!pItemManufacture)
        {
            pItemManufacture = new QStandardItem(FileIconProvider.icon(QFileIconProvider::Folder), strManufacture);

            int iPos = m_pModelProduct->rowCount();
            for(int i = 0; i < m_pModelProduct->rowCount(); i++)
            {
                if(pItemManufacture->text() < m_pModelProduct->item(i)->text())
                {
                    iPos = i;
                    break;
                }
            }
            m_pModelProduct->insertRow(iPos, pItemManufacture);
        }

        const QString strCategory = (pLibDevice->GetDeviceType() == ProjectExplorer::PbLibDevice::dtSwitch ? ProjectExplorer::PbLibDevice::GetDeviceTypeName(ProjectExplorer::PbLibDevice::dtSwitch) : pLibDevice->GetBayCategory() + pLibDevice->GetDeviceCategory());

        QStandardItem *pItemCategory = 0;
        for(int i = 0; i < pItemManufacture->rowCount(); i++)
        {
            if(pItemManufacture->child(i)->text() == strCategory)
            {
                pItemCategory = pItemManufacture->child(i);
                break;
            }
        }

        if(!pItemCategory)
        {
            pItemCategory = new QStandardItem(FileIconProvider.icon(QFileIconProvider::Folder), strCategory);

            int iPos = pItemManufacture->rowCount();
            for(int i = 0; i < pItemManufacture->rowCount(); i++)
            {
                if(pItemCategory->text() < pItemManufacture->child(i)->text())
                {
                    iPos = i;
                    break;
                }
            }
            pItemManufacture->insertRow(iPos, pItemCategory);
        }

        QStandardItem *pItemType = new QStandardItem(pLibDevice->GetDisplayIcon(), pLibDevice->GetDisplayName());
        pItemType->setData(reinterpret_cast<int>(pLibDevice));
        pItemType->setCheckable(true);

        int iPos = pItemCategory->rowCount();
        for(int i = 0; i < pItemCategory->rowCount(); i++)
        {
            if(pItemType->text() < pItemCategory->child(i)->text())
            {
                iPos = i;
                break;
            }
        }

        pItemCategory->insertRow(iPos, pItemType);
    }

    m_pViewProduct->expandAll();
}

bool PropertyDlgDevice::UpdateData(bool bSaveAndValidate)
{
    if(bSaveAndValidate)
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
                return false;
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

        iCurrentIndex = m_pComboBoxParentBay->currentIndex();
        m_pDevice->SetParentBay(iCurrentIndex < 0 ? 0 : reinterpret_cast<ProjectExplorer::PeBay*>(m_pComboBoxParentBay->itemData(iCurrentIndex).toInt()));

        iCurrentIndex = m_pComboBoxParentCubicle->currentIndex();
        m_pDevice->SetParentCubicle(iCurrentIndex < 0 ? 0 : reinterpret_cast<ProjectExplorer::PeCubicle*>(m_pComboBoxParentCubicle->itemData(iCurrentIndex).toInt()));
        if(m_pDevice->GetParentCubicle())
        {
            m_pDevice->SetCubiclePos(m_pLineEditCubiclePos->text().trimmed());
            m_pDevice->SetCubicleNum(m_pLineEditCubicleNum->text().trimmed());
        }
        else
        {
            m_pDevice->SetCubiclePos("");
            m_pDevice->SetCubicleNum("");
        }

        if(!m_pDevice->GetManufacture().isEmpty())
        {
            ProjectExplorer::PbManufacture *pManufactureFound = 0;
            foreach(ProjectExplorer::PbManufacture *pManufacture, ProjectExplorer::BaseManager::Instance()->GetAllManufactures())
            {
                if(pManufacture->GetDisplayName() == m_pDevice->GetManufacture())
                {
                    pManufactureFound = pManufacture;
                    break;
                }
            }
            if(!pManufactureFound)
            {
                ProjectExplorer::PbManufacture manufacture;
                manufacture.SetName(m_pDevice->GetManufacture());

                if(ProjectExplorer::BaseManager::Instance()->DbCreateObject(manufacture))
                    ProjectExplorer::BaseManager::Instance()->CreateObject(manufacture, false);
            }
        }

        if(!m_pDevice->GetType().isEmpty())
        {
            ProjectExplorer::PbDeviceType *pDeviceTypeFound = 0;
            foreach(ProjectExplorer::PbDeviceType *pDeviceType, ProjectExplorer::BaseManager::Instance()->GetAllDeviceTypes())
            {
                if(pDeviceType->GetDisplayName() == m_pDevice->GetType())
                {
                    pDeviceTypeFound = pDeviceType;
                    break;
                }
            }
            if(!pDeviceTypeFound)
            {
                ProjectExplorer::PbDeviceType devicetype;
                devicetype.SetName(m_pDevice->GetType());

                if(ProjectExplorer::BaseManager::Instance()->DbCreateObject(devicetype))
                    ProjectExplorer::BaseManager::Instance()->CreateObject(devicetype, false);
            }
        }
    }
    else
    {
        m_pLineEditName->setText(m_pDevice->GetName());
        m_pLineEditDescription->setText(m_pDevice->GetDescription());
        m_pComboBoxManufacture->setEditText(m_pDevice->GetManufacture());
        m_pComboBoxType->setEditText(m_pDevice->GetType());
        m_pLineEditCubiclePos->setText(m_pDevice->GetCubiclePos());
        m_pLineEditCubicleNum->setText(m_pDevice->GetCubicleNum());

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

        iCurrentIndex = m_pComboBoxParentBay->findData(reinterpret_cast<int>(m_pDevice->GetParentBay()));
        if(iCurrentIndex >= 0)
            m_pComboBoxParentBay->setCurrentIndex(iCurrentIndex);

        iCurrentIndex = m_pComboBoxParentCubicle->findData(reinterpret_cast<int>(m_pDevice->GetParentCubicle()));
        if(iCurrentIndex >= 0)
            m_pComboBoxParentCubicle->setCurrentIndex(iCurrentIndex);

        m_pLineEditCubiclePos->setText(m_pDevice->GetCubiclePos());
        m_pLineEditCubicleNum->setText(m_pDevice->GetCubicleNum());
    }

    return true;
}

void PropertyDlgDevice::SetLibDevice(ProjectExplorer::PbLibDevice *pLibDevice)
{
    if(m_pLibDevice == pLibDevice)
        return;

    m_pLibDevice = pLibDevice;
    if(pLibDevice)
    {
        if(m_pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
        {
            int iIndex = m_pComboBoxDeviceCategory->findText(m_pLibDevice->GetDeviceCategory());
            if(iIndex >= 0)
                m_pComboBoxDeviceCategory->setCurrentIndex(iIndex);

            iIndex = m_pComboBoxBayCategory->findText(m_pLibDevice->GetBayCategory());
            if(iIndex >= 0)
                m_pComboBoxBayCategory->setCurrentIndex(iIndex);
        }

        m_pComboBoxManufacture->setEditText(m_pLibDevice->GetManufacture());
        m_pComboBoxType->setEditText(m_pLibDevice->GetType());
    }

    if(m_pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
    {
        m_pComboBoxDeviceCategory->setEnabled(!m_pLibDevice);
        m_pComboBoxBayCategory->setEnabled(!m_pLibDevice);
    }
    m_pComboBoxManufacture->setEnabled(!m_pLibDevice);
    m_pComboBoxType->setEnabled(!m_pLibDevice);
}

void PropertyDlgDevice::accept()
{
    if(UpdateData(true))
        return QDialog::accept();
}

void PropertyDlgDevice::SlotUpdateName(int iIndex)
{
    Q_UNUSED(iIndex)

    if(!m_pLineEditName->isEnabled())
    {
        QString strCategroyCode = "W";
        if(m_pComboBoxDeviceCategory->isVisible() && m_pComboBoxDeviceCategory->currentIndex() >= 0)
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
}

void PropertyDlgDevice::SlotCurrentCubicleIndexChanged(int iIndex)
{
    m_pLineEditCubiclePos->setEnabled(iIndex > 0);
    m_pLineEditCubicleNum->setEnabled(iIndex > 0);
}
