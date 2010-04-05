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

#ifndef __JUFF_SETTINGS_ITEM_H__
#define __JUFF_SETTINGS_ITEM_H__

#include <QtCore/QObject>

#include "Settings.h"

class SettingsChangeNotifier : public QObject {
Q_OBJECT
public:
	SettingsChangeNotifier();

	void notifyHasChanged();
	void notifyNoChanged();

signals:
	void hasChangedItems(bool);
};

class SettingsItem : public Settings {
public:
	virtual void readValue() = 0;
	virtual void writeValue() = 0;

	static SettingsChangeNotifier* notifier();
	
protected:
	SettingsItem();
	
	static void oneMoreChanged();
	static void oneLessChanged();

private:
	static int counter_;
	static SettingsChangeNotifier* notifier_;
};

#endif /* __JUFF_SETTINGS_ITEM_H__ */
