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

    m_contentList = new FileDownload(this);
    m_imageFile = new FileDownload(this);

    connect(m_contentList, SIGNAL(downloaded()), this, SLOT(contentListDownloadComplete()));
    connect(m_contentList, SIGNAL(downloadError()), this, SLOT(fileDownloadError()));
    connect(m_imageFile, SIGNAL(downloaded()), this, SLOT(fileDownloadComplete()));
    connect(m_imageFile, SIGNAL(downloadError()), this, SLOT(fileDownloadError()));
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
    connect(player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));

    videoWidget = new QVideoWidget(this);
    player->setVideoOutput(videoWidget);
    videoWidget->hide();
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
    picXML.setUrl(settings.value("URL").toString());
    
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
	pNextImage = new QTimer();
	pNextImage->setInterval(iTimeout);
	pNextImage->setSingleShot(true);
	connect(pNextImage, SIGNAL(timeout()), this, SLOT(timeout()));
	return true;
}

void ImageProcessing::getContentList()
{
	m_contentList->getFile(picXML);
}

void ImageProcessing::fileDownloadError()
{
	qDebug() << "Unable to download" << m_fileInProgress;
	m_downloadInProgress.unlock();
}

void ImageProcessing::fileDownloadComplete()
{
	QFile f(m_fileInProgress);

	if (f.open(QIODevice::WriteOnly)) {
		QDataStream out(&f);
		out << m_imageFile->downloadedData();
	}
	f.close();
	m_fileInProgress.clear();
	m_downloadInProgress.unlock();
}

void ImageProcessing::checkFileExistsAndDownload(QString name, QString url)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QString path = settings.value("ImagePath1").toString();
	QDir *pImagePath = new QDir(path);;

	m_downloadInProgress.lock();

	if (pImagePath->exists()) {
		QStringList l = pImagePath->entryList();
		if (!l.contains(name)) {
			m_fileInProgress = name;
			lbImage->setText(QString("Downloading %1").arg(url));
			m_imageFile->getFile(url);
		}
	}
}

void ImageProcessing::contentListDownloadComplete()
{
	QXmlStreamReader doc(m_contentList->downloadedData());

	if (doc.hasError()) {
		return;
	}
	else {
	    if (doc.readNextStartElement()) {
	        if (doc.name() == "Images") {
	            while(doc.readNextStartElement()) {
	            	if (doc.name() == "Image") {
	            		while (doc.readNextStartElement()) {
	            			if (doc.name() == "Name") {
	            				QString n = doc.readElementText();
	            				QString u;
	            				doc.readNextStartElement();
	            				if (doc.name() == "Url")
	            					u = doc.readElementText();
	            				m_ImageList[n] = u;
	            				qDebug() << "Adding" << n << "with URL" << u;
	            			}
	            		}
	            	}
	            }
	        }
	    }
	    QMapIterator<QString, QString> i(m_ImageList);
	    while (i.hasNext()) {
	        i.next();
	        checkFileExistsAndDownload(i.key(), i.value());
	    }
	}
}

void ImageProcessing::networkTest()
{
	QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

	for (int i = 0; i < interfaces.size(); i++) {
		QNetworkInterface ni = interfaces[i];
		if (ni.flags().testFlag(QNetworkInterface::IsUp) && ni.flags().testFlag(QNetworkInterface::IsRunning)) {
			pNextImage->stop();
			lbImage->setText("Getting more images");
			getContentList();
			QTimer::singleShot(3600000, this, SLOT(networkTest()));
			fm.init();
			timeout();
		}
	}
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
		pNextImage->setInterval(iTimeout);
		pNextImage->setSingleShot(true);
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
		pNextImage->setInterval(iTimeout);
		pNextImage->setSingleShot(true);
		videoWidget->hide();
	}
}
