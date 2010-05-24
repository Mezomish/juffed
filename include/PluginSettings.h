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

#ifndef __JUFFED_PLUGINS_SETTINGS_H__
#define __JUFFED_PLUGINS_SETTINGS_H__

class JuffPlugin;

#include <QString>

class PluginSettings {
public:
	/**
	 * set()
	 *
	 * Stores a string \param value with a key \param key for \param plugin.
	 */
	static void set(const JuffPlugin* plugin, const QString& key, const QString& value);
	
	/**
	 * set()
	 *
	 * Stores a bool \param value with a key \param key for \param plugin.
	 */
	static void set(const JuffPlugin* plugin, const QString& key, bool value);
	
	/**
	 * set()
	 *
	 * Stores an integer \param value with a key \param key for \param plugin.
	 */
	static void set(const JuffPlugin* plugin, const QString& key, int value);
	
	/**
	 * getString()
	 *
	 * Returns a string value with a key \param key for \param plugin.
	 */
	static QString getString(const JuffPlugin* plugin, const QString& key);

	/**
	 * getBool()
	 *
	 * Returns a bool value with a key \param key for \param plugin.
	 */
	static bool getBool(const JuffPlugin* plugin, const QString& key);

	/**
	 * getInt()
	 *
	 * Returns an integer value with a key \param key for \param plugin.
	 */
	static int getInt(const JuffPlugin* plugin, const QString& key);
	
	
	// These two methods are used by SettingsDlg and PluginManager.
	static bool pluginEnabled(const QString& pluginName);
	static void setPluginEnabled(const QString& pluginName, bool enabled);
};

#endif // __JUFFED_PLUGINS_SETTINGS_H__
