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

//	Qt includes
#include <QColor>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>

//	local includes
#include "AppInfo.h"
#include "Log.h"
#include "Settings.h"

Settings* Settings::instance_ = NULL;

Settings* Settings::instance() {
	if ( instance_ == NULL )
		instance_ = new Settings();
	return instance_;
}

typedef QMap<QString, QVariant> Section;
typedef QMap<QString, Section> SettingsMap;

class Settings::SettingsData {
public:
	SettingsMap data_;
};

Settings::Settings() {
	settData_ = new Settings::SettingsData();
}


//int Settings::count() {
//	return settData_->data_.count();
//}

void Settings::read(const QString& org, const QString& appName) {
#ifdef Q_OS_WIN32
	QSettings sett(AppInfo::configFile(), QSettings::IniFormat);
#else
	QSettings sett(QSettings::IniFormat, QSettings::UserScope, org, appName);
#endif
	QStringList groups = sett.childGroups();
	foreach (QString grp, groups) {
		sett.beginGroup(grp);
		if ( !settData_->data_.contains(grp) )
			settData_->data_[grp] = Section();

		QStringList groupKeys = sett.childKeys();
		foreach (QString key, groupKeys) {
			QVariant value = sett.value(key);
			settData_->data_[grp][key] = value;
		}
		sett.endGroup();
	}
}

void Settings::write(const QString& org, const QString& appName) {
#ifdef Q_OS_WIN32
	QSettings sett(AppInfo::configFile(), QSettings::IniFormat);
#else
	QSettings sett(QSettings::IniFormat, QSettings::UserScope, org, appName);
#endif

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

bool Settings::valueExists(const QString& section, const QString& key) {
	return settData_->data_[section].contains(key);
}

QVariant Settings::value(const QString& section, const QString& key, const QVariant& defValue) {
	return settData_->data_[section].value(key, defValue);
}

int Settings::intValue(const QString& section, const QString& key) {
	QVariant def = defaultValue(section, key);
	return settData_->data_[section].value(key, def).toInt();
}

bool Settings::boolValue(const QString& section, const QString& key) {
	QVariant def = defaultValue(section, key);
	return settData_->data_[section].value(key, def).toBool	();
}

QString Settings::stringValue(const QString& section, const QString& key) {
	return settData_->data_[section].value(key, defaultValue(section, key)).toString();
}

QColor Settings::colorValue(const QString& section, const QString& key, const QColor& defaultColor) {
	QVariant value = settData_->data_[section].value(key, defaultColor);
	return value.value<QColor>();
}

void Settings::setValue(const QString& section, const QString& key, const QVariant& value) {
	settData_->data_[section][key] = value;
}

/*QStringList Settings::sectionList() {
	return settData_->data_.keys();
}
*/
QStringList Settings::keyList(const QString& section) {
	return settData_->data_[section].keys();
}



QVariant Settings::defaultValue(const QString& section, const QString& key) {
	if ( section == "main" ) {
		if ( key == "syncOpenDialogToCurDoc" ) return true;
		if ( key == "saveSessionOnClose" )     return true;
		if ( key == "makeBackupOnSave" )       return true;
//		if ( key == "exitOnLastDocClosed" )    return false;
//		if ( key == "stripTrailingSpaces" )    return false;
		if ( key == "singleInstance" )         return true;
		if ( key == "useCtrlTabMenu" )         return true;
//		if ( key == "closeButtonsOnTabs" )     return false;
//		if ( key == "closeTabsInOrderOfUse" )  return false;
		if ( key == "iconTheme" )              return "<default>";
		if ( key == "lastOpenDir" )            return QDir::homePath();
		if ( key == "lastSaveDir" )            return QDir::homePath();
		if ( key == "geometry" )               return QRect(50, 50, 800, 600);
		if ( key == "fsHideToolBar" )          return true;
		if ( key == "fsHideStatusBar" )        return true;
		if ( key == "language" )               return "auto";
		if ( key == "tabPosition" )            return 0;
		if ( key == "toolButtonStyle" )        return 0;
		if ( key == "iconSize" )               return 0;
	}
	else if ( section == "autocomplete" ) {
		if ( key == "useDocument" )   return true;
		if ( key == "useApis" )       return false;
		if ( key == "replaceWord" )   return false;
		if ( key == "caseSensitive" ) return false;
		if ( key == "threshold" )     return 2;
	}
	else if ( section == "charset" ) {
		return true;
	}
	else if ( section == "Plugins" ) {
		return true;
	}
	else if ( section == "editor" ) {
		if ( key == "showLineNumbers" )       return true;
		if ( key == "showWhitespaces" )       return false;
		if ( key == "showLineEnds" )          return false;
		if ( key == "wrapWords" )             return false;
		if ( key == "tabStopWidth" )          return 8;
		if ( key == "replaceTabsWithSpaces" ) return false;
		if ( key == "backspaceUnindents" )    return true;
		if ( key == "fontSize" )              return 10;
		if ( key == "fontFamily" )
#ifdef Q_OS_WIN32
			return "Courier New";
#else
			return "Monospace";
#endif
	}
	else if ( section == "QSci" ) {
		if ( key == "matchingBraceBgColor" )   return QColor(255, 200, 140);
		if ( key == "matchingBraceFgColor" )   return QColor(0, 0, 0);
		if ( key == "indentsColor" )           return QColor(160, 160, 160);
		if ( key == "wordHLColor" )            return QColor(190, 200, 240);
		if ( key == "searchHLColor" )          return QColor(255, 128, 0);
		if ( key == "curLineColor" )           return QColor(240, 240, 255);
		if ( key == "markersColor" )           return QColor(170, 170, 250);
		if ( key == "showIndents" )            return true;
		if ( key == "highlightCurLine" )       return true;
		if ( key == "highlightMatchingBrace" ) return true;
		if ( key == "highlightCurWord" )       return false;
		if ( key == "jumpOverWordParts" )      return false;
	}
	else if ( section == "toolBarVisible" ) {
		return true;
	}
	else if ( section == "dockVisible" ) {
		return false;
	}
	else if ( section == "printing" ) {
		if ( key == "keepColors" )  return false;
		if ( key == "keepBgColor" ) return false;
		if ( key == "alwaysWrap" )  return true;
	}
	return QVariant();
}
