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
//#include "EventProxy.h"
#include "gui/GUI.h"
#include "JuffPlugin.h"
#include "Log.h"

namespace Juff {

class PluginManager::Interior {
public:
	Interior() {
		gui_ = 0;
		managerInt_ = 0;
		curType_ = "";
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
	
	QMap<QString, PluginList> plugins_;
	GUI::GUI* gui_;
	QStringList engines_;
	ManagerInterface* managerInt_;
	
	QString curType_;
	QMap<QString, QWidgetList> docks_;
	QMap<QString, MenuList> menus_;
	QMap<QString, ToolBarList> toolBars_;
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

void PluginManager::emitInfoSignal(InfoEvent evt, const Param& param1, const Param& param2) {
	JUFFENTRY;
	
	foreach (QString engine, pmInt_->engines_) {
		foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
			if ( plugin ) {
				JUFFDEBUG(QString("Sending to plugin '%1'").arg(plugin->name()));
				plugin->onInfoEvent(evt, param1, param2);
				JUFFDEBUG("Processed successfully");
			}
		}
	}
}

void PluginManager::loadPlugin(const QString& path) {
	JUFFENTRY;

	Log::debug(path);
	QPluginLoader loader(path);
	if ( !loader.load() ) {
		Log::debug("Plugin was NOT loaded");
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

				qDebug(qPrintable(QString("Plugin '%1' loaded").arg(plugin->name())));

				///////////////////////////////
				////	GUI
				QString type = plugin->targetEngine();
				//	docks
				QWidgetList docks = plugin->dockList();
				if ( !docks.isEmpty() ) {
					pmInt_->docks_[type] << docks;
				}
				//	toolbar
				QToolBar* toolBar = plugin->toolBar();
				if ( toolBar ) {
					pmInt_->toolBars_[type] << toolBar;
				}
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
	
	ToolBarList allToolBars;
	foreach (QString type, pmInt_->toolBars_.keys()) {
		allToolBars << pmInt_->toolBars_[type];
	}
	pmInt_->gui_->addToolBars(allToolBars);
	
	activatePlugins("all");
}

ToolBarList PluginManager::getToolBars(const QString& engine) {
	ToolBarList list;
	foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
		if ( plugin->toolBar() )
			list << plugin->toolBar();
	}
	return list;
}

MenuList PluginManager::getMenus(const QString& engine) {
	MenuList menus;
	foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
		if ( plugin->menu() )
			menus << plugin->menu();
	}
	return menus;
}

QWidgetList PluginManager::getDocks(const QString& engine) {
	QWidgetList list;
	foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
		list << plugin->dockList();
	}
	return list;
}

void PluginManager::activatePlugins(const QString& type) {
	JUFFENTRY;
	
	if ( type == pmInt_->curType_ )
		return;
	
	//	hide currently opened controls
	if ( !pmInt_->curType_.isEmpty() ) {
		//	docks
		if ( pmInt_->docks_.contains(pmInt_->curType_) ) {
			foreach (QWidget* w, pmInt_->docks_[pmInt_->curType_]) {
				w->parentWidget()->hide();
			}
		}
		//	toolbars
		if ( pmInt_->toolBars_.contains(pmInt_->curType_) ) {
			foreach (QToolBar* tb, pmInt_->toolBars_[pmInt_->curType_]) {
				tb->hide();
			}
		}
	}
	
	//	requested type
	if ( pmInt_->docks_.contains(type) ) {
		foreach (QWidget* w, pmInt_->docks_[type]) {
			w->parentWidget()->show();
		}
	}
	if ( pmInt_->toolBars_.contains(type) ) {
		foreach (QToolBar* tb, pmInt_->toolBars_[type]) {
			tb->show();
		}
	}
	
	//	'all'
	if ( pmInt_->docks_.contains("all") ) {
		foreach (QWidget* w, pmInt_->docks_["all"]) {
			w->parentWidget()->show();
		}
	}
	if ( pmInt_->toolBars_.contains("all") ) {
		foreach (QToolBar* tb, pmInt_->toolBars_["all"]) {
			tb->show();
		}
	}
	
	pmInt_->curType_ = type;
}


}	//	namespace Juff
