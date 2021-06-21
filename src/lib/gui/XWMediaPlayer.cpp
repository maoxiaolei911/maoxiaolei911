/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWMediaPlayer.h"

#define SLIDER_RANGE 8

XWVideoWidget::XWVideoWidget(XWMediaPlayer *playerA, QWidget *parent)
	: Phonon::VideoWidget(parent), m_player(playerA), m_action(this)
{
	m_action.setCheckable(true);
    m_action.setChecked(false);
    m_action.setShortcut(QKeySequence( Qt::AltModifier + Qt::Key_Return));
    m_action.setShortcutContext(Qt::WindowShortcut);
    connect(&m_action, SIGNAL(toggled(bool)), SLOT(setFullScreen(bool)));
    addAction(&m_action);
    setAcceptDrops(false);
}

void XWVideoWidget::setFullScreen(bool enabled)
{
	Phonon::VideoWidget::setFullScreen(enabled);
    emit fullScreenChanged(enabled);
}

void XWVideoWidget::dragEnterEvent(QDragEnterEvent *e)
{
	if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void XWVideoWidget::dropEvent(QDropEvent *e)
{
	m_player->handleDrop(e);
}

bool XWVideoWidget::event(QEvent *e)
{
	switch(e->type())
	{
		case QEvent::Close:
			e->ignore();
         	return true;
         	
        case QEvent::MouseMove:
        case QEvent::WindowStateChange:
        	{
        		m_action.setChecked(windowState() & Qt::WindowFullScreen);
        		const Qt::WindowFlags flags = m_player->windowFlags();
        		if (windowState() & Qt::WindowFullScreen)
        			m_timer.start(1000, this);
        		else
        			m_timer.stop();
        	}
        	break;
        	
        default:
        	break;
	}
	
	return Phonon::VideoWidget::event(e);
}

void XWVideoWidget::keyPressEvent(QKeyEvent *e)
{
	if(!e->modifiers())
	{
		if (e->key() == Qt::Key_Space || e->key() == Qt::Key_0)
		{
			m_player->playPause();
			e->accept();
			return;
		}
		else if (e->key() == Qt::Key_Escape || e->key() == Qt::Key_Backspace)
		{
			setFullScreen(false);
			e->accept();
			return;
		}
	}
	
	Phonon::VideoWidget::keyPressEvent(e);
}

void XWVideoWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	Phonon::VideoWidget::mouseDoubleClickEvent(e);
	setFullScreen(!isFullScreen());
}

void XWVideoWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) 
	{
		m_player->playPause();
		event->accept();
	}
}

void XWVideoWidget::timerEvent(QTimerEvent *e)
{
	Phonon::VideoWidget::timerEvent(e);
}

