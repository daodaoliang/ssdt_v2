#ifndef PROPERTYMDLGROOM_H
#define PROPERTYMDLGROOM_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QCheckBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeRoom;

} // namespace ProjectExplorer

namespace DevExplorer {
namespace Internal {

class PropertyDlgRoom : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgRoom(ProjectExplorer::PeRoom *pRoom, QWidget *pParent = 0);

// Operations;
private:
    bool UpdateData(bool bSaveAndValidate);

// Properties
private:
    ProjectExplorer::PeRoom *m_pRoom;

    QLineEdit   *m_pLineEditName;
    QLineEdit   *m_pLineEditNumber;
    QCheckBox   *m_pCheckBoxYard;

public slots:
    virtual void accept();
};

} // namespace Internal
} // namespace DevExplorer

#endif // PROPERTYMDLGROOM_H
