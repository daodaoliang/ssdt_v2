TEMPLATE        = lib
TARGET          = devexplorer

include(../plugin.pri)
include(devexplorer_dependencies.pri)
include(../../shared/translations/translations.pri)

HEADERS         += devexplorerplugin.h \
                   devexplorermode.h \
                   devexplorermodewidget.h \
                   configwidgetdeviceobject.h \
                   configwidgetprojectphysical.h \
                   configwidgetprojectelectrical.h \
                   configwidgetbay.h \
                   configwidgetroom.h \
                   configwidgetcubicle.h \
                   configwidgetswitch.h \
                   configwidgetiedboard.h \
                   configwidgetiedport.h \
                   propertydlgbay.h \
                   propertydlgroom.h \
                   propertydlgcubicle.h \
                   propertydlgdevice.h \
                   propertydlgboard.h \
                   propertydlgfiberport.h \

SOURCES         += devexplorerplugin.cpp \
                   devexplorermode.cpp \
                   devexplorermodewidget.cpp \
                   configwidgetdeviceobject.cpp \
                   configwidgetprojectphysical.cpp \
                   configwidgetprojectelectrical.cpp \
                   configwidgetbay.cpp \
                   configwidgetroom.cpp \
                   configwidgetcubicle.cpp \
                   configwidgetswitch.cpp \
                   configwidgetiedboard.cpp \
                   configwidgetiedport.cpp \
                   propertydlgbay.cpp \
                   propertydlgroom.cpp \
                   propertydlgcubicle.cpp \
                   propertydlgdevice.cpp \
                   propertydlgboard.cpp \
                   propertydlgfiberport.cpp \

RESOURCES       += devexplorer.qrc

OTHER_FILES     += devexplorer.pluginspec











































