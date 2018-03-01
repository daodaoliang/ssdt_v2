#ifndef CONFIGMODELVIEW_H
#define CONFIGMODELVIEW_H

#include <QAbstractItemModel>
#include "utils/readonlyview.h"
#include "core_global.h"

namespace ProjectExplorer {

class PeProjectObject;

} // namespace ProjectExplorer

namespace Core {

class ConfigWidget;
class CORE_EXPORT ConfigRow
{
// Construction and Destruction
public:
    ConfigRow(const QStringList &lstData, ProjectExplorer::PeProjectObject *pProjectObject);

// Operations
public:
    QStringList                         GetData() const;
    ProjectExplorer::PeProjectObject*   GetProjectObject() const;
    QString                             GetColumnData(int iColumn) const;
    QIcon                               GetIcon() const;
    QFont                               GetFont() const;
    QColor                              GetForeColor() const;

// Properties
private:
    QStringList                         m_lstData;
    ProjectExplorer::PeProjectObject    *m_pProjectObject;
    QFont                               m_font;
    QColor                              m_crFore;

    friend class ConfigModel;
};

class CORE_EXPORT ConfigModel : public QAbstractItemModel
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigModel(ConfigWidget *pConfigWidget, int iColumnCount);
    ~ConfigModel();

// Operations
public:
    virtual QModelIndex     index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex     parent (const QModelIndex &index) const ;
    virtual int             rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int             columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant        data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags   flags(const QModelIndex &index) const;
    virtual QVariant        headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool            setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual Qt::DropActions supportedDropActions() const;
    virtual bool            dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    virtual QStringList     mimeTypes() const;
    virtual void            sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    void                    SetColumnLabel(int iColumn, const QString &strLabel);

    ConfigRow*              ConfigRowFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject) const;
    ConfigRow*              ConfigRowFromRow(int iRow) const;
    ConfigRow*              ConfigRowFromIndex(const QModelIndex &index) const;
    QModelIndex             IndexFromConfigRow(ConfigRow *pConfigRow);

    void                    SetRowFont(ConfigRow *pConfigRow, const QFont &font);
    void                    SetRowForeColor(ConfigRow *pConfigRow, const QColor &crFore);

    void                    InsertRow(ConfigRow *pConfigRow, int iRow);
    void                    AppendRow(ConfigRow *pConfigRow);
    bool                    RemoveRow(int iRow);
    bool                    RemoveRow(ConfigRow *pConfigRow);
    ConfigRow*              TakeRow(int iRow);
    void                    UpdateRow(const QStringList &lstData, ConfigRow *pConfigRow);
    void                    CleanData();

// Properties
protected:
    ConfigWidget            *m_pConfigWidget;
    QVector<QString>        m_vcColumnLabel;
    QList<ConfigRow*>       m_lstRows;
};

class CORE_EXPORT ConfigView : public Utils::ReadOnlyTableView
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigView(QWidget *pParent = 0);

// Operations
public:
    void    SelectConfigRow(ConfigRow *pConfigRow);
};

} // namespace Core

#endif // CONFIGMODELVIEW_H
