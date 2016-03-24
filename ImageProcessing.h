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

#include "FileManagement.h"
#include "VideoWidget.h"

class ImageProcessing : public QMainWindow {
	Q_OBJECT
public:
	ImageProcessing(QWidget *parent = 0);
	virtual ~ImageProcessing();

	bool init();
	void menu();
	bool validateOptions();

public slots:
	void timeout();
	void displayErrorMessage();
	void mediaStatusChanged(QMediaPlayer::MediaStatus);
	void mediaStateChanged(QMediaPlayer::State);
	void durationChanged(qint64);
	void positionChanged(qint64);
	void metaDataChanged();

protected:
	void showEvent(QShowEvent*);

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

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    VideoWidget *videoWidget;
};

#endif /* IMAGEPROCESSING_H_ */
