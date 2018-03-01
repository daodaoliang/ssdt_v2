#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QMessageBox>

#include "utils/waitcursor.h"
#include "projectexplorer/pecubicle.h"
#include "projectexplorer/pecubicleconn.h"
#include "projectexplorer/peprojectversion.h"
#include "projectexplorer/peroom.h"
#include "projectexplorer/pedevice.h"
#include "projectexplorer/peinfoset.h"

#include "widgetcubicleconn.h"

using namespace PhyNetwork::Internal;

WidgetCubicleConn::WidgetCubicleConn(QWidget *pParent) :
    StyledUi::StyledWidget(pParent), m_pProjectVersion(0), m_pCubicleConn(0)
{
    m_pActionModify = new QAction(QIcon(":/phynetwork/images/oper_edit.png"), tr("Modify"), this);
    m_pActionModify->setEnabled(false);
    connect(m_pActionModify, SIGNAL(triggered()), this, SLOT(SlotActionModify()));

    m_pActionUpdate = new QAction(tr("Update Current"), this);
    m_pActionUpdate->setEnabled(false);
    connect(m_pActionUpdate, SIGNAL(triggered()), this, SLOT(SlotActionUpdate()));

    m_pActionUpdateAll = new QAction(tr("Update All"), this);
    m_pActionUpdateAll->setEnabled(false);
    connect(m_pActionUpdateAll, SIGNAL(triggered()), this, SLOT(SlotActionUpdateAll()));

    m_pActionClear = new QAction(tr("Clear Current"), this);
    m_pActionClear->setEnabled(false);
    connect(m_pActionClear, SIGNAL(triggered()), this, SLOT(SlotActionClear()));

    m_pActionClearAll = new QAction(tr("Clear All"), this);
    m_pActionClearAll->setEnabled(false);
    connect(m_pActionClearAll, SIGNAL(triggered()), this, SLOT(SlotActionClearAll()));

    m_pMenuUpdate = new QMenu(tr("Update"), this);
    m_pMenuUpdate->addAction(m_pActionUpdate);
    m_pMenuUpdate->addAction(m_pActionUpdateAll);
    m_pMenuUpdate->menuAction()->setIcon(QIcon(":/phynetwork/images/oper_refresh.png"));
    connect(m_pMenuUpdate->menuAction(), SIGNAL(triggered()), this, SLOT(SlotActionUpdate()));

    m_pMenuClear = new QMenu(tr("Clear"), this);
    m_pMenuClear->addAction(m_pActionClear);
    m_pMenuClear->addAction(m_pActionClearAll);
    m_pMenuClear->menuAction()->setIcon(QIcon(":/phynetwork/images/oper_removeall.png"));
    connect(m_pMenuClear->menuAction(), SIGNAL(triggered()), this, SLOT(SlotActionClear()));

    QHBoxLayout *pHBoxLayoutDetails = new QHBoxLayout(this);
    pHBoxLayoutDetails->addWidget(SetupCurrentCubicle());
    pHBoxLayoutDetails->addWidget(SetupCurrentPassCubicle());
    pHBoxLayoutDetails->addWidget(SetupSidePassCubicle());
    pHBoxLayoutDetails->addWidget(SetupSideCubicle());

    connect(m_pComboBoxCurrentCubicle, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotCurrentCubicleChanged(int)));
    connect(m_pComboBoxSideCubicle, SIGNAL(currentIndexChanged(int)),
            this, SLOT(SlotSideCubicleChanged(int)));
    connect(m_pGroupBoxCurrentPassCubicle, SIGNAL(toggled(bool)),
            this, SLOT(SlotUpdateUseOdfStatus(bool)));
    connect(m_pGroupBoxCurrentPassCubicle, SIGNAL(toggled(bool)),
            this, SLOT(SlotPassCubicleToggled(bool)));
    connect(m_pGroupBoxSidePassCubicle, SIGNAL(toggled(bool)),
            this, SLOT(SlotUpdateUseOdfStatus(bool)));
    connect(m_pGroupBoxSidePassCubicle, SIGNAL(toggled(bool)),
            this, SLOT(SlotPassCubicleToggled(bool)));
}

WidgetCubicleConn::~WidgetCubicleConn()
{
}

void WidgetCubicleConn::SetProjectVersion(ProjectExplorer::PeProjectVersion *pProjectVersion, ProjectExplorer::PeCubicle *pCubicle)
{
    if(m_pProjectVersion != pProjectVersion)
    {
        m_pProjectVersion = pProjectVersion;

        bool bBlock = BlockSignals(true);

        m_pComboBoxCurrentCubicle->clear();
        m_pComboBoxCurrentPassCubicle->clear();
        m_pComboBoxSideCubicle->clear();
        m_pComboBoxSidePassCubicle->clear();

        if(m_pProjectVersion)
            FillCurrentCubicle();

        BlockSignals(bBlock);

        if(m_pComboBoxCurrentCubicle->count() > 1)
            m_pComboBoxCurrentCubicle->setCurrentIndex(1);
        SlotSetCubicleConn(0);
    }

    if(pCubicle)
    {
        int iIndex = m_pComboBoxCurrentCubicle->findData(reinterpret_cast<int>(pCubicle));
        if(iIndex >= 0)
            m_pComboBoxCurrentCubicle->setCurrentIndex(iIndex);
    }
}

QList<QAction*> WidgetCubicleConn::GetActions() const
{
    return QList<QAction*>() << m_pActionModify << m_pMenuUpdate->menuAction() << m_pMenuClear->menuAction();//, m_pActionUpdate << m_pActionUpdateAll << m_pActionClear << m_pActionClearAll;
}

