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

#include "Enums.h"

#include <QWidget>

namespace Juff {

class Document : public QWidget {
Q_OBJECT
public:
	Document(const QString&);
	virtual ~Document();

	QString fileName() const;
	QString charset() const { return charset_; }
	void setCharset(const QString&);
	
	virtual int lineCount() const { return 0; }
	virtual bool isModified() const { return false; }
	virtual bool hasSelectedText() const { return false; }
	virtual bool getSelection(int&, int&, int&, int&) const { return false; }
	virtual bool getSelectedText(QString&) const { return false; }
	virtual bool getCursorPos(int&, int&) const { return false; }
	virtual QString syntax() const { return ""; }
	virtual void setModified(bool) {}
	virtual void setSelection(int, int, int, int) {}
	virtual void removeSelectedText() {}
	virtual void replaceSelectedText(const QString&) {}
	virtual void insertText(const QString&) {}
	virtual void setCursorPos(int, int) {}
	virtual void setSyntax(const QString& lexName) {}
	
	
//	Juff::Document* clone() const { return clone_; }
//	bool hasClone() const { return clone_ != NULL; }

	virtual QString type() const = 0;
	virtual bool supportsAction(Juff::ActionID) const;
	virtual void init() {}
	virtual bool save(QString& error) { Q_UNUSED(error); return false; }
	virtual void reload() {}
	virtual void print() {}
	bool saveAs(const QString& fileName, QString& error);
	
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
	
private:
	QString fileName_;
	QString charset_;
	QTextCodec* codec_;
//	Juff::Document* clone_;
	static int sCount_;
};

}

#endif // __JUFFED_DOCUMENT_H__
