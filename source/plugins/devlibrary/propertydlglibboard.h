#ifndef PROPERTYMDLGLIBBOARD_H
#define PROPERTYMDLGLIBBOARD_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PbLibBoard;

} // namespace ProjectExplorer

namespace DevLibrary {
namespace Internal {

class PropertyDlgLibBoard : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    PropertyDlgLibBoard(ProjectExplorer::PbLibBoard *pLibBoard, bool bNew, QWidget *pParent = 0);

// Operations
private:
    bool UpdateData(bool bSaveAndValidate);

// Properties
private:
    ProjectExplorer::PbLibBoard *m_pLibBoard;

    QLineEdit   *m_pLineEditPosition;
    QLineEdit   *m_pLineEditType;
    QLineEdit   *m_pLineEditDescription;

public slots:
    virtual void accept();
};

} // namespace Internal
} // namespace DevLibrary

#endif // PROPERTYMDLGLIBBOARD_H
