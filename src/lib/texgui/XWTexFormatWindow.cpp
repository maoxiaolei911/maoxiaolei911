/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWTexFormatWindow.h"


XWTexFormatWindow::XWTexFormatWindow(QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Tex Formats"));
    setWindowIcon(QIcon(":/images/xiuwen24.png"));
    
    nameLabel = new QLabel(tr("Format name:"));
    nameEdit = new QLineEdit;
    
    fmts = new QListWidget;
    params = new XWTeXFormatWidget(this);
    
    QStringList list = params->getAllFmts();
    if (list.size() > 0)
    {
    	list.sort();
    	fmts->addItems(list);
    }
    
    addButton = new QPushButton(tr("Add"));
    connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
    
    QHBoxLayout * namelayout = new QHBoxLayout;
    namelayout->addWidget(nameLabel);
    namelayout->addWidget(nameEdit, 1);
    namelayout->addWidget(addButton);
    
    QVBoxLayout * leftlayout = new QVBoxLayout;
    leftlayout->addLayout(namelayout);
    leftlayout->addWidget(fmts);
    
    QHBoxLayout * toplayout = new QHBoxLayout;
    toplayout->addLayout(leftlayout);
    toplayout->addWidget(params, 1);
    
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
		connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(toplayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    
    connect(fmts, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), 
            this, SLOT(setCurrentFormat(QListWidgetItem *, QListWidgetItem *)));
		resize(650, 400);
}

void XWTexFormatWindow::accept()
{
	params->save();
	QDialog::accept();
}

QString XWTexFormatWindow::getFmt()
{
	QString fmt;
	QListWidgetItem * item = fmts->currentItem();
	if (item)
		fmt = item->text();
	return fmt;
}

void XWTexFormatWindow::setCurrentFmt(const QString & fmt)
{
	if (fmt.isEmpty())
		return ;
		
	QListWidgetItem * item = 0;
	QList<QListWidgetItem *> list = fmts->findItems(fmt, Qt::MatchExactly);
	if (list.isEmpty())
	{
		item = new QListWidgetItem(fmt, fmts);
		fmts->addItem(item);
		fmts->setCurrentItem(item);
		params->setCurrentFormat(fmt);
	}
	else
		item = list[0];
	
	if (item != fmts->currentItem())
		fmts->setCurrentItem(item);
}

void XWTexFormatWindow::add()
{
	QString fmt = nameEdit->text();
	if (!fmt.isEmpty())
	{
		 QList<QListWidgetItem *> list = fmts->findItems(fmt, Qt::MatchExactly);
	   if (list.isEmpty())
	   {
	   		QListWidgetItem * item = new QListWidgetItem(fmt, fmts);
	   		fmts->addItem(item);
	   		fmts->setCurrentItem(item);
	   		
	   		params->setCurrentFormat(fmt);
	   }
	   else
	   {
	   		QListWidgetItem * item = list[0];
	   		fmts->setCurrentItem(item);
	   		params->setCurrentFormat(fmt);
	   }
	}
}

void XWTexFormatWindow::setCurrentFormat(QListWidgetItem * current, QListWidgetItem * previous)
{
	if (current != previous && current)
	{
		QString fmt = current->text();
		params->setCurrentFormat(fmt);
	}
}

XWTeXFormatDelegate::XWTeXFormatDelegate(QObject *parent)
	:QItemDelegate(parent)
{
}

QWidget * XWTeXFormatDelegate::createEditor(QWidget *parent, 
	                                        const QStyleOptionViewItem &,
                                            const QModelIndex &index) const
{
	if (index.column() != 1)
		return 0;
		
	int type = index.model()->data(index, Qt::UserRole).toInt();
	if (type == 0)
	{
		QLineEdit *leditor = new QLineEdit(parent);
		return leditor;
	}
	
	QSpinBox *seditor = new QSpinBox(parent);
	return seditor;
}

void XWTeXFormatDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	int type = index.model()->data(index, Qt::UserRole).toInt();
	if (type == 0)
	{
		QLineEdit *leditor = static_cast<QLineEdit*>(editor);
		QString str = index.model()->data(index, Qt::EditRole).toString();
		leditor->setText(str);
	}
	else
	{
		QSpinBox *seditor = static_cast<QSpinBox*>(editor);
		int min = index.model()->data(index, Qt::UserRole + 1).toInt();
		int max = index.model()->data(index, Qt::UserRole + 2).toInt();
		seditor->setMinimum(min);
     	seditor->setMaximum(max);
     	
     	int value = index.model()->data(index, Qt::EditRole).toInt();
     	seditor->setValue(value);
	}
}

void XWTeXFormatDelegate::setModelData(QWidget *editor, 
	                                   QAbstractItemModel *model,
                                       const QModelIndex &index) const
{
	int type = index.model()->data(index, Qt::UserRole).toInt();
	if (type == 0)
	{
		QLineEdit *leditor = static_cast<QLineEdit*>(editor);
		QString str = leditor->text();
		model->setData(index, str, Qt::EditRole);
	}
	else
	{
		QSpinBox *seditor = static_cast<QSpinBox*>(editor);
		seditor->interpretText();
     	int value = seditor->value();
     	model->setData(index, value, Qt::EditRole);
	}
}

