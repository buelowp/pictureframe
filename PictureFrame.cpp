/*
 * PictureFrame.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#include "PictureFrame.h"

PictureFrame::PictureFrame(QWidget *parent) : QMainWindow(parent) {
	m_image = new QLabel(this);
    bTurnOff = false;
    m_notRunning = true;
	setWindowState(Qt::WindowFullScreen);

    m_contentList = new FileDownload(this);
    m_imageFile = new FileDownload(this);

	m_nextImageTimer = new QTimer(this);
    m_getNewContentListTimer = new QTimer(this);

	connect(m_nextImageTimer, SIGNAL(timeout()), this, SLOT(displayNextImage()));
    connect m_getNewContentListTimer, SIGNAL(timeout()), this, SLOT(downloadContentList()));
    connect(m_contentList, SIGNAL(downloadFinished()), this, SLOT(downloadContentListComplete()));
    connect(m_contentList, SIGNAL(downloadError(QNetworkReply::NetworkError)), this, SLOT(fileDownloadError(QNetworkReply::NetworkError)));
    connect(m_imageFile, SIGNAL(downloaded()), this, SLOT(fileDownloadComplete()));
    connect(m_imageFile, SIGNAL(downloadError(QNetworkReply::NetworkError)), this, SLOT(fileDownloadError(QNetworkReply::NetworkError)));
    connect(this, SIGNAL(fileDownloadsComplete()), this, SLOT(unlockShowEvent()));

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	m_ImageTimeout = settings.value("ImageTimeout", 5000).toInt();
    m_turnOff = settings.value("TurnOff", false).toBool();
    picXML.setUrl(settings.value("URL").toString());
    m_networkInterface = settings.value("Network").toString();
    
    downloadContentList();
    m_getNewContentListTimer->start(PF_ONE_HOUR);
}

PictureFrame::~PictureFrame()
{
}

bool PictureFrame::init()
{
    
    if (bTurnOff) {
        offTime = settings.value("OffTime", "17:00").toString();
        onTime = settings.value("OnTime", "7:00").toString();
    }

	lbImage->setGeometry(0, 0, width(), height());
	lbImage->setAlignment(Qt::AlignCenter);

	checkNetwork();

	return true;
}

void PictureFrame::unlockShowEvent()
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

void PictureFrame::downloadContentList()
{
	qWarning() << __FUNCTION__ << ": Getting" << picXML << "from server";
	m_fileInProgress = picXML.fileName();
	m_contentList->getFile(picXML);
}

void PictureFrame::fileDownloadError(QNetworkReply::NetworkError error)
{
	qWarning() << __FUNCTION__ << ": Unable to download" << m_fileInProgress << "with error" << error;
	if (m_fileInProgress == "contentlist.xml") {
		timeout();
	}
	else {
		m_fileInProgress.clear();
		downloadFile();
	}
}

void PictureFrame::fileDownloadComplete()
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

void PictureFrame::checkFileExistsAndDownload(QString name, QString url)
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

void PictureFrame::deleteUnusedFiles()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QString path = settings.value("ImagePath1").toString();
	QDir pImagePath(path);;
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
        m_fileManager->deleteFile(file);
	}
}

void PictureFrame::downloadNewContent(QMap<String,QString> c)
{
	if (c.size()) {
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

void PictureFrame::downloadContentListComplete()
{
    QMap<QString,QString> newContent;
    QStringList delList;
	QXmlStreamReader doc(m_contentList->getFileContents());
    m_nextImageTimer->stop();

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
						newContent[url.fileName()] = url.toDisplayString();
                        delList << url.fileName();
					}
				}
			}
		}
		qDebug() << __FUNCTION__ << ": parsed contentlist.xml for" << m_ImageList.size() << "elements";
        if (newContent.size())
            downloadNewContent(newContent);
	}
}

void PictureFrame::timeout()
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

void PictureFrame::showEvent(QShowEvent*)
{
	qWarning() << __FUNCTION__;
	fm.init();
	showLocalImage();
}

void PictureFrame::showLocalImage()
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
