/*
JuffEd - A simple text editor
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

#include "PluginManager.h"

#include <QtCore/QDir>
#include <QtCore/QMap>
#include <QtCore/QPluginLoader>
#include <QtGui/QMainWindow>
#include <QtGui/QToolBar>

#include "AppInfo.h"
#include "gui/GUI.h"
#include "JuffPlugin.h"
#include "Log.h"

namespace Juff {

class PluginManager::Interior {
public:
	Interior() {
		managerInt_ = 0;
		gui_ = 0;
		curEngine_ = "";
	}
	~Interior() {
	}
	
	bool addPlugin(JuffPlugin* plugin) {
		QString engine = plugin->targetEngine();

		Log::debug(engine);
		
		if ( engines_.contains(engine) ) {
			//	plugin has specified its type and this type
			//	is allowed by PluginManager
			plugins_[engine].append(plugin);
			return true;
		}
		else {
			Log::printToLog(QString("Engine type %1 is not acceptable. Plugin %2 was not loaded").arg(engine).arg(plugin->name()));
			return false;
		}
	}
	
	ManagerInterface* managerInt_;
	GUI::GUI* gui_;
	QMap<QString, PluginList> plugins_;

	QStringList engines_;
	QString curEngine_;
	
	//	stores and manages dock windows (shows and hides when active engine is changed
	QMap<QString, QWidgetList> docks_;
	
	//	stores all plugins' context menu actions
	QMap<QString, ActionList> contextMenuActions_;
};


PluginManager::PluginManager(const QStringList& engines, ManagerInterface* m, GUI::GUI* gui) : QObject() {
	pmInt_ = new Interior();
	pmInt_->engines_ = engines;
	pmInt_->engines_ << "all";
	pmInt_->gui_ = gui;
	pmInt_->managerInt_ = m;
}

PluginManager::~PluginManager() {
	delete pmInt_;
}

////////////////////////////////////////////////////////////
//	Plugins Events

void PluginManager::notifyDocCreated(const QString& fileName) {
	JUFFENTRY;
	foreach (QString engine, pmInt_->engines_) {
		foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
			if ( plugin ) {
				plugin->onDocCreated(fileName);
			}
		}
	}
}

void PluginManager::notifyDocActivated(const QString& fileName) {
	JUFFENTRY;
	foreach (QString engine, pmInt_->engines_) {
		foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
			if ( plugin ) {
				plugin->onDocActivated(fileName);
			}
		}
	}
}

void PluginManager::notifyDocModified(const QString& fileName, bool modified) {
	JUFFENTRY;
	foreach (QString engine, pmInt_->engines_) {
		foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
			if ( plugin ) {
				plugin->onDocModified(fileName, modified);
			}
		}
	}
}

void PluginManager::notifyDocClosed(const QString& fileName) {
	JUFFENTRY;
	foreach (QString engine, pmInt_->engines_) {
		foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
			if ( plugin ) {
				plugin->onDocClosed(fileName);
			}
		}
	}
}

void PluginManager::notifyDocRenamed(const QString& oldFileName, const QString& newFileName) {
	JUFFENTRY;
	foreach (QString engine, pmInt_->engines_) {
		foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
			if ( plugin ) {
				plugin->onDocRenamed(oldFileName, newFileName);
			}
		}
	}
}

void PluginManager::notifyContextMenuCalled(int line, int col) {
	JUFFENTRY;
	foreach (QString engine, pmInt_->engines_) {
		foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
			if ( plugin ) {
				plugin->onContextMenuCalled(line, col);
			}
		}
	}
}


////////////////////////////////////////////////////////////


void PluginManager::loadPlugin(const QString& path) {
	JUFFENTRY;

	QPluginLoader loader(path);
	if ( !loader.load() ) {
		Log::debug(QString("Plugin '%1' was NOT loaded").arg(path));
		return;
	}
	
	QObject *obj = loader.instance();
	if ( obj ) {
		JuffPlugin* plugin = qobject_cast<JuffPlugin*>(obj);
		if ( plugin ) {
			plugin->setManager(pmInt_->managerInt_);
			
			//	Check if plugin with the same name was already loaded.
			//	If is was then exit.
//			if (pluginExists(plugin->name()))
//				return;

			if ( pmInt_->addPlugin(plugin) ) {

				qDebug(qPrintable(QString("-----=====((((( Plugin '%1' was loaded successfully! )))))=====-----").arg(plugin->name())));

				///////////////////////////////
				////	GUI
				QString type = plugin->targetEngine();
				//	docks
				QWidgetList docks = plugin->dockList();
				if ( !docks.isEmpty() ) {
					pmInt_->docks_[type] << docks;
				}
				
				//	context menu actions
				pmInt_->contextMenuActions_[type] << plugin->contextMenuActions();
			}
			else {
				loader.unload();
			}
		}
	}
}

void PluginManager::loadPlugins() {
	JUFFENTRY;
	//	user's plugins
	QDir pluginDir(AppInfo::configDirPath() + "/plugins");
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
	
	QWidgetList allDocks;
	foreach (QString type, pmInt_->docks_.keys()) {
		allDocks << pmInt_->docks_[type];
	}
	pmInt_->gui_->addDocks(allDocks);
}



////////////////////////////////////////////////////////////
//	GUI controls

MenuList PluginManager::getMenus(const QString& engine) {
	JUFFENTRY;
	MenuList menus;
	foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
		if ( plugin->menu() )
			menus << plugin->menu();
	}
	return menus;
}

MenuList PluginManager::getMenuActions(const QString& engine, MenuID id) {
	JUFFENTRY;
	MenuList list;
	return list;
}

ToolBarList PluginManager::getToolBars(const QString& engine) {
	JUFFENTRY;
	ToolBarList list;
	foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
		if ( plugin->toolBar() )
			list << plugin->toolBar();
	}
	return list;
}

QWidgetList PluginManager::getDocks(const QString& engine) {
	JUFFENTRY;
	QWidgetList list;
	foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
		list << plugin->dockList();
	}
	return list;
}

ActionList PluginManager::getContextMenuActions(const QString& engine) {
	if ( pmInt_->contextMenuActions_.contains(engine) )
		return pmInt_->contextMenuActions_[engine];
	else
		return ActionList();
}



void PluginManager::setActiveEngine(const QString& type) {
	JUFFENTRY;
	
	if ( type == pmInt_->curEngine_ )
		return;

	//	hide currently opened controls
	if ( !pmInt_->curEngine_.isEmpty() ) {
		//	docks
		if ( pmInt_->docks_.contains(pmInt_->curEngine_) ) {
			foreach (QWidget* w, pmInt_->docks_[pmInt_->curEngine_]) {
				w->parentWidget()->hide();
			}
		}
	}

	//	requested type
	if ( pmInt_->docks_.contains(type) ) {
		foreach (QWidget* w, pmInt_->docks_[type]) {
			w->parentWidget()->show();
		}
	}

	//	'all'
	if ( pmInt_->docks_.contains("all") ) {
		foreach (QWidget* w, pmInt_->docks_["all"]) {
			w->parentWidget()->show();
		}
	}

	pmInt_->curEngine_ = type;
}


}	//	namespace Juff
