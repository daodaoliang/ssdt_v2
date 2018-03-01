#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/pebay.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peboard.h"
#include "projectexplorer/peport.h"
#include "projectexplorer/pevterminal.h"
#include "projectexplorer/pestrap.h"

#include "copybaymodelview.h"

using namespace BayCopy::Internal;

///////////////////////////////////////////////////////////////////////
// CopyBayItem member functions
///////////////////////////////////////////////////////////////////////
CopyBayItem::CopyBayItem() : m_bChecked(false)
{
}

CopyBayItem::~CopyBayItem()
{
}

bool CopyBayItem::GetChecked() const
{
    return m_bChecked;
}

void CopyBayItem::SetChecked(bool bChecked)
{
    m_bChecked = bChecked;
}

///////////////////////////////////////////////////////////////////////
// CopyBayItemBay member functions
///////////////////////////////////////////////////////////////////////
CopyBayItemBay::CopyBayItemBay(ProjectExplorer::PeBay *pBaySrc) :
    CopyBayItem(), m_pBaySrc(pBaySrc), m_pBayDst(new ProjectExplorer::PeBay(*pBaySrc))
{
    m_pBayDst->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    m_pBayDst->SetProjectVersion(0);
    m_pBayDst->SetName(m_pBayDst->GetName());
}

CopyBayItemBay::~CopyBayItemBay()
{
    delete m_pBayDst;
}

QString CopyBayItemBay::GetText(int iColumn) const
{
    if(iColumn == 0)
        return m_pBaySrc->GetDisplayName();
    else if(iColumn == 2 && m_bChecked)
        return QString("(%1) %2").arg(m_pBayDst->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId ? tr("New") : tr("Reuse")).arg(m_pBayDst->GetDisplayName());

    return "";
}

QIcon CopyBayItemBay::GetIcon(int iColumn) const
{
    if(iColumn == 0)
        return m_pBaySrc->GetDisplayIcon();
    else if(iColumn == 2 && m_bChecked)
        return m_pBayDst->GetDisplayIcon();

    return QIcon();
}

