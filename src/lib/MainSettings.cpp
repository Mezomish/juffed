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

#include "MainSettings.h"

#include <QtCore/QDir>

QString MainSettings::lastOpenDir() { 
	return Settings::stringValue("main", "lastOpenDir", QDir::homePath()); 
}

bool MainSettings::syncOpenDialogToCurDoc() {
	return Settings::boolValue("main", "syncOpenDialogToCurDoc", true);
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
	return Settings::intValue("main", "startupVariant", 1); 
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

bool MainSettings::stripTrailingSpaces() { 
	return Settings::boolValue("main", "stripTrailingSpaces", false); 
}

bool MainSettings::closeTabsInOrderOfUse() { 
	return Settings::boolValue("main", "closeTabsInOrderOfUse", false); 
}

bool MainSettings::singleInstance() { 
	return Settings::boolValue("main", "singleInstance", true); 
}

QByteArray MainSettings::mwState() {
	return Settings::value("main", "mwState").toByteArray(); 
}

QString MainSettings::toolBar() {
	return Settings::value("main", "toolBar").toString();
}



void MainSettings::setLastOpenDir(const QString& dir) { 
	Settings::setValue("main", "lastOpenDir", dir); 
}
void MainSettings::setLastSaveDir(const QString& dir) { 
	Settings::setValue("main", "lastSaveDir", dir); 
}
void MainSettings::setSyncOpenDialogToCurDoc(bool sync) { 
	Settings::setValue("main", "syncOpenDialogToCurDoc", sync); 
}
void MainSettings::setWindowRect(const QRect& rect) { 
	Settings::setValue("main", "geometry", rect); 
}
void MainSettings::setMaximized(bool maximized) { 
	Settings::setValue("main", "isMaximized", maximized); 
}
void MainSettings::setTabPosition(int pos) { 
	Settings::setValue("main", "tabPosition", pos); 
}
void MainSettings::setIconTheme(const QString& theme) { 
	Settings::setValue("main", "iconTheme", theme); 
}
void MainSettings::setToolButtonStyle(int style) { 
	Settings::setValue("main", "toolButtonStyle", style); 
}
void MainSettings::setIconSize(int sz) { 
	Settings::setValue("main", "iconSize", sz); 
}
void MainSettings::setStartupVariant(int v) { 
	Settings::setValue("main", "startupVariant", v); 
}
void MainSettings::setLastSessionName(const QString& session) { 
	Settings::setValue("main", "lastSessionName", session); 
}
void MainSettings::setSaveSessionOnClose(bool save) { 
	Settings::setValue("main", "saveSessionOnClose", save); 
}
void MainSettings::setRecentFiles(const QString& files) { 
	Settings::setValue("main", "recentFiles", files); 
}
void MainSettings::setMakeBackupOnSave(bool bkp) { 
	Settings::setValue("main", "makeBackupOnSave", bkp); 
}
void MainSettings::setStripTrailingSpaces(bool strip) { 
	Settings::setValue("main", "stripTrailingSpaces", strip); 
}
void MainSettings::setSingleInstance(bool single) { 
	Settings::setValue("main", "singleInstance", single); 
}
void MainSettings::setMwState(const QByteArray& state) {
	Settings::setValue("main", "mwState", state); 
}