XWMediaPlayer::XWMediaPlayer(QWidget *parent)
	:QWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint),
	 playButton(0), 
	 nextEffect(0), 
	 settingsDialog(0),
	 m_AudioOutput(Phonon::VideoCategory),
	 m_videoWidget(new XWVideoWidget(this)),
	 m_hasSmallScreen(false)
{
	setBackgroundRole(QPalette::Dark);
	setWindowFlags(Qt::Popup);
	setWindowOpacity(0.5);
		
	extension = new QWidget;
	
	playButton = 0; 
	nextEffect = 0;
	settingsDialog = 0;
	
	setContextMenuPolicy(Qt::CustomContextMenu);
	m_videoWidget->setContextMenuPolicy(Qt::CustomContextMenu);
		
	QSize buttonSize(34, 28);
	rewindButton = new QPushButton(extension);
    rewindButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    
    forwardButton = new QPushButton(extension);
    forwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    forwardButton->setEnabled(false);
    
    playButton = new QPushButton(extension);
    playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
    pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
    playButton->setIcon(playIcon);
    
    rewindButton->setMinimumSize(buttonSize);
    forwardButton->setMinimumSize(buttonSize);
    playButton->setMinimumSize(buttonSize);
    
    slider = new Phonon::SeekSlider(extension);
    slider->setMediaObject(&m_MediaObject);
    volume = new Phonon::VolumeSlider(extension);
    volume->setFixedWidth(120);
    
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(rewindButton);
    layout->addWidget(playButton);
    layout->addWidget(forwardButton);

    layout->addStretch();
    layout->addWidget(volume);
    
    timeLabel = new QLabel(this);
    progressLabel = new QLabel(this);
    QWidget *sliderPanel = new QWidget(this);
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    sliderLayout->addWidget(slider);
    sliderLayout->addWidget(timeLabel);
    sliderLayout->addWidget(progressLabel);
    sliderLayout->setContentsMargins(0, 0, 0, 0);
    sliderPanel->setLayout(sliderLayout);
    
    QVBoxLayout *buttonPanelLayout = new QVBoxLayout();
    buttonPanelLayout->addLayout(layout);
    buttonPanelLayout->addWidget(sliderPanel);
    buttonPanelLayout->setContentsMargins(0, 0, 0, 0);
    
    extension->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    extension->setLayout(buttonPanelLayout);
    
    info = new QLabel(this);
    info->setMinimumHeight(70);
    info->setAcceptDrops(false);
    info->setMargin(2);
    info->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    info->setLineWidth(2);
    info->setAutoFillBackground(true);
    QPalette palette(Qt::black);
	info->setPalette(palette);
	
	initVideoWindow();
		
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(8, 8, 8, 8);
	vLayout->addWidget(info);
    vLayout->addWidget(&m_videoWindow);    
    vLayout->addWidget(extension);
    setLayout(vLayout);
    
    connect(rewindButton, SIGNAL(clicked()), this, SLOT(rewind()));
    connect(playButton, SIGNAL(clicked()), this, SLOT(playPause()));
    connect(forwardButton, SIGNAL(clicked()), this, SLOT(forward()));
    
    connect(m_videoWidget, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));
    
    connect(&m_MediaObject, SIGNAL(metaDataChanged()), this, SLOT(updateInfo()));
    connect(&m_MediaObject, SIGNAL(totalTimeChanged(qint64)), this, SLOT(updateTime()));
    connect(&m_MediaObject, SIGNAL(tick(qint64)), this, SLOT(updateTime()));
    connect(&m_MediaObject, SIGNAL(finished()), this, SLOT(finished()));
    connect(&m_MediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(stateChanged(Phonon::State,Phonon::State)));
    connect(&m_MediaObject, SIGNAL(bufferStatus(int)), this, SLOT(bufferStatus(int)));
    connect(&m_MediaObject, SIGNAL(hasVideoChanged(bool)), this, SLOT(hasVideoChanged(bool)));

    rewindButton->setEnabled(false);
    playButton->setEnabled(false);
    setAcceptDrops(true);

    m_audioOutputPath = Phonon::createPath(&m_MediaObject, &m_AudioOutput);
    Phonon::createPath(&m_MediaObject, m_videoWidget);
}

XWMediaPlayer::~XWMediaPlayer()
{
}

void XWMediaPlayer::dragEnterEvent(QDragEnterEvent *e)
{
	dragMoveEvent(e);
}

void XWMediaPlayer::dragMoveEvent(QDragMoveEvent *e)
{
	if (e->mimeData()->hasUrls()) 
	{
        if (e->proposedAction() == Qt::CopyAction || e->proposedAction() == Qt::MoveAction)
        {
            e->acceptProposedAction();
        }
    }
}

void XWMediaPlayer::dropEvent(QDropEvent *e)
{
	if (e->mimeData()->hasUrls() && e->proposedAction() != Qt::LinkAction) 
	{
        e->acceptProposedAction();
        handleDrop(e);
    } 
    else 
    {
        e->ignore();
    }
}

void XWMediaPlayer::handleDrop(QDropEvent *e)
{
	QList<QUrl> urls = e->mimeData()->urls();
    if (e->proposedAction() == Qt::MoveAction)
    {
         // Just add to the queue:
        for (int i=0; i<urls.size(); i++)
            m_MediaObject.enqueue(Phonon::MediaSource(urls[i].toLocalFile()));
    } 
    else 
    {
        // Create new queue:
        m_MediaObject.clearQueue();
        if (urls.size() > 0) 
        {
            QString fileName = urls[0].toLocalFile();
            QDir dir(fileName);
            if (dir.exists()) 
            {
                dir.setFilter(QDir::Files);
                QStringList entries = dir.entryList();
                if (entries.size() > 0) 
                {
                    setFile(fileName + QDir::separator() +  entries[0]);
                    for (int i=1; i< entries.size(); ++i)
                        m_MediaObject.enqueue(fileName + QDir::separator() + entries[i]);
                }
            } 
            else 
            {
                setFile(fileName);
                for (int i=1; i<urls.size(); i++)
                    m_MediaObject.enqueue(Phonon::MediaSource(urls[i].toLocalFile()));
            }
        }
    }
    forwardButton->setEnabled(m_MediaObject.queue().size() > 0);
    m_MediaObject.play();
}

