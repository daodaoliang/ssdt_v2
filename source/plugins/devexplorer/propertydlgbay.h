#ifndef PROPERTYMDLGBAY_H
#define PROPERTYMDLGBAY_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeBay;

} // namespace ProjectExplorer

namespace DevExplorer {
namespace Internal {

class PropertyDlgBay : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgBay(ProjectExplorer::PeBay *pBay, QWidget *pParent = 0);

// Operations
private:
    bool UpdateData(bool bSaveAndValidate);

// Properties
private:
    ProjectExplorer::PeBay  *m_pBay;

    QLineEdit   *m_pLineEditName;
    QLineEdit   *m_pLineEditNumber;
    QComboBox   *m_pComboBoxVLevel;

public slots:
    virtual void accept();
};

} // namespace Internal
} // namespace DevExplorer

#endif // PROPERTYMDLGBAY_H
