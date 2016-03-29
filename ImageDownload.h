/*
 * ImageDownload.h
 *
 *  Created on: Mar 29, 2016
 *      Author: pete
 */

#ifndef IMAGEDOWNLOAD_H_
#define IMAGEDOWNLOAD_H_

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>

class ImageDownload : public QObject {
	Q_OBJECT
public:
	ImageDownload(QObject *parent = 0);
	virtual ~ImageDownload();

	bool getImageList();
	void getNewImages();
	void doXMLDownload(const QUrl &);
	void doImageDownload(const QUrl &);

public slots:
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &errors);

private:
    bool saveToDisk(const QString &, QIODevice *);
    QString saveFileName(const QUrl &);
    bool parseXML(QString xmlFile);

    bool parseImageList();
    QNetworkAccessManager manager;
    QList<QNetworkReply *> currentDownloads;
    QStringList imageFiles;
};

#endif /* IMAGEDOWNLOAD_H_ */
