#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "styledui/styledwindow.h"

namespace StyledUi {

class StyledButton;

} // namespace Utils

namespace ProjectExplorer {

class PeProjectObject;
class PeProjectVersion;

} // namespace ProjectExplorer

class ExplorerWidget;
class CableWidget;
class MainWindow : public StyledUi::StyledWindow
{
    Q_OBJECT

// Construction and Destruction
public:
    MainWindow(QWidget *pParent = 0);
    ~MainWindow();

// Operations
public:
    static  MainWindow*                 Instance();
    bool                                Initialize();

    ProjectExplorer::PeProjectObject*   GetActiveObject(bool *pBayObject = 0) const;
    void                                SetActiveObject(ProjectExplorer::PeProjectObject *pProjectObject, bool bBayObject);

private:
    void                                SetupToolBar();
    bool                                ExportSqliteDb(const QString &strSqliteFileName);
    bool                                ExportCsv(const QString &strDir);

// Properties
private:
    static MainWindow*                  m_pInstance;
    QWidget                             *m_pToolBar;
    ExplorerWidget                      *m_pExplorerWidget;
    CableWidget                         *m_pCableWidget;

    StyledUi::StyledButton              *m_pButtonOpen;
    StyledUi::StyledButton              *m_pButtonClose;
    StyledUi::StyledButton              *m_pButtonRefresh;
    StyledUi::StyledButton              *m_pButtonExportDb;
    StyledUi::StyledButton              *m_pButtonExportExcel;

private slots:
    void SlotActionOpen();
    void SlotActionClose();
    void SlotActionRefresh();
    void SlotActionExportDb();
    void SlotActionExportExcel();
};

#endif // MAINWINDOW_H
