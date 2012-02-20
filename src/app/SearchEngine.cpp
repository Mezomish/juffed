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

#include "SearchEngine.h"

#include "Constants.h"
#include "Document.h"
#include "JuffMW.h"
#include "Log.h"
#include "SearchPopup.h"
#include "SearchResults.h"

int findInString(const QString& line, int, const Juff::SearchParams& params, int& length);
QString selectedTextForSearch(Juff::Document* doc);


SearchEngine::SearchEngine(Juff::DocHandlerInt* handler, JuffMW* mw) : QObject() {
	mw_ = mw;
	handler_ = handler;
	searchPopup_ = mw_->searchPopup();
	curDoc_ = NULL;
	
	connect(searchPopup_, SIGNAL(searchParamsChanged(const Juff::SearchParams&)), SLOT(onSearchParamsChanged(const Juff::SearchParams&)));
	connect(searchPopup_, SIGNAL(findNextRequested()), SLOT(onFindNext()));
	connect(searchPopup_, SIGNAL(findPrevRequested()), SLOT(onFindPrev()));
	connect(searchPopup_, SIGNAL(replaceNextRequested()), SLOT(onReplaceNext()));
	connect(searchPopup_, SIGNAL(replacePrevRequested()), SLOT(onReplacePrev()));
	connect(searchPopup_, SIGNAL(replaceAllRequested()), SLOT(onReplaceAll()));
	connect(searchPopup_, SIGNAL(closed()), SLOT(onDlgClosed()));
}

SearchEngine::~SearchEngine() {
}

void SearchEngine::setCurDoc(Juff::Document* doc) {
	changeCurDoc(doc);
}

void SearchEngine::find() {
	if ( curDoc_ == NULL || curDoc_->isNull() ) {
		qDebug("No document specified for SearchEngine");
		return;
	}
	
	// check for selected text
	QString selectedText;
	if ( !searchPopup_->isVisible() && curDoc_->hasSelectedText() ) {
		int line1, col1, line2, col2;
		curDoc_->getSelection(line1, col1, line2, col2);
		if ( line1 == line2 ) {
			curDoc_->getSelectedText(selectedText);
		}
	}
	
	searchPopup_->expand(false);
	searchPopup_->show();
	if ( !selectedText.isEmpty() ) {
		searchPopup_->setFindText(selectedText);
	}
	else {
		Juff::SearchResults* res = curDoc_->searchResults();
		if ( res != NULL ) {
			searchPopup_->setFindText(res->params().findWhat);
			curDoc_->highlightSearchResults();
		}
		else {
			searchPopup_->setFindText("");
		}
	}
	searchPopup_->setFocusOnFind(true);
}

void SearchEngine::findNext() {
	if ( curDoc_ == NULL || curDoc_->isNull() ) {
		qDebug("No document specified for SearchEngine");
		return;
	}
	
	if ( searchPopup_->isVisible() ) {
		onFindNext();
	}
	else {
		Juff::SearchResults* res = curDoc_->searchResults();
		if ( res == NULL ) {
			find();
		}
		else {
			if ( res->count() > 0 ) {
				selectNextOccurence();
				curDoc_->setFocus();
			}
			else {
				// TODO :
			}
		}
	}
}

void SearchEngine::findPrev() {
	if ( curDoc_ == NULL || curDoc_->isNull() ) {
		qDebug("No document specified for SearchEngine");
		return;
	}
	
	if ( searchPopup_->isVisible() ) {
		onFindPrev();
	}
	else {
		Juff::SearchResults* res = curDoc_->searchResults();
		if ( res == NULL ) {
			find();
		}
		else {
			if ( res->count() > 0 ) {
				selectPrevOccurence();
				curDoc_->setFocus();
			}
			else {
				// TODO :
			}
		}
	}
}

void SearchEngine::replace() {
	if ( curDoc_ == NULL || curDoc_->isNull() ) {
		qDebug("No document specified for SearchEngine");
		return;
	}
	
	searchPopup_->setFindText(selectedTextForSearch(curDoc_));
	
	searchPopup_->expand(true);
	searchPopup_->show();
	searchPopup_->setFocusOnFind(true);
}

void SearchEngine::changeCurDoc(Juff::Document* doc) {
	if ( NULL != curDoc_ ) {
		curDoc_->disconnect(SIGNAL(textChanged()), this, SLOT(onDocTextChanged()));
	}
	curDoc_ = doc;
	if ( NULL != curDoc_ ) {
		connect(curDoc_, SIGNAL(textChanged()), this, SLOT(onDocTextChanged()));
	}
	
	searchPopup_->dismiss();
	onDlgClosed();
}



void SearchEngine::onSearchParamsChanged(const Juff::SearchParams& params) {
//	LOGGER;
	clearSelection();
	curDoc_->clearHighlighting();
	
	if ( params.findWhat.isEmpty() ) {
		searchPopup_->setSearchStatus(-1, 0);
		searchPopup_->setFocusOnFind();
		return;
	}
	
	Juff::SearchResults* results = performSearch(params);
	curDoc_->setSearchResults(results);
	
	if ( results == NULL ) {
		qDebug("Error: SearchResults can't be NULL, probably it's an error in getSearchResults() method");
		return;
	}
	
	if ( results->count() > 0 ) {
		selectNextOccurence();
		if ( searchPopup_->isVisible() ) {
			curDoc_->highlightSearchResults();
		}
	}
	else {
		searchPopup_->setSearchStatus(-1, 0);
	}
	
	searchPopup_->setFocusOnFind();
}

