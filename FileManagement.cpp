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
}

FileManagement::~FileManagement()
{
}

void FileManagement::reset()
{
}

bool FileManagement::getFiles()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QString fileSetting = QString("ImagePath%1").arg(m_FilePathIndex++);
	QStringList currList;
	QString path = settings.value(fileSetting).toString();
	QDir pImagePath;

	if (path.size()) {
		qWarning() << __FUNCTION__ << ": " << fileSetting << ": checking" << path;
		if (!pImagePath.cd(path))
			return false;
	}
	else
		return false;

	if (strOrderBy.compare("Name"), Qt::CaseInsensitive) 
		currList.append(pImagePath.entryList(QDir::Files|QDir::NoDotAndDotDot, QDir::Name));
	else if (strOrderBy.compare("Time"), Qt::CaseInsensitive)
		currList.append(pImagePath.entryList(QDir::Files|QDir::NoDotAndDotDot, QDir::Time|QDir::Reversed));
	else if (strOrderBy.compare("None"), Qt::CaseInsensitive)
		currList.append(pImagePath.entryList(QDir::Files|QDir::NoDotAndDotDot, QDir::NoSort));
	else
		currList.append(pImagePath.entryList(QDir::Files|QDir::NoDotAndDotDot, QDir::Time|QDir::Reversed));

	qWarning() << __FUNCTION__ << ": File list size is" << currList.size();

	for (int i = 0; i < currList.size(); i++) {
		QString fn = currList.at(i);
		m_fileList.append(pImagePath.absoluteFilePath(fn));
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
	QString randTest = settings.value("Random").toString();
	bIsRandom = settings.value("Random").toBool();
	strOrderBy = settings.value("OrderBy").toString();

	m_index = 0;
	m_FilePathIndex = 1;
	m_fileList.clear();
	qWarning() << __FUNCTION__ << ": list size is" << m_fileList.size() << "and m_index =" << m_index;

	while (getFiles())
		;

	std::srand(QDateTime::currentMSecsSinceEpoch());
	if (bIsRandom) {
		std::random_shuffle(m_fileList.begin(), m_fileList.end());
	}

	return true;
}

bool FileManagement::isComplete()
{
	if (m_index == 0 && m_fileList.size() == 0) {
		qWarning() << __FUNCTION__ << ": no content yet";
		return false;
	}
	if (m_index >= m_fileList.size()) {
		qWarning() << __FUNCTION__ << " : true";
		return true;
	}
	return false;
}

QString FileManagement::next()
{
	if (isComplete()) {
		qWarning() << __FUNCTION__ << ": running isComplete from next()";
		init();
	}

	return m_fileList[m_index++];
}
