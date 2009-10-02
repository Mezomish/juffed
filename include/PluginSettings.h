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

#ifndef __JUFF_PLUGINS_SETTINGS_H__
#define __JUFF_PLUGINS_SETTINGS_H__

class JuffPlugin;

#include "Settings.h"

class PluginSettings : public Settings {
public:
	
	/** Reads all settings from config file that are
	 *  related to \a plugin and sends these settings
	 *  to \a plugin.
	 */
	static void readSettings(JuffPlugin* plugin);

	/** Gets settings from \a plugin and saves them
	 *  to config file.
	 */
	static void saveSettings(JuffPlugin* plugin);

	/** Returns if plugin with name \a pluginName is
	 *  enabled (supposed to be loaded).
	 */
	static bool pluginEnabled(const QString& pluginName);

	/**
	 */
	static void setPluginEnabled(const QString& pluginName, bool enabled);

	/**
	 * setStringValue()
	 *
	 * Stores a string \param value with a key \param key for \param plugin.
	 */
	static void setStringValue(JuffPlugin* plugin, const QString& key, const QString& value);
	
	/**
	 * getStringValue()
	 *
	 * Returns a string value with a key \param key for \param plugin.
	 */
	static QString getStringValue(JuffPlugin* plugin, const QString& key);

	/**
	 * setBoolValue()
	 *
	 * Stores a bool \param value with a key \param key for \param plugin.
	 */
	static void setBoolValue(JuffPlugin* plugin, const QString& key, bool value);
	
	/**
	 * getBoolValue()
	 *
	 * Returns a bool value with a key \param key for \param plugin.
	 */
	static bool getBoolValue(JuffPlugin* plugin, const QString& key);
};

#endif // __JUFF_PLUGINS_SETTINGS_H__
