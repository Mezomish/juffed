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

#include <QDebug>

#include "PluginManager.h"

#include "AppInfo.h"
#include "JuffPlugin.h"
#include "Log.h"
#include "DocHandlerInt.h"
#include "PluginNotifier.h"
#include "ui/settings/SettingsDlg.h"

#include <QDir>
#include <QPluginLoader>

PluginManager::PluginManager(Juff::DocHandlerInt* handler, Juff::PluginNotifier* notifier) {
	api_ = new JuffAPI(handler, notifier);
}

PluginManager::~PluginManager() {
	delete api_;
}

void PluginManager::loadPlugins(SettingsDlg* dlg) {
	LOGGER;
	//	user's plugins
//	QDir pluginDir(AppInfo::configDirPath() + "/plugins");
/*	QDir pluginDir("../plugins");
	foreach (QString fileName, pluginDir.entryList(QStringList() << "*.so", QDir::Files)) {
		QString path = pluginDir.absoluteFilePath(fileName);
		loadPlugin(path);
	}*/
	
	//	global plugins
	QDir gPluginDir(AppInfo::appDirPath() + "/plugins");
	foreach (QString fileName, gPluginDir.entryList(QDir::Files)) {
		QString path = gPluginDir.absoluteFilePath(fileName);
		loadPlugin(path, dlg);
	}
	
/*	foreach (QString type, pmInt_->docks_.keys()) {
		pmInt_->gui_->addDocks(type, pmInt_->docks_[type]);
	}*/
}

Juff::MenuList PluginManager::menus() const {
	Juff::MenuList menus;
	foreach (JuffPlugin* plugin, plugins_)
		if ( plugin->menu() != 0 )
			menus << plugin->menu();
	return menus;
}

Juff::ActionList PluginManager::actions(Juff::MenuID id) const {
	Juff::ActionList actions;
	foreach (JuffPlugin* plugin, plugins_)
		actions << plugin->mainMenuActions(id);
	return actions;
}

QWidgetList PluginManager::docks() const {
	QWidgetList list;
	foreach (JuffPlugin* plugin, plugins_)
		list << plugin->dockList();
	return list;
}

#include "EditorSettings.h"

void PluginManager::loadPlugin(const QString& path, SettingsDlg* dlg) {
	LOGGER;

	qDebug() << "                     FONT:" << EditorSettings::font();
	
	QPluginLoader loader(path);
	if ( !loader.load() ) {
		Log::warning(QString("Plugin '%1' was NOT loaded: %2").arg(path).arg(loader.errorString()));
		return;
	}
	
	qDebug() << "                     FONT:" << EditorSettings::font();
	
	QObject *obj = loader.instance();
	if ( obj ) {
		JuffPlugin* plugin = qobject_cast<JuffPlugin*>(obj);
		if ( plugin ) {

			//	Check if we need to load it
//			if ( !PluginSettings::pluginEnabled(plugin->name()) ) {
//				pmInt_->gui_->addPluginSettingsPage(plugin->name(), 0);
//				Log::debug("Plugin is disabled in Settings");
//				return;
//			}

			//	Check if plugin with the same name was already loaded.
			//	If is was then exit.
//			if ( pluginExists(plugin->name()) )
//				return;

//			plugin->setManager(pmInt_->managerInt_);
			plugin->setAPI(api_);
			plugin->init();
			
			plugins_ << plugin;
//			if ( plugin->settingsPage() != 0 )
				dlg->addPluginSettingsPage(plugin->name(), plugin->title(), plugin->settingsPage());
//			if ( pmInt_->addPlugin(plugin) ) {
//
				Log::debug(QString("-----=====((((( Plugin '%1' was loaded successfully! )))))=====-----").arg(plugin->name()));
//
				//	context menu actions
//				QString type = plugin->targetEngine();
//				pmInt_->contextMenuActions_[type] << plugin->contextMenuActions();
//				
				//	settings page
//				pmInt_->gui_->addPluginSettingsPage(plugin->name(), plugin->settingsPage());
//			}
//			else {
//				loader.unload();
//			}
		}
		else {
			Log::warning("Error while casting to JuffPlugin type");
		}
	}
	else {
		Log::debug("Empty plugin instance");
	}
}
