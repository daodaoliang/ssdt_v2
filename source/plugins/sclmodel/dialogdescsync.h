#ifndef DIALOGDESCSYNC_H
#define DIALOGDESCSYNC_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QStandardItemModel;
class QTableView;
class QListView;
class QStandardItem;
class QPushButton;
class QButtonGroup;
class QComboBox;
class QItemSelection;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeDevice;
class VTerminalConn;

} // namespace ProjectExplorer

namespace SclParser {

class SCLElement;

} // namespace SclParser

namespace SclModel {
namespace Internal {

class DialogDescSync : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    DialogDescSync(QWidget *pParent = 0);
    ~DialogDescSync();

// Operations
public:
    virtual QSize sizeHint () const { return QSize(1050, 600); }

private:
    void                    SetupModelViewDevice();
    void                    SetupModelViewDesc();
    SclParser::SCLElement*  GetDOIFromRef(SclParser::SCLElement *pSCLElementIED, const QString &strRef);
    bool                    SyncDeviceDesc(ProjectExplorer::PeDevice *pDevice, bool bProToSta);

// Properties
private:
    QListView           *m_pViewDevice;
    QStandardItemModel  *m_pModelDevice;

    QTableView          *m_pViewDesc;
    QStandardItemModel  *m_pModelDesc;

    QComboBox           *m_pComboBoxProject;
    QButtonGroup        *m_pButtonGroup;
    QPushButton         *m_pButtonSyncCurrent;
    QPushButton         *m_pButtonSyncAll;
    QPushButton         *m_pButtonExit;

// Slots
private slots:
    void    SlotCurrentProjectChanged(int iCurrentIndex);
    void    SlotDeviceViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void    SlotButtonSyncCurrentReleased();
    void    SlotButtonSyncAllReleased();
};

} // namespace Internal
} // namespace SclModel

#endif // DIALOGDESCSYNC_H
