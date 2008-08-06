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

#include "PluginManager.h"

#include <QtCore/QDir>
#include <QtCore/QPluginLoader>

#include "AppInfo.h"
#include "Log.h"
#include "PluginInterface.h"
#include "PluginsSettings.h"

PluginManager* PluginManager::instance_ = 0;

PluginManager::PluginManager() {
}

PluginManager::~PluginManager() {
}

PluginManager* PluginManager::instance() {
	if (instance_ == 0)
		instance_ = new PluginManager();
	return instance_;
}

void PluginManager::setHandler(QObject* handler) {
	handler_ = handler;
}

PluginList PluginManager::plugins() {
	return pluginList_;
}

void PluginManager::loadPlugin(const QString& path) {
	QPluginLoader loader(path);
	if (!loader.load())
		return;

	QObject *obj = loader.instance();
	if (obj) {
		JuffPlugin* plugin = qobject_cast<JuffPlugin*>(obj);
		if (plugin != 0) {
			Log::debug(plugin->name());
			if (PluginsSettings::pluginEnabled(plugin->name().toLower())) {
				plugin->setPath(path);
				pluginList_.append(plugin);
				plugin->setParent(handler_);
				qDebug(qPrintable(QString("Plugin '%1' loaded").arg(plugin->name())));
//				PluginsSettings::setPluginEnabled(plugin->name(), true);
			}
			else {
				Log::debug("b");
//				loader.unload();
			}
		}
	}
}

void PluginManager::loadPlugins() {
	//	user's plugins
	QDir pluginDir(AppInfo::configDir() + "/plugins");
	foreach (QString fileName, pluginDir.entryList(QDir::Files)) {
		QString path = pluginDir.absoluteFilePath(fileName);
		loadPlugin(path);
	}
	
	//	global plugins
	QDir gPluginDir(AppInfo::appDirPath() + "/plugins");
	foreach (QString fileName, gPluginDir.entryList(QDir::Files)) {
		QString path = gPluginDir.absoluteFilePath(fileName);
		loadPlugin(path);
	}
}

void PluginManager::unloadPlugin(const QString&) {
}

void PluginManager::unloadPlugins() {
	foreach (JuffPlugin* plugin, pluginList_) {
		if (plugin != 0) {
			plugin->deinit();
		}
	}
}

/* 613-736-7162 Natasha (do 22) */
