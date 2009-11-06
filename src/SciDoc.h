/*
JuffEd - An advanced text editor
Copyright 2007-2009 Mikhail Murzin

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

#ifndef _JUFF_SCI_DOCUMENT_H_
#define _JUFF_SCI_DOCUMENT_H_

#include "Document.h"

class QsciLexer;
//class QsciMacro;

namespace Juff {

class JuffScintilla;

enum EolMode {
	EolWin,
	EolUnix,
	EolMac,
};

class SciDoc : public Document {
Q_OBJECT
public:
	SciDoc(const QString&);
	virtual ~SciDoc();
	
	virtual void init();
	virtual QWidget* widget();
	virtual bool isModified() const;
	virtual void setModified(bool);
	virtual void setFileName(const QString& fileName);
	virtual bool save(const QString&, const QString&, QString&);
	virtual void print();
	virtual void reload();

	virtual void undo();
	virtual void redo();
	virtual void cut();
	virtual void copy();
	virtual void paste();

	virtual void find(const QString&, const DocFindFlags&);
	virtual void replace(const QString&, const QString&, const DocFindFlags&);
	virtual void gotoLine(int);
	virtual QString text() const;
	virtual QString text(int) const;
	virtual QString wordUnderCursor() const;
	virtual QString selectedText() const;
	virtual void getCursorPos(int&, int&) const;
	virtual void setCursorPos(int, int);
	virtual void getSelection(int&, int&, int&, int&) const;
	virtual void setSelection(int, int, int, int);
	virtual void insertText(const QString&);
	virtual void removeSelectedText();
	virtual void replaceSelectedText(const QString&);
	virtual int curLine() const;
	virtual int curScrollPos() const;
	virtual void setScrollPos(int);

	virtual int lineCount() const;
	virtual QString charset() const;
	virtual void setCharset(const QString&, bool confirm = false);

	virtual void applySettings();
	virtual void updateActivated();
	virtual void addContextMenuActions(const ActionList&);

	QString syntax() const;
	void setSyntax(const QString& lexName);
	EolMode eolMode() const;
	void setEolMode(EolMode eol);
	IntList markers() const;
	QString getLine(int) const;

	void showLineNumbers(bool);
	void wrapText(bool);
	void showInvisibleSymbols(bool);
	void zoomIn();
	void zoomOut();
	void zoom100();

	void toggleMarker();
	void nextMarker();
	void prevMarker();
	void removeAllMarkers();

//	QsciMacro* newMacro();
//	void runMacro(const QString&);

	void goToMatchingBrace();
	void selectToMatchingBrace();
	void toggleLineComment();
	void toggleBlockComment();
	void duplicateLine();
	void moveLineUp();
	void deleteCurrentLine();
	void toUpperCase();
	void toLowerCase();
	
	void changeSplitOrientation();

public slots:
	void unindent();

private slots:
	void onCursorMove(int, int);
	void onMarginClicked (int margin, int line, Qt::KeyboardModifiers state);
	void onLinesCountChanged();
	void onEditReceivedFocus();

private:
	void readDoc(bool keepCharset = false);
	void loadAutocompletionAPI(const QString& lexName, QsciLexer* lexer);
	void stripTrailingSpaces();
	void toggleMarker(int line);
	void startFind(JuffScintilla*, const QString&, const DocFindFlags&);
	void startReplace(JuffScintilla*, const QString&, const QString&, const DocFindFlags&, bool&, int&);

	bool doReplace(JuffScintilla* edit, const QString& str1, const QString& str2, const DocFindFlags& flags, bool& replaceAll);
	void commentLine(JuffScintilla* edit, int line, const QString& str1, const QString& comment);
	void uncommentLine(JuffScintilla* edit, int line, const QString& str1, const QString& comment);
	JuffScintilla* getActiveEdit() const;

	class Interior;
	Interior* docInt_;
};

}	//	namespace Juff

#endif
