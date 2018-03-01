#ifndef PROPERTYMDLGFIBERPORT_H
#define PROPERTYMDLGFIBERPORT_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QCheckBox;
class QSpinBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PePort;

} // namespace ProjectExplorer

namespace DevExplorer {
namespace Internal {

class PropertyDlgFiberPort : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgFiberPort(ProjectExplorer::PePort *pPort, QWidget *pParent = 0);

// Operations
public:
    int     GetNewPortNumber() const;

private:
    void    FillComboBoxGroup();

// Properties
public:
    const static QString   m_strSpliter;

private:
    ProjectExplorer::PePort *m_pPort;

    QSpinBox    *m_pSpinBoxNumber;
    QComboBox   *m_pComboBoxGroup;
    QComboBox   *m_pComboBoxDirection;
    QLineEdit   *m_pLineEditNameTx;
    QLineEdit   *m_pLineEditNameRx;
    QComboBox   *m_pComboBoxFiberPlug;
    QComboBox   *m_pComboBoxFiberMode;
    QCheckBox   *m_pCheckBoxCascade;

public slots:
    virtual void accept();

private slots:
    void SlotCurrentDirectionIndexChanged(int iIndex);
};

} // namespace Internal
} // namespace DevExplorer

#endif // PROPERTYMDLGFIBERPORT_H
