#ifndef PROPERTYMDLGCUBICLE_H
#define PROPERTYMDLGCUBICLE_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeCubicle;

} // namespace ProjectExplorer

namespace DevExplorer {
namespace Internal {

class PropertyDlgCubicle : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgCubicle(ProjectExplorer::PeCubicle *pCubicle, QWidget *pParent = 0);

// Operations
private:
    bool UpdateData(bool bSaveAndValidate);

// Properties
private:
    ProjectExplorer::PeCubicle  *m_pCubicle;

    QLineEdit   *m_pLineEditName;
    QLineEdit   *m_pLineEditNumber;
    QComboBox   *m_pComboBoxManufacture;
    QComboBox   *m_pComboBoxParentRoom;

public slots:
    virtual void accept();
};

} // namespace Internal
} // namespace DevExplorer

#endif // PROPERTYMDLGCUBICLE_H
