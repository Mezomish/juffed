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
#include "Constants.h"
#include "Log.h"
#include "MainSettings.h"
#include "SearchResults.h"
//#include "Utils.h"

#include <QAbstractButton>
#include <QFileInfo>
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
		watcher_.addPath(fileName_);
	}
	codec_ = QTextCodec::codecForLocale();
	charset_ = codec_->name();
	
	connect(&watcher_, SIGNAL(fileChanged(const QString&)), SLOT(onModifiedExternally(const QString&)));
	
	notificationIsInProgress_ = false;
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

void Document::setFileName(const QString& newFileName) {
	QString oldName = fileName_;
	fileName_ = newFileName;
	
	watcher_.removePath(oldName);
	watcher_.addPath(fileName_);
	
	emit renamed(oldName);
}

QString Document::fileName() const {
	return fileName_;
}

QString Document::title() const {
	if ( isNoname() ) {
		return QObject::tr("Noname %1").arg(fileName().section(' ', 1, 1).toInt());
	}
	else {
		return QFileInfo(fileName()).fileName();
	}
}

QString Document::titleWithModification() const {
	return QString(isModified() ? "*" : "") + title();
}

bool Document::isNoname(const QString& fileName, bool* ok) {
	if ( fileName.isEmpty() ) {
		if ( ok != NULL ) {
			ok = false;
		}
		return true;
	}
	return fileName.left(6) == "Noname";
}

bool Document::isNoname() const {
	return isNoname(fileName());
}

QIcon Document::icon() const {
	if ( !isNoname() && !QFileInfo(fileName()).exists() ) {
		return QIcon(":doc_icon_warning");
	}
	else {
		return QIcon( (isModified() ? ":doc_icon_red" : ":doc_icon") );
	}
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
//	LOGGER;
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
	QString lang = MainSettings::get(MainSettings::Language);
	if ( !lang.isEmpty() && lang.compare("auto") != 0 ) {
		params << "-L" << lang.left(2);
	}

	QProcess enca;
	enca.start("enca", params);
	enca.waitForFinished();

	QString output = QString(enca.readAllStandardOutput()).simplified();
	if ( !output.isEmpty() ) {
		return mapCharset(output);
	}
	else {
		// test for BOM
		QFile file(fileName);
		if ( file.open(QFile::ReadOnly) ) {
			char buf[3];
			int len = file.read(buf, 3);
			
			if ( len == 3 ) {
				unsigned char* uBuf = reinterpret_cast<unsigned char*>(buf);
				if ( uBuf[0] == 0xEF && uBuf[1] == 0xBB && uBuf[2] == 0xBF ) {
					output = "UTF-8";
				}
				else if ( uBuf[0] == 0xFE && uBuf[1] == 0xFF && uBuf[2] == 0x00 ) {
					output = "UTF-16BE";
				}
				else if ( uBuf[0] == 0xFF && uBuf[1] == 0xFE && uBuf[2] == 0x00 ) {
					output = "UTF-16LE";
				}
			}
			file.close();
		}
		
		return output;
	}
}

void Document::setSearchResults(Juff::SearchResults* results) {
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
	return true;
}

void Document::onModifiedExternally(const QString& path) {
	LOGGER;
	if ( notificationIsInProgress_ ) {
		return;
	}
	
	notificationIsInProgress_ = true;
	if ( QFile::exists(path) ) {
		// file was modified
		
		QString question = tr("The file '%1' was modified by external program.").arg(title()) + "\n";
		question += tr("What do you want to do?");
		QMessageBox msgBox(QMessageBox::Question, tr("Warning"), question,
		                   QMessageBox::Open | QMessageBox::Save | QMessageBox::Cancel, this);
		QAbstractButton* btn = msgBox.button(QMessageBox::Save);
		if ( btn ) {
			btn->setText(tr("Save current"));
//			btn->setIcon(Utils::iconManager()->icon(FILE_SAVE));
		}
		btn = msgBox.button(QMessageBox::Open);
		if ( btn ) {
			btn->setText(tr("Reload from disk"));
//			btn->setIcon(Utils::iconManager()->icon(FILE_RELOAD));
		}
		btn = msgBox.button(QMessageBox::Cancel);
		if ( btn ) {
			btn->setText(tr("Ignore"));
		}
		
		int res = msgBox.exec();
		switch (res) {
			case QMessageBox::Open:
				// Reload
				reload();
				break;
		
			case QMessageBox::Save:
			{
				// Save
				QString err;
//				save(fileName_, charset(), err);
				save(err);
			}
			break;

		case QMessageBox::Cancel:
			// Nothing to do.
			break;

			default: ;
		}
	}
	else {
		// file was removed or renamed
		QString question = tr("The file '%1' was deleted or renamed.").arg(title()) + "\n";
		question += tr("What do you want to do?");
		QMessageBox msgBox(QMessageBox::Question, tr("Warning"), question, QMessageBox::Save | QMessageBox::Cancel, this);
		QAbstractButton* btn = msgBox.button(QMessageBox::Save);
		if ( 0 != btn ) {
			btn->setText(tr("Save current"));
//			btn->setIcon(Utils::iconManager()->icon(FILE_SAVE));
		}
		btn = msgBox.button(QMessageBox::Cancel);
		if ( 0 != btn ) {
			btn->setText(tr("Ignore"));
		}
		
		if ( msgBox.exec() == QMessageBox::Save ) {
			QString err;
			save(err);
		}
	}
	notificationIsInProgress_ = false;
}

}
