#ifndef VLINK_DATA_H
#define VLINK_DATA_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#	if defined(SCD_VLINK_DATA_EXPORT)
#		define EXPORT_SCDVLINKDATA __declspec(dllexport)
#	else
#		define EXPORT_SCDVLINKDATA __declspec(dllimport)
#	endif
#else
#	define EXPORT_SCDVLINKDATA
#endif

#include <QString>
#include <QList>
#include <QMap>
#include <QString>
#include <QProgressBar>
#include <QDialog>
#include <QLabel>
#include <QTextBrowser>
#include <QColor>


enum VLink_Ied_Column
{
	VLink_Ied_Name,
	VLink_Ied_Desc,
	VLink_Ied_DescKey,
	VLink_Ied_Type,
	VLink_Ied_Manufacture,
	VLink_Ied_Version,
	VLink_Ied_ExtRule,
	VLink_Ied_Column_Count,
};

enum VLink_TxIed_Column
{
	VLink_TxIed_Name,
	VLink_TxIed_Desc,
	VLink_TxIed_DescKey,
	VLink_TxIed_Type,
	VLink_TxIed_ExtRule,
	VLink_TxIed_Column_Count,
};

enum Vlink_VTerminal_Column
{
	VLink_VTerminal_Name,
	VLink_VTerminal_Send_Desc,
	VLink_VTerminal_Send_DescKey,
	VLink_VTerminal_Send_Ref,
	VLink_VTerminal_Send_RefKey,
	VLink_VTerminal_Send_ExtRule,
	VLink_VTerminal_Rev_Desc,
	VLink_VTerminal_Rev_DescKey,
	VLink_VTerminal_Rev_Ref,
	VLink_VTerminal_Rev_RefKey,
	VLink_VTerminal_Rev_ExtRule,
	VLink_VTerminal_Column_Count,
};


enum VLink_Match_Type
{
	VLink_Match_Null,
	VLink_Match_And,
	VLink_Match_Or,
};



class CVLink_Process_Dlg : public QDialog
{
	Q_OBJECT
public:
	CVLink_Process_Dlg(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~CVLink_Process_Dlg();

	void createUi();
	void setValue(int value);
	void setLabelText(const QString& text);

private:
	QProgressBar* progressBar;


};

class CVLink_Check_Dlg : public QDialog
{
	Q_OBJECT
public:
	CVLink_Check_Dlg(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~CVLink_Check_Dlg();

	void createUi();
	void setValue(int value);
	void setLabelText(const QString& text);
	void setTextBrowser(const QString& text, QColor clr = QColor(0,0,0));

public slots:
	void slot_highlightCurrentLine();

private:
	QLabel* textLabel;
	QProgressBar* progressBar;
	QTextBrowser* textBrowser;
};


class EXPORT_SCDVLINKDATA VLink_Exp_Match
{
public:
	VLink_Exp_Match();
	~VLink_Exp_Match();
	VLink_Exp_Match& operator=(const VLink_Exp_Match& other);

	void parse_exp_match_string(const QString& expstr);
	bool check_match_string(const QString& src_str);

	void parse_ref_match_string(const QString& refstr);
	bool check_match_ref_string(const QString& ref_str);

public:
	QString m_sMatchString;

	int m_nMatchType;
	QStringList m_sSplitString;
	QList<VLink_Exp_Match*> m_match_list;

	QString m_sRefMatchString;
};


class EXPORT_SCDVLINKDATA VLink_Terminal
{
public:
	VLink_Terminal();
	~VLink_Terminal();
	VLink_Terminal& operator=(const VLink_Terminal& other);

public:
	QString stddesc;
	QString desckey;
	QString stdref;
	QString refkey;
	QString extrule;

	VLink_Exp_Match* vlink_terminal_match;
};

class EXPORT_SCDVLINKDATA VLink_VLink
{
public:
	VLink_VLink();
	~VLink_VLink();
	VLink_VLink& operator=(const VLink_VLink& other);

public:
	QString desc;
	QString mark;

