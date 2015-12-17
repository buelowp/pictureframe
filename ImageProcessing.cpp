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
    bTurnOff = false;
}

ImageProcessing::~ImageProcessing() {
	// TODO Auto-generated destructor stub
}

bool ImageProcessing::init()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	int iTimeout = settings.value("Timeout", 5000).toInt();
    bTurnOff = settings.value("TurnOff", false).toBool();
    
    if (bTurnOff) {
        offTime = settings.value("OffTime", "17:00").toString();
        onTime = settings.value("OnTime", "7:00").toString();
    }

	pNextImage->setInterval(iTimeout);
	pNextImage->start();

	lbImage->setGeometry(0, 0, width(), height());
	lbImage->setAlignment(Qt::AlignCenter);

	if (!fm.init())
		return false;

	return true;
}

void ImageProcessing::timeout()
{
	QString nextImage = fm.next();
	QPixmap image(nextImage);

	if (image.width() > image.height())
		lbImage->setPixmap(image.scaledToWidth(width()));
	else
		lbImage->setPixmap(image.scaledToHeight(height()));
}

void ImageProcessing::showEvent(QShowEvent*)
{
	QString nextImage = fm.next();
	QPixmap image(nextImage);
	if (image.width() > image.height())
		lbImage->setPixmap(image.scaledToWidth(width()));
	else
		lbImage->setPixmap(image.scaledToHeight(height()));
}
