#ifndef COPYVCONNDLG_H
#define COPYVCONNDLG_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QStandardItemModel;
class QModelIndex;
class QStandardItem;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTreeView;

} // namespace Utils

namespace ProjectExplorer {

class PeProjectObject;
class PeBay;
class PeCubicle;
class PeDevice;

} // namespace ProjectExplorer

namespace Core {
namespace Internal {

class SelectProjectObjectDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    SelectProjectObjectDlg(const QString &strProjectObjectTypeName,
                           const QList<ProjectExplorer::PeProjectObject*> lstProjectObjects,
                           ProjectExplorer::PeProjectObject *pSelectedProjectObject,
                           QWidget *pParent = 0);

// Operations
public:
    ProjectExplorer::PeProjectObject* GetSelectedProjectObject() const;

// Properties
private:
    QComboBox   *m_pComboBox;
};

class CopyVConnDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    CopyVConnDlg(QWidget *pParent = 0);
    ~CopyVConnDlg();

// Operations
public:
    virtual QSize           sizeHint() const;

// Properties
private:
    QComboBox               *m_pComboBoxSrcProject;
    QComboBox               *m_pComboBoxDstProject;

    QComboBox               *m_pComboBoxSrcIed;
    QComboBox               *m_pComboBoxDstIed;

    QStandardItemModel      *m_pModel;
    Utils::ReadOnlyTreeView *m_pView;

public slots:
    virtual void accept();

private slots:
    void SlotSrcProjectChanged(int iCurrentIndex);
    void SlotDstProjectChanged(int iCurrentIndex);
    void SlotSrcIedChanged(int iCurrentIndex);
    void SlotDstIedChanged(int iCurrentIndex);
    void SlotDoubleClicked(const QModelIndex &index);
    void SlotItemChanged(QStandardItem *pItem);
};

} // namespace Internal
} // namespace Core

#endif // COPYVCONNDLG_H
