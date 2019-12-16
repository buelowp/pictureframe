/*
 * PictureFrame.h
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#ifndef __PICTUREFRAME_H__
#define __PICTUREFRAME_H__

#include <QtWidgets/QtWidgets>
#include <QtGui/QtGui>
#include <QtCore/QtCore>
#include <QtXml/QtXml>

#include "downloadmanager.h"
#include "FileManagement.h"
#include "FileDownload.h"
#include "progressdialog.h"
#include "videowidget.h"

#define PF_ONE_HOUR     (1000 * 60 * 60)

/**
 * \class PictureFrame
 * \brief Provides primary QT Window class to display images
 */
class PictureFrame : public QMainWindow {
	Q_OBJECT
public:
	PictureFrame(QWidget *parent = 0);
	virtual ~PictureFrame();

public slots:
    void newFileMap(QMap<QString, QUrl> filelist);
	void downloadFileFailed(QString);
	void downloadContentList();
	void displayNextImage();
    void contentListFailed();
    void downloadComplete(QString file, QByteArray data);
    void downloadsComplete();
    void downloadStarted(QString file);
    void downloadProgress(QString file, qint64 percent, qint64 size);

protected:
	void showEvent(QShowEvent*);

private:
    void displayVideo(QString file);
    void displayImage(QString file);
    
	QTimer *m_nextImageTimer;
    QTimer *m_getNewContentListTimer;
	QLabel *m_image;
    QTime m_offTime;
    QTime m_onTime;
    QString m_fileInProgress;
    QMutex m_downloadMutex;
    QUrl m_contentListURL;

    int m_ImageTimeout;
	int m_NetTimeout;
    bool m_turnOff;

    FileManagement *m_files;
    DownloadManager *m_manager;
    ProgressDialog *m_progress;
    VideoWidget *m_video;
};

#endif /* IMAGEPROCESSING_H_ */
