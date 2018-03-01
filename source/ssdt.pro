#-------------------------------------------------
# Qt Version Check
#-------------------------------------------------
contains(QT_VERSION, ^4\\.[0-5]\\..*) {
    message("Cannot build ssdt with Qt version $${QT_VERSION}.")
    error("Use at least Qt 4.6.")
}

TEMPLATE    = subdirs
CONFIG      += ordered

SUBDIRS     = libs \
              apps \
              plugins
