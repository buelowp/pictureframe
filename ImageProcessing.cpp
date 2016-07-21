/*
 * ImageProcessing.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#include "ImageProcessing.h"

ImageProcessing::ImageProcessing(QWidget *parent) : QMainWindow(parent) {
	iImageIndex = 0;
	m_NetTimeout = 0;
	m_ImageTimeout = 0;

	lbImage = new QLabel(this);
    bTurnOff = false;
    m_notRunning = true;
	setWindowState(Qt::WindowFullScreen);

	player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist();
    player->setPlaylist(playlist);

    m_contentList = new FileDownload(this);
    m_imageFile = new FileDownload(this);

	pNextImage = new QTimer();
	m_synchNetworkOperation.unlock();

	connect(pNextImage, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(m_contentList, SIGNAL(downloaded()), this, SLOT(contentListDownloadComplete()));
    connect(m_contentList, SIGNAL(downloadError(QNetworkReply::NetworkError)), this, SLOT(fileDownloadError(QNetworkReply::NetworkError)));
    connect(m_imageFile, SIGNAL(downloaded()), this, SLOT(fileDownloadComplete()));
    connect(m_imageFile, SIGNAL(downloadError(QNetworkReply::NetworkError)), this, SLOT(fileDownloadError(QNetworkReply::NetworkError)));
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
    connect(player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));
    connect(this, SIGNAL(fileDownloadsComplete()), this, SLOT(unlockShowEvent()));

    videoWidget = new QVideoWidget(this);
    player->setVideoOutput(videoWidget);
    videoWidget->hide();
}

ImageProcessing::~ImageProcessing()
{
}

void ImageProcessing::durationChanged(qint64)
{
	qWarning() << "media duration changed";
}

void ImageProcessing::positionChanged(qint64)
{
	qWarning() << "media position changed";
}

void ImageProcessing::metaDataChanged()
{
	qWarning() << "Metadata changed in video file";
}

bool ImageProcessing::init()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	m_ImageTimeout = settings.value("ImageTimeout", 5000).toInt();
	m_NetTimeout = settings.value("NetworkTimeout", 60000).toInt();
    bTurnOff = settings.value("TurnOff", false).toBool();
    picXML.setUrl(settings.value("URL").toString());
    
    if (bTurnOff) {
        offTime = settings.value("OffTime", "17:00").toString();
        onTime = settings.value("OnTime", "7:00").toString();
    }

	lbImage->setGeometry(0, 0, width(), height());
	lbImage->setAlignment(Qt::AlignCenter);

	networkTest();

	if (!player->isAvailable()) {
		qWarning() << "The QMediaPlayer object does not have a valid service. Check plugins.";
		return false;
	}
	return true;
}

void ImageProcessing::unlockShowEvent()
{
	deleteUnusedFiles();
	QTimer::singleShot(m_NetTimeout, this, SLOT(networkTest()));
	fm.init();

	if (m_notRunning) {
		show();
	}
	else {
		timeout();
	}
}

void ImageProcessing::getContentList()
{
	qWarning() << "Getting" << picXML << "from server";
	m_fileInProgress = picXML.fileName();
	m_contentList->getFile(picXML);
}

void ImageProcessing::fileDownloadError(QNetworkReply::NetworkError error)
{
	qWarning() << "Unable to download" << m_fileInProgress << "with error" << error;
	m_downloadInProgress.unlock();
}

void ImageProcessing::fileDownloadComplete()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QString path = settings.value("ImagePath1").toString();
	QString dlLocation = path + "/" + m_fileInProgress;
	QFile f(dlLocation);

	qWarning() << "fileDownloadComplete:" << dlLocation;

	if (f.open(QIODevice::WriteOnly)) {
		QDataStream out(&f);
		out << m_imageFile->downloadedData();
		f.close();
	}
	m_fileInProgress.clear();
	downloadFile();
}

void ImageProcessing::checkFileExistsAndDownload(QString name, QString url)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QDir pImagePath(settings.value("ImagePath1").toString());

	qWarning() << "Checking for" << name << "at path" << pImagePath.absolutePath();

	if (pImagePath.exists()) {
		QStringList l = pImagePath.entryList();
		if (!l.contains(name)) {
			qWarning() << "Unable to find" << name;
			m_fileInProgress = name;
			lbImage->setText(QString("Downloading %1").arg(url));
			m_imageFile->getFile(url);
		}
		else {
			qWarning() << "Found" << name;
			m_fileInProgress.clear();
			downloadFile();
		}
	}
}

void ImageProcessing::deleteUnusedFiles()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QString path = settings.value("ImagePath1").toString();
	QDir pImagePath(path);;
	QMapIterator<QString, QString> it(m_ImageList);
	QStringList files = pImagePath.entryList(QDir::Files|QDir::NoDotAndDotDot, QDir::Name);


	QSet<QString> fromLocal = QSet<QString>::fromList(files);
	QSet<QString> fromServer = QSet<QString>::fromList(m_delList);

	fromLocal.subtract(fromServer);
	qWarning() << "Checking" << pImagePath.absolutePath() << "to find files that should be removed";
	QSetIterator<QString> toDel(fromLocal);
	while (toDel.hasNext()) {
		QString file = pImagePath.absolutePath() + "/" + toDel.next();
		qWarning() << "Deletting" << file;
	}
}

void ImageProcessing::downloadFile()
{
	if (m_ImageList.size()) {
		QMap<QString,QString>::iterator element = m_ImageList.begin();
		qWarning() << "Size is" << m_ImageList.size() << "Downloading" << element.key();
		QString key = element.key();
		QString value = element.value();
		m_ImageList.erase(element);
		checkFileExistsAndDownload(key, value);
	}
	else {
		emit fileDownloadsComplete();
	}
}

void ImageProcessing::contentListDownloadComplete()
{
	QXmlStreamReader doc(m_contentList->downloadedData());

	if (doc.hasError()) {
		qWarning() << "Downloaded xml has errors";
		return;
	}
	else {
		while (!doc.atEnd() && !doc.hasError()) {
			QXmlStreamReader::TokenType token = doc.readNext();
			if (token == QXmlStreamReader::StartDocument)
				continue;

			if (token == QXmlStreamReader::StartElement) {
				if (doc.name() == "Url") {
					QUrl url(doc.readElementText());
					if (url.isValid()) {
						m_ImageList[url.fileName()] = url.toDisplayString();
						m_delList << url.fileName();
						qWarning() << "Adding" << url.fileName() << "with URL" << url.toDisplayString();
					}
				}
			}
		}
		downloadFile();
	}
}

void ImageProcessing::networkTest()
{
	QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

	qWarning() << "Running the network check";
	if (pNextImage->isActive()) {
		pNextImage->stop();
	}
	for (int i = 0; i < interfaces.size(); i++) {
		QNetworkInterface ni = interfaces[i];
		if (ni.name() == "lo")
			continue;

		qWarning() << "Checking" << ni.name() << " with flags" << ni.flags();

		if (ni.flags().testFlag(QNetworkInterface::IsUp) && ni.flags().testFlag(QNetworkInterface::IsRunning)) {
			pNextImage->stop();
			lbImage->setText("Checking for new images");
			getContentList();
		}
	}
}

void ImageProcessing::timeout()
{
	QString nextImage = fm.next();
	qWarning() << "timeout: nextImage:" << nextImage;

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
		pNextImage->setInterval(m_ImageTimeout);
		pNextImage->setSingleShot(true);
		pNextImage->start();
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
	QString nextImage = fm.next();
	qWarning() << "showevent: nextImage:" << nextImage;
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
		pNextImage->setInterval(m_ImageTimeout);
		pNextImage->setSingleShot(true);
		pNextImage->start();
	}
	m_notRunning = false;
}
