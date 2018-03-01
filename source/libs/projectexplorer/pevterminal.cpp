#include <QMap>

#include "pevterminal.h"
#include "pedevice.h"

namespace ProjectExplorer {

class PeVTerminalPrivate
{
public:
    PeVTerminalPrivate()
        : m_iNumber(0), m_strIEDDesc(""), m_strProDesc(""), m_strLDInst(""), m_strLNPrefix(""), m_strLNClass(""), m_strLNInst(""), m_strDOName(""), m_strDAName(""), m_pDevice(0)
    {
    }

    PeVTerminal::TerminalType       m_eTerminalType;
    PeVTerminal::TerminalDirection  m_eTerminalDirection;
    int                             m_iNumber;
    QString                         m_strIEDDesc;
    QString                         m_strProDesc;
    QString                         m_strLDInst;
    QString                         m_strLNPrefix;
    QString                         m_strLNClass;
    QString                         m_strLNInst;
    QString                         m_strDOName;
    QString                         m_strDAName;
    PeDevice                        *m_pDevice;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeVTerminal::PeVTerminal() : PeProjectObject(PeProjectObject::otVTerminal), m_d(new PeVTerminalPrivate)
{
}

PeVTerminal::PeVTerminal(const PeVTerminal &VTerminal) : PeProjectObject(VTerminal), m_d(new PeVTerminalPrivate)
{
    *m_d = *VTerminal.m_d;
}

PeVTerminal::PeVTerminal(PeProjectVersion *pProjectVersion, int iId) :
    PeProjectObject(PeProjectObject::otVTerminal, pProjectVersion, iId), m_d(new PeVTerminalPrivate)
{
}

PeVTerminal::~PeVTerminal()
{
    delete m_d;
}

bool PeVTerminal::ComparNumber(PeVTerminal *pVTerminal1, PeVTerminal *pVTerminal2)
{
    return pVTerminal1->GetNumber() < pVTerminal2->GetNumber();
}

QString PeVTerminal::GetDisplayName() const
{
    QString strDisplay = GetRefName();
    const QString strProDesc = GetProDesc();
    if(!strProDesc.isEmpty())
        strDisplay = QString("%1 : %2").arg(strProDesc).arg(strDisplay);

    return strDisplay;
}

PeVTerminal& PeVTerminal::operator=(const PeVTerminal &VTerminal)
{
    PeProjectObject::operator =(VTerminal);
    *m_d = *VTerminal.m_d;

    return *this;
}

PeVTerminal::TerminalType PeVTerminal::GetType() const
{
    return m_d->m_eTerminalType;
}

void PeVTerminal::SetType(TerminalType eTerminalType)
{
    m_d->m_eTerminalType = eTerminalType;
}

PeVTerminal::TerminalDirection PeVTerminal::GetDirection() const
{
    return m_d->m_eTerminalDirection;
}

void PeVTerminal::SetDirection(TerminalDirection eTerminalDirection)
{
    m_d->m_eTerminalDirection = eTerminalDirection;
}

int PeVTerminal::GetNumber() const
{
    return m_d->m_iNumber;
}

void PeVTerminal::SetNumber(int iNumber)
{
    m_d->m_iNumber = iNumber;
}

QString PeVTerminal::GetIEDDesc() const
{
    return m_d->m_strIEDDesc;
}

void PeVTerminal::SetIEDDesc(const QString &strIEDDesc)
{
    m_d->m_strIEDDesc = strIEDDesc;
}

QString PeVTerminal::GetProDesc() const
{
    return m_d->m_strProDesc;
}

void PeVTerminal::SetProDesc(const QString &strProDesc)
{
    m_d->m_strProDesc = strProDesc;
}

QString PeVTerminal::GetLDInst() const
{
    return m_d->m_strLDInst;
}

void PeVTerminal::SetLDInst(const QString &strLDInst)
{
    m_d->m_strLDInst = strLDInst;
}

QString PeVTerminal::GetLNPrefix() const
{
    return m_d->m_strLNPrefix;
}

void PeVTerminal::SetLNPrefix(const QString &strLNPrefix)
{
    m_d->m_strLNPrefix = strLNPrefix;
}

QString PeVTerminal::GetLNClass() const
{
    return m_d->m_strLNClass;
}

void PeVTerminal::SetLNClass(const QString &strLNClass)
{
    m_d->m_strLNClass = strLNClass;
}

QString PeVTerminal::GetLNInst() const
{
    return m_d->m_strLNInst;
}

void PeVTerminal::SetLNInst(const QString &strLNInst)
{
    m_d->m_strLNInst = strLNInst;
}

QString PeVTerminal::GetDOName() const
{
    return m_d->m_strDOName;
}

void PeVTerminal::SetDOName(const QString &strDOName)
{
    m_d->m_strDOName = strDOName;
}

QString PeVTerminal::GetDAName() const
{
    return m_d->m_strDAName;
}

void PeVTerminal::SetDAName(const QString &strDAName)
{
    m_d->m_strDAName = strDAName;
}

QString PeVTerminal::GetRefName() const
{
    QString strRefName = QString("%1/%2%3%4.%5").arg(m_d->m_strLDInst)
                                                .arg(m_d->m_strLNPrefix)
                                                .arg(m_d->m_strLNClass)
                                                .arg(m_d->m_strLNInst)
                                                .arg(m_d->m_strDOName);
    if(!m_d->m_strDAName.isEmpty())
        strRefName = QString("%1.%2").arg(strRefName).arg(m_d->m_strDAName);

    return strRefName;
}

PeDevice* PeVTerminal::GetParentDevice() const
{
    return m_d->m_pDevice;
}

void PeVTerminal::SetParentDevice(PeDevice *pDevice)
{
    m_d->m_pDevice = pDevice;
}
