#ifndef PROPERTYMDLGODF_H
#define PROPERTYMDLGODF_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeDevice;

} // namespace ProjectExplorer

namespace PhyNetwork {
namespace Internal {

class PropertyDlgOdf : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgOdf(ProjectExplorer::PeDevice *pOdf, QWidget *pParent = 0);

// Operations
private:
    bool UpdateData(bool bSaveAndValidate);

// Properties
private:
    ProjectExplorer::PeDevice    *m_pOdf;

    QLineEdit   *m_pLineEditName;
    QComboBox   *m_pComboBoxNameSet;

public slots:
    virtual void accept();

private slots:
    void SlotUpdateName(const QString &strText);
};

} // namespace Internal
} // namespace PhyNetwork

#endif // PROPERTYMDLGODF_H
