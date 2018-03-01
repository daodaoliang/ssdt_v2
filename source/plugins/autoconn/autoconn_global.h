#ifndef AUTOCONN_GLOBAL_H
#define AUTOCONN_GLOBAL_H

#include <QtGlobal>

#if defined(AUTOCONN_LIBRARY)
#  define AUTOCONN_EXPORT Q_DECL_EXPORT
#else
#  define AUTOCONN_EXPORT Q_DECL_IMPORT
#endif

#endif // AUTOCONN_GLOBAL_H
