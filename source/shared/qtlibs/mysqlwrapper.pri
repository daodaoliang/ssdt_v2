isEmpty(MYSQLWRAPPER_PRI_INCLUDED) {
MYSQLWRAPPER_PRI_INCLUDED = 1

include(../../ssdt.pri)

QT_LIBS_CONFIG += mysqlwrapper
QT_LIBS_OUTPUT = $$PATH_OUTPUT
include($$PWD/../../../../../working/qtlibs/qtlibs.pri)
}
