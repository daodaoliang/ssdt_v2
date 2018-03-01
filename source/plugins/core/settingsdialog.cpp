#include <QSettings>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QSpacerItem>
#include <QStyle>
#include <QStackedLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QFrame>
#include <QDialogButtonBox>
#include <QListView>
#include <QApplication>
#include <QGroupBox>
#include <QStyledItemDelegate>

#include "extensionsystem/pluginmanager.h"
#include "utils/fancylineedit.h"
#include "mainwindow.h"

#include "settingsdialog.h"

static const char * const g_szSettings_GroupOptions         = "Options";
static const char * const g_szSettings_KeyLastPage          = "LastPage";

namespace Core {
namespace Internal {

///////////////////////////////////////////////////////////////////////
// CPageModel member functions
///////////////////////////////////////////////////////////////////////
class CPageModel : public QAbstractListModel
{
// Construction and Destruction
public:
    CPageModel(QObject *pParent = 0) : QAbstractListModel(pParent)
    {
        QPixmap pxmEmpty(24, 24);
        pxmEmpty.fill(Qt::transparent);
        m_iconEmpty = QIcon(pxmEmpty);
    }

// Operations
public:
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        return parent.isValid() ? 0 : m_lstPages.size();
    }

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
    {
        switch(role)
        {
            case Qt::DisplayRole:
            {
                return m_lstPages.at(index.row())->GetDisplayName();
            }
            case Qt::DecorationRole:
            {
                QIcon icon = m_lstPages.at(index.row())->GetDisplayIcon();
                if(icon.isNull())
                    icon = m_iconEmpty;

                return icon;
            }
        }

        return QVariant();
    }

    void setPages(const QList<IOptionsPage*> &lstPages)
    {
#if QT_VERSION >= 0x050000
        beginResetModel();
#endif

        m_lstPages = lstPages;

#if QT_VERSION >= 0x050000
        endResetModel();
#else
        reset();
#endif
    }

    const QList<IOptionsPage*>& GetPages() const { return m_lstPages; }

// Properties
private:
    QList<IOptionsPage*>    m_lstPages;
    QIcon                   m_iconEmpty;
};

///////////////////////////////////////////////////////////////////////
// CPageListViewDelegate member functions
///////////////////////////////////////////////////////////////////////
class CPageListViewDelegate : public QStyledItemDelegate
{
public:
    CPageListViewDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

protected:
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(qMax(size.height(), 32));
        return size;
    }
};

///////////////////////////////////////////////////////////////////////
// CPageListView member functions
///////////////////////////////////////////////////////////////////////
class CPageListView : public QListView
{
public:
    CPageListView(QWidget *pParent = 0) : QListView(pParent)
    {
        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        setItemDelegate(new CPageListViewDelegate(this));
    }

protected:
    virtual QSize sizeHint() const
    {
        int iWidth = sizeHintForColumn(0) + frameWidth() * 2 + 5;
        if(verticalScrollBar()->isVisible())
            iWidth += verticalScrollBar()->width();

        return QSize(iWidth, 100);
    }
};

///////////////////////////////////////////////////////////////////////
// CSettingsDialog member functions
///////////////////////////////////////////////////////////////////////
bool optionsPageLessThan(const IOptionsPage *p1, const IOptionsPage *p2)
{
    return p1->GetID().compare(p2->GetID()) < 0;
}

static inline QList<Core::IOptionsPage*> sortedOptionsPages()
{
    QList<Core::IOptionsPage*> rc = ExtensionSystem::PluginManager::instance()->getObjects<IOptionsPage>();
    qStableSort(rc.begin(), rc.end(), optionsPageLessThan);
    return rc;
}

