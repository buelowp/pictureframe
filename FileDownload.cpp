/*
 * FileDownload.cpp
 *
 *  Created on: Jul 14, 2016
 *      Author: pete
 */

#include "FileDownload.h"

FileDownload::FileDownload(QObject *parent) : QObject(parent)
{
	connect(&m_WebCtrl, SIGNAL (finished(QNetworkReply*)), this, SLOT (fileDownloaded(QNetworkReply*)));
}

FileDownload::~FileDownload()
{
	// TODO Auto-generated destructor stub
}

void FileDownload::getFile(QUrl imageUrl)
{
	qWarning() << "FileDownload: Getting" << imageUrl;
	QNetworkRequest request(imageUrl);
	m_WebCtrl.get(request);
}

void FileDownload::fileDownloaded(QNetworkReply* pReply)
{
	if (pReply->error() == QNetworkReply::NoError) {
		qWarning() << "Got a successful reply";
		m_DownloadedData = pReply->readAll();
		emit downloaded();
	}
	else {
		qWarning() << "Got an error reply";
		emit downloadError(pReply->error());
	}
	pReply->deleteLater();
}

QByteArray FileDownload::downloadedData()
{
	return m_DownloadedData;
}
