TEMPLATE    = lib
TARGET      = styledui
DEFINES     += STYLEDUI_LIBRARY
QT          += gui

include(../libs.pri)
include(../../shared/translations/translations.pri)

HEADERS     += styledui_global.h \
               stylehelper.h \
               styledbutton.h \
               styledbar.h \
               styledwidget.h \
               styledtitlebar.h \
               styledwindow.h \
               styleddialog.h

SOURCES     += stylehelper.cpp \
               styledbutton.cpp \
               styledbar.cpp \
               styledwidget.cpp \
               styledtitlebar.cpp \
               styledwindow.cpp \
               styleddialog.cpp

RESOURCES   += styledui.qrc

FORMS +=
