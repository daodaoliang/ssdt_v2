TEMPLATE        = lib
TARGET          = core
DEFINES         += CORE_LIBRARY
QT              += xml sql

include(../plugin.pri)
include(core_dependencies.pri)
include(../../shared/translations/translations.pri)

HEADERS         += core_global.h \
                   core_constants.h \
                   coreplugin.h \
                   mainwindow.h \
                   sidetabbar.h \
                   maintabbar.h \
                   explorerwidget.h \
                   iexplorer.h \
                   projectexplorer.h \
                   projectexplorerwidget.h \
                   projectexplorermodel.h \
                   imode.h \
                   uniqueidmanager.h \
                   actioncontainer.h \
                   actionmanager.h \
                   command.h \
                   actionmanager_p.h \
                   command_p.h \
                   actioncontainer_p.h \
                   configmodelview.h \
                   configwidget.h \
                   ioptionspage.h \
                   settingsdialog.h \
                   configcoreoperation.h \
                   openprojectdlg.h \
                   copyvconndlg.h \
                   vterminaldefine.h \
                   vterminalsettings.h \
                   managemodeldlg.h

SOURCES         += coreplugin.cpp \
                   mainwindow.cpp \
                   sidetabbar.cpp \
                   maintabbar.cpp \
                   explorerwidget.cpp \
                   projectexplorer.cpp \
                   projectexplorerwidget.cpp \
                   projectexplorermodel.cpp \
                   uniqueidmanager.cpp \
                   actionmanager_p.cpp \
                   command_p.cpp \
                   actioncontainer_p.cpp \
                   configmodelview.cpp \
                   configwidget.cpp \
                   settingsdialog.cpp \
                   configcoreoperation.cpp \
                   openprojectdlg.cpp \
                   copyvconndlg.cpp \
                   vterminaldefine.cpp \
                   vterminalsettings.cpp \
                   managemodeldlg.cpp

RESOURCES       += core.qrc

OTHER_FILES     += core.pluginspec
