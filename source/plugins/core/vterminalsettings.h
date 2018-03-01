#ifndef VTERMINALSETTINGS_H
#define VTERMINALSETTINGS_H

#include "ioptionspage.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

namespace Core {
namespace Internal {

class VTerminalSettings : public Core::IOptionsPage
{
    Q_OBJECT

// Construction and Destruction
public:
    VTerminalSettings();

// Operations
public:
    virtual QString     GetID() const;
    virtual QString     GetDisplayName() const;
    virtual QIcon       GetDisplayIcon() const;

    virtual QWidget*    CreatePage(QWidget *pParent);
    virtual void        Apply();
    virtual void        Finish();

private:
    QLineEdit   *m_pLineEditGooseTxAccessPoint;
    QLineEdit   *m_pLineEditGooseTxLDevice;
    QLineEdit   *m_pLineEditGooseTxLNode;
    QLineEdit   *m_pLineEditGooseTxDataSet;
    QLineEdit   *m_pLineEditGooseTxFCDA;

    QLineEdit   *m_pLineEditGooseRxAccessPoint;
    QLineEdit   *m_pLineEditGooseRxLDevice;
    QLineEdit   *m_pLineEditGooseRxLNode;
    QLineEdit   *m_pLineEditGooseRxFC;
    QLineEdit   *m_pLineEditGooseRxDO;
    QLineEdit   *m_pLineEditGooseRxDA;

    QLineEdit   *m_pLineEditSvTxAccessPoint;
    QLineEdit   *m_pLineEditSvTxLDevice;
    QLineEdit   *m_pLineEditSvTxLNode;
    QLineEdit   *m_pLineEditSvTxDataSet;
    QLineEdit   *m_pLineEditSvTxFCDA;

    QLineEdit   *m_pLineEditSvRxAccessPoint;
    QLineEdit   *m_pLineEditSvRxLDevice;
    QLineEdit   *m_pLineEditSvRxLNode;
    QLineEdit   *m_pLineEditSvRxFC;
    QLineEdit   *m_pLineEditSvRxDO;
    QLineEdit   *m_pLineEditSvRxDA;
};

} // namespace Internal
} // namespace Core

#endif // VTERMINALSETTINGS_H
