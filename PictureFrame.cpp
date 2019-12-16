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
    
    if (m_turnOff) {
        m_offTime = QTime::fromString(settings.value("OffTime", "17:00").toString(), "h:mm");
        m_onTime = QTime::fromString(settings.value("OnTime", "7:00").toString(), "h:mm");
    }

    m_manager = new DownloadManager();
    m_files = new FileManagement();
    m_progress = new ProgressDialog();
    m_video = new QVideoWidget(this);
	m_image = new QLabel(this);
    m_player = new QMediaPlayer(this);
    
    m_progress->hide();
    m_video->hide();
    m_progress->setGeometry(150, 150, 600, 200);
    m_player->setVideoOutput(m_video);
    
	m_image->setAlignment(Qt::AlignCenter);

    m_turnOff = false;

	m_nextImageTimer = new QTimer(this);
    m_getNewContentListTimer = new QTimer(this);

	connect(m_nextImageTimer, SIGNAL(timeout()), this, SLOT(displayNextImage()));
	connect(m_getNewContentListTimer, SIGNAL(timeout()), this, SLOT(downloadContentList()));
    connect(m_manager, SIGNAL(contentListError()), this, SLOT(contentListFailed()));
    connect(m_manager, SIGNAL(newFileMap(QMap<QString, QUrl>)), this, SLOT(newFileMap(QMap<QString, QUrl>)));
    connect(m_manager, SIGNAL(downloadComplete(QString, QByteArray)), this, SLOT(downloadComplete(QString, QByteArray)));
    connect(m_manager, SIGNAL(allDownloadsComplete()), this, SLOT(downloadsComplete()));
    connect(m_manager, SIGNAL(downloadProgress(QString, qint64, qint64)), SLOT(downloadProgress(QString, qint64, qint64)));
    connect(m_manager, SIGNAL(downloadStarted(QString)), this, SLOT(downloadStarted(QString)));
    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(videoStateChanged(QMediaPlayer::State)));
    connect(m_player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(videoError(QMediaPlayer::Error)));

    downloadContentList();
    m_getNewContentListTimer->start(PF_ONE_HOUR);
}

PictureFrame::~PictureFrame()
{
}

void PictureFrame::videoError(QMediaPlayer::Error error)
{
    qDebug() << __FUNCTION__ << ":" << error;
}

void PictureFrame::videoStateChanged(QMediaPlayer::State state)
{
    switch (state) {
        case QMediaPlayer::StoppedState:
            displayNextImage();
            m_video->hide();
            qDebug() << __FUNCTION__ << ": Video done";
            break;
        case QMediaPlayer::PlayingState:
            qDebug() << __FUNCTION__ << ": Video started";
            break;
        case QMediaPlayer::PausedState:
            qDebug() << __FUNCTION__ << ": Video started";
            break;
        default:
            qDebug() << __FUNCTION__ << ": Unknow video state";
    }
}

void PictureFrame::downloadProgress(QString file, qint64 percent, qint64 size)
{
    Q_UNUSED(file);

    m_progress->setProgress(percent, size);
}

void PictureFrame::downloadStarted(QString file)
{
    m_progress->setTitle(file);
    m_progress->show();
}

void PictureFrame::downloadComplete(QString file, QByteArray data)
{
    m_files->saveFile(file, data);
}

void PictureFrame::downloadsComplete()
{
    m_progress->hide();
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
    m_manager->addToDownloadList(m_contentListURL);    
}

void PictureFrame::newFileMap(QMap<QString, QUrl> filelist)
{
    QSet<QString> dlist = filelist.keys().toSet();
    QSet<QString> flist  = m_files->fileNameList().toSet();
    
    QSet<QString> toDownload = dlist.subtract(flist);
    dlist = filelist.keys().toSet();
    QSet<QString> toDelete = flist.subtract(dlist);
    
    m_manager->addToDownloadList(toDownload, filelist);
    m_files->setDeleteList(toDelete);
}

void PictureFrame::downloadFileFailed(QString file)
{
	qWarning() << __FUNCTION__ << ": failed to download" << file;
}

void PictureFrame::showEvent(QShowEvent*)
{
	qDebug() << __FUNCTION__ << ": width:" << width() << ", height:" << height();
    
    m_image->resize(width(), height());
    m_video->resize(width(), height());
}

void PictureFrame::displayImage(QString file)
{
    m_image->show();
    QPixmap pm(file);
    if (!pm.isNull()) {
        if (pm.width() > pm.height())
            m_image->setPixmap(pm.scaledToWidth(width()));
        else
            m_image->setPixmap(pm.scaledToHeight(height()));
        m_image->show();
    }

    m_nextImageTimer->setInterval(m_ImageTimeout);
    m_nextImageTimer->setSingleShot(true);
    m_nextImageTimer->start();
}

void PictureFrame::displayVideo(QString file)
{
    qDebug() << __PRETTY_FUNCTION__ << ":" << file;
    m_player->setMedia(QUrl::fromLocalFile(file));
    m_player->setMuted(true);
    m_image->hide();
    m_video->show();
    m_player->play();
}

void PictureFrame::displayNextImage()
{
    QMimeDatabase mimeDatabase;
    QMimeType mimeType;
    QString file;

    if (m_nextImageTimer->isActive())
        m_nextImageTimer->stop();
    
    if (m_files->nextFileInList(file)) {
        mimeType = mimeDatabase.mimeTypeForFile(file);
        if (mimeType.inherits("video/mp4"))
            displayVideo(file);
        else if (mimeType.inherits("image/jpeg"))
            displayImage(file);
        else {
            qWarning() << __FUNCTION__ << ":" << file << ": unknown mime type" << mimeType;
        }
    }
}
