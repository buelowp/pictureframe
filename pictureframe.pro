CONFIG += qt debug c++14
QT += core gui widgets network multimedia multimediawidgets

TARGET = pictureframe
TEMPLATE = app

OBJECTS_DIR = .obj
MOC_DIR = .moc
DEST_DIR = bin

SOURCES += main.cpp\
        PictureFrame.cpp \
        FileManagement.cpp  \
        FileDownload.cpp \
        downloadmanager.cpp \
        progressdialog.cpp \
        videowidget.cpp

HEADERS  += PictureFrame.h \
	FileManagement.h  \
	FileDownload.h \
	downloadmanager.h \
	progressdialog.h \
	videowidget.h

target.path = /usr/local/bin
INSTALLS += target

