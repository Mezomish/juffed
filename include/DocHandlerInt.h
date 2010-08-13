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

#ifndef __JUFFED_DOC_HANDLER_INT_H__
#define __JUFFED_DOC_HANDLER_INT_H__

namespace Juff {

class Document;
class Project;

class DocHandlerInt {
public:
	/**
	* Returns current document. If there is no documents then returns a NullDoc.
	*/
	virtual Juff::Document* curDoc() const = 0;

	/**
	* Returns document by file name. If there is no such document opened then returns a NullDoc.
	*/
	virtual Juff::Document* getDoc(const QString&) const = 0;

	/**
	* Returns current project.
	*/
	virtual Juff::Project* curPrj() const = 0;

	/**
	* Opens a document with a given file name or activates it if it is already opened.
	*/
	virtual void openDoc(const QString&, int panel = -1) = 0;

	/**
	* Closes the document with a given file name.
	*/
	virtual void closeDoc(const QString&) = 0;

	/**
	* Saves the document with a given file name.
	*/
	virtual void saveDoc(const QString&) = 0;

	/**
	* Returns the number of currently opened documents.
	*/
	virtual int docCount() const = 0;

	/**
	* Returns the list of currently opened documents.
	*/
	virtual QStringList docList() const = 0;
};

}

#endif // __JUFFED_DOC_HANDLER_INT_H__
