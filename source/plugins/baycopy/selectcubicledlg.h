#ifndef SELECTCUBICLEDLG_H
#define SELECTCUBICLEDLG_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeCubicle;

} // namespace ProjectExplorer

namespace BayCopy {
namespace Internal {

class SelectCubicleDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    SelectCubicleDlg(ProjectExplorer::PeCubicle *pCubicle, QWidget *pParent = 0);

// Operations
private:
    bool UpdateData(bool bSaveAndValidate);

// Properties
private:
    ProjectExplorer::PeCubicle  *m_pCubicle;

    QComboBox   *m_pComboBoxCubicle;
    QLineEdit   *m_pLineEditName;
    QLineEdit   *m_pLineEditNumber;
    QComboBox   *m_pComboBoxManufacture;
    QComboBox   *m_pComboBoxParentRoom;

public slots:
    virtual void accept();

private slots:
    void SlotCurrentCubicleChanged(int iCurrentIndex);
};

} // namespace Internal
} // namespace BayCopy

#endif // SELECTCUBICLEDLG_H
