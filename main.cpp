/*
 * main.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */


#include <QtWidgets/QApplication>
#include <QtCore/QDebug>
#include <QtNetwork/QtNetwork>

#include "PictureFrame.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	PictureFrame display;
	int width, height;

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	width = settings.value("Width", 100).toInt();
	height = settings.value("Height", 100).toInt();

	display.setStyleSheet("QMainWindow {background: 'black';}");
	app.setOverrideCursor(QCursor(Qt::BlankCursor));
//    display.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
//    display.setWindowState(Qt::WindowFullScreen);
    display.showFullScreen();

   	return app.exec();
}