QWidget* WidgetCubicleConn::SetupCurrentCubicle()
{
    m_pComboBoxCurrentCubicle = new QComboBox(this);
    m_pCheckBoxUseCurrentOdf = new QCheckBox(tr("Use ODF"), this);

    QGroupBox *pGroupBoxCurrentCubicle = new QGroupBox(tr("Current Cubicle"), this);
    QVBoxLayout *pVBoxLayoutCurrentCubicle = new QVBoxLayout(pGroupBoxCurrentCubicle);
    pVBoxLayoutCurrentCubicle->addWidget(m_pComboBoxCurrentCubicle);
    pVBoxLayoutCurrentCubicle->addWidget(m_pCheckBoxUseCurrentOdf);

    return pGroupBoxCurrentCubicle;
}

QWidget* WidgetCubicleConn::SetupCurrentPassCubicle()
{
    m_pComboBoxCurrentPassCubicle = new QComboBox(this);

    m_pGroupBoxCurrentPassCubicle = new QGroupBox(tr("Current Pass Cubicle"), this);
    m_pGroupBoxCurrentPassCubicle->setCheckable(true);
    m_pGroupBoxCurrentPassCubicle->setChecked(false);
    QVBoxLayout *pVBoxLayoutPassCubicle = new QVBoxLayout(m_pGroupBoxCurrentPassCubicle);
    pVBoxLayoutPassCubicle->addWidget(m_pComboBoxCurrentPassCubicle);
    pVBoxLayoutPassCubicle->addStretch(1);

    return m_pGroupBoxCurrentPassCubicle;
}

QWidget* WidgetCubicleConn::SetupSideCubicle()
{
    m_pComboBoxSideCubicle = new QComboBox(this);
    m_pCheckBoxUseSideOdf = new QCheckBox(tr("Use ODF"), this);

    QGroupBox *pGroupBoxSideCubicle = new QGroupBox(tr("Side Cubicle"), this);
    QVBoxLayout *pVBoxLayoutSideCubicle = new QVBoxLayout(pGroupBoxSideCubicle);

    pVBoxLayoutSideCubicle->addWidget(m_pComboBoxSideCubicle);
    pVBoxLayoutSideCubicle->addWidget(m_pCheckBoxUseSideOdf);

    return pGroupBoxSideCubicle;
}

QWidget* WidgetCubicleConn::SetupSidePassCubicle()
{
    m_pComboBoxSidePassCubicle = new QComboBox(this);

    m_pGroupBoxSidePassCubicle = new QGroupBox(tr("Side Pass Cubicle"), this);
    m_pGroupBoxSidePassCubicle->setCheckable(true);
    m_pGroupBoxSidePassCubicle->setChecked(false);
    QVBoxLayout *pVBoxLayoutPassCubicle = new QVBoxLayout(m_pGroupBoxSidePassCubicle);
    pVBoxLayoutPassCubicle->addWidget(m_pComboBoxSidePassCubicle);
    pVBoxLayoutPassCubicle->addStretch(1);

    return m_pGroupBoxSidePassCubicle;
}

void WidgetCubicleConn::FillCurrentCubicle()
{
    m_pComboBoxCurrentCubicle->blockSignals(true);

    ProjectExplorer::PeCubicle *pCubicleOldSelected = GetCubicle(m_pComboBoxCurrentCubicle);

    m_pComboBoxCurrentCubicle->clear();

    m_pComboBoxCurrentCubicle->addItem("", 0);
    foreach(ProjectExplorer::PeCubicle *pCubicle, GetAllCubicles(0))
        m_pComboBoxCurrentCubicle->addItem(pCubicle->GetDisplayIcon(), pCubicle->GetDisplayName(), reinterpret_cast<int>(pCubicle));

    int iIndex = m_pComboBoxCurrentCubicle->findData(reinterpret_cast<int>(pCubicleOldSelected));
    if(iIndex >= 0)
        m_pComboBoxCurrentCubicle->setCurrentIndex(iIndex);

    if(GetCubicle(m_pComboBoxCurrentCubicle) != pCubicleOldSelected)
        SlotCurrentCubicleChanged(m_pComboBoxCurrentCubicle->currentIndex());

    m_pComboBoxCurrentCubicle->blockSignals(false);
}

void WidgetCubicleConn::FillCurrentPassCubicle()
{
    m_pComboBoxCurrentPassCubicle->blockSignals(true);

    ProjectExplorer::PeCubicle *pCubicleOldSelected = GetCubicle(m_pComboBoxCurrentPassCubicle);
    ProjectExplorer::PeCubicle *pCubicleCurrent = GetCubicle(m_pComboBoxCurrentCubicle);
    ProjectExplorer::PeProjectObject *pProjectObject = 0;
    if(pCubicleCurrent)
    {
        pProjectObject = pCubicleCurrent->GetParentRoom();
        if(!pProjectObject)
            pProjectObject = pCubicleCurrent->GetProjectVersion();
    }

    m_pComboBoxCurrentPassCubicle->clear();

    m_pComboBoxCurrentPassCubicle->addItem("", 0);
    if(pProjectObject)
    {
        foreach(ProjectExplorer::PeCubicle *pCubicle, GetAllCubicles(pProjectObject))
        {
            if(pCubicle != pCubicleCurrent)
                m_pComboBoxCurrentPassCubicle->addItem(pCubicle->GetDisplayIcon(), pCubicle->GetDisplayName(), reinterpret_cast<int>(pCubicle));
        }
    }

    int iIndex = m_pComboBoxCurrentPassCubicle->findData(reinterpret_cast<int>(pCubicleOldSelected));
    if(iIndex >= 0)
        m_pComboBoxCurrentPassCubicle->setCurrentIndex(iIndex);

    m_pComboBoxCurrentPassCubicle->blockSignals(false);
}

