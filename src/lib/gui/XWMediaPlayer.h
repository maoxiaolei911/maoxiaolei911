/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWMEDIAPLAYER_H
#define XWMEDIAPLAYER_H

#include <QtGui/QWidget>
#include <QtGui/QApplication>
#include <QtCore/QTimerEvent>
#include <QtGui/QShowEvent>
#include <QtGui/QIcon>
#include <QtCore/QBasicTimer>
#include <QtGui/QAction>
#include <QDialog>

#include <phonon/audiooutput.h>
#include <phonon/backendcapabilities.h>
#include <phonon/effect.h>
#include <phonon/effectparameter.h>
#include <phonon/effectwidget.h>
#include <phonon/mediaobject.h>
#include <phonon/seekslider.h>
#include <phonon/videowidget.h>
#include <phonon/volumeslider.h>

class QPushButton;
class QLabel;
class QSlider;
class QTextEdit;
class QComboBox;
class QMenu;

class QPushButton;
class QSlider;
 
class XWMediaPlayer;

class XWVideoWidget : public Phonon::VideoWidget
{
	Q_OBJECT
	
public:
	XWVideoWidget(XWMediaPlayer *playerA, QWidget *parent = 0);
	
public slots:
	void setFullScreen(bool);
	
signals:
	void fullScreenChanged(bool);
	
protected:    
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
	
    bool event(QEvent *e);
    
    void keyPressEvent(QKeyEvent *e);
    
	void mouseDoubleClickEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *event);
    
    void timerEvent(QTimerEvent *e);
    
private:
    XWMediaPlayer *m_player;
    QBasicTimer m_timer;
    QAction m_action;
};

class XWMediaPlayer : public QWidget
{
	Q_OBJECT
	
public:
	XWMediaPlayer(QWidget *parent = 0);
	~XWMediaPlayer();
	
	void dragEnterEvent(QDragEnterEvent *e);
	void dragMoveEvent(QDragMoveEvent *e);
	void dropEvent(QDropEvent *e);
	
	void handleDrop(QDropEvent *e);
	
	void initSettingsDialog();
	void initVideoWindow();
	
	void setFile(const QString &fileName);
	void setLocation(const QString &location);
	
public slots:	
	void aspectChanged(QAction *);
	void finished();
	void forward();
	
	void playPause();
	
	void rewind();
	
	void scaleChanged(QAction *);
	
	void updateInfo();
	void updateTime();
	
protected slots:
	void showEvent(QShowEvent *);
	
private slots:
	void bufferStatus(int percent);
	
	void configureEffect();
	
	void effectChanged(int currentIndex);
	
	void hasVideoChanged(bool);
	
	void setAspect(int);
	void setBrightness(int);
	void setContrast(int);
	void setHue(int);
	void setSaturation(int);
	void setScale(int);
	void showContextMenu(const QPoint &p);
	void showSettingsDialog();
	void stateChanged(Phonon::State newstate, Phonon::State oldstate);
			
private:
	bool playPauseForDialog();
	
private:
	QWidget * extension;	
	XWVideoWidget *m_videoWidget;
	
	QDialog *settingsDialog;
	
	QIcon playIcon;
	QIcon pauseIcon;
	QPushButton *playButton;
	QPushButton *rewindButton;
	QPushButton *forwardButton;
	Phonon::SeekSlider *slider;
	QLabel *timeLabel;
	QLabel *progressLabel;
	Phonon::VolumeSlider *volume;
	QSlider *m_hueSlider;
	QSlider *m_satSlider;
	QSlider *m_contSlider;
	QLabel *info;
   	Phonon::Effect *nextEffect;
    QAction *m_fullScreenAction;
     
    QWidget m_videoWindow;
    Phonon::MediaObject m_MediaObject;
    Phonon::AudioOutput m_AudioOutput;
    Phonon::Path m_audioOutputPath;
    
    const bool m_hasSmallScreen;
    	
    QComboBox * deviceCombo;
    QComboBox * audioEffectsCombo;
    
    QPushButton * effectButton;
};

#endif // XWMEDIAPLAYER_H

