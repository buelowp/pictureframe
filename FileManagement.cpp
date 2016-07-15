/*
 * FileManagement.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#include "FileManagement.h"

FileManagement::FileManagement()
{
	m_index = 0;
	m_FilePathIndex = 1;

	// Create seed for the random
	// That is needed only once on application startup
	qsrand((uint)QTime::currentTime().msec());
}

FileManagement::~FileManagement()
{
}

bool FileManagement::getFiles()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QString fileSetting = QString("ImagePath%1").arg(m_FilePathIndex++);
	QString path = settings.value(fileSetting).toString();
	qWarning() << "fileSetting is" << fileSetting;
	qWarning() << "Set image path to be" << path;
	QDir *pImagePath;
	QStringList currList;

	if (path.size() == 0)
		return false;

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
		m_fileList.append(path + fn);
	}
	return true;
}

bool FileManagement::isValid()
{
	if (m_fileList.size() == 0)
		return false;

	return true;
}

bool FileManagement::init()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	bIsRandom = settings.value("Random").toBool();
	strOrderBy = settings.value("OrderBy").toString();

	m_FilePathIndex = 1;
	m_fileList.clear();
	while (getFiles())
		;

	if (m_fileList.size() == 0)
		return false;

	if (bIsRandom) {
		qDebug() << "Shuffling";
		std::random_shuffle(m_fileList.begin(), m_fileList.end());
	}

	return true;
}

QString FileManagement::next()
{
	if (m_index < m_fileList.size())
		return m_fileList[m_index++];
	else {
		init();
	}

	return m_fileList[m_index++];
}