void WidgetCubicleConn::FillSideCubicle()
{
    m_pComboBoxSideCubicle->blockSignals(true);

    ProjectExplorer::PeCubicle *pCubicleOldSelected = GetCubicle(m_pComboBoxSideCubicle);
    ProjectExplorer::PeCubicle *pCubicleCurrent = GetCubicle(m_pComboBoxCurrentCubicle);

    m_pComboBoxSideCubicle->clear();

    m_pComboBoxSideCubicle->addItem("", 0);
    if(pCubicleCurrent)
    {
        foreach(ProjectExplorer::PeCubicle *pCubicle, GetAllCubicles(pCubicleCurrent))
        {
            if(pCubicle != pCubicleCurrent)
                m_pComboBoxSideCubicle->addItem(pCubicle->GetDisplayIcon(), pCubicle->GetDisplayName(), reinterpret_cast<int>(pCubicle));
        }
    }

    int iIndex = m_pComboBoxSideCubicle->findData(reinterpret_cast<int>(pCubicleOldSelected));
    if(iIndex >= 0)
        m_pComboBoxSideCubicle->setCurrentIndex(iIndex);

    if(GetCubicle(m_pComboBoxSideCubicle) != pCubicleOldSelected)
        SlotSideCubicleChanged(m_pComboBoxSideCubicle->currentIndex());

    m_pComboBoxSideCubicle->blockSignals(false);
}

void WidgetCubicleConn::FillSidePassCubicle()
{
    m_pComboBoxSidePassCubicle->blockSignals(true);

    ProjectExplorer::PeCubicle *pCubicleOldSelected = GetCubicle(m_pComboBoxSidePassCubicle);
    ProjectExplorer::PeCubicle *pCubicleSide = GetCubicle(m_pComboBoxSideCubicle);
    ProjectExplorer::PeProjectObject *pProjectObject = 0;
    if(pCubicleSide)
    {
        pProjectObject = pCubicleSide->GetParentRoom();
        if(!pProjectObject)
            pProjectObject = pCubicleSide->GetProjectVersion();
    }

    m_pComboBoxSidePassCubicle->clear();

    m_pComboBoxSidePassCubicle->addItem("", 0);
    if(pProjectObject)
    {
        foreach(ProjectExplorer::PeCubicle *pCubicle, GetAllCubicles(pProjectObject))
        {
            if(pCubicle != pCubicleSide)
                m_pComboBoxSidePassCubicle->addItem(pCubicle->GetDisplayIcon(), pCubicle->GetDisplayName(), reinterpret_cast<int>(pCubicle));
        }
    }

    int iIndex = m_pComboBoxSidePassCubicle->findData(reinterpret_cast<int>(pCubicleOldSelected));
    if(iIndex >= 0)
        m_pComboBoxSidePassCubicle->setCurrentIndex(m_pComboBoxSidePassCubicle->currentIndex());

    m_pComboBoxSidePassCubicle->blockSignals(false);
}

ProjectExplorer::PeCubicle* WidgetCubicleConn::GetCubicle(QComboBox *pComboxCubicle) const
{
    ProjectExplorer::PeCubicle *pCubicle = 0;
    if(pComboxCubicle->currentIndex() >= 0)
        pCubicle = reinterpret_cast<ProjectExplorer::PeCubicle*>(pComboxCubicle->itemData(pComboxCubicle->currentIndex()).toInt());

    return pCubicle;
}

