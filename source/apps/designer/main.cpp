#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <QTextCodec>
#include <QTranslator>
#include <QMessageBox>
#include <QApplication>
#include <QMainWindow>
#include <QXmlStreamReader>

#include "extensionsystem/pluginmanager.h"
#include "extensionsystem/pluginspec.h"
#include "extensionsystem/iplugin.h"
#include "sclparser/scldoccontrol.h"
#include "projectexplorer/databasemanager.h"
#include "projectexplorer/projectmanager.h"
#include "projectexplorer/peprojectversion.h"

static inline QStringList getPluginPaths()
{
    QStringList rc;
    // Figure out root:  Up one from 'bin'
    QDir rootDir = QApplication::applicationDirPath();
    const QString rootDirPath = rootDir.canonicalPath();
    QString pluginPath = rootDirPath;
    pluginPath += QLatin1Char('/');
    pluginPath += QLatin1String("plugins");
    rc.push_back(pluginPath);
    return rc;
}

static inline QString msgCoreLoadFailure(const QString &why)
{
    return QCoreApplication::translate("Application", "Failed to load core: %1").arg(why);
}

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
    SclDoc.SetToolId("Smart Designer");

    ExtensionSystem::PluginManager pluginManager;
    pluginManager.setFileExtension(QLatin1String("pluginspec"));

    const QStringList pluginPaths = getPluginPaths();
    pluginManager.setPluginPaths(pluginPaths);

    const QList<ExtensionSystem::PluginSpec *> plugins = pluginManager.plugins();
    ExtensionSystem::PluginSpec *pPluginSpecCore = 0;
    foreach(ExtensionSystem::PluginSpec *pPluginSpec, plugins)
    {
        if(pPluginSpec->name() == QLatin1String("core"))
        {
            pPluginSpecCore = pPluginSpec;
            break;
        }
    }

    if(!pPluginSpecCore)
    {
        QString nativePaths = QDir::toNativeSeparators(pluginPaths.join(QLatin1String(",")));
        const QString reason = QCoreApplication::translate("Application", "Could not find 'Core.pluginspec' in %1").arg(nativePaths);
        QMessageBox::critical(0, QLatin1String("Smart Designer"), msgCoreLoadFailure(reason));

        return 1;
    }
    if(pPluginSpecCore->hasError())
    {
        QMessageBox::critical(0, QLatin1String("Smart Designer"), msgCoreLoadFailure(pPluginSpecCore->errorString()));
        return 1;
    }

    pluginManager.loadPlugins();
    if(pPluginSpecCore->hasError())
    {
        QMessageBox::critical(0, QLatin1String("Smart Designer"), msgCoreLoadFailure(pPluginSpecCore->errorString()));
        return 1;
    }

    {
        QStringList errors;
        foreach (ExtensionSystem::PluginSpec *p, pluginManager.plugins())
            // only show errors on startup if plugin is enabled.
            if (p->hasError() && p->isEnabled() && !p->isDisabledIndirectly())
                errors.append(p->name() + "\n" + p->errorString());
        if (!errors.isEmpty())
            QMessageBox::warning(0,
                QCoreApplication::translate("Application", "Qt Creator - Plugin loader messages"),
                errors.join(QString::fromLatin1("\n\n")));
    }

    if(iDefaultProjectId >= 0)
    {
        if(ProjectExplorer::PeProjectVersion *pProjectVersion = ProjectExplorer::ProjectManager::Instance()->FindProjectVersionById(iDefaultProjectId))
            pProjectVersion->Open();
    }

    return app.exec();
}
