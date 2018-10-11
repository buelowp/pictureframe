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

	bool init();
	void menu();
	bool validateOptions();
	void getNewContent();
	void checkFileExistsAndDownload(QString, QString);
	void deleteUnusedFiles();

public slots:
	void nextImageTimeout();
	void displayErrorMessage();
	void checkNetwork();
	void fileDownloadComplete();
	void downloadContentListComplete();
	void fileDownloadError(QNetworkReply::NetworkError);
	void unlockShowEvent();
	void downloadContentList();

signals:
	void fileDownloadsComplete();

protected:
	void showEvent(QShowEvent*);

private:
	void downloadFile();
	void showLocalImage();

	QTimer *m_nextImageTimer;
    QTimer *m_getNewContentListTimer;
	int m_ImageTimeout;
	int m_NetTimeout;
    bool m_turnOff;
	QLabel *m_image;
    QString offTime;
    QString onTime;
    QUrl m_contentFileURL;
    QString m_fileInProgress;

	FileManagement *m_fileManager;
    FileDownload *m_contentList;
    FileDownload *m_imageFile;
};

#endif /* IMAGEPROCESSING_H_ */
