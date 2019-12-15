/*
 * Copyright (c) 2019 Peter Buelow <email>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "progressdialog.h"

ProgressDialog::ProgressDialog(QDialog *parent) : QDialog(parent)
{
    m_fileName = new QLabel();
    m_progress = new QLabel();
    m_layout = new QGridLayout(this);
    m_layout->addWidget(m_fileName, 0, 0);
    m_layout->addWidget(m_progress, 1, 0);
    
    setLayout(m_layout);
}

ProgressDialog::~ProgressDialog()
{
}

void ProgressDialog::setProgress(qint64 bytes, qint64 size)
{
    m_progress->setText(QString("%1 bytes of %2").arg(bytes).arg(size));
}

void ProgressDialog::setTitle(QString title)
{
    m_fileName->setText("Downloading " + title);
}