Juff::SearchResults* SearchEngine::performSearch(const Juff::SearchParams& params) {
	if ( curDoc_ == NULL || curDoc_->isNull() ) return NULL;
	
	QString text;
	curDoc_->getText(text);
	
	if ( text.isEmpty() )
		return NULL;
	
	if ( params.findWhat.isEmpty() )
		return NULL;
	
	Juff::SearchResults* results = NULL;
	if ( params.mode == Juff::SearchParams::MultiLineRegExp ) {
		// TODO : 
	}
	else {
		results = new Juff::SearchResults(params);
		
		QStringList lines = text.split(LineSeparatorRx);
		QStringList::iterator it = lines.begin();
		int lineIndex = 0;
		while ( it != lines.end() ) {
			QString lineStr = *it;
			int length;
			int indent = 0;
			int pos = findInString(lineStr, indent, params, length);
			while ( pos >= 0 ) {
				if ( length > 0 ) {
					results->addOccurence(lineIndex, pos, lineIndex, pos + length);
				}
				else {
					// to prevent infinite loop when search ends with an empty string
					length = 1;
				}
				
				indent = pos + length;
				
				pos = findInString(lineStr, indent, params, length);
			}
			
			lineIndex++;
			it++;
		}
	}
	
	return results;
}

/**
* It DOESN'T check for results validity - check prior to calling this function.
*/
int SearchEngine::selectNextOccurence() {
	// set the cursor to the end of selection
	int line1, col1, line2, col2;
	if ( curDoc_->hasSelectedText() ) {
		curDoc_->getSelection(line1, col1, line2, col2);
	}
	else {
		curDoc_->getCursorPos(line2, col2);
	}
	
	Juff::SearchResults* results = curDoc_->searchResults();
	
	// find the next occurence
	int index = results->findIndexByCursorPos(line2, col2, true);
	
	if ( index >= 0 ) {
		const Juff::SearchOccurence& occ = results->occurence(index);
		// select it
		curDoc_->setSelection(occ.startRow, occ.startCol, occ.endRow, occ.endCol);
		// update search status
		searchPopup_->setSearchStatus(index, results->count());
	}
	return index;
}

/**
* It DOESN'T check for results validity - check prior to calling this function.
*/
int SearchEngine::selectPrevOccurence() {
	int line1, col1, line2, col2;
	if ( curDoc_->hasSelectedText() ) {
		curDoc_->getSelection(line1, col1, line2, col2);
	}
	else {
		curDoc_->getCursorPos(line1, col1);
	}
	
	Juff::SearchResults* results = curDoc_->searchResults();
	
	// find the previous occurence
	int index = results->findIndexByCursorPos(line1, col1, false);
	if ( index >= 0 ) {
		const Juff::SearchOccurence& occ = results->occurence(index);
		// select it
		curDoc_->setSelection(occ.startRow, occ.startCol, occ.endRow, occ.endCol);
		// update search status
		searchPopup_->setSearchStatus(index, results->count());
	}
	return index;
}

void SearchEngine::onFindNext() {
	if ( curDoc_ == NULL || curDoc_->isNull() ) return;
	
	Juff::SearchResults* results = curDoc_->searchResults();
	
	if ( results != NULL ) {
		if ( results->count() == 0 ) {
			searchPopup_->setSearchStatus(-1, 0);
			return;
		}
		
		curDoc_->highlightSearchResults();
		selectNextOccurence();
	}
	searchPopup_->setFocusOnFind();
}

void SearchEngine::onFindPrev() {
	if ( curDoc_ == NULL || curDoc_->isNull() ) return;
	
	Juff::SearchResults* results = curDoc_->searchResults();
	
	if ( results != NULL ) {
		if ( results->count() == 0 ) {
			searchPopup_->setSearchStatus(-1, 0);
			return;
		}
		
		curDoc_->highlightSearchResults();
		selectPrevOccurence();
	}
	searchPopup_->setFocusOnFind();
}

void expandRegExpMatches(const QString& selectedText, QString& replaceWith, const Juff::SearchParams& params) {
	QRegExp regExp(params.findWhat);
	
	if ( regExp.exactMatch(selectedText) ) {
		QStringList matches = regExp.capturedTexts();
		int n = matches.size();
		for ( int i = 0; i < n; ++i ) {
			replaceWith.replace(QString("\\%1").arg(i), matches[i]);
		}
	}
}

