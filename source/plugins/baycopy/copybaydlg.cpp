#include <QApplication>
#include <QFormLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTabBar>
#include <QStackedWidget>
#include <QHeaderView>
#include <QAction>

#include "utils/waitcursor.h"
#include "utils/readonlyview.h"
#include "utils/searchcombobox.h"
#include "sclparser/sclelement.h"
#include "projectexplorer/basemanager.h"
#include "projectexplorer/pbvlevel.h"
#include "projectexplorer/pbmanufacture.h"
#include "projectexplorer/pbdevicetype.h"
#include "projectexplorer/pbdevicecategory.h"
#include "projectexplorer/pbbaycategory.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peproject.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/pevterminal.h"
#include "projectexplorer/pestrap.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pevterminal.h"
#include "projectexplorer/pevterminalconn.h"
#include "projectexplorer/peinfoset.h"
#include "projectexplorer/peport.h"
#include "projectexplorer/peboard.h"
#include "core/configcoreoperation.h"

#include "copybaydlg.h"
#include "copybaymodelview.h"
#include "selectbaydlg.h"
#include "selectcubicledlg.h"
#include "selectdevicedlg.h"
#include "vterminalconndlg.h"
#include "infosetdlg.h"

using namespace BayCopy::Internal;

static bool ValidatePortConnection(ProjectExplorer::PeInfoSet *pInfoSet,
                                   ProjectExplorer::PePort *pPort,
                                   const QList<ProjectExplorer::PeInfoSet*> &lstAllInfoSets)
{
    QList<ProjectExplorer::PePort*> lstConnectedPorts;
    foreach(ProjectExplorer::PeInfoSet *pI, lstAllInfoSets)
    {
        ProjectExplorer::PePort *pPortConnected = 0;
        pI->IsContaintPort(pPort, &pPortConnected);

        if(pPortConnected && !lstConnectedPorts.contains(pPortConnected))
            lstConnectedPorts.append(pPortConnected);
    }

    if(lstConnectedPorts.isEmpty())
        return true;

    if(lstConnectedPorts.size() == 1)
    {
        ProjectExplorer::PePort *pPortConnectedOld = lstConnectedPorts.first();

        ProjectExplorer::PePort *pPortConnectedNew = 0;
        pInfoSet->IsContaintPort(pPort, &pPortConnectedNew);

        if(pPortConnectedOld == pPortConnectedNew)
            return true;
    }

    return false;
}

static QString GetPortPath(ProjectExplorer::PePort *pPortTx, ProjectExplorer::PePort *pPortRx)
{
    QString strPortPath;

    if(pPortTx && pPortRx)
    {
        if(pPortTx->GetParentBoard() == pPortRx->GetParentBoard())
        {
            if(ProjectExplorer::PeDevice *pDevice = pPortTx->GetParentDevice())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    strPortPath = QString("%1<%2:%3/%4>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetParentBoard()->GetDisplayName()).arg(pPortTx->GetDisplayName()).arg(pPortRx->GetDisplayName());
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                    strPortPath = QString("%1<%2/%3>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetDisplayName()).arg(pPortRx->GetDisplayName());
            }
        }
        else
        {
            if(ProjectExplorer::PeDevice *pDevice = pPortTx->GetParentDevice())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    strPortPath = QString("%1<%2:%3>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetParentBoard()->GetDisplayName()).arg(pPortTx->GetDisplayName());
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                    strPortPath = QString("%1<%2>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetDisplayName());
            }

            if(ProjectExplorer::PeDevice *pDevice = pPortRx->GetParentDevice())
            {
                if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                    strPortPath = QString("%1\n%2<%3:%4>").arg(strPortPath).arg(pDevice->GetDisplayName()).arg(pPortRx->GetParentBoard()->GetDisplayName()).arg(pPortRx->GetDisplayName());
                else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                    strPortPath = QString("%1\n%2<%3>").arg(strPortPath).arg(pDevice->GetDisplayName()).arg(pPortRx->GetDisplayName());
            }
        }
    }
    else if(pPortTx && !pPortRx)
    {
        if(ProjectExplorer::PeDevice *pDevice = pPortTx->GetParentDevice())
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                strPortPath = QString("%1<%2:%3>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetParentBoard()->GetDisplayName()).arg(pPortTx->GetDisplayName());
            else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                strPortPath = QString("%1<%2>").arg(pDevice->GetDisplayName()).arg(pPortTx->GetDisplayName());
        }
    }
    else if(pPortRx && !pPortTx)
    {
        if(ProjectExplorer::PeDevice *pDevice = pPortRx->GetParentDevice())
        {
            if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtIED)
                strPortPath = QString("%1<%2:%3>").arg(pDevice->GetDisplayName()).arg(pPortRx->GetParentBoard()->GetDisplayName()).arg(pPortRx->GetDisplayName());
            else if(pDevice->GetDeviceType() == ProjectExplorer::PeDevice::dtSwitch)
                strPortPath = QString("%1<%2>").arg(pDevice->GetDisplayName()).arg(pPortRx->GetDisplayName());
        }
    }

    return strPortPath;
}

///////////////////////////////////////////////////////////////////////
// ReplaceDlg member functions
///////////////////////////////////////////////////////////////////////
ReplaceDlg::ReplaceDlg(QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Replace"));
    setModal(false);

    m_pCheckBoxBayObjectName = new QCheckBox(tr("Bay Object Name"), this);
    m_pLineEditBayObjectNameSrc = new QLineEdit(this);
    m_pLineEditBayObjectNameSrc->setPlaceholderText(tr("Search Content"));
    m_pLineEditBayObjectNameDst = new QLineEdit(this);
    m_pLineEditBayObjectNameDst->setPlaceholderText(tr("Replac Content"));
    m_pCheckBoxBayObjectDesc = new QCheckBox(tr("Bay Object Description"), this);
    m_pLineEditBayObjectDescSrc = new QLineEdit(this);
    m_pLineEditBayObjectDescSrc->setPlaceholderText(tr("Search Content"));
    m_pLineEditBayObjectDescDst = new QLineEdit(this);
    m_pLineEditBayObjectDescDst->setPlaceholderText(tr("Replac Content"));
    m_pCheckBoxCubicleObjectName = new QCheckBox(tr("Cubicle Object Name"), this);
    m_pLineEditCubicleObjectNameSrc = new QLineEdit(this);
    m_pLineEditCubicleObjectNameSrc->setPlaceholderText(tr("Search Content"));
    m_pLineEditCubicleObjectNameDst = new QLineEdit(this);
    m_pLineEditCubicleObjectNameDst->setPlaceholderText(tr("Replac Content"));
    m_pCheckBoxCubicleObjectDesc = new QCheckBox(tr("Cubicle Object Description"), this);
    m_pLineEditCubicleObjectDescSrc = new QLineEdit(this);
    m_pLineEditCubicleObjectDescSrc->setPlaceholderText(tr("Search Content"));
    m_pLineEditCubicleObjectDescDst = new QLineEdit(this);
    m_pLineEditCubicleObjectDescDst->setPlaceholderText(tr("Replac Content"));

    m_pCheckBoxVTerminalDesc = new QCheckBox(tr("Virtual Terminal Description"), this);
    m_pLineEditVTerminalDescSrc = new QLineEdit(this);
    m_pLineEditVTerminalDescSrc->setPlaceholderText(tr("Search Content"));
    m_pLineEditVTerminalDescDst = new QLineEdit(this);
    m_pLineEditVTerminalDescDst->setPlaceholderText(tr("Replac Content"));
    m_pCheckBoxInfoSetDesc = new QCheckBox(tr("Information Logic Description"), this);
    m_pLineEditInfoSetDescSrc = new QLineEdit(this);
    m_pLineEditInfoSetDescSrc->setPlaceholderText(tr("Search Content"));
    m_pLineEditInfoSetDescDst = new QLineEdit(this);
    m_pLineEditInfoSetDescDst->setPlaceholderText(tr("Replac Content"));

    QFrame* pFrameLine = new QFrame(this);
    pFrameLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);

    QGroupBox *pGroupBox = new QGroupBox(this);
    QGridLayout *pGridLayout = new QGridLayout(pGroupBox);
    pGridLayout->addWidget(m_pCheckBoxBayObjectName, 0, 0);
    pGridLayout->addWidget(m_pLineEditBayObjectNameSrc, 0, 1);
    pGridLayout->addWidget(m_pLineEditBayObjectNameDst, 0, 2);
    pGridLayout->addWidget(m_pCheckBoxBayObjectDesc, 1, 0);
    pGridLayout->addWidget(m_pLineEditBayObjectDescSrc, 1, 1);
    pGridLayout->addWidget(m_pLineEditBayObjectDescDst, 1, 2);
    pGridLayout->addWidget(m_pCheckBoxCubicleObjectName, 2, 0);
    pGridLayout->addWidget(m_pLineEditCubicleObjectNameSrc, 2, 1);
    pGridLayout->addWidget(m_pLineEditCubicleObjectNameDst, 2, 2);
    pGridLayout->addWidget(m_pCheckBoxCubicleObjectDesc, 3, 0);
    pGridLayout->addWidget(m_pLineEditCubicleObjectDescSrc, 3, 1);
    pGridLayout->addWidget(m_pLineEditCubicleObjectDescDst, 3, 2);

    pGridLayout->addWidget(pFrameLine, 4, 0, 1, 3);

    pGridLayout->addWidget(m_pCheckBoxVTerminalDesc, 5, 0);
    pGridLayout->addWidget(m_pLineEditVTerminalDescSrc, 5, 1);
    pGridLayout->addWidget(m_pLineEditVTerminalDescDst, 5, 2);
    pGridLayout->addWidget(m_pCheckBoxInfoSetDesc, 6, 0);
    pGridLayout->addWidget(m_pLineEditInfoSetDescSrc, 6, 1);
    pGridLayout->addWidget(m_pLineEditInfoSetDescDst, 6, 2);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    QPushButton *pPushButtonReplace = pDialogButtonBox->addButton(tr("Replace"), QDialogButtonBox::ActionRole);
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(pPushButtonReplace, SIGNAL(clicked()), this, SLOT(SlotPushButtonReplaceClicked()));

    QVBoxLayout *pVBoxLayout = GetClientLayout();
    pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    pVBoxLayout->addWidget(pGroupBox);
    pVBoxLayout->addSpacing(15);
    pVBoxLayout->addWidget(pDialogButtonBox);

    connect(m_pCheckBoxBayObjectName, SIGNAL(toggled(bool)), this, SLOT(SlotCheckBoxToggled(bool)));
    connect(m_pCheckBoxBayObjectDesc, SIGNAL(toggled(bool)), this, SLOT(SlotCheckBoxToggled(bool)));
    connect(m_pCheckBoxCubicleObjectName, SIGNAL(toggled(bool)), this, SLOT(SlotCheckBoxToggled(bool)));
    connect(m_pCheckBoxCubicleObjectDesc, SIGNAL(toggled(bool)), this, SLOT(SlotCheckBoxToggled(bool)));
    connect(m_pCheckBoxVTerminalDesc, SIGNAL(toggled(bool)), this, SLOT(SlotCheckBoxToggled(bool)));
    connect(m_pCheckBoxInfoSetDesc, SIGNAL(toggled(bool)), this, SLOT(SlotCheckBoxToggled(bool)));
    SlotCheckBoxToggled(false);
}

bool ReplaceDlg::GetReplaceBayObjectName() const
{
    return m_pCheckBoxBayObjectName->isChecked();
}

QString ReplaceDlg::GetBayObjectNameSrc() const
{
    return m_pLineEditBayObjectNameSrc->text().trimmed();
}

QString ReplaceDlg::GetBayObjectNameDst() const
{
    return m_pLineEditBayObjectNameDst->text().trimmed();
}

bool ReplaceDlg::GetReplaceBayObjectDesc() const
{
    return m_pCheckBoxBayObjectDesc->isChecked();
}

QString ReplaceDlg::GetBayObjectDescSrc() const
{
    return m_pLineEditBayObjectDescSrc->text().trimmed();
}

QString ReplaceDlg::GetBayObjectDescDst() const
{
    return m_pLineEditBayObjectDescDst->text().trimmed();
}

bool ReplaceDlg::GetReplaceCubicleObjectName() const
{
    return m_pCheckBoxCubicleObjectName->isChecked();
}

QString ReplaceDlg::GetCubicleObjectNameSrc() const
{
    return m_pLineEditCubicleObjectNameSrc->text().trimmed();
}

QString ReplaceDlg::GetCubicleObjectNameDst() const
{
    return m_pLineEditCubicleObjectNameDst->text().trimmed();
}

bool ReplaceDlg::GetReplaceCubicleObjectDesc() const
{
    return m_pCheckBoxCubicleObjectDesc->isChecked();
}

QString ReplaceDlg::GetCubicleObjectDescSrc() const
{
    return m_pLineEditCubicleObjectDescSrc->text().trimmed();
}

QString ReplaceDlg::GetCubicleObjectDescDst() const
{
    return m_pLineEditCubicleObjectDescDst->text().trimmed();
}

bool ReplaceDlg::GetReplaceVTerminalDesc() const
{
    return m_pCheckBoxVTerminalDesc->isChecked();
}

QString ReplaceDlg::GetVTerminalDescSrc() const
{
    return m_pLineEditVTerminalDescSrc->text().trimmed();
}

QString ReplaceDlg::GetVTerminalDescDst() const
{
    return m_pLineEditVTerminalDescDst->text().trimmed();
}

bool ReplaceDlg::GetReplaceInfoSetDesc() const
{
    return m_pCheckBoxInfoSetDesc->isChecked();
}

QString ReplaceDlg::GetInfoSetDescSrc() const
{
    return m_pLineEditInfoSetDescSrc->text().trimmed();
}

QString ReplaceDlg::GetInfoSetDescDst() const
{
    return m_pLineEditInfoSetDescDst->text().trimmed();
}

void ReplaceDlg::SlotCheckBoxToggled(bool bToggled)
{
    Q_UNUSED(bToggled);

    if(m_pCheckBoxBayObjectName->isChecked())
    {
        m_pLineEditBayObjectNameSrc->setEnabled(true);
        m_pLineEditBayObjectNameDst->setEnabled(true);
    }
    else
    {
        m_pLineEditBayObjectNameSrc->clear();
        m_pLineEditBayObjectNameSrc->setEnabled(false);

        m_pLineEditBayObjectNameDst->clear();
        m_pLineEditBayObjectNameDst->setEnabled(false);
    }

    if(m_pCheckBoxBayObjectDesc->isChecked())
    {
        m_pLineEditBayObjectDescSrc->setEnabled(true);
        m_pLineEditBayObjectDescDst->setEnabled(true);
    }
    else
    {
        m_pLineEditBayObjectDescSrc->clear();
        m_pLineEditBayObjectDescSrc->setEnabled(false);

        m_pLineEditBayObjectDescDst->clear();
        m_pLineEditBayObjectDescDst->setEnabled(false);
    }

    if(m_pCheckBoxCubicleObjectName->isChecked())
    {
        m_pLineEditCubicleObjectNameSrc->setEnabled(true);
        m_pLineEditCubicleObjectNameDst->setEnabled(true);
    }
    else
    {
        m_pLineEditCubicleObjectNameSrc->clear();
        m_pLineEditCubicleObjectNameSrc->setEnabled(false);

        m_pLineEditCubicleObjectNameDst->clear();
        m_pLineEditCubicleObjectNameDst->setEnabled(false);
    }

    if(m_pCheckBoxCubicleObjectDesc->isChecked())
    {
        m_pLineEditCubicleObjectDescSrc->setEnabled(true);
        m_pLineEditCubicleObjectDescDst->setEnabled(true);
    }
    else
    {
        m_pLineEditCubicleObjectDescSrc->clear();
        m_pLineEditCubicleObjectDescSrc->setEnabled(false);

        m_pLineEditCubicleObjectDescDst->clear();
        m_pLineEditCubicleObjectDescDst->setEnabled(false);
    }

    if(m_pCheckBoxVTerminalDesc->isChecked())
    {
        m_pLineEditVTerminalDescSrc->setEnabled(true);
        m_pLineEditVTerminalDescDst->setEnabled(true);
    }
    else
    {
        m_pLineEditVTerminalDescSrc->clear();
        m_pLineEditVTerminalDescSrc->setEnabled(false);

        m_pLineEditVTerminalDescDst->clear();
        m_pLineEditVTerminalDescDst->setEnabled(false);
    }

    if(m_pCheckBoxInfoSetDesc->isChecked())
    {
        m_pLineEditInfoSetDescSrc->setEnabled(true);
        m_pLineEditInfoSetDescDst->setEnabled(true);
    }
    else
    {
        m_pLineEditInfoSetDescSrc->clear();
        m_pLineEditInfoSetDescSrc->setEnabled(false);

        m_pLineEditInfoSetDescDst->clear();
        m_pLineEditInfoSetDescDst->setEnabled(false);
    }
}

