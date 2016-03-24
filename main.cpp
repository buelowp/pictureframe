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
	int width, height;

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	width = settings.value("Width", 100).toInt();
	height = settings.value("Height", 100).toInt();

	qWarning() << "width:" << width << ", height:" << height;

//	display.setMinimumSize(width, height);
	display.setStyleSheet("QMainWindow {background: 'black';}");
	app.setOverrideCursor(QCursor(Qt::BlankCursor));
//    display.showMaximized();
	display.setFixedSize(QSize(width, height));

    if (display.init())
		display.show();
	else
		qWarning() << "display init failed";

	return app.exec();
}

