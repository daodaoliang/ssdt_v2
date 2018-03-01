#ifndef PHYNETWORK_GLOBAL_H
#define PHYNETWORK_GLOBAL_H

#include <QtGlobal>

#if defined(PHYNETWORK_LIBRARY)
#  define PHYNETWORK_EXPORT Q_DECL_EXPORT
#else
#  define PHYNETWORK_EXPORT Q_DECL_IMPORT
#endif

#endif // PHYNETWORK_GLOBAL_H