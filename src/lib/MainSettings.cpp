#include "MainSettings.h"

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
		
		default:
			return "";
	}
}
