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

#include "SearchEngine.h"

#include "Document.h"
#include "JuffMW.h"
#include "Log.h"
#include "SearchPopup.h"

int findInString(const QString& line, const Juff::SearchParams& params, int& length);
QString selectedTextForSearch(Juff::Document* doc);


SearchEngine::SearchEngine(Juff::DocHandlerInt* handler, JuffMW* mw) : QObject() {
	mw_ = mw;
	handler_ = handler;
	searchPopup_ = mw_->searchPopup();
	doc_ = NULL;
	
	connect(searchPopup_, SIGNAL(searchRequested()), SLOT(onSearchRequested()));
	connect(searchPopup_, SIGNAL(findNext()), SLOT(onFindNext()));
	connect(searchPopup_, SIGNAL(findPrev()), SLOT(onFindPrev()));
	connect(searchPopup_, SIGNAL(replaceNext()), SLOT(onReplaceNext()));
	connect(searchPopup_, SIGNAL(replacePrev()), SLOT(onReplacePrev()));
	connect(searchPopup_, SIGNAL(replaceAll()), SLOT(onReplaceAll()));
	connect(searchPopup_, SIGNAL(closed()), SLOT(onDlgClosed()));
}

void SearchEngine::setCurDoc(Juff::Document* doc) {
	if ( doc_ != NULL ) {
		clearSelection();
		doc_->clearHighlighting();
		restorePosition();
	}
	
	doc_ = doc;
	
	onSearchRequested();
	
//	searchPopup_->hideReplace();
//	searchPopup_->show();
	searchPopup_->focusOnFind();
}

void SearchEngine::find(Juff::Document* doc) {
	doc_ = doc;
	storePosition();
//	mw_->showFindDialog(selectedTextForSearch(doc), false);
	searchPopup_->setFindText(selectedTextForSearch(doc));
	
	searchPopup_->hideReplace();
	searchPopup_->show();
	searchPopup_->focusOnFind();
}

void SearchEngine::findNext(Juff::Document* doc) {
	doc_ = doc;
	onFindNext();
	doc_->clearHighlighting();
	doc_->setFocus();
}

void SearchEngine::findPrev(Juff::Document* doc) {
	doc_ = doc;
	onFindPrev();
	doc_->clearHighlighting();
	doc_->setFocus();
}

void SearchEngine::replace(Juff::Document* doc) {
	doc_ = doc;
	storePosition();
//	mw_->showFindDialog(selectedTextForSearch(doc), false);
	searchPopup_->setFindText(selectedTextForSearch(doc));
	
	searchPopup_->showReplace();
	searchPopup_->show();
	searchPopup_->focusOnFind();
}

void SearchEngine::storePosition() {
	if ( doc_ == NULL ) return;
		
	doc_->getCursorPos(row_, col_);
}

void SearchEngine::restorePosition() {
	if ( doc_ == NULL ) return;
		
	doc_->setCursorPos(row_, col_);
}



void SearchEngine::onSearchRequested() {
	clearSelection();
	doc_->clearHighlighting();
	
	QString findWhat = searchPopup_->searchParams().findWhat;
	if ( findWhat.isEmpty() ) {
		
		searchPopup_->highlightRed(false);
		searchPopup_->focusOnFind();
		return;
	}
	
	if ( !findAfterCursor() ) {
		if ( !findBeforeCursor() ) {
//			mw_->message(QIcon(), tr("Search"), tr("Text '%1' was not found").arg(findWhat));
			searchPopup_->highlightRed(true);
			searchPopup_->focusOnFind();
		}
		else {
			searchPopup_->highlightRed(false);
		}
	}
	else {
		searchPopup_->highlightRed(false);
	}
}

void SearchEngine::onFindNext() {
	if ( doc_ == NULL || doc_->isNull() ) return;
	
	int line1, col1, line2, col2;
	doc_->getSelection(line1, col1, line2, col2);
	doc_->setCursorPos(line2, col2);
	if ( findNext() )
		searchPopup_->focusOnFind();
}

void SearchEngine::onFindPrev() {
	if ( doc_ == NULL || doc_->isNull() ) return;
	
	int line1, col1, line2, col2;
	doc_->getSelection(line1, col1, line2, col2);
	doc_->setCursorPos(line1, col1);
	if ( findPrev() )
		searchPopup_->focusOnFind();
}

void SearchEngine::onReplaceNext() {
	LOGGER;
	
	if ( doc_ == NULL || doc_->isNull() ) return;
	const Juff::SearchParams& params = searchPopup_->searchParams();
	
	if ( doc_->hasSelectedText() ) {
		doc_->replaceSelectedText(params.replaceWith, true);
		findNext();
		searchPopup_->focusOnReplace();
	}
}

void SearchEngine::onReplacePrev() {
	LOGGER;
	
	if ( doc_ == NULL || doc_->isNull() ) return;
	const Juff::SearchParams& params = searchPopup_->searchParams();
	
	if ( doc_->hasSelectedText() ) {
		doc_->replaceSelectedText(params.replaceWith, false);
		findPrev();
		searchPopup_->focusOnReplace();
	}
}

void SearchEngine::onReplaceAll() {
	LOGGER;
	
	if ( doc_ == NULL || doc_->isNull() ) return;
	const Juff::SearchParams& params = searchPopup_->searchParams();
	
	storePosition();
	doc_->setCursorPos(0, 0);
	int replacesMade = 0;
	while ( findAfterCursor() ) {
		doc_->replaceSelectedText(params.replaceWith, true);
		++replacesMade;
//		onFindNext();
	}
	searchPopup_->focusOnReplace();
	
	restorePosition();
	
	mw_->message(QIcon(), tr("Replace"), tr("Replacement finished (%1 replacements were made)").arg(replacesMade), 5);
	// TODO : display notification here
}

