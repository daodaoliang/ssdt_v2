#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include "core_global.h"

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeProjectObject;
class PeBay;
class PeDevice;

} // namespace ProjectExplorer

namespace Core {

class ConfigModel;
class ConfigView;
class CORE_EXPORT ConfigWidget : public QWidget
{
    Q_OBJECT

// Structures and Enumerations
public:
    struct COLUMN_INFO
    {
        QString strCaption;
        int     iWidth;
        bool    bShow;
        bool    bEditable;

        COLUMN_INFO(const QString &strC, int i, bool b, bool e = true)
        { strCaption = strC; iWidth = i; bShow = b; bEditable = e;}
    };

// Construction and Destruction
public:
    ConfigWidget(QWidget *pParent = 0);
    ~ConfigWidget();

// Operations
public:
    virtual void            SetupConfigWidget();
    virtual bool            BuildModel(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual void            UpdateModel();
    virtual void            CleanModel();
    virtual bool            RestoreState(const QByteArray &baState);
    virtual QByteArray      SaveState();

    ProjectExplorer::PeProjectObject*   GetProjectObject() const;

protected:
    virtual ConfigModel*    CreateModel();
    virtual ConfigView*     CreateView();
    virtual bool            BuildModelData() = 0;
    virtual QStringList     RowDataFromProjectObject(ProjectExplorer::PeProjectObject *pProjectObject) = 0;
    virtual QList<QAction*> GetContextMenuActions();
    virtual void            UpdateActions(const QModelIndexList &lstSelectedIndex);
    virtual void            PropertyRequested(ProjectExplorer::PeProjectObject *pProjectObject);

    virtual bool            DropMimeData(const QMimeData *pMimeData, int iRow, int iColumn, const QModelIndex &parent);
    virtual QStringList     GetMimeTypes() const;

    void                    ReadSettings(const QString &strSettingsName);
    void                    SaveSettings(const QString &strSettingsName);

// Properties
protected:
    ProjectExplorer::PeProjectObject    *m_pProjectObject;

    ConfigModel             *m_pModel;
    ConfigView              *m_pView;
    QList<COLUMN_INFO>      m_lstColumnInfo;

protected slots:
    virtual void    SlotProjectObjectCreated(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual void    SlotProjectObjectPropertyChanged(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual void    SlotProjectObjectParentChanged(ProjectExplorer::PeProjectObject *pProjectObject, ProjectExplorer::PeProjectObject *pOldParentProjectObject);
    virtual void    SlotProjectDeviceBayChanged(ProjectExplorer::PeDevice *pDevice, ProjectExplorer::PeBay *pOldBay);
    virtual void    SlotProjectObjectAboutToBeDeleted(ProjectExplorer::PeProjectObject *pProjectObject);
    virtual void    SlotProjectObjectDeleted();

private slots:
    void            SlotActionShowColumn(bool bChecked);
    void            SlotViewColumnResized(int iColumn, int iOldSize, int iNewSize);
    void            SlotViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void            SlotViewDoubleClicked(const QModelIndex &index);

    friend class ConfigModel;
};

} // namespace Core

#endif // CONFIGWIDGET_H
