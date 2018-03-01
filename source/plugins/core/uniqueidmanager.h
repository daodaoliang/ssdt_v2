#ifndef UNIQUEIDMANAGER_H
#define UNIQUEIDMANAGER_H

#include <QString>
#include <QHash>
#include "core_global.h"

namespace Core {

class CORE_EXPORT CUniqueIDManager
{
// Construction and Destruction
public:
    CUniqueIDManager();
    ~CUniqueIDManager();

// Operations
public:
    static CUniqueIDManager* Instance() { return m_pInstance; }

    bool    HasUniqueIdentifier(const QString &strID) const;
    int     GenerateUniqueIdentifier(const QString &strID);
    QString GetStringForUniqueIdentifier(int nID);

// Properties
private:
    static CUniqueIDManager *m_pInstance;
    QHash<QString, int>     m_hashUniqueIdentifiers;
};

} // namespace Core

#endif // UNIQUEIDMANAGER_H
