TEMPLATE    = lib
TARGET      = vlinkparse
DEFINES     += SCD_VLINK_DATA_EXPORT
QT          += xml

include(../libs.pri)

HEADERS     += vlinkdata.h \
               vlinkselectdlg.h

SOURCES     += vlinkdata.cpp \
               vlinkselectdlg.cpp

FORMS       += dlg_selectvlink.ui
