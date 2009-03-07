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
	foreach (QString engine, pmInt_->engines_) {
		foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
			if ( plugin ) {
				plugin->onInfoEvent(evt, param1, param2);
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

//			if ( pmInt_->eventProxy_)
//				plugin->setEventProxy(pmInt_->eventProxy_);
//			pmInt_->pluginList_.append(plugin);
			if ( pmInt_->addPlugin(plugin) ) {

				qDebug(qPrintable(QString("Plugin '%1' loaded").arg(plugin->name())));
//				plugin->setEventProxy(pmInt_->eventProxy_);
//				plugin->setEventProxy(this);

				///////////////////////////////
				////	GUI
				//	toolbar
//				if ( QToolBar* tb = plugin->toolBar() )
//					pmInt_->mw_->addToolBar(tb);

				//	dock windows
/*				QWidgetList docks = plugin->dockList();
				Log::debug(docks.count());
				foreach(QWidget* w, docks) {
					QString title = w->windowTitle();
					if ( title.isEmpty() )
						title = plugin->name();
					QDockWidget* dock = new QDockWidget(title);
					dock->setObjectName(title);
					dock->setWidget(w);
					pmInt_->gui_->mw()->addDockWidget(Qt::LeftDockWidgetArea, dock);

					//	panels toggle actions
	//				if (jInt_->panelsMenu_ != 0) {
	//					jInt_->panelsMenu_->addAction(dock->toggleViewAction());
	//				}
				}*/
			}
			else {
				delete plugin;
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
}

/*void PluginManager::addToolBars(const QString& engine) {
	JUFFENTRY;
	foreach (JuffPlugin* plugin, pmInt_->plugins_[engine]) {
		pmInt_->gui_->addToolBar(plugin->toolBar());
	}
}*/

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


}	//	namespace Juff
