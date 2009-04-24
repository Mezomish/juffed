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

#ifndef _MAIN_SETTINGS_H_
#define _MAIN_SETTINGS_H_

#include "Settings.h"

#include <QtCore/QRect>
#include <QtCore/QString>

class MainSettings : public Settings {
public:
	static QString lastOpenDir();
	static QString lastSaveDir();
	static bool syncOpenDialogToCurDoc();
	static int tabPosition();
	static QRect windowRect();
	static QRect findDlgRect();
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
	static bool stripTrailingSpaces();
	static bool closeTabsInOrderOfUse();
	static bool singleInstance();
	static QByteArray mwState();
	static QString toolBar();

	static void setLastOpenDir(const QString& dir);
	static void setLastSaveDir(const QString& dir);
	static void setSyncOpenDialogToCurDoc(bool sync);
	static void setWindowRect(const QRect& rect);
	static void setFindDlgRect(const QRect& rect);
	static void setMaximized(bool maximized);
	static void setTabPosition(int pos);
	static void setIconTheme(const QString& theme);
	static void setToolButtonStyle(int style);
	static void setIconSize(int sz);
	static void setStartupVariant(int v);
	static void setLastSessionName(const QString& session);
	static void setSaveSessionOnClose(bool save);
	static void setRecentFiles(const QString& files);
	static void setMakeBackupOnSave(bool bkp);
	static void setStripTrailingSpaces(bool strip);
	static void setSingleInstance(bool single);
	static void setMwState(const QByteArray&);
};

#endif
