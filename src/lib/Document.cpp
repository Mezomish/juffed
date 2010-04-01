#include "Document.h"

#include "AppInfo.h"
#include "Functions.h"
#include "Log.h"

//#include <QMessageBox>
#include <QProcess>
#include <QTextCodec>

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
	fileName_ = fileName;
//	updateClone();
	save(error);
}

}

