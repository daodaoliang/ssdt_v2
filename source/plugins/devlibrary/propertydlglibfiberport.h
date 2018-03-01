#ifndef PROPERTYMDLGLIBFIBERPORT_H
#define PROPERTYMDLGLIBFIBERPORT_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QCheckBox;
class QSpinBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PbLibPort;

} // namespace ProjectExplorer

namespace DevLibrary {
namespace Internal {

class PropertyDlgLibFiberPort : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgLibFiberPort(ProjectExplorer::PbLibPort *pLibPort, bool bNew, QWidget *pParent = 0);

// Operations
public:
    int     GetNewPortNumber() const;

private:
    void    FillComboBoxGroup();

// Properties
public:
    const static QString   m_strSpliter;

private:
    ProjectExplorer::PbLibPort  *m_pLibPort;
    bool                        m_bNew;

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
} // namespace DevLibrary

#endif // PROPERTYMDLGLIBFIBERPORT_H
