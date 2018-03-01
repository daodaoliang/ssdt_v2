#include <QMap>

#include "pbbaseobject.h"

namespace ProjectExplorer {

class PbBaseObjectPrivate
{
public:
    PbBaseObjectPrivate(PbBaseObject::ObjectType eObjectType, int iId) :
        m_eObjectType(eObjectType),
        m_iId(iId)
    {
    }

    PbBaseObject::ObjectType    m_eObjectType;
    int                         m_iId;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PbBaseObject::PbBaseObject(const PbBaseObject &BaseObject) :
    QObject(0), m_d(new PbBaseObjectPrivate(BaseObject.GetObjectType(), BaseObject.GetId()))
{
}

PbBaseObject::PbBaseObject(ObjectType eObjectType) :
    QObject(0), m_d(new PbBaseObjectPrivate(eObjectType, m_iInvalidObjectId))
{
}

PbBaseObject::PbBaseObject(ObjectType eObjectType, int iId) :
    QObject(0), m_d(new PbBaseObjectPrivate(eObjectType, iId))
{
}

PbBaseObject::~PbBaseObject()
{
    delete m_d;
}

QIcon PbBaseObject::GetObjectIcon(const ObjectType &eObjectType)
{
    static QMap<ObjectType, QIcon> mapObjectTypeToIcon;
    if(mapObjectTypeToIcon.isEmpty())
    {
        mapObjectTypeToIcon.insert(botUnknown, QIcon());
        mapObjectTypeToIcon.insert(botLibDevice, QIcon(":/projectexplorer/images/pe_device.png"));
        mapObjectTypeToIcon.insert(botLibBoard, QIcon(":/projectexplorer/images/pe_board.png"));
        mapObjectTypeToIcon.insert(botLibPort, QIcon(":/projectexplorer/images/pe_port.png"));
        mapObjectTypeToIcon.insert(botTpBay, QIcon(":/projectexplorer/images/pe_bay.png"));
        mapObjectTypeToIcon.insert(botTpDevice, QIcon(":/projectexplorer/images/pe_device.png"));
        mapObjectTypeToIcon.insert(botTpPort, QIcon(":/projectexplorer/images/pe_port.png"));
        mapObjectTypeToIcon.insert(botTpInfoSet, QIcon(":/projectexplorer/images/pe_infoset.png"));
    }

    return mapObjectTypeToIcon.value(eObjectType, QIcon());
}

QString PbBaseObject::GetObjectTypeName(const PbBaseObject::ObjectType &eObjectType)
{
    static QMap<ObjectType, QString> mapObjectTypeToName;
    if(mapObjectTypeToName.isEmpty())
    {
        mapObjectTypeToName.insert(botUnknown, tr("Unknown"));
        mapObjectTypeToName.insert(botLibDevice, tr("Device"));
        mapObjectTypeToName.insert(botLibBoard, tr("Board"));
        mapObjectTypeToName.insert(botLibPort, tr("Port"));
        mapObjectTypeToName.insert(botTpBay, tr("Bay"));
        mapObjectTypeToName.insert(botTpDevice, tr("Device"));
        mapObjectTypeToName.insert(botTpPort, tr("Port"));
        mapObjectTypeToName.insert(botTpPort, tr("Information Set"));
    }

    return mapObjectTypeToName.value(eObjectType, tr("Unknown"));
}

bool PbBaseObject::CompareId(PbBaseObject *pBaseObject1, PbBaseObject *pBaseObject2)
{
    return pBaseObject1->GetId() < pBaseObject2->GetId();
}

bool PbBaseObject::CompareDisplayName(PbBaseObject *pBaseObject1, PbBaseObject *pBaseObject2)
{
    return pBaseObject1->GetDisplayName() < pBaseObject2->GetDisplayName();
}

QIcon PbBaseObject::GetDisplayIcon() const
{
    return GetObjectIcon(m_d->m_eObjectType);
}

QString PbBaseObject::GetObjectTypeName() const
{
    return GetObjectTypeName(m_d->m_eObjectType);
}

PbBaseObject& PbBaseObject::operator=(const PbBaseObject &BaseObject)
{
    *m_d = *BaseObject.m_d;

    return *this;
}

PbBaseObject::ObjectType PbBaseObject::GetObjectType() const
{
    return m_d->m_eObjectType;
}

void PbBaseObject::SetObjectType(const ObjectType &eObjectType)
{
    m_d->m_eObjectType = eObjectType;
}

int PbBaseObject::GetId() const
{
    return m_d->m_iId;
}

void PbBaseObject::SetId(int iId)
{
    m_d->m_iId = iId;
}
