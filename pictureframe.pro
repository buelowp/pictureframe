CONFIG += qt debug c++14
QT += core gui widgets network

TARGET = pictureframe
TEMPLATE = app

OBJECTS_DIR = .obj
MOC_DIR = .moc
DEST_DIR = bin

SOURCES += main.cpp\
        PictureFrame.cpp \
        FileManagement.cpp  \
        FileDownload.cpp \
        images.cpp \
        progressdialog.cpp

HEADERS  += PictureFrame.h \
	FileManagement.h  \
	FileDownload.h \
	images.h \
	progressdialog.h

target.path = /usr/local/bin
INSTALLS += target

