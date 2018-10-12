/*
 * FileManagement.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: pete
 */

#include "FileManagement.h"

/**
 * \fn FileManagement::FileManagement()
 * \brief Set file index to 0 so we always start from the beginning, and retrieve settings
 */
FileManagement::FileManagement()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
	QString randTest = settings.value("Random").toString();
	m_isRandom = settings.value("Random").toBool();
	m_orderBy = settings.value("OrderBy").toString();

    m_fileIndex = 0;
    m_localFiles.clear();
    updateLocalFileList();
}

FileManagement::~FileManagement()
{
}

/**
 * \fn int FileManagement::updateLocalFileList()
 * \brief Updates files available to the PictureViewer
 * \return Returns the total number of files available to retrieve
 */
int FileManagement::updateLocalFileList()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
    bool validPaths = true;
    int pathIndex = 1;
    QStringList nameFilters;
    
    nameFilters << "*.jpg" << "*.JPG" << "*.jpeg" << "*.JPEG" << "*.png" << "*.PNG";

    if (!m_localFiles.isEmpty())
        m_localFiles.clear();

    while (validPaths) {
        QString fileSetting = QString("ImagePath%1").arg(pathIndex++);
        QStringList currList;
        QString path = settings.value(fileSetting).toString();
        QDir pImagePath;

        if (path.size()) {
            qWarning() << __FUNCTION__ << ": " << fileSetting << ": checking" << path;
            if (!pImagePath.cd(path))
                validPaths = false;
        }
        else
            validPaths = false;

        if (m_orderBy.compare("Name"), Qt::CaseInsensitive) 
            currList.append(pImagePath.entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Name));
        else if (m_orderBy.compare("Time"), Qt::CaseInsensitive)
            currList.append(pImagePath.entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Time|QDir::Reversed));
        else if (m_orderBy.compare("None"), Qt::CaseInsensitive)
            currList.append(pImagePath.entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::NoSort));
        else
            currList.append(pImagePath.entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Time|QDir::Reversed));

        qWarning() << __FUNCTION__ << ": File list size is" << currList.size() << "in path" << path;

        for (int i = 0; i < currList.size(); i++) {
            QString fn = currList.at(i);
            m_localFiles.append(pImagePath.absoluteFilePath(fn));
        }
    }

    if (m_isRandom) {
		std::random_shuffle(m_localFiles.begin(), m_localFiles.end());
	}

	return m_localFiles.size();
}

/**
 * \fn bool FileManagement::nextFileInList(QString &fn)
 * \param fn Reference to a string from the caller to store the next file
 * \return Returns true on success, false if there are no files to retrieve
 * Checks to see if there are files to get, and whether we are at the end
 * of the list. Then, return the next file in the list
 */
bool FileManagement::nextFileInList(QString &fn)
{
    if (m_localFiles.size() == 0)
        return false;

    if (m_fileIndex >= m_localFiles.size())
        m_fileIndex = 0;

    qDebug() << __PRETTY_FUNCTION__ << ": index:" << m_fileIndex << ", file list size:" << m_localFiles.size();
    fn = m_localFiles.at(m_fileIndex++);
    return true;
}

bool FileManagement::deleteFile(QString &fn)
{
    if (QFile::exists(fn))
        return QFile::remove(fn);

    return true;
}

bool FileManagement::fileExists(QString fn)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Home", "PictureViewer");
    QString path = settings.value("ImagePath1").toString();
    QDir images(path);

    qDebug() << __PRETTY_FUNCTION__ << ": Checking" << path << "for file" << fn;
    if (images.exists()) {
        QStringList files = images.entryList(QDir::Files|QDir::NoDotAndDotDot);
        if (files.size()) {
            foreach (const QString &file, files) {
                if (file.contains(fn))
                    return true;
            }
        }
    }
    return false;
}
