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

class FileManagement {
public:
	FileManagement();
	virtual ~FileManagement();

	QString next();
	bool init();
	bool isValid();

private:
	bool getFiles();

	QString parentPath;
	QStringList m_fileList;
	bool bIsRandom;
	int m_index;
	QString strOrderBy;
	QVector<int> randomOrder;
	int m_FilePathIndex;
};

#endif /* FILEMANAGEMENT_H_ */
