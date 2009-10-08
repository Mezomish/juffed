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

#ifndef __JUFF_H__
#define __JUFF_H__

class QMenu;
class QToolBar;
class QAction;
	
#include <QtCore/QList>

#include "Log.h"

namespace Juff {

enum CommandID {
	ID_NONE,                  //  == 0
	ID_SEPARATOR,             //  == 1
	//
	ID_FILE_NEW,              //  == 2
	ID_FILE_OPEN,             //  == 3
	ID_FILE_SAVE,             //  == 4
	ID_FILE_SAVE_AS,          //  == 5
	ID_FILE_SAVE_ALL,         //  == 6
	ID_FILE_RELOAD,           //  == 7
	ID_FILE_CLOSE,            //  == 8
	ID_FILE_CLOSE_ALL,        //  == 9
	ID_FILE_PRINT,            //  == 10
	ID_EXIT,                  //  == 11
	//
	ID_SESSION_NEW,           //  == 12
	ID_SESSION_OPEN,          //  == 13
	ID_SESSION_SAVE,          //  == 14
	ID_SESSION_SAVE_AS,       //  == 15
	//
	ID_DOC_NEXT,              //  == 16
	ID_DOC_PREV,              //  == 17
	//
	ID_EDIT_UNDO,             //  == 18
	ID_EDIT_REDO,             //  == 19
	ID_EDIT_CUT,              //  == 20
	ID_EDIT_COPY,             //  == 21
	ID_EDIT_PASTE,            //  == 22
	ID_EDIT_SELECT_ALL,       //  == 23
	//
	ID_FIND,                  //  == 24
	ID_FIND_NEXT,             //  == 25
	ID_FIND_PREV,             //  == 26
	ID_REPLACE,               //  == 27
	ID_GOTO_LINE,             //  == 28
	//
	ID_SHOW_LINE_NUMBERS,     //  == 29
	ID_WRAP_TEXT,             //  == 30
	ID_SHOW_HIDDEN_SYMBOLS,   //  == 31
	//
	ID_ZOOM_IN,               //  == 32
	ID_ZOOM_OUT,              //  == 33
	ID_ZOOM_100,              //  == 34
	//
	ID_EOL_WIN,               //  == 35
	ID_EOL_MAC,               //  == 36
	ID_EOL_UNIX,              //  == 37
	//
	ID_MARKER_TOGGLE,         //  == 38
	ID_MARKER_NEXT,           //  == 39
	ID_MARKER_PREV,           //  == 40
	ID_MARKER_REMOVE_ALL,     //  == 41
	//
	ID_SETTINGS,              //  == 42
	ID_ABOUT,                 //  == 43
	ID_ABOUT_QT,              //  == 44
	//
	//
	ID_FILE_NEW_RICH,
	//
	ID_LAST_ITEM, // THE LAST ITEM FOR STANDARD ITEMS
	
	ID_SCI_BASE_ITEM = 1000
};

enum InfoEvent {
	INFO_DOC_CREATED,       // param1: fileName
	INFO_DOC_ACTIVATED,     // param1: newActiveFileName
	INFO_DOC_MODIFIED,      // param1: fileName,            param2:isModified()
	INFO_DOC_CLOSED,        // param1: fileName
	INFO_DOC_NAME_CHANGED,  // param1: oldFileName,         param2: newFileName
};

enum GetInfoEvent {
	GET_DOC_LIST,
	GET_CUR_DOC_NAME,
	GET_DOC_TEXT,
	GET_CURSOR_POS,
	GET_SELECTION,
	GET_SELECTED_TEXT,
};

enum DocCommand {
	ACTIVATE_DOC,
	SET_CURSOR_POS,
	SET_SELECTION,
	REMOVE_SELECTED_TEXT,
	REPLACE_SELECTED_TEXT,
	INSERT_TEXT,
};

enum MenuID {
	ID_MENU_NONE,
	ID_MENU_FILE,
	ID_MENU_EDIT,
	ID_MENU_VIEW,
	ID_MENU_FORMAT,
	ID_MENU_TOOLS,
};


typedef struct _DocFindFlags {
	_DocFindFlags(bool Replace = false, bool MatchCase = false, bool Backwards = false, 
			bool IsRegExp = false, bool WholeWords = false, bool MultiLine = false) {
		replace = Replace;
		matchCase = MatchCase;
		backwards = Backwards;
		isRegExp = IsRegExp;
		wholeWords = WholeWords;
		multiLine = MultiLine;
	}
	bool replace;
	bool matchCase;
	bool backwards;
	bool isRegExp;
	bool wholeWords;
	bool multiLine;
} DocFindFlags;

typedef QList<QMenu*> MenuList;
typedef QList<QToolBar*> ToolBarList;
typedef QList<QAction*> ActionList;
typedef QList<int> IntList;

}	//	namespace Juff

#endif // __JUFF_H__