void ReplaceDlg::SlotPushButtonReplaceClicked()
{
    if(m_pCheckBoxBayObjectName->isChecked())
    {
        const QString strBayObjectNameSrc = m_pLineEditBayObjectNameSrc->text().trimmed();
        const QString strBayObjectNameDst = m_pLineEditBayObjectNameDst->text().trimmed();

        if(strBayObjectNameSrc.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 of %2 can NOT be empty, please input again.").arg(m_pLineEditBayObjectNameSrc->placeholderText()).arg(m_pCheckBoxBayObjectName->text()));
            return;
        }

        if(strBayObjectNameDst.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 of %2 can NOT be empty, please input again.").arg(m_pLineEditBayObjectNameDst->placeholderText()).arg(m_pCheckBoxBayObjectName->text()));
            return;
        }

        if(strBayObjectNameSrc == strBayObjectNameDst)
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 and %2 of %3 can NOT be the same, please input again.").arg(m_pLineEditBayObjectNameSrc->placeholderText()).arg(m_pLineEditBayObjectNameDst->placeholderText()).arg(m_pCheckBoxBayObjectName->text()));
            return;
        }
    }

    if(m_pCheckBoxBayObjectDesc->isChecked())
    {
        const QString strBayObjectDescSrc = m_pLineEditBayObjectDescSrc->text().trimmed();
        const QString strBayObjectDescDst = m_pLineEditBayObjectDescDst->text().trimmed();

        if(strBayObjectDescSrc.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 of %2 can NOT be empty, please input again.").arg(m_pLineEditBayObjectDescSrc->placeholderText()).arg(m_pCheckBoxBayObjectDesc->text()));
            return;
        }

        if(strBayObjectDescDst.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 of %2 can NOT be empty, please input again.").arg(m_pLineEditBayObjectDescDst->placeholderText()).arg(m_pCheckBoxBayObjectDesc->text()));
            return;
        }

        if(strBayObjectDescSrc == strBayObjectDescDst)
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 and %2 of %3 can NOT be the same, please input again.").arg(m_pLineEditBayObjectDescSrc->placeholderText()).arg(m_pLineEditBayObjectDescDst->placeholderText()).arg(m_pCheckBoxBayObjectDesc->text()));
            return;
        }
    }

    if(m_pCheckBoxCubicleObjectName->isChecked())
    {
        const QString strCubicleObjectNameSrc = m_pLineEditCubicleObjectNameSrc->text().trimmed();
        const QString strCubicleObjectNameDst = m_pLineEditCubicleObjectNameDst->text().trimmed();

        if(strCubicleObjectNameSrc.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 of %2 can NOT be empty, please input again.").arg(m_pLineEditCubicleObjectNameSrc->placeholderText()).arg(m_pCheckBoxCubicleObjectName->text()));
            return;
        }

        if(strCubicleObjectNameDst.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 of %2 can NOT be empty, please input again.").arg(m_pLineEditCubicleObjectNameDst->placeholderText()).arg(m_pCheckBoxCubicleObjectName->text()));
            return;
        }

        if(strCubicleObjectNameSrc == strCubicleObjectNameDst)
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 and %2 of %3 can NOT be the same, please input again.").arg(m_pLineEditCubicleObjectNameSrc->placeholderText()).arg(m_pLineEditCubicleObjectNameDst->placeholderText()).arg(m_pCheckBoxCubicleObjectName->text()));
            return;
        }
    }

    if(m_pCheckBoxCubicleObjectDesc->isChecked())
    {
        const QString strCubicleObjectDescSrc = m_pLineEditCubicleObjectDescSrc->text().trimmed();
        const QString strCubicleObjectDescDst = m_pLineEditCubicleObjectDescDst->text().trimmed();

        if(strCubicleObjectDescSrc.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 of %2 can NOT be empty, please input again.").arg(m_pLineEditCubicleObjectDescSrc->placeholderText()).arg(m_pCheckBoxCubicleObjectDesc->text()));
            return;
        }

        if(strCubicleObjectDescDst.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 of %2 can NOT be empty, please input again.").arg(m_pLineEditCubicleObjectDescDst->placeholderText()).arg(m_pCheckBoxCubicleObjectDesc->text()));
            return;
        }

        if(strCubicleObjectDescSrc == strCubicleObjectDescDst)
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 and %2 of %3 can NOT be the same, please input again.").arg(m_pLineEditCubicleObjectDescSrc->placeholderText()).arg(m_pLineEditCubicleObjectDescDst->placeholderText()).arg(m_pCheckBoxCubicleObjectDesc->text()));
            return;
        }
    }

    if(m_pCheckBoxVTerminalDesc->isChecked())
    {
        const QString strVTerminalDescSrc = m_pLineEditVTerminalDescSrc->text().trimmed();
        const QString strVTerminalDescDst = m_pLineEditVTerminalDescDst->text().trimmed();

        if(strVTerminalDescSrc.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 of %2 can NOT be empty, please input again.").arg(m_pLineEditVTerminalDescSrc->placeholderText()).arg(m_pCheckBoxVTerminalDesc->text()));
            return;
        }

        if(strVTerminalDescDst.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 of %2 can NOT be empty, please input again.").arg(m_pLineEditVTerminalDescDst->placeholderText()).arg(m_pCheckBoxVTerminalDesc->text()));
            return;
        }

        if(strVTerminalDescSrc == strVTerminalDescDst)
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 and %2 of %3 can NOT be the same, please input again.").arg(m_pLineEditVTerminalDescSrc->placeholderText()).arg(m_pLineEditVTerminalDescDst->placeholderText()).arg(m_pCheckBoxVTerminalDesc->text()));
            return;
        }
    }

    if(m_pCheckBoxInfoSetDesc->isChecked())
    {
        const QString strInfoSetDescSrc = m_pLineEditInfoSetDescSrc->text().trimmed();
        const QString strInfoSetDescDst = m_pLineEditInfoSetDescDst->text().trimmed();

        if(strInfoSetDescSrc.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 of %2 can NOT be empty, please input again.").arg(m_pLineEditInfoSetDescSrc->placeholderText()).arg(m_pCheckBoxInfoSetDesc->text()));
            return;
        }

        if(strInfoSetDescDst.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 of %2 can NOT be empty, please input again.").arg(m_pLineEditInfoSetDescDst->placeholderText()).arg(m_pCheckBoxInfoSetDesc->text()));
            return;
        }

        if(strInfoSetDescSrc == strInfoSetDescDst)
        {
            QMessageBox::critical(this, tr("Error"), tr("%1 and %2 of %3 can NOT be the same, please input again.").arg(m_pLineEditInfoSetDescSrc->placeholderText()).arg(m_pLineEditInfoSetDescDst->placeholderText()).arg(m_pCheckBoxInfoSetDesc->text()));
            return;
        }
    }

    emit sigReplace();
}

///////////////////////////////////////////////////////////////////////
// CopyBayDlg member functions
///////////////////////////////////////////////////////////////////////
CopyBayDlg::CopyBayDlg(QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Copy Bay"));

    m_pComboBoxSrcProject = new QComboBox(this);
    m_pComboBoxSrcProject->setEditable(false);
    m_pComboBoxSrcProject->setMinimumWidth(300);
    m_pComboBoxDstProject = new QComboBox(this);
    m_pComboBoxDstProject->setEditable(false);
    m_pComboBoxDstProject->setMinimumWidth(300);

    foreach(ProjectExplorer::PeProject *pProject, ProjectExplorer::ProjectManager::Instance()->GetAllProjects())
    {
        foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, pProject->GetAllProjectVersions())
        {
            if(pProjectVersion->IsOpend())
            {
                m_pComboBoxSrcProject->addItem(pProjectVersion->GetDisplayIcon(), pProjectVersion->GetDisplayName(), reinterpret_cast<int>(pProjectVersion));
                m_pComboBoxDstProject->addItem(pProjectVersion->GetDisplayIcon(), pProjectVersion->GetDisplayName(), reinterpret_cast<int>(pProjectVersion));
            }
        }
    }

    QGroupBox *pGroupBoxProject = new QGroupBox(tr("Select Project"), this);
    QHBoxLayout *pHBoxLayoutProject = new QHBoxLayout(pGroupBoxProject);
    pHBoxLayoutProject->addWidget(new QLabel(tr("Source Project:"), this));
    pHBoxLayoutProject->addWidget(m_pComboBoxSrcProject, 1);
    pHBoxLayoutProject->addSpacing(50);
    pHBoxLayoutProject->addWidget(new QLabel(tr("Destination Project:"), this));
    pHBoxLayoutProject->addWidget(m_pComboBoxDstProject, 1);

    m_pModel = new CopyBayModel(this);
    m_pView = new CopyBayView(this);
    m_pView->SetModel(m_pModel);

    SetupIntInfoSet();
    SetupIntVTerminalConn();
    SetupExtInfoSet();
    SetupExtVTerminalConn();

    QTabBar *pTabBar = new QTabBar(this);
    pTabBar->setDrawBase(false);
    pTabBar->setExpanding(false);
    pTabBar->addTab(tr("Internal Infomation Logic"));
    pTabBar->addTab(tr("Internal Virtual Connection"));
    pTabBar->addTab(tr("External Infomation Logic"));
    pTabBar->addTab(tr("External Virtual Connection"));

    QStackedWidget *pStackedWidget = new QStackedWidget(this);
    pStackedWidget->addWidget(m_pViewIntInfoSet);
    pStackedWidget->addWidget(m_pViewIntVTerminalConn);
    pStackedWidget->addWidget(m_pViewExtInfoSet);
    pStackedWidget->addWidget(m_pViewExtVTerminalConn);

    connect(pTabBar, SIGNAL(currentChanged(int)), pStackedWidget, SLOT(setCurrentIndex(int)));

    m_pReplaceDlg = new ReplaceDlg(this);
    m_pReplaceDlg->hide();
    connect(m_pReplaceDlg, SIGNAL(sigReplace()), this, SLOT(SlotReplace()));

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    QPushButton *pPushButtonReplace = pDialogButtonBox->addButton(tr("Replace"), QDialogButtonBox::ActionRole);
    connect(pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(pPushButtonReplace, SIGNAL(clicked()), m_pReplaceDlg, SLOT(show()));

    QVBoxLayout *pVBoxLayout = GetClientLayout();
    pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    pVBoxLayout->addWidget(pGroupBoxProject);
    pVBoxLayout->addSpacing(10);
    pVBoxLayout->addWidget(m_pView);
    pVBoxLayout->addSpacing(10);
    pVBoxLayout->addWidget(pTabBar);
    pVBoxLayout->addWidget(pStackedWidget);
    pVBoxLayout->addSpacing(15);
    pVBoxLayout->addWidget(pDialogButtonBox);

    connect(m_pComboBoxSrcProject, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotSrcProjectChanged(int)));
    connect(m_pComboBoxDstProject, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotDstProjectChanged(int)));
    connect(m_pView, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotBayObjectDoubleClicked(const QModelIndex&)));
    connect(m_pModel, SIGNAL(sigItemCheckStateChanged()),
            this, SLOT(SlotUpdateNewData()));
    connect(m_pViewExtInfoSet, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotExgInfoSetDoubleClicked(const QModelIndex&)));
    connect(m_pViewExtVTerminalConn, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(SlotExtVTerminalConnDoubleClicked(const QModelIndex&)));
    connect(m_pModelIntInfoSet, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(SlotInfoSetChanged(QStandardItem*)));
    connect(m_pModelExtInfoSet, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(SlotInfoSetChanged(QStandardItem*)));
    connect(m_pModelIntVTerminalConn, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(SlotVTerminalConnChanged(QStandardItem*)));
    connect(m_pModelExtVTerminalConn, SIGNAL(itemChanged(QStandardItem*)),
            this, SLOT(SlotVTerminalConnChanged(QStandardItem*)));

    SlotSrcProjectChanged(m_pComboBoxSrcProject->currentIndex());    
}

CopyBayDlg::~CopyBayDlg()
{
    ClearModel();
}

QSize CopyBayDlg::sizeHint() const
{
    return QSize(1050, 600);
}

void CopyBayDlg::SetupIntInfoSet()
{
    m_pModelIntInfoSet = new QStandardItemModel(0, 9);
    m_pModelIntInfoSet->setHorizontalHeaderLabels(QStringList() << "" << tr("Name") << tr("Type") << tr("Send IED")
                                                                << tr("First Level Switch") << tr("Second Level Switch")
                                                                << tr("Third Level Switch") << tr("Fourth Level Switch") << tr("Receive IED"));
    m_pViewIntInfoSet = new Utils::ReadOnlyTableView(this);
    m_pViewIntInfoSet->setMinimumHeight(250);
    m_pViewIntInfoSet->setAlternatingRowColors(true);
    m_pViewIntInfoSet->setSortingEnabled(true);
    m_pViewIntInfoSet->setModel(m_pModelIntInfoSet);
    m_pViewIntInfoSet->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewIntInfoSet->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pViewIntInfoSet->horizontalHeader()->setHighlightSections(false);
    m_pViewIntInfoSet->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pViewIntInfoSet->horizontalHeader()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
    m_pViewIntInfoSet->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
#else
    m_pViewIntInfoSet->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
#endif
    m_pViewIntInfoSet->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewIntInfoSet->verticalHeader()->hide();
    m_pViewIntInfoSet->setShowGrid(false);
    m_pViewIntInfoSet->setColumnWidth(1, 200);
    m_pViewIntInfoSet->setColumnWidth(2, 60);
    m_pViewIntInfoSet->setColumnWidth(3, 200);
    m_pViewIntInfoSet->setColumnWidth(4, 200);
    m_pViewIntInfoSet->setColumnWidth(5, 200);
    m_pViewIntInfoSet->setColumnWidth(6, 200);
    m_pViewIntInfoSet->setColumnWidth(7, 200);
    m_pViewIntInfoSet->setColumnWidth(8, 200);

    QAction *pActionSelectAll = new QAction(tr("Select All"), this);
    pActionSelectAll->setData(reinterpret_cast<int>(m_pModelIntInfoSet));
    connect(pActionSelectAll, SIGNAL(triggered()), this, SLOT(SlotActionSelectAll()));

    QAction *pActionDeselectAll = new QAction(tr("Deselect All"), this);
    pActionDeselectAll->setData(reinterpret_cast<int>(m_pModelIntInfoSet));
    connect(pActionDeselectAll, SIGNAL(triggered()), this, SLOT(SlotActionDeselectAll()));

    m_pViewIntInfoSet->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewIntInfoSet->addAction(pActionSelectAll);
    m_pViewIntInfoSet->addAction(pActionDeselectAll);
}

void CopyBayDlg::SetupIntVTerminalConn()
{
    m_pModelIntVTerminalConn = new QStandardItemModel(0, 8);
    m_pModelIntVTerminalConn->setHorizontalHeaderLabels(QStringList() << "" << tr("Current IED") << tr("Current Signal") << tr("Current Strap")
                                                                      << tr("Direction") << tr("Side IED") << tr("Side Strap") << tr("Side Signal"));
    m_pViewIntVTerminalConn = new Utils::ReadOnlyTableView(this);
    m_pViewIntVTerminalConn->setMinimumHeight(250);
    m_pViewIntVTerminalConn->setAlternatingRowColors(true);
    m_pViewIntVTerminalConn->setSortingEnabled(true);
    m_pViewIntVTerminalConn->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewIntVTerminalConn->setModel(m_pModelIntVTerminalConn);
    m_pViewIntVTerminalConn->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewIntVTerminalConn->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pViewIntVTerminalConn->horizontalHeader()->setHighlightSections(false);
    m_pViewIntVTerminalConn->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pViewIntVTerminalConn->horizontalHeader()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
    m_pViewIntVTerminalConn->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
#else
    m_pViewIntVTerminalConn->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
#endif
    m_pViewIntVTerminalConn->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewIntVTerminalConn->verticalHeader()->hide();
    m_pViewIntVTerminalConn->setShowGrid(false);
    m_pViewIntVTerminalConn->setColumnWidth(1, 200);
    m_pViewIntVTerminalConn->setColumnWidth(2, 250);
    m_pViewIntVTerminalConn->setColumnWidth(3, 100);
    m_pViewIntVTerminalConn->setColumnWidth(4, 80);
    m_pViewIntVTerminalConn->setColumnWidth(5, 200);
    m_pViewIntVTerminalConn->setColumnWidth(6, 100);
    m_pViewIntVTerminalConn->setColumnWidth(7, 250);

    QAction *pActionSelectAll = new QAction(tr("Select All"), this);
    pActionSelectAll->setData(reinterpret_cast<int>(m_pModelIntVTerminalConn));
    connect(pActionSelectAll, SIGNAL(triggered()), this, SLOT(SlotActionSelectAll()));

    QAction *pActionDeselectAll = new QAction(tr("Deselect All"), this);
    pActionDeselectAll->setData(reinterpret_cast<int>(m_pModelIntVTerminalConn));
    connect(pActionDeselectAll, SIGNAL(triggered()), this, SLOT(SlotActionDeselectAll()));

    m_pViewIntVTerminalConn->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewIntVTerminalConn->addAction(pActionSelectAll);
    m_pViewIntVTerminalConn->addAction(pActionDeselectAll);
}

void CopyBayDlg::SetupExtInfoSet()
{
    m_pModelExtInfoSet = new QStandardItemModel(0, 9);
    m_pModelExtInfoSet->setHorizontalHeaderLabels(QStringList() << "" << tr("Name") << tr("Type") << tr("Send IED")
                                                                << tr("First Level Switch") << tr("Second Level Switch")
                                                                << tr("Third Level Switch") << tr("Fourth Level Switch") << tr("Receive IED"));
    m_pViewExtInfoSet = new Utils::ReadOnlyTableView(this);
    m_pViewExtInfoSet->setMinimumHeight(250);
    m_pViewExtInfoSet->setAlternatingRowColors(true);
    m_pViewExtInfoSet->setSortingEnabled(true);
    m_pViewExtInfoSet->setModel(m_pModelExtInfoSet);
    m_pViewExtInfoSet->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewExtInfoSet->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pViewExtInfoSet->horizontalHeader()->setHighlightSections(false);
    m_pViewExtInfoSet->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pViewExtInfoSet->horizontalHeader()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
    m_pViewExtInfoSet->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
#else
    m_pViewExtInfoSet->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
#endif
    m_pViewExtInfoSet->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewExtInfoSet->verticalHeader()->hide();
    m_pViewExtInfoSet->setShowGrid(false);
    m_pViewExtInfoSet->setColumnWidth(1, 200);
    m_pViewExtInfoSet->setColumnWidth(2, 60);
    m_pViewExtInfoSet->setColumnWidth(3, 200);
    m_pViewExtInfoSet->setColumnWidth(4, 200);
    m_pViewExtInfoSet->setColumnWidth(5, 200);
    m_pViewExtInfoSet->setColumnWidth(6, 200);
    m_pViewExtInfoSet->setColumnWidth(7, 200);
    m_pViewExtInfoSet->setColumnWidth(8, 200);

    QAction *pActionSelectAll = new QAction(tr("Select All"), this);
    pActionSelectAll->setData(reinterpret_cast<int>(m_pModelExtInfoSet));
    connect(pActionSelectAll, SIGNAL(triggered()), this, SLOT(SlotActionSelectAll()));

    QAction *pActionDeselectAll = new QAction(tr("Deselect All"), this);
    pActionDeselectAll->setData(reinterpret_cast<int>(m_pModelExtInfoSet));
    connect(pActionDeselectAll, SIGNAL(triggered()), this, SLOT(SlotActionDeselectAll()));

    m_pViewExtInfoSet->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewExtInfoSet->addAction(pActionSelectAll);
    m_pViewExtInfoSet->addAction(pActionDeselectAll);
}

