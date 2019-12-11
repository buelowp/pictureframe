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

	void menu();
	bool validateOptions();
	void getNewContent();
	void getNewFiles(QMap<QString,QString>);
	void deleteRemovedFiles(QStringList&);

public slots:
	void fileDownloadComplete();
	void downloadContentListComplete();
	void downloadContentListFailed(QNetworkReply::NetworkError);
	void downloadFileFailed(QNetworkReply::NetworkError);
	void downloadContentList();
	void displayNextImage();

signals:
	void fileDownloadsComplete();

protected:
	void showEvent(QShowEvent*);

private:
	void downloadFile();

	QTimer *m_nextImageTimer;
    QTimer *m_getNewContentListTimer;
	int m_ImageTimeout;
	int m_NetTimeout;
    bool m_turnOff;
	QLabel *m_image;
    QTime m_offTime;
    QTime m_onTime;
    QUrl m_contentFileURL;
    QUrl m_urlInProgress;
    QString m_fileInProgress;
    QMutex m_downloadMutex;

	FileManagement *m_fileManager;
    FileDownload *m_download;
};

#endif /* IMAGEPROCESSING_H_ */
