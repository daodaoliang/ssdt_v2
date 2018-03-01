#include "vlinkselectdlg.h"
#include "vlinkdata.h"


Vlink_Select_Dlg::Vlink_Select_Dlg(QWidget* parent /* = 0 */, Qt::WindowFlags f /* = 0 */)
:QDialog(parent,f)
{
	setupUi(this);

	m_vlink_ied_list.clear();

	init_ui();
}

Vlink_Select_Dlg::~Vlink_Select_Dlg()
{

}

void Vlink_Select_Dlg::init_ui()
{
	splitter_main->setStretchFactor(0,1);
	splitter_main->setStretchFactor(1,4);

	treeWidget->header()->hide();
	connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),this, SLOT(slot_ied_tree_clicked(QTreeWidgetItem*,int)));

	tableWidget_vlink->clear();
	tableWidget_vlink->setColumnCount(5);
	tableWidget_vlink->setHorizontalHeaderItem(0,new QTableWidgetItem(tr("虚链路类型")));
	tableWidget_vlink->setHorizontalHeaderItem(1,new QTableWidgetItem(tr("名称")));
	tableWidget_vlink->setHorizontalHeaderItem(2,new QTableWidgetItem(tr("描述")));
	tableWidget_vlink->setHorizontalHeaderItem(3,new QTableWidgetItem(tr("描述关键字")));
	tableWidget_vlink->setHorizontalHeaderItem(4,new QTableWidgetItem(tr("类型")));
	tableWidget_vlink->setAlternatingRowColors(true);
	tableWidget_vlink->setEditTriggers(QTableView::NoEditTriggers);
	tableWidget_vlink->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget_vlink->setSelectionMode(QAbstractItemView::SingleSelection);
	tableWidget_vlink->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	connect(tableWidget_vlink, SIGNAL(itemClicked(QTableWidgetItem *)),this, SLOT(slot_ied_table_vlink_clicked(QTableWidgetItem *)));
	connect(tableWidget_vlink, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this, SLOT(slot_ied_table_vlink_doubleclicked(QTableWidgetItem *)));

	tableWidget_vterminal->clear();
	tableWidget_vterminal->setColumnCount(9);
	tableWidget_vterminal->setHorizontalHeaderItem(0,new QTableWidgetItem(tr("虚端子名")));
	tableWidget_vterminal->setHorizontalHeaderItem(1,new QTableWidgetItem(tr("发送侧\n描述")));
	tableWidget_vterminal->setHorizontalHeaderItem(2,new QTableWidgetItem(tr("发送侧\n描述关键字")));
	tableWidget_vterminal->setHorizontalHeaderItem(3,new QTableWidgetItem(tr("发送则\n引用名")));
	tableWidget_vterminal->setHorizontalHeaderItem(4,new QTableWidgetItem(tr("发送侧\n引用名关键字")));
	tableWidget_vterminal->setHorizontalHeaderItem(5,new QTableWidgetItem(tr("接收侧\n描述")));
	tableWidget_vterminal->setHorizontalHeaderItem(6,new QTableWidgetItem(tr("接收侧\n描述关键字")));
	tableWidget_vterminal->setHorizontalHeaderItem(7,new QTableWidgetItem(tr("接收则\n引用名")));
	tableWidget_vterminal->setHorizontalHeaderItem(8,new QTableWidgetItem(tr("接收侧\n引用名关键字")));
	tableWidget_vterminal->setAlternatingRowColors(true);
	tableWidget_vterminal->setEditTriggers(QTableView::NoEditTriggers);
	tableWidget_vterminal->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget_vterminal->setSelectionMode(QAbstractItemView::SingleSelection);
	tableWidget_vterminal->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	pushButton_confirm->setText(tr("确定"));
	pushButton_cancel->setText(tr("取消"));
	setWindowTitle(tr("选择IED"));
}

void Vlink_Select_Dlg::set_vlink_rx_ied(VLink_IED* rx_ied)
{
	m_vlink_ied_list.clear();
	m_vlink_ied_list.append(rx_ied);

	init_vlink_ied_info();
}

void Vlink_Select_Dlg::set_vlink_rx_ied_list(QList<VLink_IED*> rx_ied_list)
{
	m_vlink_ied_list = rx_ied_list;

	init_vlink_ied_info();
}

