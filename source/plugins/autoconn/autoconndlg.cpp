#include <QApplication>
#include <QFormLayout>
#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QAction>
#include <QFile>
#include <QDir>
#include <QXmlStreamWriter>
#include <QProcess>

#include "utils/waitcursor.h"
#include "utils/searchcombobox.h"
#include "sclparser/scldoccontrol.h"
#include "sclparser/scldocument.h"
#include "sclparser/sclelement.h"
#include "sclparser/sclparser_constants.h"
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
#include "core/mainwindow.h"

#include "autoconndlg.h"

using namespace AutoConn::Internal;

AutoConnDlg::AutoConnDlg(QWidget *pParent) : StyledUi::StyledDialog(pParent)
{
    SetWindowTitle(tr("Automatical connection"));

    m_pComboBoxProject = new QComboBox(this);
    m_pComboBoxProject->setEditable(false);
    m_pComboBoxProject->setMinimumWidth(300);

    m_pTextEdit = new QTextEdit(this);

    foreach(ProjectExplorer::PeProject *pProject, ProjectExplorer::ProjectManager::Instance()->GetAllProjects())
    {
        foreach(ProjectExplorer::PeProjectVersion *pProjectVersion, pProject->GetAllProjectVersions())
        {
            if(pProjectVersion->IsOpend())
            {
                m_pComboBoxProject->addItem(pProjectVersion->GetDisplayIcon(), pProjectVersion->GetDisplayName(), reinterpret_cast<int>(pProjectVersion));
                m_pComboBoxProject->addItem(pProjectVersion->GetDisplayIcon(), pProjectVersion->GetDisplayName(), reinterpret_cast<int>(pProjectVersion));
            }
        }
    }

    QGroupBox *pGroupBoxProject = new QGroupBox(this);
    QFormLayout *pFormLayoutProject = new QFormLayout(pGroupBoxProject);
    pFormLayoutProject->addRow(tr("Select Project:"), m_pComboBoxProject);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    QPushButton *pPushButtonExecute = pDialogButtonBox->addButton(tr("Execute"), QDialogButtonBox::ActionRole);
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(pPushButtonExecute, SIGNAL(clicked()), this, SLOT(SlotActionExecute()));

    QVBoxLayout *pVBoxLayout = GetClientLayout();
    pVBoxLayout->setContentsMargins(10, 10, 10, 10);
    pVBoxLayout->addWidget(pGroupBoxProject);
    pVBoxLayout->addSpacing(10);
    pVBoxLayout->addWidget(m_pTextEdit);
    pVBoxLayout->addSpacing(15);
    pVBoxLayout->addWidget(pDialogButtonBox);

}

AutoConnDlg::~AutoConnDlg()
{
}

QSize AutoConnDlg::sizeHint() const
{
    return QSize(1050, 600);
}

bool AutoConnDlg::ExportBayConfig(ProjectExplorer::PeProjectVersion *pProjectVersion, const QString &strFileName)
{
    if(!pProjectVersion)
        return false;

    QFile file(strFileName);
    if(!file.open(QIODevice::WriteOnly))
        return false;

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(2);
    xmlWriter.setCodec("UTF-8");

    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement(QLatin1String("CONFIG"));

    foreach(ProjectExplorer::PeBay *pBay, pProjectVersion->GetAllBays())
    {
        xmlWriter.writeStartElement(QLatin1String("Bay"));

        xmlWriter.writeAttribute(QLatin1String("id"), QString::number(pBay->GetId()));
        xmlWriter.writeAttribute(QLatin1String("name"), pBay->GetName());
        xmlWriter.writeAttribute(QLatin1String("number"), pBay->GetNumber());
        xmlWriter.writeAttribute(QLatin1String("vlevel"), QString::number(pBay->GetVLevel()));

        foreach(ProjectExplorer::PeDevice *pDevice, pBay->GetChildDevices())
        {
            if(!pDevice->GetHasModel())
                continue;

            xmlWriter.writeStartElement(QLatin1String("IED"));
            xmlWriter.writeAttribute(QLatin1String("iedName"), pDevice->GetName());
            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
    }

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();

    file.close();
    return true;
}

void AutoConnDlg::SlotActionExecute()
{
    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    int iIndex = m_pComboBoxProject->currentIndex();
    if(iIndex < 0)
        return;

    ProjectExplorer::PeProjectVersion *pProjectVersion = reinterpret_cast<ProjectExplorer::PeProjectVersion*>(m_pComboBoxProject->itemData(iIndex).toInt());
    if(!pProjectVersion)
        return;

    QDir dirAutoLink(QCoreApplication::applicationDirPath() + QLatin1String("/../../template/autolink"));
    if(!dirAutoLink.exists() && !dirAutoLink.mkpath(dirAutoLink.absolutePath()))
        return;

    if(!Core::ConfigCoreOperation::Instance()->ExportToScd(pProjectVersion, dirAutoLink.absoluteFilePath(QLatin1String("project.scd")), false, false))
        return;

    if(!ExportBayConfig(pProjectVersion, dirAutoLink.absoluteFilePath(QLatin1String("bay.xml"))))
        return;

    QProcess process;
    process.start(QCoreApplication::applicationDirPath() + QLatin1String("/../../autoconnect/bin/autoconnect.exe"));
    for(int i = 0; i < 600; i++)
    {
        if(process.waitForFinished(300))
            break;
    }

    if(!Core::ConfigCoreOperation::Instance()->ImportFromScd(pProjectVersion, dirAutoLink.absoluteFilePath(QLatin1String("link.scd"))))
        return;

    QFile file(dirAutoLink.absoluteFilePath(QLatin1String("link.txt")));
    if(!file.open(QFile::ReadOnly))
        return;

    QTextStream TextStream(&file);
    QString strLine;
    while(TextStream.readLineInto(&strLine))
        m_pTextEdit->append(strLine);
}
