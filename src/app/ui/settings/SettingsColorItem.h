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

#ifndef __JUFF_SETTINGS_COLOR_ITEM_H__
#define __JUFF_SETTINGS_COLOR_ITEM_H__

#include "SettingsItem.h"

class ColorButton;

class SettingsColorItem : public QObject, public SettingsItem {
Q_OBJECT
public:
	SettingsColorItem(const QString&, const QString&, ColorButton*);

	virtual void readValue();
	virtual void writeValue();

//private slots:
//	void onChecked(bool);

private:
	ColorButton* colorBtn_;
	QString section_;
	QString key_;
//	bool default_;
//	bool curValue_;
};

#endif // __JUFF_SETTINGS_COLOR_ITEM_H__