void SearchEngine::onDlgClosed() {
	doc_->clearHighlighting();
}





void SearchEngine::clearSelection() {
	if ( doc_ == NULL || doc_->isNull() ) return;
	
	int line1, col1, line2, col2;
	doc_->getSelection(line1, col1, line2, col2);
	if ( !searchPopup_->searchParams().backwards ) {
		doc_->setCursorPos(line1, col1);
	}
	else {
		doc_->setCursorPos(line2, col2);
	}
}

bool SearchEngine::findNext() {
	if ( !findAfterCursor() ) {
		if ( !findBeforeCursor() ) {
			return false;
		}
	}
	return true;
}

bool SearchEngine::findPrev() {
	if ( !findBeforeCursor() ) {
		if ( !findAfterCursor() ) {
			return false;
		}
	}
	return true;
}

bool SearchEngine::findAfterCursor() {
	if ( doc_ == NULL || doc_->isNull() ) return false;
	
	QString text;
	doc_->getText(text);
	if ( text.isEmpty() )
		return false;
	
	const Juff::SearchParams& params = searchPopup_->searchParams();
	
	QStringList lines = text.split(QRegExp("\r\n|\n|\r"));
	int initialRow, initialCol;
	doc_->getCursorPos(initialRow, initialCol);
	
	QStringList::iterator it;
	QString line;
	int lineIndex;
	if ( params.backwards ) {
		it = lines.end();
		it--;
		lineIndex = lines.count() - 1;
		
		while ( lineIndex >= initialRow ) {
			line = (*it);
			int indent = 0;

			if ( lineIndex == initialRow ) {
				line = line.right(line.length() - initialCol);
				indent = initialCol;
			}

			int length;
			int index = findInString(line, params, length);

			if ( index >= 0 ) {
				doc_->highlightOccurence(params);
				doc_->setSelection(lineIndex, index + indent, lineIndex, index + indent + length);
				searchPopup_->focusOnFind();
				return true;
			}

			it--;
			lineIndex--;
		}
	}
	else {
		it = lines.begin();
		it += initialRow;
		lineIndex = initialRow;

		while ( it != lines.end() ) {
			line = (*it);
			int indent = 0;

			if ( lineIndex == initialRow ) {
				line = line.right(line.length() - initialCol);
				indent = initialCol;
			}

			int length;
			int index = findInString(line, params, length);

			if ( index >= 0 ) {
				doc_->highlightOccurence(params);
				doc_->setSelection(lineIndex, index + indent, lineIndex, index + indent + length);
				searchPopup_->focusOnFind();
				return true;
			}

			it++;
			lineIndex++;
		}
	}
	return false;
}

bool SearchEngine::findBeforeCursor() {
	if ( doc_ == NULL || doc_->isNull() )
		return false;
	
	QString text;
	doc_->getText(text);
	if ( text.isEmpty() )
		return false;
	
	const Juff::SearchParams& params = searchPopup_->searchParams();
	
	QStringList lines = text.split(QRegExp("\r\n|\n|\r"));
	int initialRow, initialCol;
	doc_->getCursorPos(initialRow, initialCol);
	
	QString line;
	int lineIndex;
	QStringList::iterator it;
	
	if ( params.backwards ) {
		lineIndex = initialRow;
		it = lines.begin();
		it += initialRow;
		
		while ( lineIndex >= 0 ) {
			line = (*it);
			
			if ( lineIndex == initialRow ) {
				line = line.left(initialCol);
			}
			
			int length;
			int indent = 0;
			int index = findInString(line, searchPopup_->searchParams(), length);

			if ( index >= 0 ) {
				doc_->highlightOccurence(params);
				doc_->setSelection(lineIndex, index + indent, lineIndex, index + indent + length);
				searchPopup_->focusOnFind();
				return true;
			}
			
			it--;
			lineIndex--;
		}
	}
	else {
		lineIndex = 0;
		it = lines.begin();
		
		while ( lineIndex <= initialRow ) {
			line = (*it);
			
			if ( lineIndex == initialRow ) {
				line = line.left(initialCol);
			}
			
			int length;
			int indent = 0;
			int index = findInString(line, searchPopup_->searchParams(), length);

			if ( index >= 0 ) {
				doc_->highlightOccurence(params);
				doc_->setSelection(lineIndex, index + indent, lineIndex, index + indent + length);
				searchPopup_->focusOnFind();
				return true;
			}
			
			it++;
			lineIndex++;
		}
	}	
	
	return false;
}







////////////////////////////////////////////////////////////
// Local helpers

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

int findInString(const QString& line, const Juff::SearchParams& params, int& length) {
	QString str = params.findWhat;
	bool forward = !params.backwards;
	QRegExp regExp;
	if ( params.wholeWords ) {
		regExp = QRegExp(QString("\\b%1\\b").arg(QRegExp::escape(str)));
	}
	else
		regExp = QRegExp(str);
	regExp.setCaseSensitivity(params.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);

	int index = -1;
	if ( params.regExp || params.wholeWords ) {
		index = ( forward ? line.indexOf(regExp) : line.lastIndexOf(regExp) );
		length = regExp.matchedLength();
		return index;
	}
	else {
		if ( !params.caseSensitive ) {
			index = ( forward ? line.toLower().indexOf(str.toLower()) : line.toLower().lastIndexOf(str.toLower()) );
		}
		else {
			index = ( forward ? line.indexOf(str) : line.lastIndexOf(str) );
		}
		length = str.length();
		return index;
	}
}
