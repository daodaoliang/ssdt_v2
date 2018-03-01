#ifndef BAYTEMPLATE_GLOBAL_H
#define BAYTEMPLATE_GLOBAL_H

#include <QtGlobal>

#if defined(BAYTEMPLATE_LIBRARY)
#  define BAYTEMPLATE_EXPORT Q_DECL_EXPORT
#else
#  define BAYTEMPLATE_EXPORT Q_DECL_IMPORT
#endif

#endif // BAYTEMPLATE_GLOBAL_H
