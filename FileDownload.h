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

class FileDownload {
	Q_OBJECT
public:
	FileDownload(QObject *parent = 0);
	virtual ~FileDownload();
	QByteArray downloadedData();
	void getFile(QUrl imageUrl);

signals:
	void downloaded();
	void downloadError();

private slots:
	void fileDownloaded(QNetworkReply *pReply);

private:
	QNetworkAccessManager m_WebCtrl;
	QByteArray m_DownloadedData;
};

#endif /* FILEDOWNLOAD_H_ */
