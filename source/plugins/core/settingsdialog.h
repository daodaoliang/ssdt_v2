#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QList>
#include <QSet>
#include "styledui/styleddialog.h"
#include "ioptionspage.h"

QT_BEGIN_NAMESPACE
class QModelIndex;
class QStackedLayout;
class QLabel;
class QListView;
QT_END_NAMESPACE

namespace Utils {

class FilterLineEdit;

}

namespace Core {
namespace Internal {

class CPageModel;

class CSettingsDialog : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    CSettingsDialog(QWidget *pParent, const QString &strInitialCategory = QString());
    ~CSettingsDialog();

// Operations
public:
    bool ExecDialog();

protected:
    virtual QSize sizeHint() const { return minimumSize(); }

private:
    void CreateGui();
    void ShowPage(int iIndex);

// Properties
private:
    const QList<Core::IOptionsPage*>    m_lstPages;
    QSet<Core::IOptionsPage*>           m_setVisitedPages;
    CPageModel                          *m_pPageModel;
    QListView                           *m_pListView;
    bool                                m_bApplied;
    QString                             m_strCurrentPage;
    QStackedLayout                      *m_pStackedLayout;

public slots:
    virtual void done(int iResult);
    virtual void accept();
    virtual void reject();

private slots:
    void SlotApply();
    void SlotCurrentChanged(const QModelIndex &current);    
};

} // namespace Internal
} // namespace Core

#endif // SETTINGSDIALOG_H
