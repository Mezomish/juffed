#include <QDebug>

#include "SearchEngine.h"

#include "DocHandlerInt.h"
#include "Document.h"
#include "JuffMW.h"
#include "Log.h"

void stepOver(Juff::Document* doc, bool forward);

SearchEngine::SearchEngine(Juff::DocHandlerInt* handler, JuffMW* mw) : QObject() {
	handler_ = handler;
	mw_ = mw;

	connect(mw_, SIGNAL(searchRequested(const Juff::SearchParams&)), SLOT(onSearchRequested(const Juff::SearchParams&)));
}

void SearchEngine::keepVariables(Juff::Document* doc) {
	if ( doc->fileName() != fileName_ ) {
		steppedOver_ = false;
		doc->getCursorPos(startLine_, startCol_);
		fileName_ = doc->fileName();
	}
}

void SearchEngine::clearSelection(Juff::Document* doc) {
	if ( doc->hasSelectedText() ) {
		int line1, col1, line2, col2;
		doc->getSelection(line1, col1, line2, col2);
		if ( params_.backwards )
			doc->setCursorPos(line1, col1);
		else
			doc->setCursorPos(line2, col2);
	}
}

void SearchEngine::find(Juff::Document* doc) {
	mw_->showFindDialog();

	keepVariables(doc);
}

void SearchEngine::findNext(Juff::Document* doc) {
	// in case the document is different
	keepVariables(doc);
	params_.backwards = false;

	if ( !startFind(doc) ) {
		stepOver(doc, true);
		if ( !startFind(doc) ) {
			mw_->message(QIcon(), "", tr("Line '%1' was not found").arg(params_.findWhat));
			return;
		}
	}
}

void SearchEngine::findPrev(Juff::Document* doc) {
	// in case the document is different
	keepVariables(doc);
	params_.backwards = true;

	if ( !startFind(doc) ) {
		stepOver(doc, false);
		if ( !startFind(doc) ) {
			mw_->message(QIcon(), "", tr("Text '%1' was not found").arg(params_.findWhat));
			return;
		}
	}
}


void SearchEngine::onSearchRequested(const Juff::SearchParams& params) {
	LOGGER;

	Juff::Document* doc = handler_->curDoc();
	if ( doc->isNull() ) return;

	params_ = params;
	if ( params.backwards )
		findPrev(doc);
	else
		findNext(doc);
}

int SearchEngine::findAt(const QString& line, bool forward, int& length) {
	QString str = params_.findWhat;
	QRegExp regExp;
	if ( params_.wholeWords ) {
		regExp = QRegExp(QString("\\b%1\\b").arg(QRegExp::escape(str)));
	}
	else
		regExp = QRegExp(str);
	regExp.setCaseSensitivity(params_.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);

	int index = -1;
	if ( params_.regExp || params_.wholeWords ) {
		index = ( forward ? line.indexOf(regExp) : line.lastIndexOf(regExp) );
		length = regExp.matchedLength();
		return index;
	}
	else {
		if ( !params_.caseSensitive ) {
			index = ( forward ? line.toLower().indexOf(str.toLower()) : line.toLower().lastIndexOf(str.toLower()) );
		}
		else {
			index = ( forward ? line.indexOf(str) : line.lastIndexOf(str) );
		}
		length = str.length();
		return index;
	}
}

bool SearchEngine::startFind(Juff::Document* doc) {
	if ( doc->isNull() ) return false;

	QString text;
	doc->getText(text);
	clearSelection(doc);

//	if ( params.multiLine )
//		return findML(s, flags);

	QString str = params_.findWhat;
	QStringList lines = text.split(QRegExp("\r\n|\n|\r"));
	int initialRow = -1, initialCol = -1;
	doc->getCursorPos(initialRow, initialCol);
	int lineIndex = 0;
	if (initialRow < 0 || initialCol < 0)
		return false;

	if ( !params_.backwards ) {
		// forward search
		QStringList::iterator it = lines.begin();

		// step to the initial line
		it += initialRow;
		int lineIndex = initialRow;

		bool firstLine = true;
		while ( it != lines.end() ) {
			QString line = (*it);
			int indent = 0;

			if ( firstLine ) {
				line = line.right(line.length() - initialCol);
				indent = initialCol;
				firstLine = false;
			}

			int length;
			int index = findAt(line, true, length);

			if ( index >= 0 ) {
				doc->setSelection(lineIndex, index + indent, lineIndex, index + indent + length);
				return true;
			}

			it++;
			lineIndex++;
		}
	}
	else {
		QStringList::iterator it = lines.end();

		// step to the initial row
		it -= lines.count() - initialRow;
		int lineIndex = initialRow;
		while ( lineIndex >= 0 ) {
			QString line = *it;
			if ( lineIndex == initialRow ) {
				line = line.left(initialCol);
			}

			int length;
			int index = findAt(line, false, length);

			if ( index >= 0 ) {
				doc->setSelection(lineIndex, index, lineIndex, index + length);
				return true;
			}

			it--;
			lineIndex--;
		}
	}

	return false;
}

void stepOver(Juff::Document* doc, bool forward) {
	int row(0), col(0);
	if ( !forward ) {
		row = doc->lineCount() - 1;
		col = doc->textLine(row).length();
	}
	else {
		row = 0;
		col = 0;
	}

	doc->setCursorPos(row, col);
}
