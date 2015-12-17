/*
 * ImageProcessing.h
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#ifndef IMAGEPROCESSING_H_
#define IMAGEPROCESSING_H_

#include <QMainWindow>
#include <QtGui>
#include <QtWidgets>
#include <QDebug>

#include "FileManagement.h"

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

protected:
	void showEvent(QShowEvent*);

private:
	QTimer *pNextImage;
	FileManagement fm;
	int iImageIndex;
	QString path;
    bool bTurnOff;
	QLabel *lbImage;
    QString offTime;
    QString onTime;
};

#endif /* IMAGEPROCESSING_H_ */