void Vlink_Select_Dlg::init_vlink_ied_info()
{
	treeWidget->clear();
	tableWidget_vlink->clearContents();
	tableWidget_vterminal->clearContents();
	tableWidget_vlink->setRowCount(0);
	tableWidget_vterminal->setRowCount(0);

	QTreeWidgetItem *ied_item = 0;
	for (int i = 0; i < m_vlink_ied_list.count(); i++)
	{
		VLink_IED* v_ied = m_vlink_ied_list[i];
		ied_item = new QTreeWidgetItem(treeWidget);
		ied_item->setText(0, QString("%1:%2").arg(v_ied->name).arg(v_ied->desc));
		ied_item->setData(0,Qt::UserRole,v_ied->name);
		ied_item->setIcon(0,QIcon(":/images/substation.png"));
	}
}

void Vlink_Select_Dlg::slot_ied_tree_clicked(QTreeWidgetItem* item,int nIdx)
{
	if(item)
	{
		VLink_IED* p_ied = 0;
		QString ied_name = item->data(0,Qt::UserRole).toString();
		for (int i = 0; i < m_vlink_ied_list.count(); i++)
		{
			VLink_IED* v_ied = m_vlink_ied_list[i];
			if(v_ied->name == ied_name)
			{
				p_ied = v_ied;
				break;
			}
		}

		if(p_ied)
		{
			tableWidget_vlink->clearContents();
			tableWidget_vlink->setRowCount(0);
			int sv_num = p_ied->vlink_svrx_list.count();
			int gos_num = p_ied->vlink_gorx_list.count();
			tableWidget_vlink->setRowCount(sv_num+gos_num);
			QTableWidgetItem *item = 0;
			for (int i = 0; i < p_ied->vlink_svrx_list.count(); i++)
			{
				VLink_TxIed *v_txied = p_ied->vlink_svrx_list[i];

				item = new QTableWidgetItem;
				item->setText(tr("SV"));
				item->setData(Qt::UserRole,v_txied->name);
				item->setCheckState(Qt::Unchecked);
				tableWidget_vlink->setItem(i,0,item);

				item = new QTableWidgetItem;
				item->setText(v_txied->name);
				tableWidget_vlink->setItem(i,1,item);

				item = new QTableWidgetItem;
				item->setText(v_txied->desc);
				tableWidget_vlink->setItem(i,2,item);

				item = new QTableWidgetItem;
				item->setText(v_txied->desckey);
				tableWidget_vlink->setItem(i,3,item);

				item = new QTableWidgetItem;
				item->setText(v_txied->type);
				tableWidget_vlink->setItem(i,4,item);
			}
			for (int i = 0; i < p_ied->vlink_gorx_list.count(); i++)
			{
				VLink_TxIed *v_txied = p_ied->vlink_gorx_list[i];

				item = new QTableWidgetItem;
				item->setText(tr("GOOSE"));
				item->setData(Qt::UserRole,v_txied->name);
				item->setCheckState(Qt::Unchecked);
				tableWidget_vlink->setItem(i+sv_num,0,item);

				item = new QTableWidgetItem;
				item->setText(v_txied->name);
				tableWidget_vlink->setItem(i+sv_num,1,item);

				item = new QTableWidgetItem;
				item->setText(v_txied->desc);
				tableWidget_vlink->setItem(i+sv_num,2,item);

				item = new QTableWidgetItem;
				item->setText(v_txied->desckey);
				tableWidget_vlink->setItem(i+sv_num,3,item);

				item = new QTableWidgetItem;
				item->setText(v_txied->type);
				tableWidget_vlink->setItem(i+sv_num,4,item);
			}
			tableWidget_vlink->resizeColumnsToContents();
			tableWidget_vlink->setColumnWidth(0,80);
			tableWidget_vlink->setColumnWidth(1,100);
			tableWidget_vlink->setColumnWidth(2,150);
			tableWidget_vlink->setColumnWidth(3,150);
			tableWidget_vlink->setColumnWidth(4,50);
		}
	}
}

