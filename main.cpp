/*
 * main.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */


#include <QApplication>
#include <QDebug>
#include "ImageProcessing.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	ImageProcessing display;

	QSettings settings("Home", "PictureViewer");
	settings.setValue("Random", true);
	settings.setValue("OrderBy", "time");
	settings.setValue("ImagePath", "/Users/pete/Desktop/pictureframe/");
	settings.setValue("Timeout", 5000);

	display.setMinimumSize(1024, 768);
	display.setStyleSheet("QMainWindow {background: 'black';}");
	if (display.init())
		display.show();
	else
		qDebug() << "display init failed";

	return app.exec();
}

