#include "MainSettings.h"

QString MainSettings::lastOpenDir() { 
	return Settings::value("main", "lastOpenDir").toString(); 
}

QString MainSettings::lastSaveDir() { 
	return Settings::value("main", "lastSaveDir").toString(); 
}

int MainSettings::tabPosition() { 
	return Settings::value("main", "tabPosition").toInt(); 
}

QRect MainSettings::windowRect() {
	if (Settings::valueExists("main", "geometry"))
		return Settings::value("main", "geometry").toRect(); 
	else
		return QRect(50, 50, 600, 500);
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
	return 10; /* TODO: remove this */
}

bool MainSettings::makeBackupOnSave() { 
	return Settings::value("main", "makeBackupOnSave").toBool(); 
}

int MainSettings::autoSaveInterval() { 
	return Settings::value("main", "autoSaveInterval").toInt(); 
}

bool MainSettings::closeTabsInOrderOfUse() { 
	return Settings::value("main", "closeTabsInOrderOfUse").toBool(); 
}

bool MainSettings::singleInstance() { 
	return Settings::value("main", "singleInstance").toBool(); 
}