QList<ProjectExplorer::PeCubicle*> WidgetCubicleConn::GetAllCubicles(ProjectExplorer::PeProjectObject *pProjectObject)
{
    QList<ProjectExplorer::PeCubicle*> lstCubicles;

    if(!pProjectObject)
    {
        lstCubicles = m_pProjectVersion->GetAllCubicles();
    }
    else if(ProjectExplorer::PeProjectVersion *pProjectVersion = qobject_cast<ProjectExplorer::PeProjectVersion*>(pProjectObject))
    {
        foreach(ProjectExplorer::PeCubicle *pCubicle, pProjectVersion->GetAllCubicles())
        {
            if(!pCubicle->GetParentRoom())
                lstCubicles.append(pCubicle);
        }
    }
    else if(ProjectExplorer::PeRoom *pRoom = qobject_cast<ProjectExplorer::PeRoom*>(pProjectObject))
    {
        lstCubicles = pRoom->GetChildCubicles();
    }
    else if(ProjectExplorer::PeCubicle *pCubicle = qobject_cast<ProjectExplorer::PeCubicle*>(pProjectObject))
    {
        foreach(ProjectExplorer::PeInfoSet *pInfoSet, m_pProjectVersion->GetAllInfoSets())
        {
            ProjectExplorer::PeCubicle *pCubicleTx = pInfoSet->GetTxIED() ? pInfoSet->GetTxIED()->GetParentCubicle() : 0;
            ProjectExplorer::PeCubicle *pCubicleRx = pInfoSet->GetRxIED() ? pInfoSet->GetRxIED()->GetParentCubicle() : 0;
            ProjectExplorer::PeCubicle *pCubicleSwitch1 = pInfoSet->GetSwitch1() ? pInfoSet->GetSwitch1()->GetParentCubicle() : 0;
            ProjectExplorer::PeCubicle *pCubicleSwitch2 = pInfoSet->GetSwitch2() ? pInfoSet->GetSwitch2()->GetParentCubicle() : 0;
            ProjectExplorer::PeCubicle *pCubicleSwitch3 = pInfoSet->GetSwitch3() ? pInfoSet->GetSwitch3()->GetParentCubicle() : 0;
            ProjectExplorer::PeCubicle *pCubicleSwitch4 = pInfoSet->GetSwitch4() ? pInfoSet->GetSwitch4()->GetParentCubicle() : 0;

            if(pCubicleTx && pCubicleRx && !pCubicleSwitch1 && !pCubicleSwitch2 && !pCubicleSwitch3 && !pCubicleSwitch4)
            {
                if(pCubicleTx == pCubicle && pCubicleRx != pCubicle && !lstCubicles.contains(pCubicleRx))
                    lstCubicles.append(pCubicleRx);
                else if(pCubicleTx != pCubicle && pCubicleRx == pCubicle && !lstCubicles.contains(pCubicleTx))
                    lstCubicles.append(pCubicleTx);
            }
            else if(pCubicleTx && pCubicleRx && pCubicleSwitch1 && !pCubicleSwitch2 && !pCubicleSwitch3 && !pCubicleSwitch4)
            {
                if(pCubicleTx == pCubicle && pCubicleSwitch1 != pCubicle && !lstCubicles.contains(pCubicleSwitch1))
                    lstCubicles.append(pCubicleSwitch1);
                else if(pCubicleTx != pCubicle && pCubicleSwitch1 == pCubicle && !lstCubicles.contains(pCubicleTx))
                    lstCubicles.append(pCubicleTx);

                if(pCubicleSwitch1 == pCubicle && pCubicleRx != pCubicle && !lstCubicles.contains(pCubicleRx))
                    lstCubicles.append(pCubicleRx);
                else if(pCubicleSwitch1 != pCubicle && pCubicleRx == pCubicle && !lstCubicles.contains(pCubicleSwitch1))
                    lstCubicles.append(pCubicleSwitch1);
            }
            else if(pCubicleTx && pCubicleRx && pCubicleSwitch1 && pCubicleSwitch2 && !pCubicleSwitch3 && !pCubicleSwitch4)
            {
                if(pCubicleTx == pCubicle && pCubicleSwitch1 != pCubicle && !lstCubicles.contains(pCubicleSwitch1))
                    lstCubicles.append(pCubicleSwitch1);
                else if(pCubicleTx != pCubicle && pCubicleSwitch1 == pCubicle && !lstCubicles.contains(pCubicleTx))
                    lstCubicles.append(pCubicleTx);

                if(pCubicleSwitch1 == pCubicle && pCubicleSwitch2 != pCubicle && !lstCubicles.contains(pCubicleSwitch2))
                    lstCubicles.append(pCubicleSwitch2);
                else if(pCubicleSwitch1 != pCubicle && pCubicleSwitch2 == pCubicle && !lstCubicles.contains(pCubicleSwitch1))
                    lstCubicles.append(pCubicleSwitch1);

                if(pCubicleSwitch2 == pCubicle && pCubicleRx != pCubicle && !lstCubicles.contains(pCubicleRx))
                    lstCubicles.append(pCubicleRx);
                else if(pCubicleSwitch2 != pCubicle && pCubicleRx == pCubicle && !lstCubicles.contains(pCubicleSwitch2))
                    lstCubicles.append(pCubicleSwitch2);
            }
            else if(pCubicleTx && pCubicleRx && pCubicleSwitch1 && pCubicleSwitch2 && pCubicleSwitch3 && !pCubicleSwitch4)
            {
                if(pCubicleTx == pCubicle && pCubicleSwitch1 != pCubicle && !lstCubicles.contains(pCubicleSwitch1))
                    lstCubicles.append(pCubicleSwitch1);
                else if(pCubicleTx != pCubicle && pCubicleSwitch1 == pCubicle && !lstCubicles.contains(pCubicleTx))
                    lstCubicles.append(pCubicleTx);

                if(pCubicleSwitch1 == pCubicle && pCubicleSwitch2 != pCubicle && !lstCubicles.contains(pCubicleSwitch2))
                    lstCubicles.append(pCubicleSwitch2);
                else if(pCubicleSwitch1 != pCubicle && pCubicleSwitch2 == pCubicle && !lstCubicles.contains(pCubicleSwitch1))
                    lstCubicles.append(pCubicleSwitch1);

                if(pCubicleSwitch2 == pCubicle && pCubicleSwitch3 != pCubicle && !lstCubicles.contains(pCubicleSwitch3))
                    lstCubicles.append(pCubicleSwitch3);
                else if(pCubicleSwitch2 != pCubicle && pCubicleSwitch3 == pCubicle && !lstCubicles.contains(pCubicleSwitch2))
                    lstCubicles.append(pCubicleSwitch2);

                if(pCubicleSwitch3 == pCubicle && pCubicleRx != pCubicle && !lstCubicles.contains(pCubicleRx))
                    lstCubicles.append(pCubicleRx);
                else if(pCubicleSwitch3 != pCubicle && pCubicleRx == pCubicle && !lstCubicles.contains(pCubicleSwitch3))
                    lstCubicles.append(pCubicleSwitch3);
            }
            else if(pCubicleTx && pCubicleRx && pCubicleSwitch1 && pCubicleSwitch2 && pCubicleSwitch3 && pCubicleSwitch4)
            {
                if(pCubicleTx == pCubicle && pCubicleSwitch1 != pCubicle && !lstCubicles.contains(pCubicleSwitch1))
                    lstCubicles.append(pCubicleSwitch1);
                else if(pCubicleTx != pCubicle && pCubicleSwitch1 == pCubicle && !lstCubicles.contains(pCubicleTx))
                    lstCubicles.append(pCubicleTx);

                if(pCubicleSwitch1 == pCubicle && pCubicleSwitch2 != pCubicle && !lstCubicles.contains(pCubicleSwitch2))
                    lstCubicles.append(pCubicleSwitch2);
                else if(pCubicleSwitch1 != pCubicle && pCubicleSwitch2 == pCubicle && !lstCubicles.contains(pCubicleSwitch1))
                    lstCubicles.append(pCubicleSwitch1);

                if(pCubicleSwitch2 == pCubicle && pCubicleSwitch3 != pCubicle && !lstCubicles.contains(pCubicleSwitch3))
                    lstCubicles.append(pCubicleSwitch3);
                else if(pCubicleSwitch2 != pCubicle && pCubicleSwitch3 == pCubicle && !lstCubicles.contains(pCubicleSwitch2))
                    lstCubicles.append(pCubicleSwitch2);

                if(pCubicleSwitch3 == pCubicle && pCubicleSwitch4 != pCubicle && !lstCubicles.contains(pCubicleSwitch4))
                    lstCubicles.append(pCubicleSwitch4);
                else if(pCubicleSwitch3 != pCubicle && pCubicleSwitch4 == pCubicle && !lstCubicles.contains(pCubicleSwitch3))
                    lstCubicles.append(pCubicleSwitch3);

                if(pCubicleSwitch4 == pCubicle && pCubicleRx != pCubicle && !lstCubicles.contains(pCubicleRx))
                    lstCubicles.append(pCubicleRx);
                else if(pCubicleSwitch4 != pCubicle && pCubicleRx == pCubicle && !lstCubicles.contains(pCubicleSwitch4))
                    lstCubicles.append(pCubicleSwitch4);
            }
        }
    }

    qSort(lstCubicles.begin(), lstCubicles.end(), ProjectExplorer::PeProjectObject::CompareDisplayName);
    return lstCubicles;
}

