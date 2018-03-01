#ifndef DIALOGAUTOCONNECT_H
#define DIALOGAUTOCONNECT_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
class QTreeWidget;
class QTreeWidgetItem;
class QTableWidget;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeDevice;

} // namespace ProjectExplorer

class VLink_Template;
class VLink_IED;
class VLink_TxIed;

namespace SclModel {
namespace Internal {

class DialogAutoConnect : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    DialogAutoConnect(ProjectExplorer::PeDevice *pDeviceTx, ProjectExplorer::PeDevice *pDeviceRx, QWidget *pParent = 0);
    ~DialogAutoConnect();

// Operations
public:
    virtual QSize   sizeHint() const { return QSize(900, 700); }

private:
    void                FillRxIed();
    void                FillTxIed();
    void                FillVLink();
    VLink_IED*          GetSelectedRxIed() const;
    VLink_TxIed*        GetSelectedGoTxIed() const;
    VLink_TxIed*        GetSelectedSvTxIed() const;

// Properties
private:
    VLink_Template              *m_pVTemplate;
    ProjectExplorer::PeDevice   *m_pDeviceTx;
    ProjectExplorer::PeDevice   *m_pDeviceRx;

    QListWidget                 *m_pListWidgetRxIed;
    QTreeWidget                 *m_pTreeWidgetTxIed;
    QTableWidget                *m_pTableWidgetVLink;

public slots:
    void SlotListWidgetItemChanged(QListWidgetItem *pItem);
    void SlotTreeWidgetItemChanged(QTreeWidgetItem *pItem, int iColumn);

    virtual void accept();
};

} // namespace Internal
} // namespace SclModel

#endif // DIALOGAUTOCONNECT_H
