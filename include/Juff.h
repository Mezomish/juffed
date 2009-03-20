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

#ifndef _JUFF_H_
#define _JUFF_H_

class QMenu;
class QToolBar;
class QAction;
	
#include <QtCore/QList>

#include "Log.h"

namespace Juff {

enum CommandID {
	ID_NONE,
	ID_SEPARATOR,
	//
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
	ID_FILE_SAVE_AS,
	ID_FILE_RELOAD,
	ID_FILE_CLOSE,
	ID_FILE_CLOSE_ALL,
	ID_FILE_PRINT,
	ID_EXIT,
	//
	ID_SESSION_NEW,
	ID_SESSION_OPEN,
	ID_SESSION_SAVE,
	ID_SESSION_SAVE_AS,
	//
	ID_DOC_NEXT,
	ID_DOC_PREV,
	//
	ID_EDIT_UNDO,
	ID_EDIT_REDO,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
	ID_EDIT_SELECT_ALL,
	//
	ID_FIND,
	ID_FIND_NEXT,
	ID_FIND_PREV,
	ID_REPLACE,
	ID_GOTO_LINE,
	//
	ID_SHOW_LINE_NUMBERS,
	ID_WRAP_TEXT,
	ID_SHOW_HIDDEN_SYMBOLS,
	//
	ID_ZOOM_IN,
	ID_ZOOM_OUT,
	ID_ZOOM_100,
	//
	ID_EOL_WIN,
	ID_EOL_MAC,
	ID_EOL_UNIX,
	//
	ID_MARKER_TOGGLE,
	ID_MARKER_NEXT,
	ID_MARKER_PREV,
	ID_MARKER_REMOVE_ALL,
	//
	ID_SETTINGS,
	ID_ABOUT,
	ID_ABOUT_QT,
	//
	//
	ID_FILE_NEW_RICH,
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
	ID_MENU_FILE,
	ID_MENU_EDIT,
	ID_MENU_VIEW,
	ID_MENU_FORMAT,
	ID_MENU_TOOLS,
};


typedef struct _DocFindFlags {
	_DocFindFlags(bool Replace = false, bool MatchCase = false, bool Backwards = false, bool IsRegExp = false) {
		replace = Replace;
		matchCase = MatchCase;
		backwards = Backwards;
		isRegExp = IsRegExp;
	}
	bool replace;
	bool matchCase;
	bool backwards;
	bool isRegExp;
} DocFindFlags;

typedef QList<QMenu*> MenuList;
typedef QList<QToolBar*> ToolBarList;
typedef QList<QAction*> ActionList;
typedef QList<int> IntList;

}	//	namespace Juff

#endif
