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

#ifndef __JUFFED_SEARCH_ENGINE_H__
#define __JUFFED_SEARCH_ENGINE_H__

class JuffMW;
class SearchPopup;

namespace Juff {
	class Document;
	class DocHandlerInt;
	class SearchResults;
}

#include <QObject>
#include "Types.h"

class SearchEngine : public QObject {
Q_OBJECT
public:
	SearchEngine(Juff::DocHandlerInt*, JuffMW*);
	virtual ~SearchEngine();

	void setCurDoc(Juff::Document*);
	// These methods are called to initiate a search
	void find();
	void findNext();
	void findPrev();
	void replace();
	

protected slots:
	void onSearchRequested();
	void onFindNext();
	void onFindPrev();
	void onReplaceNext();
	void onReplacePrev();
	void onReplaceAll();
	void onDlgClosed();
	void onDocTextChanged();

private:
	void clearSelection();
	void storePosition();
	void restorePosition();
	void doChangeCurDoc(Juff::Document*);

	int selectNextOccurence();
	int selectPrevOccurence();
	
	Juff::SearchResults* getSearchResults();

	JuffMW* mw_;
	Juff::DocHandlerInt* handler_;
	SearchPopup* searchPopup_;
	Juff::Document* doc_;
	int row_;
	int col_;
};

#endif // __JUFFED_SEARCH_ENGINE_H__
