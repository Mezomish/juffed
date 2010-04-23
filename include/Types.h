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

#ifndef __JUFFED_TYPES_H__
#define __JUFFED_TYPES_H__

#include <QAction>
#include <QList>
#include <QMenu>

namespace Juff {

typedef QList<QMenu*> MenuList;
typedef QList<QAction*> ActionList;

typedef struct {
	/**
	* The string needs to be find.
	*/
	QString findWhat;
	
	/**
	* The string used for replaces (can be 
	* a regular expression - see 'regExp' parameter).
	*/
	QString replaceWith;
	
	/**
	* The flag indicating whether the currently called action
	* was 'Find' or 'Replace'.
	*/
	bool replace;
	
	/**
	* The flag indicating whether the search should be case-sensitive.
	*/
	bool caseSensitive;
	
	/**
	* The flag indicating whether the search is going to be performed
	* backwards (up to the document's top).
	*/
	bool backwards;
	
	/**
	* The flag indicating whether the search should be only for 
	* the whole words.
	*/
	bool wholeWords;
	
	/**
	* The flag indicating whether the search string is a regular expression.
	*/
	bool regExp;
	
	/**
	* The flag indicating whether the regular is multi-line.
	* TODO : add more detailed explanation here
	*/
	bool multiLine;
} SearchParams;

}

#endif // __JUFFED_TYPES_H__
