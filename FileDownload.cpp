/*
 * FileDownload.cpp
 *
 *  Created on: Jul 14, 2016
 *      Author: pete
 */

#include "FileDownload.h"

FileDownload::FileDownload(QObject *parent) : QObject(parent)
{
	connect(&m_WebCtrl, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileDownloaded(QNetworkReply*)));
	connect(&m_WebCtrl, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)),
			this, SLOT(sslError(QNetworkReply*, const QList<QSslError>&)));
}

FileDownload::~FileDownload()
{
}

void FileDownload::sslError(QNetworkReply *reply, const QList<QSslError>&)
{
    qDebug() << __PRETTY_FUNCTION__ << ": SSL error";
	reply->ignoreSslErrors();
}

void FileDownload::getFile(QUrl imageUrl)
{
	qWarning() << __FUNCTION__ << ": Getting" << imageUrl;
	QNetworkRequest request(imageUrl);
	m_WebCtrl.get(request);
}

void FileDownload::fileDownloaded(QNetworkReply* pReply)
{
	if (pReply->error() == QNetworkReply::NoError) {
		m_DownloadedData = pReply->readAll();
		qWarning() << __FUNCTION__ << ": Got a file, emitting downloaded message";
		emit downloadFinished();
	}
	else {
		qWarning() << __FUNCTION__ << "Got an error reply";
		emit downloadError(pReply->error());
	}
	pReply->deleteLater();
}

QByteArray FileDownload::getFileContents()
{
	return m_DownloadedData;
}
