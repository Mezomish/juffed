/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

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

#include "PluginSettings.h"
#include "Settings.h"

#include "JuffPlugin.h"

bool PluginSettings::pluginEnabled(const QString& pluginName) {
	return Settings::instance()->boolValue("Plugins", pluginName);
}

void PluginSettings::setPluginEnabled(const QString& pluginName, bool enabled) {
	Settings::instance()->setValue("Plugins", pluginName, enabled);
}

void PluginSettings::set(const JuffPlugin* plugin, const QString& key, const QString& value) {
	if ( plugin )
		Settings::instance()->setValue(plugin->name(), key, value);
}

void PluginSettings::set(const JuffPlugin* plugin, const QString& key, bool value) {
	if ( plugin )
		Settings::instance()->setValue(plugin->name(), key, value);
}

void PluginSettings::set(const JuffPlugin* plugin, const QString& key, int value) {
	if ( plugin )
		Settings::instance()->setValue(plugin->name(), key, value);
}

QString PluginSettings::getString(const JuffPlugin* plugin, const QString& key) {
	if ( plugin )
		return Settings::instance()->stringValue(plugin->name(), key);
	else
		return "";
}

bool PluginSettings::getBool(const JuffPlugin* plugin, const QString& key) {
	if ( plugin )
		return Settings::instance()->boolValue(plugin->name(), key);
	else
		return false;
}

int PluginSettings::getInt(const JuffPlugin* plugin, const QString& key) {
	if ( plugin )
		return Settings::instance()->intValue(plugin->name(), key);
	else
		return -1;
}
