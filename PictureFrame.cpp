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
    m_contentListURL = settings.value("URL").toString();
    
    m_images = new Images();
    m_files = new FileManagement();
    m_progressLabel = new QLabel();
    m_progress = new QProgressDialog(this);
    m_progress->hide();
    m_progressLabel->hide();
    m_progress->setMinimumDuration(50);
    m_progress->setWindowModality(Qt::WindowModal);
    
    if (m_turnOff) {
        m_offTime = QTime::fromString(settings.value("OffTime", "17:00").toString(), "h:mm");
        m_onTime = QTime::fromString(settings.value("OnTime", "7:00").toString(), "h:mm");
    }

	m_image = new QLabel(this);
	m_image->setAlignment(Qt::AlignCenter);

    m_turnOff = false;

	m_nextImageTimer = new QTimer(this);
    m_getNewContentListTimer = new QTimer(this);

	connect(m_nextImageTimer, SIGNAL(timeout()), this, SLOT(displayNextImage()));
	connect(m_getNewContentListTimer, SIGNAL(timeout()), this, SLOT(downloadContentList()));
    connect(m_images, SIGNAL(contentListError()), this, SLOT(contentListFailed()));
    connect(m_images, SIGNAL(newFileMap(QMap<QString, QUrl>)), this, SLOT(newFileMap(QMap<QString, QUrl>)));
    connect(m_images, SIGNAL(downloadComplete(QString, QByteArray)), this, SLOT(downloadComplete(QString, QByteArray)));
    connect(m_images, SIGNAL(allDownloadsComplete()), this, SLOT(downloadsComplete()));
    connect(m_images, SIGNAL(downloadProgress(QString file, qint64 percent, qint64 size)), SLOT(downloadProgress(QString file, qint64 percent, qint64 size)));
    connect(m_images, SIGNAL(downloadStarted(QString file)), this, SLOT(downloadStarted(QString file)));
}

PictureFrame::~PictureFrame()
{
}

void PictureFrame::downloadProgress(QString file, qint64 percent, qint64 size)
{
    Q_UNUSED(file);

    m_progress->setMaximum(size);
    m_progress->setValue(percent);
}

void PictureFrame::downloadStarted(QString file)
{
    qDebug() << __FUNCTION__ << ": Started download for" << file;
    m_progressLabel->setText("Downloading " + file);
    m_progress->setLabel(m_progressLabel);
    m_progress->setMinimum(0);
    m_progress->setValue(0);
    m_progress->show();
}

void PictureFrame::downloadComplete(QString file, QByteArray data)
{
    m_files->saveFile(file, data);
}

void PictureFrame::downloadsComplete()
{
    m_progress->hide();
    m_files->cleanup();
    m_files->updateLocalFileList();
    displayNextImage();
}

void PictureFrame::contentListFailed()
{
    qDebug() << __FUNCTION__ << ": Didn't get a content list this time";
}

void PictureFrame::downloadContentList()
{
    m_files->updateLocalFileList();
    m_images->addToDownloadList(m_contentListURL);    
}

void PictureFrame::newFileMap(QMap<QString, QUrl> filelist)
{
    QSet<QString> dlist = filelist.keys().toSet();
    QSet<QString> flist  = m_files->localFiles().toSet();
    
    qDebug() << __FUNCTION__ << ": Local files set size" << flist.size() << ", download candidate set size" << dlist.size();
    QSet<QString> toDownload = dlist.subtract(flist);
    QSet<QString> toDelete = flist.subtract(dlist);
    
    qDebug() << __FUNCTION__ << ": Creating download list";
    m_images->addToDownloadList(toDownload, filelist);
    m_files->setDeleteList(toDelete);
}

void PictureFrame::downloadFileFailed(QString file)
{
	qWarning() << __FUNCTION__ << ": failed to download" << file;
}

void PictureFrame::showEvent(QShowEvent*)
{
	qDebug() << __FUNCTION__ << ": width:" << width() << ", height:" << height();
    
    downloadContentList();
    m_getNewContentListTimer->start(PF_ONE_HOUR);
    
    m_image->resize(width(), height());
	displayNextImage();
}

void PictureFrame::displayNextImage()
{
	QString image;

    if (m_nextImageTimer->isActive())
        m_nextImageTimer->stop();
    
    while (true) {
        if (m_files->nextFileInList(image)) {
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
