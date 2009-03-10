/*
JuffEd - A simple text editor
Copyright 2007-2009 Mikhail Murzin

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

#include "Log.h"
#include "Functions.h"

#include <QtCore/QFileInfo>
#include <QtGui/QAbstractButton>
#include <QtGui/QMessageBox>

namespace Juff {
//namespace Data {

Document::Document(const QString& fName) : QObject() {
	if ( fName.isEmpty() ) {
		static int i = 0;
		fileName_ = QString("Noname %1").arg(i++);
	}
	else {
		fileName_ = fName;
	}
	modCheckTimer_ = new QTimer(this);
	connect(modCheckTimer_, SIGNAL(timeout()), SLOT(checkLastModified()));
	if ( !isNoname(fName) && !isNull() ) {
		lastModified_ = QFileInfo(fName).lastModified();
		modCheckTimer_->start(1000);
	}
}

Document::~Document() {
}

bool Document::save(const QString&, QString&) {
	lastModified_ = QFileInfo(fileName_).lastModified();
	return true;
}

bool Document::isNull() const {
	return false;
}

QString Document::fileName() const {
	return fileName_;
}

void Document::setFileName(const QString& fileName) {
	JUFFENTRY;
	
	QString oldFileName = fileName_;
	
	if ( fileName_ != fileName ) {
		fileName_ = fileName;
		lastModified_ = QFileInfo(fileName).lastModified();
		if ( !modCheckTimer_->isActive() ) {
			modCheckTimer_->start(1000);
		}
		emit fileNameChanged(oldFileName);
	}
}

QString Document::type() const {
	return type_;
}

void Document::checkLastModified() {
	if ( !widget() || !widget()->isVisible() )
		return;
	
	QFileInfo fi(fileName_);
	if ( fi.exists() ) {
		if ( fi.lastModified() > lastModified_ ) {
			if ( checkingMutex_.tryLock() ) {
				QString question = tr("The file '%1' was modified by external program.").arg(getDocTitle(fileName_)) + "\n";
				question += tr("What do you want to do?");
				QMessageBox msgBox(QMessageBox::Question, tr("Warning"), question, 
							QMessageBox::Open | QMessageBox::Save | QMessageBox::Cancel, widget());
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
						lastModified_ = QFileInfo(fileName_).lastModified();
						break;
						
					case QMessageBox::Save:
					{
						//	Save
						QString err;
						save(fileName_, err);
						lastModified_ = QFileInfo(fileName_).lastModified();
					}
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


//}	//	namespace Data
}	//	namespace Juff
