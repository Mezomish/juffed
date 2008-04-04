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

#ifndef _APP_INFO_H_
#define _APP_INFO_H_

#include <QtCore/QDir>
#include <QtCore/QString>

class AppInfo {
public:
	static QString name() { return "JuffEd"; }
	static QString organization() { return "Juff"; }
	static QString configDir() { return QDir::homePath() + "/.config/" + organization().toLower(); }
	static QString configFile() { return configDir() + "/" + name().toLower() + ".conf"; }
	static QString version() { return "0.2-beta2"; }
	static QString logFile() { return configDir() + "/juffed.log"; }
	static QString socketPath() { return "/tmp/juffed"; }
};

#endif
