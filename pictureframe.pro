CONFIG += qt debug
QT += core gui widgets
QT += multimedia multimediawidgets

TARGET = pictureframe
TEMPLATE = app

SOURCES += main.cpp\
        ImageProcessing.cpp \
        FileManagement.cpp  \
        FileDownload.cpp

HEADERS  += ImageProcessing.h \
	FileManagement.h  \
	FileDownload.h
