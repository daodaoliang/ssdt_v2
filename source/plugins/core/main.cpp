#include <QApplication>
#include <QTextCodec>
#include <QMessageBox>
#include <QDir>
#include <QSettings>
#include <QTranslator>

#include "sclparser/scldoccontrol.h"
#include "projectexplorer/projectmanager.h"

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins/");

    QDir dirLangurage(QCoreApplication::applicationDirPath() + QLatin1String("/../share/translations/") + QLocale::system().name());
    if(dirLangurage.exists())
    {
        foreach(const QFileInfo &FileInfo, dirLangurage.entryInfoList(QStringList() << "*.qm", QDir::Files))
        {
            QTranslator *pTranslator = new QTranslator;
            if(pTranslator->load(FileInfo.baseName(), dirLangurage.absolutePath()))
                app.installTranslator(pTranslator);
        }
    }

    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

    ProjectExplorer::ProjectManager manager;
    if(!manager.Initialize("localhost",
                           "root",
                           "dctss",
                           "dctss_base",
                           "dctss_project",
                           QCoreApplication::applicationDirPath() + QLatin1String("/../share/templates/db_project/dctss_project.sql")))
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

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QLatin1String("ssdt"), QLatin1String("ssdt"), 0);
    MainWindow window(&settings);
    if(!window.Initialize())
        return -1;

    window.showMaximized();
    return app.exec();
}
