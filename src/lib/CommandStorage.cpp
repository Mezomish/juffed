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

#include "CommandStorage.h"

//	Qt
#include <QtGui/QAction>

namespace Juff {

CommandStorage* CommandStorage::instance() {
	if ( !st_ ) {
		st_ = new CommandStorage();
	}

	return st_;
}

void CommandStorage::registerExtCommand(int id, QAction* action) {
	if ( cmds_.contains(id) )
		delete action;

	cmds_[id] = action;
}

void CommandStorage::registerCommand(CommandID id, QObject* receiver, const QString& slot) {
	if ( cmds_.contains((int)id) )
		delete cmds_[id];

	QAction* action = new QAction(getIcon(id), getName(id), 0);
	action->setShortcut(getShortcut(id));
	action->setData(id);
	QObject::connect(action, SIGNAL(triggered()), receiver, qPrintable(slot));
	cmds_[(int)id] = action;
}

QIcon CommandStorage::getIcon(CommandID id) {
	return IconManager::instance()->getIcon(id);
}

QString CommandStorage::getName(CommandID id) {
	switch (id) {
		case ID_FILE_NEW :          return QObject::tr("New");
		case ID_FILE_NEW_RICH :     return QObject::tr("New Rich text");
		case ID_FILE_OPEN :         return QObject::tr("Open");
		case ID_FILE_SAVE :         return QObject::tr("Save");
		case ID_FILE_SAVE_AS :      return QObject::tr("Save as");
		case ID_FILE_RELOAD :       return QObject::tr("Reload");
		case ID_FILE_CLOSE :        return QObject::tr("Close");
		case ID_FILE_CLOSE_ALL :    return QObject::tr("Close all");
		case ID_FILE_PRINT :        return QObject::tr("Print");
		case ID_EXIT :              return QObject::tr("Exit");

		case ID_SESSION_NEW :       return QObject::tr("New session");
		case ID_SESSION_OPEN :      return QObject::tr("Open session");
		case ID_SESSION_SAVE :      return QObject::tr("Save session");
		case ID_SESSION_SAVE_AS :   return QObject::tr("Save session as");
		
		case ID_EDIT_UNDO :         return QObject::tr("Undo");
		case ID_EDIT_REDO :         return QObject::tr("Redo");
		case ID_EDIT_CUT :          return QObject::tr("Cut");
		case ID_EDIT_COPY :         return QObject::tr("Copy");
		case ID_EDIT_PASTE :        return QObject::tr("Paste");

		case ID_FIND :              return QObject::tr("Find");
		case ID_FIND_NEXT :         return QObject::tr("Find next");
		case ID_FIND_PREV :         return QObject::tr("Find previous");
		case ID_REPLACE :           return QObject::tr("Replace");
		case ID_GOTO_LINE :         return QObject::tr("Go to line");

		case ID_ZOOM_IN :           return QObject::tr("Zoom in");
		case ID_ZOOM_OUT :          return QObject::tr("Zoom out");
		case ID_ZOOM_100 :          return QObject::tr("Zoom 100%");

		case ID_EOL_WIN :           return QObject::tr("Win");
		case ID_EOL_MAC :           return QObject::tr("Mac");
		case ID_EOL_UNIX :          return QObject::tr("Unix");

		case ID_MARKER_TOGGLE :     return QObject::tr("Add/Remove marker");
		case ID_MARKER_NEXT :       return QObject::tr("Next marker");
		case ID_MARKER_PREV :       return QObject::tr("Previous marker");
		case ID_MARKER_REMOVE_ALL : return QObject::tr("Remove all markers");

		case ID_SETTINGS :          return QObject::tr("Settings");
		case ID_ABOUT :             return QObject::tr("About");
		case ID_ABOUT_QT :          return QObject::tr("About Qt");

		default:                    return QObject::tr("N/A");
	}
}

QKeySequence CommandStorage::getShortcut(CommandID id) {
	switch (id) {
		case ID_FILE_NEW :          return QKeySequence("Ctrl+N");
		case ID_FILE_NEW_RICH :     return QKeySequence("Ctrl+M");
		case ID_FILE_OPEN :         return QKeySequence("Ctrl+O");
		case ID_FILE_SAVE :         return QKeySequence("Ctrl+S");
		case ID_FILE_SAVE_AS :      return QKeySequence("Shift+Ctrl+S");
		case ID_FILE_RELOAD :       return QKeySequence("F5");
		case ID_FILE_CLOSE :        return QKeySequence("Ctrl+W");
		case ID_FILE_PRINT :        return QKeySequence("Ctrl+P");
		case ID_EXIT:               return QKeySequence("Ctrl+Q");
			
		case ID_EDIT_UNDO :         return QKeySequence("Ctrl+Z");
		case ID_EDIT_REDO :         return QKeySequence("Ctrl+Y");
		case ID_EDIT_CUT :          return QKeySequence("Ctrl+X");
		case ID_EDIT_COPY :         return QKeySequence("Ctrl+C");
		case ID_EDIT_PASTE :        return QKeySequence("Ctrl+V");

		case ID_FIND :              return QKeySequence("Ctrl+F");
		case ID_FIND_NEXT :         return QKeySequence("F3");
		case ID_FIND_PREV :         return QKeySequence("Shift+F3");
		case ID_REPLACE :           return QKeySequence("Ctrl+R");
		case ID_GOTO_LINE :         return QKeySequence("Ctrl+G");

		case ID_MARKER_TOGGLE :     return QKeySequence("Ctrl+B");
		case ID_MARKER_NEXT :       return QKeySequence("Ctrl+Alt+PgDown");
		case ID_MARKER_PREV :       return QKeySequence("Ctrl+Alt+PgUp");

		case ID_DOC_NEXT :          return QKeySequence("Alt+Right");
		case ID_DOC_PREV :          return QKeySequence("Alt+Left");
			
		case ID_ABOUT :             return QKeySequence("F1");
			
		default:                    return QKeySequence("");
	}
}


QAction* CommandStorage::action(int id) {
	if ( id == ID_SEPARATOR ) {
		QAction* sep = new QAction(0);
		sep->setSeparator(true);
		return sep;
	}
	
	return cmds_[id];
}

void CommandStorage::updateIcons() {
	QMap<int, QAction*>::iterator it = cmds_.begin();
	for ( ; it != cmds_.end(); it++) {
		QIcon icon = getIcon((CommandID)it.key());
		it.value()->setIcon(icon);
	}
}


CommandStorage* CommandStorage::st_ = 0;

};	//	namespace Juff
