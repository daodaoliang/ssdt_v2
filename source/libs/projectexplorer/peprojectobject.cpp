#include <QMap>

#include "peprojectobject.h"

namespace ProjectExplorer {

class PeProjectObjectPrivate
{
public:
    PeProjectObjectPrivate(PeProjectObject::ObjectType eObjectType, PeProjectVersion *pProjectVersion, int iId) :
        m_eObjectType(eObjectType),
        m_pProjectVersion(pProjectVersion),
        m_iId(iId)
    {
    }

    PeProjectObject::ObjectType m_eObjectType;
    PeProjectVersion            *m_pProjectVersion;
    int                         m_iId;
};

} // namespace ProjectExplorer

using namespace ProjectExplorer;

PeProjectObject::PeProjectObject(const PeProjectObject &ProjectObject) :
    QObject(0), m_d(new PeProjectObjectPrivate(ProjectObject.GetObjectType(), ProjectObject.GetProjectVersion(), ProjectObject.GetId()))
{
}

PeProjectObject::PeProjectObject(ObjectType eObjectType) :
    QObject(0), m_d(new PeProjectObjectPrivate(eObjectType, 0, m_iInvalidObjectId))
{
}

PeProjectObject::PeProjectObject(ObjectType eObjectType, PeProjectVersion *pProjectVersion, int iId) :
    QObject(0), m_d(new PeProjectObjectPrivate(eObjectType, pProjectVersion, iId))
{
}

PeProjectObject::~PeProjectObject()
{
    delete m_d;
}

QIcon PeProjectObject::GetObjectIcon(const ObjectType &eObjectType)
{
    static QMap<ObjectType, QIcon> mapObjectTypeToIcon;
    if(mapObjectTypeToIcon.isEmpty())
    {
        mapObjectTypeToIcon.insert(otUnknown, QIcon());
        mapObjectTypeToIcon.insert(otProject, QIcon(":/projectexplorer/images/pe_project.png"));
        mapObjectTypeToIcon.insert(otProjectVersion, QIcon(":/projectexplorer/images/pe_projectversion.png"));
        mapObjectTypeToIcon.insert(otRoom, QIcon(":/projectexplorer/images/pe_room.png"));
        mapObjectTypeToIcon.insert(otCubicle, QIcon(":/projectexplorer/images/pe_cubicle.png"));
        mapObjectTypeToIcon.insert(otBay, QIcon(":/projectexplorer/images/pe_bay.png"));
        mapObjectTypeToIcon.insert(otDevice, QIcon(":/projectexplorer/images/pe_ied.png"));
        mapObjectTypeToIcon.insert(otBoard, QIcon(":/projectexplorer/images/pe_board.png"));
        mapObjectTypeToIcon.insert(otPort, QIcon(":/projectexplorer/images/pe_port.png"));
        mapObjectTypeToIcon.insert(otInfoSet, QIcon(":/projectexplorer/images/pe_infoset.png"));
        mapObjectTypeToIcon.insert(otCubicleConn, QIcon(":/projectexplorer/images/pe_cubicleconn.png"));
        mapObjectTypeToIcon.insert(otVTerminal, QIcon(":/projectexplorer/images/pe_vterminal.png"));
        mapObjectTypeToIcon.insert(otStrap, QIcon(":/projectexplorer/images/pe_strap.png"));
        mapObjectTypeToIcon.insert(otVTerminalConn, QIcon(":/projectexplorer/images/pe_vterminalconn.png"));
        mapObjectTypeToIcon.insert(otCable, QIcon(":/projectexplorer/images/pe_cable.png"));
        mapObjectTypeToIcon.insert(otFiber, QIcon(":/projectexplorer/images/pe_fiber.png"));
        mapObjectTypeToIcon.insert(otVolume, QIcon(":/projectexplorer/images/pe_volume.png"));
        mapObjectTypeToIcon.insert(otDwg, QIcon(":/projectexplorer/images/pe_dwg.png"));
    }

    return mapObjectTypeToIcon.value(eObjectType, QIcon());
}

QString PeProjectObject::GetObjectTypeName(const PeProjectObject::ObjectType &eObjectType)
{
    static QMap<ObjectType, QString> mapObjectTypeToName;
    if(mapObjectTypeToName.isEmpty())
    {
        mapObjectTypeToName.insert(otUnknown, tr("Unknown"));
        mapObjectTypeToName.insert(otProject, tr("Project"));
        mapObjectTypeToName.insert(otProjectVersion, tr("Project Version"));
        mapObjectTypeToName.insert(otRoom, tr("Room"));
        mapObjectTypeToName.insert(otCubicle, tr("Cubicle"));
        mapObjectTypeToName.insert(otBay, tr("Bay"));
        mapObjectTypeToName.insert(otDevice, tr("Device"));
        mapObjectTypeToName.insert(otBoard, tr("Board"));
        mapObjectTypeToName.insert(otPort, tr("Port"));
        mapObjectTypeToName.insert(otInfoSet, tr("Information Set"));
        mapObjectTypeToName.insert(otCubicleConn, tr("Cubicle Connection"));
        mapObjectTypeToName.insert(otVTerminal, tr("Virtual Terminal"));
        mapObjectTypeToName.insert(otStrap, tr("Strap"));
        mapObjectTypeToName.insert(otVTerminalConn, tr("Virtual Connection"));
        mapObjectTypeToName.insert(otCable, tr("Cable"));
		mapObjectTypeToName.insert(otFiber, tr("Fiber"));
        mapObjectTypeToName.insert(otVolume, tr("Volume"));
        mapObjectTypeToName.insert(otDwg, tr("AutoCAD Drawing File"));
    }

    return mapObjectTypeToName.value(eObjectType, tr("Unknown"));
}

bool PeProjectObject::CompareId(PeProjectObject *pProjectObject1, PeProjectObject *pProjectObject2)
{
    return pProjectObject1->GetId() < pProjectObject2->GetId();
}

bool PeProjectObject::CompareDisplayName(PeProjectObject *pProjectObject1, PeProjectObject *pProjectObject2)
{
    return pProjectObject1->GetDisplayName() < pProjectObject2->GetDisplayName();
}

QIcon PeProjectObject::GetDisplayIcon() const
{
    return GetObjectIcon(m_d->m_eObjectType);
}

QString PeProjectObject::GetObjectTypeName() const
{
    return GetObjectTypeName(m_d->m_eObjectType);
}

PeProjectObject& PeProjectObject::operator=(const PeProjectObject &ProjectObject)
{
    *m_d = *ProjectObject.m_d;

    return *this;
}

PeProjectObject::ObjectType PeProjectObject::GetObjectType() const
{
    return m_d->m_eObjectType;
}

void PeProjectObject::SetObjectType(const ObjectType &eObjectType)
{
    m_d->m_eObjectType = eObjectType;
}

PeProjectVersion* PeProjectObject::GetProjectVersion() const
{
    return m_d->m_pProjectVersion;
}

void PeProjectObject::SetProjectVersion(PeProjectVersion *pProjectVersion)
{
    m_d->m_pProjectVersion = pProjectVersion;
}

int PeProjectObject::GetId() const
{
    return m_d->m_iId;
}

void PeProjectObject::SetId(int iId)
{
    m_d->m_iId = iId;
}
