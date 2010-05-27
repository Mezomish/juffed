#include <QDebug>
#include <QMessageBox>

#include "SearchEngine.h"

#include "DocHandlerInt.h"
#include "Document.h"
#include "JuffMW.h"
#include "Log.h"

enum Answer { Yes, No, All, Cancel };
void stepOver(Juff::Document* doc, bool forward);
Answer confirm(QWidget* w);
bool doReplace(Juff::Document*, const Juff::SearchParams&, bool&, int&);
QString selectedTextForSearch(Juff::Document*);

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
	keepVariables(doc);

	mw_->showFindDialog(selectedTextForSearch(doc), false);
}

bool SearchEngine::findNext(Juff::Document* doc) {
	// in case the document is different
	keepVariables(doc);
	params_.backwards = false;

	if ( !startFind(doc) ) {
		stepOver(doc, true);
		if ( !startFind(doc) ) {
			return false;
		}
	}
	return true;
}

bool SearchEngine::findPrev(Juff::Document* doc) {
	// in case the document is different
	keepVariables(doc);
	params_.backwards = true;

	if ( !startFind(doc) ) {
		stepOver(doc, false);
		if ( !startFind(doc) ) {
			return false;
		}
	}
	return true;
}

void SearchEngine::replace(Juff::Document* doc) {
	keepVariables(doc);

	mw_->showFindDialog(selectedTextForSearch(doc), true);
}



void SearchEngine::onSearchRequested(const Juff::SearchParams& params) {
	LOGGER;

	Juff::Document* doc = handler_->curDoc();
	if ( doc->isNull() ) return;

	params_ = params;
	
	if ( params.replace ) {
		replace(doc, params);
		return;
	}
	
	bool found;
	if ( params.backwards )
		found = findPrev(doc);
	else
		found = findNext(doc);
	
	if ( !found )
		mw_->message(QIcon(), "", tr("Text '%1' was not found").arg(params_.findWhat));
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
//	int lineIndex = 0;
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

void SearchEngine::replace(Juff::Document* doc, const Juff::SearchParams& params) {
	bool cancelled = false;
	bool replaceAll = false;
	int count = 0;
	
	if ( params.backwards ) {
		while ( !cancelled && findPrev(doc) ) {
			if ( !doReplace(doc, params, replaceAll, count) ) {
				cancelled = true;
			}
		}
	}
	else {
		while ( !cancelled && findNext(doc) ) {
			if ( !doReplace(doc, params, replaceAll, count) ) {
				cancelled = true;
			}
		}
	}
	if ( count > 0 )
		mw_->message(QIcon(), "", tr("Replacement finished (%1 replacements were made)").arg(count));
}

void stepOver(Juff::Document* doc, bool forward) {
	int row(0), col(0);
	if ( !forward ) {
		row = doc->lineCount() - 1;
		QString textLine;
		doc->getTextLine(row, textLine);
		col = textLine.length();
	}
	else {
		row = 0;
		col = 0;
	}

	doc->setCursorPos(row, col);
}

Answer confirm(QWidget* w) {
	Answer answer = No;
	QMessageBox::StandardButton btn = QMessageBox::question(w, QObject::tr("Confirmation"), 
			QObject::tr("Replace this text?"), 
			QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::Cancel, 
			QMessageBox::Yes);

	switch (btn) {
		case QMessageBox::Yes:
			answer = Yes;
				break;
		case QMessageBox::No:
			answer = No;
			break;
		case QMessageBox::Cancel:
			answer = Cancel;
			break;
		case QMessageBox::YesToAll:
			answer = All;
			break;
		default: ;
	}

	return answer;
}

QString processSelection(Juff::Document* doc, const Juff::SearchParams& params) {
	QString selectedText;
	doc->getSelectedText(selectedText);
	QString targetText = params.replaceWith;
	if ( params.regExp ) {
		QRegExp regExp(params.findWhat);
		regExp.setCaseSensitivity(params.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
		if ( regExp.exactMatch(selectedText) ) {
			QStringList matches = regExp.capturedTexts();
			int n = matches.size();
			for ( int i = 0; i < n; ++i ) {
				targetText.replace(QString("\\%1").arg(i), matches[i]);
			}
		}
		else
			return "";
	}
	return targetText;
}

bool doReplace(Juff::Document* doc, const Juff::SearchParams& params, bool& replaceAll, int& count) {
	if ( replaceAll ) {
		doc->replaceSelectedText(processSelection(doc, params));
		++count;
		return true;
	}
	else {
		Answer answer = confirm(doc);
		switch (answer) {
			case Cancel:
				return false;
			
			case Yes :
				doc->replaceSelectedText(processSelection(doc, params));
				++count;
				break;
			
			case No :
				break;
			;
			case All :
				doc->replaceSelectedText(processSelection(doc, params));
				++count;
				replaceAll = true;
				break;
		}
	}
	return true;
}

QString selectedTextForSearch(Juff::Document* doc) {
	QString text;
	if ( doc->hasSelectedText() ) {
		int line1, line2, col1, col2;
		doc->getSelection(line1, col1, line2, col2);
		if ( line1 == line2 )
			doc->getSelectedText(text);
	}
	return text;
}
