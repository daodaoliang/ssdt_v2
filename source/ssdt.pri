PATH_SOURCE         = $$PWD
PATH_SOURCE_LIBS    = $$PATH_SOURCE/libs
PATH_SOURCE_PLUGINS = $$PATH_SOURCE/plugins
PATH_DEPLOYMENT     = $$PWD/../deployment
PATH_OUTPUT         = $$PATH_DEPLOYMENT/bin
PATH_OUTPUT_PLUGINS = $$PATH_OUTPUT/plugins

INCLUDEPATH         += $$PATH_SOURCE_LIBS \
                       $$PATH_SOURCE_PLUGINS

DEPENDPATH          += $$PATH_SOURCE_LIBS \
                       $$PATH_SOURCE_PLUGINS

LIBS                += -L$$PATH_OUTPUT

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
