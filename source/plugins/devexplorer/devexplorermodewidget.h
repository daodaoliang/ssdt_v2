#ifndef DEVEXPLORERMODEWIDGET_H
#define DEVEXPLORERMODEWIDGET_H

#include <QWidget>
#include <QMap>
#include <QPointer>
#include "styledui/styledbar.h"
#include "styledui/styledwidget.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QHBoxLayout;
class QStackedWidget;
class QAction;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeProjectObject;
class PeBay;
class PeRoom;
class PeCubicle;
class PeDevice;
class PeBoard;
class PePort;

} // namespace ProjectExplorer

namespace DevExplorer {
namespace Internal {

class ConfigWidgetDeviceObject;
class DeviceToolWidget : public StyledUi::StyledBar
{
    Q_OBJECT

// Construction and Destruction
public:
    DeviceToolWidget(QWidget *pParent = 0);
    ~DeviceToolWidget();

// Operations
public:
    void    SetConfigWidget(ConfigWidgetDeviceObject *pConfigWidget);

// Properties
private:
    QLabel      *m_pLabelDisplayIcon;
    QLabel      *m_pLabelDisplayName;
    QHBoxLayout *m_pHBoxLayout;
};

class DevExplorerModeWidget : public StyledUi::StyledWidget
{
    Q_OBJECT

// Construction and Destruction
public:
    DevExplorerModeWidget(QWidget *pParent = 0);
    ~DevExplorerModeWidget();

// Operations
public:
    static DevExplorerModeWidget*   Instance();

    ProjectExplorer::PeBay*         CreateBay(ProjectExplorer::PeProjectObject *pParentObject);
    void                            UpdateBay(ProjectExplorer::PeBay *pBay);

    ProjectExplorer::PeRoom*        CreateRoom(ProjectExplorer::PeProjectObject *pParentObject);
    void                            UpdateRoom(ProjectExplorer::PeRoom *pRoom);

    ProjectExplorer::PeCubicle*     CreateCubicle(ProjectExplorer::PeProjectObject *pParentObject);
    void                            UpdateCubicle(ProjectExplorer::PeCubicle *pCubicle);

    ProjectExplorer::PeDevice*      CreateIED(ProjectExplorer::PeProjectObject *pParentObject);
    ProjectExplorer::PeDevice*      CreateSwitch(ProjectExplorer::PeProjectObject *pParentObject);
    void                            UpdateDevice(ProjectExplorer::PeDevice *pDevice);

    ProjectExplorer::PeBoard*       CreateBoard(ProjectExplorer::PeDevice *pParentDevice);
    void                            UpdateBoard(ProjectExplorer::PeBoard *pBoard);
    ProjectExplorer::PeBoard*       CloneBoard(ProjectExplorer::PeBoard *pSrcBoard, ProjectExplorer::PeDevice *pParentDevice);

    ProjectExplorer::PePort*        CreateFiberPort(ProjectExplorer::PeBoard *pParentBoard);
    void                            UpdatePort(ProjectExplorer::PePort *pPort);

    void                            DeleteObjects(const QList<ProjectExplorer::PeProjectObject*> &lstProjectObjects);
    void                            UpdateObject(ProjectExplorer::PeProjectObject *pProjectObject);

    void                            ActivateObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject);
    void                            SetContextMenuObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject);

private:
    void                            SetupContextMenuActions();
    void                            SetCurrentConfigWidget(ConfigWidgetDeviceObject *pConfigWidget);
    ConfigWidgetDeviceObject*       GetCurrentConfigWidget() const;
    ConfigWidgetDeviceObject*       CreateConfigWidgetFromObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject);

// Properties
private:
    static DevExplorerModeWidget    *m_pInstance;

    DeviceToolWidget                *m_pDeviceToolWidget;
    QStackedWidget                  *m_pStackedWidget;
    QPointer<ProjectExplorer::PeProjectObject>  m_pCopiedProjectObject;

    QAction                         *m_pContextMenuActionNewBay;
    QAction                         *m_pContextMenuActionNewRoom;
    QAction                         *m_pContextMenuActionNewCubicle;
    QAction                         *m_pContextMenuActionNewIED;
    QAction                         *m_pContextMenuActionNewSwitch;
    QAction                         *m_pContextMenuActionNewBoard;
    QAction                         *m_pContextMenuActionNewFiberPort;
    QAction                         *m_pContextMenuActionDelete;
    QAction                         *m_pContextMenuActionProperty;
    QAction                         *m_pContextMenuActionCopy;
    QAction                         *m_pContextMenuActionPaste;

private slots:
    void    SlotContextMenuActionNewBay();
    void    SlotContextMenuActionNewRoom();
    void    SlotContextMenuActionNewCubicle();
    void    SlotContextMenuActionNewIED();
    void    SlotContextMenuActionNewSwitch();
    void    SlotContextMenuActionNewBoard();
    void    SlotContextMenuActionNewFiberPort();
    void    SlotContextMenuActionDelete();
    void    SlotContextMenuActionProperty();
    void    SlotContextMenuActionCopy();
    void    SlotContextMenuActionPaste();
};

} // namespace Internal
} // namespace DevExplorer

#endif // DEVEXPLORERMODEWIDGET_H
