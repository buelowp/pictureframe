/*
 * FileDownload.cpp
 *
 *  Created on: Jul 14, 2016
 *      Author: pete
 */

#include "FileDownload.h"

FileDownload::FileDownload(QObject *parent) : QObject(parent)
{
    m_WebCtrl = new QNetworkAccessManager(this);
    m_reply = nullptr;
    m_isContentList = false;
    
	connect(m_WebCtrl, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileDownloaded(QNetworkReply*)));
	connect(m_WebCtrl, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)), this, SLOT(sslError(QNetworkReply*, const QList<QSslError>&)));
    connect(m_WebCtrl, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)), this, SLOT(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));
    connect(m_WebCtrl, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)), this, SLOT(authenticationRequired(QNetworkReply*, QAuthenticator*)));
    connect(m_WebCtrl, SIGNAL(encrypted(QNetworkReply*)), this, SLOT(encrypted(QNetworkReply*)));
    connect(m_WebCtrl, SIGNAL(preSharedKeyAuthenticationRequired(QNetworkReply*, QSslPreSharedKeyAuthenticator*)), this, SLOT(preSharedKeyAuthenticationRequired(QNetworkReply*, QSslPreSharedKeyAuthenticator*)));
    connect(m_WebCtrl, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), this, SLOT(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)));
    
}

FileDownload::~FileDownload()
{
}

void FileDownload::authenticationRequired(QNetworkReply*, QAuthenticator*)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::encrypted(QNetworkReply*)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::preSharedKeyAuthenticationRequired(QNetworkReply*, QSslPreSharedKeyAuthenticator*)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::sslError(QNetworkReply *reply, const QList<QSslError>&)
{
    qDebug() << __PRETTY_FUNCTION__ << ": SSL error";
	reply->ignoreSslErrors();
}

void FileDownload::downloadProgress(qint64 rcvd, qint64 total)
{
    qDebug() << __PRETTY_FUNCTION__ << "Got" << rcvd << "bytes, total received is" << total;
}

void FileDownload::getFile(QUrl imageUrl)
{
    QString p;
    
	qWarning() << __PRETTY_FUNCTION__ << ": Getting" << imageUrl;
	m_reply = m_WebCtrl->get(QNetworkRequest(imageUrl));
    
    if (imageUrl.fileName() == "contentlist.xml")
        m_isContentList = true;
    else
        m_isContentList = false;
    
    qDebug() << __PRETTY_FUNCTION__ << "m_reply" << p.sprintf("%08p", m_reply);
}

void FileDownload::fileDownloaded(QNetworkReply* pReply)
{
	if (pReply->error() == QNetworkReply::NoError) {
		m_DownloadedData = pReply->readAll();
        if (m_isContentList) {
            qDebug() << __PRETTY_FUNCTION__ << ": Got contentlist, emitting download complete signal";
            emit contentListFinished();
        }
        else {
            qDebug() << __PRETTY_FUNCTION__ << ": Got image, emitting download complete signal";
            emit downloadFinished();
        }
	}
	else {
		qDebug() << __PRETTY_FUNCTION__ << "Got an error reply";
		emit downloadError(pReply->error());
	}
	qDebug() << __PRETTY_FUNCTION__ << ": content size is" << m_DownloadedData.size();
	pReply->deleteLater();
    if (pReply == m_reply)
        qDebug() << __PRETTY_FUNCTION__ << ": slot reply and stored reply match";
    else
        qDebug() << __PRETTY_FUNCTION__ << ": slot reply and stored reply do not match";
    
    qDebug() << __PRETTY_FUNCTION__;
}

QByteArray FileDownload::getFileContents()
{
	return m_DownloadedData;
}

void FileDownload::encrypted()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::replyError(QNetworkReply::NetworkError)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::finished()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::metaDataChanged()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::preSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator*)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::redirectAllowed()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::redirected(const QUrl&)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::sslErrors(const QList<QSslError>&)
{
    qDebug() << __PRETTY_FUNCTION__;
}

