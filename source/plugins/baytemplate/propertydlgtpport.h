#ifndef PROPERTYMDLGTPPORT_H
#define PROPERTYMDLGTPPORT_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QCheckBox;
class QSpinBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PbTpPort;

} // namespace ProjectExplorer

namespace BayTemplate {
namespace Internal {

class PropertyDlgTpPort : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgTpPort(ProjectExplorer::PbTpPort *pTpPort, bool bNew, QWidget *pParent = 0);

// Operations
public:
    int     GetNewPortNumber() const;

private:
    void    FillComboBoxGroup();

// Properties
public:
    const static QString   m_strSpliter;

private:
    ProjectExplorer::PbTpPort  *m_pTpPort;
    bool                        m_bNew;

    QSpinBox    *m_pSpinBoxNumber;
    QComboBox   *m_pComboBoxGroup;
    QComboBox   *m_pComboBoxDirection;
    QLineEdit   *m_pLineEditNameTx;
    QLineEdit   *m_pLineEditNameRx;

public slots:
    virtual void accept();

private slots:
    void SlotCurrentDirectionIndexChanged(int iIndex);
};

} // namespace Internal
} // namespace BayTemplate

#endif // PROPERTYMDLGTPPORT_H