bool WidgetCubicleConn::BlockSignals(bool bBlock)
{
    return m_pComboBoxCurrentCubicle->blockSignals(bBlock) &&
           m_pComboBoxCurrentPassCubicle->blockSignals(bBlock) &&
           m_pComboBoxSideCubicle->blockSignals(bBlock) &&
           m_pComboBoxSidePassCubicle->blockSignals(bBlock) &&
           m_pGroupBoxSidePassCubicle->blockSignals(bBlock);
}

void WidgetCubicleConn::SetUiData(ProjectExplorer::PeCubicleConn *pCubicleConn)
{
    if(!pCubicleConn)
        return;

    ProjectExplorer::PeCubicle *pCurrentCubicle = 0;
    bool bCurrentUseOdf = false;
    ProjectExplorer::PeCubicle *pCurrentPassCubicle = 0;
    ProjectExplorer::PeCubicle *pSideCubicle = 0;
    bool bSideUseOdf = false;
    ProjectExplorer::PeCubicle *pSidePassCubicle = 0;

    if(pCubicleConn->GetCubicle1() == GetCubicle(m_pComboBoxCurrentCubicle))
    {
        pCurrentCubicle = pCubicleConn->GetCubicle1();
        bCurrentUseOdf = pCubicleConn->GetUseOdf1();
        pCurrentPassCubicle = pCubicleConn->GetPassCubicle1();
        pSideCubicle = pCubicleConn->GetCubicle2();
        bSideUseOdf = pCubicleConn->GetUseOdf2();
        pSidePassCubicle = pCubicleConn->GetPassCubicle2();
    }
    else if(pCubicleConn->GetCubicle2() == GetCubicle(m_pComboBoxCurrentCubicle))
    {
        pCurrentCubicle = pCubicleConn->GetCubicle2();
        bCurrentUseOdf = pCubicleConn->GetUseOdf2();
        pCurrentPassCubicle = pCubicleConn->GetPassCubicle2();
        pSideCubicle = pCubicleConn->GetCubicle1();
        bSideUseOdf = pCubicleConn->GetUseOdf1();
        pSidePassCubicle = pCubicleConn->GetPassCubicle1();
    }

    int iCurrentIndex = m_pComboBoxCurrentCubicle->findData(reinterpret_cast<int>(pCurrentCubicle));
    if(iCurrentIndex > 0)
    {
        m_pComboBoxCurrentCubicle->setCurrentIndex(iCurrentIndex);
        m_pCheckBoxUseCurrentOdf->setChecked(bCurrentUseOdf);
    }
    else
    {
        m_pComboBoxCurrentCubicle->setCurrentIndex(0);
    }

    iCurrentIndex = m_pComboBoxCurrentPassCubicle->findData(reinterpret_cast<int>(pCurrentPassCubicle));
    if(iCurrentIndex > 0)
    {
        m_pGroupBoxCurrentPassCubicle->setChecked(true);
        m_pComboBoxCurrentPassCubicle->setCurrentIndex(iCurrentIndex);
    }
    else
    {
        m_pGroupBoxCurrentPassCubicle->setChecked(false);
        m_pComboBoxCurrentPassCubicle->setCurrentIndex(0);
    }

    iCurrentIndex = m_pComboBoxSideCubicle->findData(reinterpret_cast<int>(pSideCubicle));
    if(iCurrentIndex > 0)
    {
        m_pComboBoxSideCubicle->setCurrentIndex(iCurrentIndex);
        m_pCheckBoxUseSideOdf->setChecked(bSideUseOdf);
    }
    else
    {
        m_pComboBoxSideCubicle->setCurrentIndex(0);
    }

    iCurrentIndex = m_pComboBoxSidePassCubicle->findData(reinterpret_cast<int>(pSidePassCubicle));
    if(iCurrentIndex > 0)
    {
        m_pGroupBoxSidePassCubicle->setChecked(true);
        m_pComboBoxSidePassCubicle->setCurrentIndex(iCurrentIndex);
    }
    else
    {
        m_pGroupBoxSidePassCubicle->setChecked(false);
        m_pComboBoxSidePassCubicle->setCurrentIndex(0);
    }
}