void Vlink_Select_Dlg::slot_ied_table_vlink_clicked(QTableWidgetItem * item)
{
	tableWidget_vterminal->clearContents();
	tableWidget_vterminal->setRowCount(0);

	QString ied_name = treeWidget->currentItem()->data(0,Qt::UserRole).toString();

	int nrow = tableWidget_vlink->currentRow();
	if(nrow < 0)
		return;

	for (int i = 0; i < tableWidget_vlink->rowCount(); i++)
	{
		tableWidget_vlink->item(i,0)->setCheckState(Qt::Unchecked);
	}
	tableWidget_vlink->item(nrow,0)->setCheckState(Qt::Checked);

	QString stype = tableWidget_vlink->item(nrow,0)->text();
	QString stxied = tableWidget_vlink->item(nrow,1)->text();

	m_sel_rx_ied_type = stype;
	m_sel_rx_ied_name = ied_name;

	VLink_IED* p_ied = 0;
	for (int i = 0; i < m_vlink_ied_list.count(); i++)
	{
		VLink_IED* v_ied = m_vlink_ied_list[i];
		if(v_ied->name == ied_name)
		{
			p_ied = v_ied;
			break;
		}
	}

	if(p_ied)
	{
		VLink_TxIed* p_txied = 0;
		if(stype == "GOOSE")
		{
			for (int i = 0; i < p_ied->vlink_gorx_list.count(); i++)
			{
				VLink_TxIed *v_txied = p_ied->vlink_gorx_list[i];
				if(v_txied->name == stxied)
				{
					p_txied = v_txied;
					break;
				}
			}
		}
		else if(stype == "SV")
		{
			for (int i = 0; i < p_ied->vlink_svrx_list.count(); i++)
			{
				VLink_TxIed *v_txied = p_ied->vlink_svrx_list[i];
				if(v_txied->name == stxied)
				{
					p_txied = v_txied;
					break;
				}
			}
		}
		if(p_txied)
		{
			tableWidget_vterminal->clearContents();
			tableWidget_vterminal->setRowCount(0);
			tableWidget_vterminal->setRowCount(p_txied->vlink_vlink_list.count());

			m_sel_tx_ied_name = p_txied->name;

			QTableWidgetItem *item = 0;
			for (int i = 0; i < p_txied->vlink_vlink_list.count(); i++)
			{
				VLink_VLink *pvlink = p_txied->vlink_vlink_list[i];

				item = new QTableWidgetItem;
				item->setText(pvlink->desc);
				tableWidget_vterminal->setItem(i,0,item);

				item = new QTableWidgetItem;
				item->setText(pvlink->vlink_tx->stddesc);
				tableWidget_vterminal->setItem(i,1,item);

				item = new QTableWidgetItem;
				item->setText(pvlink->vlink_tx->desckey);
				tableWidget_vterminal->setItem(i,2,item);

				item = new QTableWidgetItem;
				item->setText(pvlink->vlink_tx->stdref);
				tableWidget_vterminal->setItem(i,3,item);

				item = new QTableWidgetItem;
				item->setText(pvlink->vlink_tx->refkey);
				tableWidget_vterminal->setItem(i,4,item);

				item = new QTableWidgetItem;
				item->setText(pvlink->vlink_rx->stddesc);
				tableWidget_vterminal->setItem(i,5,item);

				item = new QTableWidgetItem;
				item->setText(pvlink->vlink_rx->desckey);
				tableWidget_vterminal->setItem(i,6,item);

				item = new QTableWidgetItem;
				item->setText(pvlink->vlink_rx->stdref);
				tableWidget_vterminal->setItem(i,7,item);

				item = new QTableWidgetItem;
				item->setText(pvlink->vlink_rx->refkey);
				tableWidget_vterminal->setItem(i,8,item);
			}
			tableWidget_vterminal->resizeColumnsToContents();
		}
	}
}

void Vlink_Select_Dlg::slot_ied_table_vlink_doubleclicked(QTableWidgetItem * item)
{
	accept();
}

void Vlink_Select_Dlg::on_pushButton_confirm_clicked()
{
	accept();
}

void Vlink_Select_Dlg::on_pushButton_cancel_clicked()
{
	reject();
}

QString Vlink_Select_Dlg::get_sel_rx_ied_name()
{
	return m_sel_rx_ied_name;
}

QString Vlink_Select_Dlg::get_sel_tx_ied_name()
{
	return m_sel_tx_ied_name;
}

QString Vlink_Select_Dlg::get_sel_rx_ied_type()
{
	return m_sel_rx_ied_type;
}

