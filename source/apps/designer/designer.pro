include(../apps.pri)
include(../../shared/qtlibs/sclparser.pri)
include(../../shared/qtlibs/extensionsystem.pri)
include(../../shared/translations/translations.pri)

CONFIG(debug, debug|release){
    TARGET  = designerd
    LIBS    *= -lprojectexplorerd
} else {
    TARGET = designer
    LIBS *= -lprojectexplorer
}

SOURCES     += main.cpp
RC_FILE     = designer.rc
