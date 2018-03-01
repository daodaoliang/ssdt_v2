#ifndef PROPERTYMDLGDEVICE_H
#define PROPERTYMDLGDEVICE_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QStandardItem;
class QItemSelection;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTreeView;

} // namespace Utils

namespace ProjectExplorer {

class PeDevice;
class PbLibDevice;

} // namespace ProjectExplorer

namespace DevExplorer {
namespace Internal {

class ProductModel;
class PropertyDlgDevice : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgDevice(ProjectExplorer::PeDevice *pDevice, ProjectExplorer::PbLibDevice *pLibDevice = 0, QWidget *pParent = 0);

// Operations
public:
    ProjectExplorer::PbLibDevice*   GetLibDevice() const;

private:
    void    BuildProductModel();
    bool    UpdateData(bool bSaveAndValidate);
    void    SetLibDevice(ProjectExplorer::PbLibDevice *pLibDevice);

// Properties
private:
    ProjectExplorer::PeDevice       *m_pDevice;
    ProjectExplorer::PbLibDevice    *m_pLibDevice;

    ProductModel                    *m_pModelProduct;
    Utils::ReadOnlyTreeView         *m_pViewProduct;

    QLineEdit           *m_pLineEditName;
    QComboBox           *m_pComboBoxDeviceCategory;
    QComboBox           *m_pComboBoxBayCategory;
    QComboBox           *m_pComboBoxVLevel;
    QComboBox           *m_pComboBoxCircuitIndex;
    QComboBox           *m_pComboBoxSetIndex;
    QLineEdit           *m_pLineEditDescription;
    QComboBox           *m_pComboBoxManufacture;
    QComboBox           *m_pComboBoxType;
    QComboBox           *m_pComboBoxParentBay;
    QComboBox           *m_pComboBoxParentCubicle;
    QLineEdit           *m_pLineEditCubiclePos;
    QLineEdit           *m_pLineEditCubicleNum;

    QStandardItem       *m_pCurrentSourceItem;

public slots:
    virtual void accept();

private slots:
    void SlotUpdateName(int iIndex);
    void SlotCurrentCubicleIndexChanged(int iIndex);

    friend class ProductModel;
};

} // namespace Internal
} // namespace DevExplorer

#endif // PROPERTYMDLGDEVICE_H
