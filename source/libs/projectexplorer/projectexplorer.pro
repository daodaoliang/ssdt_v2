TEMPLATE    = lib
TARGET      = projectexplorer
DEFINES     += PROJECTEXPLORER_LIBRARY

include(../libs.pri)
include(projectexplorer_dependencies.pri)
include(../../shared/translations/translations.pri)

HEADERS     += projectexplorer_global.h \
               databasemanager.h \
               projectmanager.h \
               peprojectobject.h \
               peproject.h \
               peprojectversion.h \
               peroom.h \
               pecubicle.h \
               pebay.h \
               pedevice.h \
               peboard.h \
               peport.h \
               peinfoset.h \
               pecubicleconn.h \
               pevterminal.h \
               pestrap.h \
               pevterminalconn.h \
               pecable.h \
               pefiber.h \
               pevolume.h \
               pedwg.h \
               basemanager.h \
               pbbaseobject.h \
               pbdevicecategory.h \
               pbbaycategory.h \
               pbvlevel.h \
               pbmanufacture.h \
               pbdevicetype.h \
               pblibdevice.h \
               pblibboard.h \
               pblibport.h \
               pbtpbay.h \
               pbtpdevice.h \
               pbtpport.h \
               pbtpinfoset.h

SOURCES     += databasemanager.cpp \
               projectmanager.cpp \
               peprojectobject.cpp \
               peproject.cpp \
               peprojectversion.cpp \
               peroom.cpp \
               pecubicle.cpp \
               pebay.cpp \
               pedevice.cpp \
               peboard.cpp \
               peport.cpp \
               peinfoset.cpp \
               pecubicleconn.cpp \
               pevterminal.cpp \
               pestrap.cpp \
               pevterminalconn.cpp \
               pecable.cpp \
               pefiber.cpp \
               pevolume.cpp \
               pedwg.cpp \
               basemanager.cpp \
               pbbaseobject.cpp \
               pbdevicecategory.cpp \
               pbbaycategory.cpp \
               pbvlevel.cpp \
               pbmanufacture.cpp \
               pbdevicetype.cpp \
               pblibdevice.cpp \
               pblibboard.cpp \
               pblibport.cpp \
               pbtpbay.cpp \
               pbtpdevice.cpp \
               pbtpport.cpp \
               pbtpinfoset.cpp

RESOURCES   += projectexplorer.qrc











































