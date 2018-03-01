#ifndef MANAGEMODELDLG_H
#define MANAGEMODELDLG_H

#include <QMap>
#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QStandardItem;
class QItemSelection;
class QComboBox;
class QPushButton;
class QAbstractButton;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTableView;

} // namespace Utils

namespace Core {
namespace Internal {

class ManageModelDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    ManageModelDlg(QWidget *pParent = 0);
    ~ManageModelDlg();

// Operations
public:
    virtual QSize       sizeHint() const { return QSize(900, 600); }

private:
    void                ReadSettings();
    void                SaveSettings();

// Properties
private:
    QComboBox                   *m_pComboBoxProjectVersion;
    QStandardItemModel          *m_pModel;
    Utils::ReadOnlyTableView    *m_pView;
    QPushButton                 *m_pPushButtonImportScd;
    QPushButton                 *m_pPushButtonExportScd;
    QPushButton                 *m_pPushButtonExportSpcd;
    QPushButton                 *m_pPushButtonExportIpcd;
    QPushButton                 *m_pPushButtonImportIpcd;
    QPushButton                 *m_pPushButtonAssociate;
    QPushButton                 *m_pPushButtonClear;

    QString                     m_strScdFileDir;
    QString                     m_strIcdFileDir;

private slots:
    void    SlotDialogButtonBoxClicked(QAbstractButton* pButton);
    void    SlotCurrentProjectVersionChanged(int iCurrentIndex);
    void    SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
};

} // namespace Internal
} // namespace Core

#endif // MANAGEMODELDLG_H
