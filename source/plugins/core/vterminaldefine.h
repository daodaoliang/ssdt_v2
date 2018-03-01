#ifndef VTERMINALDEFINE_H
#define VTERMINALDEFINE_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include "core_global.h"

namespace SclParser {

class SCLElement;

} // namespace SclParser

namespace Core {

class VTerminalDefinePrivate;

class CORE_EXPORT VTerminalDefine : public QObject
{
    Q_OBJECT

// Construction and Destruction
public:
    VTerminalDefine(QObject *pParent = 0);
    ~VTerminalDefine();

// Operations
public:
    static VTerminalDefine* Instance();

    QMap<QString, QString>  GetGooseTx() const;
    void                    SetGooseTx(const QMap<QString, QString> &mapGooseTx);
    QMap<QString, QString>  GetGooseRx() const;
    void                    SetGooseRx(const QMap<QString, QString> &mapGooseRx);
    QMap<QString, QString>  GetSvTx() const;
    void                    SetSvTx(const QMap<QString, QString> &mapSvTx);
    QMap<QString, QString>  GetSvRx() const;
    void                    SetSvRx(const QMap<QString, QString> &mapSvRx);

    QMap<QString, QString>  GetDefaultGooseTx() const;
    QMap<QString, QString>  GetDefaultGooseRx() const;
    QMap<QString, QString>  GetDefaultSvTx() const;
    QMap<QString, QString>  GetDefaultSvRx() const;

    QList<QStringList>      PickupGooseTx(SclParser::SCLElement *pSCLElement);
    QList<QStringList>      PickupGooseRx(SclParser::SCLElement *pSCLElement);
    QList<QStringList>      PickupSvTx(SclParser::SCLElement *pSCLElement);
    QList<QStringList>      PickupSvRx(SclParser::SCLElement *pSCLElement);

private:
    QList<QStringList>      PickupTx(SclParser::SCLElement *pSCLElement, const QMap<QString, QString> &mapTx);
    QList<QStringList>      PickupRx(SclParser::SCLElement *pSCLElementLDevice, const QMap<QString, QString> &mapRx);

    QList<QStringList>      PickupDOTypeSDO(SclParser::SCLElement *pSCLElementSDO, SclParser::SCLElement *pSCLElementDOISDI, const QMap<QString, QString> &mapRx);
    QStringList             PickupBDA(SclParser::SCLElement *pSCLElementDA, const QMap<QString, QString> &mapRx);

    void ReadSettings();
    void SaveSettings();

// Properties
private:
    VTerminalDefinePrivate *m_d;
};

} // namespace Core

#endif // VTERMINALDEFINE_H