void CopyBayDlg::SetupExtVTerminalConn()
{
    m_pModelExtVTerminalConn = new QStandardItemModel(0, 8);
    m_pModelExtVTerminalConn->setHorizontalHeaderLabels(QStringList() << "" << tr("Current IED") << tr("Current Signal") << tr("Current Strap")
                                                                      << tr("Direction") << tr("Side IED") << tr("Side Strap") << tr("Side Signal"));
    m_pViewExtVTerminalConn = new Utils::ReadOnlyTableView(this);
    m_pViewExtVTerminalConn->setMinimumHeight(250);
    m_pViewExtVTerminalConn->setAlternatingRowColors(true);
    m_pViewExtVTerminalConn->setSortingEnabled(true);
    m_pViewExtVTerminalConn->setModel(m_pModelExtVTerminalConn);
    m_pViewExtVTerminalConn->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pViewExtVTerminalConn->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pViewExtVTerminalConn->horizontalHeader()->setHighlightSections(false);
    m_pViewExtVTerminalConn->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pViewExtVTerminalConn->horizontalHeader()->setStretchLastSection(true);
#if QT_VERSION >= 0x050000
    m_pViewExtVTerminalConn->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
#else
    m_pViewExtVTerminalConn->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
#endif
    m_pViewExtVTerminalConn->verticalHeader()->setDefaultSectionSize(qApp->fontMetrics().height() + 6);
    m_pViewExtVTerminalConn->verticalHeader()->hide();
    m_pViewExtVTerminalConn->setShowGrid(false);
    m_pViewExtVTerminalConn->setColumnWidth(1, 200);
    m_pViewExtVTerminalConn->setColumnWidth(2, 250);
    m_pViewExtVTerminalConn->setColumnWidth(3, 100);
    m_pViewExtVTerminalConn->setColumnWidth(4, 80);
    m_pViewExtVTerminalConn->setColumnWidth(5, 200);
    m_pViewExtVTerminalConn->setColumnWidth(6, 100);
    m_pViewExtVTerminalConn->setColumnWidth(7, 250);

    QAction *pActionSelectAll = new QAction(tr("Select All"), this);
    pActionSelectAll->setData(reinterpret_cast<int>(m_pModelExtVTerminalConn));
    connect(pActionSelectAll, SIGNAL(triggered()), this, SLOT(SlotActionSelectAll()));

    QAction *pActionDeselectAll = new QAction(tr("Deselect All"), this);
    pActionDeselectAll->setData(reinterpret_cast<int>(m_pModelExtVTerminalConn));
    connect(pActionDeselectAll, SIGNAL(triggered()), this, SLOT(SlotActionDeselectAll()));

    m_pViewExtVTerminalConn->setContextMenuPolicy(Qt::ActionsContextMenu);
    m_pViewExtVTerminalConn->addAction(pActionSelectAll);
    m_pViewExtVTerminalConn->addAction(pActionDeselectAll);
}

void CopyBayDlg::ClearModel()
{
    m_pModelIntInfoSet->removeRows(0, m_pModelIntInfoSet->rowCount());
    m_pModelIntVTerminalConn->removeRows(0, m_pModelIntVTerminalConn->rowCount());
    m_pModelExtInfoSet->removeRows(0, m_pModelExtInfoSet->rowCount());
    m_pModelExtVTerminalConn->removeRows(0, m_pModelExtVTerminalConn->rowCount());

    m_mapIntInfoSetToItem.clear();
    m_mapExtInfoSetToItem.clear();
    m_mapIntVTerminalConnToItem.clear();
    m_mapExtVTerminalConnToItem.clear();
	
	qDeleteAll(m_lstNewInfoSets);
    m_lstNewInfoSets.clear();

    qDeleteAll(m_lstNewVTerminalConns);
    m_lstNewVTerminalConns.clear();
}

void CopyBayDlg::AddIntInfoSetItem(ProjectExplorer::PeInfoSet *pInfoSet)
{
    if(!pInfoSet)
        return;

    ProjectExplorer::PeDevice *pTxIED = pInfoSet->GetTxIED();
    ProjectExplorer::PeDevice *pRxIED = pInfoSet->GetRxIED();
    ProjectExplorer::PeDevice *pSwitch1 = pInfoSet->GetSwitch1();
    ProjectExplorer::PeDevice *pSwitch2 = pInfoSet->GetSwitch2();
    ProjectExplorer::PeDevice *pSwitch3 = pInfoSet->GetSwitch3();
    ProjectExplorer::PeDevice *pSwitch4 = pInfoSet->GetSwitch4();

    QList<QStandardItem*> lstItems;

    QStandardItem *pItem = new QStandardItem;
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    pItem->setCheckable(true);
    pItem->setCheckState(Qt::Checked);
    lstItems.append(pItem);

    pItem = new QStandardItem(pInfoSet->GetDisplayIcon(), pInfoSet->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    pItem = new QStandardItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(pInfoSet->GetInfoSetType()));
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    QString strTxIED = "";
    if(pTxIED)
    {
        strTxIED = pTxIED->GetDisplayName();
        if(ProjectExplorer::PePort *pTxPort = pInfoSet->GetTxPort())
            strTxIED = QString("%1<%2:%3>").arg(strTxIED).arg(pTxPort->GetParentBoard()->GetDisplayName()).arg(pTxPort->GetDisplayName());
    }
    pItem = new QStandardItem(strTxIED);
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    QString strSwitch1 = "";
    if(pSwitch1)
    {
        strSwitch1 = pSwitch1->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch1TxPort = pInfoSet->GetSwitch1TxPort())
            strSwitch1 = QString("%1<%2>").arg(strSwitch1).arg(pSwitch1TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch1RxPort = pInfoSet->GetSwitch1RxPort())
            strSwitch1 = QString("<%1>%2").arg(pSwitch1RxPort->GetDisplayName()).arg(strSwitch1);
    }
    pItem = new QStandardItem(strSwitch1);
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    QString strSwitch2 = "";
    if(pSwitch2)
    {
        strSwitch2 = pSwitch2->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch2TxPort = pInfoSet->GetSwitch2TxPort())
            strSwitch2 = QString("%1<%2>").arg(strSwitch2).arg(pSwitch2TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch2RxPort = pInfoSet->GetSwitch2RxPort())
            strSwitch2 = QString("<%1>%2").arg(pSwitch2RxPort->GetDisplayName()).arg(strSwitch2);
    }
    pItem = new QStandardItem(strSwitch2);
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    QString strSwitch3 = "";
    if(pSwitch3)
    {
        strSwitch3 = pSwitch3->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch3TxPort = pInfoSet->GetSwitch3TxPort())
            strSwitch3 = QString("%1<%2>").arg(strSwitch3).arg(pSwitch3TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch3RxPort = pInfoSet->GetSwitch3RxPort())
            strSwitch3 = QString("<%1>%2").arg(pSwitch3RxPort->GetDisplayName()).arg(strSwitch3);
    }
    pItem = new QStandardItem(strSwitch3);
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    QString strSwitch4 = "";
    if(pSwitch4)
    {
        strSwitch4 = pSwitch4->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch4TxPort = pInfoSet->GetSwitch4TxPort())
            strSwitch4 = QString("%1<%2>").arg(strSwitch4).arg(pSwitch4TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch4RxPort = pInfoSet->GetSwitch4RxPort())
            strSwitch4 = QString("<%1>%2").arg(pSwitch4RxPort->GetDisplayName()).arg(strSwitch4);
    }
    pItem = new QStandardItem(strSwitch4);
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    QString strRxIED = "";
    if(pRxIED)
    {
        strRxIED = pRxIED->GetDisplayName();
        if(ProjectExplorer::PePort *pRxPort = pInfoSet->GetRxPort())
            strRxIED = QString("<%1:%2>%3").arg(pRxPort->GetParentBoard()->GetDisplayName()).arg(pRxPort->GetDisplayName()).arg(strRxIED);
    }
    pItem = new QStandardItem(strRxIED);
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    m_pModelIntInfoSet->appendRow(lstItems);
    m_mapIntInfoSetToItem.insert(pInfoSet, lstItems.first());
}

void CopyBayDlg::AddIntVTerminalConnItem(ProjectExplorer::PeVTerminalConn *pVTerminalConn)
{
    if(!pVTerminalConn)
        return;

    ProjectExplorer::PeVTerminal *pTxVTerminal = pVTerminalConn->GetTxVTerminal();
    ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();
    if(!pTxVTerminal || !pRxVTerminal)
        return;

    ProjectExplorer::PeDevice *pTxDevice = pTxVTerminal->GetParentDevice();
    ProjectExplorer::PeDevice *pRxDevice = pRxVTerminal->GetParentDevice();
    if(!pTxDevice || !pRxDevice)
        return;

    ProjectExplorer::PeDevice *pCurrentDevice = 0, *pSideDevice = 0;
    ProjectExplorer::PeVTerminal *pCurrentVTerminal = 0, *pSideVTerminal = 0;
    ProjectExplorer::PeStrap *pCurrentStrap = 0, *pSideStrap = 0;
    QString strDirection;
    if(pTxDevice->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
    {
        pCurrentDevice = pTxDevice;
        pSideDevice = pRxDevice;

        pCurrentVTerminal = pTxVTerminal;
        pSideVTerminal = pRxVTerminal;

        pCurrentStrap = pVTerminalConn->GetTxStrap();
        pSideStrap = pVTerminalConn->GetRxStrap();

        strDirection = ">>>";
    }
    else
    {
        pCurrentDevice = pRxDevice;
        pSideDevice = pTxDevice;

        pCurrentVTerminal = pRxVTerminal;
        pSideVTerminal = pTxVTerminal;

        pCurrentStrap = pVTerminalConn->GetRxStrap();
        pSideStrap = pVTerminalConn->GetTxStrap();

        strDirection = "<<<";
    }

    QList<QStandardItem*> lstItems;

    QStandardItem *pItem = new QStandardItem;
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    pItem->setCheckable(true);
    pItem->setCheckState(Qt::Checked);
    lstItems.append(pItem);

    pItem = new QStandardItem(ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otVTerminalConn), pCurrentDevice->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    pItem = new QStandardItem(pCurrentVTerminal->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    pItem = new QStandardItem(pCurrentStrap ? pCurrentStrap->GetDisplayName() : "");
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    pItem = new QStandardItem(strDirection);
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    pItem = new QStandardItem(pSideDevice->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    pItem = new QStandardItem(pSideStrap ? pSideStrap->GetDisplayName() : "");
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    pItem = new QStandardItem(pSideVTerminal->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    m_pModelIntVTerminalConn->appendRow(lstItems);
    m_mapIntVTerminalConnToItem.insert(pVTerminalConn, lstItems.first());
}

void CopyBayDlg::AddExtInfoSetItem(ProjectExplorer::PeInfoSet *pInfoSet)
{
    if(!pInfoSet)
        return;

    ProjectExplorer::PeDevice *pTxIED = pInfoSet->GetTxIED();
    ProjectExplorer::PeDevice *pRxIED = pInfoSet->GetRxIED();
    ProjectExplorer::PeDevice *pSwitch1 = pInfoSet->GetSwitch1();
    ProjectExplorer::PeDevice *pSwitch2 = pInfoSet->GetSwitch2();
    ProjectExplorer::PeDevice *pSwitch3 = pInfoSet->GetSwitch3();
    ProjectExplorer::PeDevice *pSwitch4 = pInfoSet->GetSwitch4();

    QList<QStandardItem*> lstItems;

    QStandardItem *pItem = new QStandardItem;
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    pItem->setCheckable(true);
    pItem->setCheckState(Qt::Checked);
    lstItems.append(pItem);

    pItem = new QStandardItem(pInfoSet->GetDisplayIcon(), pInfoSet->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    pItem = new QStandardItem(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(pInfoSet->GetInfoSetType()));
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    QString strTxIED = "";
    if(pTxIED)
    {
        strTxIED = pTxIED->GetDisplayName();
        if(ProjectExplorer::PePort *pTxPort = pInfoSet->GetTxPort())
            strTxIED = QString("%1<%2:%3>").arg(strTxIED).arg(pTxPort->GetParentBoard()->GetDisplayName()).arg(pTxPort->GetDisplayName());
    }
    pItem = new QStandardItem(strTxIED);
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    QString strSwitch1 = "";
    if(pSwitch1)
    {
        strSwitch1 = pSwitch1->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch1TxPort = pInfoSet->GetSwitch1TxPort())
            strSwitch1 = QString("%1<%2>").arg(strSwitch1).arg(pSwitch1TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch1RxPort = pInfoSet->GetSwitch1RxPort())
            strSwitch1 = QString("<%1>%2").arg(pSwitch1RxPort->GetDisplayName()).arg(strSwitch1);
    }
    pItem = new QStandardItem(strSwitch1);
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    QString strSwitch2 = "";
    if(pSwitch2)
    {
        strSwitch2 = pSwitch2->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch2TxPort = pInfoSet->GetSwitch2TxPort())
            strSwitch2 = QString("%1<%2>").arg(strSwitch2).arg(pSwitch2TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch2RxPort = pInfoSet->GetSwitch2RxPort())
            strSwitch2 = QString("<%1>%2").arg(pSwitch2RxPort->GetDisplayName()).arg(strSwitch2);
    }
    pItem = new QStandardItem(strSwitch2);
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    QString strSwitch3 = "";
    if(pSwitch3)
    {
        strSwitch3 = pSwitch3->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch3TxPort = pInfoSet->GetSwitch3TxPort())
            strSwitch3 = QString("%1<%2>").arg(strSwitch3).arg(pSwitch3TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch3RxPort = pInfoSet->GetSwitch3RxPort())
            strSwitch3 = QString("<%1>%2").arg(pSwitch3RxPort->GetDisplayName()).arg(strSwitch3);
    }
    pItem = new QStandardItem(strSwitch3);
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    QString strSwitch4 = "";
    if(pSwitch4)
    {
        strSwitch4 = pSwitch4->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch4TxPort = pInfoSet->GetSwitch4TxPort())
            strSwitch4 = QString("%1<%2>").arg(strSwitch4).arg(pSwitch4TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch4RxPort = pInfoSet->GetSwitch4RxPort())
            strSwitch4 = QString("<%1>%2").arg(pSwitch4RxPort->GetDisplayName()).arg(strSwitch4);
    }
    pItem = new QStandardItem(strSwitch4);
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    QString strRxIED = "";
    if(pRxIED)
    {
        strRxIED = pRxIED->GetDisplayName();
        if(ProjectExplorer::PePort *pRxPort = pInfoSet->GetRxPort())
            strRxIED = QString("<%1:%2>%3").arg(pRxPort->GetParentBoard()->GetDisplayName()).arg(pRxPort->GetDisplayName()).arg(strRxIED);
    }
    pItem = new QStandardItem(strRxIED);
    pItem->setData(reinterpret_cast<int>(pInfoSet));
    lstItems.append(pItem);

    m_pModelExtInfoSet->appendRow(lstItems);
    m_mapExtInfoSetToItem.insert(pInfoSet, lstItems.first());
}

void CopyBayDlg::AddExtVTerminalConnItem(ProjectExplorer::PeVTerminalConn *pVTerminalConn)
{
    if(!pVTerminalConn)
        return;

    ProjectExplorer::PeVTerminal *pTxVTerminal = pVTerminalConn->GetTxVTerminal();
    ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();
    if(!pTxVTerminal || !pRxVTerminal)
        return;

    ProjectExplorer::PeDevice *pTxDevice = pTxVTerminal->GetParentDevice();
    ProjectExplorer::PeDevice *pRxDevice = pRxVTerminal->GetParentDevice();
    if(!pTxDevice || !pRxDevice)
        return;

    ProjectExplorer::PeDevice *pCurrentDevice = 0, *pSideDevice = 0;
    ProjectExplorer::PeVTerminal *pCurrentVTerminal = 0, *pSideVTerminal = 0;
    ProjectExplorer::PeStrap *pCurrentStrap = 0, *pSideStrap = 0;
    QString strDirection;
    if(pTxDevice->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
    {
        pCurrentDevice = pTxDevice;
        pSideDevice = pRxDevice;

        pCurrentVTerminal = pTxVTerminal;
        pSideVTerminal = pRxVTerminal;

        pCurrentStrap = pVTerminalConn->GetTxStrap();
        pSideStrap = pVTerminalConn->GetRxStrap();

        strDirection = ">>>";
    }
    else
    {
        pCurrentDevice = pRxDevice;
        pSideDevice = pTxDevice;

        pCurrentVTerminal = pRxVTerminal;
        pSideVTerminal = pTxVTerminal;

        pCurrentStrap = pVTerminalConn->GetRxStrap();
        pSideStrap = pVTerminalConn->GetTxStrap();

        strDirection = "<<<";
    }

    QList<QStandardItem*> lstItems;

    QStandardItem *pItem = new QStandardItem;
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    pItem->setCheckable(true);
    pItem->setCheckState(Qt::Checked);
    lstItems.append(pItem);

    pItem = new QStandardItem(ProjectExplorer::PeProjectObject::GetObjectIcon(ProjectExplorer::PeProjectObject::otVTerminalConn), pCurrentDevice->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    pItem = new QStandardItem(pCurrentVTerminal->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    pItem = new QStandardItem(pCurrentStrap ? pCurrentStrap->GetDisplayName() : "");
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    pItem = new QStandardItem(strDirection);
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    pItem = new QStandardItem(pSideDevice->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    pItem = new QStandardItem(pSideStrap ? pSideStrap->GetDisplayName() : "");
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    pItem = new QStandardItem(pSideVTerminal->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
    lstItems.append(pItem);

    m_pModelExtVTerminalConn->appendRow(lstItems);
    m_mapExtVTerminalConnToItem.insert(pVTerminalConn, lstItems.first());
}

void CopyBayDlg::UpdateInternalVTerminalConn(ProjectExplorer::PeVTerminalConn *pVTerminalConn)
{
    if(!pVTerminalConn)
        return;

    int iRow = -1;
    for(int i = 0; i < m_pModelIntVTerminalConn->rowCount(); i++)
    {
        QStandardItem *pItem = m_pModelIntVTerminalConn->item(i, 0);
        if(reinterpret_cast<ProjectExplorer::PeVTerminalConn*>(pItem->data().toInt()) == pVTerminalConn)
        {
            iRow = i;
            break;
        }
    }
    if(iRow < 0)
        return;

    ProjectExplorer::PeVTerminal *pTxVTerminal = pVTerminalConn->GetTxVTerminal();
    ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();
    if(!pTxVTerminal || !pRxVTerminal)
        return;

    ProjectExplorer::PeDevice *pTxDevice = pTxVTerminal->GetParentDevice();
    ProjectExplorer::PeDevice *pRxDevice = pRxVTerminal->GetParentDevice();
    if(!pTxDevice || !pRxDevice)
        return;

    ProjectExplorer::PeDevice *pCurrentDevice = 0, *pSideDevice = 0;
    ProjectExplorer::PeVTerminal *pCurrentVTerminal = 0, *pSideVTerminal = 0;
    ProjectExplorer::PeStrap *pCurrentStrap = 0, *pSideStrap = 0;
    QString strDirection;
    if(pTxDevice->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
    {
        pCurrentDevice = pTxDevice;
        pSideDevice = pRxDevice;

        pCurrentVTerminal = pTxVTerminal;
        pSideVTerminal = pRxVTerminal;

        pCurrentStrap = pVTerminalConn->GetTxStrap();
        pSideStrap = pVTerminalConn->GetRxStrap();

        strDirection = ">>>";
    }
    else
    {
        pCurrentDevice = pRxDevice;
        pSideDevice = pTxDevice;

        pCurrentVTerminal = pRxVTerminal;
        pSideVTerminal = pTxVTerminal;

        pCurrentStrap = pVTerminalConn->GetRxStrap();
        pSideStrap = pVTerminalConn->GetTxStrap();

        strDirection = "<<<";
    }

    QStandardItem *pItem = m_pModelIntVTerminalConn->item(iRow, 1);
    pItem->setText(pCurrentDevice->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));

    pItem = m_pModelIntVTerminalConn->item(iRow, 2);
    pItem->setText(pCurrentVTerminal->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));

    pItem = m_pModelIntVTerminalConn->item(iRow, 3);
    pItem->setText(pCurrentStrap ? pCurrentStrap->GetDisplayName() : "");
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));

    pItem = m_pModelIntVTerminalConn->item(iRow, 4);
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));

    pItem = m_pModelIntVTerminalConn->item(iRow, 5);
    pItem->setText(pSideDevice->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));

    pItem = m_pModelIntVTerminalConn->item(iRow, 6);
    pItem->setText(pSideStrap ? pSideStrap->GetDisplayName() : "");
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));

    pItem = m_pModelIntVTerminalConn->item(iRow, 7);
    pItem->setText(pSideVTerminal->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
}

