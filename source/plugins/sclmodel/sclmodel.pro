TEMPLATE        = lib
TARGET          = sclmodel
DEFINES         += SCLMODEL_LIBRARY

include(../plugin.pri)
include(sclmodel_dependencies.pri)
include(../../shared/translations/translations.pri)

HEADERS         += sclmodel_global.h \
                   sclmodel_constants.h \
                   sclmodelplugin.h \
                   vterminalconnmode.h \
                   subnetworkmode.h \
                   vterminalconnmodewidget.h \
                   subnetworkmodewidget.h \
                   configwidgetvterminalconn.h \
                   widgetvterminalconn.h \
                   propertydlgaddress.h \
                   propertydlggse.h \
                   propertydlgsmv.h \
                   dialogbatchedit.h \
                   dialogautoconnect.h \
                   dialogdescsync.h

SOURCES         += sclmodelplugin.cpp \
                   vterminalconnmode.cpp \
                   subnetworkmode.cpp \
                   vterminalconnmodewidget.cpp \
                   subnetworkmodewidget.cpp \
                   configwidgetvterminalconn.cpp \
                   widgetvterminalconn.cpp \
                   propertydlgaddress.cpp \
                   propertydlggse.cpp \
                   propertydlgsmv.cpp \
                   dialogbatchedit.cpp \
                   dialogautoconnect.cpp \
                   dialogdescsync.cpp

RESOURCES       += sclmodel.qrc

OTHER_FILES     += sclmodel.pluginspec











































