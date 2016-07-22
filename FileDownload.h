/*
 * FileDownload.h
 *
 *  Created on: Jul 14, 2016
 *      Author: pete
 */

#ifndef FILEDOWNLOAD_H_
#define FILEDOWNLOAD_H_

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>

class FileDownload : public QObject {
	Q_OBJECT
public:
	FileDownload(QObject *parent = 0);
	virtual ~FileDownload();
	QByteArray downloadedData();
	void getFile(QUrl imageUrl);

signals:
	void downloaded();
	void downloadError(QNetworkReply::NetworkError);

private slots:
	void fileDownloaded(QNetworkReply *pReply);
	void sslError(QNetworkReply *reply, const QList<QSslError>&);

private:
	QNetworkAccessManager m_WebCtrl;
	QByteArray m_DownloadedData;
};

#endif /* FILEDOWNLOAD_H_ */