void XWMediaPlayer::initVideoWindow()
{
	QVBoxLayout *videoLayout = new QVBoxLayout();
    videoLayout->addWidget(m_videoWidget);
    videoLayout->setContentsMargins(0, 0, 0, 0);
    m_videoWindow.setLayout(videoLayout);
    m_videoWindow.setMinimumSize(100, 100);
}

void XWMediaPlayer::setFile(const QString &fileName)
{
	setWindowTitle(fileName.right(fileName.length() - fileName.lastIndexOf('/') - 1));
    m_MediaObject.setCurrentSource(Phonon::MediaSource(fileName));
}

void XWMediaPlayer::setLocation(const QString &location)
{
	setWindowTitle(location.right(location.length() - location.lastIndexOf('/') - 1));
    m_MediaObject.setCurrentSource(Phonon::MediaSource(QUrl::fromEncoded(location.toUtf8())));
    m_MediaObject.play();
}

void XWMediaPlayer::aspectChanged(QAction *act)
{
	if (act->text() == tr("16/9"))
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio16_9);
	else if (act->text() == tr("Scale"))
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
	else if (act->text() == tr("4/3"))
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio4_3);
	else
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
}

void XWMediaPlayer::finished()
{
	if (!(extension->isVisible()))
		extension->show();
}

void XWMediaPlayer::scaleChanged(QAction *act)
{
	if (act->text() == tr("Scale and crop"))
		m_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleAndCrop);
	else
		m_videoWidget->setScaleMode(Phonon::VideoWidget::FitInView);
}

void XWMediaPlayer::bufferStatus(int percent)
{
	if (percent == 0 || percent == 100)
         progressLabel->setText(QString());
    else 
    {
        QString str = QString::fromLatin1("(%1%)").arg(percent);
        progressLabel->setText(str);
    }
}

void XWMediaPlayer::configureEffect()
{
	if (!nextEffect)
         return;

    QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
    const QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
    if (audioEffectsCombo->currentIndex() > 0) 
    {
        Phonon::EffectDescription chosenEffect = availableEffects[audioEffectsCombo->currentIndex() - 1];

        QDialog effectDialog;
        effectDialog.setWindowTitle(tr("Configure effect"));
        QVBoxLayout *topLayout = new QVBoxLayout(&effectDialog);

        QLabel *description = new QLabel("<b>Description:</b><br>" + chosenEffect.description(), &effectDialog);
        description->setWordWrap(true);
        topLayout->addWidget(description);

        QScrollArea *scrollArea = new QScrollArea(&effectDialog);
        topLayout->addWidget(scrollArea);

        QVariantList savedParamValues;
        foreach(Phonon::EffectParameter param, nextEffect->parameters()) 
        {
            savedParamValues << nextEffect->parameterValue(param);
        }

        QWidget *scrollWidget = new Phonon::EffectWidget(nextEffect);
        scrollWidget->setMinimumWidth(320);
        scrollWidget->setContentsMargins(10, 10, 10,10);
        scrollArea->setWidget(scrollWidget);

        QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &effectDialog);
        connect(bbox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), &effectDialog, SLOT(accept()));
        connect(bbox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), &effectDialog, SLOT(reject()));
        topLayout->addWidget(bbox);

        effectDialog.exec();

        if (effectDialog.result() != QDialog::Accepted) 
        {
            //we need to restore the paramaters values
            int currentIndex = 0;
            foreach(Phonon::EffectParameter param, nextEffect->parameters()) 
            {
                nextEffect->setParameterValue(param, savedParamValues.at(currentIndex++));
            }

        }
    }
}

void XWMediaPlayer::effectChanged(int currentIndex)
{
	if (currentIndex)
	{
		QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
        Phonon::EffectDescription chosenEffect = availableEffects[currentIndex - 1];

        QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
        Phonon::Effect *currentEffect = currEffects.size() ? currEffects[0] : 0;
        if (nextEffect && !(currentEffect && (currentEffect->description().name() == nextEffect->description().name())))
             delete nextEffect;

         nextEffect = new Phonon::Effect(chosenEffect);
     }
     
     effectButton->setEnabled(currentIndex);
}

