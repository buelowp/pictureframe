/*
 * PictureFrame.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#include "PictureFrame.h"

PictureFrame::PictureFrame(QWidget *parent) : QMainWindow(parent) {
	m_NetTimeout = 0;
	m_ImageTimeout = 0;

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	m_ImageTimeout = settings.value("ImageTimeout", 5000).toInt();
	m_NetTimeout = settings.value("NetworkTimeout", 60000).toInt();
    m_turnOff = settings.value("TurnOff", false).toBool();
    m_contentFileURL.setUrl(settings.value("URL").toString());
    
    if (m_turnOff) {
        m_offTime = QTime::fromString(settings.value("OffTime", "17:00").toString(), "h:mm");
        m_onTime = QTime::fromString(settings.value("OnTime", "7:00").toString(), "h:mm");
    }

	m_image = new QLabel(this);
	m_image->setAlignment(Qt::AlignCenter);

    m_turnOff = false;

    m_download = new FileDownload(this);
    m_fileManager = new FileManagement();

	m_nextImageTimer = new QTimer(this);
    m_getNewContentListTimer = new QTimer(this);

	connect(m_nextImageTimer, SIGNAL(timeout()), this, SLOT(displayNextImage()));
	connect(m_getNewContentListTimer, SIGNAL(timeout()), this, SLOT(downloadContentList()));
    connect(m_download, SIGNAL(downloadFinished()), this, SLOT(fileDownloadComplete()), Qt::QueuedConnection);
    connect(m_download, SIGNAL(contentListFinished()), this, SLOT(downloadContentListComplete()), Qt::QueuedConnection);
    connect(m_download, SIGNAL(downloadError(QNetworkReply::NetworkError)), this, SLOT(downloadFileFailed(QNetworkReply::NetworkError)));
}

PictureFrame::~PictureFrame()
{
}

void PictureFrame::downloadContentList()
{
	qWarning() << __FUNCTION__ << ": Getting" << m_contentFileURL << "from server";
	m_fileInProgress = m_contentFileURL.fileName();
	m_download->getFile(m_contentFileURL);
}

void PictureFrame::downloadContentListFailed(QNetworkReply::NetworkError error)
{
	qWarning() << __FUNCTION__ << ": ContentList download failed: " << error;
}

void PictureFrame::downloadFileFailed(QNetworkReply::NetworkError error)
{
	qWarning() << __FUNCTION__ << ": failed to download" << m_urlInProgress.toDisplayString() << ":" << error;
}

void PictureFrame::fileDownloadComplete()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QString path = settings.value("ImagePath1").toString();
	QString dlLocation = path + "/" + m_fileInProgress;
	QFile f(dlLocation);

    qDebug() << __PRETTY_FUNCTION__ << ": got data for" << m_fileInProgress;
	if (f.open(QIODevice::WriteOnly)) {
		QDataStream out(&f);
        QByteArray data = m_download->getFileContents();
		out.writeRawData(data.data(), data.size());
		f.close();
	}
	m_fileInProgress.clear();
}

void PictureFrame::deleteRemovedFiles(QStringList &deleteList)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QString path = settings.value("ImagePath1").toString();
	QDir pImagePath(path);;
	QStringList files = pImagePath.entryList(QDir::Files|QDir::NoDotAndDotDot, QDir::Name);
	QSet<QString> fromLocal = QSet<QString>::fromList(files);
	QSet<QString> fromServer = QSet<QString>::fromList(deleteList);

	qWarning() << __FUNCTION__ << ": there are" << files.size() << "local files";
	qWarning() << __FUNCTION__ << ": there are" << deleteList.size() << "files in the delete list";
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

/**
 * \fn void PictureFrame::getNewFiles(QMap<QString,QString> imageList)
 * \param imageList QMap of strings with key of filename, and value of file URL
 * \brief cycle through map to download all files that don't exist yet
 */
void PictureFrame::getNewFiles(QMap<QString,QString> imageList)
{
	QMapIterator<QString,QString> it(imageList);

    while (it.hasNext()) {
        it.next();
        if (!m_fileManager->fileExists(it.key())) {
            qDebug() << __PRETTY_FUNCTION__ << ": getting file" << it.key();
            m_download->getFile(QUrl(it.value()));
            m_fileInProgress = it.key();
            m_urlInProgress = QUrl(it.value());
        }
        else {
            qDebug() << __PRETTY_FUNCTION__ << ": file exists, move along";
        }
    }
}

void PictureFrame::downloadContentListComplete()
{
    QMap<QString,QString> imageList;
    QStringList deleteList;
	QXmlStreamReader doc(m_download->getFileContents());

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
						imageList[url.fileName()] = url.toDisplayString();
                        deleteList << url.fileName();
					}
				}
			}
		}
        m_fileManager->updateLocalFileList();
		if (!imageList.empty()) {
            getNewFiles(imageList);
            deleteRemovedFiles(deleteList);
        }
	}
	m_getNewContentListTimer->start(PF_ONE_HOUR);
    show();
    displayNextImage();
}

void PictureFrame::showEvent(QShowEvent*)
{
	qDebug() << __FUNCTION__ << ": width:" << width() << ", height:" << height();
    m_image->resize(width(), height());
	displayNextImage();
}

void PictureFrame::displayNextImage()
{
	QString image;

    while (true) {
        if (m_fileManager->nextFileInList(image)) {
            QPixmap pm(image);
            if (pm.isNull())
                continue;

            qDebug() << __FUNCTION__ << ": showing" << image << "at" << width() << ":" << height();
            if (pm.width() > pm.height())
                m_image->setPixmap(pm.scaledToWidth(width()));
            else
                m_image->setPixmap(pm.scaledToHeight(height()));
        }
        else {
            m_image->setText("Image not available");
        }

        m_image->show();
        m_nextImageTimer->setInterval(m_ImageTimeout);
        m_nextImageTimer->setSingleShot(true);
        m_nextImageTimer->start();
        break;
    }
}
