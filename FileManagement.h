/*
 * FileManagement.h
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#ifndef FILEMANAGEMENT_H_
#define FILEMANAGEMENT_H_

#include <QtCore>
#include <QDebug>
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
	int randInt(int low, int high)

	QString parentPath;
	QDir *pImagePath;
	QStringList pFileList;
	bool bValid;
	bool bIsRandom;
	int iIndex;
	int iLastRandIndex;
	QString strOrderBy;
	QVector<int> randomOrder;
};

#endif /* FILEMANAGEMENT_H_ */