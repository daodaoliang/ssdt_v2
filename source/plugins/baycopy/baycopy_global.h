#ifndef BAYCOPY_GLOBAL_H
#define BAYCOPY_GLOBAL_H

#include <QtGlobal>

#if defined(BAYCOPY_LIBRARY)
#  define BAYCOPY_EXPORT Q_DECL_EXPORT
#else
#  define BAYCOPY_EXPORT Q_DECL_IMPORT
#endif

#endif // BAYCOPY_GLOBAL_H
