#ifndef STYLEDUI_GLOBAL_H
#define STYLEDUI_GLOBAL_H

#include <QtGlobal>

#if defined(STYLEDUI_LIBRARY)
#  define STYLEDUI_EXPORT Q_DECL_EXPORT
#else
#  define STYLEDUI_EXPORT Q_DECL_IMPORT
#endif

#endif // STYLEDUI_GLOBAL_H
