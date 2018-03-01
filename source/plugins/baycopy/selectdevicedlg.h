#ifndef SELECTDEVICEDLG_H
#define SELECTDEVICEDLG_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeDevice;

} // namespace ProjectExplorer

namespace BayCopy {
namespace Internal {

class SelectDeviceDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    SelectDeviceDlg(ProjectExplorer::PeDevice *pDevice, QWidget *pParent = 0);

// Properties
private:
    ProjectExplorer::PeDevice   *m_pDevice;

    QLineEdit   *m_pLineEditName;
    QComboBox   *m_pComboBoxDeviceCategory;
    QComboBox   *m_pComboBoxBayCategory;
    QComboBox   *m_pComboBoxVLevel;
    QComboBox   *m_pComboBoxCircuitIndex;
    QComboBox   *m_pComboBoxSetIndex;
    QLineEdit   *m_pLineEditDescription;
    QComboBox   *m_pComboBoxManufacture;
    QComboBox   *m_pComboBoxType;
    QLineEdit   *m_pLineEditParentBay;
    QLineEdit   *m_pLineEditParentCubicle;
    QLineEdit   *m_pLineEditCubiclePos;
    QLineEdit   *m_pLineEditCubicleNum;

public slots:
    virtual void accept();

private slots:
    void SlotUpdateName(int iIndex);
};

} // namespace Internal
} // namespace BayCopy

#endif // SELECTDEVICEDLG_H
