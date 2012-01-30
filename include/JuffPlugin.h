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

#include "LibConfig.h"

#include "DocHandlerInt.h"
#include "Enums.h"
#include "JuffAPI.h"
#include "Types.h"

#include <QtGui/QWidgetList>

class QToolBar;

class LIBJUFF_EXPORT JuffPlugin {
public:
	/**
	* This method is called after plugin is loaded and all handlers are set.
	* If the plugin connects to any signals from JuffAPI (gotten by calling 
	* the "api()" method) then these connections should be performed in this 
	* method.
	*
	* WARNING!!! WARNING!!! WARNING!!!
	* ----------------------------------------------------------------
	* DO NOT connect any signals in plugin's constructor since        
	* calling the "api()" method from the constructor will return 0.  
	* ----------------------------------------------------------------
	*/
	virtual void init() {}
	
	////////////////////////////////////////////////////////
	//	Plugin information functions
	////////////////////////////////////////////////////////

	/**
	* Returns plugin's unique name. Must NOT be translated.
	*/
	virtual QString name() const = 0;
	
	/**
	* Returns plugin's title. Can be translated.
	* If not reimplemented then returns JuffPlugin::name().
	*/
	virtual QString title() const { return name(); }
	
	/**
	* Returns plugin's short description.
	*/
	virtual QString description() const = 0;

	/**
	* Returns the engine this plugin is intended for.
	*/
	virtual QString targetEngine() const = 0;



	////////////////////////////////////////////////////////
	//	Plugin controls functions
	////////////////////////////////////////////////////////

	/**
	* Returns the QMenu of the plugin. 
	* Reimplement this method if you need you own menu.
	*/
	virtual QMenu* menu() const { return 0; }

	/**
	* Returns list of actions that should be added to menu with given MenuID.
	* Reimplement this method if you need to add items to main menu. For the
	* list of available menu IDs see the file Enums.h.
	*/
	virtual Juff::ActionList mainMenuActions(Juff::MenuID) const { return Juff::ActionList(); }
	
	/**
	* Returns the QToolBar of the plugin. 
	* Reimplement this method if you need you own toolbar.
	*/
	virtual QToolBar* toolBar() const { return 0; }

	/**
	* Returns list of actions to be added to document's context menu.
	* If you need to add a sub-menu use QMenu::menuAction() method.
	*/
	virtual Juff::ActionList contextMenuActions() const { return Juff::ActionList(); }
	
	/**
	* Returns the list of dock windows of the plugin. 
	* Reimplement this method if you have dock window(s).
	*/
	virtual QWidgetList dockList() const { return QWidgetList(); }
	
	/**
	* Returns the settings page of the plugin. 
	* Reimplement this method if your plugin has settings page.
	*/
	virtual QWidget* settingsPage() const { return 0; }
	
	/**
	* applySettings()
	*
	* Called when 'Apply' or "OK' button in 'Settings' dialog was pressed.
	* Use this method to store settings from plugin's settings page.
	*/
	virtual void applySettings() { }

	/**
	* Returns the default position of plugin's dock widget.
	*/
	virtual Qt::DockWidgetArea dockPosition(QWidget*) const { return Qt::LeftDockWidgetArea; }

	/**
	* Returns the default visibility of plugin's dock widget.
	*/
	virtual bool dockVisible(QWidget*) const { return false; }

	/**
	* Called after all plugins were loaded.
	*/
	virtual void allPluginsLoaded() {}
	
	JuffPlugin() { api_ = 0; }
	void setAPI(JuffAPI* api) { api_ = api; }

protected:
	/// accessor to JuffAPI object
	JuffAPI* api() const { return api_; }
	
private:
	JuffAPI* api_;
};

Q_DECLARE_INTERFACE(JuffPlugin, "JuffEd.JuffPlugin/2.693")

#endif	//	__JUFF_PLUGIN_H__
