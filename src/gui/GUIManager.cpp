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

#include <QtGui/QDockWidget>
#include <QtGui/QMenu>
#include <QtGui/QToolBar>

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
}

void GUIManager::addToolBars(const QString& type, const Juff::ToolBarList toolBars) {
	if ( !toolBars_.contains(type) )
		toolBars_[type] = Juff::ToolBarList();
	toolBars_[type] << toolBars;
	foreach(QToolBar* tb, toolBars)
		tb->hide();
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
	
	//	remembering the visibility of docks of type 'all'
	if ( docks_.contains("all") ) {
		foreach (QWidget* w, docks_["all"]) {
			dockVisible_[w] = w->parentWidget()->isVisible();
		}
	}
	
	//	hide controls of the current type
	if ( toolBars_.contains(curType_) ) {
		foreach (QToolBar* tb, toolBars_[curType_]) {
			tb->hide();
		}
	}
	if ( menus_.contains(curType_) ) {
		foreach (QMenu* menu, menus_[curType_]) {
			menu->menuAction()->setVisible(false);
		}
	}
	if ( actions_.contains(curType_) ) {
		foreach (QAction* act, actions_[curType_] ) {
			act->setVisible(false);
		}
	}
	if ( docks_.contains(curType_) ) {
		foreach (QWidget* w, docks_[curType_]) {
			QDockWidget* dock = qobject_cast<QDockWidget*>(w->parentWidget());
			if ( dock ) {
				dockVisible_[w] = dock->isVisible();
				dock->hide();
				dock->toggleViewAction()->setVisible(false);
			}
		}
	}
	
	//	show controls of the new type
	if ( toolBars_.contains(type) ) {
		foreach (QToolBar* tb, toolBars_[type]) {
			tb->show();
		}
	}
	if ( menus_.contains(type) ) {
		foreach (QMenu* menu, menus_[type]) {
			menu->menuAction()->setVisible(true);
		}
	}
	if ( actions_.contains(type) ) {
		foreach (QAction* act, actions_[type] ) {
			act->setVisible(true);
		}
	}
	if ( docks_.contains(type) ) {
		foreach (QWidget* w, docks_[type]) {
			QDockWidget* dock = qobject_cast<QDockWidget*>(w->parentWidget());
			if ( dock ) {
				if ( dockVisible_.contains(w) && dockVisible_[w] ) {
					dock->show();
				}
				dock->toggleViewAction()->setVisible(true);
			}
		}
	}
	
	if ( type != "all" ) {
		//	show controls of 'all' type
		if ( toolBars_.contains("all") ) {
			foreach (QToolBar* tb, toolBars_["all"]) {
				tb->show();
			}
		}
		if ( menus_.contains("all") ) {
			foreach (QMenu* menu, menus_["all"]) {
				menu->menuAction()->setVisible(true);
			}
		}
		if ( actions_.contains("all") ) {
			foreach (QAction* act, actions_["all"] ) {
				act->setVisible(true);
			}
		}
		if ( docks_.contains("all") ) {
			foreach (QWidget* w, docks_["all"] ) {
				QDockWidget* dock = qobject_cast<QDockWidget*>(w->parentWidget());
				if ( dockVisible_.contains(w) ) {
					dock->setVisible(dockVisible_[w]);
				}
				else {
					dock->setVisible(true);
					dockVisible_[w] = true;
				}
				dock->toggleViewAction()->setVisible(true);
			}
		}
	}
	
	curType_ = type;
}
