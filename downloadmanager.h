/*
 * Copyright (c) 2019 Peter Buelow <email>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __PF_IMAGES_H
#define __PF_IMAGES_H

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>

#include "FileDownload.h"

class DownloadManager : public QObject
{
    Q_OBJECT

public:
    DownloadManager(QObject* parent = nullptr);
    ~DownloadManager();
    
    void addToDownloadList(QSet<QString> index, QMap<QString, QUrl> potentials);
    void addToDownloadList(QUrl url);
    
signals:
    void newFileMap(QMap<QString, QUrl>);
    void contentListError();
    void downloadError(QString file);
    void downloadProgress(QString file, qint64 percent, qint64 size);
    void downloadComplete(QString file, QByteArray data);
    void allDownloadsComplete();
    void downloadStarted(QString file);
    
public slots:
    void checkForNewContent(QUrl url);
	void contentListComplete(QString, QByteArray);
    void fileDownloadComplete(QString, QByteArray);
	void contentListDownloadError();
    void fileDownloadError(QString);
    void progress(QString file, qint64, qint64);
    void downloadsFinished();
    void newDownloadStarted(QString file);
    
private:
    FileDownload *m_contentList;
    FileDownload *m_fileDownloads;
};

#endif // __PF_IMAGES_H
