#include <QDir>
#include <QFileInfo>
#include <QTextCodec>
#include <QTranslator>
#include <QMessageBox>
#include <QApplication>
#include <QMainWindow>
#include <QXmlStreamReader>

#include "sclparser/scldoccontrol.h"
#include "projectexplorer/databasemanager.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peprojectversion.h"

#include "mainwindow.h"

void ReadConfig(int &iDefaultProjectId, QString &strHost, QString &strUser, QString &strPassword)
{
    const QFileInfo FileInfo(QCoreApplication::applicationDirPath() + "/../../sys/settings.xml");
    QFile file(FileInfo.absoluteFilePath());
    if(!file.open(QIODevice::ReadOnly))
        return;

    QString strConnectionString;
    QXmlStreamReader reader(&file);
    if(reader.readNextStartElement())
    {
        if(reader.name() == "configuration")
        {
            while(reader.readNextStartElement())
            {
                if(reader.name() == "connection_string")
                {
                    if(reader.attributes().value("used").toString() == "1")
                    {
                        iDefaultProjectId = reader.attributes().value("projectid").toString().toInt();
                        strConnectionString = reader.readElementText();
                    }
                    else
                    {
                        reader.skipCurrentElement();
                    }
                }
                else
                {
                    reader.skipCurrentElement();
                }
            }
        }
    }

    if(iDefaultProjectId < 0 || strConnectionString.isEmpty())
        return;

    QStringList lstConnectionStrings = strConnectionString.split(QLatin1Char(';'));
    foreach(const QString &strSection, lstConnectionStrings)
    {
        QStringList lstSections = strSection.split(QLatin1Char('='));
        if(lstSections.size() != 2)
            continue;

        const QString strType = lstSections.first().trimmed();
        if(strType.compare("Data Source", Qt::CaseInsensitive) == 0)
            strHost = lstSections.at(1).trimmed();
        else if(strType.compare("User Id", Qt::CaseInsensitive) == 0)
            strUser = lstSections.at(1).trimmed();
        else if(strType.compare("Password", Qt::CaseInsensitive) == 0)
            strPassword = lstSections.at(1).trimmed();
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins/");

#if QT_VERSION < 0x050000
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
#endif

    QDir dirLangurage(QCoreApplication::applicationDirPath() + QLatin1String("/../share/translations/zh_CN"));
    if(dirLangurage.exists())
    {
        foreach(const QFileInfo &FileInfo, dirLangurage.entryInfoList(QStringList() << "*.qm", QDir::Files))
        {
            QTranslator *pTranslator = new QTranslator;
            if(pTranslator->load(FileInfo.baseName(), dirLangurage.absolutePath()))
                app.installTranslator(pTranslator);
        }
    }

    int iDefaultProjectId = -1;
    QString strHost = "localhost";
    QString strUser = "root";
    QString strPassword = "ssdt";
    ReadConfig(iDefaultProjectId, strHost, strUser, strPassword);

    ProjectExplorer::DatabaseManager manager;
    if(!manager.Initialize(strHost,
                           strUser,
                           strPassword,
                           "ssdt_base",
                           "ssdt_project"))
    {
        QMessageBox::critical(0, "Error", "Failed to connect database");
        return 1;
    }

    SclParser::SCLDocControl SclDoc;
    const QString strSchemaFileFullPath = QCoreApplication::applicationDirPath() + QLatin1String("/../share/schema/SCL.xsd");
    if(!SclDoc.RegisterSchema(strSchemaFileFullPath))
    {
        QMessageBox::critical(0, "Error", QString("Failed to register schema file '%1'").arg(QDir::toNativeSeparators(strSchemaFileFullPath)));
        return 1;
    }

    if(iDefaultProjectId >= 0)
    {
        if(ProjectExplorer::PeProjectVersion *pProjectVersion = ProjectExplorer::ProjectManager::Instance()->FindProjectVersionById(iDefaultProjectId))
            pProjectVersion->Open();
    }

    MainWindow window;
    window.Initialize();

    return app.exec();
}
