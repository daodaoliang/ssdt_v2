TEMPLATE        = lib
TARGET          = baytemplate
DEFINES         += BAYTEMPLATE_LIBRARY

include(../plugin.pri)
include(baytemplate_dependencies.pri)
include(../../shared/translations/translations.pri)

HEADERS         += baytemplate_global.h \
                   baytemplate_constants.h \
                   baytemplateplugin.h \
                   baytemplateexplorer.h \
                   baytemplateexplorerwidget.h \
                   baytemplateconfigdlg.h \
                   baytemplatedlg.h \
                   configwidgettpdevice.h \
                   configwidgettpinfoset.h \
                   propertydlgtpbay.h \
                   propertydlgtpdevice.h \
                   propertydlgtpport.h \
                   propertydlgtpinfoset.h

SOURCES         += baytemplateplugin.cpp \
                   baytemplateexplorer.cpp \
                   baytemplateexplorerwidget.cpp \
                   baytemplateconfigdlg.cpp \
                   baytemplatedlg.cpp \
                   configwidgettpdevice.cpp \
                   configwidgettpinfoset.cpp \
                   propertydlgtpbay.cpp \
                   propertydlgtpdevice.cpp \
                   propertydlgtpport.cpp \
                   propertydlgtpinfoset.cpp

RESOURCES       += baytemplate.qrc

OTHER_FILES     += baytemplate.pluginspec











































