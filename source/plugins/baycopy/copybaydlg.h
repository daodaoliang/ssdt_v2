#ifndef COPYBAYDLG_H
#define COPYBAYDLG_H

#include <QMap>
#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QModelIndex;
class QComboBox;
class QLineEdit;
class QCheckBox;
class QStandardItemModel;
class QStandardItem;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTableView;
class ReadOnlyListView;

} // namespace Utils

namespace ProjectExplorer {

class PeBay;
class PeCubicle;
class PeDevice;
class PeVTerminal;
class PeStrap;
class PeInfoSet;
class PeVTerminalConn;

} // namespace ProjectExplorer

namespace BayCopy {
namespace Internal {

class CopyBayModel;
class CopyBayView;

class ReplaceDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    ReplaceDlg(QWidget *pParent = 0);

// Operations
public:
    bool        GetReplaceBayObjectName() const;
    QString     GetBayObjectNameSrc() const;
    QString     GetBayObjectNameDst() const;
    bool        GetReplaceBayObjectDesc() const;
    QString     GetBayObjectDescSrc() const;
    QString     GetBayObjectDescDst() const;
    bool        GetReplaceCubicleObjectName() const;
    QString     GetCubicleObjectNameSrc() const;
    QString     GetCubicleObjectNameDst() const;
    bool        GetReplaceCubicleObjectDesc() const;
    QString     GetCubicleObjectDescSrc() const;
    QString     GetCubicleObjectDescDst() const;

    bool        GetReplaceVTerminalDesc() const;
    QString     GetVTerminalDescSrc() const;
    QString     GetVTerminalDescDst() const;
    bool        GetReplaceInfoSetDesc() const;
    QString     GetInfoSetDescSrc() const;
    QString     GetInfoSetDescDst() const;

// Properties
private:
    QCheckBox   *m_pCheckBoxBayObjectName;
    QLineEdit   *m_pLineEditBayObjectNameSrc;
    QLineEdit   *m_pLineEditBayObjectNameDst;
    QCheckBox   *m_pCheckBoxBayObjectDesc;
    QLineEdit   *m_pLineEditBayObjectDescSrc;
    QLineEdit   *m_pLineEditBayObjectDescDst;
    QCheckBox   *m_pCheckBoxCubicleObjectName;
    QLineEdit   *m_pLineEditCubicleObjectNameSrc;
    QLineEdit   *m_pLineEditCubicleObjectNameDst;
    QCheckBox   *m_pCheckBoxCubicleObjectDesc;
    QLineEdit   *m_pLineEditCubicleObjectDescSrc;
    QLineEdit   *m_pLineEditCubicleObjectDescDst;

    QCheckBox   *m_pCheckBoxVTerminalDesc;
    QLineEdit   *m_pLineEditVTerminalDescSrc;
    QLineEdit   *m_pLineEditVTerminalDescDst;
    QCheckBox   *m_pCheckBoxInfoSetDesc;
    QLineEdit   *m_pLineEditInfoSetDescSrc;
    QLineEdit   *m_pLineEditInfoSetDescDst;

private slots:
    void SlotCheckBoxToggled(bool bToggled);
    void SlotPushButtonReplaceClicked();

signals:
    void sigReplace();
};

class CopyBayDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    CopyBayDlg(QWidget *pParent = 0);
    ~CopyBayDlg();

// Operations
public:
    virtual QSize       sizeHint() const;

private:
    void                SetupIntInfoSet();
    void                SetupIntVTerminalConn();
    void                SetupExtInfoSet();
    void                SetupExtVTerminalConn();

    void                ClearModel();
    void                AddIntInfoSetItem(ProjectExplorer::PeInfoSet *pInfoSet);
    void                AddIntVTerminalConnItem(ProjectExplorer::PeVTerminalConn *pVTerminalConn);
    void                AddExtInfoSetItem(ProjectExplorer::PeInfoSet *pInfoSet);
    void                AddExtVTerminalConnItem(ProjectExplorer::PeVTerminalConn *pVTerminalConn);
    void                UpdateInternalVTerminalConn(ProjectExplorer::PeVTerminalConn *pVTerminalConn);
    void                UpdateInternalInfoset(ProjectExplorer::PeInfoSet *pInfoSet);
    void                UpdateExternalVTerminalConn(ProjectExplorer::PeVTerminalConn *pVTerminalConn);
    void                UpdateExternalInfoset(ProjectExplorer::PeInfoSet *pInfoSet);
    QString             ValidInfoset(ProjectExplorer::PeInfoSet *pInfoSet, const QList<ProjectExplorer::PeInfoSet*> &lstAllInfoSets);
    QString             ValidVTerminalConn(ProjectExplorer::PeVTerminalConn *pVTerminalConn, const QList<ProjectExplorer::PeVTerminalConn*> &lstAllVTerminalConns);
    void                UpdateInfosetItemStatus();
    void                UpdateVTerminalConnItemStatus();

    QList<ProjectExplorer::PeInfoSet*>          GetNewInfoSets() const;
    QList<ProjectExplorer::PeVTerminalConn*>    GetNewVTerminalConns() const;

// Properties
private:
    QComboBox                   *m_pComboBoxSrcProject;
    QComboBox                   *m_pComboBoxDstProject;

    CopyBayModel                *m_pModel;
    CopyBayView                 *m_pView;

    QStandardItemModel          *m_pModelIntInfoSet;
    Utils::ReadOnlyTableView    *m_pViewIntInfoSet;

    QStandardItemModel          *m_pModelIntVTerminalConn;
    Utils::ReadOnlyTableView    *m_pViewIntVTerminalConn;

    QStandardItemModel          *m_pModelExtInfoSet;
    Utils::ReadOnlyTableView    *m_pViewExtInfoSet;

    QStandardItemModel          *m_pModelExtVTerminalConn;
    Utils::ReadOnlyTableView    *m_pViewExtVTerminalConn;

    ReplaceDlg                  *m_pReplaceDlg;

    QMap<ProjectExplorer::PeInfoSet*, QStandardItem*>       m_mapIntInfoSetToItem;
    QMap<ProjectExplorer::PeInfoSet*, QStandardItem*>       m_mapExtInfoSetToItem;
    QMap<ProjectExplorer::PeVTerminalConn*, QStandardItem*> m_mapIntVTerminalConnToItem;
    QMap<ProjectExplorer::PeVTerminalConn*, QStandardItem*> m_mapExtVTerminalConnToItem;

	QList<ProjectExplorer::PeInfoSet*>          m_lstNewInfoSets;
    QList<ProjectExplorer::PeVTerminalConn*>    m_lstNewVTerminalConns;

public slots:
    virtual void accept();

private slots:
    void SlotSrcProjectChanged(int iCurrentIndex);
    void SlotDstProjectChanged(int iCurrentIndex);
    void SlotBayObjectDoubleClicked(const QModelIndex &index);
    void SlotReplace();
    void SlotUpdateNewData();
    void SlotExgInfoSetDoubleClicked(const QModelIndex &index);
    void SlotExtVTerminalConnDoubleClicked(const QModelIndex &index);
    void SlotInfoSetChanged(QStandardItem *pItem);
    void SlotVTerminalConnChanged(QStandardItem *pItem);
    void SlotActionSelectAll();
    void SlotActionDeselectAll();
};

} // namespace Internal
} // namespace BayCopy

#endif // COPYBAYDLG_H