void XWMediaPlayer::forward()
{
	QList<Phonon::MediaSource> queue = m_MediaObject.queue();
    if (queue.size() > 0) 
    {
        m_MediaObject.setCurrentSource(queue[0]);
        forwardButton->setEnabled(queue.size() > 1);
        m_MediaObject.play();
    }
}

void XWMediaPlayer::hasVideoChanged(bool bHasVideo)
{
	info->setVisible(!bHasVideo);
	m_videoWindow.setVisible(bHasVideo);
	m_fullScreenAction->setEnabled(bHasVideo);
}

void XWMediaPlayer::playPause()
{
	if (m_MediaObject.state() == Phonon::PlayingState)
         m_MediaObject.pause();
    else 
    {
        if (m_MediaObject.currentTime() == m_MediaObject.totalTime())
            m_MediaObject.seek(0);
        m_MediaObject.play();
    }
}

void XWMediaPlayer::rewind()
{
	m_MediaObject.seek(0);
}

void XWMediaPlayer::updateInfo()
{
	int maxLength = 30;
    QString font = "<font color=#ffeeaa>";
    QString fontmono = "<font family=\"monospace,courier new\" color=#ffeeaa>";

    QMap <QString, QString> metaData = m_MediaObject.metaData();
    QString trackArtist = metaData.value("ARTIST");
    if (trackArtist.length() > maxLength)
        trackArtist = trackArtist.left(maxLength) + "...";

    QString trackTitle = metaData.value("TITLE");
    int trackBitrate = metaData.value("BITRATE").toInt();

    QString fileName;
    if (m_MediaObject.currentSource().type() == Phonon::MediaSource::Url) 
    {
        fileName = m_MediaObject.currentSource().url().toString();
    } 
    else 
    {
        fileName = m_MediaObject.currentSource().fileName();
        fileName = fileName.right(fileName.length() - fileName.lastIndexOf('/') - 1);
        if (fileName.length() > maxLength)
            fileName = fileName.left(maxLength) + "...";
    }

    QString title;
    if (!trackTitle.isEmpty()) 
    {
        if (trackTitle.length() > maxLength)
            trackTitle = trackTitle.left(maxLength) + "...";
        title = "Title: " + font + trackTitle + "<br></font>";
    } 
    else if (!fileName.isEmpty()) 
    {
        if (fileName.length() > maxLength)
            fileName = fileName.left(maxLength) + "...";
        title = font + fileName + "</font>";
        if (m_MediaObject.currentSource().type() == Phonon::MediaSource::Url) 
        {
            title.prepend("Url: ");
        } 
        else 
        {
            title.prepend("File: ");
        }
    }

    QString artist;
    if (!trackArtist.isEmpty())
        artist = "Artist:  " + font + trackArtist + "</font>";

    QString bitrate;
    if (trackBitrate != 0)
        bitrate = "<br>Bitrate:  " + font + QString::number(trackBitrate/1000) + "kbit</font>";

    info->setText(title + artist + bitrate);
}

void XWMediaPlayer::updateTime()
{
	long len = m_MediaObject.totalTime();
    long pos = m_MediaObject.currentTime();
    QString timeString;
    if (pos || len)
    {
        int sec = pos/1000;
        int min = sec/60;
        int hour = min/60;
        int msec = pos;

        QTime playTime(hour%60, min%60, sec%60, msec%1000);
        sec = len / 1000;
        min = sec / 60;
        hour = min / 60;
        msec = len;

        QTime stopTime(hour%60, min%60, sec%60, msec%1000);
        QString timeFormat = "m:ss";
        if (hour > 0)
            timeFormat = "h:mm:ss";
        timeString = playTime.toString(timeFormat);
        if (len)
            timeString += " / " + stopTime.toString(timeFormat);
    }
    timeLabel->setText(timeString);
}

void XWMediaPlayer::showEvent(QShowEvent *)
{
	if (m_MediaObject.hasVideo())
		m_MediaObject.play();
}

void XWMediaPlayer::setAspect(int val)
{
	m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio(val));
}