bool CopyBayItemBay::GetValid(int iColumn) const
{
    if(iColumn == 2 && m_bChecked)
    {
        if(m_pBayDst->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        {
            if(m_pBayDst->GetNumber().isEmpty())
                return false;

            if(ProjectExplorer::PeProjectVersion *pProjectVersion = m_pBayDst->GetProjectVersion())
            {
                foreach(ProjectExplorer::PeBay *pBay, pProjectVersion->GetAllBays())
                {
                    if(m_pBayDst->GetNumber() == pBay->GetNumber())
                        return false;
                }
            }
        }
    }

    return true;
}

ProjectExplorer::PeBay* CopyBayItemBay::GetBaySrc() const
{
    return m_pBaySrc;
}

ProjectExplorer::PeBay* CopyBayItemBay::GetBayDst() const
{
    return m_pBayDst;
}

void CopyBayItemBay::AppendChild(CopyBayItemDevice *pChildItem)
{
    if(pChildItem)
        m_lstChildItems.append(pChildItem);
}

QList<CopyBayItemDevice*> CopyBayItemBay::GetChildItems() const
{
    return m_lstChildItems;
}

///////////////////////////////////////////////////////////////////////
// CopyBayItemDevice member functions
///////////////////////////////////////////////////////////////////////
CopyBayItemDevice::CopyBayItemDevice(ProjectExplorer::PeDevice *pDeviceSrc, CopyBayItemBay *pParentItem) :
    CopyBayItem(), m_pDeviceSrc(pDeviceSrc), m_pParentItem(pParentItem), m_pDeviceDst(new ProjectExplorer::PeDevice(*pDeviceSrc))
{
    m_pDeviceDst->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
    m_pDeviceDst->SetProjectVersion(0);
    m_pDeviceDst->SetParentBay(m_pParentItem->GetBayDst());

    foreach(ProjectExplorer::PeBoard *pBoardSrc, pDeviceSrc->GetChildBoards())
    {
        ProjectExplorer::PeBoard *pBoardDst = new ProjectExplorer::PeBoard(*pBoardSrc);
        pBoardDst->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        pBoardDst->SetProjectVersion(0);
        pBoardDst->SetParentDevice(m_pDeviceDst);

        m_pDeviceDst->RemoveChildBoard(pBoardSrc);
        m_pDeviceDst->AddChildBoard(pBoardDst);

        foreach(ProjectExplorer::PePort *pPortSrc, pBoardSrc->GetChildPorts())
        {
            ProjectExplorer::PePort *pPortDst = new ProjectExplorer::PePort(*pPortSrc);
            pPortDst->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            pPortDst->SetProjectVersion(0);
            pPortDst->SetParentBoard(pBoardDst);

            pBoardDst->RemoveChildPort(pPortSrc);
            pBoardDst->AddChildPort(pPortDst);
        }
    }

    foreach(ProjectExplorer::PeVTerminal *pVTerminalSrc, pDeviceSrc->GetVTerminals())
    {
        ProjectExplorer::PeVTerminal *pVTerminalDst = new ProjectExplorer::PeVTerminal(*pVTerminalSrc);
        pVTerminalDst->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        pVTerminalDst->SetProjectVersion(0);
        pVTerminalDst->SetParentDevice(m_pDeviceDst);

        m_pDeviceDst->RemoveVTerminal(pVTerminalSrc);
        m_pDeviceDst->AddVTerminal(pVTerminalDst);
    }

    foreach(ProjectExplorer::PeStrap *pStrapSrc, pDeviceSrc->GetStraps())
    {
        ProjectExplorer::PeStrap *pStrapDst = new ProjectExplorer::PeStrap(*pStrapSrc);
        pStrapDst->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        pStrapDst->SetProjectVersion(0);
        pStrapDst->SetParentDevice(m_pDeviceDst);

        m_pDeviceDst->RemoveStrap(pStrapSrc);
        m_pDeviceDst->AddStrap(pStrapDst);
    }
}

CopyBayItemDevice::~CopyBayItemDevice()
{
    foreach(ProjectExplorer::PeBoard *pBoard, m_pDeviceDst->GetChildBoards())
    {
        foreach(ProjectExplorer::PePort *pPort, pBoard->GetChildPorts())
            delete pPort;

        delete pBoard;
    }

    foreach(ProjectExplorer::PeVTerminal *pVTerminal, m_pDeviceDst->GetVTerminals())
        delete pVTerminal;

    foreach(ProjectExplorer::PeStrap *pStrap, m_pDeviceDst->GetStraps())
        delete pStrap;

    delete m_pDeviceDst;
}

QString CopyBayItemDevice::GetText(int iColumn) const
{
    if(iColumn == 0)
        return m_pDeviceSrc->GetDisplayName();
    else if(iColumn == 1 && m_pDeviceSrc->GetParentCubicle())
        return m_pDeviceSrc->GetParentCubicle()->GetDisplayName();
    else if(iColumn == 2 && m_bChecked)
        return "(" + tr("New") + ") " + m_pDeviceDst->GetDisplayName();
    else if(iColumn == 3 && m_bChecked && m_pDeviceDst->GetParentCubicle())
        return QString("(%1) %2").arg(m_pDeviceDst->GetParentCubicle()->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId ? tr("New") : tr("Reuse")).arg(m_pDeviceDst->GetParentCubicle()->GetDisplayName());

    return "";
}

QIcon CopyBayItemDevice::GetIcon(int iColumn) const
{
    if(iColumn == 0)
        return m_pDeviceSrc->GetDisplayIcon();
    else if(iColumn == 1 && m_pDeviceSrc->GetParentCubicle())
        return m_pDeviceSrc->GetParentCubicle()->GetDisplayIcon();
    else if(iColumn == 2 && m_bChecked)
        return m_pDeviceDst->GetDisplayIcon();
    else if(iColumn == 3 && m_bChecked && m_pDeviceDst->GetParentCubicle())
        return m_pDeviceDst->GetParentCubicle()->GetDisplayIcon();

    return QIcon();
}

bool CopyBayItemDevice::GetValid(int iColumn) const
{
    if(iColumn == 2 && m_bChecked)
    {
        if(m_pDeviceDst->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        {
            if(m_pDeviceDst->GetName().isEmpty())
                return false;

            if(ProjectExplorer::PeProjectVersion *pProjectVersion = m_pDeviceDst->GetProjectVersion())
            {
                foreach(ProjectExplorer::PeDevice *pDevice, pProjectVersion->GetAllDevices())
                {
                    if(m_pDeviceDst->GetName() == pDevice->GetName())
                        return false;
                }
            }
        }
    }
    else if(iColumn == 3 && m_bChecked && m_pDeviceDst->GetParentCubicle())
    {
        if(ProjectExplorer::PeCubicle *pCubicleDst = m_pDeviceDst->GetParentCubicle())
        {
            if(pCubicleDst->GetId() == ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
            {
                if(pCubicleDst->GetNumber().isEmpty())
                    return false;

                if(ProjectExplorer::PeProjectVersion *pProjectVersion = pCubicleDst->GetProjectVersion())
                {
                    foreach(ProjectExplorer::PeCubicle *pCubicle, pProjectVersion->GetAllCubicles())
                    {
                        if(pCubicleDst->GetNumber() == pCubicle->GetNumber())
                            return false;
                    }
                }
            }
        }
    }

    return true;
}

ProjectExplorer::PeDevice* CopyBayItemDevice::GetDeviceSrc() const
{
    return m_pDeviceSrc;
}

ProjectExplorer::PeDevice* CopyBayItemDevice::GetDeviceDst() const
{
    return m_pDeviceDst;
}

CopyBayItemBay* CopyBayItemDevice::GetParentItem() const
{
    return m_pParentItem;
}

///////////////////////////////////////////////////////////////////////
// CopyBayModel member functions
///////////////////////////////////////////////////////////////////////
CopyBayModel::CopyBayModel(QObject *pParent) :
    QAbstractItemModel(pParent), m_pProjectVersionSrc(0), m_pProjectVersionDst(0)
{
}

CopyBayModel::~CopyBayModel()
{
    qDeleteAll(m_lstBayItems);
    m_lstBayItems.clear();

    qDeleteAll(m_mapCubicleSrcToDst.values());
    m_mapCubicleSrcToDst.clear();
}

QModelIndex CopyBayModel::index(int row, int column, const QModelIndex &parent) const
{
    if(column >= 0 && column < 4)
    {
        if(!parent.isValid())
        {
            if(row >= 0 && row < m_lstBayItems.size())
                return createIndex(row, column, m_lstBayItems.at(row));
        }
        else if(parent.row() >= 0 && parent.row() < m_lstBayItems.size())
        {
            CopyBayItemBay *pItemBay = m_lstBayItems.at(parent.row());
            if(row >= 0 && row < pItemBay->GetChildItems().size())
                return createIndex(row, column, pItemBay->GetChildItems().at(row));
        }
    }

    return QModelIndex();
}

QModelIndex CopyBayModel::parent(const QModelIndex &index) const
{
    if(CopyBayItem *pItem = ItemFromIndex(index))
    {
        if(CopyBayItemDevice *pItemDevice = dynamic_cast<CopyBayItemDevice*>(pItem))
        {
            if(CopyBayItemBay *pItemBay = pItemDevice->GetParentItem())
            {
                if(m_lstBayItems.contains(pItemBay))
                    return createIndex(m_lstBayItems.indexOf(pItemBay), 0, pItemBay);
            }
        }
    }

    return QModelIndex();
}

int CopyBayModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if(!parent.isValid())
        return m_lstBayItems.size();

    if(CopyBayItemBay *pItemBay = dynamic_cast<CopyBayItemBay*>(ItemFromIndex(parent)))
        return pItemBay->GetChildItems().size();

    return 0;
}

int CopyBayModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 4;
}

QVariant CopyBayModel::data(const QModelIndex &index, int role) const
{
    if(CopyBayItem *pItem = ItemFromIndex(index))
    {
        if(role == Qt::DisplayRole || role == Qt::EditRole)
            return pItem->GetText(index.column());
        else if(role == Qt::DecorationRole)
            return pItem->GetIcon(index.column());
        else if(role == Qt::ForegroundRole)
            return pItem->GetValid(index.column()) ? QColor(Qt::black) : QColor(Qt::red);
        else if(role == Qt::TextAlignmentRole)
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        else if(role == Qt::BackgroundRole)
            return pItem->GetChecked() ? QColor(Qt::magenta) : QColor(Qt::white);
        else if(role == Qt::CheckStateRole && index.column() == 0)
            return pItem->GetChecked() ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

Qt::ItemFlags CopyBayModel::flags(const QModelIndex &index) const
{
    if(index.column() == 0)
        return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;

    return QAbstractItemModel::flags(index);
}

QVariant CopyBayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QStringList lstHeaders = QStringList() << tr("Source Bay Object")
                                                  << tr("Source Cubicle Object")
                                                  << tr("Destination Bay Object")
                                                  << tr("Destination Cubicle Object");
    if(role == Qt::DisplayRole)
    {
        if(orientation == Qt::Horizontal && section >= 0 && section < lstHeaders.size())
            return lstHeaders.at(section);
    }
    else if(role == Qt::TextAlignmentRole)
    {
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }

    return QVariant();
}

bool CopyBayModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    if(role == Qt::CheckStateRole)
    {
        if(CopyBayItem *pItem = ItemFromIndex(idx))
        {
            const bool bChecked = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked);
            if(pItem->GetChecked() != bChecked)
            {
                pItem->SetChecked(bChecked);
                UpdateItem(pItem);

                if(CopyBayItemBay *pItemBay = dynamic_cast<CopyBayItemBay*>(pItem))
                {
                    foreach(CopyBayItemDevice *pChildItem, pItemBay->GetChildItems())
                    {
                        if(pChildItem->GetChecked() != bChecked)
                        {
                            pChildItem->SetChecked(bChecked);
                            UpdateItem(pChildItem);
                        }
                    }
                }
                else if(CopyBayItemDevice *pItemDevice = dynamic_cast<CopyBayItemDevice*>(pItem))
                {
                    if(CopyBayItemBay *pParentItem = pItemDevice->GetParentItem())
                    {
                        bool bHasChildChecked = false;
                        foreach(CopyBayItemDevice *pChildItem, pParentItem->GetChildItems())
                        {
                            if(pChildItem->GetChecked())
                            {
                                bHasChildChecked = true;
                                break;
                            }
                        }

                        if(pParentItem->GetChecked() != bHasChildChecked)
                        {
                            pParentItem->SetChecked(bHasChildChecked);
                            UpdateItem(pParentItem);
                        }
                    }
                }

                emit sigItemCheckStateChanged();
            }            
        }
    }

    return QAbstractItemModel::setData(idx, value, role);
}