	VLink_Terminal *vlink_tx;
	VLink_Terminal *vlink_rx;
	VLink_Exp_Match* vlink_vlink_match;
};

class EXPORT_SCDVLINKDATA VLink_TxIed
{
public:
	VLink_TxIed();
	~VLink_TxIed();
	VLink_TxIed& operator=(const VLink_TxIed& other);

public:
	QString name;
	QString desc;
	QString desckey;
	QString type;
	QString manufacture;
	QString extrule;

	QList<VLink_VLink*> vlink_vlink_list;
	VLink_Exp_Match* vlink_txied_match;
};

class EXPORT_SCDVLINKDATA VLink_IED
{
public:
	VLink_IED();
	~VLink_IED();
	VLink_IED& operator=(const VLink_IED& other);

public:
	QString name;
	QString desc;
	QString desckey;
	QString type;
	QString manufacture;
	QString configversion;
	QString extrule;

	QList<VLink_TxIed*> vlink_gorx_list;
	QList<VLink_TxIed*> vlink_svrx_list;
	VLink_Exp_Match* vlink_ied_desckey_match;
};

class EXPORT_SCDVLINKDATA VLink_MatchVLink
{
public:
	VLink_MatchVLink();
	~VLink_MatchVLink();
	VLink_MatchVLink& operator=(const VLink_MatchVLink& other);

public:
	QString rx_ied_name;
	QString rx_ied_desc;

	VLink_TxIed* vlink_tx_ied;
};

class EXPORT_SCDVLINKDATA VLink_Template : public QObject
{
	Q_OBJECT
public:
	VLink_Template();
	~VLink_Template();
	VLink_Template& operator=(const VLink_Template& other);

	void clear_vlink_template();
	
	bool write_vlink_template(const QString& sfile);

	//暂时未用
	bool read_vlink_template_from_scd_file(const QString& sfile);
	
	//{{外部接口
	/*
	** 读取单个虚端子连接模板
	** 输入参数：sfile，全路径文件名
	** 返回值：bool，true-成功，false-失败
	*/
	bool read_vlink_template(const QString& sfile);

	/*
	** 读取多个虚端子连接模板
	** 输入参数：sfilelist，全路径文件名列表
	** 返回值：bool，true-成功，false-失败
	*/
	bool read_vlink_template_list(const QStringList& sfilelist);
	
	/* 
	** 读取SCD文件
	** 输入参数：scd_file
	** 返回值：bool，true-成功，false-失败
	*/
	bool read_scd_file(const QString& scd_file);

	/* 
	** 用虚端子连接模板校验SCD文件
	** 返回值：
	*/
	bool check_scd_with_vlink_template();

	/*
	** 根据接收端ied名称和描述，获取匹配的回路
	** 输入参数：rx_ied_desc-接收侧ied描述，rx_ied_name-接收侧ied名称
	** 返回值：VLink_MatchVLink结构
	*/
	VLink_MatchVLink* get_vlink_match_vlink(const QString& rx_ied_desc, const QString& rx_ied_name);

	/*
	** 根据接收端ied名称和描述列表，获取匹配的回路
	** 输入参数：rx_ied_desc_list-接收侧ied描述列表，rx_ied_name_list-接收侧ied名称列表
	** 返回值：VLink_MatchVLink结构
	*/
	VLink_MatchVLink* get_vlink_match_vlink(const QStringList& rx_ied_desc_list, const QStringList& rx_ied_name_list);
	//}}外部接口

public slots:
	void slot_parse_scd_result(int exitCode);

private:
	void scd_to_pro();
	void del_testpro_files(const QString& file_dir);

	bool read_crc_file(const QString& sfile);
	bool read_ied_file(const QString& sfile, VLink_IED* v_ied);
	bool check_ied_file(VLink_IED* v_ied, QList<QMap<QString,QColor> >& check_info);

public:
	QList<VLink_IED*> vlink_ied_list;

private:
	QString m_sScdFile;
	QString m_sScdDir;

	CVLink_Process_Dlg* m_pProgressDlg;
	CVLink_Check_Dlg* m_pCheckDlg;

	QMap<QString,QString> m_ied_name_desc_map;
};






#endif//VLINK_DATA_H