bool WidgetCubicleConn::GetUiData(ProjectExplorer::PeCubicleConn *pCubicleConn)
{
    if(!pCubicleConn)
        return false;

    ProjectExplorer::PeCubicle *pCurrentCubicle = GetCubicle(m_pComboBoxCurrentCubicle);
    if(!pCurrentCubicle)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The current cubicle can NOT be empty, please input again."));
        return false;
    }

    ProjectExplorer::PeCubicle *pSideCubicle = GetCubicle(m_pComboBoxSideCubicle);
    if(!pSideCubicle)
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("The side cubicle can NOT be empty, please input again."));
        return false;
    }

    ProjectExplorer::PeCubicle *pCurrentPassCubicle = GetCubicle(m_pComboBoxCurrentPassCubicle);
    ProjectExplorer::PeCubicle *pSidePassCubicle = GetCubicle(m_pComboBoxSidePassCubicle);

    bool bUseCurrentOdf = m_pCheckBoxUseCurrentOdf->isChecked();
    bool bUseSideOdf = m_pCheckBoxUseSideOdf->isChecked();

    pCubicleConn->SetCubicle1(pCurrentCubicle);
    pCubicleConn->SetUseOdf1(bUseCurrentOdf);
    pCubicleConn->SetPassCubicle1(pCurrentPassCubicle);
    pCubicleConn->SetCubicle2(pSideCubicle);
    pCubicleConn->SetUseOdf2(bUseSideOdf);
    pCubicleConn->SetPassCubicle2(pSidePassCubicle);

    return true;
}

void WidgetCubicleConn::UpdateAction()
{
    ProjectExplorer::PeCubicle *pCubicle = GetCubicle(m_pComboBoxCurrentCubicle);

    m_pActionModify->setEnabled(m_pCubicleConn);
    m_pActionUpdate->setEnabled(pCubicle);
    m_pActionUpdateAll->setEnabled(m_pProjectVersion);
    m_pActionClear->setEnabled(pCubicle);
    m_pActionClearAll->setEnabled(m_pProjectVersion);
    m_pMenuUpdate->menuAction()->setEnabled(m_pProjectVersion);
    m_pMenuClear->menuAction()->setEnabled(m_pProjectVersion);
}

