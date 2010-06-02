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
}

#include <QObject>
#include "Types.h"

class SearchEngine : public QObject {
Q_OBJECT
public:
	SearchEngine(Juff::DocHandlerInt*, JuffMW*);

	void setCurDoc(Juff::Document*);
	// These methods are called to initiate a search
	void find(Juff::Document*);
	void findNext(Juff::Document*);
	void findPrev(Juff::Document*);
	void replace(Juff::Document*);
	
	void storePosition();
	void restorePosition();

protected slots:
	void onSearchRequested();
	void onFindNext();
	void onFindPrev();
	void onReplaceNext();
	void onReplacePrev();
	void onReplaceAll();
	void onDlgClosed();

private:
	void clearSelection();

	/**
	* Performs a search FORWARD for an occurence specified by SearchParams
	* that can be obtained from SearchPopup.
	* Returns true if found something, otherwise returns false.
	*/
	bool findNext();
	/**
	* Performs a search BACKWARD for an occurence specified by SearchParams
	* that can be obtained from SearchPopup.
	* Returns true if found something, otherwise returns false.
	*/
	bool findPrev();

	/**
	* Tries to find an occurence AFTER the current cursor position.
	* Returns true if found something, otherwise returns false.
	*/
	bool findAfterCursor();

	/**
	* Tries to find an occurence BEFORE the current cursor position.
	* Returns true if found something, otherwise returns false.
	*/
	bool findBeforeCursor();

	JuffMW* mw_;
	Juff::DocHandlerInt* handler_;
	SearchPopup* searchPopup_;
	Juff::Document* doc_;
	int row_;
	int col_;
};

#endif // __JUFFED_SEARCH_ENGINE_H__
