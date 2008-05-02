#ifndef _APP_INFO_H_
#define _APP_INFO_H_

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QString>

class AppInfo {
public:
	static QString name() { return "JuffEd"; }
	static QString organization() { return "Juff"; }
	static QString configDir() { return QDir::homePath() + "/.config/" + organization().toLower(); }
	static QString configFile() { return configDir() + "/" + name().toLower() + ".conf"; }
	static QString version() { return "0.3-beta1"; }
	static QString logFile() { return configDir() + "/juffed.log"; }
	static QString socketPath() { return "/tmp/juffed"; }
	static QString appDirPath() { return QCoreApplication::applicationDirPath(); }
};

#endif
