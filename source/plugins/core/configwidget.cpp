#include <QApplication>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QAction>
#include <QSettings>

#include "projectexplorer/peprojectversion.h"
#include "core/mainwindow.h"

#include "configwidget.h"
#include "configmodelview.h"

using namespace Core;

ConfigWidget::ConfigWidget(QWidget *pParent) :
    QWidget(pParent), m_pProjectObject(0), m_pModel(0), m_pView(0)
{
}

ConfigWidget::~ConfigWidget()
{
    CleanModel();
}

void ConfigWidget::SetupConfigWidget()
{
    // Create View
    m_pView = CreateView();

    // Create View Context Menu
    QList<QAction*> lstContextMenuActions = GetContextMenuActions();
    m_pView->addActions(lstContextMenuActions);

    // Create View Header Context Menu
    for(int i = 0; i < m_lstColumnInfo.size(); i++)
    {
        QAction *pAction = new QAction(m_lstColumnInfo.at(i).strCaption, this);
        pAction->setData(i);
        pAction->setCheckable(true);
        pAction->setChecked(m_lstColumnInfo.at(i).bShow);
        connect(pAction, SIGNAL(triggered(bool)), this, SLOT(SlotActionShowColumn(bool)));

        m_pView->horizontalHeader()->addAction(pAction);
        m_pView->verticalHeader()->addAction(pAction);
    }

    QVBoxLayout *pVBoxLayout = new QVBoxLayout(this);
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->setSpacing(0);
    pVBoxLayout->addWidget(m_pView);

    connect(m_pView->horizontalHeader(),
            SIGNAL(sectionResized(int, int, int)),
            this,
            SLOT(SlotViewColumnResized(int, int, int)));

    UpdateActions(QModelIndexList());
}

bool ConfigWidget::BuildModel(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(m_pProjectObject == pProjectObject || pProjectObject == 0)
        return false;

    CleanModel();

    m_pProjectObject = pProjectObject;
    m_pModel = CreateModel();

    if(!BuildModelData())
    {
        CleanModel();
        return false;
    }

    m_pView->setModel(m_pModel);

    for(int i = 0; i < m_lstColumnInfo.size(); i++)
    {
        m_pView->horizontalHeader()->resizeSection(i, m_lstColumnInfo.at(i).iWidth);
        m_pView->horizontalHeader()->setSectionHidden(i, !m_lstColumnInfo.at(i).bShow);
    }

    connect(m_pProjectObject->GetProjectVersion(), SIGNAL(sigObjectCreated(ProjectExplorer::PeProjectObject*)),
            this, SLOT(SlotProjectObjectCreated(ProjectExplorer::PeProjectObject*)));
    connect(m_pProjectObject->GetProjectVersion(), SIGNAL(sigObjectPropertyChanged(ProjectExplorer::PeProjectObject*)),
            this, SLOT(SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject*)));
    connect(m_pProjectObject->GetProjectVersion(), SIGNAL(sigObjectParentChanged(ProjectExplorer::PeProjectObject*,ProjectExplorer::PeProjectObject*)),
            this, SLOT(SlotProjectObjectParentChanged(ProjectExplorer::PeProjectObject*,ProjectExplorer::PeProjectObject*)));
    connect(m_pProjectObject->GetProjectVersion(), SIGNAL(sigDeviceBayChanged(ProjectExplorer::PeDevice*,ProjectExplorer::PeBay*)),
            this, SLOT(SlotProjectDeviceBayChanged(ProjectExplorer::PeDevice*,ProjectExplorer::PeBay*)));
    connect(m_pProjectObject->GetProjectVersion(), SIGNAL(sigObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject*)),
            this, SLOT(SlotProjectObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject*)));
    connect(m_pProjectObject->GetProjectVersion(), SIGNAL(sigObjectDeleted()),
            this, SLOT(SlotProjectObjectDeleted()));

    connect(m_pView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(SlotViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
    connect(m_pView, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotViewDoubleClicked(const QModelIndex&)));

    return true;
}

