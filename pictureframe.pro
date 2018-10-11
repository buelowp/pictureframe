CONFIG += qt debug
QT += core gui widgets network

TARGET = pictureframe
TEMPLATE = app

SOURCES += main.cpp\
        PictureFrame.cpp \
        FileManagement.cpp  \
        FileDownload.cpp

HEADERS  += PictureFrame.h \
	FileManagement.h  \
	FileDownload.h

target.path = /usr/local/bin
INSTALLS += target

