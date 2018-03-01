#ifndef PROPERTYMDLGBOARD_H
#define PROPERTYMDLGBOARD_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeBoard;

} // namespace ProjectExplorer

namespace DevExplorer {
namespace Internal {

class PropertyDlgBoard : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgBoard(ProjectExplorer::PeBoard *pBoard, QWidget *pParent = 0);

// Operations
private:
    bool UpdateData(bool bSaveAndValidate);

// Properties
private:
    ProjectExplorer::PeBoard    *m_pBoard;

    QLineEdit   *m_pLineEditPosition;
    QLineEdit   *m_pLineEditType;
    QLineEdit   *m_pLineEditDescription;

public slots:
    virtual void accept();
};

} // namespace Internal
} // namespace DevExplorer

#endif // PROPERTYMDLGBOARD_H
