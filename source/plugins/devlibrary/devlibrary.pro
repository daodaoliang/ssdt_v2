TEMPLATE        = lib
TARGET          = devlibrary
DEFINES         += DEVLIBRARY_LIBRARY

include(../plugin.pri)
include(devlibrary_dependencies.pri)
include(../../shared/translations/translations.pri)

HEADERS         += devlibrary_global.h \
                   devlibrary_constants.h \
                   devlibraryplugin.h \
                   devlibraryexplorer.h \
                   devlibraryexplorerwidget.h \
                   propertydlglibdevice.h \
                   propertydlglibswitch.h \
                   propertydlglibboard.h \
                   propertydlglibfiberport.h

SOURCES         += devlibraryplugin.cpp \
                   devlibraryexplorer.cpp \
                   devlibraryexplorerwidget.cpp \
                   propertydlglibdevice.cpp \
                   propertydlglibswitch.cpp \
                   propertydlglibboard.cpp \
                   propertydlglibfiberport.cpp

RESOURCES       += devlibrary.qrc

OTHER_FILES     += devlibrary.pluginspec











