void ConfigWidget::UpdateModel()
{
    ProjectExplorer::PeProjectObject *pProjectObject = m_pProjectObject;

    CleanModel();
    BuildModel(pProjectObject);
}

void ConfigWidget::CleanModel()
{
    if(QItemSelectionModel *pItemSelectionModel = m_pView->selectionModel())
        delete pItemSelectionModel;

    m_pView->setModel(0);
    if(m_pModel)
    {
        delete m_pModel;
        m_pModel = 0;
    }

    if(m_pProjectObject)
    {
        disconnect(m_pProjectObject->GetProjectVersion(), 0, this, 0);
        m_pProjectObject = 0;
    }
}

ProjectExplorer::PeProjectObject* ConfigWidget::GetProjectObject() const
{
    return m_pProjectObject;
}

ConfigModel* ConfigWidget::CreateModel()
{
    ConfigModel *pConfigModel = new ConfigModel(this, m_lstColumnInfo.size());
    for(int i = 0; i < m_lstColumnInfo.size(); i++)
        pConfigModel->SetColumnLabel(i, m_lstColumnInfo.at(i).strCaption);

    return pConfigModel;
}

ConfigView* ConfigWidget::CreateView()
{
    return new ConfigView(this);
}

QList<QAction*> ConfigWidget::GetContextMenuActions()
{
    return QList<QAction*>();
}

void ConfigWidget::UpdateActions(const QModelIndexList &lstSelectedIndex)
{
    Q_UNUSED(lstSelectedIndex);
}

void ConfigWidget::PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject)
{
    Q_UNUSED(pProjectObject);
}

bool ConfigWidget::DropMimeData(const QMimeData *pMimeData, int iRow, int iColumn, const QModelIndex &parent)
{
    Q_UNUSED(pMimeData)
    Q_UNUSED(iRow)
    Q_UNUSED(iColumn)
    Q_UNUSED(parent)

    return false;
}

QStringList ConfigWidget::GetMimeTypes() const
{
    return QStringList();
}

bool ConfigWidget::RestoreState(const QByteArray &baState)
{
    if(!qobject_cast<Utils::ReadOnlyTableView*>(m_pView))
        return false;

    QDataStream DataStream(baState);

    QString strClassName;
    DataStream >> strClassName;
    if(strClassName != metaObject()->className())
        return false;

    int iRow, iColumn;
    DataStream >> iRow;
    DataStream >> iColumn;
    if(iRow >= 0 && iRow < m_pModel->rowCount() && iColumn >= 0 && iColumn < m_pModel->columnCount())
        m_pView->scrollTo(m_pModel->index(iRow, iColumn), QAbstractItemView::PositionAtTop);

    QList<int> lstSelectedRows;
    DataStream >> lstSelectedRows;
    foreach(int iRow, lstSelectedRows)
    {
        QModelIndex index = m_pModel->index(iRow, 0);
        if(index.isValid())
            m_pView->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }

    return true;
}

QByteArray ConfigWidget::SaveState()
{
    if(!qobject_cast<Utils::ReadOnlyTableView*>(m_pView))
        return QByteArray();

    QByteArray baState;
    QDataStream DataStream(&baState, QIODevice::WriteOnly);

    DataStream << QString(metaObject()->className());

    QModelIndex index = m_pView->indexAt(QPoint(0, 0));
    DataStream << index.row();
    DataStream << index.column();

    QList<int> lstSelectedRows;
    foreach(const QModelIndex &index, m_pView->selectionModel()->selectedRows())
        lstSelectedRows.append(index.row());
    DataStream << lstSelectedRows;

    return baState;
}

