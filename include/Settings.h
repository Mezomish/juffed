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

#ifndef __JUFFED_SETTINGS_H__
#define __JUFFED_SETTINGS_H__

#include "LibConfig.h"

class QString;

#include <QtCore/QStringList>
#include <QtCore/QVariant>

class LIBJUFF_EXPORT Settings {
public:
	static Settings* instance();

	void read(const QString&, const QString&);
	void write(const QString&, const QString&);

	bool valueExists(const QString& section, const QString& key);
	int intValue(const QString& section, const QString& key);
	bool boolValue(const QString& section, const QString& key);
	QString stringValue(const QString& section, const QString& key);
	QColor colorValue(const QString& section, const QString& key, const QColor& defaultColor);
	void setValue(const QString& section, const QString& key, const QVariant& value);
	QVariant value(const QString& section, const QString& key, const QVariant& defValue = QVariant());
	

	QStringList keyList(const QString& section);
	QVariant defaultValue(const QString& section, const QString& key);

protected:
//
//
//	static QStringList sectionList();

	static Settings* instance_;

private:
	Settings();
	
	class SettingsData;
	SettingsData* settData_;
};

#endif // __JUFFED_SETTINGS_H__
