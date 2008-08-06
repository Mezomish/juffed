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

#ifndef _PLUGINS_SETTINGS_H_
#define _PLUGINS_SETTINGS_H_

#include "Settings.h"

class PluginsSettings : public Settings {
public:
	static bool pluginEnabled(const QString& name);
	static QString pluginPath(const QString& name);

//	static void setPluginEnabled(const QString& name, bool);
	static void setPluginPath(const QString& name, const QString& path);
};

#endif
