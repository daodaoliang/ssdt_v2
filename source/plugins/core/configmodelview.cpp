#include <QApplication>
#include <QDragMoveEvent>
#include <QPainter>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QScrollBar>

#include "projectexplorer/peprojectobject.h"

#include "configmodelview.h"
#include "configwidget.h"

using namespace Core;

class QStandardItemModelLessThan
{
public:
    inline QStandardItemModelLessThan()
        { }

    inline bool operator()(const QPair<QString, int> &l,
                           const QPair<QString, int> &r) const
    {
        return l.first < r.first;
    }
};

class QStandardItemModelGreaterThan
{
public:
    inline QStandardItemModelGreaterThan()
        { }

    inline bool operator()(const QPair<QString, int> &l,
                           const QPair<QString, int> &r) const
    {
        return r.first < l.first;
    }
};

///////////////////////////////////////////////////////////////////////
// ConfigRow member functions
///////////////////////////////////////////////////////////////////////
ConfigRow::ConfigRow(const QStringList &lstData, ProjectExplorer::PeProjectObject *pProjectObject) :
    m_lstData(lstData), m_pProjectObject(pProjectObject)
{
}

QStringList ConfigRow::GetData() const
{
    return m_lstData;
}

ProjectExplorer::PeProjectObject* ConfigRow::GetProjectObject() const
{
    return m_pProjectObject;
}

QString ConfigRow::GetColumnData(int iColumn) const
{
    if(iColumn >= 0 && iColumn < m_lstData.size())
        return m_lstData.at(iColumn);

    return QString();
}

QIcon ConfigRow::GetIcon() const
{
    return m_pProjectObject->GetDisplayIcon();
}

QFont ConfigRow::GetFont() const
{
    return m_font;
}

QColor ConfigRow::GetForeColor() const
{
    return m_crFore;
}

///////////////////////////////////////////////////////////////////////
// ConfigModel member functions
///////////////////////////////////////////////////////////////////////
ConfigModel::ConfigModel(ConfigWidget *pConfigWidget, int iColumnCount) :
    QAbstractItemModel(pConfigWidget), m_pConfigWidget(pConfigWidget)
{
    m_vcColumnLabel = QVector<QString>(iColumnCount, "");
}

ConfigModel::~ConfigModel()
{
    qDeleteAll(m_lstRows);
}

QModelIndex ConfigModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if(column < 0 || column >= m_vcColumnLabel.size() || row < 0 || row >= m_lstRows.size())
        return QModelIndex();

    return createIndex(row, column, m_lstRows.at(row));
}

QModelIndex ConfigModel::parent (const QModelIndex &index) const
{
    Q_UNUSED(index)

    return QModelIndex();
}

int ConfigModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_lstRows.size();
}

int ConfigModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_vcColumnLabel.size();
}

QVariant ConfigModel::data(const QModelIndex &index, int role) const
{
    if(ConfigRow *pConfigRow = ConfigRowFromIndex(index))
    {
        if(role == Qt::DisplayRole || role == Qt::EditRole)
            return pConfigRow->GetColumnData(index.column());
        else if(role == Qt::DecorationRole && index.column() == 0)
            return pConfigRow->GetIcon();
        else if(role == Qt::TextAlignmentRole)
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        else if(role == Qt::FontRole)
            return pConfigRow->GetFont();
        else if(role == Qt::ForegroundRole)
            return pConfigRow->GetForeColor();
    }

    return QVariant();
}

Qt::ItemFlags ConfigModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsDropEnabled;
}

QVariant ConfigModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole)
        return (orientation == Qt::Horizontal ? m_vcColumnLabel.at(section) : QString("%1").arg(section + 1));
    else if(role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

    return QVariant();
}

bool ConfigModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole)
    {
        const QString strValue = value.toString().trimmed();

        if(ConfigRow *pConfigRow = ConfigRowFromIndex(index))
        {
            if(index.column() < 0 || index.column() >= pConfigRow->m_lstData.size())
                return false;

            pConfigRow->m_lstData[index.column()] = strValue;

            emit dataChanged(index, index);
            return true;
        }
    }
    else if(role == Qt::FontRole)
    {
        QFont font = value.value<QFont>();
        if(ConfigRow *pConfigRow = ConfigRowFromIndex(index))
        {
            if(index.column() < 0 || index.column() >= pConfigRow->m_lstData.size())
                return false;

            pConfigRow->m_font = font;

            emit dataChanged(index, index);
            return true;
        }
    }
    else if(role == Qt::ForegroundRole)
    {
        QColor crFore = value.value<QColor>();
        if(ConfigRow *pConfigRow = ConfigRowFromIndex(index))
        {
            if(index.column() < 0 || index.column() >= pConfigRow->m_lstData.size())
                return false;

            pConfigRow->m_crFore = crFore;

            emit dataChanged(index, index);
            return true;
        }
    }

    return false;
}

Qt::DropActions ConfigModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

bool ConfigModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if(action == Qt::IgnoreAction)
        return true;

    return m_pConfigWidget->DropMimeData(data, row, column, parent);
}

QStringList ConfigModel::mimeTypes() const
{
    return m_pConfigWidget->GetMimeTypes();
}

