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
	fileGetterIndex = 1;

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

bool FileManagement::getFiles()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QString fileSetting = QString("ImagePath%1").arg(fileGetterIndex);
	QString path = settings.value(fileSetting).toString();
	qWarning() << "fileSetting is" << fileSetting;
	qWarning() << "Set image path to be" << path;
	QDir *pImagePath;
	QStringList currList;

	if (path.size() == 0)
		return false;

	fileGetterIndex++;
	pImagePath = new QDir(path);

	if (!pImagePath->exists()) {
		qDebug() << "Could not find" << path;
		return false;
	}

	if (strOrderBy.compare("Name"), Qt::CaseInsensitive) 
		currList.append(pImagePath->entryList(QDir::Files, QDir::Name));
	else if (strOrderBy.compare("Time"), Qt::CaseInsensitive)
		currList.append(pImagePath->entryList(QDir::Files, QDir::Time|QDir::Reversed));
	else if (strOrderBy.compare("None"), Qt::CaseInsensitive)
		currList.append(pImagePath->entryList(QDir::Files, QDir::NoSort));
	else
		currList.append(pImagePath->entryList(QDir::Files, QDir::Time|QDir::Reversed));

	for (int i = 0; i < currList.size(); i++) {
		QString fn = currList.at(i);
		pFileList.append(path + fn);
	}
	return true;
}

bool FileManagement::init()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	bIsRandom = settings.value("Random").toBool();
	strOrderBy = settings.value("OrderBy").toString();

	while (getFiles())
		;

	if (pFileList.size() == 0)
		return false;

	if (bIsRandom) {
		qDebug() << "Shuffling";
		std::random_shuffle(pFileList.begin(), pFileList.end());
	}

	bValid = true;
	return true;
}

QString FileManagement::nextByDate()
{
	if (iIndex < pFileList.size())
		return pFileList[iIndex++];
	else {
		iIndex = 0;
		pFileList.clear();
		fileGetterIndex = 1;
		init();
	}

	return pFileList[iIndex++];
}

QString FileManagement::nextRandom()
{
	int index;

	while ((index = rand() % pFileList.size()) == iLastRandIndex)
		;

	iLastRandIndex = index;

	return pFileList[index];
}

QString FileManagement::next()
{
	return nextByDate();
}
