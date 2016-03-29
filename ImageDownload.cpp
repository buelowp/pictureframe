/*
 * ImageDownload.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: pete
 */

#include "ImageDownload.h"

ImageDownload::ImageDownload(QObject *parent) : QObject(parent)
{
    connect(&manager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
}

ImageDownload::~ImageDownload()
{
}

void ImageDownload::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    if (reply->error()) {
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(reply->errorString()));
    } else {
        QString filename = saveFileName(url);
        if (saveToDisk(filename, reply))
            printf("Download of %s succeeded (saved to %s)\n",
                   url.toEncoded().constData(), qPrintable(filename));
    }

    currentDownloads.removeAll(reply);
    reply->deleteLater();

    if (currentDownloads.isEmpty())
        // all downloads finished
        QCoreApplication::instance()->quit();
}

bool ImageDownload::saveToDisk(const QString &filename, QIODevice *data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }

    file.write(data->readAll());
    file.close();

    return true;
}

QString ImageDownload::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    if (QFile::exists(basename)) {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }

    return basename;
}

void ImageDownload::doImageDownload(const QUrl &url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif

    currentDownloads.append(reply);
}

bool ImageDownload::parseXML(QString xmlFile)
{
	QFile inputFile(xmlFile);

	if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return false;
	}

	QXmlStreamReader reader(&inputFile);

	while (!reader.atEnd()) {
		reader.readNext();
        if (reader.error())
        {
        	qDebug() << "Error:" << reader.errorString() << "with file" << xmlFile << "at line" << QString::number(reader.lineNumber());
        	return false;
        }
	}

}
