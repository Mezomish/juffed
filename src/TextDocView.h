/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

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

#ifndef _TEXT_DOC_VIEW_H_
#define _TEXT_DOC_VIEW_H_

class DocFindFlags;
class QTextCursor;
class QTextDocument;
class TDViewInterior;

//	Qt headers
#include <QtCore/QList>

//	local headers
#include "DocView.h"

typedef QList<int> IntList;

class TextDocView : public DocView {
Q_OBJECT
public:
	TextDocView(QWidget*);
	virtual ~TextDocView();

	bool lineNumIsVisible() const;
	void setLineNumVisible(bool);
	void showHiddenSymbols(bool);
	bool hiddenSymbolsVisible() const;
	bool isAdjustedByWidth() const;
	void setAdjustedByWidth(bool adjust);
	void setModified(bool);
	void getText(QString&) const;
	void setText(const QString&);
	void insertText(const QString&);
	void getSelectedText(QString&) const;
	void replaceSelectedText(const QString&);
	void setSelection(int, int, int, int);
	int scrollPos() const;
	void setScrollPos(int);
	void getCursorPos(int&, int&) const;
	void setCursorPos(int, int);
	void gotoLine(int) const;
	void unindentSelectedLines();
	void unindentLines(int, int);
	int lineCount() const;
	void setSyntax(const QString&);
	QString syntax() const;
	virtual void applySettings();

	virtual void setDocument(Juff::Document*);

	void cut();
	void copy();
	void paste();
	void undo();
	void redo();

	void find(const QString& str, bool isRegExp, DocFindFlags flags);
	void replace(const QString& from, bool isRegExp, const QString& to, DocFindFlags flags);

	void toggleMarker();
	void gotoNextMarker();
	void gotoPrevMarker();
	void removeAllMarkers();
	IntList markers() const;
	QString markedLine(int) const;
	
signals:
	void modified(bool);
	void cursorPositionChanged(int, int);
	
protected:	
	virtual void resizeEvent(QResizeEvent*);
	
protected slots:	
	void rehighlight();
	void updateLineNums();
	
private:
	bool doReplace(const QString& text, bool& replaceAll);
	bool continueOverTheEnd(bool back);

	TDViewInterior* vInt_;
};

#endif
