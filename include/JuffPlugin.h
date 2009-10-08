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

#ifndef __JUFF_PLUGIN_H__
#define __JUFF_PLUGIN_H__

#include "Juff.h"
#include "Parameter.h"
#include "ManagerInterface.h"

#include <QtGui/QWidgetList>

class QToolBar;
class QMenu;

class JuffPlugin {
public:
	JuffPlugin() {
		manager_ = 0;
	}

	void setManager(ManagerInterface* m) {
		manager_ = m;
	}

	////////////////////////////////////////////////////////
	//	Plugin information functions
	////////////////////////////////////////////////////////

	/**
	* name()
	*
	* Returns plugin's unique name.
	*/
	virtual QString name() const = 0;
	
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



	////////////////////////////////////////////////////////
	// Info Events
	////////////////////////////////////////////////////////

	/**
	* onDocCreated()
	*
	* This method is called once new doc is created or opened.
	* Parameter \par fileName contains document's file name.
	*/
	virtual void onDocCreated(const QString& fileName) { Q_UNUSED(fileName); }
	
	/**
	* onDocActivated()
	*
	* This method is called once document was activated. 
	* Parameter \par fileName contains document's file name.
	*/
	virtual void onDocActivated(const QString& fileName) { Q_UNUSED(fileName); }
	
	/**
	* onDocModified()
	*
	* This method is called once document's modification status
	* was changed. Parameter \par fileName contains document's file name, 
	* parameter \par modified contains new modification status.
	*/
	virtual void onDocModified(const QString& fileName, bool modified) { Q_UNUSED(fileName); Q_UNUSED(modified); }
	
	/**
	* onDocClosed()
	*
	* This method is called once document was closed. 
	* Parameter \par fileName contains document's file name.
	*/
	virtual void onDocClosed(const QString& fileName) { Q_UNUSED(fileName); }
	
	/**
	* onDocRenamed()
	*
	* This method is called once document was renamed.
	* Parameter \par oldFileName contains document's old file name,
	* parameter \par newFileName contains document's new file name.
	*/
	virtual void onDocRenamed(const QString& oldFileName, const QString& newFileName) { Q_UNUSED(newFileName); Q_UNUSED(oldFileName); }
	
	/**
	* onDocSaved()
	*
	* This method is called once document with file name \par fileName was saved.
	*/
	virtual void onDocSaved(const QString& fileName) { Q_UNUSED(fileName); }
	
	/**
	*	onContextMenuCalled()
	*
	* This method is called once document's context menu is about to be
	* displayed. Parameters \par line, \par col contain cursor position
	* (starting with 0, 0).
	*/
	virtual void onContextMenuCalled(int line, int col) { Q_UNUSED(line); Q_UNUSED(col); }

	/**
	* onTabMoved()
	*
	* This method is called once document's tab was moved from 
	* position \par from to position \par to.
	*/
	virtual void onTabMoved(int from, int to) { Q_UNUSED(from); Q_UNUSED(to); }


protected:
	///	trivial accessor to Manager
	ManagerInterface* manager() const { return manager_; }
	
private:
	ManagerInterface* manager_;
};

Q_DECLARE_INTERFACE(JuffPlugin, "JuffEd.JuffPlugin/2.2")

#endif	//	__JUFF_PLUGIN_H__
