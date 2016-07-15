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

public slots:
	void timeout();
	void displayErrorMessage();
	void mediaStatusChanged(QMediaPlayer::MediaStatus);
	void mediaStateChanged(QMediaPlayer::State);
	void durationChanged(qint64);
	void positionChanged(qint64);
	void metaDataChanged();
	void networkTest();
	void fileDownloadComplete();
	void contentListDownloadComplete();
	void fileDownloadError();

protected:
	void showEvent(QShowEvent*);

private slots:
	void downloadsFinished();

signals:
	void doneGettingFiles();
	void doneGettingContentList();

private:
	QTimer *pNextImage;
	FileManagement fm;
	int iImageIndex;
	int iTimeout;
	QString path;
    bool bTurnOff;
	QLabel *lbImage;
    QString offTime;
    QString onTime;
    QUrl picXML;
    QMap<QString, QString> m_ImageList;

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QVideoWidget *videoWidget;

    FileDownload *m_contentList;
    FileDownload *m_imageFile;
};

#endif /* IMAGEPROCESSING_H_ */
