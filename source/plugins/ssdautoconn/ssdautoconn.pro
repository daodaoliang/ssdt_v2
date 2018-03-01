TEMPLATE        = lib
TARGET          = ssdautoconn
DEFINES         += SSDAUTOCONN_LIBRARY

include(../plugin.pri)
include(ssdautoconn_dependencies.pri)
include(../../shared/translations/translations.pri)

HEADERS         += ssdautoconn_global.h \
                   ssdautoconn_constants.h \
                   ssdautoconnplugin.h \
                   ssdautoconndlg.h

SOURCES         += ssdautoconnplugin.cpp \
                   ssdautoconndlg.cpp

OTHER_FILES     += ssdautoconn.pluginspec











































