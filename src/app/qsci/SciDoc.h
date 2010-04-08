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

#ifndef __JUFFED_SCI_DOC_H__
#define __JUFFED_SCI_DOC_H__

class JuffScintilla;

#include "Document.h"

class SciDoc : public Juff::Document {
Q_OBJECT
public:
	SciDoc(const QString&);
	virtual ~SciDoc();

	virtual int lineCount() const;
	virtual bool isModified() const;
	virtual bool hasSelectedText() const;
	virtual bool getSelection(int&, int&, int&, int&) const;
	virtual bool getSelectedText(QString&) const;
	virtual bool getText(QString&) const;
	virtual QString textLine(int) const;
	virtual bool getCursorPos(int&, int&) const;
	virtual QString syntax() const;
	virtual void setModified(bool);
	virtual void setSelection(int, int, int, int);
	virtual void removeSelectedText();
	virtual void replaceSelectedText(const QString&);
	virtual void insertText(const QString&);
	virtual void setCursorPos(int, int);
	virtual void setSyntax(const QString& lexName);


	virtual QString type() const;
//	virtual Juff::Document* createClone();
	virtual bool supportsAction(Juff::ActionID) const;
	virtual void init();
	virtual void print();
	virtual void reload();
	virtual bool save(QString& error);

	virtual void undo();
	virtual void redo();
	virtual void cut();
	virtual void copy();
	virtual void paste();
	virtual void gotoLine(int);

	virtual bool find(const Juff::SearchParams&);

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
	
	void toUpperCase();
	void toLowerCase();
	void swapLines();
	void moveUp();
	void moveDown();
	
	void toggleCommentLines();
	void toggleCommentBlock();
	void duplicateText();
	
private slots:
	void onCursorMoved(int, int);
	void onMarginClicked(int, int, Qt::KeyboardModifiers);
	void onLineCountChanged();
	void onEditFocused();

protected:
	SciDoc(Juff::Document*);
//	virtual void updateClone();

private:
	void readFile();
	void setLexer(const QString& lexName);
	void applySettings();
	
	void commentLine(JuffScintilla* edit, int line, const QString& str1, const QString& comment);
	void uncommentLine(JuffScintilla* edit, int line, const QString& str1, const QString& comment);
	void stripTrailingSpaces();

	class Interior;
	Interior* int_;
};

#endif // __JUFFED_SCI_DOC_H__