CSettingsDialog::CSettingsDialog(QWidget *pParent, const QString &strInitialPage) :
        StyledUi::StyledDialog(pParent),
        m_lstPages(sortedOptionsPages()),
        m_pPageModel(new CPageModel(this)),
        m_pListView(new CPageListView),
        m_bApplied(false),
        m_pStackedLayout(new QStackedLayout)
{
    CreateGui();
    SetWindowTitle(tr("Options"));

    m_pPageModel->setPages(m_lstPages);

    QString strTempInitialPage = strInitialPage;
    if(strTempInitialPage.isEmpty())
    {
        QSettings *pSettings = MainWindow::Instance()->GetSettings();
        pSettings->beginGroup(QLatin1String(g_szSettings_GroupOptions));
        strTempInitialPage = pSettings->value(QLatin1String(g_szSettings_KeyLastPage), QVariant(QString())).toString();
        pSettings->endGroup();
    }

    int iInitialPageIndex = -1;
    const QList<IOptionsPage*> &lstPages = m_pPageModel->GetPages();
    for(int i = 0; i < lstPages.size(); ++i)
    {
        IOptionsPage *pOptionsPage = lstPages.at(i);
        if(pOptionsPage->GetID() == strTempInitialPage)
            iInitialPageIndex = i;

        m_pStackedLayout->addWidget(pOptionsPage->CreatePage(0));
    }

    m_pListView->setIconSize(QSize(24, 24));
    m_pListView->setModel(m_pPageModel);
    m_pListView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(m_pListView->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(SlotCurrentChanged(QModelIndex)));

    if(iInitialPageIndex != -1)
    {
        const QModelIndex index = m_pPageModel->index(iInitialPageIndex);
        m_pListView->setCurrentIndex(index);
    }
}

CSettingsDialog::~CSettingsDialog()
{
}

bool CSettingsDialog::ExecDialog()
{
    m_pListView->setFocus();
    m_bApplied = false;
    exec();
    return m_bApplied;
}

void CSettingsDialog::CreateGui()
{
    m_pStackedLayout->setMargin(0);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
    pDialogButtonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(pDialogButtonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(SlotApply()));
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGridLayout *pGridLayout = new QGridLayout;
    pGridLayout->setSpacing(5);
    pGridLayout->addWidget(m_pListView, 0, 0, 1, 1);
    pGridLayout->addLayout(m_pStackedLayout, 0, 1, 1, 1);
    pGridLayout->addWidget(pDialogButtonBox, 1, 0, 1, 2);
    pGridLayout->setColumnStretch(1, 4);

    QVBoxLayout *pVBoxLayout = GetClientLayout();
    pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    pVBoxLayout->addLayout(pGridLayout);
}

void CSettingsDialog::ShowPage(int iIndex)
{
    IOptionsPage *pPage = m_pPageModel->GetPages().at(iIndex);

    m_strCurrentPage = pPage->GetID();
    m_setVisitedPages.insert(pPage);
    m_pStackedLayout->setCurrentIndex(iIndex);
}

void CSettingsDialog::done(int iResult)
{
    QSettings *pSettings = MainWindow::Instance()->GetSettings();
    pSettings->beginGroup(QLatin1String(g_szSettings_GroupOptions));
    pSettings->setValue(QLatin1String(g_szSettings_KeyLastPage), m_strCurrentPage);
    pSettings->endGroup();

    QDialog::done(iResult);
}

void CSettingsDialog::accept()
{
    m_bApplied = true;
    foreach(IOptionsPage *pOptionsPage, m_setVisitedPages)
        pOptionsPage->Apply();
    foreach (IOptionsPage *pOptionsPage, m_lstPages)
        pOptionsPage->Finish();

    done(QDialog::Accepted);
}

void CSettingsDialog::reject()
{
    foreach(IOptionsPage *pOptionsPage, m_lstPages)
        pOptionsPage->Finish();

    done(QDialog::Rejected);
}

void CSettingsDialog::SlotApply()
{
    foreach(IOptionsPage *pOptionsPage, m_setVisitedPages)
        pOptionsPage->Apply();

    m_bApplied = true;
}

void CSettingsDialog::SlotCurrentChanged(const QModelIndex &current)
{
    if(current.isValid())
        ShowPage(current.row());
}


} // namespace Internal
} // namespace Core
