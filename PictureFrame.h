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

#include "images.h"
#include "FileManagement.h"
#include "FileDownload.h"

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
    QProgressDialog *m_progress;
    QLabel *m_progressLabel;
	QTimer *m_nextImageTimer;
    QTimer *m_getNewContentListTimer;
	int m_ImageTimeout;
	int m_NetTimeout;
    bool m_turnOff;
	QLabel *m_image;
    QTime m_offTime;
    QTime m_onTime;
    QString m_fileInProgress;
    QMutex m_downloadMutex;
    QUrl m_contentListURL;
    FileManagement *m_files;
    Images *m_images;
};

#endif /* IMAGEPROCESSING_H_ */
