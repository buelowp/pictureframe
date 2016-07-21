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
	qWarning() << __FUNCTION__ << ": media duration changed";
}

void ImageProcessing::positionChanged(qint64)
{
	qWarning() << __FUNCTION__ << ": media position changed";
}

void ImageProcessing::metaDataChanged()
{
	qWarning() << __FUNCTION__ << ": metadata changed in video file";
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

	checkNetwork();

	if (!player->isAvailable()) {
		qWarning() << __FUNCTION__ << ": The QMediaPlayer object does not have a valid service. Check plugins.";
		return false;
	}
	return true;
}

void ImageProcessing::unlockShowEvent()
{
	deleteUnusedFiles();

	if (m_notRunning) {
		qWarning() << __FUNCTION__ << ": not running";
		m_notRunning = false;
		show();
	}
	else {
		qWarning() << __FUNCTION__ << ": running";
		showLocalImage();
	}
}

void ImageProcessing::getContentList()
{
	qWarning() << __FUNCTION__ << ": Getting" << picXML << "from server";
	m_fileInProgress = picXML.fileName();
	m_contentList->getFile(picXML);
}

void ImageProcessing::fileDownloadError(QNetworkReply::NetworkError error)
{
	qWarning() << __FUNCTION__ << ": Unable to download" << m_fileInProgress << "with error" << error;
}

void ImageProcessing::fileDownloadComplete()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QString path = settings.value("ImagePath1").toString();
	QString dlLocation = path + "/" + m_fileInProgress;
	QFile f(dlLocation);

	if (f.open(QIODevice::WriteOnly)) {
		QDataStream out(&f);
		out.writeRawData(m_imageFile->downloadedData().data(), m_imageFile->downloadedData().size());
		f.close();
	}
	m_fileInProgress.clear();
	downloadFile();
}

void ImageProcessing::checkFileExistsAndDownload(QString name, QString url)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QDir pImagePath(settings.value("ImagePath1").toString());

	if (pImagePath.exists()) {
		QStringList l = pImagePath.entryList();
		if (!l.contains(name)) {
			m_fileInProgress = name;
			qWarning() << __FUNCTION__ << ": Downloading" << name;
			m_imageFile->getFile(url);
		}
		else {
			m_fileInProgress.clear();
			qWarning() << __FUNCTION__ << ": going recursive";
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

	qWarning() << __FUNCTION__ << ": there are" << files.size() << "local files";
	qWarning() << __FUNCTION__ << ": there are" << m_delList.size() << "files in the delete list";
	fromLocal.subtract(fromServer);
	qWarning() << __FUNCTION__ << ": The list of files to delete has" << fromLocal.size() << "entries";
	QSetIterator<QString> toDel(fromLocal);
	while (toDel.hasNext()) {
		QString file = pImagePath.absolutePath() + "/" + toDel.next();
		qWarning() << __FUNCTION__ << ": Deleting" << file;
		QFile f(file);
		f.remove();
	}
}

void ImageProcessing::downloadFile()
{
	if (m_ImageList.size()) {
		QMap<QString,QString>::iterator element = m_ImageList.begin();
		QString key = element.key();
		QString value = element.value();
		m_ImageList.erase(element);
		checkFileExistsAndDownload(key, value);
	}
	else {
		qWarning() << __FUNCTION__ << ": Done retrieving network contents";
		emit fileDownloadsComplete();
	}
}

void ImageProcessing::contentListDownloadComplete()
{
	QXmlStreamReader doc(m_contentList->downloadedData());
	m_ImageList.clear();
	m_delList.clear();

	if (doc.hasError()) {
		qWarning() << __FUNCTION__ << ": Downloaded xml has errors";
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
					}
				}
			}
		}
		qWarning() << __FUNCTION__ << ": parsed contentlist.xml for" << m_ImageList.size() << "elements";
		downloadFile();
	}
}

void ImageProcessing::checkNetwork()
{
	bool running = false;
	QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

	qWarning() << __FUNCTION__ << ": Running the network check";
	if (pNextImage->isActive()) {
		qWarning() << __FUNCTION__ << ": Turning off image display timer";
		pNextImage->stop();
	}
	for (int i = 0; i < interfaces.size(); i++) {
		QNetworkInterface ni = interfaces[i];
		if (ni.name() == "lo")
			continue;

		qWarning() << __FUNCTION__ << ": Checking interface" << ni.name() << " with flags" << ni.flags();

		if (ni.flags().testFlag(QNetworkInterface::IsUp) && ni.flags().testFlag(QNetworkInterface::IsRunning)) {
			running = true;
			getContentList();
		}
	}
	if (!running) {
		qWarning() << __FUNCTION__ << ": Network check didn't find an available interface";
		showLocalImage();
	}
}

void ImageProcessing::displayErrorMessage()
{
	qWarning() << __FUNCTION__ << "Video Player Error:" << player->errorString();
}

void ImageProcessing::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
	switch (status) {
	case QMediaPlayer::EndOfMedia:
		qWarning() << __FUNCTION__ << ": Finished playing video";
		QTimer::singleShot(0, this, SLOT(timeout()));
		break;
	case QMediaPlayer::InvalidMedia:
		qWarning() << __FUNCTION__ << ": InvalidMedia Error:" << player->errorString();
		break;
	default:
		qWarning() << __FUNCTION__ << ": Media Status Changed:" << status;
	}
}

void ImageProcessing::mediaStateChanged(QMediaPlayer::State state)
{
	switch (state) {
	case QMediaPlayer::StoppedState:
		qWarning() << __FUNCTION__ << ": Got a stop state message";
		break;
	case QMediaPlayer::PlayingState:
		qWarning() << __FUNCTION__ << ": Got a playing state message";
		break;
	case QMediaPlayer::PausedState:
		qWarning() << __FUNCTION__ << ": Got a paused state message";
		break;
	}
}

void ImageProcessing::timeout()
{
	qWarning() << __FUNCTION__;
	if (fm.isComplete()) {
		qWarning() << __FUNCTION__ << ": running network check from isComplete = true";
		checkNetwork();
	}
	else {
		showLocalImage();
	}
}

void ImageProcessing::showEvent(QShowEvent*)
{
	qWarning() << __FUNCTION__;
	fm.init();
	showLocalImage();
}

void ImageProcessing::showLocalImage()
{
	QString nextImage = fm.next();
	qWarning() << __FUNCTION__ << ": showing" << nextImage;

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
		qWarning() << __FUNCTION__ << ": Starting timer with timeout" << m_ImageTimeout;
		pNextImage->setInterval(m_ImageTimeout);
		pNextImage->setSingleShot(true);
		pNextImage->start();
	}
}