void ConfigWidget::ReadSettings(const QString &strSettingsName)
{
    QSettings *pSettings = Core::MainWindow::Instance()->GetSettings();

    pSettings->beginGroup("ConfigWidget");
    pSettings->beginGroup(strSettingsName);

    for(int i = 0; i < m_lstColumnInfo.size(); i++)
    {
        QVariant width = pSettings->value(m_lstColumnInfo.at(i).strCaption + "@Width");
        if(width.isValid())
            m_lstColumnInfo[i].iWidth = width.toInt();

        QVariant show = pSettings->value(m_lstColumnInfo.at(i).strCaption + "@Show");
        if(show.isValid())
            m_lstColumnInfo[i].bShow = show.toBool();
    }

    pSettings->endGroup();
    pSettings->endGroup();
}

void ConfigWidget::SaveSettings(const QString &strSettingsName)
{
    QSettings *pSettings = Core::MainWindow::Instance()->GetSettings();

    pSettings->beginGroup("ConfigWidget");
    pSettings->beginGroup(strSettingsName);

    for(int i = 0; i < m_lstColumnInfo.size(); i++)
    {
        pSettings->setValue(m_lstColumnInfo.at(i).strCaption + "@Width", m_lstColumnInfo.at(i).iWidth);
        pSettings->setValue(m_lstColumnInfo.at(i).strCaption + "@Show", m_lstColumnInfo.at(i).bShow);
    }

    pSettings->endGroup();
    pSettings->endGroup();
}

void ConfigWidget::SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject)
{
    Q_UNUSED(pProjectObject);
}

void ConfigWidget::SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!m_pModel)
        return;

    if(ConfigRow *pConfigRow = m_pModel->ConfigRowFromProjectObject(pProjectObject))
    {
        m_pModel->UpdateRow(RowDataFromProjectObject(pProjectObject), pConfigRow);
        m_pView->SelectConfigRow(pConfigRow);
    }
}

void ConfigWidget::SlotProjectObjectParentChanged(ProjectExplorer::PeProjectObject *pProjectObject, ProjectExplorer::PeProjectObject *pOldParentProjectObject)
{
    Q_UNUSED(pProjectObject);
    Q_UNUSED(pOldParentProjectObject);
}

void ConfigWidget::SlotProjectDeviceBayChanged(ProjectExplorer::PeDevice *pDevice, ProjectExplorer::PeBay *pOldBay)
{
    Q_UNUSED(pDevice);
    Q_UNUSED(pOldBay);
}

void ConfigWidget::SlotProjectObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject *pProjectObject)
{
    if(!m_pModel)
        return;

    if(ConfigRow *pConfigRow = m_pModel->ConfigRowFromProjectObject(pProjectObject))
        m_pModel->RemoveRow(pConfigRow);
}

void ConfigWidget::SlotProjectObjectDeleted()
{
}

void ConfigWidget::SlotActionShowColumn(bool bChecked)
{
    QAction *pAction = qobject_cast<QAction*>(sender());
    int iIndex = pAction->data().toInt();
    if(iIndex >= 0)
    {
        m_lstColumnInfo[iIndex].bShow = bChecked;
        m_pView->horizontalHeader()->setSectionHidden(iIndex, !bChecked);
    }
}

void ConfigWidget::SlotViewColumnResized(int iColumn, int iOldSize, int iNewSize)
{
    Q_UNUSED(iOldSize)

    if(!m_lstColumnInfo[iColumn].bShow)
        return;

    m_lstColumnInfo[iColumn].iWidth = iNewSize;
}

void ConfigWidget::SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    QItemSelectionModel *pSelectionModel = m_pView->selectionModel();
    QModelIndexList lstSelectedIndex = pSelectionModel->selectedRows();
    qSort(lstSelectedIndex);

    UpdateActions(lstSelectedIndex);
}

void ConfigWidget::SlotViewDoubleClicked(const QModelIndex &index)
{
    if(ConfigRow *pConfigRow = m_pModel->ConfigRowFromIndex(index))
    {
        if(ProjectExplorer::PeProjectObject *pProjectObject = pConfigRow->GetProjectObject())
            PropertyRequested(pProjectObject);
    }
}
