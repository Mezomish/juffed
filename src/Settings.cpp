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

#include "AppInfo.h"
#include "Settings.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>

typedef QMap<QString, QVariant> Section;
typedef QMap<QString, Section> SettingsMap;

class SettingsData {
public:
	SettingsMap data_;
};

void Settings::read() {
	QSettings sett(AppInfo::configFile(), QSettings::IniFormat);

	QStringList groups = sett.childGroups();
	foreach (QString grp, groups) {
		sett.beginGroup(grp);
		if (!settData_->data_.contains(grp))
			settData_->data_[grp] = Section();

		QStringList groupKeys = sett.childKeys();
		foreach (QString key, groupKeys) {
			QVariant value = sett.value(key);
			settData_->data_[grp][key] = value;
		}
		sett.endGroup();
	}
}

void Settings::write() {
	QSettings sett(AppInfo::configFile(), QSettings::IniFormat);

	QStringList groups = settData_->data_.keys();
	foreach (QString grp, groups) {
		QStringList keys = settData_->data_[grp].keys();
		sett.beginGroup(grp);
		foreach (QString key, keys) {
			sett.setValue(key, value(grp, key));
		}
		sett.endGroup();
	}
}

QVariant Settings::value(const QString& section, const QString& key) {
	return settData_->data_[section][key];
}

void Settings::setValue(const QString& section, const QString& key, const QVariant& value) {
	settData_->data_[section][key] = value;
}

QStringList Settings::sectionList() {
	return settData_->data_.keys();
}

QStringList Settings::keyList(const QString& section) {
	return settData_->data_[section].keys();
}

SettingsData* Settings::settData_ = new SettingsData();
