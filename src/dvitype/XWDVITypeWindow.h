/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWDVITYPEWINDOW_H
#define XWDVITYPEWINDOW_H

#include <QPlainTextEdit>
#include <QObject>
#include <QString>

class XWDVITypeWindow : public QPlainTextEdit
{
	Q_OBJECT
	
public:
	XWDVITypeWindow(QWidget * parent = 0);
	
	QString getFileName() {return fullFileName;}
	
	void loadFile(const QString & filename);
	
private:
	QString fullFileName;
};

#endif //XWDVITYPEWINDOW_H
