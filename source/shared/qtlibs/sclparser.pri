isEmpty(SCLPARSER_PRI_INCLUDED) {
SCLPARSER_PRI_INCLUDED = 1

include(../../ssdt.pri)

QT_LIBS_CONFIG += sclparser
QT_LIBS_OUTPUT = $$PATH_OUTPUT
include($$PWD/../../../../../working/qtlibs/qtlibs.pri)
}
