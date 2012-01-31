#include "DocWrapper.h"
#include "NullDoc.h"
#include "Log.h"
#include "Constants.h"

void DocWrapper::setDoc(Juff::Document* doc) {
	doc_ = ( doc == NULL ? NullDoc::instance() : doc );
}

bool DocWrapper::usesTabs() {
	return EditorSettings::get(EditorSettings::UseTabs);
}

int DocWrapper::tabWidth() {
	return EditorSettings::get(EditorSettings::TabWidth);
}

QString DocWrapper::fileName() {
	if ( !doc_->isNull() && !doc_->isNoname() ) {
		return doc_->fileName();
	}
	else {
		return "";
	}
}

long DocWrapper::selectionStart() {
	if ( doc_->isNull() ) {
		return 0;
	}
	else {
		return doc_->positionGetSelectionStart();
	}
}

long DocWrapper::selectionEnd() {
	if ( doc_->isNull() ) {
		return 0;
	}
	else {
		return doc_->positionGetSelectionEnd();
	}
}

void DocWrapper::createSelection(const QVariant& start, const QVariant& end) {
//	LOGGER;
	if ( !doc_->isNull() ) {
		doc_->positionSetSelection(start.toInt(), end.toInt());
	}
}

void DocWrapper::moveCursor(const QVariant& pos) {
	if ( !doc_->isNull() ) {
		doc_->positionSetCursor(pos.toInt());
	}
}

long DocWrapper::cursorPosition() {
	if ( !doc_->isNull() ) {
		return doc_->positionGetCursor();
	}
	else {
		return 0;
	}
}

QString DocWrapper::selectedText() {
	if ( !doc_->isNull() ) {
		QString text;
		if ( doc_->getSelectedText(text) ) {
			return text;
		}
	}
	return "";
}

QString DocWrapper::text() {
	if ( !doc_->isNull() ) {
		QString text;
		if ( doc_->getText(text) ) {
			return text;
		}
	}
	return "";
}

QString DocWrapper::currentLineText() {
	if ( !doc_->isNull() ) {
		QString text;
		int row, col;
		if ( doc_->getCursorPos(row, col) && doc_->getTextLine(row, text) ) {
//			return text.remove(LineSeparatorRx);
			return text;
		}
	}
	return "";
}

int DocWrapper::currentLineLength() {
	return currentLineText().length();
}

long DocWrapper::currentLinePosition() {
	if ( !doc_->isNull() ) {
		int row, col;
		if ( doc_->getCursorPos(row, col) ) {
			return cursorPosition() - col;
		}
	}
	return 0;
}

void DocWrapper::replaceContent(const QVariant& value, const QVariant& start, const QVariant& end, const QVariant& no_indent) {
	if ( doc_->isNull() ) {
		return;
	}
	
	Q_UNUSED(no_indent);
	QString str = value.toString();
	createSelection(start, end);
	doc_->replaceSelectedText(str);
}

int DocWrapper::eol() {
	if ( doc_->isNull() ) {
		return -1;
	}
	
	switch ( doc_->eol() ) {
		case Juff::Document::EolWin : return 0;
		case Juff::Document::EolMac : return 1;
		case Juff::Document::EolUnix : return 2;
		default : return -1;
	}
}

//void DocWrapper::abr() {
//	qDebug("????");
//}
