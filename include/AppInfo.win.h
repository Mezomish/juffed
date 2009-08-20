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

#ifndef _APP_INFO_H_
#define _APP_INFO_H_

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QString>

class AppInfo {
public:
	static QString name() { return "JuffEd"; }
	static QString organization() { return "Juff"; }
	static QString configDirPath() { return QDir::homePath() + "/.config/" + organization().toLower(); }
	static QString configFile() { return configDirPath() + "/" + name().toLower() + ".conf"; }
	static QString version() { return "@VERSION@"; }
	static QString logFile() { return configDirPath() + "/juffed.log"; }
	static QString socketPath() { return "/tmp/juffed"; }
	static QString appDirPath() { return QCoreApplication::applicationDirPath(); }
	static QString translationPath() { return appDirPath() + "/l10n"; }
};

#endif
