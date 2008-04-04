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

#include "CommandStorage.h"

//	Qt
#include <QtGui/QAction>

CommandStorage* CommandStorage::instance() {
	if (st_ == 0)
		st_ = new CommandStorage();

	return st_;
}

void CommandStorage::registerCommand(const Command& cmd) {
	CommandID id = cmd.id;

	if (cmds_.contains(id))
		delete cmds_[id];

	QAction* action = new QAction(cmd.icon, cmd.name, 0);
	action->setShortcut(cmd.shortcut);
	action->setData(id);
	QObject::connect(action, SIGNAL(triggered()), cmd.reciever, qPrintable(cmd.slot));
	cmds_[id] = action;
}

QAction* CommandStorage::action(CommandID id) {
	return cmds_[id];
}

CommandStorage* CommandStorage::st_ = 0;