void XWTeXFormatDelegate::updateEditorGeometry(QWidget *editor,
                                               const QStyleOptionViewItem &option, 
                                               const QModelIndex & /*index*/) const
{
	editor->setGeometry(option.rect);
}


XWTeXFormatWidget::XWTeXFormatWidget(QWidget *parent)
	:QTreeWidget(parent)
{
	setup();	
	setDefault();
	allFmts << setting.allFormats();
	dirty = false;
	
	XWTeXFormatDelegate * delegate = new XWTeXFormatDelegate(this);
	setItemDelegate(delegate);
	resizeColumnToContents(0);
	resizeColumnToContents(1);
	connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)), 
	        this, SLOT(setDirty(QTreeWidgetItem *, int)));
}

QStringList XWTeXFormatWidget::getAllFmts()
{
	return allFmts;
}

void XWTeXFormatWidget::save()
{
	if (!dirty)
		return ;
		
	saveItem(0);
	saveItem(1);
	saveItem(2);
	saveItem(3);
	saveItem(4);
	saveItem(5);
	
	saveItem(6, "main_memory");
	saveItem(7, "extra_mem_top");
	saveItem(8, "extra_mem_bot");
	saveItem(9, "pool_size");
	saveItem(10, "string_vacancies");
	saveItem(11, "pool_free");
	saveItem(12, "max_strings");
	saveItem(13, "strings_free");
	saveItem(14, "trie_size");
	saveItem(15, "hyph_size");
	saveItem(16, "buf_size");
	saveItem(17, "nest_size");
	saveItem(18, "max_in_open");
	saveItem(19, "param_size");
	saveItem(20, "save_size");
	saveItem(21, "stack_size");
	saveItem(22, "error_line");
	saveItem(23, "half_error_line");
	saveItem(24, "max_print_line");
	saveItem(25, "ocp_list_size");
	saveItem(26, "ocp_buf_size");
	saveItem(27, "ocp_stack_size");
	if (!allFmts.contains(current))
		allFmts << current;
		
	dirty = false;
}

void XWTeXFormatWidget::setCurrentFormat(const QString & fmt)
{
	if (fmt == current)
		return ;
		
	current = fmt;
	if (!allFmts.contains(fmt))
	{
		setDefault();
		allFmts << fmt;
		dirty = true;
	}
	else
	{
		setting.setFormat(fmt);
		QString str = setting.getDescription();
		setValue(0, str);
		str = setting.getCompiler();
		setValue(1, str);
		str = setting.getInput();
		setValue(2, str);
		str = setting.getPreload();
		setValue(3, str);
		str = setting.getArguments();
		setValue(4, str);		
		str = setting.getAttributes();
		setValue(5, str);
		
		int iv = setting.setupBoundVar("main_memory", 1000000);
		setValue(6, iv);
		iv = setting.setupBoundVar("extra_mem_top", 0);
		setValue(7, iv);
		iv = setting.setupBoundVar("extra_mem_bot", 0);
		setValue(8, iv);
		iv = setting.setupBoundVar("pool_size", 300000);
		setValue(9, iv);
		iv = setting.setupBoundVar("string_vacancies", 200000);
		setValue(10, iv);
		iv = setting.setupBoundVar("pool_free", 5000);
		setValue(11, iv);
		iv = setting.setupBoundVar("max_strings", 100000);
		setValue(12, iv);
		iv = setting.setupBoundVar("strings_free", 100);
		setValue(13, iv);
		iv = setting.setupBoundVar("trie_size", 20000);
		setValue(14, iv);
		iv = setting.setupBoundVar("hyph_size", 659);
		setValue(15, iv);
		iv = setting.setupBoundVar("buf_size", 20000);
		setValue(16, iv);
		iv = setting.setupBoundVar("nest_size", 50);
		setValue(17, iv);
		iv = setting.setupBoundVar("max_in_open", 15);
		setValue(18, iv);
		iv = setting.setupBoundVar("param_size", 60);
		setValue(19, iv);
		iv = setting.setupBoundVar("save_size", 4000);
		setValue(20, iv);
		iv = setting.setupBoundVar("stack_size", 300);
		setValue(21, iv);
		iv = setting.setupBoundVar("error_line", 79);
		setValue(22, iv);
		iv = setting.setupBoundVar("half_error_line", 50);
		setValue(23, iv);
		iv = setting.setupBoundVar("max_print_line", 79);
		setValue(24, iv);
		iv = setting.setupBoundVar("ocp_list_size", 1000);
		setValue(25, iv);
		iv = setting.setupBoundVar("ocp_buf_size", 1000);
		setValue(26, iv);
		iv = setting.setupBoundVar("ocp_stack_size", 1000);
		setValue(27, iv);
		
		dirty = false;
	}
}