ProjectExplorer::PeProjectVersion* CopyBayModel::GetSrcProject() const
{
    return m_pProjectVersionSrc;
}

void CopyBayModel::SetSrcProject(ProjectExplorer::PeProjectVersion *pProjectVersionSrc)
{
    if(m_pProjectVersionSrc == pProjectVersionSrc)
        return;

    qDeleteAll(m_mapCubicleSrcToDst.values());
    m_mapCubicleSrcToDst.clear();

    if(!m_lstBayItems.isEmpty())
    {
        beginRemoveRows(QModelIndex(), 0, m_lstBayItems.size() - 1);
        qDeleteAll(m_lstBayItems);
        m_lstBayItems.clear();
        endRemoveRows();
    }

    m_pProjectVersionSrc = pProjectVersionSrc;
    if(!m_pProjectVersionSrc)
        return;

    QList<ProjectExplorer::PeBay*> lstBays = m_pProjectVersionSrc->GetAllBays();
    qSort(lstBays.begin(), lstBays.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    foreach(ProjectExplorer::PeBay *pBay, lstBays)
    {
        CopyBayItemBay *pItemBay = new CopyBayItemBay(pBay);
        m_lstBayItems.append(pItemBay);

        QList<ProjectExplorer::PeDevice*> lstBayDevices;
        foreach(ProjectExplorer::PeDevice *pBayDevice, pBay->GetChildDevices())
        {
            if(pBayDevice->GetDeviceType() != ProjectExplorer::PeDevice::dtODF)
                lstBayDevices.append(pBayDevice);
        }
        qSort(lstBayDevices.begin(), lstBayDevices.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);

        foreach(ProjectExplorer::PeDevice *pDevice, lstBayDevices)
        {
            CopyBayItemDevice *pItemDevice = new CopyBayItemDevice(pDevice, pItemBay);
            pItemBay->AppendChild(pItemDevice);

            if(ProjectExplorer::PeCubicle *pCubicleSrc = pDevice->GetParentCubicle())
            {
                ProjectExplorer::PeCubicle *pCubicleDst = m_mapCubicleSrcToDst.value(pCubicleSrc, 0);
                if(!pCubicleDst)
                {
                    pCubicleDst = new ProjectExplorer::PeCubicle(*pCubicleSrc);
                    pCubicleDst->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
                    pCubicleDst->SetParentRoom(0);
                    pCubicleDst->SetProjectVersion(0);

                    m_mapCubicleSrcToDst.insert(pCubicleSrc, pCubicleDst);
                }

                pItemDevice->GetDeviceDst()->SetParentCubicle(pCubicleDst);
            }
            else
            {
                pItemDevice->GetDeviceDst()->SetParentCubicle(0);
            }
        }
    }
}

ProjectExplorer::PeProjectVersion* CopyBayModel::GetDstProject() const
{
    return m_pProjectVersionDst;
}

void CopyBayModel::SetDstProject(ProjectExplorer::PeProjectVersion *pProjectVersionDst)
{
    m_pProjectVersionDst = pProjectVersionDst;

    foreach(ProjectExplorer::PeCubicle *pCubicleSrc, m_mapCubicleSrcToDst.keys())
    {
        ProjectExplorer::PeCubicle *pCubicleDst = m_mapCubicleSrcToDst.value(pCubicleSrc);
        if(pCubicleDst->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        {
            *pCubicleDst = *pCubicleSrc;
            pCubicleDst->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        }
        pCubicleDst->SetProjectVersion(m_pProjectVersionDst);
    }

    foreach(CopyBayItemBay *pItemBay, m_lstBayItems)
    {
        ProjectExplorer::PeBay *pBaySrc = pItemBay->GetBaySrc();
        ProjectExplorer::PeBay *pBayDst = pItemBay->GetBayDst();
        if(!pBaySrc || !pBayDst)
            continue;

        if(pBayDst->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
        {
            *pBayDst = *pBaySrc;
            pBayDst->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
        }
        pBayDst->SetProjectVersion(m_pProjectVersionDst);
        UpdateItem(pItemBay);

        foreach(CopyBayItemDevice *pChildItem, pItemBay->GetChildItems())
        {
            ProjectExplorer::PeDevice *pDeviceSrc = pChildItem->GetDeviceSrc();
            ProjectExplorer::PeDevice *pDeviceDst = pChildItem->GetDeviceDst();
            if(!pDeviceSrc || !pDeviceDst)
                continue;

            if(pDeviceDst->GetId() != ProjectExplorer::PeProjectObject::m_iInvalidObjectId)
            {
                *pDeviceDst = *pDeviceSrc;
                pDeviceDst->SetId(ProjectExplorer::PeProjectObject::m_iInvalidObjectId);
            }
            pDeviceDst->SetProjectVersion(m_pProjectVersionDst);
            UpdateItem(pChildItem);
        }
    }
}

CopyBayItem* CopyBayModel::ItemFromIndex(const QModelIndex &index) const
{
    if(index.isValid())
        return static_cast<CopyBayItem*>(index.internalPointer());

    return 0;
}

void CopyBayModel::UpdateItem(CopyBayItem *pItem)
{
    if(CopyBayItemBay *pItemBay = dynamic_cast<CopyBayItemBay*>(pItem))
    {
        const int iRow = m_lstBayItems.indexOf(pItemBay);
        if(iRow >= 0)
            emit dataChanged(index(iRow, 0, QModelIndex()), index(iRow, 3, QModelIndex()));
    }
    else if(CopyBayItemDevice *pItemDevice = dynamic_cast<CopyBayItemDevice*>(pItem))
    {
        if(CopyBayItemBay *pItemBay = pItemDevice->GetParentItem())
        {
            const int iParentRow = m_lstBayItems.indexOf(pItemBay);
            if(iParentRow >= 0)
            {
                QModelIndex indexParent = index(iParentRow, 0, QModelIndex());

                const int iRow = pItemBay->GetChildItems().indexOf(pItemDevice);
                if(iRow >= 0)
                {
                    emit dataChanged(index(iRow, 0, indexParent), index(iRow, 3, indexParent));
                }
            }
        }
    }
}

QList<CopyBayItem*> CopyBayModel::GetCheckedItems() const
{
    QList<CopyBayItem*> lstCheckedItems;

    foreach(CopyBayItemBay *pItemBay, m_lstBayItems)
    {
        if(pItemBay->GetChecked())
            lstCheckedItems.append(pItemBay);

        foreach(CopyBayItemDevice *pItemDevice, pItemBay->GetChildItems())
        {
            if(pItemDevice->GetChecked())
                lstCheckedItems.append(pItemDevice);
        }
    }

    return lstCheckedItems;
}

bool CopyBayModel::GetValid() const
{
    foreach(CopyBayItemBay *pItemBay, m_lstBayItems)
    {
        for(int i = 0; i < columnCount(); i++)
        {
            if(!pItemBay->GetValid(i))
                return false;
        }

        foreach(CopyBayItemDevice *pItemDevice, pItemBay->GetChildItems())
        {
            for(int i = 0; i < columnCount(); i++)
            {
                if(!pItemDevice->GetValid(i))
                    return false;
            }
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////
// CopyBayModel member functions
///////////////////////////////////////////////////////////////////////
CopyBayView::CopyBayView(QWidget *pParent) : Utils::ReadOnlyTreeView(pParent), m_pModel(0)
{
    setMinimumSize(QSize(1200, 250));
    setIndentation(indentation() * 8/10);
    setUniformRowHeights(true);
    setTextElideMode(Qt::ElideNone);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setExpandsOnDoubleClick(false);
}

CopyBayView::~CopyBayView()
{
}

void CopyBayView::SetModel(CopyBayModel *pModel)
{
    if(m_pModel == pModel)
        return;

    m_pModel = pModel;
    setModel(m_pModel);

    setColumnWidth(0, 300);
    setColumnWidth(1, 250);
    setColumnWidth(2, 250);
    setColumnWidth(3, 250);
}
