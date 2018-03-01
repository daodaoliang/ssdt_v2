#ifndef SCLMODEL_GLOBAL_H
#define SCLMODEL_GLOBAL_H

#include <QtGlobal>

#if defined(SCLMODEL_LIBRARY)
#  define SCLMODEL_EXPORT Q_DECL_EXPORT
#else
#  define SCLMODEL_EXPORT Q_DECL_IMPORT
#endif

#endif // SCLMODEL_GLOBAL_H
