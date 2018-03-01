isEmpty(EXTENSIONSYSTEM_PRI_INCLUDED) {
EXTENSIONSYSTEM_PRI_INCLUDED = 1

include(../../ssdt.pri)

QT_LIBS_CONFIG += extensionsystem
QT_LIBS_OUTPUT = $$PATH_OUTPUT
include($$PWD/../../../../../working/qtlibs/qtlibs.pri)
}
