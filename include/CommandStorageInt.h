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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef __JUFFED_COMMAND_STORAGE_INTERFACE_H__
#define __JUFFED_COMMAND_STORAGE_INTERFACE_H__

class QAction;
class QObject;
class QKeySequence;
class QString;
class QStringList;

class CommandStorageInt {
public:
	CommandStorageInt() {}
	virtual ~CommandStorageInt() {}

	/**
	* Adds an action with given \param key and given \param name attached to the \param slot of the \param obj.
	*/
	virtual void addAction(const QString& key, const QString& name, QObject* obj, const char* slot) = 0;

	/**
	* Adds an action with given \param key and given \param action.
	*/
	virtual void addAction(const QString& key, QAction* action) = 0;

	/**
	* Returns an action with given \param id.
	*/
	virtual QAction* action(const QString& id) const = 0;

	/**
	* Returns shortcut for the action with given \param id.
	*/
	virtual QKeySequence shortcut(const QString& id) const = 0;

	/**
	* Sets the \param shortcut for the action with a given \param key.
	*/
	virtual void setShortcut(const QString& key, const QKeySequence& shortcut) = 0;

	/**
	* Updates actions' icons using current settings.
	*/
	virtual void updateIcons() = 0;
	
	/**
	* Returns a list of actions IDs (strings).
	*/
	virtual QStringList actionIDs() const = 0;
};

#endif // __JUFFED_COMMAND_STORAGE_INTERFACE_H__
