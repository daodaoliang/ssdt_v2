include(../apps.pri)
include(../../shared/qtlibs/sclparser.pri)
include(../../shared/qtlibs/utils.pri)
include(../../shared/translations/translations.pri)

QT          += sql
TARGET      = label

CONFIG(debug, debug|release):LIBS *= -lstyleduid -lprojectexplorerd
else:LIBS *= -lstyledui -lprojectexplorer

HEADERS     += mainwindow.h \
               explorerwidget.h \
               cablewidget.h \
               openprojectdlg.h

SOURCES     += main.cpp \
               mainwindow.cpp \
               explorerwidget.cpp \
               cablewidget.cpp \
               openprojectdlg.cpp

RESOURCES   += label.qrc
RC_FILE     = label.rc
