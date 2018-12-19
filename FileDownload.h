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
class FileDownload : public QObject {
	Q_OBJECT
public:
	FileDownload(QObject *parent = 0);
	virtual ~FileDownload();
	QByteArray getFileContents();
	void getFile(QUrl imageUrl);

signals:
	void downloadFinished();
    void contentListFinished();
	void downloadError(QNetworkReply::NetworkError);

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
    void finished();
    void metaDataChanged();
    void preSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator*);
    void redirectAllowed();
    void redirected(const QUrl&);
    void sslErrors(const QList<QSslError>&);
    
private:
	QNetworkAccessManager *m_WebCtrl;
    QNetworkReply *m_reply;
	QByteArray m_DownloadedData;
    bool m_isContentList;
};

#endif /* FILEDOWNLOAD_H_ */