void XWMediaPlayer::setBrightness(int val)
{
	m_videoWidget->setBrightness(val / qreal(SLIDER_RANGE));
}

void XWMediaPlayer::setContrast(int val)
{
	m_videoWidget->setContrast(val / qreal(SLIDER_RANGE));
}

void XWMediaPlayer::setHue(int val)
{
	m_videoWidget->setHue(val / qreal(SLIDER_RANGE));
}

void XWMediaPlayer::setSaturation(int val)
{
	m_videoWidget->setSaturation(val / qreal(SLIDER_RANGE));
}

void XWMediaPlayer::setScale(int val)
{
	m_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleMode(val));
}

void XWMediaPlayer::showContextMenu(const QPoint &p)
{
	QMenu menu(this);
	
	QMenu *aspectMenu = menu.addMenu(tr("Aspect ratio"));
	QActionGroup *aspectGroup = new QActionGroup(aspectMenu);
	connect(aspectGroup, SIGNAL(triggered(QAction*)), this, SLOT(aspectChanged(QAction*)));
	aspectGroup->setExclusive(true);
	QAction *aspectActionAuto = aspectMenu->addAction(tr("Auto"));
	aspectActionAuto->setCheckable(true);
	aspectActionAuto->setChecked(true);
	aspectGroup->addAction(aspectActionAuto);
	QAction *aspectActionScale = aspectMenu->addAction(tr("Scale"));
	aspectActionScale->setCheckable(true);
	aspectGroup->addAction(aspectActionScale);
	QAction *aspectAction16_9 = aspectMenu->addAction(tr("16/9"));
	aspectAction16_9->setCheckable(true);
	aspectGroup->addAction(aspectAction16_9);
	QAction *aspectAction4_3 = aspectMenu->addAction(tr("4/3"));
	aspectAction4_3->setCheckable(true);
	aspectGroup->addAction(aspectAction4_3);
	
	QMenu *scaleMenu = menu.addMenu(tr("Scale mode"));
	QActionGroup *scaleGroup = new QActionGroup(scaleMenu);
	connect(scaleGroup, SIGNAL(triggered(QAction*)), this, SLOT(scaleChanged(QAction*)));
	scaleGroup->setExclusive(true);
	QAction *scaleActionFit = scaleMenu->addAction(tr("Fit in view"));
	scaleActionFit->setCheckable(true);
	scaleActionFit->setChecked(true);
	scaleGroup->addAction(scaleActionFit);
	QAction *scaleActionCrop = scaleMenu->addAction(tr("Scale and crop"));
	scaleActionCrop->setCheckable(true);
	scaleGroup->addAction(scaleActionCrop);
	
	menu.addSeparator();
	QAction *settingsAction = menu.addAction(tr("Settings..."));
	connect(settingsAction, SIGNAL(triggered(bool)), this, SLOT(showSettingsDialog()));
	menu.popup(m_videoWidget->isFullScreen() ? p : mapToGlobal(p));
		
	menu.addSeparator();
	QAction *closeAction = menu.addAction(tr("Close"));
	connect(closeAction, SIGNAL(triggered(bool)), this, SLOT(close()));
}

void XWMediaPlayer::showSettingsDialog()
{
	const bool hasPausedForDialog = playPauseForDialog();

    if (!settingsDialog)
        initSettingsDialog();

    float oldBrightness = m_videoWidget->brightness();
    float oldHue = m_videoWidget->hue();
    float oldSaturation = m_videoWidget->saturation();
    float oldContrast = m_videoWidget->contrast();
    Phonon::VideoWidget::AspectRatio oldAspect = m_videoWidget->aspectRatio();
    Phonon::VideoWidget::ScaleMode oldScale = m_videoWidget->scaleMode();
    int currentEffect = audioEffectsCombo->currentIndex();
    settingsDialog->exec();

    if (settingsDialog->result() == QDialog::Accepted)
    {
        QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();
        m_AudioOutput.setOutputDevice(devices[deviceCombo->currentIndex()]);
        QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
        QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();

        if (audioEffectsCombo->currentIndex() > 0)
        {
            Phonon::Effect *currentEffect = currEffects.size() ? currEffects[0] : 0;
            if (!currentEffect || currentEffect->description() != nextEffect->description())
            {
                foreach(Phonon::Effect *effect, currEffects) 
                {
                    m_audioOutputPath.removeEffect(effect);
                    delete effect;
                }
                m_audioOutputPath.insertEffect(nextEffect);
            }
        } 
        else 
        {
            foreach(Phonon::Effect *effect, currEffects) 
            {
                m_audioOutputPath.removeEffect(effect);
                delete effect;
                nextEffect = 0;
            }
        }
    } 
    else 
    {
        // Restore previous settings
        m_videoWidget->setBrightness(oldBrightness);
        m_videoWidget->setSaturation(oldSaturation);
        m_videoWidget->setHue(oldHue);
        m_videoWidget->setContrast(oldContrast);
        m_videoWidget->setAspectRatio(oldAspect);
        m_videoWidget->setScaleMode(oldScale);
        audioEffectsCombo->setCurrentIndex(currentEffect);
    }

    if (hasPausedForDialog)
        m_MediaObject.play();
}

