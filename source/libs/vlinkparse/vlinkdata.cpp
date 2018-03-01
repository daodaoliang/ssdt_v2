#include "vlinkdata.h"
#include "vlinkselectdlg.h"

#include <QDomDocument>
#include <QFile>
#include <QDomElement>
#include <QDomNode>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QPalette>
#include <QVBoxLayout>
#include <QProcess>
#include <QDir>
#include <QFileInfoList>
#include <QApplication>
#include <QDebug>


CVLink_Process_Dlg::CVLink_Process_Dlg(QWidget* parent/* = 0*/, Qt::WindowFlags f/* = 0*/)
:QDialog(parent,f)
{
	setObjectName("PadVoltageCurrentKairuLiang");
	//setWindowFlags(Qt::Widget|Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
	setAutoFillBackground(true);

	createUi();

	QPalette pal = palette();
	pal.setBrush(QPalette::Window,QColor(Qt::gray));
	setPalette(pal);

	setFixedSize(300,50);
}

CVLink_Process_Dlg::~CVLink_Process_Dlg()
{

}

void CVLink_Process_Dlg::createUi()
{
	QLabel* label1 = new QLabel(this);
	label1->setObjectName("progressText");
	label1->setAlignment(Qt::AlignCenter);
	label1->setText(tr("..."));
	QPalette pal = label1->palette();
	pal.setColor(QPalette::WindowText,QColor(Qt::black));
	label1->setPalette(pal);

	progressBar = new QProgressBar(this);
	progressBar->setTextVisible(false);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(3);
	layout->addWidget(label1);
	layout->addWidget(progressBar);

	setLayout(layout);
}

void CVLink_Process_Dlg::setValue(int value)
{
	if(progressBar)
		progressBar->setValue(value);
}

void CVLink_Process_Dlg::setLabelText(const QString& text)
{
	QString objName = QString("progressText");
	QLabel* label = this->findChild<QLabel*>(objName);
	if(label)
	{
		label->setText(text);
	}
}






CVLink_Check_Dlg::CVLink_Check_Dlg(QWidget* parent /* = 0 */, Qt::WindowFlags f /* = 0 */)
:QDialog(parent,f)
{
	setAutoFillBackground(true);

	createUi();

	QPalette pal = palette();
	pal.setBrush(QPalette::Window,QColor(Qt::gray));
	setPalette(pal);

	//setFixedSize(600,500);
	setMinimumSize(600,500);
}

CVLink_Check_Dlg::~CVLink_Check_Dlg()
{

}

void CVLink_Check_Dlg::createUi()
{
	textLabel = new QLabel(this);
	textLabel->setAlignment(Qt::AlignCenter);
	textLabel->setText(tr("..."));
	QPalette pal = textLabel->palette();
	pal.setColor(QPalette::WindowText,QColor(Qt::black));
	textLabel->setPalette(pal);

	progressBar = new QProgressBar(this);
	progressBar->setTextVisible(false);

	textBrowser = new QTextBrowser(this);
	textBrowser->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	textBrowser->clear();
	connect(textBrowser, SIGNAL(cursorPositionChanged()), this, SLOT(slot_highlightCurrentLine()));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(3);
	layout->addWidget(textLabel);
	layout->addWidget(progressBar);
	layout->addWidget(textBrowser);

	setLayout(layout);
}

void CVLink_Check_Dlg::setValue(int value)
{
	if(progressBar)
		progressBar->setValue(value);
}

void CVLink_Check_Dlg::setLabelText(const QString& text)
{
	if(textLabel)
	{
		textLabel->setText(text);
	}
}

void CVLink_Check_Dlg::setTextBrowser(const QString& text, QColor clr)
{
	if(textBrowser)
	{
		textBrowser->setTextColor(clr);
		textBrowser->append(text);
	}
}

void CVLink_Check_Dlg::slot_highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (textBrowser->isReadOnly())
	{
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::yellow).lighter(160);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textBrowser->textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	textBrowser->setExtraSelections(extraSelections);
}






VLink_Exp_Match::VLink_Exp_Match()
{
	m_sMatchString = "";
	m_nMatchType = VLink_Match_Null;
	m_match_list.clear();

	m_sRefMatchString = "";
}

VLink_Exp_Match::~VLink_Exp_Match()
{

}

VLink_Exp_Match& VLink_Exp_Match::operator=(const VLink_Exp_Match& other)
{
	if(this == &other)
		return *this;

	m_sMatchString = other.m_sMatchString;
	m_nMatchType = other.m_nMatchType;
	m_match_list = other.m_match_list;

	return *this;
}

void VLink_Exp_Match::parse_exp_match_string(const QString& expstr)
{
	qDeleteAll(m_match_list);
	m_match_list.clear();
	m_sSplitString.clear();
	m_sMatchString = "";


	if(expstr.trimmed().isEmpty())
		return;

	m_sMatchString = expstr.trimmed();
	QString compute_str = m_sMatchString;

	int left_kuohao = compute_str.count(QRegExp("\\{"));
	int right_kuohao = compute_str.count(QRegExp("\\{"));
	if(left_kuohao == 0 && right_kuohao == 0)
	{
		int and_cnt = compute_str.count("and",Qt::CaseInsensitive);
		int or_cnt = compute_str.count("or",Qt::CaseInsensitive);
		if(and_cnt > 0)
		{
			m_nMatchType = VLink_Match_And;
			m_sSplitString = compute_str.remove(" ").split("and",QString::SkipEmptyParts,Qt::CaseInsensitive);
		}
		else if(or_cnt > 0)
		{
			m_nMatchType = VLink_Match_Or;
			m_sSplitString = compute_str.remove(" ").split("or",QString::SkipEmptyParts,Qt::CaseInsensitive);
		}
		else
		{
			m_nMatchType = VLink_Match_And;
			m_sSplitString.append(compute_str.remove(" "));
		}
	}
	else if(left_kuohao == 1 && right_kuohao == 1)
	{
		int left_kuohao_index = compute_str.indexOf(QRegExp("\\{"));
		int right_kuohao_index = compute_str.indexOf(QRegExp("\\}"));
		QString left_str = compute_str.mid(0,left_kuohao_index-1);
		if(left_kuohao_index == 0)
			left_str = "";
		QString mid_str = compute_str.mid(left_kuohao_index+1,right_kuohao_index-left_kuohao_index-1);
		QString right_str = compute_str.mid(right_kuohao_index+1);

		if(!left_str.trimmed().isEmpty())
		{
			int and_cnt = left_str.count("and",Qt::CaseInsensitive);
			int or_cnt = left_str.count("or",Qt::CaseInsensitive);
			if(and_cnt > 0)
			{
				m_nMatchType = VLink_Match_And;
				m_sSplitString.append(left_str.remove(" ").split("and",QString::SkipEmptyParts,Qt::CaseInsensitive));
			}
			else if(or_cnt > 0)
			{
				m_nMatchType = VLink_Match_Or;
				m_sSplitString.append(left_str.remove(" ").split("or",QString::SkipEmptyParts,Qt::CaseInsensitive));
			}
			else
			{
				m_nMatchType = VLink_Match_And;
				m_sSplitString.append(left_str.remove(" "));
			}
		}

		if(!mid_str.trimmed().isEmpty())
		{
			VLink_Exp_Match* vlink_match = new VLink_Exp_Match;
			vlink_match->parse_exp_match_string(mid_str.trimmed());
			m_match_list.append(vlink_match);
		}

		if(!right_str.trimmed().isEmpty())
		{
			int and_cnt = right_str.count("and",Qt::CaseInsensitive);
			int or_cnt = right_str.count("or",Qt::CaseInsensitive);
			if(and_cnt > 0)
			{
				m_nMatchType = VLink_Match_And;
				m_sSplitString.append(right_str.remove(" ").split("and",QString::SkipEmptyParts,Qt::CaseInsensitive));
			}
			else if(or_cnt > 0)
			{
				m_nMatchType = VLink_Match_Or;
				m_sSplitString.append(right_str.remove(" ").split("or",QString::SkipEmptyParts,Qt::CaseInsensitive));
			}
			else
			{
				m_nMatchType = VLink_Match_And;
				m_sSplitString.append(right_str.remove(" "));
			}
		}

	}
	else if(left_kuohao == 2 && right_kuohao == 2)
	{
		int left_kuohao_index = compute_str.lastIndexOf(QRegExp("\\{"));
		int right_kuohao_index = compute_str.indexOf(QRegExp("\\}"));
		if(left_kuohao_index > right_kuohao_index)
		{
			int left_kuohao_index1 = compute_str.indexOf(QRegExp("\\{"));
			int left_kuohao_index2 = compute_str.lastIndexOf(QRegExp("\\{"));
			int right_kuohao_index1 = compute_str.indexOf(QRegExp("\\}"));
			int right_kuohao_index2 = compute_str.lastIndexOf(QRegExp("\\}"));
			QString str1 = compute_str.mid(0,left_kuohao_index1-1);
			QString str2 = compute_str.mid(left_kuohao_index1+1,right_kuohao_index1-left_kuohao_index1-1);
			QString str3 = compute_str.mid(right_kuohao_index1+1,left_kuohao_index2-right_kuohao_index1-1);
			QString str4 = compute_str.mid(left_kuohao_index2+1,right_kuohao_index2-left_kuohao_index2-1);
			QString str5 = compute_str.mid(right_kuohao_index2+1);

			if(left_kuohao_index1 == 0)
				str1 = "";

			if(!str1.trimmed().isEmpty())
			{
				int and_cnt = str1.count("and",Qt::CaseInsensitive);
				int or_cnt = str1.count("or",Qt::CaseInsensitive);
				if(and_cnt > 0)
				{
					m_nMatchType = VLink_Match_And;
					m_sSplitString.append(str1.remove(" ").split("and",QString::SkipEmptyParts,Qt::CaseInsensitive));
				}
				else if(or_cnt > 0)
				{
					m_nMatchType = VLink_Match_Or;
					m_sSplitString.append(str1.remove(" ").split("or",QString::SkipEmptyParts,Qt::CaseInsensitive));
				}
				else
				{
					m_nMatchType = VLink_Match_And;
					m_sSplitString.append(str1.remove(" "));
				}
			}

			if(!str2.trimmed().isEmpty())
			{
				VLink_Exp_Match* vlink_match = new VLink_Exp_Match;
				vlink_match->parse_exp_match_string(str2.trimmed());
				m_match_list.append(vlink_match);
			}

			if(!str3.trimmed().isEmpty())
			{
				int and_cnt = str3.count("and",Qt::CaseInsensitive);
				int or_cnt = str3.count("or",Qt::CaseInsensitive);
				if(and_cnt > 0)
				{
					m_nMatchType = VLink_Match_And;
					m_sSplitString.append(str3.remove(" ").split("and",QString::SkipEmptyParts,Qt::CaseInsensitive));
				}
				else if(or_cnt > 0)
				{
					m_nMatchType = VLink_Match_Or;
					m_sSplitString.append(str3.remove(" ").split("or",QString::SkipEmptyParts,Qt::CaseInsensitive));
				}
				else
				{
					m_nMatchType = VLink_Match_And;
					m_sSplitString.append(str3.remove(" "));
				}
			}

			if(!str4.trimmed().isEmpty())
			{
				VLink_Exp_Match* vlink_match = new VLink_Exp_Match;
				vlink_match->parse_exp_match_string(str4.trimmed());
				m_match_list.append(vlink_match);
			}

			if(!str5.trimmed().isEmpty())
			{
				int and_cnt = str5.count("and",Qt::CaseInsensitive);
				int or_cnt = str5.count("or",Qt::CaseInsensitive);
				if(and_cnt > 0)
				{
					m_nMatchType = VLink_Match_And;
					m_sSplitString.append(str5.remove(" ").split("and",QString::SkipEmptyParts,Qt::CaseInsensitive));
				}
				else if(or_cnt > 0)
				{
					m_nMatchType = VLink_Match_Or;
					m_sSplitString.append(str5.remove(" ").split("or",QString::SkipEmptyParts,Qt::CaseInsensitive));
				}
				else
				{
					m_nMatchType = VLink_Match_And;
					m_sSplitString.append(str5.remove(" "));
				}
			}
		}
		else
		{
			;
		}
	}

// 	qDebug()<<m_sMatchString;
// 	qDebug()<<m_sSplitString;
}

