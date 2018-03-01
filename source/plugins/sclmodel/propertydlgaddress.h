#ifndef PROPERTYMDLGADDRESS_H
#define PROPERTYMDLGADDRESS_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QCheckBox;
QT_END_NAMESPACE

namespace SclModel {
namespace Internal {

class PropertyDlgAddress : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgAddress(QWidget *pParent = 0);

// Operations
public:
    void        SetDevice(const QString &strDevice);
    QString     GetDevice() const;

    void        SetAccessPoint(const QString &strAccessPoint);
    QString     GetAccessPoint() const;

    void        SetIp(const QString &strIp);
    QString     GetIp() const;

    void        SetIpSubnet(const QString &strIpSubnet);
    QString     GetIpSubnet() const;

// Properties
private:
    QLineEdit   *m_pLineEditDevice;
    QLineEdit   *m_pLineEditAccessPoint;
    QLineEdit   *m_pLineEditIp;
    QLineEdit   *m_pLineEditIpSubnet;
};

} // namespace Internal
} // namespace DevExplorer

#endif // PROPERTYMDLGADDRESS_H
