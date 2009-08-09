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

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

class QString;
class SettingsData;

#include <QtCore/QStringList>
#include <QtCore/QVariant>

class Settings {
public:
	static void read();
	static void write();

	static int count();

	static QVariant defaultValue(const QString& section, const QString& key);

protected:
	static bool valueExists(const QString& section, const QString& key);
	static QString stringValue(const QString& section, const QString& key, const QString& def = "");
	static int intValue(const QString& section, const QString& key, int def = 0);
	static bool boolValue(const QString& section, const QString& key);
	static QVariant value(const QString& section, const QString& key, const QVariant& defValue = QVariant());

	static void setValue(const QString& section, const QString& key, const QVariant& value);

	static QStringList sectionList();
	static QStringList keyList(const QString& section);

private:
	static SettingsData* settData_;
};

#endif
