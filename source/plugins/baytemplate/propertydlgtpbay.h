#ifndef PROPERTYMDLGTPBAY_H
#define PROPERTYMDLGTPBAY_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PbTpBay;

} // namespace ProjectExplorer

namespace BayTemplate {
namespace Internal {

class PropertyDlgTpBay : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgTpBay(ProjectExplorer::PbTpBay *pBay, QWidget *pParent = 0);

// Operations
private:
    bool UpdateData(bool bSaveAndValidate);

// Properties
private:
    ProjectExplorer::PbTpBay    *m_pBay;

    QLineEdit   *m_pLineEditName;
    QComboBox   *m_pComboBoxVLevel;

public slots:
    virtual void accept();
};

} // namespace Internal
} // namespace BayTemplate

#endif // PROPERTYMDLGTPBAY_H
