#ifndef PEVTERMINAL_H
#define PEVTERMINAL_H

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeVTerminalPrivate;
class PeDevice;
class PROJECTEXPLORER_EXPORT PeVTerminal : public PeProjectObject
{
    Q_OBJECT

// Structures and Enumerations
public:
    enum TerminalType { ttGoose = 0, ttSv };
    enum TerminalDirection { tdIn = 0, tdOut };

// Construction and Destruction
public:
    PeVTerminal();
    PeVTerminal(const PeVTerminal &VTerminal);
    PeVTerminal(PeProjectVersion *pProjectVersion, int iId);
    virtual ~PeVTerminal();

// Operations
public:
    static bool         ComparNumber(PeVTerminal *pVTerminal1, PeVTerminal *pVTerminal2);

    // Derived from PeProjectObject
    virtual QString     GetDisplayName() const;

    // Operator overload
    PeVTerminal&        operator=(const PeVTerminal &VTerminal);

    // Property operations
    TerminalType        GetType() const;
    void                SetType(TerminalType eTerminalType);
    TerminalDirection   GetDirection() const;
    void                SetDirection(TerminalDirection eTerminalDirection);
    int                 GetNumber() const;
    void                SetNumber(int iNumber);
    QString             GetIEDDesc() const;
    void                SetIEDDesc(const QString &strIEDDesc);
    QString             GetProDesc() const;
    void                SetProDesc(const QString &strProDesc);
    QString             GetLDInst() const;
    void                SetLDInst(const QString &strLDInst);
    QString             GetLNPrefix() const;
    void                SetLNPrefix(const QString &strLNPrefix);
    QString             GetLNClass() const;
    void                SetLNClass(const QString &strLNClass);
    QString             GetLNInst() const;
    void                SetLNInst(const QString &strLNInst);
    QString             GetDOName() const;
    void                SetDOName(const QString &strDOName);
    QString             GetDAName() const;
    void                SetDAName(const QString &strDAName);

    QString             GetRefName() const;

    // Project object operation
    PeDevice*           GetParentDevice() const;
    void                SetParentDevice(PeDevice *pDevice);

// Properties
private:
    PeVTerminalPrivate *m_d;
};

} // namespace ProjectExplorer

#endif // PEVTERMINAL_H