bool VLink_Exp_Match::check_match_string(const QString& src_str)
{
	bool bFind = false;
	if(m_nMatchType == VLink_Match_Null)
	{
		return false;
	}
	else if(m_nMatchType == VLink_Match_And)
	{
		bFind = true;
		for (int i = 0; i < m_sSplitString.count(); i++)
		{
			QString match_str = m_sSplitString[i];
			bool item_find = src_str.contains(QRegExp(m_sSplitString[i],Qt::CaseInsensitive));
			bFind &= item_find;
		}
		for (int i = 0; i < m_match_list.count(); i++)
		{
			VLink_Exp_Match* exp_match = m_match_list[i];
			bFind &= m_match_list[i]->check_match_string(src_str);
		}
	}
	else if(m_nMatchType == VLink_Match_Or)
	{
		bFind = false;
		for (int i = 0; i < m_sSplitString.count(); i++)
		{
			QString match_str = m_sSplitString[i];
			bool item_find = src_str.contains(QRegExp(m_sSplitString[i],Qt::CaseInsensitive));
			bFind |= item_find;
		}
		for (int i = 0; i < m_match_list.count(); i++)
		{
			VLink_Exp_Match* exp_match = m_match_list[i];
			bFind |= m_match_list[i]->check_match_string(src_str);
		}
	}

	return bFind;
}

void VLink_Exp_Match::parse_ref_match_string(const QString& refstr)
{
	m_sRefMatchString = refstr;
}

bool VLink_Exp_Match::check_match_ref_string(const QString& ref_str)
{
	bool bFind = false;

	if(!m_sRefMatchString.isEmpty())
	{
		QString ref_exp = m_sRefMatchString;
		ref_exp = ref_exp.replace(".","\\.");
		ref_exp = ref_exp.replace("*","[\\w|\\d]*");
		ref_exp.prepend("[\\w|\\d]*");

		QRegExp reg_exp(ref_exp,Qt::CaseInsensitive);
		if(ref_str.indexOf(reg_exp) >= 0)
		{
			bFind = true;
		}
	}

	return bFind;
}







VLink_Terminal::VLink_Terminal()
{
	stddesc = "";
	desckey = "";
	stdref = "";
	refkey = "";
}

VLink_Terminal::~VLink_Terminal()
{

}

VLink_Terminal& VLink_Terminal::operator=(const VLink_Terminal& other)
{
	if(this == &other)
		return *this;

	stddesc = other.stddesc;
	desckey = other.desckey;
	stdref = other.stdref;
	refkey = other.stddesc;

	return *this;
}







VLink_VLink::VLink_VLink()
{
	desc = "";
	mark = "";

	vlink_tx = 0;
	vlink_rx = 0;
}

VLink_VLink::~VLink_VLink()
{

}

VLink_VLink& VLink_VLink::operator=(const VLink_VLink& other)
{
	if(this == &other)
		return *this;

	desc = other.desc;
	mark = other.mark;

	vlink_tx = other.vlink_tx;
	vlink_rx = other.vlink_rx;

	return *this;
}








VLink_TxIed::VLink_TxIed()
{
	name = "";
	desc = "";
	desckey = "";
	type = "";
	manufacture = "";

	vlink_vlink_list.clear();
}

VLink_TxIed::~VLink_TxIed()
{

}

VLink_TxIed& VLink_TxIed::operator =(const VLink_TxIed& other)
{
	if(this == &other)
		return *this;

	name = other.name;
	desc = other.desc;
	desckey = other.desckey;
	type = other.type;
	manufacture = other.manufacture;

	vlink_vlink_list = other.vlink_vlink_list;

	return *this;
}







VLink_IED::VLink_IED()
{
	name = "";
	desc = "";
	desckey = "";
	type = "";
	manufacture = "";
	configversion = "";

	vlink_gorx_list.clear();
	vlink_svrx_list.clear();
}

VLink_IED::~VLink_IED()
{

}

VLink_IED& VLink_IED::operator =(const VLink_IED& other)
{
	if(this == &other)
		return *this;

	name = other.name;
	desc = other.desc;
	desckey = other.desckey;
	type = other.type;
	manufacture = other.manufacture;
	configversion = other.configversion;

	vlink_gorx_list = other.vlink_gorx_list;
	vlink_svrx_list = other.vlink_svrx_list;

	return *this;
}







VLink_MatchVLink::VLink_MatchVLink()
{
	rx_ied_name = "";
	rx_ied_desc = "";

	vlink_tx_ied = 0;
}

VLink_MatchVLink::~VLink_MatchVLink()
{

}

VLink_MatchVLink& VLink_MatchVLink::operator =(const VLink_MatchVLink& other)
{
	if(this == &other)
		return *this;

	rx_ied_name = other.rx_ied_name;
	rx_ied_desc = other.rx_ied_desc;

	vlink_tx_ied = other.vlink_tx_ied;

	return *this;
}







VLink_Template::VLink_Template()
{
	vlink_ied_list.clear();
	m_pProgressDlg = 0;
	m_pCheckDlg = 0;
}

VLink_Template::~VLink_Template()
{

}

VLink_Template& VLink_Template::operator =(const VLink_Template& other)
{
	if(this == &other)
		return *this;

	vlink_ied_list = other.vlink_ied_list;

	return *this;
}

void VLink_Template::clear_vlink_template()
{
	qDeleteAll(vlink_ied_list);
	vlink_ied_list.clear();
}

bool VLink_Template::read_vlink_template(const QString& sfile)
{
	qDeleteAll(vlink_ied_list);
	vlink_ied_list.clear();

	QFile file(sfile);
	if( !file.open(QIODevice::ReadOnly|QIODevice::Text) )
	{
		return false;
	}

	QDomDocument doc("mydocument");
	if(!doc.setContent(&file))
	{
		file.close();
		return false;
	}
	file.close();

	QDomElement root = doc.documentElement();
	if( root.isNull() )
	{
		return false;
	}

	for( QDomNode n=root.firstChild(); !n.isNull(); n=n.nextSibling() )
	{
		QDomElement elRoot = n.toElement();
		if( elRoot.isNull() )
			continue;
		if( elRoot.tagName() != "IED" )
			continue;
		VLink_IED *v_ied = new VLink_IED;
		v_ied->name = elRoot.attribute("name");
		v_ied->desc = elRoot.attribute("desc");
		v_ied->desckey = elRoot.attribute("descKey");
		v_ied->type = elRoot.attribute("type");
		v_ied->manufacture = elRoot.attribute("manufacturer");
		v_ied->configversion = elRoot.attribute("configVersion");
		v_ied->extrule = elRoot.attribute("extRule").trimmed();
		v_ied->vlink_ied_desckey_match = new VLink_Exp_Match;
        v_ied->vlink_ied_desckey_match->parse_exp_match_string(v_ied->desckey);

		for( QDomNode gocbnode=elRoot.firstChild(); !gocbnode.isNull(); gocbnode=gocbnode.nextSibling() )
		{
			QDomElement elgocb = gocbnode.toElement();
			if(elgocb.isNull())
				continue;
			if(elgocb.tagName() == "GoRx")
			{
				for( QDomNode txiednode=elgocb.firstChild(); !txiednode.isNull(); txiednode=txiednode.nextSibling() )
				{
					QDomElement eltxied = txiednode.toElement();
					if(eltxied.isNull())
						continue;
					if(eltxied.tagName() != "TxIED")
						continue;

					VLink_TxIed *v_txied = new VLink_TxIed;
					v_txied->name = eltxied.attribute("name");
					v_txied->desc = eltxied.attribute("desc");
					v_txied->desckey = eltxied.attribute("descKey");
					v_txied->type = eltxied.attribute("type");
					v_txied->manufacture = eltxied.attribute("manufacturer");
					v_txied->extrule = eltxied.attribute("extRule").trimmed();
					v_txied->vlink_txied_match = new VLink_Exp_Match;
                    v_txied->vlink_txied_match->parse_exp_match_string(v_txied->desckey);

					for( QDomNode vlinknode=eltxied.firstChild(); !vlinknode.isNull(); vlinknode=vlinknode.nextSibling() )
					{
						QDomElement elvlink = vlinknode.toElement();
						if(elvlink.isNull())
							continue;
						if(elvlink.tagName() != "VLink")
							continue;

						VLink_VLink *v_vlink = new VLink_VLink;
						v_vlink->desc = elvlink.attribute("desc");
						v_vlink->mark = elvlink.attribute("mark");

						for( QDomNode vterminalnode=elvlink.firstChild(); !vterminalnode.isNull(); vterminalnode=vterminalnode.nextSibling() )
						{
							QDomElement elvterminal = vterminalnode.toElement();
							if(elvterminal.isNull())
								continue;

							if(elvterminal.tagName() == "Tx")
							{
								VLink_Terminal *v_terminal = new VLink_Terminal;
								v_terminal->stddesc = elvterminal.attribute("stddesc");
								v_terminal->desckey = elvterminal.attribute("descKey");
								v_terminal->stdref = elvterminal.attribute("stdref");
								v_terminal->refkey = elvterminal.attribute("refKey");
								v_terminal->extrule = elvterminal.attribute("extRule").trimmed();
								v_terminal->vlink_terminal_match = new VLink_Exp_Match;
                                v_terminal->vlink_terminal_match->parse_exp_match_string(v_terminal->desckey);
								v_vlink->vlink_tx = v_terminal;
							}
							else if(elvterminal.tagName() == "Rx")
							{
								VLink_Terminal *v_terminal = new VLink_Terminal;
								v_terminal->stddesc = elvterminal.attribute("stddesc");
								v_terminal->desckey = elvterminal.attribute("descKey");
								v_terminal->stdref = elvterminal.attribute("stdref");
								v_terminal->refkey = elvterminal.attribute("refKey");
								v_terminal->extrule = elvterminal.attribute("extRule").trimmed();
								v_terminal->vlink_terminal_match = new VLink_Exp_Match;
                                v_terminal->vlink_terminal_match->parse_exp_match_string(v_terminal->desckey);
								v_vlink->vlink_rx = v_terminal;
							}
						}

						v_txied->vlink_vlink_list.append(v_vlink);
					}

					v_ied->vlink_gorx_list.append(v_txied);
				}
			}
			else if(elgocb.tagName() == "SvRx")
			{
				for( QDomNode txiednode=elgocb.firstChild(); !txiednode.isNull(); txiednode=txiednode.nextSibling() )
				{
					QDomElement eltxied = txiednode.toElement();
					if(eltxied.isNull())
						continue;
					if(eltxied.tagName() != "TxIED")
						continue;

					VLink_TxIed *v_txied = new VLink_TxIed;
					v_txied->name = eltxied.attribute("name");
					v_txied->desc = eltxied.attribute("desc");
					v_txied->desckey = eltxied.attribute("descKey");
					v_txied->type = eltxied.attribute("type");
					v_txied->manufacture = eltxied.attribute("manufacturer");
					v_txied->extrule = eltxied.attribute("extRule").trimmed();
					v_txied->vlink_txied_match = new VLink_Exp_Match;
                    v_txied->vlink_txied_match->parse_exp_match_string(v_txied->desckey);

					for( QDomNode vlinknode=eltxied.firstChild(); !vlinknode.isNull(); vlinknode=vlinknode.nextSibling() )
					{
						QDomElement elvlink = vlinknode.toElement();
						if(elvlink.isNull())
							continue;
						if(elvlink.tagName() != "VLink")
							continue;

						VLink_VLink *v_vlink = new VLink_VLink;
						v_vlink->desc = elvlink.attribute("desc");
						v_vlink->mark = elvlink.attribute("mark");

						for( QDomNode vterminalnode=elvlink.firstChild(); !vterminalnode.isNull(); vterminalnode=vterminalnode.nextSibling() )
						{
							QDomElement elvterminal = vterminalnode.toElement();
							if(elvterminal.isNull())
								continue;

							if(elvterminal.tagName() == "Tx")
							{
								VLink_Terminal *v_terminal = new VLink_Terminal;
								v_terminal->stddesc = elvterminal.attribute("stddesc");
								v_terminal->desckey = elvterminal.attribute("descKey");
								v_terminal->stdref = elvterminal.attribute("stdref");
								v_terminal->refkey = elvterminal.attribute("refKey");
								v_terminal->extrule = elvterminal.attribute("extRule").trimmed();
								v_terminal->vlink_terminal_match = new VLink_Exp_Match;
                                v_terminal->vlink_terminal_match->parse_exp_match_string(v_terminal->desckey);
								v_vlink->vlink_tx = v_terminal;
							}
							else if(elvterminal.tagName() == "Rx")
							{
								VLink_Terminal *v_terminal = new VLink_Terminal;
								v_terminal->stddesc = elvterminal.attribute("stddesc");
								v_terminal->desckey = elvterminal.attribute("descKey");
								v_terminal->stdref = elvterminal.attribute("stdref");
								v_terminal->refkey = elvterminal.attribute("refKey");
								v_terminal->extrule = elvterminal.attribute("extRule").trimmed();
								v_terminal->vlink_terminal_match = new VLink_Exp_Match;
                                v_terminal->vlink_terminal_match->parse_exp_match_string(v_terminal->desckey);
								v_vlink->vlink_rx = v_terminal;
							}
						}

						v_txied->vlink_vlink_list.append(v_vlink);
					}

					v_ied->vlink_svrx_list.append(v_txied);
				}
			}
		}
		vlink_ied_list.append(v_ied);
	}

	return true;
}

