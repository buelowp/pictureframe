/*
 * FileDownload.h
 *
 *  Created on: Jul 14, 2016
 *      Author: pete
 */

#ifndef __FILEDOWNLOAD_H_
#define __FILEDOWNLOAD_H_

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>

/**
 * \class FileDownload
 * Provides a Qt download class to get image and content files from the server
 */
class FileDownload : public QObject 
{
	Q_OBJECT
public:
	FileDownload(QObject *parent = nullptr);
	virtual ~FileDownload();

	void addToDownloadList(QUrl url);
    void addToDownloadList(QSet<QString> names, QMap<QString, QUrl> urls);
    void startDownload();

signals:
    void contentListComplete(QByteArray data);
	void downloadFinished(QString file, QByteArray data);
	void downloadError(QString file);
    void progress(QString file, qint64 percent, qint64 total);
    void downloadsFinished();
    void downloading(QString file);

public slots:
	void fileDownloaded(QNetworkReply*);
	void sslError(QNetworkReply*, const QList<QSslError>&);
    void authenticationRequired(QNetworkReply*, QAuthenticator*);
    void encrypted(QNetworkReply*);
    void networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility );
    void preSharedKeyAuthenticationRequired(QNetworkReply*, QSslPreSharedKeyAuthenticator*);
    void proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*);
    void downloadProgress(qint64, qint64);
    void encrypted();
    void replyError(QNetworkReply::NetworkError);
    void metaDataChanged();
    void preSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator*);
    void redirectAllowed();
    void redirected(const QUrl&);
    void sslErrors(const QList<QSslError>&);
    
private:
	QNetworkAccessManager *m_WebCtrl;
	QByteArray m_DownloadedData;
    QMap<QString, QUrl> m_toDownload;
};

#endif /* FILEDOWNLOAD_H_ */
