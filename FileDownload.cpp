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
    m_WebCtrl->deleteLater();
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
    auto file = m_toDownload.begin();
    if (file != m_toDownload.end()) {
        emit progress(file.key(), rcvd, total);
    }
}

void FileDownload::fileDownloaded(QNetworkReply* pReply)
{
    QByteArray data;
    auto file = m_toDownload.begin();
    
    if (file == m_toDownload.end()) {
        qDebug() << __FUNCTION__ << ": Odd, we got a download done, but there is nothing in the queue";
        disconnect(m_reply, SIGNAL(downloadProgress(qint64, qint64)));
        pReply->deleteLater();
        return;
    }
	if (pReply->error() == QNetworkReply::NoError) {
		data = pReply->readAll();
        emit downloadFinished(file.key(), data);
    }
    else {
        qDebug() << __FUNCTION__ << ": Error downloading" << file.key();
        emit downloadError(file.key());
    }
    disconnect(m_reply, SIGNAL(downloadProgress(qint64, qint64)));
    pReply->deleteLater();
    m_toDownload.remove(file.key());
    startDownload();
}

void FileDownload::encrypted()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void FileDownload::replyError(QNetworkReply::NetworkError)
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

void FileDownload::addToDownloadList(QUrl url)
{
    qDebug() << __FUNCTION__ << ": Adding" << url << "to download list";
    m_toDownload[url.fileName()] = url;
    startDownload();
}

void FileDownload::addToDownloadList(QSet<QString> names, QMap<QString, QUrl> urls)
{
    QSetIterator<QString> i(names);
    while (i.hasNext()) {
        auto key = urls.find(i.next());
        if (key != urls.end()) {
            m_toDownload[key.key()] = key.value();
            qDebug() << __FUNCTION__ << ": Adding" << key.value() << "to download list";
        }
    }
    startDownload();
}

void FileDownload::startDownload()
{
    auto first = m_toDownload.begin();
    
    if (first != m_toDownload.end()) {
        qDebug() << __FUNCTION__ << ": Downloading" << first.value();
        m_reply = m_WebCtrl->get(QNetworkRequest(first.value()));
        connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
        emit downloading(first.key());
    }
    else {
        emit downloadsFinished();
    }
}