void CopyBayDlg::UpdateInternalInfoset(ProjectExplorer::PeInfoSet *pInfoSet)
{
    if(!pInfoSet)
        return;

    int iRow = -1;
    for(int i = 0; i < m_pModelIntInfoSet->rowCount(); i++)
    {
        QStandardItem *pItem = m_pModelIntInfoSet->item(i, 0);
        if(reinterpret_cast<ProjectExplorer::PeInfoSet*>(pItem->data().toInt()) == pInfoSet)
        {
            iRow = i;
            break;
        }
    }
    if(iRow < 0)
        return;

    ProjectExplorer::PeDevice *pTxIED = pInfoSet->GetTxIED();
    ProjectExplorer::PeDevice *pRxIED = pInfoSet->GetRxIED();
    ProjectExplorer::PeDevice *pSwitch1 = pInfoSet->GetSwitch1();
    ProjectExplorer::PeDevice *pSwitch2 = pInfoSet->GetSwitch2();
    ProjectExplorer::PeDevice *pSwitch3 = pInfoSet->GetSwitch3();
    ProjectExplorer::PeDevice *pSwitch4 = pInfoSet->GetSwitch4();

    QStandardItem *pItem = m_pModelIntInfoSet->item(iRow, 1);
    pItem->setText(pInfoSet->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pInfoSet));

    pItem = m_pModelIntInfoSet->item(iRow, 2);
    pItem->setText(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(pInfoSet->GetInfoSetType()));
    pItem->setData(reinterpret_cast<int>(pInfoSet));

    QString strTxIED = "";
    if(pTxIED)
    {
        strTxIED = pTxIED->GetDisplayName();
        if(ProjectExplorer::PePort *pTxPort = pInfoSet->GetTxPort())
            strTxIED = QString("%1<%2:%3>").arg(strTxIED).arg(pTxPort->GetParentBoard()->GetDisplayName()).arg(pTxPort->GetDisplayName());
    }
    pItem = m_pModelIntInfoSet->item(iRow, 3);
    pItem->setText(strTxIED);
    pItem->setData(reinterpret_cast<int>(pInfoSet));

    QString strSwitch1 = "";
    if(pSwitch1)
    {
        strSwitch1 = pSwitch1->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch1TxPort = pInfoSet->GetSwitch1TxPort())
            strSwitch1 = QString("%1<%2>").arg(strSwitch1).arg(pSwitch1TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch1RxPort = pInfoSet->GetSwitch1RxPort())
            strSwitch1 = QString("<%1>%2").arg(pSwitch1RxPort->GetDisplayName()).arg(strSwitch1);
    }
    pItem = m_pModelIntInfoSet->item(iRow, 4);
    pItem->setText(strSwitch1);

    QString strSwitch2 = "";
    if(pSwitch2)
    {
        strSwitch2 = pSwitch2->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch2TxPort = pInfoSet->GetSwitch2TxPort())
            strSwitch2 = QString("%1<%2>").arg(strSwitch2).arg(pSwitch2TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch2RxPort = pInfoSet->GetSwitch2RxPort())
            strSwitch2 = QString("<%1>%2").arg(pSwitch2RxPort->GetDisplayName()).arg(strSwitch2);
    }
    pItem = m_pModelIntInfoSet->item(iRow, 5);
    pItem->setText(strSwitch2);

    QString strSwitch3 = "";
    if(pSwitch3)
    {
        strSwitch3 = pSwitch3->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch3TxPort = pInfoSet->GetSwitch3TxPort())
            strSwitch3 = QString("%1<%2>").arg(strSwitch3).arg(pSwitch3TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch3RxPort = pInfoSet->GetSwitch3RxPort())
            strSwitch3 = QString("<%1>%2").arg(pSwitch3RxPort->GetDisplayName()).arg(strSwitch3);
    }
    pItem = m_pModelIntInfoSet->item(iRow, 6);
    pItem->setText(strSwitch3);

    QString strSwitch4 = "";
    if(pSwitch4)
    {
        strSwitch4 = pSwitch4->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch4TxPort = pInfoSet->GetSwitch4TxPort())
            strSwitch4 = QString("%1<%2>").arg(strSwitch4).arg(pSwitch4TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch4RxPort = pInfoSet->GetSwitch4RxPort())
            strSwitch4 = QString("<%1>%2").arg(pSwitch4RxPort->GetDisplayName()).arg(strSwitch4);
    }
    pItem = m_pModelIntInfoSet->item(iRow, 7);
    pItem->setText(strSwitch4);

    QString strRxIED = "";
    if(pRxIED)
    {
        strRxIED = pRxIED->GetDisplayName();
        if(ProjectExplorer::PePort *pRxPort = pInfoSet->GetRxPort())
            strRxIED = QString("<%1:%2>%3").arg(pRxPort->GetParentBoard()->GetDisplayName()).arg(pRxPort->GetDisplayName()).arg(strRxIED);
    }
    pItem = m_pModelIntInfoSet->item(iRow, 8);
    pItem->setText(strRxIED);
}

void CopyBayDlg::UpdateExternalVTerminalConn(ProjectExplorer::PeVTerminalConn *pVTerminalConn)
{
    if(!pVTerminalConn)
        return;

    int iRow = -1;
    for(int i = 0; i < m_pModelExtVTerminalConn->rowCount(); i++)
    {
        QStandardItem *pItem = m_pModelExtVTerminalConn->item(i, 0);
        if(reinterpret_cast<ProjectExplorer::PeVTerminalConn*>(pItem->data().toInt()) == pVTerminalConn)
        {
            iRow = i;
            break;
        }
    }
    if(iRow < 0)
        return;

    ProjectExplorer::PeVTerminal *pTxVTerminal = pVTerminalConn->GetTxVTerminal();
    ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();
    if(!pTxVTerminal || !pRxVTerminal)
        return;

    ProjectExplorer::PeDevice *pTxDevice = pTxVTerminal->GetParentDevice();
    ProjectExplorer::PeDevice *pRxDevice = pRxVTerminal->GetParentDevice();
    if(!pTxDevice || !pRxDevice)
        return;

    ProjectExplorer::PeDevice *pCurrentDevice = 0, *pSideDevice = 0;
    ProjectExplorer::PeVTerminal *pCurrentVTerminal = 0, *pSideVTerminal = 0;
    ProjectExplorer::PeStrap *pCurrentStrap = 0, *pSideStrap = 0;
    QString strDirection;
    if(pTxDevice->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
    {
        pCurrentDevice = pTxDevice;
        pSideDevice = pRxDevice;

        pCurrentVTerminal = pTxVTerminal;
        pSideVTerminal = pRxVTerminal;

        pCurrentStrap = pVTerminalConn->GetTxStrap();
        pSideStrap = pVTerminalConn->GetRxStrap();

        strDirection = ">>>";
    }
    else
    {
        pCurrentDevice = pRxDevice;
        pSideDevice = pTxDevice;

        pCurrentVTerminal = pRxVTerminal;
        pSideVTerminal = pTxVTerminal;

        pCurrentStrap = pVTerminalConn->GetRxStrap();
        pSideStrap = pVTerminalConn->GetTxStrap();

        strDirection = "<<<";
    }

    QStandardItem *pItem = m_pModelExtVTerminalConn->item(iRow, 1);
    pItem->setText(pCurrentDevice->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));

    pItem = m_pModelExtVTerminalConn->item(iRow, 2);
    pItem->setText(pCurrentVTerminal->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));

    pItem = m_pModelExtVTerminalConn->item(iRow, 3);
    pItem->setText(pCurrentStrap ? pCurrentStrap->GetDisplayName() : "");
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));

    pItem = m_pModelExtVTerminalConn->item(iRow, 4);
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));

    pItem = m_pModelExtVTerminalConn->item(iRow, 5);
    pItem->setText(pSideDevice->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));

    pItem = m_pModelExtVTerminalConn->item(iRow, 6);
    pItem->setText(pSideStrap ? pSideStrap->GetDisplayName() : "");
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));

    pItem = m_pModelExtVTerminalConn->item(iRow, 7);
    pItem->setText(pSideVTerminal->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pVTerminalConn));
}

void CopyBayDlg::UpdateExternalInfoset(ProjectExplorer::PeInfoSet *pInfoSet)
{
    if(!pInfoSet)
        return;

    int iRow = -1;
    for(int i = 0; i < m_pModelExtInfoSet->rowCount(); i++)
    {
        QStandardItem *pItem = m_pModelExtInfoSet->item(i, 0);
        if(reinterpret_cast<ProjectExplorer::PeInfoSet*>(pItem->data().toInt()) == pInfoSet)
        {
            iRow = i;
            break;
        }
    }
    if(iRow < 0)
        return;

    ProjectExplorer::PeDevice *pTxIED = pInfoSet->GetTxIED();
    ProjectExplorer::PeDevice *pRxIED = pInfoSet->GetRxIED();
    ProjectExplorer::PeDevice *pSwitch1 = pInfoSet->GetSwitch1();
    ProjectExplorer::PeDevice *pSwitch2 = pInfoSet->GetSwitch2();
    ProjectExplorer::PeDevice *pSwitch3 = pInfoSet->GetSwitch3();
    ProjectExplorer::PeDevice *pSwitch4 = pInfoSet->GetSwitch4();

    QStandardItem *pItem = m_pModelExtInfoSet->item(iRow, 1);
    pItem->setText(pInfoSet->GetDisplayName());
    pItem->setData(reinterpret_cast<int>(pInfoSet));

    pItem = m_pModelExtInfoSet->item(iRow, 2);
    pItem->setText(ProjectExplorer::PeInfoSet::GetInfoSetTypeName(pInfoSet->GetInfoSetType()));
    pItem->setData(reinterpret_cast<int>(pInfoSet));

    QString strTxIED = "";
    if(pTxIED)
    {
        strTxIED = pTxIED->GetDisplayName();
        if(ProjectExplorer::PePort *pTxPort = pInfoSet->GetTxPort())
            strTxIED = QString("%1<%2:%3>").arg(strTxIED).arg(pTxPort->GetParentBoard()->GetDisplayName()).arg(pTxPort->GetDisplayName());
    }
    pItem = m_pModelExtInfoSet->item(iRow, 3);
    pItem->setText(strTxIED);
    pItem->setData(reinterpret_cast<int>(pInfoSet));

    QString strSwitch1 = "";
    if(pSwitch1)
    {
        strSwitch1 = pSwitch1->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch1TxPort = pInfoSet->GetSwitch1TxPort())
            strSwitch1 = QString("%1<%2>").arg(strSwitch1).arg(pSwitch1TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch1RxPort = pInfoSet->GetSwitch1RxPort())
            strSwitch1 = QString("<%1>%2").arg(pSwitch1RxPort->GetDisplayName()).arg(strSwitch1);
    }
    pItem = m_pModelExtInfoSet->item(iRow, 4);
    pItem->setText(strSwitch1);

    QString strSwitch2 = "";
    if(pSwitch2)
    {
        strSwitch2 = pSwitch2->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch2TxPort = pInfoSet->GetSwitch2TxPort())
            strSwitch2 = QString("%1<%2>").arg(strSwitch2).arg(pSwitch2TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch2RxPort = pInfoSet->GetSwitch2RxPort())
            strSwitch2 = QString("<%1>%2").arg(pSwitch2RxPort->GetDisplayName()).arg(strSwitch2);
    }
    pItem = m_pModelExtInfoSet->item(iRow, 5);
    pItem->setText(strSwitch2);

    QString strSwitch3 = "";
    if(pSwitch3)
    {
        strSwitch3 = pSwitch3->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch3TxPort = pInfoSet->GetSwitch3TxPort())
            strSwitch3 = QString("%1<%2>").arg(strSwitch3).arg(pSwitch3TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch3RxPort = pInfoSet->GetSwitch3RxPort())
            strSwitch3 = QString("<%1>%2").arg(pSwitch3RxPort->GetDisplayName()).arg(strSwitch3);
    }
    pItem = m_pModelExtInfoSet->item(iRow, 6);
    pItem->setText(strSwitch3);

    QString strSwitch4 = "";
    if(pSwitch4)
    {
        strSwitch4 = pSwitch4->GetDisplayName();
        if(ProjectExplorer::PePort *pSwitch4TxPort = pInfoSet->GetSwitch4TxPort())
            strSwitch4 = QString("%1<%2>").arg(strSwitch4).arg(pSwitch4TxPort->GetDisplayName());

        if(ProjectExplorer::PePort *pSwitch4RxPort = pInfoSet->GetSwitch4RxPort())
            strSwitch4 = QString("<%1>%2").arg(pSwitch4RxPort->GetDisplayName()).arg(strSwitch4);
    }
    pItem = m_pModelExtInfoSet->item(iRow, 7);
    pItem->setText(strSwitch4);

    QString strRxIED = "";
    if(pRxIED)
    {
        strRxIED = pRxIED->GetDisplayName();
        if(ProjectExplorer::PePort *pRxPort = pInfoSet->GetRxPort())
            strRxIED = QString("<%1:%2>%3").arg(pRxPort->GetParentBoard()->GetDisplayName()).arg(pRxPort->GetDisplayName()).arg(strRxIED);
    }
    pItem = m_pModelExtInfoSet->item(iRow, 8);
    pItem->setText(strRxIED);
}

