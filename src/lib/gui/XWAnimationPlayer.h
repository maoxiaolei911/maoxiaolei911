/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWANIMATIONPLAYER_H
#define XWANIMATIONPLAYER_H

#include <QWidget>

class QLabel;
class QMovie;
class QSlider;
class QSpinBox;
class QToolButton;

class XWAnimationPlayer : public QWidget
{
	Q_OBJECT
	
public:       
    XWAnimationPlayer(QWidget * parent = 0);
        
    void setFileName(const QString &fileName);
    
private slots:	
	void goToFrame(int frame);
	
	void updateButtons();
	void updateFrameSlider();
	
protected:
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void showEvent(QShowEvent *);
    
private:
	void createControls();
	
private:
    QLabel *movieLabel;
    QMovie *movie;
    
    QWidget *extension;
    
    QToolButton *playButton;
    QToolButton *pauseButton;
    QToolButton *stopButton;
    QToolButton *quitButton;
    QSlider *frameSlider;
    QSpinBox *speedSpinBox;
    QLabel *frameLabel;
    QLabel *speedLabel;
    
    QPoint dragPosition;
};

#endif // XWANIMATIONPLAYER_H

