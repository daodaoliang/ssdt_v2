include(../ssdt.pri)

DESTDIR     = $$PATH_OUTPUT_PLUGINS
LIBS        += -L$$DESTDIR
INCLUDEPATH += $$PATH_SOURCE_PLUGINS
DEPENDPATH  += $$PATH_SOURCE_PLUGINS

# copy the plugin spec
isEmpty(TARGET) {
    error("plugin.pri: You must provide a TARGET")
}

PLUGINSPECS = $${_PRO_FILE_PWD_}/$${TARGET}.pluginspec
copy2build.input = PLUGINSPECS
copy2build.output = $$DESTDIR/$${TARGET}.pluginspec
isEmpty(vcproj):copy2build.variable_out = PRE_TARGETDEPS
copy2build.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copy2build.name = COPY ${QMAKE_FILE_IN}
copy2build.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += copy2build

TARGET = $$qtLibraryTarget($$TARGET)
CONFIG += plugin plugin_with_soname
