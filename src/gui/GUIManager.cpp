#include <QDebug>

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

#include "GUIManager.h"
#include "Settings.h"

#include <QtGui/QDockWidget>
#include <QtGui/QMenu>
#include <QtGui/QToolBar>

class GUISettings : public Settings {
public:
	static void saveToolBarLastState(QToolBar* tb, bool visible) {
		Settings::setValue("toolBarVisible", tb->windowTitle(), visible);
	}
	static bool toolBarLastState(QToolBar* tb) {
		return Settings::boolValue("toolBarVisible", tb->windowTitle(), true);
	}
	static void saveDockLastState(QWidget* w, bool visible) {
		Settings::setValue("dockVisible", w->parentWidget()->windowTitle(), visible);
	}
	static bool dockLastState(QWidget* w) {
		return Settings::boolValue("dockVisible", w->parentWidget()->windowTitle(), true);
	}
};

void GUIManager::saveLastStates() {
	QMap<QString, Juff::ToolBarList>::iterator it = toolBars_.begin();
	while (it != toolBars_.end()) {
		Juff::ToolBarList list = it.value();
		QString type = it.key();
		foreach (QToolBar* tb, list) {
			bool visible;
			if ( type == curType_ ) 
				visible = tb->isVisible();
			else
				visible = tbLastState_[tb];
			qDebug() << "Saving toolbar" << tb->windowTitle() << "is" << (visible ? "" : "not") << "visible";
			GUISettings::saveToolBarLastState(tb, visible);
		}
		it++;
	}
	QMap<QString, QWidgetList>::iterator it2 = docks_.begin();
	while (it2 != docks_.end()) {
		QWidgetList list = it2.value();
		QString type = it2.key();
		foreach (QWidget* w, list) {
			bool visible;
			if ( type == curType_ ) 
				visible = w->parentWidget()->isVisible();
			else
				visible = dockLastState_[w];
			qDebug() << "Saving dock" << w->parentWidget()->windowTitle() << "is" << (visible ? "" : "not") << "visible";
			GUISettings::saveDockLastState(w, visible);
		}
		it2++;
	}
}

void GUIManager::loadLastStates() {
	QMap<QString, Juff::ToolBarList>::iterator it =  toolBars_.begin();
	while (it != toolBars_.end()) {
		Juff::ToolBarList list = it.value();
		QString type = it.key();
		foreach (QToolBar* tb, list) {
			bool visible = GUISettings::toolBarLastState(tb);
			tbLastState_[tb] = visible;
			qDebug() << "Loading toolbar" << tb->windowTitle() << "is" << (visible ? "" : "not") << "visible";
		}
		it++;
	}
	QMap<QString, QWidgetList>::iterator it2 = docks_.begin();
	while (it2 != docks_.end()) {
		QWidgetList list = it2.value();
		QString type = it2.key();
		foreach (QWidget* w, list) {
			bool visible = GUISettings::dockLastState(w);
			dockLastState_[w] = visible;
			qDebug() << "Loading dock" << w->parentWidget()->windowTitle() << "is" << (visible ? "" : "not") << "visible";
		}
		it2++;
	}
}



void GUIManager::addMenu(const QString& type, QMenu* menu) {
	if ( !menus_.contains(type) )
		menus_[type] = Juff::MenuList();
	menus_[type] << menu;
	menu->menuAction()->setVisible(false);
}

void GUIManager::addMenus(const QString& type, const Juff::MenuList menus) {
	if ( !menus_.contains(type) )
		menus_[type] = Juff::MenuList();
	menus_[type] << menus;
	foreach (QMenu* menu, menus) {
		menu->menuAction()->setVisible(false);
	}
}

void GUIManager::addToolBar(const QString& type, QToolBar* tb) {
	if ( !toolBars_.contains(type) )
		toolBars_[type] = Juff::ToolBarList();
	toolBars_[type] << tb;
	tb->hide();
	tb->toggleViewAction()->setVisible(false);
}

void GUIManager::addToolBars(const QString& type, const Juff::ToolBarList toolBars) {
	if ( !toolBars_.contains(type) )
		toolBars_[type] = Juff::ToolBarList();
	toolBars_[type] << toolBars;
	foreach(QToolBar* tb, toolBars) {
		tb->hide();
		tb->toggleViewAction()->setVisible(false);
	}
}

