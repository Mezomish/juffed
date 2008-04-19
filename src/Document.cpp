/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "Document.h"

Document::Document(const QString& fileName, DocView* view) : QObject(), fileName_(fileName), view_(view), modified_(false) {
	modCheckTimer_ = new QTimer(this);
	connect(modCheckTimer_, SIGNAL(timeout()), SLOT(checkLastModified()));
}

Document::~Document() {
	//	modCheckTimer_ will be automatically deleted
	//	by QObject class destructor, that's why
	//	here we just stop it. If I delete it by myself 
	//	there is a segfault here (I think a bug in Qt).
	modCheckTimer_->stop();

	if (view_ != 0)
		delete view_;
}

const QString& Document::fileName() const { 
	return fileName_; 
}

bool Document::isModified() const { 
	return modified_; 
}

void Document::setModified(bool mod) { 
	modified_ = mod; 
}

DocView* Document::view() const { 
	return view_; 
}
	
void Document::applySettings() {
}
	
void Document::extModMonitoringStart() {
	modCheckTimer_->start(1000);
}
	
void Document::extModMonitoringStop() {
	modCheckTimer_->stop();
}

void Document::setFileName(const QString& fileName) {
	if (fileName_.compare(fileName) != 0) {
		fileName_ = QFileInfo(fileName).canonicalFilePath();
		emit fileNameChanged();
	}
}

QDateTime Document::lastModified() const {
	return lastModified_;
}

void Document::setLastModified(const QDateTime& dt) {
	lastModified_ = dt;
}
	
void Document::checkLastModified() {
	QFileInfo fi(fileName_);
	if (fi.exists()) {
		if (fi.lastModified() > lastModified_) {
			if (checkingMutex_.tryLock()) {
				QString question(tr("The file was modified by external program\nWhat do you want to do?"));
				QMessageBox msgBox(QMessageBox::Question, tr("Warning"), question, 
							QMessageBox::Open | QMessageBox::Save | QMessageBox::Cancel, view_);
				QAbstractButton* btn = msgBox.button(QMessageBox::Save);
				if (btn != 0)
					btn->setText(tr("Save current"));
				btn = msgBox.button(QMessageBox::Open);
				if (btn != 0)
					btn->setText(tr("Reload from disk"));
				btn = msgBox.button(QMessageBox::Cancel);
				if (btn != 0)
					btn->setText(tr("Ignore"));
					
				int res = msgBox.exec();
				switch (res) {
					case QMessageBox::Open:
						//	Reload
						reload();
						break;
						
					case QMessageBox::Save:
						save();
						//	Save
						break;
						
					case QMessageBox::Cancel:
						//	Nothing to do. In this case we just make 
						//	local "check date" equal to file's real 
						//	"last modified date" on file system (to 
						//	prevent asking "What to do" again)
						lastModified_ = fi.lastModified();
						break;
					
					default: ;
				}
				checkingMutex_.unlock();
			}
		}
	}
}
	