void SearchEngine::onReplaceNext() {
	if ( curDoc_ == NULL || curDoc_->isNull() ) return;
	const Juff::SearchParams& params = searchPopup_->searchParams();
	
	if ( curDoc_->hasSelectedText() ) {
		QString selectedText;
		curDoc_->getSelectedText(selectedText);
		QString replaceWith = params.replaceWith;

		if ( params.mode == Juff::SearchParams::RegExp || params.mode == Juff::SearchParams::MultiLineRegExp ) {
			expandRegExpMatches(selectedText, replaceWith, params);
		}
		curDoc_->replaceSelectedText(replaceWith, true);
	}
	onFindNext();
	searchPopup_->setFocusOnReplace(false);
}

void SearchEngine::onReplacePrev() {
	if ( curDoc_ == NULL || curDoc_->isNull() ) return;
	const Juff::SearchParams& params = searchPopup_->searchParams();
	
	if ( curDoc_->hasSelectedText() ) {
		QString selectedText;
		curDoc_->getSelectedText(selectedText);
		QString replaceWith = params.replaceWith;

		if ( params.mode == Juff::SearchParams::RegExp || params.mode == Juff::SearchParams::MultiLineRegExp ) {
			expandRegExpMatches(selectedText, replaceWith, params);
		}
		curDoc_->replaceSelectedText(replaceWith, false);
	}
	onFindPrev();
	searchPopup_->setFocusOnReplace(false);
}

void SearchEngine::onReplaceAll() {
	if ( curDoc_ == NULL || curDoc_->isNull() ) return;
	
	Juff::SearchResults* results = curDoc_->searchResults();
	if ( results == NULL ) return;
	
	const Juff::SearchParams& params = searchPopup_->searchParams();
	
	disconnect(curDoc_, SIGNAL(textChanged()), this, SLOT(onDocTextChanged()));
	
	curDoc_->setCursorPos(0, 0);
	int replacesMade = 0;
	int count = results->count();
	
	// we want all replaces to be a single undo action - mark the beginning of this action
	curDoc_->beginUndoAction();
	
	for ( int i = count - 1; i >= 0; --i ) {
		const Juff::SearchOccurence& occ = results->occurence(i);
		curDoc_->setSelection(occ.startRow, occ.startCol, occ.endRow, occ.endCol);

		QString selectedText;
		curDoc_->getSelectedText(selectedText);
		QString replaceWith = params.replaceWith;

		if ( params.mode == Juff::SearchParams::RegExp || params.mode == Juff::SearchParams::MultiLineRegExp ) {
			expandRegExpMatches(selectedText, replaceWith, params);
		}
		curDoc_->replaceSelectedText(replaceWith);
		replacesMade++;
	}
	
	// mark the end of a single undo action for replacements
	curDoc_->endUndoAction();
	
	searchPopup_->setFocusOnReplace();
	
	mw_->message(QIcon(), tr("Replace"), tr("Replacement finished (%1 replacements were made)").arg(replacesMade), Qt::AlignBottom | Qt::AlignLeft, 5);
	
	connect(curDoc_, SIGNAL(textChanged()), this, SLOT(onDocTextChanged()));
	
	curDoc_->setSearchResults(NULL);
}

void SearchEngine::onDlgClosed() {
	if ( curDoc_ != NULL ) {
		curDoc_->clearHighlighting();
		curDoc_->setFocus();
	}
}

void SearchEngine::onDocTextChanged() {
	clearSelection();
	curDoc_->clearHighlighting();
	
	Juff::SearchParams params = searchPopup_->searchParams();
	params.backwards = false;
	Juff::SearchResults* results = performSearch(params);
	curDoc_->setSearchResults(results);
	if ( searchPopup_->isVisible() ) {
		curDoc_->highlightSearchResults();
		if ( results != NULL ) {
			searchPopup_->setSearchStatus(-1, results->count());
		}
	}
}



void SearchEngine::clearSelection() {
	if ( curDoc_ == NULL || curDoc_->isNull() || !curDoc_->hasSelectedText() ) return;
	
	int line1, col1, line2, col2;
	curDoc_->getSelection(line1, col1, line2, col2);
	if ( !searchPopup_->searchParams().backwards ) {
		curDoc_->setCursorPos(line1, col1);
	}
	else {
		curDoc_->setCursorPos(line2, col2);
	}
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

/**
* It ignores 'backwards' flag and always searches forward. But we don't need searching
* back since we are searching for all occurences at once.
*/
int findInString(const QString& line, int startFrom, const Juff::SearchParams& params, int& length) {
	if ( line.isEmpty() ) {
		return -1;
	}
	
	QString str = params.findWhat;
	QRegExp regExp;
	if ( params.mode == Juff::SearchParams::WholeWords ) {
		regExp = QRegExp(QString("\\b%1\\b").arg(QRegExp::escape(str)));
	}
	else
		regExp = QRegExp(str);
	regExp.setCaseSensitivity(params.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);

	int index = -1;
	if ( params.mode != Juff::SearchParams::PlainText ) {
		index = line.indexOf(regExp, startFrom);
		length = regExp.matchedLength();
		return index;
	}
	else {
		if ( !params.caseSensitive ) {
			index = line.indexOf(str, startFrom, Qt::CaseInsensitive);
		}
		else {
			index = line.indexOf(str, startFrom);
		}
		length = str.length();
		return index;
	}
}
