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

#ifndef __JUFFED_DOCUMENT_H__
#define __JUFFED_DOCUMENT_H__

class QTextCodec;
class QTimer;

#include "Enums.h"
#include "Types.h"

#include <QDateTime>
#include <QMutex>
#include <QWidget>

namespace Juff {

class SearchResults;

class Document : public QWidget {
Q_OBJECT
public:
	Document(const QString&);
	virtual ~Document();

	/**
	* Returns document's file name on disk or a string like "Noname N" if 
	* the document was not saved yet (where N is some number). This file 
	* name is unique for a document within application's running time.
	*/
	QString fileName() const;
	
	/**
	* Returns document's line count.
	*/
	virtual int lineCount() const { return 0; }
	
	/**
	* Returns whether document is modified.
	*/
	virtual bool isModified() const { return false; }
	
	/**
	* Returns whether document has selected text.
	*/
	virtual bool hasSelectedText() const { return false; }
	
	/**
	* Lets to obtain document's selection coordinates.
	* Params:
	*        line1 - the line number where selection starts
	*        col1  - the position of selection beginning (within \param line1)
	*        line2 - the line number where selection ends
	*        col2  - the position of selection end (within \param line2)
	*
	* If there is no selection then line1 == line2 and row1 == row2 and they 
	* reflect current cursor position.
	*
	* Returns true if successful, otherwise (e.g. document doesn't support 
	* text selection) returns false;
	*/
	virtual bool getSelection(int& line1, int& col1, int& line2, int& col2) const { return false; }

	/**
	* Lets to obtain curent selection text.
	*
	* Returns true if successful, otherwise (e.g. document doesn't support 
	* text selection) returns false;
	*/
	virtual bool getSelectedText(QString&) const { return false; }
	
	/**
	* Lets to obtain document's text.
	*
	* Returns true if successful, otherwise (e.g. document doesn't support 
	* text) returns false;
	*/
	virtual bool getText(QString&) const { return false; }
	
	/**
	* Lets to obtain \param n-th line of text.
	*
	* Returns true if successful, otherwise (e.g. document doesn't support 
	* text) returns false;
	*/
	virtual bool getTextLine(int n, QString&) const { return false; }
	
	/**
	* Lets to obtain document's cursor coordinates.
	* Params:
	*        line - line number where the cursor is
	*        col  - the cursor position within a line
	*
	* Returns true if successful, otherwise (e.g. document doesn't support 
	* text) returns false;
	*/
	virtual bool getCursorPos(int&, int&) const { return false; }
	
	/**
	* Returns document's charset.
	*/
	QString charset() const { return charset_; }
	/**
	* Sets document's charset. Doesn't convert document's text, just sets the attribute.
	*/
	void setCharset(const QString&);
	
	/**
	* Returns the current document's syntax highlighting scheme.
	* Returns "none" if no scheme is set.
	*/
	virtual QString syntax() const { return ""; }
	
	/**
	* Sets the syntax highlighting scheme. If the scheme is not 
	* supported nothing happens. If it is supported then scheme changes and API
	* emits a signal "docSyntaxChanged(const QString& oldSyntax)".
	*/
	virtual void setSyntax(const QString& syntax) {}
	
	/**
	* Sets the document's selection.
	*/
	virtual void setSelection(int line1, int col1, int line2, int col2) {}
	
	/**
	* Sets the cursor to ( \param line, \param col ) position.
	*/
	virtual void setCursorPos(int line, int col) {}
	
	/**
	*/
	virtual void highlightSearchResults() {}
	
	/**
	*/
	virtual void clearHighlighting() {}
	
	/**
	* Removes selected text.
	*/
	virtual void removeSelectedText() {}
	
	/**
	* Sets the 'modified' flag to the document.
	*/
	virtual void setModified(bool) {}
	
	/**
	* Replaces the currently selected text with \param text. If there was
	* no selectin text then inserts \param text at the current cursor position.
	*/
	virtual void replaceSelectedText(const QString& text, bool cursorToTheEnd = true) {}
	
	/**
	* Inserts the \param text at the current cursor position.
	*/
	virtual void insertText(const QString&) {}
	
	/**
	* Sets params loaded from saved session.
	*/
	virtual void setSessionParams(const Juff::SessionParams&) {}
	
	/**
	* Sets params for saving in session.
	*/
	virtual Juff::SessionParams sessionParams() const { return Juff::SessionParams(); }
	
//	Juff::Document* clone() const { return clone_; }
//	bool hasClone() const { return clone_ != NULL; }

	virtual QString type() const = 0;
	virtual bool supportsAction(Juff::ActionID) const;
	virtual void init() {}
	virtual void print() {}
	virtual void reload() {}
	virtual bool save(QString& error);
	virtual bool saveAs(const QString& fileName, QString& error);
	
	/**
	* Clone is not updated yet, you need to call updateClone() function
	* on the ORIGINAL document after creating a clone.
	*/
//	virtual Juff::Document* createClone() = 0;
	
	virtual void undo() {}
	virtual void redo() {}
	virtual void cut() {}
	virtual void copy() {}
	virtual void paste() {}
	virtual void gotoLine(int) {}

	virtual bool find(const Juff::SearchParams&) { return false; }

	virtual void setWrapWords(bool) {}
	virtual void setShowLineNumbers(bool) {}
	virtual void setShowWhitespaces(bool) {}
	virtual void setShowLineEndings(bool) {}
	virtual void zoomIn() {}
	virtual void zoomOut() {}
	virtual void zoom100() {}

	virtual bool wrapWords() const { return false;}
	virtual bool lineNumbersVisible() const { return false;}
	virtual bool whitespacesVisible() const { return false;}
	virtual bool lineEndingsVisible() const { return false;}
	
	void setSearchResults(Juff::SearchResults*);
	SearchResults* searchResults() const;
	
	virtual void applySettings() {}
	
	// This method must be reimplemented ONLY (!!!) in NullDoc
	virtual bool isNull() const { return false; }

signals:
	void modified(bool);
	void focused();
	void cursorPosChanged(int, int);
	void lineCountChanged(int);
	void textChanged();
	void syntaxChanged(const QString&);
	void charsetChanged(const QString&);
	void renamed(const QString&);

protected:
	// This constructor is being used for creating clones
//	Document(Juff::Document*);
//	virtual void updateClone();

	static QString guessCharset(const QString&);
	void setCodec(QTextCodec*);

	QTextCodec* codec() const {
		return codec_;
	}
	
	void startCheckingTimer();
	void stopCheckingTimer();

protected slots:
	void checkLastModified();

private:
	QString fileName_;
	QString charset_;
	QTextCodec* codec_;
//	Juff::Document* clone_;
	static int sCount_;
	
	QDateTime lastModified_;
	QTimer* modCheckTimer_;
	QMutex checkingMutex_;
	QMutex lastModMutex_;
	Juff::SearchResults* searchResults_;
};

typedef QList<Juff::Document*> DocList;

}

#endif // __JUFFED_DOCUMENT_H__