void XWMediaPlayer::stateChanged(Phonon::State newstate, Phonon::State oldstate)
{
	Q_UNUSED(oldstate);

    if (oldstate == Phonon::LoadingState) 
    {
        QRect videoHintRect = QRect(QPoint(0, 0), m_videoWindow.sizeHint());
        QRect newVideoRect = QApplication::desktop()->screenGeometry().intersected(videoHintRect);
        if (!m_hasSmallScreen) 
        {
            if (m_MediaObject.hasVideo()) 
            {
                // Flush event que so that sizeHint takes the
                // recently shown/hidden m_videoWindow into account:
                qApp->processEvents();
                resize(sizeHint());
            } 
            else
                resize(minimumSize());
        }
    }

    switch (newstate) 
    {
        case Phonon::ErrorState:
            if (m_MediaObject.errorType() == Phonon::FatalError) 
            {
                playButton->setEnabled(false);
                rewindButton->setEnabled(false);
            } 
            else 
            {
                m_MediaObject.pause();
            }
            QMessageBox::warning(this, "Phonon Mediaplayer", m_MediaObject.errorString(), QMessageBox::Close);
            break;

        case Phonon::StoppedState:
            m_videoWidget->setFullScreen(false);
            // Fall through
        case Phonon::PausedState:
        	if (!(extension->isVisible()))
        		extension->show();
            playButton->setIcon(playIcon);
            if (m_MediaObject.currentSource().type() != Phonon::MediaSource::Invalid)
            {
                playButton->setEnabled(true);
                rewindButton->setEnabled(true);
            } 
            else 
            {
                playButton->setEnabled(false);
                rewindButton->setEnabled(false);
            }
            break;
            
        case Phonon::PlayingState:
        	if (extension->isVisible())
        		extension->hide();
            playButton->setEnabled(true);
            playButton->setIcon(pauseIcon);
            if (m_MediaObject.hasVideo())
                m_videoWindow.show();
            // Fall through
        case Phonon::BufferingState:
            rewindButton->setEnabled(true);
            break;
            
        case Phonon::LoadingState:
            rewindButton->setEnabled(false);
            break;
    }
}

