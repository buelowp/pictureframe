/*
 * ImageProcessing.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#include "ImageProcessing.h"

ImageProcessing::ImageProcessing(QWidget *parent) : QMainWindow(parent) {
	iImageIndex = 0;

	pNextImage = new QTimer(this);
	connect(pNextImage, SIGNAL(timeout()), this, SLOT(timeout()));
	lbImage = new QLabel(this);
	lbImage->setGeometry(0, 0, 1024, 768);
}

ImageProcessing::~ImageProcessing() {
	// TODO Auto-generated destructor stub
}

bool ImageProcessing::init()
{
	QSettings settings("Home", "PictureViewer");
	int iTimeout = settings.value("Timeout", 5000).toInt();

	qDebug() << iTimeout;

	pNextImage->setInterval(iTimeout);
	pNextImage->start();

	if (!fm.init())
		return false;

	return true;
}

void ImageProcessing::timeout()
{
	QString nextImage = fm.next();
	QPixmap image(nextImage);
	lbImage->setPixmap(image.scaledToWidth(1024));
}

void ImageProcessing::showEvent(QShowEvent*)
{
	QString nextImage = fm.next();
	QPixmap image(nextImage);
	lbImage->setPixmap(image.scaledToWidth(1024));
}
