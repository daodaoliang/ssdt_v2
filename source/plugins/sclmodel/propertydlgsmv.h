#ifndef PROPERTYMDLGSMV_H
#define PROPERTYMDLGSMV_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QCheckBox;
QT_END_NAMESPACE

namespace SclModel {
namespace Internal {

class PropertyDlgSmv : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgSmv(QWidget *pParent = 0);

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
};

} // namespace Internal
} // namespace DevExplorer

#endif // PROPERTYMDLGSMV_H
