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

#include "MainSettings.h"
#include "Settings.h"

//#include "Log.h"

#include <QRect>

void MainSettings::set(IntKey key, int value) {
	switch (key) {
		case ToolButtonStyle :
			Settings::instance()->setValue("main", "toolButtonStyle", value);
			break;
		
		case TabPosition :
			Settings::instance()->setValue("main", "tabPosition", value);
			break;
		
		case IconSize :
			Settings::instance()->setValue("main", "iconSize", value);
			break;
		
		case SearchMode : 
			Settings::instance()->setValue("main", "searchMode", value);
			break;
	}
}

void MainSettings::set(BoolKey key, bool value) {
	switch (key) {
		case SyncToCurDoc :
			Settings::instance()->setValue("main", "syncOpenDialogToCurDoc", value);
			break;
		
		case Maximized :
			Settings::instance()->setValue("main", "isMaximized", value);
			break;
		
		case FullScreen :
			Settings::instance()->setValue("main", "isFullScreen", value);
			break;
		
		case ExitOnLastDocClosed :
			Settings::instance()->setValue("main", "exitOnLastDocClosed", value);
			break;
		
		case SingleInstance :
			Settings::instance()->setValue("main", "singleInstance", value);
			break;
		
		case StripTrailingSpaces :
			Settings::instance()->setValue("main", "stripTrailingSpaces", value);
			break;
		
		case MakeBackupCopy :
			Settings::instance()->setValue("main", "makeBackupOnSave", value);
			break;
		
//		case FSHideMenubar :
//			Settings::instance()->setValue("main", "fsHideMenubar", value);
//			break;
		
		case FSHideToolbar :
			Settings::instance()->setValue("main", "fsHideToolbar", value);
			break;
		
		case FSHideStatusbar :
			Settings::instance()->setValue("main", "fsHideStatusbar", value);
			break;
		
		case UseCtrlTabMenu :
			Settings::instance()->setValue("main", "useCtrlTabMenu", value);
			break;
		
		case SearchCaseSensitive :
			Settings::instance()->setValue("main", "searchCaseSensitive", value);
			break;
		
		default: ;
	}
}

void MainSettings::set(StringKey key, const QString& value) {
	switch (key) {
		case LastSession :
			Settings::instance()->setValue("main", "lastSession", value);
			break;
		
		case LastDir :
			Settings::instance()->setValue("main", "lastOpenDir", value);
			break;
		
		case Language :
			Settings::instance()->setValue("main", "language", value);
			break;
		
		case RecentFiles :
			Settings::instance()->setValue("main", "recentFiles", value);
			break;
	}
}


int MainSettings::get(IntKey key) {
	switch (key) {
		case ToolButtonStyle :
			return Settings::instance()->intValue("main", "toolButtonStyle");
			break;
		
		case TabPosition :
			return Settings::instance()->intValue("main", "tabPosition");
			break;
		
		case IconSize :
			return Settings::instance()->intValue("main", "iconSize");
			break;
		
		case SearchMode :
			return Settings::instance()->intValue("main", "searchMode");
			break;
	}
	return -1;
}

bool MainSettings::get(BoolKey key) {
	switch (key) {
		case SyncToCurDoc :
			return Settings::instance()->boolValue("main", "syncOpenDialogToCurDoc");
		
		case Maximized :
			return Settings::instance()->boolValue("main", "isMaximized");
		
		case FullScreen :
			return Settings::instance()->boolValue("main", "isFullScreen");
		
		case ExitOnLastDocClosed :
			return Settings::instance()->boolValue("main", "exitOnLastDocClosed");
		
		case SingleInstance :
			return Settings::instance()->boolValue("main", "singleInstance");
		
		case StripTrailingSpaces :
			return Settings::instance()->boolValue("main", "stripTrailingSpaces");
		
		case MakeBackupCopy :
			return Settings::instance()->boolValue("main", "makeBackupOnSave");
		
//		case FSHideMenubar :
//			return Settings::instance()->boolValue("main", "fsHideMenubar");
//			break;
		
		case FSHideToolbar :
			return Settings::instance()->boolValue("main", "fsHideToolbar");
			break;
		
		case FSHideStatusbar :
			return Settings::instance()->boolValue("main", "fsHideStatusbar");
			break;
		
		case UseCtrlTabMenu :
			return Settings::instance()->boolValue("main", "useCtrlTabMenu");
			break;
		
		case SearchCaseSensitive :
			return Settings::instance()->boolValue("main", "searchCaseSensitive");
			break;
		
		default :
			return false;
	}
}

QString MainSettings::get(StringKey key) {
	switch (key) {
		case LastSession :
			return Settings::instance()->stringValue("main", "lastSession");
		
		case LastDir:
			return Settings::instance()->stringValue("main", "lastOpenDir");
		
		case Language :
			return Settings::instance()->stringValue("main", "language");
		
		case RecentFiles :
			return Settings::instance()->stringValue("main", "recentFiles");
		
		default:
			return "";
	}
}

void MainSettings::setGeometry(const QRect& rect) {
	Settings::instance()->setValue("main", "geometry", rect);
}

QRect MainSettings::geometry() {
	return Settings::instance()->value("main", "geometry", Settings::instance()->defaultValue("main", "geometry")).toRect();
}

void MainSettings::setMwState(const QByteArray& state) {
	Settings::instance()->setValue("main", "mwState", state); 
}

QByteArray MainSettings::mwState() {
	return Settings::instance()->value("main", "mwState").toByteArray(); 
}
