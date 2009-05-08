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

#include "PluginSettings.h"

#include "JuffPlugin.h"

void PluginSettings::readSettings(JuffPlugin* plugin) {
/*	if (plugin == 0)
		return;
	
	QString section = plugin->name();
	QStringList keys = Settings::keyList(section);
	QMap<QString, QVariant> map;
	foreach (QString key, keys) {
		map[key] = Settings::value(section, key);
	}
	plugin->setSettings(map);*/
}

void PluginSettings::saveSettings(JuffPlugin* plugin) {
/*	QString section = plugin->name();
	QMap<QString, QVariant> map = plugin->settings();
	QMap<QString, QVariant>::iterator it = map.begin();
	while (it != map.end()) {
		Settings::setValue(section, it.key(), it.value());
		it++;
	}*/
}

bool PluginSettings::pluginEnabled(const QString& pluginName) {
	return Settings::boolValue("Plugins", pluginName, true);
}

void PluginSettings::setPluginEnabled(const QString& pluginName, bool enabled) {
	Settings::setValue("Plugins", pluginName, enabled);
}

void PluginSettings::setStringValue(JuffPlugin* plugin, const QString& key, const QString& value) {
	if ( plugin )
		Settings::setValue(plugin->name(), key, value);
}

QString PluginSettings::getStringValue(JuffPlugin* plugin, const QString& key) {
	if ( plugin )
		return Settings::stringValue(plugin->name(), key, "");
	else
		return "";
}
