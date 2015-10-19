QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pictureframe
TEMPLATE = app

SOURCES += main.cpp\
        ImageProcessing.cpp \
        FileManagement.cpp

HEADERS  += ImageProcessing.h \
			FileManagement.h
