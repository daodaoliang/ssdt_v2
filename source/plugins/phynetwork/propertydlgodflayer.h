#ifndef PROPERTYMDLGODFLAYER_H
#define PROPERTYMDLGODFLAYER_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QSpinBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeBoard;

} // namespace ProjectExplorer

namespace PhyNetwork {
namespace Internal {

class PropertyDlgOdfLayer : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgOdfLayer(ProjectExplorer::PeBoard *pLayer, QWidget *pParent = 0);

// Operations
public:
    int GetPortNumber() const;
    int GetFiberPlug() const;

private:
    bool UpdateData(bool bSaveAndValidate);

// Properties
private:
    ProjectExplorer::PeBoard    *m_pLayer;

    QComboBox   *m_pComboBoxParentOdf;
    QComboBox   *m_pComboBoxPosition;
    QSpinBox    *m_pSpinBoxPortNumber;
    QComboBox   *m_pComboBoxFiberPlug;

public slots:
    virtual void accept();

private slots:
    void        SlotParentOdfChanged(int iCurrentIndex);
};

} // namespace Internal
} // namespace PhyNetwork

#endif // PROPERTYMDLGODFLAYER_H
