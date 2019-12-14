/*
 * Copyright (c) 2019 Peter Buelow <email>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "images.h"

Images::~Images()
{
}

Images::Images(QObject* parent) : QObject(parent)
{
    m_contentList = new FileDownload(this);
    m_fileDownloads = new FileDownload(this);
    connect(m_contentList, SIGNAL(downloadFinished(QString, QByteArray)), this, SLOT(contentListComplete(QString, QByteArray)));
    connect(m_contentList, SIGNAL(downloadError(QString)), this, SLOT(contentListDownloadError()));
    connect(m_fileDownloads, SIGNAL(downloadFinished(QString, QByteArray)), this, SLOT(fileDownloadComplete(QString, QByteArray)));
    connect(m_fileDownloads, SIGNAL(downloadError(QString)), this, SLOT(fileDownloadError(QString)));
    connect(m_fileDownloads, SIGNAL(progress(QString, qint64, qint64)), this, SLOT(progress(QString, qint64, qint64)));
    connect(m_fileDownloads, SIGNAL(downloadsFinished()), this, SLOT(downloadsFinished()));
    connect(m_fileDownloads, SIGNAL(downloading(QString)), this, SLOT(newDownloadStarted(QString)));
}

void Images::downloadsFinished()
{
    emit allDownloadsComplete();
}

void Images::fileDownloadComplete(QString file, QByteArray data)
{
    emit downloadComplete(file, data);
}

void Images::checkForNewContent(QUrl url)
{
    m_contentList->addToDownloadList(url);
}

void Images::fileDownloadError(QString file)
{
    qDebug() << __FUNCTION__ << ": Error downloading " << file;
    emit downloadError(file);
}

void Images::progress(QString file, qint64 percent, qint64 total)
{
    emit downloadProgress(file, percent, total);
}

void Images::contentListComplete(QString, QByteArray data)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QString path = settings.value("ImagePath1").toString();
    QMap<QString, QUrl> imageList;
	QXmlStreamReader doc;

    qDebug() << __PRETTY_FUNCTION__ << ": got a new content list xml";
	
    doc.addData(data);
	if (doc.hasError()) {
		qWarning() << __FUNCTION__ << ": Downloaded xml has errors";
		return;
	}
	else {
		while (!doc.atEnd() && !doc.hasError()) {
			QXmlStreamReader::TokenType token = doc.readNext();
			if (token == QXmlStreamReader::StartDocument)
				continue;

			if (token == QXmlStreamReader::StartElement) {
				if (doc.name() == "Url") {
					QUrl url(doc.readElementText());
					if (url.isValid()) {
						imageList[url.fileName()] = url;
					}
				}
			}
		}
		if (!imageList.empty()) {
            emit newFileMap(imageList);
        }
	}
}

void Images::addToDownloadList(QSet<QString> index, QMap<QString, QUrl> potentials)
{
    m_fileDownloads->addToDownloadList(index, potentials);
}

void Images::addToDownloadList(QUrl url)
{
    m_contentList->addToDownloadList(url);
}

void Images::contentListDownloadError()
{
    emit contentListError();
}

void Images::newDownloadStarted(QString file)
{
    qDebug() << __FUNCTION__ << file;
    emit downloadStarted(file);
}

