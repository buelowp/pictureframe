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
    m_path = settings.value("ImagePath").toString();

    m_fileIndex = 0;
}

FileManagement::~FileManagement()
{
}

void FileManagement::saveFile(QString file, QByteArray data)
{
    QString path = m_path + "/" + file;
    QFile destination(path);
    
    if (destination.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        destination.write(data);
    }
    else {
        qWarning() << __FUNCTION__ << ": Unable to open" << file;
    }
}

void FileManagement::setDeleteList(QSet<QString> list)
{
    m_deleteList = list;
    QSetIterator<QString> i(m_deleteList);
    while (i.hasNext()) {
        qDebug() << __FUNCTION__ << ":" << i.next();
    }
}

void FileManagement::cleanup()
{
    QSetIterator<QString> i(m_deleteList);
    while (i.hasNext()) {
        qDebug() << __FUNCTION__ << ": Deleting" << i.peekNext();
        deleteFile(i.next());
    }
}

/**
 * \fn int FileManagement::updateLocalFileList()
 * \brief Updates files available to the PictureViewer
 * \return Returns the total number of files available to retrieve
 */
int FileManagement::updateLocalFileList()
{
    QStringList nameFilters;
    QStringList currList;
    qDebug() << __FUNCTION__ << ":" << m_path;
    QDir imagePath(m_path);
    
    nameFilters << "*.jpg" << "*.JPG" << "*.jpeg" << "*.JPEG" << "*.png" << "*.PNG";

    cleanup();
    
    if (m_localFiles.size())
        m_localFiles.clear();

    if (imagePath.exists()) {
        imagePath.setNameFilters(nameFilters);
        
        if (m_orderBy.compare("Name"), Qt::CaseInsensitive) 
            currList = imagePath.entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Name);
        else if (m_orderBy.compare("Time"), Qt::CaseInsensitive)
            currList = imagePath.entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Time|QDir::Reversed);
        else if (m_orderBy.compare("None"), Qt::CaseInsensitive)
            currList = imagePath.entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::NoSort);
        else
            currList = imagePath.entryList(nameFilters, QDir::Files|QDir::NoDotAndDotDot, QDir::Time|QDir::Reversed);

        qDebug() << __FUNCTION__ << ": File list size is" << currList.size() << "in path" << m_path;

        for (int i = 0; i < currList.size(); i++) {
            QString fn = currList.at(i);
            m_localFiles.append(imagePath.absoluteFilePath(fn));
        }
    }

    if (m_isRandom) {
		std::random_shuffle(m_localFiles.begin(), m_localFiles.end());
	}

	m_fileIndex = 0;
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

    fn = m_localFiles.at(m_fileIndex++);
    return true;
}

void FileManagement::deleteFile(const QString file)
{
    QString path = m_path + "/" + file;
    
    if (QFile::exists(path))
        QFile::remove(path);
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

QStringList FileManagement::fileNameList()
{
    QStringList shortlist;
    
    for (int i = 0; i < m_localFiles.size(); i++) {
        QFileInfo fi(m_localFiles.at(i));
        shortlist.append(fi.fileName());
    }
    return shortlist;
}
