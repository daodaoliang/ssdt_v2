#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QVBoxLayout>

#include "projectexplorer/peprojectobject.h"

#include "vterminalsettings.h"
#include "vterminaldefine.h"

using namespace Core::Internal;

///////////////////////////////////////////////////////////////////////
// VTerminalSettings member functions
///////////////////////////////////////////////////////////////////////
VTerminalSettings::VTerminalSettings() : Core::IOptionsPage(0)
{
}

QString VTerminalSettings::GetID() const
{
    return QLatin1String("Option.Page.1.VTerminal");
}

QString VTerminalSettings::GetDisplayName() const
{
    return tr("Virtual Terminal Define");
}

QIcon VTerminalSettings::GetDisplayIcon() const
{
    return ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otVTerminal);
}

QWidget* VTerminalSettings::CreatePage(QWidget *pParent)
{
    QWidget *pWidget = new QWidget(pParent);
    QGridLayout *pGridLayoutMain = new QGridLayout(pWidget);

    QGroupBox *pGroupBoxGooseTx = new QGroupBox(tr("Goose Tx"), pWidget);
    pGroupBoxGooseTx->setMinimumWidth(300);
    QGridLayout *pGridLayoutGooseTx = new QGridLayout(pGroupBoxGooseTx);

    QGroupBox *pGroupBoxGooseRx = new QGroupBox(tr("Goose Rx"), pWidget);
    pGroupBoxGooseRx->setMinimumWidth(300);
    QGridLayout *pGridLayoutGooseRx = new QGridLayout(pGroupBoxGooseRx);

    QGroupBox *pGroupBoxSvTx = new QGroupBox(tr("Sv Tx"), pWidget);
    pGroupBoxSvTx->setMinimumWidth(300);
    QGridLayout *pGridLayoutSvTx = new QGridLayout(pGroupBoxSvTx);

    QGroupBox *pGroupBoxSvRx = new QGroupBox(tr("Sv Rx"), pWidget);
    pGroupBoxSvRx->setMinimumWidth(300);
    QGridLayout *pGridLayoutSvRx = new QGridLayout(pGroupBoxSvRx);

    pGridLayoutMain->addWidget(pGroupBoxGooseTx, 0, 0);
    pGridLayoutMain->addWidget(pGroupBoxSvTx, 0, 1);
    pGridLayoutMain->addWidget(pGroupBoxGooseRx, 1, 0);
    pGridLayoutMain->addWidget(pGroupBoxSvRx, 1, 1);

    // Goose Tx
    QLabel *pLabelGooseTxAccessPoint = new QLabel(tr("AccessPoint") + ":", pWidget);
    m_pLineEditGooseTxAccessPoint = new QLineEdit(pWidget);
    pLabelGooseTxAccessPoint->setBuddy(m_pLineEditGooseTxAccessPoint);
    pGridLayoutGooseTx->addWidget(pLabelGooseTxAccessPoint, 0, 0);
    pGridLayoutGooseTx->addWidget(m_pLineEditGooseTxAccessPoint, 0, 1);

    QLabel *pLabelGooseTxLDevice = new QLabel(tr("LDevice") + ":", pWidget);
    m_pLineEditGooseTxLDevice = new QLineEdit(pWidget);
    pLabelGooseTxLDevice->setBuddy(m_pLineEditGooseTxLDevice);
    pGridLayoutGooseTx->addWidget(pLabelGooseTxLDevice, 1, 0);
    pGridLayoutGooseTx->addWidget(m_pLineEditGooseTxLDevice, 1, 1);

    QLabel *pLabelGooseTxLNode = new QLabel(tr("LNode") + ":", pWidget);
    m_pLineEditGooseTxLNode = new QLineEdit(pWidget);
    pLabelGooseTxLNode->setBuddy(m_pLineEditGooseTxLNode);
    pGridLayoutGooseTx->addWidget(pLabelGooseTxLNode, 2, 0);
    pGridLayoutGooseTx->addWidget(m_pLineEditGooseTxLNode, 2, 1);

    QLabel *pLabelGooseTxDataSet = new QLabel(tr("DataSet") + ":", pWidget);
    m_pLineEditGooseTxDataSet = new QLineEdit(pWidget);
    pLabelGooseTxDataSet->setBuddy(m_pLineEditGooseTxDataSet);
    pGridLayoutGooseTx->addWidget(pLabelGooseTxDataSet, 3, 0);
    pGridLayoutGooseTx->addWidget(m_pLineEditGooseTxDataSet, 3, 1);

    QLabel *pLabelGooseTxFCDA = new QLabel(tr("FCDA") + ":", pWidget);
    m_pLineEditGooseTxFCDA = new QLineEdit(pWidget);
    pLabelGooseTxFCDA->setBuddy(m_pLineEditGooseTxFCDA);
    pGridLayoutGooseTx->addWidget(pLabelGooseTxFCDA, 4, 0);
    pGridLayoutGooseTx->addWidget(m_pLineEditGooseTxFCDA, 4, 1);

    // Goose Rx
    QLabel *pLabelGooseRxAccessPoint = new QLabel(tr("AccessPoint") + ":", pWidget);
    m_pLineEditGooseRxAccessPoint = new QLineEdit(pWidget);
    pLabelGooseRxAccessPoint->setBuddy(m_pLineEditGooseRxAccessPoint);
    pGridLayoutGooseRx->addWidget(pLabelGooseRxAccessPoint, 0, 0);
    pGridLayoutGooseRx->addWidget(m_pLineEditGooseRxAccessPoint, 0, 1);

    QLabel *pLabelGooseRxLDevice = new QLabel(tr("LDevice") + ":", pWidget);
    m_pLineEditGooseRxLDevice = new QLineEdit(pWidget);
    pLabelGooseRxLDevice->setBuddy(m_pLineEditGooseRxLDevice);
    pGridLayoutGooseRx->addWidget(pLabelGooseRxLDevice, 1, 0);
    pGridLayoutGooseRx->addWidget(m_pLineEditGooseRxLDevice, 1, 1);

    QLabel *pLabelGooseRxLNode = new QLabel(tr("LNode") + ":", pWidget);
    m_pLineEditGooseRxLNode = new QLineEdit(pWidget);
    pLabelGooseRxLNode->setBuddy(m_pLineEditGooseRxLNode);
    pGridLayoutGooseRx->addWidget(pLabelGooseRxLNode, 2, 0);
    pGridLayoutGooseRx->addWidget(m_pLineEditGooseRxLNode, 2, 1);

    QLabel *pLabelGooseRxFC = new QLabel(tr("FC") + ":", pWidget);
    m_pLineEditGooseRxFC = new QLineEdit(pWidget);
    pLabelGooseRxFC->setBuddy(m_pLineEditGooseRxFC);
    pGridLayoutGooseRx->addWidget(pLabelGooseRxFC, 3, 0);
    pGridLayoutGooseRx->addWidget(m_pLineEditGooseRxFC, 3, 1);

    QLabel *pLabelGooseRxDO = new QLabel(tr("DO") + ":", pWidget);
    m_pLineEditGooseRxDO = new QLineEdit(pWidget);
    pLabelGooseRxDO->setBuddy(m_pLineEditGooseRxDO);
    pGridLayoutGooseRx->addWidget(pLabelGooseRxDO, 4, 0);
    pGridLayoutGooseRx->addWidget(m_pLineEditGooseRxDO, 4, 1);

    QLabel *pLabelGooseRxDA = new QLabel(tr("DA") + ":", pWidget);
    m_pLineEditGooseRxDA = new QLineEdit(pWidget);
    pLabelGooseRxDA->setBuddy(m_pLineEditGooseRxDA);
    pGridLayoutGooseRx->addWidget(pLabelGooseRxDA, 5, 0);
    pGridLayoutGooseRx->addWidget(m_pLineEditGooseRxDA, 5, 1);

    // Sv Tx
    QLabel *pLabelSvTxAccessPoint = new QLabel(tr("AccessPoint") + ":", pWidget);
    m_pLineEditSvTxAccessPoint = new QLineEdit(pWidget);
    pLabelSvTxAccessPoint->setBuddy(m_pLineEditSvTxAccessPoint);
    pGridLayoutSvTx->addWidget(pLabelSvTxAccessPoint, 0, 0);
    pGridLayoutSvTx->addWidget(m_pLineEditSvTxAccessPoint, 0, 1);

    QLabel *pLabelSvTxLDevice = new QLabel(tr("LDevice") + ":", pWidget);
    m_pLineEditSvTxLDevice = new QLineEdit(pWidget);
    pLabelSvTxLDevice->setBuddy(m_pLineEditSvTxLDevice);
    pGridLayoutSvTx->addWidget(pLabelSvTxLDevice, 1, 0);
    pGridLayoutSvTx->addWidget(m_pLineEditSvTxLDevice, 1, 1);

    QLabel *pLabelSvTxLNode = new QLabel(tr("LNode") + ":", pWidget);
    m_pLineEditSvTxLNode = new QLineEdit(pWidget);
    pLabelSvTxLNode->setBuddy(m_pLineEditSvTxLNode);
    pGridLayoutSvTx->addWidget(pLabelSvTxLNode, 2, 0);
    pGridLayoutSvTx->addWidget(m_pLineEditSvTxLNode, 2, 1);

    QLabel *pLabelSvTxDataSet = new QLabel(tr("DataSet") + ":", pWidget);
    m_pLineEditSvTxDataSet = new QLineEdit(pWidget);
    pLabelSvTxDataSet->setBuddy(m_pLineEditSvTxDataSet);
    pGridLayoutSvTx->addWidget(pLabelSvTxDataSet, 3, 0);
    pGridLayoutSvTx->addWidget(m_pLineEditSvTxDataSet, 3, 1);

    QLabel *pLabelSvTxFCDA = new QLabel(tr("FCDA") + ":", pWidget);
    m_pLineEditSvTxFCDA = new QLineEdit(pWidget);
    pLabelSvTxFCDA->setBuddy(m_pLineEditSvTxFCDA);
    pGridLayoutSvTx->addWidget(pLabelSvTxFCDA, 4, 0);
    pGridLayoutSvTx->addWidget(m_pLineEditSvTxFCDA, 4, 1);

    // Sv Rx
    QLabel *pLabelSvRxAccessPoint = new QLabel(tr("AccessPoint") + ":", pWidget);
    m_pLineEditSvRxAccessPoint = new QLineEdit(pWidget);
    pLabelSvRxAccessPoint->setBuddy(m_pLineEditSvRxAccessPoint);
    pGridLayoutSvRx->addWidget(pLabelSvRxAccessPoint, 0, 0);
    pGridLayoutSvRx->addWidget(m_pLineEditSvRxAccessPoint, 0, 1);

    QLabel *pLabelSvRxLDevice = new QLabel(tr("LDevice") + ":", pWidget);
    m_pLineEditSvRxLDevice = new QLineEdit(pWidget);
    pLabelSvRxLDevice->setBuddy(m_pLineEditSvRxLDevice);
    pGridLayoutSvRx->addWidget(pLabelSvRxLDevice, 1, 0);
    pGridLayoutSvRx->addWidget(m_pLineEditSvRxLDevice, 1, 1);

    QLabel *pLabelSvRxLNode = new QLabel(tr("LNode") + ":", pWidget);
    m_pLineEditSvRxLNode = new QLineEdit(pWidget);
    pLabelSvRxLNode->setBuddy(m_pLineEditSvRxLNode);
    pGridLayoutSvRx->addWidget(pLabelSvRxLNode, 2, 0);
    pGridLayoutSvRx->addWidget(m_pLineEditSvRxLNode, 2, 1);

    QLabel *pLabelSvRxFC = new QLabel(tr("FC") + ":", pWidget);
    m_pLineEditSvRxFC = new QLineEdit(pWidget);
    pLabelSvRxFC->setBuddy(m_pLineEditSvRxFC);
    pGridLayoutSvRx->addWidget(pLabelSvRxFC, 3, 0);
    pGridLayoutSvRx->addWidget(m_pLineEditSvRxFC, 3, 1);

    QLabel *pLabelSvRxDO = new QLabel(tr("DO") + ":", pWidget);
    m_pLineEditSvRxDO = new QLineEdit(pWidget);
    pLabelSvRxDO->setBuddy(m_pLineEditSvRxDO);
    pGridLayoutSvRx->addWidget(pLabelSvRxDO, 4, 0);
    pGridLayoutSvRx->addWidget(m_pLineEditSvRxDO, 4, 1);

    QLabel *pLabelSvRxDA = new QLabel(tr("DA") + ":", pWidget);
    m_pLineEditSvRxDA = new QLineEdit(pWidget);
    pLabelSvRxDA->setBuddy(m_pLineEditSvRxDA);
    pGridLayoutSvRx->addWidget(pLabelSvRxDA, 5, 0);
    pGridLayoutSvRx->addWidget(m_pLineEditSvRxDA, 5, 1);

    const QMap<QString, QString> &mapGooseTx = VTerminalDefine::Instance()->GetGooseTx();
    m_pLineEditGooseTxAccessPoint->setText(mapGooseTx.value("AccessPoint"));
    m_pLineEditGooseTxLDevice->setText(mapGooseTx.value("LDevice"));
    m_pLineEditGooseTxLNode->setText(mapGooseTx.value("LNode"));
    m_pLineEditGooseTxDataSet->setText(mapGooseTx.value("DataSet"));
    m_pLineEditGooseTxFCDA->setText(mapGooseTx.value("FCDA"));

    const QMap<QString, QString> &mapGooseRx = VTerminalDefine::Instance()->GetGooseRx();
    m_pLineEditGooseRxAccessPoint->setText(mapGooseRx.value("AccessPoint"));
    m_pLineEditGooseRxLDevice->setText(mapGooseRx.value("LDevice"));
    m_pLineEditGooseRxLNode->setText(mapGooseRx.value("LNode"));
    m_pLineEditGooseRxFC->setText(mapGooseRx.value("FC"));
    m_pLineEditGooseRxDO->setText(mapGooseRx.value("DO"));
    m_pLineEditGooseRxDA->setText(mapGooseRx.value("DA"));

    const QMap<QString, QString> &mapSvTx = VTerminalDefine::Instance()->GetSvTx();
    m_pLineEditSvTxAccessPoint->setText(mapSvTx.value("AccessPoint"));
    m_pLineEditSvTxLDevice->setText(mapSvTx.value("LDevice"));
    m_pLineEditSvTxLNode->setText(mapSvTx.value("LNode"));
    m_pLineEditSvTxDataSet->setText(mapSvTx.value("DataSet"));
    m_pLineEditSvTxFCDA->setText(mapSvTx.value("FCDA"));

    const QMap<QString, QString> &mapSvRx = VTerminalDefine::Instance()->GetSvRx();
    m_pLineEditSvRxAccessPoint->setText(mapSvRx.value("AccessPoint"));
    m_pLineEditSvRxLDevice->setText(mapSvRx.value("LDevice"));
    m_pLineEditSvRxLNode->setText(mapSvRx.value("LNode"));
    m_pLineEditSvRxFC->setText(mapSvRx.value("FC"));
    m_pLineEditSvRxDO->setText(mapSvRx.value("DO"));
    m_pLineEditSvRxDA->setText(mapSvRx.value("DA"));

    return pWidget;
}

