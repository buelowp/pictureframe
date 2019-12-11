CONFIG += qt debug
QT += core gui widgets network

TARGET = pictureframe
TEMPLATE = app

OBJECTS_DIR = .obj
MOC_DIR = .moc
DEST_DIR = bin

SOURCES += main.cpp\
        PictureFrame.cpp \
        FileManagement.cpp  \
        FileDownload.cpp

HEADERS  += PictureFrame.h \
	FileManagement.h  \
	FileDownload.h

target.path = /usr/local/bin
INSTALLS += target

