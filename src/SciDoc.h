/*
JuffEd - A simple text editor
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

namespace Juff {

class MyQScintilla;

class SciDoc : public Document {
Q_OBJECT
public:
	SciDoc(const QString&);
	virtual ~SciDoc();
	
	virtual QWidget* widget();
	virtual bool isModified() const;
	virtual void setModified(bool);
	virtual bool save(const QString&, QString&);

	virtual void undo();
	virtual void redo();
	virtual void cut();
	virtual void copy();
	virtual void paste();

	virtual void find(const QString&, const DocFindFlags&);
	virtual void replace(const QString&, const QString&, const DocFindFlags&);
	virtual void gotoLine(int);
	virtual QString text() const;
	virtual QString selectedText() const;
	virtual void getCursorPos(int&, int&) const;
	virtual void setCursorPos(int, int);
	virtual void getSelection(int&, int&, int&, int&) const;
	virtual void setSelection(int, int, int, int);
	virtual void insertText(const QString&);
	virtual void removeSelectedText();
	virtual int curLine() const;
	virtual int curScrollPos() const;
	virtual void setScrollPos(int);

	virtual int lineCount() const;
	virtual QString charset() const;
	virtual void setCharset(const QString&);

	virtual void applySettings();
	virtual void updateActivated();

	QString syntax() const;
	void setSyntax(const QString& lexName);
//	void getCursorPos(int&, int&);
	IntList markers() const;
	QString getLine(int) const;

	void showLineNumbers(bool);
	void wrapText(bool);
	void zoomIn();
	void zoomOut();
	void zoom100();
	
	void toggleMarker();
	void nextMarker();
	void prevMarker();
	void removeAllMarkers();

signals:
//	QMenu* getMarkersMenu();
	
private:
	void readDoc();

	bool doReplace(MyQScintilla* edit, const QString& str1, const QString& str2, const DocFindFlags& flags, bool& replaceAll);
	MyQScintilla* getActiveEdit() const;

	class Interior;
	Interior* docInt_;
};

}	//	namespace Juff

#endif
