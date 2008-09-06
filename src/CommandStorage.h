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

#ifndef _COMMAND_STORAGE_H_
#define _COMMAND_STORAGE_H_

class QAction;

//	Qt
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtGui/QKeySequence>

typedef enum {
	ID_NONE				= 0,
	ID_SEPARATOR		= 1000,
	//
	ID_FILE_NEW			= 1001,
	ID_FILE_OPEN		= 1002,
	ID_FILE_SAVE		= 1003,
	ID_FILE_SAVE_AS		= 1004,
	ID_FILE_CLOSE		= 1005,
	ID_FILE_CLOSE_ALL	= 1006,
	ID_EXIT				= 1007,
	ID_FILE_RELOAD		= 1008,
	ID_FILE_PRINT		= 1009,
	ID_FILE_PRINT_SELECTED = 1010,
	//
	ID_EDIT_CUT			= 1101,
	ID_EDIT_COPY		= 1102,
	ID_EDIT_PASTE		= 1103,
	ID_EDIT_UNDO		= 1104,
	ID_EDIT_REDO		= 1105,
	ID_UNINDENT			= 1106,
	ID_ZOOM_IN			= 1107,
	ID_ZOOM_OUT			= 1108,
	//
	ID_FIND				= 1201,
	ID_FIND_NEXT		= 1202,
	ID_FIND_PREV		= 1203,
	ID_GOTO_LINE		= 1204,
	//
	ID_DOC_NEXT			= 1301,
	ID_DOC_PREV			= 1302,
	//
	ID_SETTINGS			= 1401,
	ID_ABOUT			= 1402,
	ID_ABOUT_QT			= 1403,
	//
	ID_SESSION_NEW		= 1501,
	ID_SESSION_OPEN		= 1502,
	ID_SESSION_SAVE		= 1503,
	ID_SESSION_SAVE_AS	= 1504,
	//
	ID_VIEW_SHOW_LINE_NUMBERS	= 1601,
	ID_VIEW_WIDTH_ADJUST		= 1602,
	ID_VIEW_SHOW_HIDDEN_SYMBOLS	= 1603,
	//
	ID_MARKER_TOGGLE	= 1701,
	ID_MARKER_REMOVE_ALL= 1702,
	ID_MARKER_NEXT		= 1703,
	ID_MARKER_PREV		= 1704
} CommandID;

struct Command {
	Command() {
		id = ID_NONE;
	}
	Command(CommandID Id, QString Name, QIcon Icon, QKeySequence Seq, QObject* Reciever, const QString& Slot) {
		id = Id;
		name = Name;
		icon = Icon;
		shortcut = Seq;
		reciever = Reciever;
		slot = Slot;
	}

	CommandID id;
	QString name;
	QIcon icon;
	QKeySequence shortcut;
	QObject* reciever;
	QString slot;
};

class CommandStorage {
public:
	static CommandStorage* instance();

	void registerCommand(const Command&);
	QAction* action(CommandID);

private:
	CommandStorage() { }
	static CommandStorage* st_;
	QMap<CommandID, QAction*> cmds_;
};

#endif
