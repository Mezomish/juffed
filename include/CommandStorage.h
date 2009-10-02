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

#ifndef __JUFF_COMMAND_STORAGE_H__
#define __JUFF_COMMAND_STORAGE_H__

class QAction;

//	local headers
#include "IconManager.h"
#include "Juff.h"

//	Qt headers
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtGui/QKeySequence>

namespace Juff {

/**
* Command
*
* This struct represents a command action and contains 
* all necessary information about it.
*/
struct Command {
	Command() {
		id = ID_NONE;
		receiver = NULL;
	}
	Command(CommandID ID, QString nm, QKeySequence seq, QObject* rcvr, const QString& slt) {
		id = ID;
		name = nm;
		icon = IconManager::instance()->getIcon(id);
		shortcut = seq;
		receiver = rcvr;
		slot = slt;
	}

	CommandID id;           //  command ID (see enum Juff::CommandID
	QString name;           //  command name (e.g. "New", "Open", etc.)
	QIcon icon;             //  command icon
	QKeySequence shortcut;  //  command shortcut (can be combination of two, see QKeySequence reference)
	QObject* receiver;      //  object that receives activation signal
	QString slot;           //  slot of receiving object that is activated by the action
};

/**
* CommandStorage
*
* Singleton class that contains, stores 
* and manages application's actions.
*/
class CommandStorage {
public:
	/**
	* instance()
	*
	* Singleton instance getter
	*/
	static CommandStorage* instance();

	/**
	* registerCommand()
	*
	* Registers a command and stores it. After registering 
	* the command can be accessed through \ref action() function.
	*/
	void registerCommand(CommandID id, QObject* receiver, const QString& slot);

	/**
	* registerExtCommand()
	*
	* Registers an "extended" command.
	*/
	void registerExtCommand(int, QAction*);

	/**
	* action()
	*
	* Returns action with requested ID.
	*
	* PARAMS:	id          - requested command ID
	*
	* RETURN:   command     - if the command with that ID exists
	*           0           - otherwise
	*/
	QAction* action(int id);

	/**
	* updateIcons()
	*
	* Updates actions' icons according to the current icon theme
	*/
	void updateIcons();
	
	void updateShortcuts();
	
private:
	/**
	* CommandStorage()
	*
	* Constructor
	*/
	CommandStorage() { }
	
	QIcon getIcon(CommandID);
	QString getName(CommandID);
	QKeySequence getShortcut(CommandID);
	
	static CommandStorage* st_;			///< storage instance
	QMap<int, QAction*> cmds_;	///< map for storing action
};

};	//	namespace Juff

#endif // __JUFF_COMMAND_STORAGE_H__
