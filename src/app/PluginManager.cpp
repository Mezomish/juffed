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
#include "Enums.h"
#include "JuffPlugin.h"
#include "Log.h"
#include "DocHandlerInt.h"
#include "PluginNotifier.h"
#include "PluginSettings.h"
#include "Settings.h"
#include "Utils.h"
#include "ui/settings/SettingsDlg.h"

#include <QDir>
#include <QPluginLoader>

PluginManager::PluginManager(Juff::DocHandlerInt* handler, Juff::PluginNotifier* notifier) {
	api_ = new JuffAPI(handler, notifier, Juff::Utils::commandStorage(), Juff::Utils::iconManager());
}

PluginManager::~PluginManager() {
	delete api_;
}

void PluginManager::loadPlugins(SettingsDlg* dlg) {
	// switch off some plugins by default since user may not want their behavior by default
	QStringList names;
	names << "Autosave" << "Color Picker";
	foreach (QString name, names) {
		if ( !Settings::instance()->valueExists("Plugins", name) ) {
			Settings::instance()->setValue("Plugins", name, false);
		}
	}
	
	// global plugins
	QDir gPluginDir(AppInfo::pluginsPath());
	foreach (QString fileName, gPluginDir.entryList(QDir::Files)) {
		loadPlugin(gPluginDir.absoluteFilePath(fileName), dlg);
	}
	
	Juff::MenuID ids[] = { Juff::MenuFile, Juff::MenuEdit, Juff::MenuView, Juff::MenuSearch, Juff::MenuFormat, Juff::MenuTools, Juff::MenuNULL };
	int i = 0;
	while ( ids[i] != Juff::MenuNULL) {
		Juff::MenuID id = ids[i];
		foreach (JuffPlugin* plugin, plugins_) {
			Juff::ActionList pluginActions = plugin->mainMenuActions(id);
			QString name = plugin->name();
			foreach (QAction* a, pluginActions) {
				Juff::Utils::commandStorage()->addAction(QString("plugin_") + name + ":" + a->text(), a);
			}
		}
		++i;
	}
	
	
	foreach (JuffPlugin* plugin, plugins_) {
		plugin->allPluginsLoaded();
	}
}

Juff::MenuList PluginManager::menus() const {
	Juff::MenuList menus;
	foreach (JuffPlugin* plugin, plugins_) {
		if ( plugin->menu() != 0 ) {
			menus << plugin->menu();
		}
	}
	return menus;
}

Juff::ActionList PluginManager::actions(Juff::MenuID id) const {
	Juff::ActionList actions;
	foreach (JuffPlugin* plugin, plugins_) {
		actions << plugin->mainMenuActions(id);
	}
	return actions;
}

QWidgetList PluginManager::docks(QList<Qt::DockWidgetArea>& positions, QList<bool>& visibility) const {
	QWidgetList list;
	positions.clear();
	visibility.clear();
	foreach (JuffPlugin* plugin, plugins_) {
		foreach ( QWidget* w, plugin->dockList() ) {
			list << w;
			positions << plugin->dockPosition(w);
			visibility << plugin->dockVisible(w);
		}
	}
	return list;
}

Juff::ToolBarList PluginManager::toolbars() const {
	Juff::ToolBarList list;
	foreach ( JuffPlugin* plugin, plugins_ ) {
		QToolBar* bar = plugin->toolBar();
		if ( NULL != bar ) {
			list.append(bar);
		}
	}
	return list;
}

#include "EditorSettings.h"

void PluginManager::loadPlugin(const QString& path, SettingsDlg* dlg) {
//	Log::warning(path);
	QPluginLoader loader(path);
	if ( !loader.load() ) {
		Log::warning(QString("Plugin '%1' was NOT loaded: %2").arg(path).arg(loader.errorString()));
		return;
	}
	
	QObject *obj = loader.instance();
	if ( obj ) {
#ifdef Q_WS_MAC
		// qobject_cast dows not work on mac
		JuffPlugin* plugin = dynamic_cast<JuffPlugin*>(obj);
#else
		JuffPlugin* plugin = qobject_cast<JuffPlugin*>(obj);
#endif
		if ( plugin ) {
			//	Check if we need to load it
			if ( !PluginSettings::pluginEnabled(plugin->name()) ) {
				dlg->addPluginSettingsPage(plugin->name(), plugin->title(), 0);
				Log::debug( QString("--- Plugin '%1' is disabled in Settings.").arg(plugin->name()) );
				loader.unload();
				return;
			}

			//	Check if plugin with the same name was already loaded.
			//	If is was then exit.
//			if ( pluginExists(plugin->name()) )
//				return;

			plugin->setAPI(api_);
			plugin->init();
			
			plugins_ << plugin;
			dlg->addPluginSettingsPage(plugin->name(), plugin->title(), plugin->settingsPage());

			Log::debug(QString("+++ Plugin '%1' was loaded successfully.").arg(plugin->name()));
//
			//	context menu actions
//			QString type = plugin->targetEngine();
//			pmInt_->contextMenuActions_[type] << plugin->contextMenuActions();
		}
		else {
			Log::warning("Error while casting to JuffPlugin type");
		}
	}
	else {
		Log::debug("Empty plugin instance");
	}
}

void PluginManager::applySettings() {
//	LOGGER;
	foreach (JuffPlugin* plugin, plugins_) {
		plugin->applySettings();
	}
}
