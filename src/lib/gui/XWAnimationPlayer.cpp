/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWAnimationPlayer.h"

XWAnimationPlayer::XWAnimationPlayer(QWidget * parent)
	:QWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint)
{
	setBackgroundRole(QPalette::Dark);
	//    setWindowTitle(tr("Animation Player"));
	setWindowFlags(Qt::Popup);
	setWindowOpacity(0.5);
    resize(400, 400);
	
	movie = new QMovie(this);
	movie->setCacheMode(QMovie::CacheAll);
	movieLabel = new QLabel(tr("No movie loaded"));
	movieLabel->setAlignment(Qt::AlignCenter);
	movieLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	movieLabel->setBackgroundRole(QPalette::Dark);
	movieLabel->setAutoFillBackground(true);
	movieLabel->setScaledContents(true);
	
	extension = new QWidget;
	
	createControls();
	
	connect(movie, SIGNAL(frameChanged(int)), this, SLOT(updateFrameSlider()));
    connect(movie, SIGNAL(stateChanged(QMovie::MovieState)),
            this, SLOT(updateButtons()));
    connect(frameSlider, SIGNAL(valueChanged(int)), this, SLOT(goToFrame(int)));
    connect(speedSpinBox, SIGNAL(valueChanged(int)),
             movie, SLOT(setSpeed(int)));
             
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(movieLabel);
    mainLayout->addWidget(extension);
    setLayout(mainLayout);

    updateFrameSlider();
    updateButtons();
}

void XWAnimationPlayer::setFileName(const QString &fileName)
{
	movie->stop();
	movieLabel->setMovie(movie);
	movie->setFileName(fileName);
}

void XWAnimationPlayer::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) 
	{
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void XWAnimationPlayer::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) 
	{
		if (movie->isValid() && movie->frameCount() != 1
                            && movie->state() == QMovie::NotRunning)
        {
        	movie->start();
        }
		else if (movie->state() == QMovie::Paused)
			movie->setPaused(false);
		else if (movie->state() == QMovie::Running)
			movie->setPaused(true);
			
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void XWAnimationPlayer::showEvent(QShowEvent *)
{
	if (movie->isValid())
	{
		movie->start();
		updateFrameSlider();
		updateButtons();
	}
}

void XWAnimationPlayer::goToFrame(int frame)
{
	movie->jumpToFrame(frame);
}

void XWAnimationPlayer::updateButtons()
{
	playButton->setEnabled(movie->isValid() && movie->frameCount() != 1
                            && movie->state() == QMovie::NotRunning);
    pauseButton->setEnabled(movie->state() != QMovie::NotRunning);
    pauseButton->setChecked(movie->state() == QMovie::Paused);
    stopButton->setEnabled(movie->state() != QMovie::NotRunning);
    if (movie->state() == QMovie::Running || movie->state() == QMovie::Paused)
    	extension->hide();
    else
    	extension->setVisible(true);
}

void XWAnimationPlayer::updateFrameSlider()
{
	bool hasFrames = (movie->currentFrameNumber() >= 0);
	if (hasFrames) 
	{
        if (movie->frameCount() > 0) 
            frameSlider->setMaximum(movie->frameCount() - 1);
        else 
        {
            if (movie->currentFrameNumber() > frameSlider->maximum())
                frameSlider->setMaximum(movie->currentFrameNumber());
        }
        frameSlider->setValue(movie->currentFrameNumber());
    } 
    else 
        frameSlider->setMaximum(0);
    frameLabel->setEnabled(hasFrames);
    frameSlider->setEnabled(hasFrames);
}

void XWAnimationPlayer::createControls()
{
	frameLabel = new QLabel(tr("Current frame:"));
	frameSlider = new QSlider(Qt::Horizontal);
	frameSlider->setTickPosition(QSlider::TicksBelow);
	frameSlider->setTickInterval(10);
	speedLabel = new QLabel(tr("Speed:"));
	speedSpinBox = new QSpinBox;
    speedSpinBox->setRange(1, 9999);
    speedSpinBox->setValue(100);
    speedSpinBox->setSuffix(tr("%"));

    QGridLayout * controlsLayout = new QGridLayout;
    controlsLayout->addWidget(frameLabel, 0, 0);
    controlsLayout->addWidget(frameSlider, 0, 1, 1, 2);
    controlsLayout->addWidget(speedLabel, 1, 0);
    controlsLayout->addWidget(speedSpinBox, 1, 1);
    
    QSize iconSize(36, 36);
    playButton = new QToolButton;
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    playButton->setIconSize(iconSize);
    playButton->setToolTip(tr("Play"));
    connect(playButton, SIGNAL(clicked()), movie, SLOT(start()));
    
    pauseButton = new QToolButton;
    pauseButton->setCheckable(true);
    pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    pauseButton->setIconSize(iconSize);
    pauseButton->setToolTip(tr("Pause"));
    connect(pauseButton, SIGNAL(clicked(bool)), movie, SLOT(setPaused(bool)));

    stopButton = new QToolButton;
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stopButton->setIconSize(iconSize);
    stopButton->setToolTip(tr("Stop"));
    connect(stopButton, SIGNAL(clicked()), movie, SLOT(stop()));

    quitButton = new QToolButton;
    quitButton->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    quitButton->setIconSize(iconSize);
    quitButton->setToolTip(tr("Quit"));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout * buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(playButton);
    buttonsLayout->addWidget(pauseButton);
    buttonsLayout->addWidget(stopButton);
    buttonsLayout->addWidget(quitButton);
    buttonsLayout->addStretch();
    
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(controlsLayout);
    layout->addLayout(buttonsLayout);
    extension->setLayout(layout);
}

