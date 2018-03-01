#ifndef PROPERTYMDLGCABLE_H
#define PROPERTYMDLGCABLE_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QSpinBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeCable;

} // namespace ProjectExplorer

namespace PhyNetwork {
namespace Internal {

class PropertyDlgCable : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgCable(ProjectExplorer::PeCable *pCable, bool bBatchRename = false, QWidget *pParent = 0);

// Operations
public:
    int GetFiberNumber() const;

private:
    bool UpdateData(bool bSaveAndValidate);

// Properties
private:
    ProjectExplorer::PeCable    *m_pCable;

    bool        m_bBatchRename;
    QLineEdit   *m_pLineEditName;
    QComboBox   *m_pComboBoxNameBay;
    QSpinBox    *m_pSpinBoxNameNumber;
    QComboBox   *m_pComboBoxNameSet;
    QComboBox   *m_pComboBoxFiberNumber;

public slots:
    virtual void accept();

private slots:
    void SlotUpdateName(const QString &strText);
};

} // namespace Internal
} // namespace PhyNetwork

#endif // PROPERTYMDLGCABLE_H