bool WidgetCubicleConn::UpdateCubicleConn(ProjectExplorer::PeCubicle *pCubicle)
{
    if(!pCubicle)
        return false;

    ////////////////////////////////////////////////////////
    /// Prepare Cubicle Update
    ////////////////////////////////////////////////////////

    // Get current cubicle connections for 'pCubicle'
    QList<ProjectExplorer::PeCubicleConn*> lstCurrentCubicleConns;
    foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, m_pProjectVersion->GetAllCubicleConns())
    {
        if(pCubicleConn->GetCubicle1() == pCubicle || pCubicleConn->GetCubicle2() == pCubicle)
            lstCurrentCubicleConns.append(pCubicleConn);
    }

    QList<ProjectExplorer::PeCubicle*> lstConnectedCubicles = GetAllCubicles(pCubicle);
    lstConnectedCubicles.removeAll(pCubicle);

    // Get deleted cubicle connections
    QList<ProjectExplorer::PeCubicleConn*> lstDeleteCubicleConns;
    foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, lstCurrentCubicleConns)
    {
        bool bHasCubicleConn = false;
        foreach(ProjectExplorer::PeCubicle *pConnectedCubicle, lstConnectedCubicles)
        {
            if((pCubicleConn->GetCubicle1() == pCubicle && pCubicleConn->GetCubicle2() == pConnectedCubicle) ||
               (pCubicleConn->GetCubicle1() == pConnectedCubicle && pCubicleConn->GetCubicle2() == pCubicle))
            {
                bHasCubicleConn = true;
                break;
            }
        }

        if(!bHasCubicleConn)
        {
            lstDeleteCubicleConns.append(pCubicleConn);
            lstCurrentCubicleConns.removeOne(pCubicleConn);
        }
    }

    // Get created cubicle connections
    QList<ProjectExplorer::PeCubicleConn> lstCreateCubicleConns;
    foreach(ProjectExplorer::PeCubicle *pConnectedCubicle, lstConnectedCubicles)
    {
        bool bHasCubicleConn = false;
        foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, lstCurrentCubicleConns)
        {
            if((pCubicleConn->GetCubicle1() == pCubicle && pCubicleConn->GetCubicle2() == pConnectedCubicle) ||
               (pCubicleConn->GetCubicle1() == pConnectedCubicle && pCubicleConn->GetCubicle2() == pCubicle))
            {
                bHasCubicleConn = true;
                break;
            }
        }

        if(!bHasCubicleConn)
        {
            ProjectExplorer::PeCubicleConn CubicleConn;
            CubicleConn.SetProjectVersion(m_pProjectVersion);
            CubicleConn.SetCubicle1(pCubicle);
            CubicleConn.SetUseOdf1(false);
            CubicleConn.SetCubicle2(pConnectedCubicle);
            CubicleConn.SetUseOdf2(false);

            if((pCubicle->GetParentRoom() != pConnectedCubicle->GetParentRoom()) || (pCubicle->IsYardCubile() && pConnectedCubicle->IsYardCubile()))
            {
                CubicleConn.SetUseOdf1(true);
                CubicleConn.SetUseOdf2(true);
            }

            lstCreateCubicleConns.append(CubicleConn);
        }
    }

    // Validate current cubicle connections and update them if needed
    QList<ProjectExplorer::PeCubicleConn> lstUpdateCubicleConns;
    foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, lstCurrentCubicleConns)
    {
        bool bUsePassCubicle1 = pCubicleConn->GetPassCubicle1();
        bool bUsePassCubicle2 = pCubicleConn->GetPassCubicle2();

        bool bNeedUpdate = false;
        ProjectExplorer::PeCubicleConn CubicleConn(*pCubicleConn);
        if(bUsePassCubicle1 || bUsePassCubicle2)
        {
            if(CubicleConn.GetUseOdf1() == bUsePassCubicle1 || CubicleConn.GetUseOdf2() == bUsePassCubicle2)
            {
                CubicleConn.SetUseOdf1(!bUsePassCubicle1);
                CubicleConn.SetUseOdf2(!bUsePassCubicle2);
                bNeedUpdate = true;
            }
        }
        else if(CubicleConn.GetUseOdf1() != CubicleConn.GetUseOdf2())
        {
            CubicleConn.SetUseOdf2(CubicleConn.GetUseOdf1());
        }

        if(bNeedUpdate)
            lstUpdateCubicleConns.append(CubicleConn);
    }

    ////////////////////////////////////////////////////////
    /// Start Transaction
    ////////////////////////////////////////////////////////
    ProjectExplorer::DbTransaction DbTrans(m_pProjectVersion, true);

    // Create cubicle connctions into database
    for(int i = 0; i < lstCreateCubicleConns.size(); i++)
    {
        ProjectExplorer::PeCubicleConn &CubicleConn = lstCreateCubicleConns[i];
        if(!m_pProjectVersion->DbCreateObject(CubicleConn, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Delete cubicle connctions from database
    foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, lstDeleteCubicleConns)
    {
        if(!m_pProjectVersion->DbDeleteObject(pCubicleConn->GetObjectType(), pCubicleConn->GetId(), false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    // Update cubicle connctions into database
    foreach(const ProjectExplorer::PeCubicleConn &CubicleConn, lstUpdateCubicleConns)
    {
        if(!m_pProjectVersion->DbUpdateObject(CubicleConn, false))
        {
            DbTrans.Rollback();
            return false;
        }
    }

    ////////////////////////////////////////////////////////
    /// Commit Transaction
    ////////////////////////////////////////////////////////
    if(!DbTrans.Commit())
        return false;


    ////////////////////////////////////////////////////////
    /// Handle Object Update
    ////////////////////////////////////////////////////////

    foreach(const ProjectExplorer::PeCubicleConn &CubicleConn, lstCreateCubicleConns)
        m_pProjectVersion->CreateObject(CubicleConn);

    foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, lstDeleteCubicleConns)
        m_pProjectVersion->DeleteObject(pCubicleConn);

    foreach(const ProjectExplorer::PeCubicleConn &CubicleConn, lstUpdateCubicleConns)
        m_pProjectVersion->UpdateObject(CubicleConn);

    return true;
}

bool WidgetCubicleConn::ClearInvalidInfoSet()
{
    if(!m_pProjectVersion)
        return false;

    QList<ProjectExplorer::PeInfoSet*> lstInvalidInfoSets;
    foreach(ProjectExplorer::PeInfoSet *pInfoSet, m_pProjectVersion->GetAllInfoSets())
    {
        if(pInfoSet->GetInfoSetType() == ProjectExplorer::PeInfoSet::itNone && !pInfoSet->GetPairInfoSet())
            lstInvalidInfoSets.append(pInfoSet);
    }

    ProjectExplorer::DbTransaction DbTrans(m_pProjectVersion, true);
    foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstInvalidInfoSets)
    {
        if(!m_pProjectVersion->DbDeleteObject(pInfoSet->GetObjectType(), pInfoSet->GetId(), false))
        {
            DbTrans.Rollback();
            return false;
        }
    }
    if(!DbTrans.Commit())
        return false;

    foreach(ProjectExplorer::PeInfoSet *pInfoSet, lstInvalidInfoSets)
        m_pProjectVersion->DeleteObject(pInfoSet);

    return true;
}

void WidgetCubicleConn::SlotSetCubicleConn(ProjectExplorer::PeCubicleConn *pCubicleConn)
{
    Utils::WaitCursor waitcursor;
    Q_UNUSED(waitcursor)

    m_pCubicleConn = pCubicleConn;
    SetUiData(m_pCubicleConn);

    UpdateAction();
}

void WidgetCubicleConn::SlotCurrentCubicleChanged(int iIndex)
{
    ProjectExplorer::PeCubicle *pCubicle = 0;
    if(iIndex >= 0)
        pCubicle = reinterpret_cast<ProjectExplorer::PeCubicle*>(m_pComboBoxCurrentCubicle->itemData(iIndex).toInt());

    FillCurrentPassCubicle();
    FillSideCubicle();

    emit sigCurrentCubicleChanged(pCubicle);

    UpdateAction();
}

void WidgetCubicleConn::SlotSideCubicleChanged(int iIndex)
{
    Q_UNUSED(iIndex)

    FillSidePassCubicle();
}

void WidgetCubicleConn::SlotPassCubicleToggled(bool bToggled)
{
    Q_UNUSED(bToggled)

    if(!m_pGroupBoxCurrentPassCubicle->isChecked())
        m_pComboBoxCurrentPassCubicle->setCurrentIndex(0);

    m_pGroupBoxSidePassCubicle->setEnabled(m_pGroupBoxCurrentPassCubicle->isChecked());
    if(!m_pGroupBoxSidePassCubicle->isEnabled())
        m_pGroupBoxSidePassCubicle->setChecked(false);
    if(!m_pGroupBoxSidePassCubicle->isChecked())
        m_pComboBoxSidePassCubicle->setCurrentIndex(0);
}

void WidgetCubicleConn::SlotUpdateUseOdfStatus(bool bToggled)
{
    Q_UNUSED(bToggled)

    bool bUseCurrentPassCubicle = m_pGroupBoxCurrentPassCubicle->isChecked();
    bool bUseSidePassCubicle = m_pGroupBoxSidePassCubicle->isChecked();

    if(bUseCurrentPassCubicle || bUseSidePassCubicle)
    {
        m_pCheckBoxUseCurrentOdf->setEnabled(false);
        m_pCheckBoxUseSideOdf->setEnabled(false);

        m_pCheckBoxUseCurrentOdf->setChecked(!bUseCurrentPassCubicle);
        m_pCheckBoxUseSideOdf->setChecked(!bUseSidePassCubicle);
    }
    else
    {
        m_pCheckBoxUseCurrentOdf->setEnabled(true);
        m_pCheckBoxUseSideOdf->setEnabled(true);
    }

    /*if(bUseCurrentPassCubicle && bUseSidePassCubicle)
    {
        m_pCheckBoxUseCurrentOdf->setChecked(false);
        m_pCheckBoxUseCurrentOdf->setEnabled(false);
        m_pCheckBoxUseSideOdf->setChecked(false);
        m_pCheckBoxUseSideOdf->setEnabled(false);
    }
    else if(bUseCurrentPassCubicle && !bUseSidePassCubicle)
    {
        m_pCheckBoxUseCurrentOdf->setChecked(false);
        m_pCheckBoxUseCurrentOdf->setEnabled(false);
        m_pCheckBoxUseSideOdf->setChecked(true);
        m_pCheckBoxUseSideOdf->setEnabled(false);
    }
    else if(!bUseCurrentPassCubicle && bUseSidePassCubicle)
    {
        m_pCheckBoxUseCurrentOdf->setChecked(true);
        m_pCheckBoxUseCurrentOdf->setEnabled(false);
        m_pCheckBoxUseSideOdf->setChecked(false);
        m_pCheckBoxUseSideOdf->setEnabled(false);
    }*/
}

void WidgetCubicleConn::SlotActionModify()
{
    if(!m_pCubicleConn)
        return;

    ProjectExplorer::PeCubicleConn CubicleConn(*m_pCubicleConn);

    if(!GetUiData(&CubicleConn))
        return;

    Utils::WaitCursor cursor;
    Q_UNUSED(cursor)

    if(!m_pProjectVersion->DbUpdateObject(CubicleConn, true))
        return;

    if(m_pProjectVersion->UpdateObject(CubicleConn))
        SlotSetCubicleConn(m_pCubicleConn);
}

void WidgetCubicleConn::SlotActionUpdate()
{
    ProjectExplorer::PeCubicle *pCubicle = GetCubicle(m_pComboBoxCurrentCubicle);
    if(!pCubicle)
        return;

    Utils::WaitCursor waitcursor;
    Q_UNUSED(waitcursor)

    if(ClearInvalidInfoSet())
        UpdateCubicleConn(pCubicle);
}

void WidgetCubicleConn::SlotActionUpdateAll()
{
    if(!m_pProjectVersion->ValidateVersionPwd(this))
        return;

    Utils::WaitCursor waitcursor;
    Q_UNUSED(waitcursor)

    if(ClearInvalidInfoSet())
    {
        foreach(ProjectExplorer::PeCubicle *pCubicle, m_pProjectVersion->GetAllCubicles())
            UpdateCubicleConn(pCubicle);
    }
}

void WidgetCubicleConn::SlotActionClear()
{
    ProjectExplorer::PeCubicle *pCubicle = GetCubicle(m_pComboBoxCurrentCubicle);
    if(!pCubicle)
        return;

    if(QMessageBox::question(this,
                             tr("Clear Confirmation"),
                             tr("Are you sure you want to clear cubicle connections for current cubicle?"),
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No) == QMessageBox::Yes)
    {
        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        // Get all cubicle connections for 'pCubicle'
        QList<ProjectExplorer::PeCubicleConn*> lstCubicleConns;
        foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, m_pProjectVersion->GetAllCubicleConns())
        {
            if(pCubicleConn->GetCubicle1() == pCubicle || pCubicleConn->GetCubicle2() == pCubicle)
                lstCubicleConns.append(pCubicleConn);
        }

        if(lstCubicleConns.isEmpty())
            return;

        ProjectExplorer::DbTransaction DbTrans(m_pProjectVersion, true);
        foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, lstCubicleConns)
        {
            if(!m_pProjectVersion->DbDeleteObject(pCubicleConn->GetObjectType(), pCubicleConn->GetId(), false))
            {
                DbTrans.Rollback();
                return;
            }
        }
        if(!DbTrans.Commit())
            return;

        foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, lstCubicleConns)
            m_pProjectVersion->DeleteObject(pCubicleConn);
    }
}

void WidgetCubicleConn::SlotActionClearAll()
{
    if(!m_pProjectVersion->ValidateVersionPwd(this))
        return;

    if(QMessageBox::question(this,
                             tr("Clear Confirmation"),
                             tr("Are you sure you want to clear all cubicle connections?"),
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No) == QMessageBox::Yes)
    {
        Utils::WaitCursor waitcursor;
        Q_UNUSED(waitcursor)

        ProjectExplorer::DbTransaction DbTrans(m_pProjectVersion, true);
        foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, m_pProjectVersion->GetAllCubicleConns())
        {
            if(!m_pProjectVersion->DbDeleteObject(pCubicleConn->GetObjectType(), pCubicleConn->GetId(), false))
            {
                DbTrans.Rollback();
                return;
            }
        }
        if(!DbTrans.Commit())
            return;

        foreach(ProjectExplorer::PeCubicleConn *pCubicleConn, m_pProjectVersion->GetAllCubicleConns())
            m_pProjectVersion->DeleteObject(pCubicleConn);
    }
}