void GUIManager::addDocks(const QString& type, const QWidgetList& list) {
	if ( !docks_.contains(type) )
		docks_[type] = QWidgetList();
	docks_[type] << list;
	foreach(QWidget* w, list)
		w->parentWidget()->hide();

	//	We can call widget's parentWidget() without checking 
	//	because we know for sure that it has been already embedded
	//	into a dock window (its parent widget).
}


void GUIManager::addActions(const QString& type, const Juff::ActionList& list) {
	if ( !actions_.contains(type) )
		actions_[type] = Juff::ActionList();
	actions_[type] << list;
	foreach(QAction* act, list)
		act->setVisible(false);
}
void GUIManager::addAction(const QString& type, QAction* act) {
	if ( !actions_.contains(type) )
		actions_[type] = Juff::ActionList();
	actions_[type] << act;
	act->setVisible(false);
}


void GUIManager::setCurType(const QString& type) {
	if ( type == curType_ )
		return;
	
	// hide "old" controls
	if ( !curType_.isEmpty() ) {
		foreach(QToolBar* tb, toolBars_[curType_]) {
			tbLastState_[tb] = tb->isVisible();
			tb->hide();
//			qDebug() << "Hiding" << tb->windowTitle() << "toolbar";
		}
		
		
		foreach(QAction* act, actions_[curType_]) {
			act->setVisible(false);
		}
		foreach(QMenu* menu, menus_[curType_]) {
			menu->menuAction()->setVisible(false);
		}
		foreach (QToolBar* tb, toolBars_[curType_]) {
			tb->toggleViewAction()->setVisible(false);
		}
		foreach (QWidget* w, docks_[curType_]) {
			w->parentWidget()->hide();
		}
	}
	else {
		// very first call
		
		foreach (QToolBar* tb, toolBars_["all"]) {
			bool visible = tbLastState_[tb];
			tb->setVisible(visible);
//			qDebug() << "1: Toolbar" << tb->windowTitle() << "is" << (visible ? "" : "not") << "visible";
		}
		foreach (QWidget* w, docks_["all"]) {
			bool visible = dockLastState_[w];
			w->parentWidget()->setVisible(visible);
		}
	}


	// showing the controls of the "new" type (if needed)
	if ( !type.isEmpty() && type != "all" ) {
		foreach (QToolBar* tb, toolBars_[type]) {
			if ( tbLastState_.contains(tb) ) {
				if ( tbLastState_[tb] ) {
					tb->show();
	//				qDebug() << "Showing" << tb->windowTitle() << "toolbar";
				}
			}
			else {
				JUFFDEBUG("Not supposed to be here!!!");
				tb->setVisible(true);
				tbLastState_[tb] = true;
			}
		}
		
		foreach (QWidget* w, docks_[type]) {
			if ( dockLastState_.contains(w) ) {
				if ( dockLastState_[w] ) {
					w->parentWidget()->show();
	//				qDebug() << "Showing" << tb->windowTitle() << "toolbar";
				}
			}
			else {
				JUFFDEBUG("Not supposed to be here!!!");
				w->parentWidget()->setVisible(true);
				dockLastState_[w] = true;
			}
		}
		
		foreach(QAction* act, actions_[type]) {
			act->setVisible(true);
		}
		foreach(QMenu* menu, menus_[type]) {
			menu->menuAction()->setVisible(true);
		}
		foreach (QToolBar* tb, toolBars_[type]) {
			tb->toggleViewAction()->setVisible(true);
		}
	}

	// showing controls of "all" type (if needed)
	foreach(QToolBar* tb, toolBars_["all"]) {
		tbLastState_ [tb] = tb->isVisible();
//		qDebug() << "3: Toolbar" << tb->windowTitle() << "is" << (tb->isVisible() ? "" : "not") << "visible";
	}
	foreach(QWidget* w, docks_["all"]) {
		dockLastState_ [w] = w->parentWidget()->isVisible();
//		qDebug() << "3: Toolbar" << tb->windowTitle() << "is" << (tb->isVisible() ? "" : "not") << "visible";
	}
	foreach(QAction* act, actions_["all"]) {
		act->setVisible(true);
	}
	foreach(QMenu* menu, menus_["all"]) {
		menu->menuAction()->setVisible(true);
	}
	foreach (QToolBar* tb, toolBars_["all"]) {
		tb->toggleViewAction()->setVisible(true);
	}

	if ( type != "all" )
		curType_ = type;
	else
		curType_ = "";
}
