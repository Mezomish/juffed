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
	static QByteArray mwState();
	
	static void setLastOpenDir(const QString& dir);
	static void setLastSaveDir(const QString& dir);
	static void setSyncOpenDialogToCurDoc(bool sync);
	static void setWindowRect(const QRect& rect);
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
	static void setAutoSaveInterval(int interval);
	static void setSingleInstance(bool single);
	static void setMwState(const QByteArray&);
};

#endif
