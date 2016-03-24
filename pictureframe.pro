CONFIG += qt debug
QT += core gui widgets
QT += multimedia multimediawidgets

TARGET = pictureframe
TEMPLATE = app

SOURCES += main.cpp\
        ImageProcessing.cpp \
        FileManagement.cpp \
        VideoWidget.cpp

HEADERS  += ImageProcessing.h \
	FileManagement.h \
	VideoWidget.h
