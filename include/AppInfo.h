#ifndef __JUFFED_APP_INFO_H__
#define __JUFFED_APP_INFO_H__

#include <QDir>
#include <QLocale>

class AppInfo {
public:
	static QString name()            { return "JuffEd"; }
	static QString organization()    { return "Juff"; }
	static QString configDirPath()   { return QDir::homePath() + "/.config/" + organization().toLower(); }
	static QString language()        { return QLocale::system().name(); }
	static QString logFile()         { return configDirPath() + "/juffed.log"; }
	
//	static QString translationPath() { return ; }
};

#endif // __JUFFED_APP_INFO_H__
