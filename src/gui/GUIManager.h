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

#ifndef _GUI_MANAGER_H_
#define _GUI_MANAGER_H_

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtGui/QWidgetList>

#include "Juff.h"

class GUIManager {
public:
	/*
	*	Menus
	*/
	void addMenu(const QString& type, QMenu* menu);
	void addMenus(const QString& type, const Juff::MenuList menus);

	/*
	*	ToolBars
	*/
	void addToolBar(const QString& type, QToolBar* tb);
	void addToolBars(const QString& type, const Juff::ToolBarList toolBars);

	/*
	*	Docks
	*/
	void addDocks(const QString&, const QWidgetList&);

	/*
	*	Misc actions
	*/
	void addAction(const QString& type, QAction* act);
	void addActions(const QString& type, const Juff::ActionList& list);

	void setCurType(const QString& type);
	
	void saveLastStates();
	void loadLastStates();
	
private:
	QMap<QString, Juff::ToolBarList> toolBars_;
	QMap<QString, Juff::MenuList> menus_;
	QMap<QString, Juff::ActionList> actions_;
	QMap<QString, QWidgetList> docks_;

	QString curType_;

	QMap<QToolBar*, bool> tbLastState_;
	QMap<QWidget*, bool> dockLastState_;
};

#endif
