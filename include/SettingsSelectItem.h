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

#ifndef __JUFF_SETTINGS_SELECT_ITEM_H__
#define __JUFF_SETTINGS_SELECT_ITEM_H__

#include "LibConfig.h"
#include "SettingsItem.h"

class QComboBox;

class LIBJUFF_EXPORT SettingsSelectItem : public QObject, public SettingsItem {
Q_OBJECT
public:
	enum Mode {
		IndexMode,
		StringMode
	};
	SettingsSelectItem(const QString&, const QString&, QComboBox*, Mode mode);

	virtual void readValue();
	virtual void writeValue();

private slots:
	void onSelected(int);

private:
	bool isString_;
	QComboBox* comboBox_;
	QString section_;
	QString key_;
	int curIndex_;
	QString curString_;
	Mode mode_;
};

#endif /* __JUFF_SETTINGS_SELECT_ITEM_H__ */
