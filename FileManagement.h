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
	bool isValid() { return bValid; }

private:
	QString nextRandom();
	QString nextByDate();
	int randInt(int low, int high);
	bool getFiles();

	QString parentPath;
	QStringList pFileList;
	bool bValid;
	bool bIsRandom;
	int iIndex;
	int iLastRandIndex;
	QString strOrderBy;
	QVector<int> randomOrder;
	int fileGetterIndex;
};

#endif /* FILEMANAGEMENT_H_ */
