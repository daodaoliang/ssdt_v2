#ifndef DIALOGBATCHEDIT_H
#define DIALOGBATCHEDIT_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLineEdit;
QT_END_NAMESPACE

namespace SclModel {
namespace Internal {

class DialogBatchEditAddress : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    DialogBatchEditAddress(QWidget *pParent = 0);

// Operations
public:
    QString GetIpAddress() const;
    void    SetIpAddress(const QString &strIpAddress);
    QString GetIpSubnet() const;
    void    SetIpSubnet(const QString &strIpSubnet);

// Properties
private:
    QCheckBox   *m_pCheckBoxIpAddress;
    QLineEdit   *m_pLineEditIpAddress;
    QCheckBox   *m_pCheckBoxIpSubnet;
    QLineEdit   *m_pLineEditIpSubnet;

private slots:
    void SlotCheckBoxClicked();

public slots:
    virtual void accept();
};

class DialogBatchEditGSE : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    DialogBatchEditGSE(QWidget *pParent = 0);

// Operations
public:
    QString GetMacAddress() const;
    void    SetMacAddress(const QString &strMacAddress);
    QString GetVLanId() const;
    void    SetVLanId(const QString &strVLanId);
    QString GetVLanPriority() const;
    void    SetVLanPriority(const QString &strVLanPriority);
    QString GetAppId() const;
    void    SetAppId(const QString &strAppId);
    QString GetMinTime() const;
    void    SetMinTime(const QString &strMinTime);
    QString GetMaxTime() const;
    void    SetMaxTime(const QString &strMaxTime);

// Properties
private:
    QCheckBox   *m_pCheckBoxMacAddress;
    QLineEdit   *m_pLineEditMacAddress;
    QCheckBox   *m_pCheckBoxVLanId;
    QLineEdit   *m_pLineEditVLanId;
    QCheckBox   *m_pCheckBoxVLanPriority;
    QLineEdit   *m_pLineEditVLanPriority;
    QCheckBox   *m_pCheckBoxAppId;
    QLineEdit   *m_pLineEditAppId;
    QCheckBox   *m_pCheckBoxMinTime;
    QLineEdit   *m_pLineEditMinTime;
    QCheckBox   *m_pCheckBoxMaxTime;
    QLineEdit   *m_pLineEditMaxTime;

private slots:
    void SlotCheckBoxClicked();

public slots:
    virtual void accept();
};

class DialogBatchEditSMV : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    DialogBatchEditSMV(QWidget *pParent = 0);

// Operations
public:
    QString GetMacAddress() const;
    void    SetMacAddress(const QString &strMacAddress);
    QString GetVLanId() const;
    void    SetVLanId(const QString &strVLanId);
    QString GetVLanPriority() const;
    void    SetVLanPriority(const QString &strVLanPriority);
    QString GetAppId() const;
    void    SetAppId(const QString &strAppId);

// Properties
private:
    QCheckBox   *m_pCheckBoxMacAddress;
    QLineEdit   *m_pLineEditMacAddress;
    QCheckBox   *m_pCheckBoxVLanId;
    QLineEdit   *m_pLineEditVLanId;
    QCheckBox   *m_pCheckBoxVLanPriority;
    QLineEdit   *m_pLineEditVLanPriority;
    QCheckBox   *m_pCheckBoxAppId;
    QLineEdit   *m_pLineEditAppId;

private slots:
    void SlotCheckBoxClicked();

public slots:
    virtual void accept();
};

} // namespace Internal
} // namespace SclModel

#endif // DIALOGBATCHEDIT_H
