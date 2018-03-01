#include "core_constants.h"
#include "uniqueidmanager.h"

using namespace Core;

CUniqueIDManager* CUniqueIDManager::m_pInstance = 0;

CUniqueIDManager::CUniqueIDManager()
{
    m_pInstance = this;
    m_hashUniqueIdentifiers.insert(Constants::g_szUID_Context_Global, Constants::g_iUID_Context_Global);
}

CUniqueIDManager::~CUniqueIDManager()
{
    m_pInstance = 0;
}

bool CUniqueIDManager::HasUniqueIdentifier(const QString &strID) const
{
    return m_hashUniqueIdentifiers.contains(strID);
}

int CUniqueIDManager::GenerateUniqueIdentifier(const QString &strID)
{
    if(HasUniqueIdentifier(strID))
        return m_hashUniqueIdentifiers.value(strID);

    int nID = m_hashUniqueIdentifiers.count() + 1;
    m_hashUniqueIdentifiers.insert(strID, nID);

    return nID;
}

QString CUniqueIDManager::GetStringForUniqueIdentifier(int nID)
{
    return m_hashUniqueIdentifiers.key(nID);
}