void ConfigModel::sort(int column, Qt::SortOrder order)
{
    if((column < 0) || rowCount() == 0 || column >= columnCount())
        return;

    emit layoutAboutToBeChanged();

    QVector<QPair<QString, int> > sortable;
    sortable.reserve(rowCount());

    for(int row = 0; row < rowCount(); ++row)
    {
        QString strValue = m_lstRows.at(row)->GetColumnData(column);
        sortable.append(QPair<QString,int>(strValue, row));
    }

    if (order == Qt::AscendingOrder)
    {
        QStandardItemModelLessThan lt;
        qStableSort(sortable.begin(), sortable.end(), lt);
    }
    else
    {
        QStandardItemModelGreaterThan gt;
        qStableSort(sortable.begin(), sortable.end(), gt);
    }

    QModelIndexList changedPersistentIndexesFrom, changedPersistentIndexesTo;
    QVector<ConfigRow*> sorted_children(rowCount());
    for (int i = 0; i < rowCount(); ++i)
    {
        int r = sortable.at(i).second;
        for (int c = 0; c < columnCount(); ++c)
        {
            ConfigRow *pConfigRow = m_lstRows.at(r);
            sorted_children[i] = pConfigRow;

                QModelIndex from = createIndex(r, c, pConfigRow);
                if (persistentIndexList().contains(from)) {
                    QModelIndex to = createIndex(i, c, pConfigRow);
                    changedPersistentIndexesFrom.append(from);
                    changedPersistentIndexesTo.append(to);
            }
        }
    }

    m_lstRows = sorted_children.toList();

    changePersistentIndexList(changedPersistentIndexesFrom, changedPersistentIndexesTo);

    emit layoutChanged();
}

void ConfigModel::SetColumnLabel(int iColumn, const QString &strLabel)
{
    if(iColumn >= 0 && iColumn < m_vcColumnLabel.size())
        m_vcColumnLabel[iColumn] = strLabel;
}

ConfigRow* ConfigModel::ConfigRowFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject) const
{
    foreach(ConfigRow *pConfigRow, m_lstRows)
    {
        if(pConfigRow->GetProjectObject() == pProjectObject)
            return pConfigRow;
    }

    return 0;
}

ConfigRow* ConfigModel::ConfigRowFromRow(int iRow) const
{
    if(iRow >= 0 && iRow < m_lstRows.size())
        return m_lstRows.at(iRow);

    return 0;
}

ConfigRow* ConfigModel::ConfigRowFromIndex(const QModelIndex &index) const
{
    if(index.isValid())
        return static_cast<ConfigRow*>(index.internalPointer());

    return 0;
}

QModelIndex ConfigModel::IndexFromConfigRow(ConfigRow *pConfigRow)
{
    int iRow = m_lstRows.indexOf(pConfigRow);
    if(iRow == -1)
        return QModelIndex();

    return index(iRow, 0);
}

void ConfigModel::InsertRow(ConfigRow *pConfigRow, int iRow)
{
    if(iRow < 0)
        iRow = 0;
    else if(iRow > m_lstRows.size())
        iRow = m_lstRows.size();

    beginInsertRows(QModelIndex(), iRow, iRow);
    m_lstRows.insert(iRow, pConfigRow);
    endInsertRows();
}

void ConfigModel::AppendRow(ConfigRow *pConfigRow)
{
    beginInsertRows(QModelIndex(), m_lstRows.size(), m_lstRows.size());
    m_lstRows.append(pConfigRow);
    endInsertRows();
}

ConfigRow* ConfigModel::TakeRow(int iRow)
{
    if(iRow < 0 || iRow >= m_lstRows.size())
        return 0;

    beginRemoveRows(QModelIndex(), iRow, iRow);
    ConfigRow *pConfigRow = m_lstRows.takeAt(iRow);
    endRemoveRows();

    return pConfigRow;
}

bool ConfigModel::RemoveRow(int iRow)
{
    if(ConfigRow *pConfigRow = TakeRow(iRow))
    {
        delete pConfigRow;
        return true;
    }

    return false;
}

bool ConfigModel::RemoveRow(ConfigRow *pConfigRow)
{
    return RemoveRow(m_lstRows.indexOf(pConfigRow));
}

void ConfigModel::UpdateRow(const QStringList &lstData, ConfigRow *pConfigRow)
{
    QModelIndex index = IndexFromConfigRow(pConfigRow);
    if(index.isValid() && pConfigRow->m_lstData.size() == lstData.size())
    {
        pConfigRow->m_lstData = lstData;
        emit dataChanged(index, index);
    }
}

void ConfigModel::CleanData()
{
    if(m_lstRows.isEmpty())
        return;

    beginRemoveRows(QModelIndex(), 0, m_lstRows.size() - 1);
    qDeleteAll(m_lstRows);
    m_lstRows.clear();
    endRemoveRows();
}

///////////////////////////////////////////////////////////////////////
// ConfigView member functions
///////////////////////////////////////////////////////////////////////
ConfigView::ConfigView(QWidget *pParent) : Utils::ReadOnlyTableView(pParent)
{
    setObjectName("Styled_TableView");
    setFrameStyle(QFrame::NoFrame);
    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    verticalHeader()->hide();
#if QT_VERSION >= 0x050000
    verticalHeader()->setSectionsClickable(false);
    horizontalHeader()->setSectionsClickable(true);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
    verticalHeader()->setClickable(false);
    horizontalHeader()->setClickable(true);
    verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
    setContextMenuPolicy(Qt::ActionsContextMenu);
    horizontalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
    verticalHeader()->setContextMenuPolicy(Qt::ActionsContextMenu);
    horizontalHeader()->setHighlightSections(false);
    horizontalHeader()->setStretchLastSection(true);
    setShowGrid(false);
}

void ConfigView::SelectConfigRow(ConfigRow *pConfigRow)
{
    if(ConfigModel *pMode = qobject_cast<ConfigModel*>(model()))
    {
        QModelIndex index = pMode->IndexFromConfigRow(pConfigRow);
        selectRow(index.row());
        scrollTo(index);
        setFocus();
    }
}
