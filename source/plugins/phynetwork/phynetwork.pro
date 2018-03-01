TEMPLATE        = lib
TARGET          = phynetwork
DEFINES         += PHYNETWORK_LIBRARY

include(../plugin.pri)
include(phynetwork_dependencies.pri)
include(../../shared/translations/translations.pri)

HEADERS         += phynetwork_global.h \
                   phynetwork_constants.h \
                   phynetworkplugin.h \
                   infosetmode.h \
                   cubicleconnmode.h \
                   cablemode.h \
                   infosetmodewidget.h \
                   cubicleconnmodewidget.h \
                   cablemodewidget.h \
                   configwidgetinfoset.h \
                   configwidgetcubicleconn.h \
                   widgetinfoset.h \
                   widgetcubicleconn.h \
                   cablegenerator.h \
                   phynetworksettings.h \
                   propertydlgcable.h \
                   propertydlgodf.h \
                   propertydlgodflayer.h \
                   renamecabledlg.h

SOURCES         += phynetworkplugin.cpp \
                   infosetmode.cpp \
                   cubicleconnmode.cpp \
                   cablemode.cpp \
                   infosetmodewidget.cpp \
                   cubicleconnmodewidget.cpp \
                   cablemodewidget.cpp \
                   configwidgetinfoset.cpp \
                   configwidgetcubicleconn.cpp \
                   widgetinfoset.cpp \
                   widgetcubicleconn.cpp \
                   cablegenerator.cpp \
                   phynetworksettings.cpp \
                   propertydlgcable.cpp \
                   propertydlgodf.cpp \
                   propertydlgodflayer.cpp \
                   renamecabledlg.cpp

RESOURCES       += phynetwork.qrc

OTHER_FILES     += phynetwork.pluginspec











































