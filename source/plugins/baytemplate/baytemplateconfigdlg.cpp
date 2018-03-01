#include <QTabBar>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "projectexplorer/pbtpbay.h"

#include "baytemplateconfigdlg.h"
#include "configwidgettpdevice.h"
#include "configwidgettpinfoset.h"

using namespace BayTemplate::Internal;

BayTemplateConfigDlg::BayTemplateConfigDlg(ProjectExplorer::PbTpBay *pBay, QWidget *pParent) :
    StyledUi::StyledDialog(pParent), m_pBay(pBay)
{
    SetWindowTitle(tr("Bay Configuration"));

    m_pTabBar = new QTabBar(this);
    m_pTabBar->setDrawBase(false);
    m_pTabBar->setExpanding(false);
    m_pTabBar->addTab(tr("Device Object"));
    m_pTabBar->addTab(tr("Infomation Logic"));

    m_pStackedWidget = new QStackedWidget(this);

    QDialogButtonBox *pDialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    connect(pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *pLayout = GetClientLayout();
    pLayout->setContentsMargins(10, 10, 10, 10);
    pLayout->addWidget(m_pTabBar);
    pLayout->addWidget(m_pStackedWidget);
    pLayout->addSpacing(15);
    pLayout->addWidget(pDialogButtonBox);

    connect(m_pTabBar, SIGNAL(currentChanged(int)), this, SLOT(SlotCurrentTabChanged(int)));

    SlotCurrentTabChanged(0);
}

QSize BayTemplateConfigDlg::sizeHint() const
{
    return QSize(1050, 600);
}

void BayTemplateConfigDlg::SlotCurrentTabChanged(int iCurrentIndex)
{
    while(m_pStackedWidget->count())
    {
        QWidget *pWidget = m_pStackedWidget->widget(0);
        m_pStackedWidget->removeWidget(pWidget);
        delete pWidget;
    }

    if(iCurrentIndex == 0)
        m_pStackedWidget->addWidget(new ConfigWidgetTpDevice(m_pBay, this));
    else if(iCurrentIndex == 1)
        m_pStackedWidget->addWidget(new ConfigWidgetTpInfoSet(m_pBay, this));
}