void VTerminalSettings::Apply()
{
    QMap<QString, QString> mapGooseTx;
    mapGooseTx.insert("AccessPoint", m_pLineEditGooseTxAccessPoint->text().trimmed());
    mapGooseTx.insert("LDevice", m_pLineEditGooseTxLDevice->text().trimmed());
    mapGooseTx.insert("LNode", m_pLineEditGooseTxLNode->text().trimmed());
    mapGooseTx.insert("DataSet", m_pLineEditGooseTxDataSet->text().trimmed());
    mapGooseTx.insert("FCDA", m_pLineEditGooseTxFCDA->text().trimmed());
    VTerminalDefine::Instance()->SetGooseTx(mapGooseTx);

    QMap<QString, QString> mapGooseRx;
    mapGooseRx.insert("AccessPoint", m_pLineEditGooseRxAccessPoint->text().trimmed());
    mapGooseRx.insert("LDevice", m_pLineEditGooseRxLDevice->text().trimmed());
    mapGooseRx.insert("LNode", m_pLineEditGooseRxLNode->text().trimmed());
    mapGooseRx.insert("FC", m_pLineEditGooseRxFC->text().trimmed());
    mapGooseRx.insert("DO", m_pLineEditGooseRxDO->text().trimmed());
    mapGooseRx.insert("DA", m_pLineEditGooseRxDA->text().trimmed());
    VTerminalDefine::Instance()->SetGooseRx(mapGooseRx);

    QMap<QString, QString> mapSvTx;
    mapSvTx.insert("AccessPoint", m_pLineEditSvTxAccessPoint->text().trimmed());
    mapSvTx.insert("LDevice", m_pLineEditSvTxLDevice->text().trimmed());
    mapSvTx.insert("LNode", m_pLineEditSvTxLNode->text().trimmed());
    mapSvTx.insert("DataSet", m_pLineEditSvTxDataSet->text().trimmed());
    mapSvTx.insert("FCDA", m_pLineEditSvTxFCDA->text().trimmed());
    VTerminalDefine::Instance()->SetSvTx(mapSvTx);

    QMap<QString, QString> mapSvRx;
    mapSvRx.insert("AccessPoint", m_pLineEditSvRxAccessPoint->text().trimmed());
    mapSvRx.insert("LDevice", m_pLineEditSvRxLDevice->text().trimmed());
    mapSvRx.insert("LNode", m_pLineEditSvRxLNode->text().trimmed());
    mapSvRx.insert("FC", m_pLineEditSvRxFC->text().trimmed());
    mapSvRx.insert("DO", m_pLineEditSvRxDO->text().trimmed());
    mapSvRx.insert("DA", m_pLineEditSvRxDA->text().trimmed());
    VTerminalDefine::Instance()->SetSvRx(mapSvRx);
}

void VTerminalSettings::Finish()
{
}

