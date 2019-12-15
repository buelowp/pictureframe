/*
 * FileManagement.h
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#ifndef FILEMANAGEMENT_H_
#define FILEMANAGEMENT_H_

#include <QtCore/QtCore>
#include <QtCore/QDebug>
#include <cstdlib>
#include <algorithm>

/**
 * \class FileManagement
 * \brief Contains and manages the file list to be displayed
 */
class FileManagement {
public:
	FileManagement();
	virtual ~FileManagement();

    void saveFile(QString file, QByteArray data);
    void setDeleteList(QSet<QString> list);
    void cleanup();
	bool nextFileInList(QString&);
	int updateLocalFileList();
    bool fileExists(QString);
    QStringList localFiles() { return m_localFiles; }
    QStringList fileNameList();

private:
    void deleteFile(const QString file);

    QStringList m_localFiles;        /**< QStringList holding all currently available files */
	bool m_isRandom;               /**< boolean indicating whether list is randomized */
	int m_fileIndex;               /**< integer index for next available file in list */
	QString m_orderBy;             /**< QString indicating ordering */
	QString m_path;
    QSet<QString> m_deleteList;
};

#endif /* FILEMANAGEMENT_H_ */
