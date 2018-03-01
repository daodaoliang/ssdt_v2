#ifndef SSDAUTOCONNDLG_H
#define SSDAUTOCONNDLG_H

#include <QMap>
#include "styledui/styleddialog.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QTextEdit;
QT_END_NAMESPACE

namespace Utils {

class PathChooser;

} // namespace Utils

namespace ProjectExplorer {

class PeBay;
class PeCubicle;
class PeDevice;
class PeVTerminal;
class PeStrap;
class PeInfoSet;
class PeVTerminalConn;

} // namespace ProjectExplorer

namespace SsdAutoConn {
namespace Internal {

class SsdAutoConnDlg : public StyledUi::StyledDialog
{
    Q_OBJECT

// Construction and Destruction
public:
    SsdAutoConnDlg(QWidget *pParent = 0);
    ~SsdAutoConnDlg();

// Operations
public:
    virtual QSize       sizeHint() const;

private:
    void                WaitMSec(int iMSec);

// Properties
private:
    QComboBox                   *m_pComboBoxProject;
    Utils::PathChooser          *m_pPathChooserScdFileName;
    QTextEdit                   *m_pTextEdit;

private slots:
    void SlotActionExecute();
};

} // namespace Internal
} // namespace SsdAutoConn

#endif // SSDAUTOCONNDLG_H
