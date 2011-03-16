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

#ifndef __JUFFED_COMMAND_STORAGE_H__
#define __JUFFED_COMMAND_STORAGE_H__

#include "LibConfig.h"

#include "CommandStorageInt.h"

class IconManagerInt;
class QAction;
class QKeySequence;

#include <QObject>
#include <QMap>

class LIBJUFF_EXPORT CommandStorage : public QObject, public CommandStorageInt {
public:
	CommandStorage(IconManagerInt* mgr);

	/**
	* Adds an action with given \param key and given \param name attached to the \param slot of the \param obj.
	*/
	void addAction(const QString& key, const QString& name, QObject* obj, const char* slot);

	/**
	* Adds an action with given \param key and given \param action.
	*/
	virtual void addAction(const QString& key, QAction* action);

	/**
	* Returns an action with given \param id.
	*/
	QAction* action(const QString& id) const;

	/**
	* Returns shortcut for the action with given \param id.
	*/
	QKeySequence shortcut(const QString& id) const;

	/**
	* Sets the \param shortcut for the action with a given \param key.
	*/
	void setShortcut(const QString& key, const QKeySequence& shortcut);

	/**
	* Updates actions' icons using current settings.
	*/
	void updateIcons();

	/**
	* Returns a list of actions IDs (strings).
	*/
	virtual QStringList actionIDs() const;
	
private:

	QMap<QString, QAction*> actions_;
	QMap<QString, QKeySequence> keys_;
	IconManagerInt* iconManager_;
};

#endif // __JUFFED_COMMAND_STORAGE_H__
