#ifndef VLINK_SELECT_DLG_H
#define VLINK_SELECT_DLG_H

#include "ui_dlg_selectvlink.h"

#include <QDialog>

class VLink_IED;

class Vlink_Select_Dlg : public QDialog, public Ui::selectvlinkform
{
	Q_OBJECT
public:
	Vlink_Select_Dlg(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~Vlink_Select_Dlg();

	void init_ui();
	void init_vlink_ied_info();
	void set_vlink_rx_ied(VLink_IED* rx_ied);
	void set_vlink_rx_ied_list(QList<VLink_IED*> rx_ied_list);

	QString get_sel_rx_ied_name();
	QString get_sel_tx_ied_name();
	QString get_sel_rx_ied_type();

public slots:
	void slot_ied_tree_clicked(QTreeWidgetItem* item,int nIdx);
	void slot_ied_table_vlink_clicked(QTableWidgetItem * item);
	void slot_ied_table_vlink_doubleclicked(QTableWidgetItem * item);

	void on_pushButton_confirm_clicked();
	void on_pushButton_cancel_clicked();

private:
	QList<VLink_IED*> m_vlink_ied_list;

	QString m_sel_rx_ied_name;
	QString m_sel_rx_ied_type;
	QString m_sel_tx_ied_name;
};


#endif//VLINK_SELECT_DLG_H