void XWTeXFormatWidget::setDirty(QTreeWidgetItem * item, int column)
{
	if (item && column == 1)
		dirty = true;
}

void XWTeXFormatWidget::createItem(const QString & title, 
	                               int min,
	                               int max)
{
	QTreeWidgetItem * item = new QTreeWidgetItem(this);
	addTopLevelItem(item);
	item->setText(0, title);
	item->setFlags(Qt::ItemIsSelectable | 
		           Qt::ItemIsEnabled | 
		           Qt::ItemIsEditable);
	item->setData(1, Qt::UserRole, 1);
	item->setData(1, Qt::UserRole + 1, min);
	item->setData(1, Qt::UserRole + 2, max);
}

void XWTeXFormatWidget::createItem(const QString & title)
{
	QTreeWidgetItem * item = new QTreeWidgetItem(this);
	addTopLevelItem(item);
	item->setText(0, title);
	item->setFlags(Qt::ItemIsSelectable | 
		           Qt::ItemIsEnabled | 
		           Qt::ItemIsEditable);
	item->setData(1, Qt::UserRole, 0);
}

void XWTeXFormatWidget::saveItem(int i)
{
	QTreeWidgetItem * item = topLevelItem(i);
	QString str = item->data(1, Qt::EditRole).toString();
	switch (i)
	{
		case 0:
			setting.setDescription(str);
			break;
				
		case 1:
			setting.setCompiler(str);
			break;
				
		case 2:
			setting.setInput(str);
			break;
			
		case 3:
			setting.setPreload(str);
			break;
				
		case 4:
			setting.setArguments(str);
			break;
				
		case 5:
			setting.setAttributes(str);
			break;
				
		default:
			break;
	}
}

void XWTeXFormatWidget::saveItem(int i, const QString & key)
{
	QTreeWidgetItem * item = topLevelItem(i);
	int iv = item->data(1, Qt::EditRole).toInt();
	setting.setBound(key, iv);
}

void XWTeXFormatWidget::setDefault()
{
	setValue(0, "");
	setValue(1, "");
	setValue(2, "");
	setValue(3, "");
	setValue(4, "");
	setValue(5, "");
	
	setValue(6, 2000000);
	setValue(7, 0);
	setValue(8, 0);
	setValue(9, 300000);
	setValue(10, 200000);
	setValue(11, 5000);
	setValue(12, 100000);
	setValue(13, 100);
	setValue(14, 20000);
	setValue(15, 659);
	setValue(16, 20000);
	setValue(17, 50);
	setValue(18, 15);
	setValue(19, 60);
	setValue(20, 4000);
	setValue(21, 300);
	setValue(22, 79);
	setValue(23, 50);
	setValue(24, 79);
	setValue(25, 1000);
	setValue(26, 1000);
	setValue(27, 1000);
}

void XWTeXFormatWidget::setup()
{
	QStringList labels;
	labels << tr("Parameter") << tr("Value");	
	setHeaderLabels(labels);
	
	createItem(tr("Description")); //0
	createItem(tr("Compiler")); //1
	createItem(tr("Input file name")); //2
	createItem(tr("Preload format")); //3
	createItem(tr("Arguments")); //4
	createItem(tr("Attributes")); //5
	
	createItem(tr("Main memory"), 2000000, 16000000); //6
	createItem(tr("Extra memory top"), 0, 16000000); //7
	createItem(tr("Extra memory bottom"), 0, 16000000); //8
	createItem(tr("Pool size"), 32000, 10000000); //9
	createItem(tr("String vacancies"), 8000, 9977000); //10
	createItem(tr("Pool free"), 1000, 10000000); //11
	createItem(tr("Max strings"), 100000, 200000); //12
	createItem(tr("Strings free"), 100, 200000); //13
	createItem(tr("Trie size"), 80000, 262143); //14
	createItem(tr("Hyph size"), 610, 65535); //15
	createItem(tr("Buf size"), 500, 200000); //16
	createItem(tr("Nest size"), 40, 400); //17
	createItem(tr("Max in open"), 6, 127); //18
	createItem(tr("Param size"), 60, 600); //19
	createItem(tr("Save size"), 600, 40000); //20
	createItem(tr("Stack size"), 200, 3000); //21
	createItem(tr("Error line"), 50, 255); //22
	createItem(tr("Half error line"), 31, 240); //23
	createItem(tr("Max print line"), 61, 255); //24
	createItem(tr("Ocp list size"), 500, 10000); //25
	createItem(tr("Ocp buf size"), 500, 10000); //26
	createItem(tr("Ocp stack size"), 500, 10000); //27
}

void XWTeXFormatWidget::setValue(int i, int v)
{
	QTreeWidgetItem * item = topLevelItem(i);
	QString str = QString("%1").arg(v);
	item->setText(1, str);
	item->setData(1, Qt::EditRole, v);
}

void XWTeXFormatWidget::setValue(int i, const QString & v)
{
	QTreeWidgetItem * item = topLevelItem(i);
	item->setText(1, v);
	item->setData(1, Qt::EditRole, v);
}

