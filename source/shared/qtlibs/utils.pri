isEmpty(UTILS_PRI_INCLUDED) {
UTILS_PRI_INCLUDED = 1

include(../../ssdt.pri)

QT_LIBS_CONFIG += utils
QT_LIBS_OUTPUT = $$PATH_OUTPUT
include($$PWD/../../../../../working/qtlibs/qtlibs.pri)
}
