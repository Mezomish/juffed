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

#ifndef _MAIN_SETTINGS_H_
#define _MAIN_SETTINGS_H_

#include "Settings.h"

#include <QtCore/QRect>
#include <QtCore/QString>

class MainSettings : public Settings {
public:
	static QString lastOpenDir();
	static bool useCurrentDocDir();
	static QString lastSaveDir();
	static int tabPosition();
	static QRect windowRect();
	static bool isMaximized();
	static QString iconTheme();
	static int toolButtonStyle();
	static int iconSize();
	static int startupVariant();
	static QString lastSessionName();
	static bool saveSessionOnClose();
	static QString recentFiles();
	static int recentFilesCount();
	static bool makeBackupOnSave();
	static int autoSaveInterval();
	static bool closeTabsInOrderOfUse();
	static bool singleInstance();
	
	static void setLastOpenDir(const QString& dir) { Settings::setValue("main", "lastOpenDir", dir); }
	static void setLastSaveDir(const QString& dir) { Settings::setValue("main", "lastSaveDir", dir); }
	static void setWindowRect(const QRect& rect) { Settings::setValue("main", "geometry", rect); }
	static void setMaximized(bool maximized) { Settings::setValue("main", "isMaximized", maximized); }
	static void setTabPosition(int pos) { Settings::setValue("main", "tabPosition", pos); }
	static void setIconTheme(const QString& theme) { Settings::setValue("main", "iconTheme", theme); }
	static void setToolButtonStyle(int style) { Settings::setValue("main", "toolButtonStyle", style); }
	static void setIconSize(int sz) { Settings::setValue("main", "iconSize", sz); }
	static void setStartupVariant(int v) { Settings::setValue("main", "startupVariant", v); }
	static void setLastSessionName(const QString& session) { Settings::setValue("main", "lastSessionName", session); }
	static void setSaveSessionOnClose(bool save) { Settings::setValue("main", "saveSessionOnClose", save); }
	static void setRecentFiles(const QString& files) { Settings::setValue("main", "recentFiles", files); }
	static void setMakeBackupOnSave(bool bkp) { Settings::setValue("main", "makeBackupOnSave", bkp); }
	static void setAutoSaveInterval(int interval) { Settings::setValue("main", "autoSaveInterval", interval); }
	static void setSingleInstance(bool single) { Settings::setValue("main", "singleInstance", single); }
};

#endif
