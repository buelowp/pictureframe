/*
 * ImageProcessing.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#include "ImageProcessing.h"

ImageProcessing::ImageProcessing(QWidget *parent) : QMainWindow(parent) {
	iImageIndex = 0;
	iTimeout = 5000;

	lbImage = new QLabel(this);
    bTurnOff = false;
	setWindowState(Qt::WindowFullScreen);

	player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist();
    player->setPlaylist(playlist);

    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
    connect(player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));

    videoWidget = new VideoWidget(this);
    player->setVideoOutput(videoWidget);
    videoWidget->hide();
    qWarning() << "finished constructor";
}

ImageProcessing::~ImageProcessing() {
	// TODO Auto-generated destructor stub
}

void ImageProcessing::durationChanged(qint64)
{

}

void ImageProcessing::positionChanged(qint64)
{

}

void ImageProcessing::metaDataChanged()
{
	qWarning() << "Metadata changed in video file";
}

bool ImageProcessing::init()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	iTimeout = settings.value("Timeout", 5000).toInt();
    bTurnOff = settings.value("TurnOff", false).toBool();
    
    if (bTurnOff) {
        offTime = settings.value("OffTime", "17:00").toString();
        onTime = settings.value("OnTime", "7:00").toString();
    }

	lbImage->setGeometry(0, 0, width(), height());
	lbImage->setAlignment(Qt::AlignCenter);

	if (!fm.init())
		return false;

	if (!player->isAvailable()) {
		qWarning() << "The QMediaPlayer object does not have a valid service. Check plugins.";
		return false;
	}
	return true;
}

void ImageProcessing::timeout()
{
	QString nextImage = fm.next();
	qWarning() << "nextImage:" << nextImage;

	if (nextImage.endsWith(".mp4")) {
		QUrl url = QUrl::fromLocalFile(nextImage);
		playlist->addMedia(url);
		lbImage->hide();
		videoWidget->show();
		player->play();
	}
	else {
		QPixmap image(nextImage);

		if (image.width() > image.height())
			lbImage->setPixmap(image.scaledToWidth(width()));
		else
			lbImage->setPixmap(image.scaledToHeight(height()));

		lbImage->show();
		videoWidget->hide();
		QTimer::singleShot(iTimeout, this, SLOT(timeout()));
	}
}

void ImageProcessing::displayErrorMessage()
{
	qWarning() << "Video Player Error:" << player->errorString();
}

void ImageProcessing::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
	switch (status) {
	case QMediaPlayer::EndOfMedia:
		qWarning() << "Finished playing video";
		QTimer::singleShot(0, this, SLOT(timeout()));
		break;
	case QMediaPlayer::InvalidMedia:
		qWarning() << "InvalidMedia Error:" << player->errorString();
		break;
	default:
		qWarning() << "Media Status Changed:" << status;
	}
}

void ImageProcessing::mediaStateChanged(QMediaPlayer::State state)
{
	switch (state) {
	case QMediaPlayer::StoppedState:
		qWarning() << "Got a stop state message";
		break;
	case QMediaPlayer::PlayingState:
		qWarning() << "Got a playing state message";
		break;
	case QMediaPlayer::PausedState:
		qWarning() << "Got a paused state message";
		break;
	}
}

void ImageProcessing::showEvent(QShowEvent*)
{
	qWarning() << "In showevent";

	QString nextImage = fm.next();
	qWarning() << "nextImage:" << nextImage;
	if (nextImage.endsWith(".mp4")) {
		QUrl url = QUrl::fromLocalFile(nextImage);
		playlist->addMedia(url);
		lbImage->hide();
		videoWidget->show();
		player->play();
	}
	else {
		QPixmap image(nextImage);

		if (image.width() > image.height())
			lbImage->setPixmap(image.scaledToWidth(width()));
		else
			lbImage->setPixmap(image.scaledToHeight(height()));

		videoWidget->hide();
		lbImage->show();
		QTimer::singleShot(iTimeout, this, SLOT(timeout()));
		videoWidget->hide();
	}
}
