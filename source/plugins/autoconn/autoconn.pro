TEMPLATE        = lib
TARGET          = autoconn
DEFINES         += AUTOCONN_LIBRARY

include(../plugin.pri)
include(autoconn_dependencies.pri)
include(../../shared/translations/translations.pri)

HEADERS         += autoconn_global.h \
                   autoconn_constants.h \
                   autoconnplugin.h \
                   autoconndlg.h

SOURCES         += autoconnplugin.cpp \
                   autoconndlg.cpp

OTHER_FILES     += autoconn.pluginspec











