bool VLink_Template::read_vlink_template_list(const QStringList& sfilelist)
{
	QList<VLink_IED*> vied_list;
	for (int i = 0; i < sfilelist.count(); i++)
	{
		QString sfile = sfilelist[i];
		read_vlink_template(sfile);
		vied_list.append(vlink_ied_list);
		vlink_ied_list.clear();
	}

	vlink_ied_list.clear();
	vlink_ied_list.append(vied_list);

	return true;
}

bool VLink_Template::write_vlink_template(const QString& sfile)
{
	QFile file(sfile);
	if( !file.open(QIODevice::WriteOnly|QIODevice::Text) )
	{
		return false;
	}

	QTextStream dsm(&file);
	QDomDocument doc;
	QDomProcessingInstruction dp = doc.createProcessingInstruction("xml",
		" version=\"1.0\" encoding=\"GB18030\" ");
	doc.appendChild( dp );

	QDomElement root = doc.createElement( "VLTPL" );
	doc.appendChild(root);

	//IED
	for (int i = 0; i < vlink_ied_list.count(); i++)
	{
		VLink_IED* v_ied = vlink_ied_list[i];
		QDomElement node_ied = doc.createElement( "IED" );
		if( node_ied.isNull() )
		{
			continue;
		}
		node_ied.setAttribute("name",v_ied->name);
		node_ied.setAttribute("desc",v_ied->desc);
		node_ied.setAttribute("descKey",v_ied->desckey);
		node_ied.setAttribute("type",v_ied->type);
		node_ied.setAttribute("manufacturer",v_ied->manufacture);
		node_ied.setAttribute("configVersion",v_ied->configversion);
		node_ied.setAttribute("extRule",v_ied->extrule);

		QDomElement node_gorx = doc.createElement( "GoRx" );
		if( node_gorx.isNull() )
		{
			continue;
		}

		QDomElement node_svrx = doc.createElement( "SvRx" );
		if( node_svrx.isNull() )
		{
			continue;
		}

		for (int j = 0; j < v_ied->vlink_gorx_list.count(); j++)
		{
			VLink_TxIed* v_txied = v_ied->vlink_gorx_list[j];

			QDomElement node_txied = doc.createElement( "TxIED" );
			if( node_txied.isNull() )
			{
				continue;
			}

			node_txied.setAttribute("name",v_txied->name);
			node_txied.setAttribute("desc",v_txied->desc);
			node_txied.setAttribute("descKey",v_txied->desckey);
			node_txied.setAttribute("type",v_txied->type);
			node_txied.setAttribute("manufacturer",v_txied->manufacture);
			node_txied.setAttribute("extRule",v_txied->extrule);

			for (int k = 0; k < v_txied->vlink_vlink_list.count(); k++)
			{
				VLink_VLink* v_vlink = v_txied->vlink_vlink_list[k];

				QDomElement node_vlink = doc.createElement( "VLink" );
				if( node_vlink.isNull() )
				{
					continue;
				}

				node_vlink.setAttribute("desc",v_vlink->desc);
				node_vlink.setAttribute("mark",v_vlink->mark);

				QDomElement node_vlink_rx = doc.createElement( "Rx" );
				if( node_vlink_rx.isNull() )
				{
					continue;
				}

				QDomElement node_vlink_tx = doc.createElement( "Tx" );
				if( node_vlink_tx.isNull() )
				{
					continue;
				}

				if(v_vlink->vlink_rx)
				{
					node_vlink_rx.setAttribute("stddesc",v_vlink->vlink_rx->stddesc);
					node_vlink_rx.setAttribute("descKey",v_vlink->vlink_rx->desckey);
					node_vlink_rx.setAttribute("stdref",v_vlink->vlink_rx->stdref);
					node_vlink_rx.setAttribute("refKey",v_vlink->vlink_rx->refkey);
					node_vlink_rx.setAttribute("extRule",v_vlink->vlink_rx->extrule);
				}
				else
				{
					node_vlink_rx.setAttribute("stddesc","");
					node_vlink_rx.setAttribute("descKey","");
					node_vlink_rx.setAttribute("stdref","");
					node_vlink_rx.setAttribute("refKey","");
					node_vlink_rx.setAttribute("extRule","");
				}
				
				if(v_vlink->vlink_tx)
				{
					node_vlink_tx.setAttribute("stddesc",v_vlink->vlink_tx->stddesc);
					node_vlink_tx.setAttribute("descKey",v_vlink->vlink_tx->desckey);
					node_vlink_tx.setAttribute("stdref",v_vlink->vlink_tx->stdref);
					node_vlink_tx.setAttribute("refKey",v_vlink->vlink_tx->refkey);
					node_vlink_tx.setAttribute("extRule",v_vlink->vlink_tx->extrule);
				}
				else
				{
					node_vlink_tx.setAttribute("stddesc","");
					node_vlink_tx.setAttribute("descKey","");
					node_vlink_tx.setAttribute("stdref","");
					node_vlink_tx.setAttribute("refKey","");
					node_vlink_tx.setAttribute("extRule","");
				}

				node_vlink.appendChild(node_vlink_rx);
				node_vlink.appendChild(node_vlink_tx);

				node_txied.appendChild(node_vlink);
			}
			node_gorx.appendChild(node_txied);
		}

		for (int j = 0; j < v_ied->vlink_svrx_list.count(); j++)
		{
			VLink_TxIed* v_txied = v_ied->vlink_svrx_list[j];

			QDomElement node_txied = doc.createElement( "TxIED" );
			if( node_txied.isNull() )
			{
				continue;
			}

			node_txied.setAttribute("name",v_txied->name);
			node_txied.setAttribute("desc",v_txied->desc);
			node_txied.setAttribute("descKey",v_txied->desckey);
			node_txied.setAttribute("type",v_txied->type);
			node_txied.setAttribute("manufacturer",v_txied->manufacture);
			node_txied.setAttribute("extRule",v_txied->extrule);

			for (int k = 0; k < v_txied->vlink_vlink_list.count(); k++)
			{
				VLink_VLink* v_vlink = v_txied->vlink_vlink_list[k];

				QDomElement node_vlink = doc.createElement( "VLink" );
				if( node_vlink.isNull() )
				{
					continue;
				}

				node_vlink.setAttribute("desc",v_vlink->desc);
				node_vlink.setAttribute("mark",v_vlink->mark);

				QDomElement node_vlink_rx = doc.createElement( "Rx" );
				if( node_vlink_rx.isNull() )
				{
					continue;
				}

				QDomElement node_vlink_tx = doc.createElement( "Tx" );
				if( node_vlink_tx.isNull() )
				{
					continue;
				}

				if(v_vlink->vlink_rx)
				{
					node_vlink_rx.setAttribute("stddesc",v_vlink->vlink_rx->stddesc);
					node_vlink_rx.setAttribute("descKey",v_vlink->vlink_rx->desckey);
					node_vlink_rx.setAttribute("stdref",v_vlink->vlink_rx->stdref);
					node_vlink_rx.setAttribute("refKey",v_vlink->vlink_rx->refkey);
					node_vlink_rx.setAttribute("extRule",v_vlink->vlink_rx->extrule);
				}
				else
				{
					node_vlink_rx.setAttribute("stddesc","");
					node_vlink_rx.setAttribute("descKey","");
					node_vlink_rx.setAttribute("stdref","");
					node_vlink_rx.setAttribute("refKey","");
					node_vlink_rx.setAttribute("extRule","");
				}

				if(v_vlink->vlink_tx)
				{
					node_vlink_tx.setAttribute("stddesc",v_vlink->vlink_tx->stddesc);
					node_vlink_tx.setAttribute("descKey",v_vlink->vlink_tx->desckey);
					node_vlink_tx.setAttribute("stdref",v_vlink->vlink_tx->stdref);
					node_vlink_tx.setAttribute("refKey",v_vlink->vlink_tx->refkey);
					node_vlink_tx.setAttribute("extRule",v_vlink->vlink_tx->extrule);
				}
				else
				{
					node_vlink_tx.setAttribute("stddesc","");
					node_vlink_tx.setAttribute("descKey","");
					node_vlink_tx.setAttribute("stdref","");
					node_vlink_tx.setAttribute("refKey","'");
					node_vlink_tx.setAttribute("extRule","");
				}

				node_vlink.appendChild(node_vlink_rx);
				node_vlink.appendChild(node_vlink_tx);

				node_txied.appendChild(node_vlink);
			}
			node_svrx.appendChild(node_txied);
		}

		node_ied.appendChild(node_gorx);
		node_ied.appendChild(node_svrx);

		root.appendChild(node_ied);
	}
	

	doc.save(dsm,4);
	file.close();

	return true;
}

bool VLink_Template::read_vlink_template_from_scd_file(const QString& sfile)
{
	return true;
}

VLink_MatchVLink* VLink_Template::get_vlink_match_vlink(const QString& rx_ied_desc, const QString& rx_ied_name)
{
	QStringList rx_ied_name_list;
	QStringList rx_ied_desc_list;
	rx_ied_name_list.append(rx_ied_name);
	rx_ied_desc_list.append(rx_ied_desc);

	return get_vlink_match_vlink(rx_ied_desc_list,rx_ied_name_list);
}