void XWMediaPlayer::initSettingsDialog()
{
	settingsDialog = new QDialog(this);
	settingsDialog->setWindowTitle(tr("Media options"));
	setWindowIcon(QIcon(":images/xiuwen24.png"));
	QLabel * contrastlabel = new QLabel(tr("Contrast"), settingsDialog);
	QSlider * contrastSlider = new QSlider(settingsDialog);
	connect(contrastSlider , SIGNAL(valueChanged(int)), this, SLOT(setContrast(int)));
	
	QLabel * brightnesslabel = new QLabel(tr("Brightness"), settingsDialog);
	QSlider * brightnessSlider = new QSlider(settingsDialog);
	connect(brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(setBrightness(int)));
	
	QLabel * saturationlabel = new QLabel(tr("Saturation"), settingsDialog);
	QSlider * saturationSlider = new QSlider(settingsDialog);
	connect(saturationSlider, SIGNAL(valueChanged(int)), this, SLOT(setSaturation(int)));
	
	QLabel * huelabel = new QLabel(tr("Hue"), settingsDialog);
	QSlider * hueSlider = new QSlider(settingsDialog);
	connect(hueSlider, SIGNAL(valueChanged(int)), this, SLOT(setHue(int)));
	
	QLabel * aspectlabel = new QLabel(tr("Aspect"), settingsDialog);
	QComboBox * aspectcombo = new QComboBox(settingsDialog);
	aspectcombo->addItem(tr("Auto"));
	aspectcombo->addItem(tr("Stretch"));
	aspectcombo->addItem(tr("4/3"));
	aspectcombo->addItem(tr("16/9"));
	connect(aspectcombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setAspect(int)));
	
	QLabel * scalelabel = new QLabel(tr("Scale"), settingsDialog);
	QComboBox * scalecombo = new QComboBox(settingsDialog);
	scalecombo->addItem(tr("Fit in view"));
	scalecombo->addItem(tr("Scale and crop "));
	connect(scalecombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setScale(int)));
	
	contrastSlider->setValue(int(m_videoWidget->contrast() * SLIDER_RANGE));
	brightnessSlider->setValue(int(m_videoWidget->brightness() * SLIDER_RANGE));
	saturationSlider->setValue(int(m_videoWidget->saturation() * SLIDER_RANGE));
	hueSlider->setValue(int(m_videoWidget->hue() * SLIDER_RANGE));
	aspectcombo->setCurrentIndex(m_videoWidget->aspectRatio());
	scalecombo->setCurrentIndex(m_videoWidget->scaleMode());
	
	QLabel * devicelabel = new QLabel(tr("Audio device"), settingsDialog);
	deviceCombo = new QComboBox(settingsDialog);
	QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();
    for (int i=0; i<devices.size(); i++)
    {
         QString itemText = devices[i].name();
         if (!devices[i].description().isEmpty()) {
             itemText += QString::fromLatin1(" (%1)").arg(devices[i].description());
         }
         deviceCombo->addItem(itemText);
         if (devices[i] == m_AudioOutput.outputDevice())
             deviceCombo->setCurrentIndex(i);
    }
    
    QLabel * effectlabel = new QLabel(tr("Audio effect"), settingsDialog);
	audioEffectsCombo = new QComboBox(settingsDialog);	
	audioEffectsCombo->addItem(tr("<no effect>"));
    QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
    Phonon::Effect *currEffect = currEffects.size() ? currEffects[0] : 0;
    QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
    for (int i=0; i<availableEffects.size(); i++)
    {
         audioEffectsCombo->addItem(availableEffects[i].name());
         if (currEffect && availableEffects[i] == currEffect->description())
             audioEffectsCombo->setCurrentIndex(i+1);
    }
    effectButton = new QPushButton(tr("Setup"), settingsDialog);
    connect(effectButton, SIGNAL(clicked()), this, SLOT(configureEffect()));
    connect(audioEffectsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(effectChanged(int)));
    
    QHBoxLayout *effectlayout = new QHBoxLayout();
    effectlayout->addWidget(effectlabel);
    effectlayout->addWidget(audioEffectsCombo);
    
    QDialogButtonBox *buttonBox  = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), settingsDialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), settingsDialog, SLOT(reject()));
    
    QGridLayout * layout = new QGridLayout;
	layout->addWidget(contrastlabel, 0, 0);
	layout->addWidget(contrastSlider, 0, 1);
	layout->addWidget(brightnesslabel, 1, 0);
	layout->addWidget(brightnessSlider, 1, 1);
	layout->addWidget(saturationlabel, 2, 0);
	layout->addWidget(saturationSlider, 2, 1);
	layout->addWidget(huelabel, 3, 0);
	layout->addWidget(hueSlider, 3, 1);
	layout->addWidget(aspectlabel, 4, 0);
	layout->addWidget(aspectcombo, 4, 1);
	layout->addWidget(scalelabel, 5, 0);
	layout->addWidget(scalecombo, 5, 1);
	layout->addWidget(devicelabel, 6, 0);
	layout->addWidget(deviceCombo, 6, 1);
	layout->addLayout(effectlayout, 7, 0);
	layout->addWidget(effectButton, 7, 1);
	layout->addWidget(buttonBox, 8, 1);
	settingsDialog->setLayout(layout);
}


bool XWMediaPlayer::playPauseForDialog()
{
	if (m_hasSmallScreen && m_MediaObject.hasVideo()) 
	{
        if (Phonon::PlayingState == m_MediaObject.state()) 
        {
            m_MediaObject.pause();
            return true;
        }
    }
    return false;
}
