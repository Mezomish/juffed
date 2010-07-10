#include <QDebug>
/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

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

#include "AppInfo.h"
#include "Functions.h"
#include "IconManager.h"
#include "Log.h"
#include "SearchResults.h"

#include <QAbstractButton>
#include <QMessageBox>
#include <QProcess>
#include <QTextCodec>
#include <QTimer>

QString mapCharset(const QString& encaName) {
	if ( encaName == "windows-1251" ) {
		return "Windows-1251";
	}
	else if ( encaName == "IBM866" ) {
		return "IBM 866";
	}
	else {
		return encaName;
	}
}


namespace Juff {

int Document::sCount_ = 0;

Document::Document(const QString& fileName)
: QWidget()
//, clone_(NULL)
{
	if (fileName.isEmpty()) {
		fileName_ = QString("Noname %1").arg(sCount_++);
	}
	else {
		fileName_ = fileName;
	}
	codec_ = QTextCodec::codecForLocale();
	charset_ = codec_->name();
	
	
	modCheckTimer_ = new QTimer(this);
	connect(modCheckTimer_, SIGNAL(timeout()), SLOT(checkLastModified()));
	
	searchResults_ = NULL;
}

// This constructor is being used for creating clones
/*Document::Document(Juff::Document* doc) : QWidget() {
	doc->clone_ = this;
	this->clone_ = doc;
}

void Document::updateClone() {
	LOGGER;
	
	if ( clone_ != NULL ) {
		clone_->fileName_ = fileName_;
		clone_->charset_ = charset_;
		clone_->codec_ = codec_;
	}
}*/

Document::~Document() {
/*	if ( hasClone() ) {
		clone_->clone_ = NULL;
	}*/
}

QString Document::fileName() const {
	return fileName_;
}

bool Document::supportsAction(Juff::ActionID id) const {
	switch (id) {
		case FileClone : return false;
		default :        return true;
	}
}

void Document::setCodec(QTextCodec* codec) {
	codec_ = codec;
//	updateClone();
}

void Document::setCharset(const QString& charset) {
	LOGGER;
	QTextCodec* codec = QTextCodec::codecForName(charset.toAscii());
	if ( codec != 0 ) {
		QString oldCharset = charset_;
		codec_ = codec;
		charset_ = codec->name();
		
		// notify plugins
		emit charsetChanged(oldCharset);
	}
//	updateClone();
}

QString Document::guessCharset(const QString& fileName) {
	QStringList params;
	params << "-m" << fileName;
	if ( !AppInfo::language().isEmpty() ) {
		params << "-L" << AppInfo::language().left(2);
	}

	QProcess enca;
	enca.start("enca", params);
	enca.waitForFinished();

	QString output = QString(enca.readAllStandardOutput()).simplified();
	if ( !output.isEmpty() ) {
		return mapCharset(output);
	}
	else {
		return "";
	}
}

void Document::setSearchResults(Juff::SearchResults* results) {
	LOGGER;
	clearHighlighting();
	if ( searchResults_ != NULL )
		delete searchResults_;
	searchResults_ = results;
}

SearchResults* Document::searchResults() const {
	return searchResults_;
}


/*bool Document::tryToClose(QString& error) {
	// This method must NOT be called on Noname documents.
	// Set the doc's fileName before calling.
	LOGGER;
	
	if ( Juff::isNoname(this) ) {
		Log::warning("Document::tryToClose() must NOT be called on Noname documents");
		return false;
	}
	
	if ( isModified() ) {
		//	TODO : move this question to GUI
		QString str = tr("The document ") + fileName();
		str += tr(" has been modified.\nDo you want to save your changes?");
		int ret = QMessageBox::warning(this, tr("Close document"),
				str, QMessageBox::Save | QMessageBox::Discard
				| QMessageBox::Cancel, QMessageBox::Save);

		switch (ret) {
			case QMessageBox::Save:
				return save(error);

			case QMessageBox::Discard:
				return true;

			case QMessageBox::Cancel:
				return false;
		}
	}
	else
		return true;
}*/

bool Document::saveAs(const QString& fileName, QString& error) {
	QString oldName = fileName_;
	fileName_ = fileName;
	if ( save(error) ) {
		// notify plugins
		emit renamed(oldName);
		
//		updateClone();
		return true;
	}
	else {
		fileName_ = oldName;
		return false;
	}
}

bool Document::save(QString&) {
	LOGGER;
	lastModMutex_.lock();
	lastModified_ = QFileInfo(fileName_).lastModified();
	qDebug() << "'Last modified' from saved file:" << lastModified_;
	lastModMutex_.unlock();
	return true;
}

void Document::startCheckingTimer() {
	LOGGER;
	if ( !fileName_.isEmpty() && !Juff::isNoname(this) ) {
		lastModMutex_.lock();
		lastModified_ = QFileInfo(fileName_).lastModified();
		qDebug() << "'Last modified' from file:" << lastModified_;
		qDebug() << "'Last modified' saved    :" << QFileInfo(fileName_).lastModified();
		lastModMutex_.unlock();
		modCheckTimer_->start(1000);
	}
}

void Document::stopCheckingTimer() {
	LOGGER;
	modCheckTimer_->stop();
}

void Document::checkLastModified() {
//	LOGGER;
	QFileInfo fi(fileName_);
	if ( fi.exists() ) {
		lastModMutex_.lock();
		if ( fi.lastModified() > lastModified_ ) {
			
			qDebug() << "Current 'last modified'    :" << lastModified_;
			qDebug() << "Real file's 'last modified':" << fi.lastModified();
			
			if ( checkingMutex_.tryLock() ) {
				QString question = tr("The file '%1' was modified by external program.").arg(Juff::docTitle(this)) + "\n";
				question += tr("What do you want to do?");
				QMessageBox msgBox(QMessageBox::Question, tr("Warning"), question, 
						QMessageBox::Open | QMessageBox::Save | QMessageBox::Cancel, this);
				QAbstractButton* btn = msgBox.button(QMessageBox::Save);
				if ( btn ) {
					btn->setText(tr("Save current"));
					btn->setIcon(IconManager::instance()->icon(Juff::FileSave));
				}
				btn = msgBox.button(QMessageBox::Open);
				if ( btn ) {
					btn->setText(tr("Reload from disk"));
					btn->setIcon(IconManager::instance()->icon(Juff::FileReload));
				}
				btn = msgBox.button(QMessageBox::Cancel);
				if ( btn ) {
					btn->setText(tr("Ignore"));
				}

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
//						save(fileName_, charset(), err);
						save(err);
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
		lastModMutex_.unlock();
	}
}

}
