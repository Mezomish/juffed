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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef __JUFFED_SCI_DOC_H__
#define __JUFFED_SCI_DOC_H__

#include "QSciLibConfig.h"

class QsciLexer;

#include "Document.h"

namespace Juff {

class JuffScintilla;

class QSCI_ENGINE_EXPORT SciDoc : public Juff::Document {
Q_OBJECT
public:
	enum Eol {
		EolWin,
		EolMac,
		EolUnix,
	};
	
	SciDoc(const QString&);
	virtual ~SciDoc();

	virtual int lineCount() const;
	virtual bool isModified() const;
	virtual bool hasSelectedText() const;
	virtual bool getSelection(int&, int&, int&, int&) const;
	virtual bool getSelectedText(QString&) const;
	virtual bool getText(QString&) const;
	virtual void setText(const QString&);
	virtual bool getTextLine(int, QString&) const;
	virtual bool getCursorPos(int&, int&) const;
	virtual QString syntax() const;
	virtual void setModified(bool);
	virtual void setSelection(int, int, int, int);
	virtual void removeSelectedText();
	virtual void replaceSelectedText(const QString&, bool cursorToTheEnd = true);
	virtual void insertText(const QString&);
	virtual void setCursorPos(int, int);
	virtual void setSyntax(const QString& lexName);
	virtual void highlightSearchResults();
	virtual void clearHighlighting();
	virtual void beginUndoAction();
	virtual void endUndoAction();

	virtual void setSessionParams(const Juff::SessionParams&);
	virtual Juff::SessionParams sessionParams() const;

	virtual QString type() const;
//	virtual Juff::Document* createClone();
	virtual bool supportsAction(Juff::ActionID) const;
	virtual void init();
	virtual void print();
	virtual void reload();
	virtual bool save(QString& error);
	virtual bool saveAs(const QString& fileName, QString& error);

	virtual void undo();
	virtual void redo();
	virtual void cut();
	virtual void copy();
	virtual void paste();
	virtual void gotoLine(int);

	virtual void setWrapWords(bool);
	virtual void setShowLineNumbers(bool);
	virtual void setShowWhitespaces(bool);
	virtual void setShowLineEndings(bool);
	virtual void zoomIn();
	virtual void zoomOut();
	virtual void zoom100();
	
	virtual bool wrapWords() const;
	virtual bool lineNumbersVisible() const;
	virtual bool whitespacesVisible() const;
	virtual bool lineEndingsVisible() const;
	
	virtual void applySettings();
	
	void toUpperCase();
	void toLowerCase();
	void swapLines();
	void moveUp();
	void moveDown();
	
	void toggleCommentLines();
	void toggleCommentBlock();
	void duplicateText();
	void unindent();
	void insertTab();
	void removeLine();
	void removeLineLeft();
	void removeLineRight();
	void foldUnfoldAll();
	
	int scrollPos() const;
	void setScrollPos(int);
	SciDoc::Eol eol() const;
	void setEol(SciDoc::Eol);
	bool isExternalModified() const;
	void setExternalModified(bool);
	bool indentationsUseTabs() const;
	void setIndentationsUseTabs(bool);
	int tabWidth() const;
	void setTabWidth(int);
	
	void toggleMarker(int);
	void removeAllMarkers();
	QList<int> markers() const;
	
public slots:
	void highlightWord();
	
private slots:
	void onCursorMoved(int, int);
	void onMarginClicked(int, int, Qt::KeyboardModifiers);
	void onLineCountChanged();
	void onEditFocused();

signals:
	void markersMenuRequested(const QPoint&);
	void escapePressed();

protected:
	SciDoc(Juff::Document*);
//	virtual void updateClone();

private:
	void readFile();
	void setLexer(const QString& lexName);
	void loadAutocompletionAPI(const QString& lexName, QsciLexer* lexer);

	void commentLine(JuffScintilla* edit, int line, const QString& str1, const QString& comment);
	void uncommentLine(JuffScintilla* edit, int line, const QString& str1, const QString& comment);
	void stripTrailingSpaces();
	void moveCursorToTheEnd(int line, int col, const QString& text);

	class Interior;
	Interior* int_;
	bool extModif_;
};

}

#endif // __JUFFED_SCI_DOC_H__
