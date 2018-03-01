#ifndef SELECTBAYDLG_H
#define SELECTBAYDLG_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeBay;

} // namespace ProjectExplorer

namespace BayCopy {
namespace Internal {

class SelectBayDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    SelectBayDlg(ProjectExplorer::PeBay *pBay, QWidget *pParent = 0);

// Properties
private:
    ProjectExplorer::PeBay  *m_pBay;

    QComboBox   *m_pComboBoxBay;
    QLineEdit   *m_pLineEditName;
    QLineEdit   *m_pLineEditNumber;
    QComboBox   *m_pComboBoxVLevel;

public slots:
    virtual void accept();

private slots:
    void SlotCurrentBayChanged(int iCurrentIndex);
};

} // namespace Internal
} // namespace BayCopy

#endif // SELECTBAYDLG_H