VLink_MatchVLink* VLink_Template::get_vlink_match_vlink(const QStringList& rx_ied_desc_list, const QStringList& rx_ied_name_list)
{
	VLink_MatchVLink* pmatch_vlink = 0;

	QList<VLink_IED*> find_vlink_ied_list;
	for(int k = 0; k < rx_ied_desc_list.count(); k++)
	{
		QString rx_ied_name = "";
		QString rx_ied_desc = rx_ied_desc_list[k];
		if(rx_ied_name_list.count() > k)
			rx_ied_name = rx_ied_name_list[k];
		bool bFind = false;
		for (int i = 0; i < vlink_ied_list.count(); i++)
		{
			VLink_IED* v_ied = vlink_ied_list[i];
			if(!rx_ied_desc.trimmed().isEmpty())
			{
				v_ied->vlink_ied_desckey_match->parse_exp_match_string(v_ied->desckey);
				if(v_ied->vlink_ied_desckey_match->check_match_string(rx_ied_desc))
				{
					bFind = true;
					find_vlink_ied_list.append(v_ied);
					break;
				}
				else if(rx_ied_desc == v_ied->desc)
				{
					bFind = true;
					find_vlink_ied_list.append(v_ied);
					break;
				}
			}
			if(!bFind)
			{
// 				QString sied = v_ied->name;
// 				sied = sied.remove("(").remove(")");
// 				sied = sied.replace("or","|",Qt::CaseInsensitive);
// 				sied = sied.simplified();
// 				if(rx_ied_name.contains(QRegExp(sied,Qt::CaseInsensitive)))
// 				{
// 					find_vlink_ied_list.append(v_ied);
// 					break;
// 				}

				VLink_Exp_Match vlink_name_match;
				vlink_name_match.parse_exp_match_string(v_ied->name);
				if(vlink_name_match.check_match_string(rx_ied_name))
				{
					find_vlink_ied_list.append(v_ied);
					break;
				}
			}
		}
	}

	if(find_vlink_ied_list.count() > 0)
	{
		Vlink_Select_Dlg vlink_dlg(0);
		vlink_dlg.set_vlink_rx_ied_list(find_vlink_ied_list);
		if(vlink_dlg.exec() == QDialog::Accepted)
		{
			QString rx_ied_name = vlink_dlg.get_sel_rx_ied_name();
			QString rx_ied_type = vlink_dlg.get_sel_rx_ied_type();
			QString tx_ied_name = vlink_dlg.get_sel_tx_ied_name();

			VLink_IED* p_ied = 0;
			for (int i = 0; i < vlink_ied_list.count(); i++)
			{
				VLink_IED* v_ied = vlink_ied_list[i];
				if(v_ied->name == rx_ied_name)
				{
					p_ied = v_ied;
					break;
				}
			}
			if(p_ied)
			{
				VLink_TxIed* p_txied = 0;
				if(rx_ied_type == "GOOSE")
				{
					for (int i = 0; i < p_ied->vlink_gorx_list.count(); i++)
					{
						VLink_TxIed *v_txied = p_ied->vlink_gorx_list[i];
						if(v_txied->name == tx_ied_name)
						{
							p_txied = v_txied;
							break;
						}
					}
				}
				else if(rx_ied_type == "SV")
				{
					for (int i = 0; i < p_ied->vlink_svrx_list.count(); i++)
					{
						VLink_TxIed *v_txied = p_ied->vlink_svrx_list[i];
						if(v_txied->name == tx_ied_name)
						{
							p_txied = v_txied;
							break;
						}
					}
				}
				if(p_txied)
				{
					pmatch_vlink = new VLink_MatchVLink;
					pmatch_vlink->rx_ied_name = rx_ied_name;
					pmatch_vlink->rx_ied_desc = p_ied->desc;
					pmatch_vlink->vlink_tx_ied = p_txied;
				}
			}
		}
	}

	return pmatch_vlink;
}

bool VLink_Template::read_scd_file(const QString& scd_file)
{
	m_sScdFile = scd_file;
	scd_to_pro();
	return true;
}

bool VLink_Template::check_scd_with_vlink_template()
{
	if(m_pCheckDlg)
	{
		delete m_pCheckDlg;
		m_pCheckDlg = 0;
	}
	if(!m_pCheckDlg)
	{
		m_pCheckDlg = new CVLink_Check_Dlg();
		m_pCheckDlg->setWindowTitle(tr("校验SCD文件"));
	}

	m_pCheckDlg->show();
	m_pCheckDlg->setLabelText(tr("解析IED设备"));
	m_pCheckDlg->setTextBrowser(tr("解析IED设备..."));
	m_pCheckDlg->setValue(0);

	QString crc_file = QString("%1/__CRC__.pro").arg(m_sScdDir);
	if(!QFileInfo(crc_file).exists())
	{
		return false;
	}

	m_ied_name_desc_map.clear();
	if(!read_crc_file(crc_file))
	{
		QApplication::processEvents();
		m_pCheckDlg->setLabelText(tr("校验失败"));
		m_pCheckDlg->setTextBrowser(tr("校验失败!"),QColor(255,0,0));
		m_pCheckDlg->setValue(100);
		QApplication::processEvents();
		m_pCheckDlg->hide();
		delete m_pCheckDlg;
		m_pCheckDlg = 0;
		return false;
	}

	QApplication::processEvents();
	m_pCheckDlg->setLabelText(tr("校验"));
	m_pCheckDlg->setTextBrowser(tr("校验..."));
	m_pCheckDlg->setValue(10);
	QApplication::processEvents();

	QStringList ied_list = m_ied_name_desc_map.keys();
	int ied_num = ied_list.count();
	for (int i = 0; i < ied_list.count(); i++)
	{
		QString ied_name = ied_list[i];
		QString ied_file = QString("%1/%2.pro").arg(m_sScdDir).arg(ied_name);

		QApplication::processEvents();
		m_pCheckDlg->setLabelText(tr("校验 %1(%2)").arg(m_ied_name_desc_map[ied_name]).arg(ied_name));
		m_pCheckDlg->setTextBrowser(tr("校验 %1(%2)...").arg(m_ied_name_desc_map[ied_name]).arg(ied_name));
		m_pCheckDlg->setValue(10+i*90/ied_num);
		QApplication::processEvents();

		VLink_IED* v_ied = new VLink_IED;
		bool bret = read_ied_file(ied_file,v_ied);
		QList<QMap<QString,QColor> > check_info;
		bret = check_ied_file(v_ied,check_info);
		delete v_ied;

		QApplication::processEvents();
		if(bret)
		{
			for (int j = 0; j < check_info.count(); j++)
			{
				QStringList infokeys = check_info[j].keys();
				m_pCheckDlg->setTextBrowser(infokeys[0],check_info[j].value(infokeys[0]));
			}
			m_pCheckDlg->setTextBrowser(tr("    校验 %1(%2) 完成").arg(m_ied_name_desc_map[ied_name]).arg(ied_name),QColor(0,0,255));
		}
		else
		{
			for (int j = 0; j < check_info.count(); j++)
			{
				QStringList infokeys = check_info[j].keys();
				m_pCheckDlg->setTextBrowser(infokeys[0],check_info[j].value(infokeys[0]));
			}
			m_pCheckDlg->setTextBrowser(tr("    校验 %1(%2) 失败").arg(m_ied_name_desc_map[ied_name]).arg(ied_name),QColor(255,0,0));
		}
		m_pCheckDlg->setValue(10+(i+1)*90/ied_num);
		QApplication::processEvents();
	}

	QApplication::processEvents();
	m_pCheckDlg->setLabelText(tr("校验完成"));
	m_pCheckDlg->setTextBrowser(tr("校验完成!"));
	m_pCheckDlg->setValue(100);
	QApplication::processEvents();
// 	m_pCheckDlg->hide();
// 	delete m_pCheckDlg;
// 	m_pCheckDlg = 0;

	return true;
}

void VLink_Template::scd_to_pro()
{
	if(!m_pProgressDlg)
	{
		m_pProgressDlg = new CVLink_Process_Dlg();
		m_pProgressDlg->setWindowTitle(tr("打开SCD文件"));
		m_pProgressDlg->setHidden(true);
	}
	if(m_pProgressDlg->isHidden())
	{
		m_pProgressDlg->show();
	}
	else
	{
		m_pProgressDlg->setHidden(true);
	}
	m_pProgressDlg->setModal(true);

	m_pProgressDlg->setLabelText(tr("删除原始文件"));
	m_pProgressDlg->setValue(0);

	QString runPath = QCoreApplication::applicationDirPath();
	runPath = runPath.replace("\\","/");
	m_sScdDir = runPath+"/temp";

	del_testpro_files(m_sScdDir);

	m_pProgressDlg->setLabelText(tr("解析SCD文件"));
	m_pProgressDlg->setValue(10);

	QString str = runPath+"/scd2pro/scd2pro.exe";
	str = QString("\"%1\"").arg(str);

	QString export_dir = m_sScdDir;
	//export_dir = "temp";
	QString sparam = QString("%1|%2").arg(m_sScdFile).arg(export_dir);  

	QProcess* p = new QProcess();
	connect(p, SIGNAL(finished(int)), this, SLOT(slot_parse_scd_result(int)));  
	p->start(str);
	p->write(sparam.toUtf8().data());
	p->write("\n");

	QApplication::setOverrideCursor(Qt::WaitCursor);
}

void VLink_Template::del_testpro_files(const QString& file_dir)
{
	QDir dir;
	QFileInfoList fileInfoList;

	if(!dir.cd(file_dir))
	{
		dir.mkdir(file_dir);
	}

	if(dir.cd(file_dir))
	{
		fileInfoList = dir.entryInfoList();
		foreach (QFileInfo fileInfo,fileInfoList)
		{
			if(!dir.remove(fileInfo.absoluteFilePath()))
			{
				dir.remove(fileInfo.absoluteFilePath());
			}
		}
	}
}

void VLink_Template::slot_parse_scd_result(int exitCode)
{
	if(exitCode == 0)
	{   
		//恢复光标
		QApplication::restoreOverrideCursor();

		if(m_pProgressDlg)
		{
			m_pProgressDlg->setValue(100);
			m_pProgressDlg->hide();
			delete m_pProgressDlg;
			m_pProgressDlg = 0;
		}

	} 
	else
	{
		if(m_pProgressDlg)
		{
			m_pProgressDlg->setValue(100);
			m_pProgressDlg->hide();
			delete m_pProgressDlg;
			m_pProgressDlg = 0;
		}

		QMessageBox::information(0, tr("提示"), tr("文件转换失败:请确认SCD文件是否在英文目录下，同时SCD文件名不能带有中文"));
	}
}

bool VLink_Template::read_crc_file(const QString& sfile)
{
	QFile file(sfile);
	if( !file.open(QIODevice::ReadOnly|QIODevice::Text) )
	{
		return false;
	}

	QString errstr;
	int nrow = 0;
	int ncol = 0;
//	QString content_text = file.readAll();
// 	QString content_head = content_text.section("\n",0,0);
// 	content_text = content_text.section("\n",1);
// 	content_text.prepend("<IEDS>\n");
// 	content_text.append("</IEDS>\n");
// 	content_text.prepend(content_head+"\n");

	QDomDocument doc("mydocument");
	if(!doc.setContent(&file) )
	{
        printf("[scd_pro_infor_config::read_crc_file_new] err: %s, %d,%d\n",errstr.toLatin1().data(),nrow,ncol);
		file.close();
		return false;
	}

	file.close();

	QDomElement root = doc.documentElement();
	if( root.isNull() )
	{
		return false;
	}

	for( QDomNode n=root.firstChild(); !n.isNull(); n=n.nextSibling() )
	{
		QDomElement elRoot = n.toElement();
		if( elRoot.isNull() )
			continue;
		if( elRoot.tagName() == "IED" )
		{
			QString ied_name = elRoot.attribute("name");
			QString ied_desc = elRoot.attribute("desc");
			if(ied_name != "__SCD__")
				m_ied_name_desc_map[ied_name] = ied_desc;
		}
	}

	return true;
}

