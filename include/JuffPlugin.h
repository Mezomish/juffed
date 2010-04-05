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

#ifndef __JUFF_PLUGIN_H__
#define __JUFF_PLUGIN_H__

#include "DocHandlerInt.h"
#include "Enums.h"
#include "PluginNotifier.h"
#include "Types.h"

#include <QtGui/QWidgetList>

class QToolBar;

class JuffPlugin {
public:
	JuffPlugin() {
		handler_ = 0;
	}

	void setHandler(Juff::DocHandlerInt* h) {
		handler_ = h;
	}

	void setNotifier(Juff::PluginNotifier* n) {
		notifier_ = n;
	}

	virtual void init() {}
	
	////////////////////////////////////////////////////////
	//	Plugin information functions
	////////////////////////////////////////////////////////

	/**
	* name()
	*
	* Returns plugin's unique name. Must NOT be translated.
	*/
	virtual QString name() const = 0;
	
	/**
	* title()
	*
	* Returns plugin's title. Can be translated.
	*/
	virtual QString title() const { return name(); }
	
	/**
	* description()
	*
	* Returns plugin's short description.
	*/
	virtual QString description() const = 0;

	/**
	* targetEngine()
	*
	* Returns the engine this plugin is intended for.
	*/
	virtual QString targetEngine() const = 0;



	////////////////////////////////////////////////////////
	//	Plugin controls functions
	////////////////////////////////////////////////////////

	/**
	* menu()
	*
	* Returns the QMenu of the plugin. 
	* Reimplement this method if you need you own menu.
	*/
	virtual QMenu* menu() const { return 0; }

	/**
	* mainMenuActions()
	*
	* Returns list of actions that should be added to menu
	* with given MenuID.
	* Reimplement this method if you need to add items to main menu.
	*/
	virtual Juff::ActionList mainMenuActions(Juff::MenuID) const { return Juff::ActionList(); }
	
	/**
	* toolBar()
	*
	* Returns the QToolBarof the plugin. 
	* Reimplement this method if you need you own toolbar.
	*/
	virtual QToolBar* toolBar() const { return 0; }

	/**
	* contextMenuActions()
	*
	* Returns list of actions to be added to document's context menu.
	* Reimplement this method if your plugin adds some.
	*/
	virtual Juff::ActionList contextMenuActions() const { return Juff::ActionList(); }
	
	/**
	* dockList()
	*
	* Returns the list of dock windows of the plugin. 
	* Reimplement this method if you have dock window(s).
	*/
	virtual QWidgetList dockList() const { return QWidgetList(); }
	
	/**
	* settingsPage()
	*
	* Returns the settings page of the plugin. 
	* Reimplement this method if your plugin has settings page.
	*/
	virtual QWidget* settingsPage() const { return 0; }
	
	/**
	* applySettings()
	*
	* Called when 'Apply' or "OK' button in 'Settings' dialog was pressed.
	*/
	virtual void applySettings() { }

	/**
	* dockPosition()
	*
	* Returns the default position of plugin's dock widget.
	*/
	virtual Qt::DockWidgetArea dockPosition(QWidget*) const { return Qt::LeftDockWidgetArea; }



protected:
	/// accessor to DocHandler
	Juff::DocHandlerInt* handler() const { return handler_; }
	/// accessor to PluginNotifier
	Juff::PluginNotifier* notifier() const { return notifier_; }
	
private:
	Juff::DocHandlerInt* handler_;
	Juff::PluginNotifier* notifier_;
};

Q_DECLARE_INTERFACE(JuffPlugin, "JuffEd.JuffPlugin/2.5")

#endif	//	__JUFF_PLUGIN_H__
