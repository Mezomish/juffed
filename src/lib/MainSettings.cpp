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

#include "MainSettings.h"

#include "Log.h"

#include <QRect>

void MainSettings::set(BoolKey key, bool value) {
	switch (key) {
		case SyncToCurDoc :
			Settings::setValue("main", "syncOpenDialogToCurDoc", value);
			break;
		
		case Maximized :
			Settings::setValue("main", "isMaximized", value);
			break;
		
		case FullScreen :
			Settings::setValue("main", "isFullScreen", value);
			break;
		
		case ExitOnLastDocClosed :
			Settings::setValue("main", "exitOnLastDocClosed", value);
			break;
		
		case SingleInstance :
			Settings::setValue("main", "singleInstance", value);
			break;
		
		case StripTrailingSpaces :
			Settings::setValue("main", "stripTrailingSpaces", value);
			break;
		
		case MakeBackupCopy :
			Settings::setValue("main", "makeBackupOnSave", value);
			break;
		
//		case FSHideMenubar :
//			Settings::setValue("main", "fsHideMenubar", value);
//			break;
		
		case FSHideToolbar :
			Settings::setValue("main", "fsHideToolbar", value);
			break;
		
		case FSHideStatusbar :
			Settings::setValue("main", "fsHideStatusbar", value);
			break;
		
		default: ;
	}
}

void MainSettings::set(StringKey key, const QString& value) {
	switch (key) {
		case LastProject :
			Settings::setValue("main", "lastProject", value);
			break;
		
		case LastDir :
			Settings::setValue("main", "lastOpenDir", value);
			break;
		
		case Language :
			Settings::setValue("main", "language", value);
			break;
		
		case RecentFiles :
			Settings::setValue("main", "recentFiles", value);
			break;
	}
}


bool MainSettings::get(BoolKey key) {
	switch (key) {
		case SyncToCurDoc :
			return Settings::boolValue("main", "syncOpenDialogToCurDoc");
		
		case Maximized :
			return Settings::boolValue("main", "isMaximized");
		
		case FullScreen :
			return Settings::boolValue("main", "isFullScreen");
		
		case ExitOnLastDocClosed :
			return Settings::boolValue("main", "exitOnLastDocClosed");
		
		case SingleInstance :
			return Settings::boolValue("main", "singleInstance");
		
		case StripTrailingSpaces :
			return Settings::boolValue("main", "stripTrailingSpaces");
		
		case MakeBackupCopy :
			return Settings::boolValue("main", "makeBackupOnSave");
		
//		case FSHideMenubar :
//			return Settings::boolValue("main", "fsHideMenubar");
//			break;
		
		case FSHideToolbar :
			return Settings::boolValue("main", "fsHideToolbar");
			break;
		
		case FSHideStatusbar :
			return Settings::boolValue("main", "fsHideStatusbar");
			break;
		
		default :
			return false;
	}
}

QString MainSettings::get(StringKey key) {
	switch (key) {
		case LastProject :
			return Settings::stringValue("main", "lastProject");
		
		case LastDir:
			return Settings::stringValue("main", "lastOpenDir");
		
		case Language :
			return Settings::stringValue("main", "language");
		
		case RecentFiles :
			return Settings::stringValue("main", "recentFiles");
		
		default:
			return "";
	}
}

void MainSettings::setGeometry(const QRect& rect) {
	Settings::setValue("main", "geometry", rect);
}

QRect MainSettings::geometry() {
	return Settings::value("main", "geometry", defaultValue("main", "geometry")).toRect();
}

void MainSettings::setMwState(const QByteArray& state) {
	Settings::setValue("main", "mwState", state); 
}

QByteArray MainSettings::mwState() {
	return Settings::value("main", "mwState").toByteArray(); 
}
