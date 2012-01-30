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
#include <QApplication>
#include <QPalette>
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
	if ( section.compare("main") == 0 ) {
		if ( key.compare("syncOpenDialogToCurDoc") == 0 ) return true;
		if ( key.compare("saveSessionOnClose") == 0 )     return true;
		if ( key.compare("makeBackupOnSave") == 0 )       return true;
//		if ( key.compare("exitOnLastDocClosed") == 0 )    return false;
//		if ( key.compare("stripTrailingSpaces") == 0 )    return false;
		if ( key.compare("singleInstance") == 0 )         return true;
		if ( key.compare("useCtrlTabMenu") == 0 )         return true;
//		if ( key.compare("closeButtonsOnTabs") == 0 )     return false;
//		if ( key.compare("closeTabsInOrderOfUse") == 0 )  return false;
		if ( key.compare("iconTheme") == 0 )              return "<default>";
		if ( key.compare("lastOpenDir") == 0 )            return QDir::homePath();
		if ( key.compare("lastSaveDir") == 0 )            return QDir::homePath();
		if ( key.compare("geometry") == 0 )               return QRect(50, 50, 800, 600);
		if ( key.compare("fsHideToolBar") == 0 )          return true;
		if ( key.compare("fsHideStatusBar") == 0 )        return true;
		if ( key.compare("language") == 0 )               return "auto";
		if ( key.compare("tabPosition") == 0 )            return 0;
		if ( key.compare("toolButtonStyle") == 0 )        return 0;
		if ( key.compare("iconSize") == 0 )               return 0;
	}
	else if ( section.compare("autocomplete") == 0 ) {
		if ( key.compare("useDocument") == 0 )   return true;
		if ( key.compare("useApis") == 0 )       return false;
		if ( key.compare("replaceWord") == 0 )   return false;
		if ( key.compare("caseSensitive") == 0 ) return false;
		if ( key.compare("threshold") == 0 )     return 2;
	}
	else if ( section.compare("charset") == 0 ) {
		return true;
	}
	else if ( section.compare("Plugins") == 0 ) {
		return true;
	}
	else if ( section.compare("editor") == 0 ) {
		if ( key.compare("showLineNumbers") == 0 )       return true;
		if ( key.compare("showWhitespaces") == 0 )       return false;
		if ( key.compare("showLineEnds") == 0 )          return false;
		if ( key.compare("wrapWords") == 0 )             return false;
		if ( key.compare("tabStopWidth") == 0 )          return 8;
		if ( key.compare("replaceTabsWithSpaces") == 0 ) return false;
		if ( key.compare("backspaceUnindents") == 0 )    return true;
		if ( key.compare("fontSize") == 0 )              return 10;
		if ( key.compare("fontFamily") == 0 )
#ifdef Q_OS_WIN32
			return "Courier New";
#else
			return "Monospace";
#endif
	}
	else if ( section.compare("QSci") == 0 ) {
		if ( key.compare("matchingBraceBgColor") == 0 )   return QColor(255, 200, 140);
		if ( key.compare("matchingBraceFgColor") == 0 )   return QColor(0, 0, 0);
		if ( key.compare("indentsColor") == 0 )           return QColor(160, 160, 160);
		if ( key.compare("wordHLColor") == 0 )            return QColor(190, 200, 240);
		if ( key.compare("searchHLColor") == 0 )          return QColor(255, 128, 0);
		if ( key.compare("curLineColor") == 0 )           return QColor(240, 240, 255);
		if ( key.compare("markersColor") == 0 )           return QColor(170, 170, 250);
		if ( key.compare("marginsBgColor") == 0 )         return QApplication::palette().color(QPalette::Window);
		if ( key.compare("whiteSpaceColor") == 0 )        return QColor(155, 155, 185);
		if ( key.compare("showIndents") == 0 )            return true;
		if ( key.compare("highlightCurLine") == 0 )       return true;
		if ( key.compare("highlightMatchingBrace") == 0 ) return true;
		if ( key.compare("highlightCurWord") == 0 )       return false;
		if ( key.compare("jumpOverWordParts") == 0 )      return false;
	}
	else if ( section.compare("toolBarVisible") == 0 ) {
		return true;
	}
	else if ( section.compare("dockVisible") == 0 ) {
		return false;
	}
	else if ( section.compare("printing") == 0 ) {
		if ( key.compare("keepColors") == 0 )  return false;
		if ( key.compare("keepBgColor") == 0 ) return false;
		if ( key.compare("alwaysWrap") == 0 )  return true;
	}
	return QVariant();
}
