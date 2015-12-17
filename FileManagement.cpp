/*
 * FileManagement.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#include "FileManagement.h"

FileManagement::FileManagement()
{
	bValid = false;
	iIndex = 0;
	iLastRandIndex = 0;
	pImagePath = NULL;

	srand (time(NULL));
	// Create seed for the random
	// That is needed only once on application startup
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());
}

FileManagement::~FileManagement()
{
}

int FileManagement::randInt(int low, int high)
{
	// Random number between low and high
	return qrand() % ((high + 1) - low) + low;
}

bool FileManagement::init()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	bIsRandom = settings.value("Random").toBool();
	strOrderBy = settings.value("OrderBy").toString();
	QString path = settings.value("ImagePath").toString();

	pImagePath = new QDir(path);

	if (!pImagePath->exists()) {
		qDebug() << "Could not find" << path;
		return false;
	}

	if (strOrderBy.compare("Name"), Qt::CaseInsensitive)
		pFileList = pImagePath->entryList(QDir::Files, QDir::Name);
	else if (strOrderBy.compare("Time"), Qt::CaseInsensitive)
		pFileList = pImagePath->entryList(QDir::Files, QDir::Time|QDir::Reversed);
	else if (strOrderBy.compare("None"), Qt::CaseInsensitive)
		pFileList = pImagePath->entryList(QDir::Files, QDir::NoSort);
	else
		pFileList = pImagePath->entryList(QDir::Files, QDir::Time|QDir::Reversed);


	if (bIsRandom) {
		std::random_shuffle(pFileList.begin(), pFileList.end());
	}

	if (pFileList.size() == 0)
		return false;

	bValid = true;
	return true;
}

QString FileManagement::nextByDate()
{
	if (iIndex < pFileList.size())
		return pImagePath->filePath(pFileList[iIndex++]);
	else {
		iIndex = 0;
		if (bIsRandom)
			std::random_shuffle(pFileList.begin(), pFileList.end());
	}

	return pImagePath->filePath(pFileList[iIndex++]);
}

QString FileManagement::nextRandom()
{
	int index;

	while ((index = rand() % pFileList.size()) == iLastRandIndex)
		;

	iLastRandIndex = index;

	return pImagePath->filePath(pFileList[index]);
}

QString FileManagement::next()
{
	if (bIsRandom) {
		return next();
	}

	return nextByDate();
}