bool VLink_Template::read_ied_file(const QString& sfile, VLink_IED* v_ied)
{
	QFile file(sfile);
	if( !file.open(QIODevice::ReadOnly|QIODevice::Text) )
	{
		return false;
	}

	QDomDocument doc("mydocument");
	if(!doc.setContent(&file))
	{
		file.close();
		return false;
	}
	file.close();

	QDomElement root = doc.documentElement();
	if( root.isNull() )
	{
		return false;
	}

	QString ied_desc = root.attribute("desc");
	QString ied_name = root.attribute("name");
	v_ied->name = ied_name;
	v_ied->desc = ied_desc;

	for( QDomNode n=root.firstChild(); !n.isNull(); n=n.nextSibling() )
	{
		QDomElement elRoot = n.toElement();
		if( elRoot.isNull() )
			continue;
		if( elRoot.tagName() == "GOOSESUB" )
		{
			int gocb_input_valid_idx = 0;
			int gocb_num = 0;
			for( QDomNode gocbnode=elRoot.firstChild(); !gocbnode.isNull(); gocbnode=gocbnode.nextSibling() )
			{
				QDomElement elgocb = gocbnode.toElement();
				if(!elgocb.isNull() && elgocb.tagName() == "GOCBref")
				{
					gocb_num++;
	
					QString send_ied_name = "";
					//ConnectedAP
					QDomElement elConAp = elgocb.firstChildElement("ConnectedAP");
					if(!elConAp.isNull())
					{
						send_ied_name = elConAp.attribute("iedName");
					}
					VLink_TxIed* v_txied = 0;
					for (int i = 0; i < v_ied->vlink_gorx_list.count(); i++)
					{
						VLink_TxIed* ptxied = v_ied->vlink_gorx_list[i];
						if(ptxied->name == send_ied_name)
						{
							v_txied = ptxied;
							break;
						}
					}
					if(!v_txied)
					{
						v_txied = new VLink_TxIed;
						v_txied->name = send_ied_name;
						v_txied->desc = m_ied_name_desc_map.value(send_ied_name,"");
					}

					//Dataset
					for( QDomNode dsnode=elgocb.firstChild(); !dsnode.isNull(); dsnode=dsnode.nextSibling() )
					{
						QDomElement eldataset = dsnode.toElement();
						if(!eldataset.isNull() && eldataset.tagName() == "DataSet")
						{
							int fcda_idx = 0;
							for( QDomNode fcdanode=eldataset.firstChild(); !fcdanode.isNull(); fcdanode=fcdanode.nextSibling() )
							{
								QDomElement elfcda = fcdanode.toElement();
								if(!elfcda.isNull() && elfcda.tagName() == "FCDA")
								{
									fcda_idx++;

									QString stype = "";
									QString sdaname = "";
									QString sdesc = "";
									QString sdoname = "";
									QString sfc = "";
									QString sldinst = "";
									QString slninst = "";
									QString slnclass = "";
									QString sprefix = "";
									QString sintaddr = "";
									QString sintaddr_name = "";
									QString sintaddr_desc = "";
									QString soutvarname = "";
									QString ssendref = "";
									QString srevref = "";

									stype = elfcda.attribute("bType");
									if(stype.toLower() == "boolean")
										stype = "Bool";
									else if(stype.toLower() == "dbpos")
										stype = "Bstring2";
									sdaname = elfcda.attribute("daName");
									sdesc = elfcda.attribute("desc");
									sdoname = elfcda.attribute("doName");
									sfc = elfcda.attribute("fc");
									sldinst = elfcda.attribute("ldInst");
									slnclass = elfcda.attribute("lnClass");
									slninst = elfcda.attribute("lnInst");
									sprefix = elfcda.attribute("prefix");

									QDomElement elIntaddr = elfcda.firstChildElement("intAddr");
									if(!elIntaddr.isNull())
									{
										sintaddr_name = elIntaddr.attribute("name");
										sintaddr_desc = elIntaddr.attribute("desc");
										if(sintaddr_name != "NULL")
										{
											QDomElement elDai = elIntaddr.firstChildElement("DAI");
											if(!elDai.isNull())
											{
												soutvarname = elDai.attribute("sAddr");
											}
										}
									}

									if(0 != sintaddr_name.compare("NULL",Qt::CaseInsensitive))
									{
										if(sintaddr_name.indexOf(":") >= 0)
											sintaddr_name = sintaddr_name.section(":",1);
										//sintaddr_name = sintaddr_name.replace(".","$");

										if(soutvarname.indexOf(":") >= 0)
										{
											stype = soutvarname.section(":",0,0);
											soutvarname = soutvarname.section(":",1);
										}

										if(stype.toLower() == "boolean" || stype.toLower() == "bool")
											stype = "Bool";
										else if(stype.toLower() == "dbpos" || stype.toLower() == "bstring2")
											stype = "Bstring2";
										else if(stype.toLower() == "timestamp")
											stype = "Utctime";
										else
											stype = "Bool";

// 										ssendref = QString("%1%2/%3%4%5.%6.%7")
// 											.arg(send_ied_name)
// 											.arg(sldinst)
// 											.arg(sprefix)
// 											.arg(slnclass)
// 											.arg(slninst)
// 											.arg(sfc)
// 											.arg(sdoname);

										ssendref = QString("%1%2/%3%4.%5")
											.arg(send_ied_name)
											.arg(sldinst)
											.arg(slnclass)
											.arg(slninst)
											.arg(sdoname);

										if(!sdaname.trimmed().isEmpty())
											ssendref = QString("%1.%2").arg(ssendref).arg(sdaname);

										srevref = QString("%1%2").arg(ied_name).arg(sintaddr_name);

										gocb_input_valid_idx++;
										
										VLink_VLink* v_vlink = new VLink_VLink;
										v_vlink->desc = sintaddr_desc;
										VLink_Terminal* v_tx_terminal = new VLink_Terminal;
										VLink_Terminal* v_rx_terminal = new VLink_Terminal;
										v_vlink->vlink_tx = v_tx_terminal;
										v_vlink->vlink_rx = v_rx_terminal;
										v_tx_terminal->stddesc = sdesc;
										v_tx_terminal->stdref = ssendref;
										v_rx_terminal->stddesc = sintaddr_desc;
										v_rx_terminal->stdref = srevref;

										v_txied->vlink_vlink_list.append(v_vlink);
									}
								}
							}
						}
					}
					v_ied->vlink_gorx_list.append(v_txied);
				}
			}
		}
		else if( elRoot.tagName() == "SVSUB" )
		{
			int svcb_input_valid_idx = 0;
			int svcb_idx = 0;
			for( QDomNode svnode=elRoot.firstChild(); !svnode.isNull(); svnode=svnode.nextSibling() )
			{
				QDomElement elsvcb = svnode.toElement();

				if(!elsvcb.isNull() && elsvcb.tagName() == "SMVCBref")
				{
					svcb_idx++;

					QString send_ied_name = "";
					QString sref = "";
					QString saddr = "";
					QString sappid = "";
					QString sconfrev = "";
					QString snoasdu = "";
					QString svid = "";

					sref = elsvcb.attribute("name");

					//connected ap
					QDomElement elSvConAp = elsvcb.firstChildElement("ConnectedAP");
					if(!elSvConAp.isNull())
					{
						send_ied_name = elSvConAp.attribute("iedName");
					}

					VLink_TxIed* v_txied = 0;
					for (int i = 0; i < v_ied->vlink_svrx_list.count(); i++)
					{
						VLink_TxIed* ptxied = v_ied->vlink_svrx_list[i];
						if(ptxied->name == send_ied_name)
						{
							v_txied = ptxied;
							break;
						}
					}
					if(!v_txied)
					{
						v_txied = new VLink_TxIed;
						v_txied->name = send_ied_name;
						v_txied->desc = m_ied_name_desc_map.value(send_ied_name,"");
					}

					//Dataset
					for( QDomNode dsnode=elsvcb.firstChild(); !dsnode.isNull(); dsnode=dsnode.nextSibling() )
					{
						QDomElement eldataset = dsnode.toElement();
						if(!eldataset.isNull() && eldataset.tagName() == "DataSet")
						{
							int fcda_idx = 0;
							for( QDomNode fcdanode=eldataset.firstChild(); !fcdanode.isNull(); fcdanode=fcdanode.nextSibling() )
							{
								QDomElement elfcda = fcdanode.toElement();
								if(!elfcda.isNull() && elfcda.tagName() == "FCDA")
								{
									fcda_idx++;

									QString sdesc = "";
									QString sdoname = "";
									QString sdiname = "";
									QString sdaname = "";
									QString sfc = "";
									QString sldinst = "";
									QString slninst = "";
									QString slnclass = "";
									QString sprefix = "";
									QString sintaddr = "";
									QString sintaddr_name = "";
									QString sintaddr_desc = "";
									QString soutvartype = "";
									QString soutvarname = "";
									QString ssendref = "";
									QString srevref = "";

									sdesc = elfcda.attribute("desc");
									sdoname = elfcda.attribute("doName");
									sfc = elfcda.attribute("fc");
									sldinst = elfcda.attribute("ldInst");
									slnclass = elfcda.attribute("lnClass");
									slninst = elfcda.attribute("lnInst");
									sprefix = elfcda.attribute("prefix");

									QDomElement elindaddr = elfcda.firstChildElement("intAddr");
									if(!elindaddr.isNull())
									{
										sintaddr_name = elindaddr.attribute("name");
										sintaddr_desc = elindaddr.attribute("desc");
										if(sintaddr_name != "NULL")
										{
											QDomElement eldai = elindaddr.firstChildElement("DAI");
											if(!eldai.isNull())
											{
												soutvarname = eldai.attribute("sAddr");
											}
											else
											{
												QDomElement eldoi = elindaddr.firstChildElement("DOI");
												if(!eldoi.isNull())
												{
													QDomElement elsdi = eldoi.firstChildElement("SDI");
													if(!elsdi.isNull())
													{
														sdiname = elsdi.attribute("name");
														QDomElement eldai = elsdi.firstChildElement("DAI");
														if(!eldai.isNull())
														{
															sdaname = eldai.attribute("name");
															soutvarname = eldai.attribute("sAddr");
														}
													}
												}
											}
										}
										else
										{
											QDomElement eldoi = elindaddr.firstChildElement("DOI");
											if(!eldoi.isNull())
											{
												QDomElement elsdi = eldoi.firstChildElement("SDI");
												if(!elsdi.isNull())
												{
													QDomElement eldai = elsdi.firstChildElement("DAI");
													if(!eldai.isNull())
													{
														soutvarname = eldai.attribute("sAddr");
													}
												}
											}
										}
									}

// 									ssendref = QString("%1%2/%3%4%5$%6$%7")
// 										.arg(send_ied_name)
// 										.arg(sldinst)
// 										.arg(sprefix)
// 										.arg(slnclass)
// 										.arg(slninst)
// 										.arg(sfc)
// 										.arg(sdoname);

									ssendref = QString("%1%2/%3%4.%5")
										.arg(send_ied_name)
										.arg(sldinst)
										.arg(slnclass)
										.arg(slninst)
										.arg(sdoname);

									srevref = QString("%1%2").arg(ied_name).arg(sintaddr_name);

									if(0 != sintaddr_name.compare("NULL",Qt::CaseInsensitive))
									{
										if(sintaddr_desc.trimmed().isEmpty())
											sintaddr_desc = sdesc;
										if(soutvarname.indexOf(":") >= 0)
										{
											soutvartype = soutvarname.section(":",0,0);
											soutvarname = soutvarname.section(":",1);
										}

										svcb_input_valid_idx++;

										VLink_VLink* v_vlink = new VLink_VLink;
										v_vlink->desc = sintaddr_desc;
										VLink_Terminal* v_tx_terminal = new VLink_Terminal;
										VLink_Terminal* v_rx_terminal = new VLink_Terminal;
										v_vlink->vlink_tx = v_tx_terminal;
										v_vlink->vlink_rx = v_rx_terminal;
										v_tx_terminal->stddesc = sdesc;
										v_tx_terminal->stdref = ssendref;
										v_rx_terminal->stddesc = sintaddr_desc;
										v_rx_terminal->stdref = srevref;

										v_txied->vlink_vlink_list.append(v_vlink);
									}
								}
							}
						}
					}
					v_ied->vlink_svrx_list.append(v_txied);
				}
			}
		}
	}

	return true;
}

