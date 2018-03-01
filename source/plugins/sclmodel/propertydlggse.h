#ifndef PROPERTYMDLGGSE_H
#define PROPERTYMDLGGSE_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QCheckBox;
QT_END_NAMESPACE

namespace SclModel {
namespace Internal {

class PropertyDlgGse : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgGse(QWidget *pParent = 0);

// Operations
public:
    void        SetDevice(const QString &strDevice);
    QString     GetDevice() const;

    void        SetAccessPoint(const QString &strAccessPoint);
    QString     GetAccessPoint() const;

    void        SetLDevice(const QString &strLDevice);
    QString     GetLDevice() const;

    void        SetControlBlock(const QString &strControlBlock);
    QString     GetControlBlock() const;

    void        SetMacAddress(const QString &strMacAddress);
    QString     GetMacAddress() const;

    void        SetVLanId(const QString &strVLanId);
    QString     GetVLanId() const;

    void        SetVLanPriority(const QString &strVLanPriority);
    QString     GetVLanPriority() const;

    void        SetAppId(const QString &strAppId);
    QString     GetAppId() const;

    void        SetMinTime(const QString &strMinTime);
    QString     GetMinTime() const;

    void        SetMaxTime(const QString &strMaxTime);
    QString     GetMaxTime() const;

// Properties
private:
    QLineEdit   *m_pLineEditDevice;
    QLineEdit   *m_pLineEditAccessPoint;
    QLineEdit   *m_pLineEditLDevice;
    QLineEdit   *m_pLineEditControlBlock;
    QLineEdit   *m_pLineEditMacAddress;
    QLineEdit   *m_pLineEditVLanId;
    QLineEdit   *m_pLineEditVLanPriority;
    QLineEdit   *m_pLineEditAppId;
    QLineEdit   *m_pLineEditMinTime;
    QLineEdit   *m_pLineEditMaxTime;
};

} // namespace Internal
} // namespace DevExplorer

#endif // PROPERTYMDLGADDRESS_H
