#ifndef CONFIGCOREOPERATION_H
#define CONFIGCOREOPERATION_H

#include <QObject>
#include "core_global.h"
#include "projectexplorer/pevterminal.h"
#include "projectexplorer/pestrap.h"

namespace SclParser {

class SCLDocument;
class SCLElement;

} // namespace ProjectExplorer

namespace ProjectExplorer {

class PeDevice;
class PbLibDevice;

} // namespace ProjectExplorer

namespace Core {

class ConfigCoreOperationPrivate;
class CORE_EXPORT ConfigCoreOperation : public QObject
{
    Q_OBJECT

// Construction and Destruction
public:
    ConfigCoreOperation(QObject *pParent = 0);
    ~ConfigCoreOperation();

// Operations
public:
    static ConfigCoreOperation*         Instance();

    bool                                CloneDevices(const QList<ProjectExplorer::PeDevice*> &lstSrcDevices,
                                                     QList<ProjectExplorer::PeDevice> lstDstDevices);

    ProjectExplorer::PeDevice*          CreateDeviceFromLibDevice(ProjectExplorer::PeDevice device,
                                                                  ProjectExplorer::PbLibDevice *pLibDevice);

    QList<ProjectExplorer::PeVTerminal> PickupVTerminals(SclParser::SCLElement *pSCLElementIED);

    QList<ProjectExplorer::PeStrap>     PickupStraps(SclParser::SCLElement *pSCLElementIED);

    bool                                UpdateDeviceModel(SclParser::SCLDocument *pSCLDocumentIcd,
                                                          QString strIcdFileName,
                                                          const QList<ProjectExplorer::PeDevice*> &lstDevices,
                                                          QList<ProjectExplorer::PeDevice*> *plstFailedDevices = 0);

    bool                                ClearDeviceModel(const QList<ProjectExplorer::PeDevice*> &lstDevices,
                                                         QList<ProjectExplorer::PeDevice*> *plstFailedDevices = 0);

    bool                                ExportToScd(ProjectExplorer::PeProjectVersion *pProjectVersion,
                                                    const QString &strScdFileName,
                                                    bool bExportPortConnection = true,
                                                    bool bExportConnection = true);

    bool                                ImportFromScd(ProjectExplorer::PeProjectVersion *pProjectVersion,
                                                      const QString &strScdFileName);

    bool                                ExportSpcd(ProjectExplorer::PeProjectVersion *pProjectVersion,
                                                   const QString &strSpcdFileName);

    bool                                ExportIpcd(ProjectExplorer::PeDevice *pDevice,
                                                   const QString &strIpcdFileName);

    bool                                ExportIpcd(ProjectExplorer::PbLibDevice *pLibDevice,
                                                   const QString &strIpcdFileName);

    bool                                ImportIpcd(const QList<ProjectExplorer::PeDevice*> &lstDevices,
                                                   const QString &strIpcdFileName);

// Properties
private:
    ConfigCoreOperationPrivate *m_d;
};

} // namespace Core

#endif // CONFIGCOREOPERATION_H
