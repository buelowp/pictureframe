/*
 * ImageProcessing.h
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#ifndef IMAGEPROCESSING_H_
#define IMAGEPROCESSING_H_

#include <QtWidgets/QMainWindow>
#include <QtGui/QtGui>
#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QWidget>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtXml/QtXml>

#include "FileManagement.h"
#include "FileDownload.h"

class ImageProcessing : public QMainWindow {
	Q_OBJECT
public:
	ImageProcessing(QWidget *parent = 0);
	virtual ~ImageProcessing();

	bool init();
	void menu();
	bool validateOptions();
	void getNewContent();
	void getContentList();
	void checkFileExistsAndDownload(QString, QString);
	void deleteUnusedFiles();

public slots:
	void timeout();
	void displayErrorMessage();
	void mediaStatusChanged(QMediaPlayer::MediaStatus);
	void mediaStateChanged(QMediaPlayer::State);
	void durationChanged(qint64);
	void positionChanged(qint64);
	void metaDataChanged();
	void checkNetwork();
	void fileDownloadComplete();
	void contentListDownloadComplete();
	void fileDownloadError(QNetworkReply::NetworkError);
	void unlockShowEvent();

signals:
	void fileDownloadsComplete();

protected:
	void showEvent(QShowEvent*);

private:
	void downloadFile();
	void showLocalImage();

	QTimer *pNextImage;
	FileManagement fm;
	int iImageIndex;
	int m_ImageTimeout;
	int m_NetTimeout;
	QString path;
    bool bTurnOff;
	QLabel *lbImage;
    QString offTime;
    QString onTime;
    QUrl picXML;
    QMap<QString, QString> m_ImageList;
    QString m_fileInProgress;
    bool m_notRunning;
    QStringList m_delList;

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QVideoWidget *videoWidget;

    FileDownload *m_contentList;
    FileDownload *m_imageFile;
};

#endif /* IMAGEPROCESSING_H_ */
