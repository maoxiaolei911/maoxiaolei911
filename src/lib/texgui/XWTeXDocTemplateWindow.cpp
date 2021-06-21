/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTextCodec>
#include <QTextStream>
#include "XWApplication.h"
#include "XWTeXHighlighter.h"
#include "XWTeXDocTemplateWindow.h"

XWTeXDocTemplateWindow::XWTeXDocTemplateWindow(QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("New File"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  fileNameLabel = new QLabel(tr("File name:"));
  fileNameEdit = new QLineEdit;
  browseButton = new QPushButton(tr("Browse"));
  connect(browseButton, SIGNAL(clicked()), this, SLOT(saveAs()));
  QHBoxLayout * namelayout = new QHBoxLayout;
  namelayout->addWidget(fileNameLabel);
  namelayout->addWidget(fileNameEdit, 1);
  namelayout->addWidget(browseButton);

  templatesList = new QListWidget;
  QGroupBox * tmpbox = new QGroupBox(tr("Templates"));
  QVBoxLayout *tmplayout = new QVBoxLayout;
  tmplayout->addWidget(templatesList);
  tmpbox->setLayout(tmplayout);

  tmpbrowser = new QPlainTextEdit;
  tmpbrowser->setReadOnly(true);
  tmpbrowser->setWordWrapMode(QTextOption::WordWrap);

	if (xwApp->hasSetting("TextBackgroundColor"))
	{
		QVariant variant = xwApp->getSetting("TextBackgroundColor");
		QColor color = variant.value<QColor>();
		QPalette p = tmpbrowser->viewport()->palette();
		p.setColor(QPalette::Base, color);
		p.setColor(QPalette::HighlightedText, color);

		variant = xwApp->getSetting("TextColor");
		color = variant.value<QColor>();
		p.setColor(QPalette::Text, color);
		p.setColor(QPalette::Highlight, color);

		tmpbrowser->viewport()->setPalette(p);
		tmpbrowser->setPalette(p);

		QString fam = xwApp->getSetting("TextFontFamily").toString();
		int weight = xwApp->getSetting("TextFontWeight").toInt();
		bool italic = xwApp->getSetting("TextFontItalic").toBool();
		int size = xwApp->getSetting("TextFontSize").toInt();
		if (size < 0 || size > 72)
		  size = 10;

		QFont font = tmpbrowser->document()->defaultFont();
		font.setFamily(fam);
		font.setWeight(weight);
		font.setItalic(italic);
		font.setPointSize(size);
		tmpbrowser->document()->setDefaultFont(font);
	}

  QGroupBox * bbox = new QGroupBox(tr("Template browser"));
  QVBoxLayout *blayout = new QVBoxLayout;
  blayout->addWidget(tmpbrowser);
  bbox->setLayout(blayout);

  QVBoxLayout *leftlayout = new QVBoxLayout;
  leftlayout->addLayout(namelayout);
  leftlayout->addWidget(tmpbox);

  QHBoxLayout * toplayout = new QHBoxLayout;
  toplayout->addLayout(leftlayout);
  toplayout->addWidget(bbox);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(toplayout);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

	highlighter = new XWTeXHighlighter(this);
	highlighter->setDocument(tmpbrowser->document());

  findTemplates();
  if (templatesList->count() > 0)
  {
  	templatesList->setCurrentRow(0);
  	setTemplate(0);
  }
  connect(templatesList, SIGNAL(currentRowChanged(int)), this, SLOT(setTemplate(int)));

  resize(800, 500);
}

QString XWTeXDocTemplateWindow::getFileName()
{
	QString filename = fileNameEdit->text();
	return filename;
}

void XWTeXDocTemplateWindow::accept()
{
	QString filename = fileNameEdit->text();
	if (!filename.isEmpty())
	{
		QListWidgetItem * item = templatesList->currentItem();
		if (!item)
			item = templatesList->item(0);

		QString tname = item->text();
		if (!tname.isEmpty())
		{
			if (QFile::exists(filename))
				QFile::remove(filename);

			QFile::copy(tname, filename);
		}
	}

	QDialog::accept();
}

void XWTeXDocTemplateWindow::saveAs()
{
	QString path = QCoreApplication::applicationDirPath();
	int idx = path.lastIndexOf(QChar('/'));
	if (idx > 0)
		path = path.left(idx + 1);

	if (path[path.length() - 1] != QChar('/'))
		path += QChar('/');

	path += "doc/tex";
	QDir d(path);
	if (!d.exists())
		d.mkdir(path);

	QString files = tr("Files (*.tex)");
	QString filename = QFileDialog::getSaveFileName(this, tr("Save as"), path, files);
	if (filename.isEmpty())
		return ;

	fileNameEdit->setText(filename);
}

void XWTeXDocTemplateWindow::setTemplate(int row)
{
	QListWidgetItem * item = templatesList->item(row);
	if (item)
	{
		QString tname = item->text();
		if (!tname.isEmpty())
		{
			QFile file(tname);
			if (file.open(QFile::ReadOnly | QFile::Text))
			{
				char buf[1025];
				qint64 len = file.read(buf, 1024);
				file.seek(0);
				QTextCodec * codec = 0;
				if (len > 2)
				{
					if ((buf[0] == 0x5c || buf[0] == 0x25) && buf[1] == 0)
						codec = QTextCodec::codecForName("UTF-16LE");
					else if (buf[0] == 0 && (buf[0] == 0x5c || buf[0] == 0x25))
						codec = QTextCodec::codecForName("UTF-16BE");
					else
					{
						QByteArray ba = QByteArray::fromRawData(buf, len);
						if (buf[0] == 0x25)
						{
							int l = ba.indexOf("\n");
							if (l > 1)
							{
								ba = ba.left(l);
								if (ba.contains("utf-8") || ba.contains("UTF-8"))
									codec = QTextCodec::codecForName("UTF-8");
								else if (ba.contains("gb18030") || ba.contains("GB18030"))
									codec = QTextCodec::codecForName("GB18030-0");
								else if (ba.contains("gbk") || ba.contains("GBK"))
									codec = QTextCodec::codecForName("GBK");
								else if (ba.contains("gb") || ba.contains("GB"))
									codec = QTextCodec::codecForName("GB");
								else if (ba.contains("big5") || ba.contains("Big5") || ba.contains("BIG5"))
									codec = QTextCodec::codecForName("Big5");
								else if (ba.contains("EUC-JP") || ba.contains("euc-jp"))
									codec = QTextCodec::codecForName("EUC-JP");
								else if (ba.contains("EUC-KR") || ba.contains("euc-kr"))
									codec = QTextCodec::codecForName("EUC-KR");
								else if (ba.contains("sjis") || ba.contains("SJIS"))
									codec = QTextCodec::codecForName("Shift-JIS");
							}
						}

						if (!codec)
						{
							if (ba.contains("\\begin{CJK}") || ba.contains("\\begin{CJK*}"))
							{
								if (ba.contains("{GBK}"))
									codec = QTextCodec::codecForName("GBK");
								else if (ba.contains("{GB}"))
									codec = QTextCodec::codecForName("GB");
								else if (ba.contains("{Bg5}"))
									codec = QTextCodec::codecForName("Big5");
								else if (ba.contains("{EUC-JP}"))
									codec = QTextCodec::codecForName("EUC-JP");
								else if (ba.contains("{KS}"))
									codec = QTextCodec::codecForName("EUC-KR");
								else if (ba.contains("{SJIS}"))
									codec = QTextCodec::codecForName("Shift-JIS");
							}
						}
					}
				}

				if (!codec)
					codec = QTextCodec::codecForLocale();

				QTextStream stream(&file);
				stream.setCodec(codec);
				QApplication::setOverrideCursor(Qt::WaitCursor);
			  tmpbrowser->setPlainText(stream.readAll());
			  QApplication::restoreOverrideCursor();
			  file.close();
			}
		}
	}
}

void XWTeXDocTemplateWindow::findTemplates()
{
	QString path = QCoreApplication::applicationDirPath();
	int idx = path.lastIndexOf(QChar('/'));
	if (idx > 0)
		path = path.left(idx + 1);

	if (path[path.length() - 1] != QChar('/'))
		path += QChar('/');

	path += "template";
	QString curDir = "";
	QDir d(path);
	if (!d.exists())
		d.mkdir(path);

	travers(path, curDir);
	if (templatesList->count() > 0)
		templatesList->setCurrentRow(0);
}

void XWTeXDocTemplateWindow::travers(const QString & parentDir, const QString & curDir)
{
	QString path = parentDir;
  if (curDir.length() > 0)
      path = path + curDir;
  QDir dir(path);

  QStringList files = dir.entryList(QDir::Files);
  for (int i = 0; i < files.size(); i++)
  {
     QString name = files.at(i);
     QString p = QString("%1/%2").arg(path).arg(name);
     templatesList->addItem(p);
  }

  QStringList dirs = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
  for (int i = 0; i < dirs.size(); i++)
  {
     QString cur = curDir + "/" + dirs.at(i);
     travers(parentDir, cur);
  }
}
