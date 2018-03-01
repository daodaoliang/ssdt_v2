#ifndef PHYNETWORKSETTINGS_H
#define PHYNETWORKSETTINGS_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
class QLineEdit;
class QComboBox;
class QAction;
class QSpinBox;
QT_END_NAMESPACE

namespace PhyNetwork {
namespace Internal {

class PhyNetworkCableSettingsDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PhyNetworkCableSettingsDlg(QWidget *pParent = 0);

// Operations
public:
    virtual void    accept();

private:
    QComboBox   *m_pComboBoxOpticalGroupType;
    QListWidget *m_pListWidgetOpticalFiberNumber;
    QComboBox   *m_pComboBoxOpticalReserveType;
    QLineEdit   *m_pLineEditOpticalReserveRate;
    QAction     *m_pActionOpticalAdd;
    QAction     *m_pActionOpticalEdit;
    QAction     *m_pActionOpticalRemove;

    QComboBox   *m_pComboBoxTailGroupType;
    QListWidget *m_pListWidgetTailFiberNumber;
    QComboBox   *m_pComboBoxTailReserveType;
    QLineEdit   *m_pLineEditTailReserveRate;
    QAction     *m_pActionTailAdd;
    QAction     *m_pActionTailEdit;
    QAction     *m_pActionTailRemove;

    QLineEdit   *m_pLineEditOpticalNameRule;
    QLineEdit   *m_pLineEditTailNameRule;
    QLineEdit   *m_pLineEditJumpNameRule;

    QLineEdit   *m_pLineEditOdfNameRule;
    QSpinBox    *m_pSpinBoxPortNumber;

private slots:
    void SlotCurrentGroupTypeChanged(int iIndex, QObject *pSender = 0);
    void SlotCurrentFiberNumberChanged(int iIndex, QObject *pSender = 0);
    void SlotCurrentFiberNumberDoubleClicked(QListWidgetItem *pItem);
    void SlotActionAdd(QObject *pSender = 0);
    void SlotActionEdit(QObject *pSender = 0);
    void SlotActionRemove(QObject *pSender = 0);
};

} // namespace Internal
} // namespace PhyNetwork

#endif // PHYNETWORKSETTINGS_H
