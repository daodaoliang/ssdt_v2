#ifndef DEVLIBRARY_GLOBAL_H
#define DEVLIBRARY_GLOBAL_H

#include <QtGlobal>

#if defined(DEVLIBRARY_LIBRARY)
#  define DEVLIBRARY_EXPORT Q_DECL_EXPORT
#else
#  define DEVLIBRARY_EXPORT Q_DECL_IMPORT
#endif

#endif // SCLMODEL_GLOBAL_H
