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

#include "MainSettings.h"

#include <QtCore/QDir>

QString MainSettings::lastOpenDir() { 
	return Settings::stringValue("main", "lastOpenDir", QDir::homePath()); 
}

bool MainSettings::syncOpenDialogToCurDoc() {
	return Settings::boolValue("main", "syncOpenDialogToCurDoc", false);
}

QString MainSettings::lastSaveDir() { 
	return Settings::stringValue("main", "lastSaveDir", QDir::homePath()); 
}

int MainSettings::tabPosition() { 
	return Settings::value("main", "tabPosition").toInt(); 
}

QRect MainSettings::windowRect() {
	if (Settings::valueExists("main", "geometry"))
		return Settings::value("main", "geometry").toRect(); 
	else
		return QRect(50, 50, 750, 550);
}

bool MainSettings::isMaximized() { 
	return Settings::value("main", "isMaximized").toBool(); 
}

QString MainSettings::iconTheme() { 
	return Settings::value("main", "iconTheme").toString(); 
}

int MainSettings::toolButtonStyle() { 
	return Settings::value("main", "toolButtonStyle").toInt(); 
}

int MainSettings::iconSize() { 
	return Settings::intValue("main", "iconSize", 1); 
}

int MainSettings::startupVariant() { 
	return Settings::value("main", "startupVariant").toInt(); 
}

QString MainSettings::lastSessionName() { 
	return Settings::value("main", "lastSessionName").toString(); 
}

bool MainSettings::saveSessionOnClose() { 
	return Settings::boolValue("main", "saveSessionOnClose", true); 
}

QString MainSettings::recentFiles() { 
	return Settings::value("main", "recentFiles").toString(); 
}

int MainSettings::recentFilesCount() { 
	return Settings::intValue("main", "recentFilesCount", 10);
}

bool MainSettings::makeBackupOnSave() { 
	return Settings::value("main", "makeBackupOnSave").toBool(); 
}

int MainSettings::autoSaveInterval() { 
	return Settings::value("main", "autoSaveInterval").toInt(); 
}

bool MainSettings::closeTabsInOrderOfUse() { 
	return Settings::boolValue("main", "closeTabsInOrderOfUse", false); 
}

bool MainSettings::singleInstance() { 
	return Settings::value("main", "singleInstance").toBool(); 
}
