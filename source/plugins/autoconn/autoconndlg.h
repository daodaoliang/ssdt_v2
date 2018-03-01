#ifndef AUTOCONNDLG_H
#define AUTOCONNDLG_H

#include <QMap>
#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QTextEdit;
QT_END_NAMESPACE

namespace ProjectExplorer {

class PeProjectVersion;

} // namespace ProjectExplorer

namespace AutoConn {
namespace Internal {

class AutoConnDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    AutoConnDlg(QWidget *pParent = 0);
    ~AutoConnDlg();

// Operations
public:
    virtual QSize       sizeHint() const;

private:
    bool                ExportBayConfig(ProjectExplorer::PeProjectVersion *pProjectVersion, const QString &strFileName);

// Properties
private:
    QComboBox           *m_pComboBoxProject;
    QTextEdit           *m_pTextEdit;

private slots:
    void SlotActionExecute();
};

} // namespace Internal
} // namespace AutoConn

#endif // AUTOCONNDLG_H
