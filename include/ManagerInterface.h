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

#ifndef _JUFF_MANAGER_INTERFACE_H_
#define _JUFF_MANAGER_INTERFACE_H_

#include <QtCore/QString>
#include <QtCore/QStringList>

class ManagerInterface {
public:
	/**
	*	openDoc()
	*
	*	Opens the document with file name \par fileName or
	*	activates it if already opened.
	*/
	virtual void openDoc(const QString& fileName) = 0;

	/**
	*	closeDoc()
	*
	*	Closes the document with file name \par fileName.
	*/
	virtual bool closeDoc(const QString& fileName) = 0;

	/**
	*	saveDoc()
	*
	*	Saves the document with file name \par fileName.
	*/
	virtual void saveDoc(const QString& fileName) = 0;

	/**
	* docList()
	*
	* Returns the list of currently opened docs
	*/
	virtual void getDocList(QStringList&) const = 0;

	/**
	* curDocName()
	*
	* Returns the fileName of the current doc or 
	* empty string if no documents are opened.
	*/
	virtual void getCurDocName(QString&) const = 0;

	/**
	* getCurrentDocText()
	*
	* Returns the \par text of the current document.
	*/
	virtual void getCurrentDocText(QString& text) = 0;

	/**
	* getCurrentDocText()
	*
	* Returns one line of \par text of the current document.
	* Parameter \par contains line index (starting with 0).
	*/
	virtual void getCurrentDocText(int line, QString& text) = 0;

	/**
	* getDocText()
	*
	* Returns the \par text of the document with name \par fileName
	*/
	virtual void getDocText(const QString& fileName, QString& text) = 0;

	/**
	* getDocText()
	*
	* Returns one line of \par text of the document with name \par fileName.
	* Parameter \par contains line index (starting with 0).
	*/
	virtual void getDocText(const QString& fileName, int line, QString& text) = 0;

	/**
	* getCursorPos()
	*
	* Returns the current documen's cursor position or (-1, -1)
	* if no documents are opened.
	*/
	virtual void getCursorPos(int& line, int& column) = 0;

	/**
	* getSelection()
	*
	* Returns the current document's selection borders or (-1, -1, -1, -1)
	* if no documents are opened.
	*/
	virtual void getSelection(int& line1, int& column1, int& line2, int& column2) = 0;

	/**
	* getSelectedText()
	*
	* Returns the currently selected \par text.
	*/
	virtual void getSelectedText(QString& text) = 0;

	/**
	* setCursorPos()
	*
	  Sets the current document's cursor position to ( \par line, \par col ).
	*/
	virtual void setCursorPos(int line, int col) = 0;

	/**
	*	setSelection()
	*
	*	Selects the text in current document from ( \par line1, \par column1) 
	*	to ( \par line2, \par column2).
	*/
	virtual void setSelection(int line1, int column1, int line2, int column2) = 0;

	/**
	*	removeSelectedText()
	*
	*	Removes current document's selected text.
	*/
	virtual void removeSelectedText() = 0;

	/**
	*	replaceSelectedText()
	*
	*	Replaces current document's selected text with \par text.
	*/
	virtual void replaceSelectedText(const QString& text) = 0;

	/**
	*	insertText()
	*
	*	Inserts the text \par text to the current cursor position.
	*/
	virtual void insertText(const QString& text) = 0;
};

#endif
