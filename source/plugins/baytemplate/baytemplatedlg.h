#ifndef BAYTEMPLATEDLG_H
#define BAYTEMPLATEDLG_H

#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QStandardItemModel;
class QItemSelection;
class QModelIndex;
QT_END_NAMESPACE

namespace Utils {

class ReadOnlyTreeView;
class ReadOnlyTableView;

} // namespace Utils

namespace ProjectExplorer {

class PeProjectObject;
class PeInfoSet;

} // namespace ProjectExplorer

namespace BayTemplate {
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

class BayTemplateDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    BayTemplateDlg(QWidget *pParent = 0);
    ~BayTemplateDlg();

// Operations
public:
    virtual QSize               sizeHint() const;

private:
    QWidget*                    SetupExplorerModelView();
    QWidget*                    SetupTpInfoSetModelView();
    QWidget*                    SetupReplaceTpInfoSetModelView();
    QString                     ValidInfoset(ProjectExplorer::PeInfoSet *pInfoSet, const QList<ProjectExplorer::PeInfoSet*> &lstAllInfoSets);

// Properties
private:
    QStandardItemModel          *m_pModelExplorer;
    Utils::ReadOnlyTreeView     *m_pViewExplorer;

    QStandardItemModel          *m_pModelTpInfoSet;
    Utils::ReadOnlyTableView    *m_pViewTpInfoSet;

    QStandardItemModel          *m_pModelReplaceTpInfoSet;
    Utils::ReadOnlyTreeView     *m_pViewReplaceTpInfoSet;

    QComboBox                   *m_pComboBoxProjectVersion;

public slots:
    virtual void accept();

private slots:
    void SlotExplorerViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void SlotCurrentProjectVersionChanged(int iCurrentIndex);
    void SlotDoubleClicked(const QModelIndex &index);
};

} // namespace Internal
} // namespace BayTemplate

#endif // BAYTEMPLATEDLG_H