bool VLink_Template::check_ied_file(VLink_IED* check_ied, QList<QMap<QString,QColor> > & check_info)
{
	QMap<QString,QColor> infor_text;
	QString str_text = "";
	VLink_IED* v_ied = 0;
	for (int i = 0; i < vlink_ied_list.count(); i++)
	{
		VLink_IED* pied = vlink_ied_list[i];
		if(!pied->desc.trimmed().isEmpty() && pied->desc == check_ied->desc)
		{
			v_ied = pied;
			break;
		}
		else
		{
			pied->vlink_ied_desckey_match->parse_exp_match_string(pied->desckey);
			if(pied->vlink_ied_desckey_match->check_match_string(check_ied->desc))
			{
				if(!pied->extrule.trimmed().isEmpty())
				{
					VLink_Exp_Match ext_exp_match;
					ext_exp_match.parse_exp_match_string(pied->extrule);
					bool bFind = true;
					for (int k = 0; k < ext_exp_match.m_sSplitString.count(); k++)
					{
						QString ext_item = ext_exp_match.m_sSplitString[k];
						if(ext_item.compare("name",Qt::CaseInsensitive) == 0)
						{
							VLink_Exp_Match ext_match;
							ext_match.parse_exp_match_string(pied->name);
							bool bret = ext_match.check_match_string(check_ied->name);
							bFind &= bret;
						}
					}
					if(bFind)
					{
						v_ied = pied;
						break;
					}
				}
				else
				{
					v_ied = pied;
					break;
				}
			}
		}
	}

	if(!v_ied)
	{
		infor_text.clear();
		str_text = QString("    ")+tr("IED 【%1(%2)】 不匹配!").arg(check_ied->desc).arg(check_ied->name);
		infor_text[str_text] = QColor(255,0,0);
		check_info.append(infor_text);
		return false;
	}

	infor_text.clear();
	str_text = QString("    ")+tr("IED 【%1(%2)】 匹配到对应的IED连接模板 【%3(%4)】!").arg(check_ied->desc).arg(check_ied->name).arg(v_ied->desc).arg(v_ied->name);
	infor_text[str_text] = QColor(0,0,0);
	check_info.append(infor_text);

	for (int i = 0; i < check_ied->vlink_gorx_list.count(); i++)
	{
		VLink_TxIed* check_txied = check_ied->vlink_gorx_list[i];
		VLink_TxIed* p_txied = 0;
		for (int j = 0; j < v_ied->vlink_gorx_list.count(); j++)
		{
			VLink_TxIed* ptxied = v_ied->vlink_gorx_list[j];
			if(!ptxied->desc.trimmed().isEmpty() && ptxied->desc == check_txied->desc)
			{
				p_txied = ptxied;
				break;
			}
			else 
			{
				ptxied->vlink_txied_match->parse_exp_match_string(ptxied->desckey);
				if(ptxied->vlink_txied_match->check_match_string(check_txied->desc))
				{
					if(!ptxied->extrule.trimmed().isEmpty())
					{
						VLink_Exp_Match ext_exp_match;
						ext_exp_match.parse_exp_match_string(ptxied->extrule);
						bool bFind = true;
						for (int k = 0; k < ext_exp_match.m_sSplitString.count(); k++)
						{
							QString ext_item = ext_exp_match.m_sSplitString[k];
							if(ext_item.compare("name",Qt::CaseInsensitive) == 0)
							{
								VLink_Exp_Match ext_match;
								ext_match.parse_exp_match_string(ptxied->name);
								bool bret = ext_match.check_match_string(check_txied->name);
								bFind &= bret;
							}
						}
						if(bFind)
						{
							p_txied = ptxied;
							break;
						}
					}
					else
					{
						p_txied = ptxied;
						break;
					}
				}
			}
		}
		if(!p_txied)
		{
			infor_text.clear();
			str_text = QString("    ")+tr("GOOSE虚回路 【%1(%2)】 不匹配!").arg(check_txied->desc).arg(check_txied->name);
			infor_text[str_text] = QColor(255,0,0);
			check_info.append(infor_text);
			continue;
		}

		infor_text.clear();
		str_text = QString("    ")+tr("GOOSE虚回路 【%1(%2)】 匹配到对应的GOOSE虚回路连接模板 【%3(%4)】!").arg(check_txied->desc).arg(check_txied->name).arg(p_txied->desc).arg(p_txied->name);
		infor_text[str_text] = QColor(0,0,0);
		check_info.append(infor_text);

		if(p_txied->vlink_vlink_list.count() != check_txied->vlink_vlink_list.count())
		{
			infor_text.clear();
			str_text = QString("        ")+tr("GOOSE虚回路 【%1(%2)】 虚端子个数 (%3) 与对应的GOOSE虚回路连接模板 【%4(%5)】 虚端子个数 (%6) 不一致!")
				.arg(check_txied->desc).arg(check_txied->name).arg(check_txied->vlink_vlink_list.count())
				.arg(p_txied->desc).arg(p_txied->name).arg(p_txied->vlink_vlink_list.count());
			infor_text[str_text] = QColor(255,0,0);
			check_info.append(infor_text);
		}

		for (int j = 0; j < p_txied->vlink_vlink_list.count(); j++)
		{
			VLink_VLink* pvlink = p_txied->vlink_vlink_list[j];
			if(!pvlink->vlink_rx)
				continue;
			VLink_VLink* p_check_vlink = 0;
			for (int k = 0; k < check_txied->vlink_vlink_list.count(); k++)
			{
				VLink_VLink* check_vlink = check_txied->vlink_vlink_list[k];
				if(pvlink->vlink_rx)
				{
					pvlink->vlink_rx->vlink_terminal_match->parse_exp_match_string(pvlink->vlink_rx->desckey);
					if(pvlink->vlink_rx->vlink_terminal_match->check_match_string(check_vlink->desc))
					{
						{
							p_check_vlink = check_vlink;
							break;
						}
					}
				}
			}

			if(!p_check_vlink)
			{
				infor_text.clear();
				str_text = QString("        ")+tr("GOOSE虚端子连接模板 【%1】 未匹配到SCD中GOOSE虚端子连接!").arg(pvlink->desc);
				infor_text[str_text] = QColor(255,0,0);
				check_info.append(infor_text);
				continue;
			}

			infor_text.clear();
			str_text = QString("        ")+tr("GOOSE虚端子连接模板 【%1】 匹配到SCD中GOOSE虚端子连接 【%2(%3)】!").arg(pvlink->desc).arg(p_check_vlink->desc).arg(p_check_vlink->mark);
			infor_text[str_text] = QColor(0,0,0);
			check_info.append(infor_text);

			if(pvlink->vlink_rx && pvlink->vlink_tx)
			{
				if(pvlink->vlink_rx->desckey.trimmed().isEmpty())
				{
					if(pvlink->vlink_rx->refkey.trimmed().isEmpty())
					{
						infor_text.clear();
						str_text = QString("            ")+tr("GOOSE虚端子连接模板 【%1】 接收侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_rx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
					else
					{
						pvlink->vlink_rx->vlink_terminal_match->parse_ref_match_string(pvlink->vlink_rx->refkey);
						if(pvlink->vlink_rx->vlink_terminal_match->check_match_ref_string(p_check_vlink->vlink_rx->stdref))
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接模板 【%1】 接收侧 【%2】 匹配!").arg(pvlink->desc).arg(pvlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接模板 【%1】 接收侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
				}
				else
				{
					pvlink->vlink_rx->vlink_terminal_match->parse_exp_match_string(pvlink->vlink_rx->desckey);
					if(pvlink->vlink_rx->vlink_terminal_match->check_match_string(p_check_vlink->vlink_rx->stddesc))
					{
						bool bFind = true;
						if(!pvlink->vlink_rx->extrule.trimmed().isEmpty())
						{
							VLink_Exp_Match ext_exp_match;
							ext_exp_match.parse_exp_match_string(pvlink->vlink_rx->extrule);
							for (int k = 0; k < ext_exp_match.m_sSplitString.count(); k++)
							{
								QString ext_item = ext_exp_match.m_sSplitString[k];
								if(ext_item.compare("refKey",Qt::CaseInsensitive) == 0)
								{
									VLink_Exp_Match ext_match;
									ext_match.parse_ref_match_string(pvlink->vlink_rx->refkey);
									bool bret = ext_match.check_match_ref_string(p_check_vlink->vlink_rx->stdref);
									bFind &= bret;
								}
							}

						}
						if(bFind)
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接模板 【%1】 接收侧 【%2】 匹配!").arg(pvlink->desc).arg(pvlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接模板 【%1】 接收侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
					else
					{
						infor_text.clear();
						str_text = QString("            ")+tr("GOOSE虚端子连接模板 【%1】 接收侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_rx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
				}

				if(pvlink->vlink_tx->desckey.trimmed().isEmpty())
				{
					if(pvlink->vlink_tx->refkey.isEmpty())
					{
						infor_text.clear();
						str_text = QString("            ")+tr("GOOSE虚端子连接模板 【%1】 发送侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_tx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
					else
					{
						pvlink->vlink_tx->vlink_terminal_match->parse_ref_match_string(pvlink->vlink_tx->refkey);
						if(pvlink->vlink_tx->vlink_terminal_match->check_match_ref_string(p_check_vlink->vlink_tx->stdref))
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接模板 【%1】 发送侧 【%2】 匹配!").arg(pvlink->desc).arg(pvlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接模板 【%1】 发送侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
				}
				else
				{
					pvlink->vlink_tx->vlink_terminal_match->parse_exp_match_string(pvlink->vlink_tx->desckey);
					if(pvlink->vlink_tx->vlink_terminal_match->check_match_string(p_check_vlink->vlink_tx->stddesc))
					{
						bool bFind = true;
						if(!pvlink->vlink_tx->extrule.trimmed().isEmpty())
						{
							VLink_Exp_Match ext_exp_match;
							ext_exp_match.parse_exp_match_string(pvlink->vlink_tx->extrule);
							for (int k = 0; k < ext_exp_match.m_sSplitString.count(); k++)
							{
								QString ext_item = ext_exp_match.m_sSplitString[k];
								if(ext_item.compare("refKey",Qt::CaseInsensitive) == 0)
								{
									VLink_Exp_Match ext_match;
									ext_match.parse_ref_match_string(pvlink->vlink_tx->refkey);
									bool bret = ext_match.check_match_ref_string(p_check_vlink->vlink_tx->stdref);
									bFind &= bret;
								}
							}

						}
						if(bFind)
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接模板 【%1】 发送侧 【%2】 匹配!").arg(pvlink->desc).arg(pvlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接模板 【%1】 发送侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
					else
					{
						infor_text.clear();
						str_text = QString("            ")+tr("GOOSE虚端子连接模板 【%1】 发送侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_tx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
				}
			}
		}

		//未匹配上的scd虚端子
		QStringList unmatch_list;
		for (int j = 0; j < check_txied->vlink_vlink_list.count(); j++)
		{
			VLink_VLink* check_vlink = check_txied->vlink_vlink_list[j];
			VLink_VLink* p_vlink = 0;
			for (int k = 0; k < p_txied->vlink_vlink_list.count(); k++)
			{
				VLink_VLink* pvlink = p_txied->vlink_vlink_list[k];
				if(pvlink->vlink_rx)
				{
					pvlink->vlink_rx->vlink_terminal_match->parse_exp_match_string(pvlink->vlink_rx->desckey);
					if(pvlink->vlink_rx->vlink_terminal_match->check_match_string(check_vlink->desc))
					{
						{
							p_vlink = pvlink;
							break;
						}
					}
				}
			}

			if(!p_vlink)
			{
				unmatch_list.append(check_vlink->desc);
				continue;
			}
		}

		if(unmatch_list.count() > 0)
		{
			infor_text.clear();
			str_text = QString("        ")+tr("SCD中未匹配上的虚端子!");
			infor_text[str_text] = QColor(255,0,0);
			check_info.append(infor_text);

			for (int j = 0; j < unmatch_list.count(); j++)
			{
				infor_text.clear();
				str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】").arg(unmatch_list[j]);
				infor_text[str_text] = QColor(255,0,0);
				check_info.append(infor_text);
			}
		}
	}

	for (int i = 0; i < check_ied->vlink_svrx_list.count(); i++)
	{
		VLink_TxIed* check_txied = check_ied->vlink_svrx_list[i];
		VLink_TxIed* p_txied = 0;
		for (int j = 0; j < v_ied->vlink_svrx_list.count(); j++)
		{
			VLink_TxIed* ptxied = v_ied->vlink_svrx_list[j];
			if(!ptxied->desc.trimmed().isEmpty() && ptxied->desc == check_txied->desc)
			{
				p_txied = ptxied;
				break;
			}
			else 
			{
				ptxied->vlink_txied_match->parse_exp_match_string(ptxied->desckey);
				if(ptxied->vlink_txied_match->check_match_string(check_txied->desc))
				{
					if(!ptxied->extrule.trimmed().isEmpty())
					{
						VLink_Exp_Match ext_exp_match;
						ext_exp_match.parse_exp_match_string(ptxied->extrule);
						bool bFind = true;
						for (int k = 0; k < ext_exp_match.m_sSplitString.count(); k++)
						{
							QString ext_item = ext_exp_match.m_sSplitString[k];
							if(ext_item.compare("name",Qt::CaseInsensitive) == 0)
							{
								VLink_Exp_Match ext_match;
								ext_match.parse_exp_match_string(ptxied->name);
								bool bret = ext_match.check_match_string(check_txied->name);
								bFind &= bret;
							}
						}
						if(bFind)
						{
							p_txied = ptxied;
							break;
						}
					}
					else
					{
						p_txied = ptxied;
						break;
					}
				}
			}
		}
		if(!p_txied)
		{
			infor_text.clear();
			str_text = QString("    ")+tr("SV虚回路 【%1(%2)】 不匹配!").arg(check_txied->desc).arg(check_txied->name);
			infor_text[str_text] = QColor(255,0,0);
			check_info.append(infor_text);
			continue;
		}

		infor_text.clear();
		str_text = QString("    ")+tr("SV虚回路 【%1(%2)】 匹配到对应的SV虚回路连接模板 【%3(%4)】!").arg(check_txied->desc).arg(check_txied->name).arg(p_txied->desc).arg(p_txied->name);
		infor_text[str_text] = QColor(0,0,0);
		check_info.append(infor_text);

		if(p_txied->vlink_vlink_list.count() != check_txied->vlink_vlink_list.count())
		{
			infor_text.clear();
			str_text = QString("        ")+tr("SV虚回路 【%1(%2)】 虚端子个数 (%3) 与对应的SV虚回路连接模板 【%4(%5)】 虚端子个数 (%6) 不一致!")
				.arg(check_txied->desc).arg(check_txied->name).arg(check_txied->vlink_vlink_list.count())
				.arg(p_txied->desc).arg(p_txied->name).arg(p_txied->vlink_vlink_list.count());
			infor_text[str_text] = QColor(255,0,0);
			check_info.append(infor_text);
		}

		for (int j = 0; j < p_txied->vlink_vlink_list.count(); j++)
		{
			VLink_VLink* pvlink = p_txied->vlink_vlink_list[j];
			if(!pvlink->vlink_rx)
				continue;
			VLink_VLink* p_check_vlink = 0;
			for (int k = 0; k < check_txied->vlink_vlink_list.count(); k++)
			{
				VLink_VLink* check_vlink = check_txied->vlink_vlink_list[k];
				if(pvlink->vlink_rx)
				{
					pvlink->vlink_rx->vlink_terminal_match->parse_exp_match_string(pvlink->vlink_rx->desckey);
					if(pvlink->vlink_rx->vlink_terminal_match->check_match_string(check_vlink->desc))
					{
						{
							p_check_vlink = check_vlink;
							break;
						}
					}
				}
			}

			if(!p_check_vlink)
			{
				infor_text.clear();
				str_text = QString("        ")+tr("SV虚端子连接模板 【%1】 未匹配到SCD中SV虚端子连接!").arg(pvlink->desc);
				infor_text[str_text] = QColor(255,0,0);
				check_info.append(infor_text);
				continue;
			}

			infor_text.clear();
			str_text = QString("        ")+tr("SV虚端子连接模板 【%1】 匹配到SCD中SV虚端子连接 【%2(%3)】!").arg(pvlink->desc).arg(p_check_vlink->desc).arg(p_check_vlink->mark);
			infor_text[str_text] = QColor(0,0,0);
			check_info.append(infor_text);

			if(pvlink->vlink_rx && pvlink->vlink_tx)
			{
				if(pvlink->vlink_rx->desckey.trimmed().isEmpty())
				{
					if(pvlink->vlink_rx->refkey.trimmed().isEmpty())
					{
						infor_text.clear();
						str_text = QString("            ")+tr("SV虚端子连接模板 【%1】 接收侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_rx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
					else
					{
						pvlink->vlink_rx->vlink_terminal_match->parse_ref_match_string(pvlink->vlink_rx->refkey);
						if(pvlink->vlink_rx->vlink_terminal_match->check_match_ref_string(p_check_vlink->vlink_rx->stdref))
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接模板 【%1】 接收侧 【%2】 匹配!").arg(pvlink->desc).arg(pvlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接模板 【%1】 接收侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
				}
				else
				{
					pvlink->vlink_rx->vlink_terminal_match->parse_exp_match_string(pvlink->vlink_rx->desckey);
					if(pvlink->vlink_rx->vlink_terminal_match->check_match_string(p_check_vlink->vlink_rx->stddesc))
					{
						bool bFind = true;
						if(!pvlink->vlink_rx->extrule.trimmed().isEmpty())
						{
							VLink_Exp_Match ext_exp_match;
							ext_exp_match.parse_exp_match_string(pvlink->vlink_rx->extrule);
							for (int k = 0; k < ext_exp_match.m_sSplitString.count(); k++)
							{
								QString ext_item = ext_exp_match.m_sSplitString[k];
								if(ext_item.compare("refKey",Qt::CaseInsensitive) == 0)
								{
									VLink_Exp_Match ext_match;
									ext_match.parse_ref_match_string(pvlink->vlink_rx->refkey);
									bool bret = ext_match.check_match_ref_string(p_check_vlink->vlink_rx->stdref);
									bFind &= bret;
								}
							}

						}
						if(bFind)
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接模板 【%1】 接收侧 【%2】 匹配!").arg(pvlink->desc).arg(pvlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接模板 【%1】 接收侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
					else
					{
						infor_text.clear();
						str_text = QString("            ")+tr("SV虚端子连接模板 【%1】 接收侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_rx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
				}

				if(pvlink->vlink_tx->desckey.trimmed().isEmpty())
				{
					if(pvlink->vlink_tx->refkey.isEmpty())
					{
						infor_text.clear();
						str_text = QString("            ")+tr("SV虚端子连接模板 【%1】 发送侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_tx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
					else
					{
						pvlink->vlink_tx->vlink_terminal_match->parse_ref_match_string(pvlink->vlink_tx->refkey);
						if(pvlink->vlink_tx->vlink_terminal_match->check_match_ref_string(p_check_vlink->vlink_tx->stdref))
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接模板 【%1】 发送侧 【%2】 匹配!").arg(pvlink->desc).arg(pvlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接模板 【%1】 发送侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
				}
				else
				{
					pvlink->vlink_tx->vlink_terminal_match->parse_exp_match_string(pvlink->vlink_tx->desckey);
					if(pvlink->vlink_tx->vlink_terminal_match->check_match_string(p_check_vlink->vlink_tx->stddesc))
					{
						bool bFind = true;
						if(!pvlink->vlink_tx->extrule.trimmed().isEmpty())
						{
							VLink_Exp_Match ext_exp_match;
							ext_exp_match.parse_exp_match_string(pvlink->vlink_tx->extrule);
							for (int k = 0; k < ext_exp_match.m_sSplitString.count(); k++)
							{
								QString ext_item = ext_exp_match.m_sSplitString[k];
								if(ext_item.compare("refKey",Qt::CaseInsensitive) == 0)
								{
									VLink_Exp_Match ext_match;
									ext_match.parse_ref_match_string(pvlink->vlink_tx->refkey);
									bool bret = ext_match.check_match_ref_string(p_check_vlink->vlink_tx->stdref);
									bFind &= bret;
								}
							}

						}
						if(bFind)
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接模板 【%1】 发送侧 【%2】 匹配!").arg(pvlink->desc).arg(pvlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接模板 【%1】 发送侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
					else
					{
						infor_text.clear();
						str_text = QString("            ")+tr("SV虚端子连接模板 【%1】 发送侧 【%2】 不匹配!").arg(pvlink->desc).arg(pvlink->vlink_tx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
				}
			}
		}

		//未匹配上的scd虚端子
		QStringList unmatch_list;
		for (int j = 0; j < check_txied->vlink_vlink_list.count(); j++)
		{
			VLink_VLink* check_vlink = check_txied->vlink_vlink_list[j];
			VLink_VLink* p_vlink = 0;
			for (int k = 0; k < p_txied->vlink_vlink_list.count(); k++)
			{
				VLink_VLink* pvlink = p_txied->vlink_vlink_list[k];
				if(pvlink->vlink_rx)
				{
					pvlink->vlink_rx->vlink_terminal_match->parse_exp_match_string(pvlink->vlink_rx->desckey);
					if(pvlink->vlink_rx->vlink_terminal_match->check_match_string(check_vlink->desc))
					{
						{
							p_vlink = pvlink;
							break;
						}
					}
				}
			}

			if(!p_vlink)
			{
				unmatch_list.append(check_vlink->desc);
				continue;
			}
		}

		if(unmatch_list.count() > 0)
		{
			infor_text.clear();
			str_text = QString("        ")+tr("SCD中未匹配上的虚端子!");
			infor_text[str_text] = QColor(255,0,0);
			check_info.append(infor_text);

			for (int j = 0; j < unmatch_list.count(); j++)
			{
				infor_text.clear();
				str_text = QString("            ")+tr("SV虚端子连接 【%1】").arg(unmatch_list[j]);
				infor_text[str_text] = QColor(255,0,0);
				check_info.append(infor_text);
			}
		}
	}

	/*
	for (int i = 0; i < check_ied->vlink_gorx_list.count(); i++)
	{
		VLink_TxIed* check_txied = check_ied->vlink_gorx_list[i];
		VLink_TxIed* p_txied = 0;
		for (int j = 0; j < check_txied->vlink_vlink_list.count(); j++)
		{
			VLink_VLink* check_vlink = check_txied->vlink_vlink_list[j];
			VLink_VLink* p_vlink = 0;
			for (int k = 0; k < p_txied->vlink_vlink_list.count(); k++)
			{
				VLink_VLink* pvlink = p_txied->vlink_vlink_list[k];
				if(pvlink->vlink_rx)
				{
					pvlink->vlink_rx->vlink_terminal_match->parse_exp_match_string(pvlink->vlink_rx->desckey);
					if(pvlink->vlink_rx->vlink_terminal_match->check_match_string(check_vlink->desc))
					{
						{
							p_vlink = pvlink;
							break;
						}
					}
				}
// 				if(check_vlink->desc == pvlink->desc)
// 				{
// 					p_vlink = pvlink;
// 					break;
// 				}
			}

			if(!p_vlink)
			{
				infor_text.clear();
				str_text = QString("        ")+tr("GOOSE虚端子连接 【%1】 不匹配!").arg(check_vlink->desc);
				infor_text[str_text] = QColor(255,0,0);
				check_info.append(infor_text);
				continue;
			}

			infor_text.clear();
			str_text = QString("        ")+tr("GOOSE虚端子连接 【%1】 匹配到对应的GOOSE虚端子连接模板 【%2(%3)】!").arg(check_vlink->desc).arg(p_vlink->desc).arg(p_vlink->mark);
			infor_text[str_text] = QColor(0,0,0);
			check_info.append(infor_text);
			
			if(p_vlink->vlink_rx && p_vlink->vlink_tx)
			{
				if(p_vlink->vlink_rx->desckey.trimmed().isEmpty())
				{
					if(p_vlink->vlink_rx->refkey.trimmed().isEmpty())
					{
						infor_text.clear();
						str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】 接收侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_rx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
					else
					{
						//p_vlink->vlink_rx->vlink_terminal_match->parse_exp_match_string(p_vlink->vlink_rx->refkey);
						p_vlink->vlink_rx->vlink_terminal_match->parse_ref_match_string(p_vlink->vlink_rx->refkey);
						if(p_vlink->vlink_rx->vlink_terminal_match->check_match_ref_string(check_vlink->vlink_rx->stdref))
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】 接收侧 【%2】 匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】 接收侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
				}
				else
				{
					p_vlink->vlink_rx->vlink_terminal_match->parse_exp_match_string(p_vlink->vlink_rx->desckey);
					if(p_vlink->vlink_rx->vlink_terminal_match->check_match_string(check_vlink->vlink_rx->stddesc))
					{
						bool bFind = true;
						if(!p_vlink->vlink_rx->extrule.trimmed().isEmpty())
						{
							VLink_Exp_Match ext_exp_match;
							ext_exp_match.parse_exp_match_string(p_vlink->vlink_rx->extrule);
							for (int k = 0; k < ext_exp_match.m_sSplitString.count(); k++)
							{
								QString ext_item = ext_exp_match.m_sSplitString[k];
								if(ext_item.compare("refKey",Qt::CaseInsensitive) == 0)
								{
									VLink_Exp_Match ext_match;
									ext_match.parse_ref_match_string(p_vlink->vlink_rx->refkey);
									bool bret = ext_match.check_match_ref_string(check_vlink->vlink_rx->stdref);
									bFind &= bret;
								}
							}

						}
						if(bFind)
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】 接收侧 【%2】 匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】 接收侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
					else
					{
						infor_text.clear();
						str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】 接收侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_rx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
				}
				
				if(p_vlink->vlink_tx->desckey.trimmed().isEmpty())
				{
					if(p_vlink->vlink_tx->refkey.isEmpty())
					{
						infor_text.clear();
						str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】 发送侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_tx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
					else
					{
						p_vlink->vlink_tx->vlink_terminal_match->parse_ref_match_string(p_vlink->vlink_tx->refkey);
						if(p_vlink->vlink_tx->vlink_terminal_match->check_match_ref_string(check_vlink->vlink_tx->stdref))
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】 发送侧 【%2】 匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】 发送侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
				}
				else
				{
					p_vlink->vlink_tx->vlink_terminal_match->parse_exp_match_string(p_vlink->vlink_tx->desckey);
					if(p_vlink->vlink_tx->vlink_terminal_match->check_match_string(check_vlink->vlink_tx->stddesc))
					{
						bool bFind = true;
						if(!p_vlink->vlink_tx->extrule.trimmed().isEmpty())
						{
							VLink_Exp_Match ext_exp_match;
							ext_exp_match.parse_exp_match_string(p_vlink->vlink_tx->extrule);
							for (int k = 0; k < ext_exp_match.m_sSplitString.count(); k++)
							{
								QString ext_item = ext_exp_match.m_sSplitString[k];
								if(ext_item.compare("refKey",Qt::CaseInsensitive) == 0)
								{
									VLink_Exp_Match ext_match;
									ext_match.parse_ref_match_string(p_vlink->vlink_tx->refkey);
									bool bret = ext_match.check_match_ref_string(check_vlink->vlink_tx->stdref);
									bFind &= bret;
								}
							}

						}
						if(bFind)
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】 发送侧 【%2】 匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】 发送侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
					else
					{
						infor_text.clear();
						str_text = QString("            ")+tr("GOOSE虚端子连接 【%1】 发送侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_tx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
				}
			}
		}
	}
	
	for (int i = 0; i < check_ied->vlink_svrx_list.count(); i++)
	{
		VLink_TxIed* check_txied = check_ied->vlink_svrx_list[i];
		VLink_TxIed* p_txied = 0;
		for (int j = 0; j < v_ied->vlink_svrx_list.count(); j++)
		{
			VLink_TxIed* ptxied = v_ied->vlink_svrx_list[j];
			if(!ptxied->desc.trimmed().isEmpty() && ptxied->desc == check_txied->desc)
			{
				p_txied = ptxied;
				break;
			}
			else 
			{
				ptxied->vlink_txied_match->parse_exp_match_string(ptxied->desckey);
				if(ptxied->vlink_txied_match->check_match_string(check_txied->desc))
				{
					if(!ptxied->extrule.trimmed().isEmpty())
					{
						VLink_Exp_Match ext_exp_match;
						ext_exp_match.parse_exp_match_string(ptxied->extrule);
						bool bFind = true;
						for (int k = 0; k < ext_exp_match.m_sSplitString.count(); k++)
						{
							QString ext_item = ext_exp_match.m_sSplitString[k];
							if(ext_item.compare("name",Qt::CaseInsensitive) == 0)
							{
								VLink_Exp_Match ext_match;
								ext_match.parse_exp_match_string(ptxied->name);
								bool bret = ext_match.check_match_string(check_txied->name);
								bFind &= bret;
							}
						}
						if(bFind)
						{
							p_txied = ptxied;
							break;
						}
					}
					else
					{
						p_txied = ptxied;
						break;
					}
				}
			}
		}
		if(!p_txied)
		{
			infor_text.clear();
			str_text = QString("    ")+tr("SV虚回路 【%1(%2)】 不匹配!").arg(check_txied->desc).arg(check_txied->name);
			infor_text[str_text] = QColor(255,0,0);
			check_info.append(infor_text);
			continue;
		}


		infor_text.clear();
		str_text = QString("    ")+tr("SV虚回路 【%1(%2)】 匹配到对应的SV虚回路连接模板 【%3(%4)】!").arg(check_txied->desc).arg(check_txied->name).arg(p_txied->desc).arg(p_txied->name);
		infor_text[str_text] = QColor(0,0,0);
		check_info.append(infor_text);

		for (int j = 0; j < check_txied->vlink_vlink_list.count(); j++)
		{
			VLink_VLink* check_vlink = check_txied->vlink_vlink_list[j];
			VLink_VLink* p_vlink = 0;
			for (int k = 0; k < p_txied->vlink_vlink_list.count(); k++)
			{
				VLink_VLink* pvlink = p_txied->vlink_vlink_list[k];
				if(pvlink->vlink_rx)
				{
					pvlink->vlink_rx->vlink_terminal_match->parse_exp_match_string(pvlink->vlink_rx->desckey);
					if(pvlink->vlink_rx->vlink_terminal_match->check_match_string(check_vlink->desc))
					{
						{
							p_vlink = pvlink;
							break;
						}
					}
				}
// 				if(check_vlink->desc == pvlink->desc)
// 				{
// 					p_vlink = pvlink;
// 					break;
// 				}
			}

			if(!p_vlink)
			{
				infor_text.clear();
				str_text = QString("        ")+tr("SV虚端子连接 【%1】 未匹配!").arg(check_vlink->desc);
				infor_text[str_text] = QColor(255,0,0);
				check_info.append(infor_text);
				continue;
			}

			infor_text.clear();
			str_text = QString("        ")+tr("SV虚端子连接 【%1】 匹配到对应的SV虚端子连接模板 【%2(%3)】!").arg(check_vlink->desc).arg(p_vlink->desc).arg(p_vlink->mark);
			infor_text[str_text] = QColor(0,0,0);
			check_info.append(infor_text);

			if(p_vlink->vlink_rx && p_vlink->vlink_tx)
			{
				if(p_vlink->vlink_rx->desckey.trimmed().isEmpty())
				{
					if(p_vlink->vlink_rx->refkey.trimmed().isEmpty())
					{
						infor_text.clear();
						str_text = QString("            ")+tr("SV虚端子连接 【%1】 接收侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_rx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
					else
					{
						p_vlink->vlink_rx->vlink_terminal_match->parse_ref_match_string(p_vlink->vlink_rx->refkey);
						if(p_vlink->vlink_rx->vlink_terminal_match->check_match_ref_string(check_vlink->vlink_rx->stdref))
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接 【%1】 接收侧 【%2】 匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接 【%1】 接收侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
				}
				else
				{
					p_vlink->vlink_rx->vlink_terminal_match->parse_exp_match_string(p_vlink->vlink_rx->desckey);
					if(p_vlink->vlink_rx->vlink_terminal_match->check_match_string(check_vlink->vlink_rx->stddesc))
					{
						bool bFind = true;
						if(!p_vlink->vlink_rx->extrule.trimmed().isEmpty())
						{
							VLink_Exp_Match ext_exp_match;
							ext_exp_match.parse_exp_match_string(p_vlink->vlink_rx->extrule);
							for (int k = 0; k < ext_exp_match.m_sSplitString.count(); k++)
							{
								QString ext_item = ext_exp_match.m_sSplitString[k];
								if(ext_item.compare("refKey",Qt::CaseInsensitive) == 0)
								{
									VLink_Exp_Match ext_match;
									ext_match.parse_ref_match_string(p_vlink->vlink_rx->refkey);
									bool bret = ext_match.check_match_ref_string(check_vlink->vlink_rx->stdref);
									bFind &= bret;
								}
							}

						}
						if(bFind)
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接 【%1】 接收侧 【%2】 匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接 【%1】 接收侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_rx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
					else
					{
						infor_text.clear();
						str_text = QString("            ")+tr("SV虚端子连接 【%1】 接收侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_rx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
				}
				

				if(p_vlink->vlink_tx->desckey.trimmed().isEmpty())
				{
					if(p_vlink->vlink_tx->refkey.trimmed().isEmpty())
					{
						infor_text.clear();
						str_text = QString("            ")+tr("SV虚端子连接 【%1】 发送侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_tx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
					else
					{
						p_vlink->vlink_tx->vlink_terminal_match->parse_ref_match_string(p_vlink->vlink_tx->refkey);
						if(p_vlink->vlink_tx->vlink_terminal_match->check_match_ref_string(check_vlink->vlink_tx->stdref))
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接 【%1】 发送侧 【%2】 匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接 【%1】 发送侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
				}
				else
				{
					p_vlink->vlink_tx->vlink_terminal_match->parse_exp_match_string(p_vlink->vlink_tx->desckey);
					if(p_vlink->vlink_tx->vlink_terminal_match->check_match_string(check_vlink->vlink_tx->stddesc))
					{
						bool bFind = true;
						if(!p_vlink->vlink_tx->extrule.trimmed().isEmpty())
						{
							VLink_Exp_Match ext_exp_match;
							ext_exp_match.parse_exp_match_string(p_vlink->vlink_tx->extrule);
							for (int k = 0; k < ext_exp_match.m_sSplitString.count(); k++)
							{
								QString ext_item = ext_exp_match.m_sSplitString[k];
								if(ext_item.compare("refKey",Qt::CaseInsensitive) == 0)
								{
									VLink_Exp_Match ext_match;
									ext_match.parse_ref_match_string(p_vlink->vlink_tx->refkey);
									bool bret = ext_match.check_match_ref_string(check_vlink->vlink_tx->stdref);
									bFind &= bret;
								}
							}

						}
						if(bFind)
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接 【%1】 发送侧 【%2】 匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(0,0,0);
							check_info.append(infor_text);
						}
						else
						{
							infor_text.clear();
							str_text = QString("            ")+tr("SV虚端子连接 【%1】 发送侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_tx->stddesc);
							infor_text[str_text] = QColor(255,0,0);
							check_info.append(infor_text);
						}
					}
					else
					{
						infor_text.clear();
						str_text = QString("            ")+tr("SV虚端子连接 【%1】 发送侧 【%2】 不匹配!").arg(check_vlink->desc).arg(check_vlink->vlink_tx->stddesc);
						infor_text[str_text] = QColor(255,0,0);
						check_info.append(infor_text);
					}
				}
				
			}
		}
	}
	*/

	return true;
}




