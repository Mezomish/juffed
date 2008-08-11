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

#ifndef _PLUGIN_MANAGER_H_
#define _PLUGIN_MANAGER_H_

class JuffPlugin;
class QObject;

#include <QtCore/QList>

typedef QList<JuffPlugin*> PluginList;

class PluginManager {
public:
	~PluginManager();

	void setHandler(QObject* handler);
	PluginList plugins();
	void loadPlugins();
	void loadPlugin(const QString&);
	void unloadPlugins();
	void unloadPlugin(const QString&);
	void enablePlugin(const QString& pluginName, bool enable = true);

	static PluginManager* instance();

private:
	PluginManager();
	bool pluginExists(const QString& name) const;
	JuffPlugin* findPlugin(const QString& name) const;

	class PMInterior;
	PMInterior* pmInt_;
	static PluginManager* instance_;
};

#endif