QString CopyBayDlg::ValidInfoset(ProjectExplorer::PeInfoSet *pInfoSet, const QList<ProjectExplorer::PeInfoSet*> &lstAllInfoSets)
{
    if(!pInfoSet)
        return "";

    ProjectExplorer::PePort *pPortError = 0;
    if(pInfoSet->GetTxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetTxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetTxPort();
    else if(pInfoSet->GetRxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetRxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetRxPort();
    else if(pInfoSet->GetSwitch1RxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch1RxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch1RxPort();
    else if(pInfoSet->GetSwitch1TxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch1TxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch1TxPort();
    else if(pInfoSet->GetSwitch2RxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch2RxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch2RxPort();
    else if(pInfoSet->GetSwitch2TxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch2TxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch2TxPort();
    else if(pInfoSet->GetSwitch3RxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch3RxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch3RxPort();
    else if(pInfoSet->GetSwitch3TxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch3TxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch3TxPort();
    else if(pInfoSet->GetSwitch4RxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch4RxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch4RxPort();
    else if(pInfoSet->GetSwitch4TxPort() && !ValidatePortConnection(pInfoSet, pInfoSet->GetSwitch4TxPort(), lstAllInfoSets))
        pPortError = pInfoSet->GetSwitch4TxPort();

    if(pPortError)
        return tr("The port '%1' has connected to a different port in other information logic.").arg(GetPortPath(pPortError, 0));

    return "";
}

QString CopyBayDlg::ValidVTerminalConn(ProjectExplorer::PeVTerminalConn *pVTerminalConn, const QList<ProjectExplorer::PeVTerminalConn*> &lstAllVTerminalConns)
{
    if(!pVTerminalConn)
        return "";

   ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();
   foreach(ProjectExplorer::PeVTerminalConn *pVC, lstAllVTerminalConns)
   {
       if(pVC->GetRxVTerminal() == pRxVTerminal)
           return tr("The signal '%1' has been used").arg(pRxVTerminal->GetDisplayName());
   }

   return "";
}

void CopyBayDlg::UpdateInfosetItemStatus()
{
    ProjectExplorer::PeProjectVersion *pProjectSrc = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxSrcProject->itemData(m_pComboBoxSrcProject->currentIndex()).toInt());
    if(!pProjectSrc)
        return;

    m_pModelIntInfoSet->blockSignals(true);
    m_pModelExtInfoSet->blockSignals(true);

    QList<ProjectExplorer::PeInfoSet*> lstAllInfoSets = GetNewInfoSets() + pProjectSrc->GetAllInfoSets();
    for(int i = 0; i < m_pModelIntInfoSet->rowCount(); i++)
    {
        QStandardItem *pItem = m_pModelIntInfoSet->item(i, 0);
        ProjectExplorer::PeInfoSet *pInfoSet = reinterpret_cast<ProjectExplorer::PeInfoSet*>(pItem->data().toInt());

        if(pItem->checkState() == Qt::Checked)
        {
            QList<ProjectExplorer::PeInfoSet*> lstInfoSets = lstAllInfoSets;
            lstInfoSets.removeAll(pInfoSet);

            QString strError = ValidInfoset(pInfoSet, lstInfoSets);
            for(int i = 0; i < m_pModelIntInfoSet->columnCount(); i++)
            {
                m_pModelIntInfoSet->item(pItem->row(), i)->setForeground(strError.isEmpty() ? Qt::black : Qt::red);
                m_pModelIntInfoSet->item(pItem->row(), i)->setToolTip(strError);
            }

        }
        else
        {
            for(int i = 0; i < m_pModelIntInfoSet->columnCount(); i++)
            {
                m_pModelIntInfoSet->item(pItem->row(), i)->setForeground(Qt::gray);
                m_pModelIntInfoSet->item(pItem->row(), i)->setToolTip("");
            }
        }
    }

    for(int i = 0; i < m_pModelExtInfoSet->rowCount(); i++)
    {
        QStandardItem *pItem = m_pModelExtInfoSet->item(i, 0);
        ProjectExplorer::PeInfoSet *pInfoSet = reinterpret_cast<ProjectExplorer::PeInfoSet*>(pItem->data().toInt());

        if(pItem->checkState() == Qt::Checked)
        {
            QList<ProjectExplorer::PeInfoSet*> lstInfoSets = lstAllInfoSets;
            lstInfoSets.removeAll(pInfoSet);

            QString strError = ValidInfoset(pInfoSet, lstInfoSets);
            for(int i = 0; i < m_pModelExtInfoSet->columnCount(); i++)
            {
                m_pModelExtInfoSet->item(pItem->row(), i)->setForeground(strError.isEmpty() ? Qt::black : Qt::red);
                m_pModelExtInfoSet->item(pItem->row(), i)->setToolTip(strError);
            }

        }
        else
        {
            for(int i = 0; i < m_pModelExtInfoSet->columnCount(); i++)
            {
                m_pModelExtInfoSet->item(pItem->row(), i)->setForeground(Qt::gray);
                m_pModelExtInfoSet->item(pItem->row(), i)->setToolTip("");
            }
        }
    }

    m_pModelIntInfoSet->blockSignals(false);
    m_pModelExtInfoSet->blockSignals(false);
}

void CopyBayDlg::UpdateVTerminalConnItemStatus()
{
    ProjectExplorer::PeProjectVersion *pProjectSrc = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxSrcProject->itemData(m_pComboBoxSrcProject->currentIndex()).toInt());
    if(!pProjectSrc)
        return;

    m_pModelIntVTerminalConn->blockSignals(true);
    m_pModelExtVTerminalConn->blockSignals(true);

    QList<ProjectExplorer::PeVTerminalConn*> lstAllVTerminalConns = GetNewVTerminalConns() + pProjectSrc->GetAllVTerminalConns();
    for(int i = 0; i < m_pModelIntVTerminalConn->rowCount(); i++)
    {
        QStandardItem *pItem = m_pModelIntVTerminalConn->item(i, 0);
        ProjectExplorer::PeVTerminalConn *pVTerminalConn = reinterpret_cast<ProjectExplorer::PeVTerminalConn*>(pItem->data().toInt());

        if(pItem->checkState() == Qt::Checked)
        {
            QList<ProjectExplorer::PeVTerminalConn*> lstVTerminalConns = lstAllVTerminalConns;
            lstVTerminalConns.removeAll(pVTerminalConn);

            QString strError = ValidVTerminalConn(pVTerminalConn, lstVTerminalConns);
            for(int i = 0; i < m_pModelIntVTerminalConn->columnCount(); i++)
            {
                m_pModelIntVTerminalConn->item(pItem->row(), i)->setForeground(strError.isEmpty() ? Qt::black : Qt::red);
                m_pModelIntVTerminalConn->item(pItem->row(), i)->setToolTip(strError);
            }

        }
        else
        {
            for(int i = 0; i < m_pModelIntVTerminalConn->columnCount(); i++)
            {
                m_pModelIntVTerminalConn->item(pItem->row(), i)->setForeground(Qt::gray);
                m_pModelIntVTerminalConn->item(pItem->row(), i)->setToolTip("");
            }
        }
    }

    for(int i = 0; i < m_pModelExtVTerminalConn->rowCount(); i++)
    {
        QStandardItem *pItem = m_pModelExtVTerminalConn->item(i, 0);
        ProjectExplorer::PeVTerminalConn *pVTerminalConn = reinterpret_cast<ProjectExplorer::PeVTerminalConn*>(pItem->data().toInt());

        if(pItem->checkState() == Qt::Checked)
        {
            QList<ProjectExplorer::PeVTerminalConn*> lstVTerminalConns = lstAllVTerminalConns;
            lstVTerminalConns.removeAll(pVTerminalConn);

            QString strError = ValidVTerminalConn(pVTerminalConn, lstVTerminalConns);
            for(int i = 0; i < m_pModelExtVTerminalConn->columnCount(); i++)
            {
                m_pModelExtVTerminalConn->item(pItem->row(), i)->setForeground(strError.isEmpty() ? Qt::black : Qt::red);
                m_pModelExtVTerminalConn->item(pItem->row(), i)->setToolTip(strError);
            }

        }
        else
        {
            for(int i = 0; i < m_pModelExtVTerminalConn->columnCount(); i++)
            {
                m_pModelExtVTerminalConn->item(pItem->row(), i)->setForeground(Qt::gray);
                m_pModelExtVTerminalConn->item(pItem->row(), i)->setToolTip("");
            }
        }
    }

    m_pModelIntVTerminalConn->blockSignals(false);
    m_pModelExtVTerminalConn->blockSignals(false);
}

QList<ProjectExplorer::PeInfoSet*> CopyBayDlg::GetNewInfoSets() const
{
    QList<int> lstNewInfoSetGroups;

    for(int i = 0; i < m_pModelIntInfoSet->rowCount(); i++)
    {
        QStandardItem *pItem = m_pModelIntInfoSet->item(i, 0);
        if(pItem->checkState() != Qt::Checked)
            continue;

        if(ProjectExplorer::PeInfoSet *pInfoSet = reinterpret_cast<ProjectExplorer::PeInfoSet*>(pItem->data().toInt()))
            lstNewInfoSetGroups.append(pInfoSet->GetGroup());
    }

    for(int i = 0; i < m_pModelExtInfoSet->rowCount(); i++)
    {
        QStandardItem *pItem = m_pModelExtInfoSet->item(i, 0);
        if(pItem->checkState() != Qt::Checked)
            continue;

        if(ProjectExplorer::PeInfoSet *pInfoSet = reinterpret_cast<ProjectExplorer::PeInfoSet*>(pItem->data().toInt()))
            lstNewInfoSetGroups.append(pInfoSet->GetGroup());
    }

    QList<ProjectExplorer::PeInfoSet*> lstNewInfoSets;
    foreach(ProjectExplorer::PeInfoSet *pInfoset, m_lstNewInfoSets)
    {
        if(lstNewInfoSetGroups.contains(pInfoset->GetGroup()))
            lstNewInfoSets.append(pInfoset);
    }

    return lstNewInfoSets;
}

QList<ProjectExplorer::PeVTerminalConn*> CopyBayDlg::GetNewVTerminalConns() const
{
    QList<ProjectExplorer::PeVTerminalConn*> lstNewVTerminalConns;

    for(int i = 0; i < m_pModelIntVTerminalConn->rowCount(); i++)
    {
        QStandardItem *pItem = m_pModelIntVTerminalConn->item(i, 0);
        if(pItem->checkState() != Qt::Checked)
            continue;

        if(ProjectExplorer::PeVTerminalConn *pVTerminalConn = reinterpret_cast<ProjectExplorer::PeVTerminalConn*>(pItem->data().toInt()))
            lstNewVTerminalConns.append(pVTerminalConn);
    }

    for(int i = 0; i < m_pModelExtVTerminalConn->rowCount(); i++)
    {
        QStandardItem *pItem = m_pModelExtVTerminalConn->item(i, 0);
        if(pItem->checkState() != Qt::Checked)
            continue;

        if(ProjectExplorer::PeVTerminalConn *pVTerminalConn = reinterpret_cast<ProjectExplorer::PeVTerminalConn*>(pItem->data().toInt()))
            lstNewVTerminalConns.append(pVTerminalConn);
    }

    return lstNewVTerminalConns;
}

void CopyBayDlg::accept()
{
    int iCurrentIndex = m_pComboBoxSrcProject->currentIndex();
    if(iCurrentIndex < 0)
        return;
    ProjectExplorer::PeProjectVersion *pProjectSrc = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxSrcProject->itemData(iCurrentIndex).toInt());
    if(!pProjectSrc)
        return;

    iCurrentIndex = m_pComboBoxDstProject->currentIndex();
    if(iCurrentIndex < 0)
        return;
    ProjectExplorer::PeProjectVersion *pProjectDst = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxDstProject->itemData(iCurrentIndex).toInt());
    if(!pProjectDst)
        return;

    ////////////////////////////////////////////////////////////////////
    // Validate Devices
    ////////////////////////////////////////////////////////////////////

    QStringList lstExistedBayNumbers;
    foreach(ProjectExplorer::PeBay *pBay, pProjectDst->GetAllBays())
        lstExistedBayNumbers.append(pBay->GetNumber());

    QStringList lstExistedDeviceNames;
    foreach(ProjectExplorer::PeDevice *pDevice, pProjectDst->GetAllDevices())
        lstExistedDeviceNames.append(pDevice->GetName());

    QStringList lstExistedCubicleNumbers;
    foreach(ProjectExplorer::PeCubicle *pCubicle, pProjectDst->GetAllCubicles())
        lstExistedCubicleNumbers.append(pCubicle->GetNumber());

    QList<ProjectExplorer::PeBay*> lstCheckedBays;
    QList<ProjectExplorer::PeCubicle*> lstCheckedCubicles;
    QList<ProjectExplorer::PeDevice*> lstSrcDevices;
    QList<ProjectExplorer::PeDevice*> lstDstDevices;
    foreach(CopyBayItem *pItem, m_pModel->GetCheckedItems())
    {
        if(CopyBayItemBay *pItemBay = dynamic_cast<CopyBayItemBay*>(pItem))
        {
            if(ProjectExplorer::PeBay *pBay = pItemBay->GetBayDst())
            {
                if(pBay->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId && !lstCheckedBays.contains(pBay))
                {
                    const QString strBayNumber = pBay->GetNumber();
                    if(lstExistedBayNumbers.contains(strBayNumber))
                    {
                        QMessageBox::critical(this, tr("Error"), tr("%1 '%2' has existed, please input a valid value.").arg(pBay->GetObjectTypeName()).arg(pBay->GetDisplayName()));
                        return;
                    }

                    lstExistedBayNumbers.append(strBayNumber);
                    lstCheckedBays.append(pBay);
                }
            }
        }
        else if(CopyBayItemDevice *pItemDevice = dynamic_cast<CopyBayItemDevice*>(pItem))
        {
            if(ProjectExplorer::PeDevice *pDevice = pItemDevice->GetDeviceDst())
            {
                if(pDevice->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
                {
                    const QString strDeviceName = pDevice->GetName();
                    if(lstExistedDeviceNames.contains(strDeviceName))
                    {
                        QMessageBox::critical(this, tr("Error"), tr("%1 '%2' has existed, please input a valid value.").arg(pDevice->GetObjectTypeName()).arg(pDevice->GetDisplayName()));
                        return;
                    }

                    lstExistedDeviceNames.append(strDeviceName);
                }

                if(ProjectExplorer::PeCubicle *pCubicle = pDevice->GetParentCubicle())
                {
                    if(pCubicle->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId && !lstCheckedCubicles.contains(pCubicle))
                    {
                        const QString strCubicleNumber = pCubicle->GetNumber();
                        if(lstExistedCubicleNumbers.contains(strCubicleNumber))
                        {
                            QMessageBox::critical(this, tr("Error"), tr("%1 '%2' has existed, please input a valid value.").arg(pCubicle->GetObjectTypeName()).arg(pCubicle->GetDisplayName()));
                            return;
                        }

                        lstExistedCubicleNumbers.append(strCubicleNumber);
                        lstCheckedCubicles.append(pCubicle);
                    }
                }
            }

            lstSrcDevices.append(pItemDevice->GetDeviceSrc());

            ProjectExplorer::PeDevice *pDeviceDst = pItemDevice->GetDeviceDst();
            if(ProjectExplorer::PeBay *pBay = pDeviceDst->GetParentBay())
            {
                if(pBay->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
                    pDeviceDst->SetParentBay(pProjectDst->FindBayById(pBay->GetId()));
            }
            if(ProjectExplorer::PeCubicle *pCubicle = pDeviceDst->GetParentCubicle())
            {
                if(pCubicle->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
                    pDeviceDst->SetParentCubicle(pProjectDst->FindCubicleById(pCubicle->GetId()));
            }
            lstDstDevices.append(pDeviceDst);
        }
    }

    if(lstSrcDevices.isEmpty() || lstDstDevices.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Please select at least one device for copy operation."));
        return;
    }

    ////////////////////////////////////////////////////////////////////
    // Validate VTerminalConns
    ////////////////////////////////////////////////////////////////////
    const QList<ProjectExplorer::PeVTerminalConn*> lstNewVTerminalConns = GetNewVTerminalConns();
    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, lstNewVTerminalConns)
    {
        QList<ProjectExplorer::PeVTerminalConn*> lstAllVTerminalConns = lstNewVTerminalConns + pProjectSrc->GetAllVTerminalConns();
        lstAllVTerminalConns.removeAll(pVTerminalConn);

        QString strError = ValidVTerminalConn(pVTerminalConn, lstAllVTerminalConns);
        if(!strError.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), strError);
            return;
        }
    }

    ////////////////////////////////////////////////////////////////////
    // Validate Infosets
    ////////////////////////////////////////////////////////////////////
    const QList<ProjectExplorer::PeInfoSet*> lstNewInfoSets = GetNewInfoSets();
    foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstNewInfoSets)
    {
        QList<ProjectExplorer::PeInfoSet*> lstAllInfoSets = lstNewInfoSets + pProjectSrc->GetAllInfoSets();
        lstAllInfoSets.removeAll(pInfoSet);

        QString strError = ValidInfoset(pInfoSet, lstAllInfoSets);
        if(!strError.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), strError);
            return;
        }
    }

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    ///////////////////////////////////////////////////////////////////////
    // Prepare
    ///////////////////////////////////////////////////////////////////////
    QList<ProjectExplorer::PeBay*> lstNewBays;
    QList<ProjectExplorer::PeCubicle*> lstNewCubicles;
    foreach(const ProjectExplorer::PeDevice *pDevice, lstDstDevices)
    {
        if(ProjectExplorer::PeBay *pBay = pDevice->GetParentBay())
        {
            if(pBay->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId && !lstNewBays.contains(pBay))
                lstNewBays.append(pBay);
        }

        if(ProjectExplorer::PeCubicle *pCubicle = pDevice->GetParentCubicle())
        {
            if(pCubicle->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId && !lstNewCubicles.contains(pCubicle))
                lstNewCubicles.append(pCubicle);
        }
    }

    ///////////////////////////////////////////////////////////////////////
    // Handle Database
    ///////////////////////////////////////////////////////////////////////
    ProjectExplorer::DbTransaction DbTrans(pProjectDst, true);

    // Create new bays
    foreach(ProjectExplorer::PeBay *pBay, lstNewBays)
    {
        if(!pProjectDst->DbCreateObject(*pBay, false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    // Create new cubicles
    foreach(ProjectExplorer::PeCubicle *pCubicle, lstNewCubicles)
    {
        if(!pProjectDst->DbCreateObject(*pCubicle, false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    // Create new devices
    for(int i = 0; i < lstDstDevices.size(); i++)
    {
        ProjectExplorer::PeDevice *pNewDevice = lstDstDevices.at(i);
        ProjectExplorer::PeDevice *pOriDevice = lstSrcDevices.at(i);

        if(!pProjectDst->DbCreateObject(*pNewDevice, false))
        {
            DbTrans.Rollback();
            return;
        }

        QByteArray baModelContent;
        if(!pProjectSrc->DbReadDeviceModel(pOriDevice->GetId(), baModelContent))
        {
            DbTrans.Rollback();
            return;
        }

        if(!pProjectDst->DbUpdateDeviceModel(pNewDevice->GetId(), baModelContent, false))
        {
            DbTrans.Rollback();
            return;
        }

        QByteArray baCommunicationContent;
        if(!pProjectDst->DbReadSclCommunication(baCommunicationContent))
        {
            DbTrans.Rollback();
            return;
        }

        if(!baCommunicationContent.isNull() && !baCommunicationContent.isEmpty())
        {
            SclParser::SCLElement SCLElementCommunication("Communication", 0, 0);
            SCLElementCommunication.ReadFromBinary(baCommunicationContent);

            bool bNeedUpdateCommunication = false;
            foreach(SclParser::SCLElement *pSCLElementSubNetwork, SCLElementCommunication.FindChildByType("SubNetwork"))
            {
                foreach(SclParser::SCLElement *pSCLElementConnectedAP, pSCLElementSubNetwork->FindChildByType("ConnectedAP"))
                {
                    if(pSCLElementConnectedAP->GetAttribute("iedName") == pOriDevice->GetName())
                    {
                        SclParser::SCLElement *pSCLElementConnectedAPClone = pSCLElementConnectedAP->Clone(true);
                        pSCLElementConnectedAPClone->SetAttribute("iedName", pNewDevice->GetName());
                        pSCLElementSubNetwork->AttachChild(pSCLElementConnectedAPClone, -1, false);

                        bNeedUpdateCommunication = true;
                    }
                }
            }

            if(bNeedUpdateCommunication)
            {
                if(!pProjectDst->DbUpdateSclCommunication(SCLElementCommunication.WriteToBinary(), false))
                {
                    DbTrans.Rollback();
                    return;
                }
            }
        }

        // Create new boards
        foreach(ProjectExplorer::PeBoard *pBoard, pNewDevice->GetChildBoards())
        {
            if(!pProjectDst->DbCreateObject(*pBoard, false))
            {
                DbTrans.Rollback();
                return;
            }

            // Create new ports
            foreach(ProjectExplorer::PePort *pPort, pBoard->GetChildPorts())
            {
                if(!pProjectDst->DbCreateObject(*pPort, false))
                {
                    DbTrans.Rollback();
                    return;
                }
            }
        }

        // Create new vterminals
        foreach(ProjectExplorer::PeVTerminal *pVTerminal, pNewDevice->GetVTerminals())
        {
            if(!pProjectDst->DbCreateObject(*pVTerminal, false))
            {
                DbTrans.Rollback();
                return;
            }
        }

        // Create new straps
        foreach(ProjectExplorer::PeStrap *pStrap, pNewDevice->GetStraps())
        {
            if(!pProjectDst->DbCreateObject(*pStrap, false))
            {
                DbTrans.Rollback();
                return;
            }
        }
    }

    // Create new infoset
    foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstNewInfoSets)
    {
        if(!pProjectDst->DbCreateObject(*pInfoSet, false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    // Create new verminalconn
    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, lstNewVTerminalConns)
    {
        if(!pProjectDst->DbCreateObject(*pVTerminalConn, false))
        {
            DbTrans.Rollback();
            return;
        }
    }

    if(!DbTrans.Commit())
        return;

    ///////////////////////////////////////////////////////////////////////
    // Handle Object
    ///////////////////////////////////////////////////////////////////////

    // Create new bays
    QMap<ProjectExplorer::PeBay*, ProjectExplorer::PeBay*> mapTempToCreateBay;
    foreach(ProjectExplorer::PeBay *pBay, lstNewBays)
    {
        ProjectExplorer::PeBay *pBayCreated = qobject_cast<ProjectExplorer::PeBay*>(pProjectDst->CreateObject(*pBay));
        if(!pBayCreated)
            return;

        mapTempToCreateBay.insert(pBay, pBayCreated);
    }

    // Create new cubicles
    QMap<ProjectExplorer::PeCubicle*, ProjectExplorer::PeCubicle*> mapTempToCreateCubicle;
    foreach(ProjectExplorer::PeCubicle *pCubicle, lstNewCubicles)
    {
        ProjectExplorer::PeCubicle *pCubicleCreated = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectDst->CreateObject(*pCubicle));
        if(!pCubicleCreated)
            return;

        mapTempToCreateCubicle.insert(pCubicle, pCubicleCreated);
    }

    // Create new devices
    QMap<ProjectExplorer::PeDevice*, ProjectExplorer::PeDevice*> mapTempToCreateDevice;
    QMap<ProjectExplorer::PePort*, ProjectExplorer::PePort*> mapTempToCreatePort;
    QMap<ProjectExplorer::PeVTerminal*, ProjectExplorer::PeVTerminal*> mapTempToCreateVTerminal;
    QMap<ProjectExplorer::PeStrap*, ProjectExplorer::PeStrap*> mapTempToCreateStrap;
    for(int i = 0; i < lstDstDevices.size(); i++)
    {
        ProjectExplorer::PeDevice *pNewDevice = lstDstDevices.at(i);
        ProjectExplorer::PeDevice *pOriDevice = lstSrcDevices.at(i);

        if(ProjectExplorer::PeBay *pBayCreated = mapTempToCreateBay.value(pNewDevice->GetParentBay(), 0))
            pNewDevice->SetParentBay(pBayCreated);

        if(ProjectExplorer::PeCubicle *pCubicleCreated = mapTempToCreateCubicle.value(pNewDevice->GetParentCubicle(), 0))
            pNewDevice->SetParentCubicle(pCubicleCreated);

        ProjectExplorer::PeDevice *pDeviceCreated = qobject_cast<ProjectExplorer::PeDevice*>(pProjectDst->CreateObject(*pNewDevice));
        if(!pDeviceCreated)
            return;
        pProjectDst->UpdateDeviceModelStatus(pDeviceCreated, pOriDevice->GetHasModel());

        mapTempToCreateDevice.insert(pNewDevice, pDeviceCreated);

        // Create new boards
        foreach(ProjectExplorer::PeBoard *pBoard, pNewDevice->GetChildBoards())
        {
            pBoard->SetParentDevice(pDeviceCreated);
            ProjectExplorer::PeBoard *pBoardCreated = qobject_cast<ProjectExplorer::PeBoard*>(pProjectDst->CreateObject(*pBoard));
            if(!pBoardCreated)
                return;

            // Create new ports
            foreach(ProjectExplorer::PePort *pPort, pBoard->GetChildPorts())
            {
                pPort->SetParentBoard(pBoardCreated);
                ProjectExplorer::PePort *pPortCreated = qobject_cast<ProjectExplorer::PePort*>(pProjectDst->CreateObject(*pPort));
                if(!pPortCreated)
                    return;

                mapTempToCreatePort.insert(pPort, pPortCreated);
            }
        }

        // Create new vterminals
        foreach(ProjectExplorer::PeVTerminal *pVTerminal, pNewDevice->GetVTerminals())
        {
            pVTerminal->SetParentDevice(pDeviceCreated);
            ProjectExplorer::PeVTerminal *pVTerminalCreated = qobject_cast<ProjectExplorer::PeVTerminal*>(pProjectDst->CreateObject(*pVTerminal));
            if(!pVTerminalCreated)
                return;

            mapTempToCreateVTerminal.insert(pVTerminal, pVTerminalCreated);
        }

        // Create new straps
        foreach(ProjectExplorer::PeStrap *pStrap, pNewDevice->GetStraps())
        {
            pStrap->SetParentDevice(pDeviceCreated);
            ProjectExplorer::PeStrap *pStrapCreated = qobject_cast<ProjectExplorer::PeStrap*>(pProjectDst->CreateObject(*pStrap));
            if(!pStrapCreated)
                return;

            mapTempToCreateStrap.insert(pStrap, pStrapCreated);
        }
    }

    // Create new infoset
    foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstNewInfoSets)
    {
        if(ProjectExplorer::PeDevice *pTxIED = mapTempToCreateDevice.value(pInfoSet->GetTxIED(), 0))
            pInfoSet->SetTxIED(pTxIED);

        if(pInfoSet->GetTxPort())
        {
            if(ProjectExplorer::PePort *pTxPort = mapTempToCreatePort.value(pInfoSet->GetTxPort()))
                pInfoSet->SetTxPort(pTxPort);
        }

        if(ProjectExplorer::PeDevice *pRxIED = mapTempToCreateDevice.value(pInfoSet->GetRxIED(), 0))
            pInfoSet->SetRxIED(pRxIED);

        if(pInfoSet->GetRxPort())
        {
            if(ProjectExplorer::PePort *pRxPort = mapTempToCreatePort.value(pInfoSet->GetRxPort()))
                pInfoSet->SetRxPort(pRxPort);
        }

        if(pInfoSet->GetSwitch1())
        {
            if(ProjectExplorer::PeDevice *pSwitch1 = mapTempToCreateDevice.value(pInfoSet->GetSwitch1(), 0))
                pInfoSet->SetSwitch1(pSwitch1);
        }
        if(pInfoSet->GetSwitch1TxPort())
        {
            if(ProjectExplorer::PePort *pSwitch1TxPort = mapTempToCreatePort.value(pInfoSet->GetSwitch1TxPort()))
                pInfoSet->SetSwitch1TxPort(pSwitch1TxPort);
        }
        if(pInfoSet->GetSwitch1RxPort())
        {
            if(ProjectExplorer::PePort *pSwitch1RxPort = mapTempToCreatePort.value(pInfoSet->GetSwitch1RxPort()))
                pInfoSet->SetSwitch1RxPort(pSwitch1RxPort);
        }

        if(pInfoSet->GetSwitch2())
        {
            if(ProjectExplorer::PeDevice *pSwitch2 = mapTempToCreateDevice.value(pInfoSet->GetSwitch2(), 0))
                pInfoSet->SetSwitch2(pSwitch2);
        }
        if(pInfoSet->GetSwitch2TxPort())
        {
            if(ProjectExplorer::PePort *pSwitch2TxPort = mapTempToCreatePort.value(pInfoSet->GetSwitch2TxPort()))
                pInfoSet->SetSwitch2TxPort(pSwitch2TxPort);
        }
        if(pInfoSet->GetSwitch2RxPort())
        {
            if(ProjectExplorer::PePort *pSwitch2RxPort = mapTempToCreatePort.value(pInfoSet->GetSwitch2RxPort()))
                pInfoSet->SetSwitch2RxPort(pSwitch2RxPort);
        }

        if(pInfoSet->GetSwitch3())
        {
            if(ProjectExplorer::PeDevice *pSwitch3 = mapTempToCreateDevice.value(pInfoSet->GetSwitch3(), 0))
                pInfoSet->SetSwitch3(pSwitch3);
        }
        if(pInfoSet->GetSwitch3TxPort())
        {
            if(ProjectExplorer::PePort *pSwitch3TxPort = mapTempToCreatePort.value(pInfoSet->GetSwitch3TxPort()))
                pInfoSet->SetSwitch3TxPort(pSwitch3TxPort);
        }
        if(pInfoSet->GetSwitch3RxPort())
        {
            if(ProjectExplorer::PePort *pSwitch3RxPort = mapTempToCreatePort.value(pInfoSet->GetSwitch3RxPort()))
                pInfoSet->SetSwitch3RxPort(pSwitch3RxPort);
        }

        if(pInfoSet->GetSwitch4())
        {
            if(ProjectExplorer::PeDevice *pSwitch4 = mapTempToCreateDevice.value(pInfoSet->GetSwitch4(), 0))
                pInfoSet->SetSwitch4(pSwitch4);
        }
        if(pInfoSet->GetSwitch4TxPort())
        {
            if(ProjectExplorer::PePort *pSwitch4TxPort = mapTempToCreatePort.value(pInfoSet->GetSwitch4TxPort()))
                pInfoSet->SetSwitch4TxPort(pSwitch4TxPort);
        }
        if(pInfoSet->GetSwitch4RxPort())
        {
            if(ProjectExplorer::PePort *pSwitch4RxPort = mapTempToCreatePort.value(pInfoSet->GetSwitch4RxPort()))
                pInfoSet->SetSwitch4RxPort(pSwitch4RxPort);
        }

        if(!pProjectDst->CreateObject(*pInfoSet))
            return;
    }

    // Create new vterminalconn
    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, lstNewVTerminalConns)
    {
        if(ProjectExplorer::PeVTerminal *pTxVTerminal = mapTempToCreateVTerminal.value(pVTerminalConn->GetTxVTerminal()))
            pVTerminalConn->SetTxVTerminal(pTxVTerminal);

        if(ProjectExplorer::PeVTerminal *pRxVTerminal = mapTempToCreateVTerminal.value(pVTerminalConn->GetRxVTerminal()))
            pVTerminalConn->SetRxVTerminal(pRxVTerminal);

        if(pVTerminalConn->GetTxStrap())
        {
            if(ProjectExplorer::PeStrap *pTxStrap = mapTempToCreateStrap.value(pVTerminalConn->GetTxStrap()))
                pVTerminalConn->SetTxStrap(pTxStrap);
        }

        if(pVTerminalConn->GetRxStrap())
        {
            if(ProjectExplorer::PeStrap *pRxStrap = mapTempToCreateStrap.value(pVTerminalConn->GetRxStrap()))
                pVTerminalConn->SetRxStrap(pRxStrap);
        }

        if(!pProjectDst->CreateObject(*pVTerminalConn))
            return;
    }

    QDialog::accept();
}

void CopyBayDlg::SlotSrcProjectChanged(int iCurrentIndex)
{
    if(iCurrentIndex < 0)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxSrcProject->itemData(iCurrentIndex).toInt());
    if(!pProjectVersion)
        return;

    m_pModel->SetSrcProject(pProjectVersion);

    SlotDstProjectChanged(m_pComboBoxDstProject->currentIndex());
    SlotUpdateNewData();
}

void CopyBayDlg::SlotDstProjectChanged(int iCurrentIndex)
{
    ProjectExplorer::PeProjectVersion *pProjectVersion = 0;
    if(iCurrentIndex >= 0)
        pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxDstProject->itemData(iCurrentIndex).toInt());

    m_pModel->SetDstProject(pProjectVersion);
    SlotUpdateNewData();
}

void CopyBayDlg::SlotBayObjectDoubleClicked(const QModelIndex &index)
{
    CopyBayItem *pItem = m_pModel->ItemFromIndex(index);
    if(!pItem || !pItem->GetChecked())
        return;

    if(CopyBayItemBay *pItemBay = dynamic_cast<CopyBayItemBay*>(pItem))
    {
        if(index.column() == 2)
        {
            SelectBayDlg dlg(pItemBay->GetBayDst(), this);
            if(dlg.exec() == QDialog::Accepted)
            {
                m_pModel->UpdateItem(pItemBay);
                SlotUpdateNewData();
            }
        }
    }
    else if(CopyBayItemDevice *pItemDevice = dynamic_cast<CopyBayItemDevice*>(pItem))
    {
        if(index.column() == 2)
        {
            SelectDeviceDlg dlg(pItemDevice->GetDeviceDst(), this);
            if(dlg.exec() == QDialog::Accepted)
            {
                m_pModel->UpdateItem(pItemDevice);
                SlotUpdateNewData();
            }
        }
        else if(index.column() == 3 && pItemDevice->GetDeviceDst()->GetParentCubicle())
        {
            SelectCubicleDlg dlg(pItemDevice->GetDeviceDst()->GetParentCubicle(), this);
            if(dlg.exec() == QDialog::Accepted)
            {
                m_pModel->UpdateItem(pItemDevice);
                SlotUpdateNewData();
            }
        }
    }
}

void CopyBayDlg::SlotReplace()
{
    if(!m_pReplaceDlg)
        return;

    QList<ProjectExplorer::PeProjectObject*> lstReplacedObjects;

    lstReplacedObjects.clear();
    if(m_pReplaceDlg->GetReplaceBayObjectName())
    {
        const QString strSearchContent = m_pReplaceDlg->GetBayObjectNameSrc();
        const QString strReplaceContent = m_pReplaceDlg->GetBayObjectNameDst();

        foreach(CopyBayItem *pItem, m_pModel->GetCheckedItems())
        {
            if(CopyBayItemBay *pItemBay = dynamic_cast<CopyBayItemBay*>(pItem))
            {
                if(ProjectExplorer::PeBay *pBay = pItemBay->GetBayDst())
                {
                    if(!lstReplacedObjects.contains(pBay))
                    {
                        pBay->SetNumber(pBay->GetNumber().replace(strSearchContent, strReplaceContent));
                        lstReplacedObjects.append(pBay);
                    }
                }
            }
            else if(CopyBayItemDevice *pItemDevice = dynamic_cast<CopyBayItemDevice*>(pItem))
            {
                if(ProjectExplorer::PeDevice *pDevice = pItemDevice->GetDeviceDst())
                {
                    if(!lstReplacedObjects.contains(pDevice))
                    {
                        pDevice->SetName(pDevice->GetName().replace(strSearchContent, strReplaceContent));
                        lstReplacedObjects.append(pDevice);
                    }
                }
            }
        }
    }

    lstReplacedObjects.clear();
    if(m_pReplaceDlg->GetReplaceBayObjectDesc())
    {
        const QString strSearchContent = m_pReplaceDlg->GetBayObjectDescSrc();
        const QString strReplaceContent = m_pReplaceDlg->GetBayObjectDescDst();

        foreach(CopyBayItem *pItem, m_pModel->GetCheckedItems())
        {
            if(CopyBayItemBay *pItemBay = dynamic_cast<CopyBayItemBay*>(pItem))
            {
                if(ProjectExplorer::PeBay *pBay = pItemBay->GetBayDst())
                {
                    if(!lstReplacedObjects.contains(pBay))
                    {
                        pBay->SetName(pBay->GetName().replace(strSearchContent, strReplaceContent));
                        lstReplacedObjects.append(pBay);
                    }
                }
            }
            else if(CopyBayItemDevice *pItemDevice = dynamic_cast<CopyBayItemDevice*>(pItem))
            {
                if(ProjectExplorer::PeDevice *pDevice = pItemDevice->GetDeviceDst())
                {
                    if(!lstReplacedObjects.contains(pDevice))
                    {
                        pDevice->SetDescription(pDevice->GetDescription().replace(strSearchContent, strReplaceContent));
                        lstReplacedObjects.append(pDevice);
                    }
                }
            }
        }
    }

    lstReplacedObjects.clear();
    if(m_pReplaceDlg->GetReplaceCubicleObjectName())
    {
        const QString strSearchContent = m_pReplaceDlg->GetCubicleObjectNameSrc();
        const QString strReplaceContent = m_pReplaceDlg->GetCubicleObjectNameDst();

        foreach(CopyBayItem *pItem, m_pModel->GetCheckedItems())
        {
            if(CopyBayItemDevice *pItemDevice = dynamic_cast<CopyBayItemDevice*>(pItem))
            {
                if(ProjectExplorer::PeDevice *pDevice = pItemDevice->GetDeviceDst())
                {
                    if(ProjectExplorer::PeCubicle *pCubicle = pDevice->GetParentCubicle())
                    {
                        if(!lstReplacedObjects.contains(pCubicle))
                        {
                            pCubicle->SetNumber(pCubicle->GetNumber().replace(strSearchContent, strReplaceContent));
                            lstReplacedObjects.append(pCubicle);
                        }
                    }
                }
            }
        }
    }

    lstReplacedObjects.clear();
    if(m_pReplaceDlg->GetReplaceCubicleObjectDesc())
    {
        const QString strSearchContent = m_pReplaceDlg->GetCubicleObjectDescSrc();
        const QString strReplaceContent = m_pReplaceDlg->GetCubicleObjectDescDst();

        foreach(CopyBayItem *pItem, m_pModel->GetCheckedItems())
        {
            if(CopyBayItemDevice *pItemDevice = dynamic_cast<CopyBayItemDevice*>(pItem))
            {
                if(ProjectExplorer::PeDevice *pDevice = pItemDevice->GetDeviceDst())
                {
                    if(ProjectExplorer::PeCubicle *pCubicle = pDevice->GetParentCubicle())
                    {
                        if(!lstReplacedObjects.contains(pCubicle))
                        {
                            pCubicle->SetName(pCubicle->GetName().replace(strSearchContent, strReplaceContent));
                            lstReplacedObjects.append(pCubicle);
                        }
                    }
                }
            }
        }
    }

    if(m_pReplaceDlg->GetReplaceBayObjectName() || m_pReplaceDlg->GetReplaceBayObjectDesc() ||
       m_pReplaceDlg->GetReplaceCubicleObjectName() ||m_pReplaceDlg->GetReplaceCubicleObjectDesc())
    {
        SlotUpdateNewData();
    }

    if(m_pReplaceDlg->GetReplaceVTerminalDesc())
    {
        const QString strSearchContent = m_pReplaceDlg->GetVTerminalDescSrc();
        const QString strReplaceContent = m_pReplaceDlg->GetVTerminalDescDst();

        for(int i = 0; i < m_pModelIntVTerminalConn->rowCount(); i++)
        {
            if(QStandardItem *pItem = m_pModelIntVTerminalConn->item(i))
            {
                if(ProjectExplorer::PeVTerminalConn *pVTerminalConn = reinterpret_cast<ProjectExplorer::PeVTerminalConn*>(pItem->data().toInt()))
                {
                    if(ProjectExplorer::PeVTerminal *pVTerminalTx = pVTerminalConn->GetTxVTerminal())
                        pVTerminalTx->SetProDesc(pVTerminalTx->GetProDesc().replace(strSearchContent, strReplaceContent));
                    if(ProjectExplorer::PeVTerminal *pVTerminalRx = pVTerminalConn->GetRxVTerminal())
                        pVTerminalRx->SetProDesc(pVTerminalRx->GetProDesc().replace(strSearchContent, strReplaceContent));

                    UpdateInternalVTerminalConn(pVTerminalConn);
                }
            }
        }

        for(int i = 0; i < m_pModelExtVTerminalConn->rowCount(); i++)
        {
            if(QStandardItem *pItem = m_pModelExtVTerminalConn->item(i))
            {
                if(ProjectExplorer::PeVTerminalConn *pVTerminalConn = reinterpret_cast<ProjectExplorer::PeVTerminalConn*>(pItem->data().toInt()))
                {
                    if(ProjectExplorer::PeVTerminal *pVTerminalTx = pVTerminalConn->GetTxVTerminal())
                        pVTerminalTx->SetProDesc(pVTerminalTx->GetProDesc().replace(strSearchContent, strReplaceContent));
                    if(ProjectExplorer::PeVTerminal *pVTerminalRx = pVTerminalConn->GetRxVTerminal())
                        pVTerminalRx->SetProDesc(pVTerminalRx->GetProDesc().replace(strSearchContent, strReplaceContent));

                    UpdateExternalVTerminalConn(pVTerminalConn);
                }
            }
        }
    }

    if(m_pReplaceDlg->GetReplaceInfoSetDesc())
    {
        const QString strSearchContent = m_pReplaceDlg->GetInfoSetDescSrc();
        const QString strReplaceContent = m_pReplaceDlg->GetInfoSetDescDst();

        for(int i = 0; i < m_pModelIntInfoSet->rowCount(); i++)
        {
            if(QStandardItem *pItem = m_pModelIntInfoSet->item(i))
            {
                if(ProjectExplorer::PeInfoSet *pInfoSet = reinterpret_cast<ProjectExplorer::PeInfoSet*>(pItem->data().toInt()))
                {
                    pInfoSet->SetDescription(pInfoSet->GetDescription().replace(strSearchContent, strReplaceContent));
                    UpdateInternalInfoset(pInfoSet);
                }
            }
        }

        for(int i = 0; i < m_pModelExtInfoSet->rowCount(); i++)
        {
            if(QStandardItem *pItem = m_pModelExtInfoSet->item(i))
            {
                if(ProjectExplorer::PeInfoSet *pInfoSet = reinterpret_cast<ProjectExplorer::PeInfoSet*>(pItem->data().toInt()))
                {
                    pInfoSet->SetDescription(pInfoSet->GetDescription().replace(strSearchContent, strReplaceContent));
                    UpdateExternalInfoset(pInfoSet);
                }
            }
        }
    }

    m_pReplaceDlg->hide();
    m_pReplaceDlg->show();
}

void CopyBayDlg::SlotUpdateNewData()
{
    Utils::WaitCursor waitcursor;
    Q_UNUSED(waitcursor)

    ClearModel();

    ProjectExplorer::PeProjectVersion *pSrcProjectVersion = m_pModel->GetSrcProject();
    ProjectExplorer::PeProjectVersion *pDstProjectVersion = m_pModel->GetDstProject();
    if(!pSrcProjectVersion || pSrcProjectVersion != pDstProjectVersion)
        return;

    QMap<ProjectExplorer::PeDevice*, ProjectExplorer::PeDevice*> mapSrcToDstDevices;
    QMap<ProjectExplorer::PePort*, ProjectExplorer::PePort*> mapSrcToDstPorts;
    QMap<ProjectExplorer::PeVTerminal*, ProjectExplorer::PeVTerminal*> mapSrcToDstVTerminals;
    QMap<ProjectExplorer::PeStrap*, ProjectExplorer::PeStrap*> mapSrcToDstStraps;
    foreach(CopyBayItem *pItem, m_pModel->GetCheckedItems())
    {
        if(CopyBayItemDevice *pItemDevice = dynamic_cast<CopyBayItemDevice*>(pItem))
        {
            ProjectExplorer::PeDevice *pDeviceSrc = pItemDevice->GetDeviceSrc();
            ProjectExplorer::PeDevice *pDeviceDst = pItemDevice->GetDeviceDst();
            mapSrcToDstDevices.insert(pDeviceSrc, pDeviceDst);

            QList<ProjectExplorer::PePort*> lstSrcPorts;
            foreach(ProjectExplorer::PeBoard *pBoard, pDeviceSrc->GetChildBoards())
            {
                foreach(ProjectExplorer::PePort *pPort, pBoard->GetChildPorts())
                    lstSrcPorts.append(pPort);
            }
            QList<ProjectExplorer::PePort*> lstDstPorts;
            foreach(ProjectExplorer::PeBoard *pBoard, pDeviceDst->GetChildBoards())
            {
                foreach(ProjectExplorer::PePort *pPort, pBoard->GetChildPorts())
                    lstDstPorts.append(pPort);
            }
            for(int i = 0; i < lstSrcPorts.size(); i++)
                mapSrcToDstPorts.insert(lstSrcPorts.at(i), lstDstPorts.at(i));

            QList<ProjectExplorer::PeVTerminal*> lstSrcVTerminals = pDeviceSrc->GetVTerminals();
            QList<ProjectExplorer::PeVTerminal*> lstDstVTerminals = pDeviceDst->GetVTerminals();
            for(int i = 0; i < lstSrcVTerminals.size(); i++)
                mapSrcToDstVTerminals.insert(lstSrcVTerminals.at(i), lstDstVTerminals.at(i));

            QList<ProjectExplorer::PeStrap*> lstSrcStraps = pDeviceSrc->GetStraps();
            QList<ProjectExplorer::PeStrap*> lstDstStraps = pDeviceDst->GetStraps();
            for(int i = 0; i < lstSrcStraps.size(); i++)
                mapSrcToDstStraps.insert(lstSrcStraps.at(i), lstDstStraps.at(i));
        }
    }
    if(mapSrcToDstDevices.isEmpty())
        return;

    int iCurrrentGroup = pSrcProjectVersion->DbGenerateNewInfoSetGroup();
    QStringList lstExtraInfoSetNames;

    /////////////////////////////////////////////////////////////
    // Update Internal InfoSets
    /////////////////////////////////////////////////////////////
    QList<ProjectExplorer::PeInfoSet*> lstAllInfoSets = pSrcProjectVersion->GetAllInfoSets();
    while(!lstAllInfoSets.isEmpty())
    {
        ProjectExplorer::PeInfoSet *pInfoSet = lstAllInfoSets.takeLast();

        ProjectExplorer::PeDevice *pTxIED = pInfoSet->GetTxIED();
        ProjectExplorer::PePort *pTxPort = pInfoSet->GetTxPort();
        ProjectExplorer::PeDevice *pRxIED = pInfoSet->GetRxIED();
        ProjectExplorer::PePort *pRxPort = pInfoSet->GetRxPort();
        ProjectExplorer::PeDevice *pSwitch1 = pInfoSet->GetSwitch1();
        ProjectExplorer::PePort *pSwitch1TxPort = pInfoSet->GetSwitch1TxPort();
        ProjectExplorer::PePort *pSwitch1RxPort = pInfoSet->GetSwitch1RxPort();
        ProjectExplorer::PeDevice *pSwitch2 = pInfoSet->GetSwitch2();
        ProjectExplorer::PePort *pSwitch2TxPort = pInfoSet->GetSwitch2TxPort();
        ProjectExplorer::PePort *pSwitch2RxPort = pInfoSet->GetSwitch2RxPort();
        ProjectExplorer::PeDevice *pSwitch3 = pInfoSet->GetSwitch3();
        ProjectExplorer::PePort *pSwitch3TxPort = pInfoSet->GetSwitch3TxPort();
        ProjectExplorer::PePort *pSwitch3RxPort = pInfoSet->GetSwitch3RxPort();
        ProjectExplorer::PeDevice *pSwitch4 = pInfoSet->GetSwitch4();
        ProjectExplorer::PePort *pSwitch4TxPort = pInfoSet->GetSwitch4TxPort();
        ProjectExplorer::PePort *pSwitch4RxPort = pInfoSet->GetSwitch4RxPort();

        if(!pTxIED || !pRxIED)
            continue;

        const bool bHasTxIED = mapSrcToDstDevices.contains(pTxIED);
        const bool bHasRxIED = mapSrcToDstDevices.contains(pRxIED);

        bool bInternal = (bHasTxIED && bHasRxIED);
        if(bInternal && pSwitch1)
        {
            bInternal = (bInternal && mapSrcToDstDevices.contains(pSwitch1));
            if(bInternal && pSwitch2)
            {
                bInternal = (bInternal && mapSrcToDstDevices.contains(pSwitch2));
                if(bInternal && pSwitch3)
                {
                    bInternal = (bInternal && mapSrcToDstDevices.contains(pSwitch3));
                    if(bInternal && pSwitch4)
                        bInternal = (bInternal && mapSrcToDstDevices.contains(pSwitch4));
                }
            }
        }

        if(!bInternal)
            continue;

        // Create InfoSet
        ProjectExplorer::PeInfoSet *pInfoSetNew = new ProjectExplorer::PeInfoSet(*pInfoSet);
        pInfoSetNew->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        pInfoSetNew->SetGroup(iCurrrentGroup);
        pInfoSetNew->SetTxIED(pTxIED ? mapSrcToDstDevices.value(pTxIED, pTxIED) : 0);
        pInfoSetNew->SetTxPort(pTxPort ? mapSrcToDstPorts.value(pTxPort, pTxPort) : 0);
        pInfoSetNew->SetRxIED(pRxIED ? mapSrcToDstDevices.value(pRxIED, pRxIED) : 0);
        pInfoSetNew->SetRxPort(pRxPort ? mapSrcToDstPorts.value(pRxPort, pRxPort) : 0);
        pInfoSetNew->SetSwitch1(pSwitch1 ? mapSrcToDstDevices.value(pSwitch1, pSwitch1) : 0);
        pInfoSetNew->SetSwitch1TxPort(pSwitch1TxPort ? mapSrcToDstPorts.value(pSwitch1TxPort, pSwitch1TxPort) : 0);
        pInfoSetNew->SetSwitch1RxPort(pSwitch1RxPort ? mapSrcToDstPorts.value(pSwitch1RxPort, pSwitch1RxPort) : 0);
        pInfoSetNew->SetSwitch2(pSwitch2 ? mapSrcToDstDevices.value(pSwitch2, pSwitch2) : 0);
        pInfoSetNew->SetSwitch2TxPort(pSwitch2TxPort ? mapSrcToDstPorts.value(pSwitch2TxPort, pSwitch2TxPort) : 0);
        pInfoSetNew->SetSwitch2RxPort(pSwitch2RxPort ? mapSrcToDstPorts.value(pSwitch2RxPort, pSwitch2RxPort) : 0);
        pInfoSetNew->SetSwitch3(pSwitch3 ? mapSrcToDstDevices.value(pSwitch3, pSwitch3) : 0);
        pInfoSetNew->SetSwitch3TxPort(pSwitch3TxPort ? mapSrcToDstPorts.value(pSwitch3TxPort, pSwitch3TxPort) : 0);
        pInfoSetNew->SetSwitch3RxPort(pSwitch3RxPort ? mapSrcToDstPorts.value(pSwitch3RxPort, pSwitch3RxPort) : 0);
        pInfoSetNew->SetSwitch4(pSwitch4 ? mapSrcToDstDevices.value(pSwitch4, pSwitch4) : 0);
        pInfoSetNew->SetSwitch4TxPort(pSwitch4TxPort ? mapSrcToDstPorts.value(pSwitch4TxPort, pSwitch4TxPort) : 0);
        pInfoSetNew->SetSwitch4RxPort(pSwitch4RxPort ? mapSrcToDstPorts.value(pSwitch4RxPort, pSwitch4RxPort) : 0);

        pInfoSetNew->UpdateName(lstExtraInfoSetNames);
        if(!pInfoSetNew->GetName().isEmpty())
            lstExtraInfoSetNames.append(pInfoSetNew->GetName());

        if(pInfoSet->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
            AddIntInfoSetItem(pInfoSetNew);

        m_lstNewInfoSets.append(pInfoSetNew);

        // Create Pair InfoSet
        if(ProjectExplorer::PeInfoSet *pInfoSetPair = pInfoSet->GetPairInfoSet())
        {
            lstAllInfoSets.removeAll(pInfoSetPair);

            ProjectExplorer::PeDevice *pTxIED = pInfoSetPair->GetTxIED();
            ProjectExplorer::PePort *pTxPort = pInfoSetPair->GetTxPort();
            ProjectExplorer::PeDevice *pRxIED = pInfoSetPair->GetRxIED();
            ProjectExplorer::PePort *pRxPort = pInfoSetPair->GetRxPort();
            ProjectExplorer::PeDevice *pSwitch1 = pInfoSetPair->GetSwitch1();
            ProjectExplorer::PePort *pSwitch1TxPort = pInfoSetPair->GetSwitch1TxPort();
            ProjectExplorer::PePort *pSwitch1RxPort = pInfoSetPair->GetSwitch1RxPort();
            ProjectExplorer::PeDevice *pSwitch2 = pInfoSetPair->GetSwitch2();
            ProjectExplorer::PePort *pSwitch2TxPort = pInfoSetPair->GetSwitch2TxPort();
            ProjectExplorer::PePort *pSwitch2RxPort = pInfoSetPair->GetSwitch2RxPort();
            ProjectExplorer::PeDevice *pSwitch3 = pInfoSetPair->GetSwitch3();
            ProjectExplorer::PePort *pSwitch3TxPort = pInfoSetPair->GetSwitch3TxPort();
            ProjectExplorer::PePort *pSwitch3RxPort = pInfoSetPair->GetSwitch3RxPort();
            ProjectExplorer::PeDevice *pSwitch4 = pInfoSetPair->GetSwitch4();
            ProjectExplorer::PePort *pSwitch4TxPort = pInfoSetPair->GetSwitch4TxPort();
            ProjectExplorer::PePort *pSwitch4RxPort = pInfoSetPair->GetSwitch4RxPort();

            ProjectExplorer::PeInfoSet *pInfoSetPairNew = new ProjectExplorer::PeInfoSet(*pInfoSetPair);
            pInfoSetPairNew->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            pInfoSetPairNew->SetGroup(iCurrrentGroup);
            pInfoSetPairNew->SetTxIED(pTxIED ? mapSrcToDstDevices.value(pTxIED, pTxIED) : 0);
            pInfoSetPairNew->SetTxPort(pTxPort ? mapSrcToDstPorts.value(pTxPort, pTxPort) : 0);
            pInfoSetPairNew->SetRxIED(pRxIED ? mapSrcToDstDevices.value(pRxIED, pRxIED) : 0);
            pInfoSetPairNew->SetRxPort(pRxPort ? mapSrcToDstPorts.value(pRxPort, pRxPort) : 0);
            pInfoSetPairNew->SetSwitch1(pSwitch1 ? mapSrcToDstDevices.value(pSwitch1, pSwitch1) : 0);
            pInfoSetPairNew->SetSwitch1TxPort(pSwitch1TxPort ? mapSrcToDstPorts.value(pSwitch1TxPort, pSwitch1TxPort) : 0);
            pInfoSetPairNew->SetSwitch1RxPort(pSwitch1RxPort ? mapSrcToDstPorts.value(pSwitch1RxPort, pSwitch1RxPort) : 0);
            pInfoSetPairNew->SetSwitch2(pSwitch2 ? mapSrcToDstDevices.value(pSwitch2, pSwitch2) : 0);
            pInfoSetPairNew->SetSwitch2TxPort(pSwitch2TxPort ? mapSrcToDstPorts.value(pSwitch2TxPort, pSwitch2TxPort) : 0);
            pInfoSetPairNew->SetSwitch2RxPort(pSwitch2RxPort ? mapSrcToDstPorts.value(pSwitch2RxPort, pSwitch2RxPort) : 0);
            pInfoSetPairNew->SetSwitch3(pSwitch3 ? mapSrcToDstDevices.value(pSwitch3, pSwitch3) : 0);
            pInfoSetPairNew->SetSwitch3TxPort(pSwitch3TxPort ? mapSrcToDstPorts.value(pSwitch3TxPort, pSwitch3TxPort) : 0);
            pInfoSetPairNew->SetSwitch3RxPort(pSwitch3RxPort ? mapSrcToDstPorts.value(pSwitch3RxPort, pSwitch3RxPort) : 0);
            pInfoSetPairNew->SetSwitch4(pSwitch4 ? mapSrcToDstDevices.value(pSwitch4, pSwitch4) : 0);
            pInfoSetPairNew->SetSwitch4TxPort(pSwitch4TxPort ? mapSrcToDstPorts.value(pSwitch4TxPort, pSwitch4TxPort) : 0);
            pInfoSetPairNew->SetSwitch4RxPort(pSwitch4RxPort ? mapSrcToDstPorts.value(pSwitch4RxPort, pSwitch4RxPort) : 0);

            pInfoSetPairNew->UpdateName(lstExtraInfoSetNames);
            if(!pInfoSetPairNew->GetName().isEmpty())
                lstExtraInfoSetNames.append(pInfoSetPairNew->GetName());

            if(pInfoSetPair->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
                AddIntInfoSetItem(pInfoSetPairNew);

            m_lstNewInfoSets.append(pInfoSetPairNew);
        }

        iCurrrentGroup++;
    }

    /////////////////////////////////////////////////////////////
    // Update External InfoSets
    /////////////////////////////////////////////////////////////
    lstAllInfoSets = pSrcProjectVersion->GetAllInfoSets();
    while(!lstAllInfoSets.isEmpty())
    {
        ProjectExplorer::PeInfoSet *pInfoSet = lstAllInfoSets.takeLast();

        ProjectExplorer::PeDevice *pTxIED = pInfoSet->GetTxIED();
        ProjectExplorer::PePort *pTxPort = pInfoSet->GetTxPort();
        ProjectExplorer::PeDevice *pRxIED = pInfoSet->GetRxIED();
        ProjectExplorer::PePort *pRxPort = pInfoSet->GetRxPort();
        ProjectExplorer::PeDevice *pSwitch1 = pInfoSet->GetSwitch1();
        ProjectExplorer::PePort *pSwitch1TxPort = pInfoSet->GetSwitch1TxPort();
        ProjectExplorer::PePort *pSwitch1RxPort = pInfoSet->GetSwitch1RxPort();
        ProjectExplorer::PeDevice *pSwitch2 = pInfoSet->GetSwitch2();
        ProjectExplorer::PePort *pSwitch2TxPort = pInfoSet->GetSwitch2TxPort();
        ProjectExplorer::PePort *pSwitch2RxPort = pInfoSet->GetSwitch2RxPort();
        ProjectExplorer::PeDevice *pSwitch3 = pInfoSet->GetSwitch3();
        ProjectExplorer::PePort *pSwitch3TxPort = pInfoSet->GetSwitch3TxPort();
        ProjectExplorer::PePort *pSwitch3RxPort = pInfoSet->GetSwitch3RxPort();
        ProjectExplorer::PeDevice *pSwitch4 = pInfoSet->GetSwitch4();
        ProjectExplorer::PePort *pSwitch4TxPort = pInfoSet->GetSwitch4TxPort();
        ProjectExplorer::PePort *pSwitch4RxPort = pInfoSet->GetSwitch4RxPort();

        if(!pTxIED || !pRxIED)
            continue;

        const bool bHasTxIED = mapSrcToDstDevices.contains(pTxIED);
        const bool bHasRxIED = mapSrcToDstDevices.contains(pRxIED);

        bool bExternal = (bHasTxIED != bHasRxIED);
        if(!bExternal && pSwitch1)
        {
            bExternal = (bHasTxIED != mapSrcToDstDevices.contains(pSwitch1));
            if(!bExternal && pSwitch2)
            {
                bExternal = (bHasTxIED != mapSrcToDstDevices.contains(pSwitch2));
                if(!bExternal && pSwitch3)
                {
                    bExternal = (bHasTxIED != mapSrcToDstDevices.contains(pSwitch3));
                    if(!bExternal && pSwitch4)
                        bExternal = (bHasTxIED != mapSrcToDstDevices.contains(pSwitch4));
                }
            }
        }

        if(!bExternal)
            continue;

        // Create InfoSet
        ProjectExplorer::PeInfoSet *pInfoSetNew = new ProjectExplorer::PeInfoSet(*pInfoSet);
        pInfoSetNew->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        pInfoSetNew->SetGroup(iCurrrentGroup);
        pInfoSetNew->SetTxIED(pTxIED ? mapSrcToDstDevices.value(pTxIED, pTxIED) : 0);
        pInfoSetNew->SetTxPort(pTxPort ? mapSrcToDstPorts.value(pTxPort, pTxPort) : 0);
        pInfoSetNew->SetRxIED(pRxIED ? mapSrcToDstDevices.value(pRxIED, pRxIED) : 0);
        pInfoSetNew->SetRxPort(pRxPort ? mapSrcToDstPorts.value(pRxPort, pRxPort) : 0);
        pInfoSetNew->SetSwitch1(pSwitch1 ? mapSrcToDstDevices.value(pSwitch1, pSwitch1) : 0);
        pInfoSetNew->SetSwitch1TxPort(pSwitch1TxPort ? mapSrcToDstPorts.value(pSwitch1TxPort, pSwitch1TxPort) : 0);
        pInfoSetNew->SetSwitch1RxPort(pSwitch1RxPort ? mapSrcToDstPorts.value(pSwitch1RxPort, pSwitch1RxPort) : 0);
        pInfoSetNew->SetSwitch2(pSwitch2 ? mapSrcToDstDevices.value(pSwitch2, pSwitch2) : 0);
        pInfoSetNew->SetSwitch2TxPort(pSwitch2TxPort ? mapSrcToDstPorts.value(pSwitch2TxPort, pSwitch2TxPort) : 0);
        pInfoSetNew->SetSwitch2RxPort(pSwitch2RxPort ? mapSrcToDstPorts.value(pSwitch2RxPort, pSwitch2RxPort) : 0);
        pInfoSetNew->SetSwitch3(pSwitch3 ? mapSrcToDstDevices.value(pSwitch3, pSwitch3) : 0);
        pInfoSetNew->SetSwitch3TxPort(pSwitch3TxPort ? mapSrcToDstPorts.value(pSwitch3TxPort, pSwitch3TxPort) : 0);
        pInfoSetNew->SetSwitch3RxPort(pSwitch3RxPort ? mapSrcToDstPorts.value(pSwitch3RxPort, pSwitch3RxPort) : 0);
        pInfoSetNew->SetSwitch4(pSwitch4 ? mapSrcToDstDevices.value(pSwitch4, pSwitch4) : 0);
        pInfoSetNew->SetSwitch4TxPort(pSwitch4TxPort ? mapSrcToDstPorts.value(pSwitch4TxPort, pSwitch4TxPort) : 0);
        pInfoSetNew->SetSwitch4RxPort(pSwitch4RxPort ? mapSrcToDstPorts.value(pSwitch4RxPort, pSwitch4RxPort) : 0);

        pInfoSetNew->UpdateName(lstExtraInfoSetNames);
        if(!pInfoSetNew->GetName().isEmpty())
            lstExtraInfoSetNames.append(pInfoSetNew->GetName());

        if(pInfoSet->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
            AddExtInfoSetItem(pInfoSetNew);

        m_lstNewInfoSets.append(pInfoSetNew);

        // Create Pair InfoSet
        if(ProjectExplorer::PeInfoSet *pInfoSetPair = pInfoSet->GetPairInfoSet())
        {
            lstAllInfoSets.removeAll(pInfoSetPair);

            ProjectExplorer::PeDevice *pTxIED = pInfoSetPair->GetTxIED();
            ProjectExplorer::PePort *pTxPort = pInfoSetPair->GetTxPort();
            ProjectExplorer::PeDevice *pRxIED = pInfoSetPair->GetRxIED();
            ProjectExplorer::PePort *pRxPort = pInfoSetPair->GetRxPort();
            ProjectExplorer::PeDevice *pSwitch1 = pInfoSetPair->GetSwitch1();
            ProjectExplorer::PePort *pSwitch1TxPort = pInfoSetPair->GetSwitch1TxPort();
            ProjectExplorer::PePort *pSwitch1RxPort = pInfoSetPair->GetSwitch1RxPort();
            ProjectExplorer::PeDevice *pSwitch2 = pInfoSetPair->GetSwitch2();
            ProjectExplorer::PePort *pSwitch2TxPort = pInfoSetPair->GetSwitch2TxPort();
            ProjectExplorer::PePort *pSwitch2RxPort = pInfoSetPair->GetSwitch2RxPort();
            ProjectExplorer::PeDevice *pSwitch3 = pInfoSetPair->GetSwitch3();
            ProjectExplorer::PePort *pSwitch3TxPort = pInfoSetPair->GetSwitch3TxPort();
            ProjectExplorer::PePort *pSwitch3RxPort = pInfoSetPair->GetSwitch3RxPort();
            ProjectExplorer::PeDevice *pSwitch4 = pInfoSetPair->GetSwitch4();
            ProjectExplorer::PePort *pSwitch4TxPort = pInfoSetPair->GetSwitch4TxPort();
            ProjectExplorer::PePort *pSwitch4RxPort = pInfoSetPair->GetSwitch4RxPort();

            ProjectExplorer::PeInfoSet *pInfoSetPairNew = new ProjectExplorer::PeInfoSet(*pInfoSetPair);
            pInfoSetPairNew->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            pInfoSetPairNew->SetGroup(iCurrrentGroup);
            pInfoSetPairNew->SetTxIED(pTxIED ? mapSrcToDstDevices.value(pTxIED, pTxIED) : 0);
            pInfoSetPairNew->SetTxPort(pTxPort ? mapSrcToDstPorts.value(pTxPort, pTxPort) : 0);
            pInfoSetPairNew->SetRxIED(pRxIED ? mapSrcToDstDevices.value(pRxIED, pRxIED) : 0);
            pInfoSetPairNew->SetRxPort(pRxPort ? mapSrcToDstPorts.value(pRxPort, pRxPort) : 0);
            pInfoSetPairNew->SetSwitch1(pSwitch1 ? mapSrcToDstDevices.value(pSwitch1, pSwitch1) : 0);
            pInfoSetPairNew->SetSwitch1TxPort(pSwitch1TxPort ? mapSrcToDstPorts.value(pSwitch1TxPort, pSwitch1TxPort) : 0);
            pInfoSetPairNew->SetSwitch1RxPort(pSwitch1RxPort ? mapSrcToDstPorts.value(pSwitch1RxPort, pSwitch1RxPort) : 0);
            pInfoSetPairNew->SetSwitch2(pSwitch2 ? mapSrcToDstDevices.value(pSwitch2, pSwitch2) : 0);
            pInfoSetPairNew->SetSwitch2TxPort(pSwitch2TxPort ? mapSrcToDstPorts.value(pSwitch2TxPort, pSwitch2TxPort) : 0);
            pInfoSetPairNew->SetSwitch2RxPort(pSwitch2RxPort ? mapSrcToDstPorts.value(pSwitch2RxPort, pSwitch2RxPort) : 0);
            pInfoSetPairNew->SetSwitch3(pSwitch3 ? mapSrcToDstDevices.value(pSwitch3, pSwitch3) : 0);
            pInfoSetPairNew->SetSwitch3TxPort(pSwitch3TxPort ? mapSrcToDstPorts.value(pSwitch3TxPort, pSwitch3TxPort) : 0);
            pInfoSetPairNew->SetSwitch3RxPort(pSwitch3RxPort ? mapSrcToDstPorts.value(pSwitch3RxPort, pSwitch3RxPort) : 0);
            pInfoSetPairNew->SetSwitch4(pSwitch4 ? mapSrcToDstDevices.value(pSwitch4, pSwitch4) : 0);
            pInfoSetPairNew->SetSwitch4TxPort(pSwitch4TxPort ? mapSrcToDstPorts.value(pSwitch4TxPort, pSwitch4TxPort) : 0);
            pInfoSetPairNew->SetSwitch4RxPort(pSwitch4RxPort ? mapSrcToDstPorts.value(pSwitch4RxPort, pSwitch4RxPort) : 0);

            pInfoSetPairNew->UpdateName(lstExtraInfoSetNames);
            if(!pInfoSetPairNew->GetName().isEmpty())
                lstExtraInfoSetNames.append(pInfoSetPairNew->GetName());

            if(pInfoSetPair->GetInfoSetType() != ProjectExplorer::PeInfoSet::itNone)
                AddExtInfoSetItem(pInfoSetPairNew);

            m_lstNewInfoSets.append(pInfoSetPairNew);
        }

        iCurrrentGroup++;
    }

    /////////////////////////////////////////////////////////////
    // Update Internal VTerminalConns
    /////////////////////////////////////////////////////////////
    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, pSrcProjectVersion->GetAllVTerminalConns())
    {
        ProjectExplorer::PeVTerminal *pTxVTerminal = pVTerminalConn->GetTxVTerminal();
        ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();
        if(!pTxVTerminal || !pRxVTerminal)
            continue;

        ProjectExplorer::PeDevice *pTxDevice = pTxVTerminal->GetParentDevice();
        ProjectExplorer::PeDevice *pRxDevice = pRxVTerminal->GetParentDevice();
        if(!pTxDevice || !pRxDevice)
            continue;

        ProjectExplorer::PeStrap *pTxStrap = pVTerminalConn->GetTxStrap();
        ProjectExplorer::PeStrap *pRxStrap = pVTerminalConn->GetRxStrap();

        if(!(mapSrcToDstDevices.contains(pTxDevice) && mapSrcToDstDevices.contains(pRxDevice)))
            continue;

        ProjectExplorer::PeVTerminalConn *pVTerminalConnNew = new ProjectExplorer::PeVTerminalConn(*pVTerminalConn);
        pVTerminalConnNew->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        pVTerminalConnNew->SetTxVTerminal(pTxVTerminal ? mapSrcToDstVTerminals.value(pTxVTerminal, pTxVTerminal) : 0);
        pVTerminalConnNew->SetTxStrap(pTxStrap ? mapSrcToDstStraps.value(pTxStrap, pTxStrap) : 0);
        pVTerminalConnNew->SetRxVTerminal(pRxVTerminal ? mapSrcToDstVTerminals.value(pRxVTerminal, pRxVTerminal) : 0);
        pVTerminalConnNew->SetRxStrap(pRxStrap ? mapSrcToDstStraps.value(pRxStrap, pRxStrap) : 0);
        AddIntVTerminalConnItem(pVTerminalConnNew);

        m_lstNewVTerminalConns.append(pVTerminalConnNew);
    }

    /////////////////////////////////////////////////////////////
    // Update External VTerminalConns
    /////////////////////////////////////////////////////////////
    foreach(ProjectExplorer::PeVTerminalConn *pVTerminalConn, pSrcProjectVersion->GetAllVTerminalConns())
    {
        ProjectExplorer::PeVTerminal *pTxVTerminal = pVTerminalConn->GetTxVTerminal();
        ProjectExplorer::PeVTerminal *pRxVTerminal = pVTerminalConn->GetRxVTerminal();
        if(!pTxVTerminal || !pRxVTerminal)
            continue;

        ProjectExplorer::PeDevice *pTxDevice = pTxVTerminal->GetParentDevice();
        ProjectExplorer::PeDevice *pRxDevice = pRxVTerminal->GetParentDevice();
        if(!pTxDevice || !pRxDevice)
            continue;

        ProjectExplorer::PeStrap *pTxStrap = pVTerminalConn->GetTxStrap();
        ProjectExplorer::PeStrap *pRxStrap = pVTerminalConn->GetRxStrap();

        if(mapSrcToDstDevices.contains(pTxDevice) == mapSrcToDstDevices.contains(pRxDevice))
            continue;

        ProjectExplorer::PeVTerminalConn *pVTerminalConnNew = new ProjectExplorer::PeVTerminalConn(*pVTerminalConn);
        pVTerminalConnNew->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        pVTerminalConnNew->SetTxVTerminal(pTxVTerminal ? mapSrcToDstVTerminals.value(pTxVTerminal, pTxVTerminal) : 0);
        pVTerminalConnNew->SetTxStrap(pTxStrap ? mapSrcToDstStraps.value(pTxStrap, pTxStrap) : 0);
        pVTerminalConnNew->SetRxVTerminal(pRxVTerminal ? mapSrcToDstVTerminals.value(pRxVTerminal, pRxVTerminal) : 0);
        pVTerminalConnNew->SetRxStrap(pRxStrap ? mapSrcToDstStraps.value(pRxStrap, pRxStrap) : 0);
        AddExtVTerminalConnItem(pVTerminalConnNew);

        m_lstNewVTerminalConns.append(pVTerminalConnNew);
    }

    UpdateInfosetItemStatus();
    UpdateVTerminalConnItemStatus();
}

void CopyBayDlg::SlotExgInfoSetDoubleClicked(const QModelIndex &index)
{
    if(!m_pModel->GetValid())
    {
        if(QMessageBox::question(this,
                                 tr("Modify Confirmation"),
                                 tr("There are unresolved errors for copied object, the modified will be lost after correction, are you sure you want to continue?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
    }

    QStandardItem *pItem = m_pModelExtInfoSet->itemFromIndex(index);
    ProjectExplorer::PeInfoSet *pInfoSet = reinterpret_cast<ProjectExplorer::PeInfoSet*>(pItem->data().toInt());
    if(!pInfoSet)
        return;

    ProjectExplorer::PeInfoSet *pInfoSetPair = 0;
    foreach(ProjectExplorer::PeInfoSet *pI, m_lstNewInfoSets)
    {
        if((pI != pInfoSet) && (pI->GetGroup() == pInfoSet->GetGroup()))
        {
            pInfoSetPair = pI;
            break;
        }
    }
    if(!pInfoSetPair)
        return;

    InfoSetDlg dlg(pInfoSet, pInfoSetPair, m_lstNewInfoSets, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    UpdateExternalInfoset(pInfoSet);
    UpdateExternalInfoset(pInfoSetPair);
    foreach(ProjectExplorer::PeInfoSet *pInfoSetAffected, dlg.GetAffectedInfoSets())
        UpdateExternalInfoset(pInfoSetAffected);

    UpdateInfosetItemStatus();
}

void CopyBayDlg::SlotExtVTerminalConnDoubleClicked(const QModelIndex &index)
{
    if(!m_pModel->GetValid())
    {
        if(QMessageBox::question(this,
                                 tr("Modify Confirmation"),
                                 tr("There are unresolved errors for copied object, the modified will be lost after correction, are you sure you want to continue?"),
                                 QMessageBox::Yes | QMessageBox::No,
                                 QMessageBox::No) != QMessageBox::Yes)
        {
            return;
        }
    }

    QStandardItem *pItem = m_pModelExtVTerminalConn->itemFromIndex(index);
    ProjectExplorer::PeVTerminalConn *pVTerminalConn = reinterpret_cast<ProjectExplorer::PeVTerminalConn*>(pItem->data().toInt());
    if(!pVTerminalConn)
        return;

    VTerminalConnDlg dlg(pVTerminalConn, this);
    if(dlg.exec() != QDialog::Accepted)
        return;

    UpdateExternalVTerminalConn(pVTerminalConn);

    UpdateVTerminalConnItemStatus();
}

void CopyBayDlg::SlotInfoSetChanged(QStandardItem *pItem)
{
    if(!pItem || pItem->column() != 0)
        return;

    ProjectExplorer::PeInfoSet *pInfoSet = reinterpret_cast<ProjectExplorer::PeInfoSet*>(pItem->data().toInt());
    if(!pInfoSet)
        return;

    ProjectExplorer::PeInfoSet *pInfoSetPair = 0;
    foreach(ProjectExplorer::PeInfoSet *pI, m_lstNewInfoSets)
    {
        if((pI != pInfoSet) && (pI->GetGroup() == pInfoSet->GetGroup()))
        {
            pInfoSetPair = pI;
            break;
        }
    }
    if(!pInfoSetPair)
        return;

    if(QStandardItem *pItemPair = m_mapIntInfoSetToItem.value(pInfoSetPair, 0))
        pItemPair->setCheckState(pItem->checkState());

    if(QStandardItem *pItemPair = m_mapExtInfoSetToItem.value(pInfoSetPair, 0))
        pItemPair->setCheckState(pItem->checkState());

    UpdateInfosetItemStatus();
}

void CopyBayDlg::SlotVTerminalConnChanged(QStandardItem *pItem)
{
    if(!pItem || pItem->column() != 0)
        return;

    UpdateVTerminalConnItemStatus();
}

void CopyBayDlg::SlotActionSelectAll()
{
    if(QAction *pAction = qobject_cast<QAction*>(sender()))
    {
        if(QStandardItemModel *pModel = reinterpret_cast<QStandardItemModel*>(pAction->data().toInt()))
        {
            for(int i = 0; i < pModel->rowCount(); i++)
                pModel->item(i, 0)->setCheckState(Qt::Checked);
        }
    }
}

void CopyBayDlg::SlotActionDeselectAll()
{
    if(QAction *pAction = qobject_cast<QAction*>(sender()))
    {
        if(QStandardItemModel *pModel = reinterpret_cast<QStandardItemModel*>(pAction->data().toInt()))
        {
            for(int i = 0; i < pModel->rowCount(); i++)
                pModel->item(i, 0)->setCheckState(Qt::Unchecked);
        }
    }
}
