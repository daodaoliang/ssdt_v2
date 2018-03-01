TEMPLATE        = lib
TARGET          = baycopy
DEFINES         += BAYCOPY_LIBRARY

include(../plugin.pri)
include(baycopy_dependencies.pri)
include(../../shared/translations/translations.pri)

HEADERS         += baycopy_global.h \
                   baycopy_constants.h \
                   baycopyplugin.h \
                   copybaymodelview.h \
                   copybaydlg.h \
                   selectbaydlg.h \
                   selectcubicledlg.h \
                   selectdevicedlg.h \
                   vterminalconndlg.h \
                   infosetdlg.h

SOURCES         += baycopyplugin.cpp \
                   copybaymodelview.cpp \
                   copybaydlg.cpp \
                   selectbaydlg.cpp \
                   selectcubicledlg.cpp\
                   selectdevicedlg.cpp \
                   vterminalconndlg.cpp \
                   infosetdlg.cpp

RESOURCES       += baycopy.qrc

OTHER_FILES     += baycopy.pluginspec











































