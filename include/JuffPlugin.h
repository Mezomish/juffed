#ifndef _JUFF_PLUGIN_H_
#define _JUFF_PLUGIN_H_

//#include "Functions.h"
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

	/**
	*	name()
	*
	*	Returns plugin's unique name.
	*/
	virtual QString name() const = 0;
	
	/**
	*	description()
	*
	*	Returns plugin's short description.
	*/
	virtual QString description() const = 0;

	/**
	*	targetEngine()
	*
	*	Returns the engine this plugin is intended for.
	*/
	virtual QString targetEngine() const = 0;
	
	/**
	*	menu()
	*
	*	Returns the QMenu of the plugin. 
	*	Reimplement this method if you need you own menu.
	*/
	virtual QMenu* menu() const { return 0; }

	/**
	*	toolBar()
	*
	*	Returns the QToolBarof the plugin. 
	*	Reimplement this method if you need you own toolbar.
	*/
	virtual QToolBar* toolBar() const { return 0; }

	/**
	*	settingsPage()
	*
	*	Returns the settings page of the plugin. 
	*	Reimplement this method if your plugin has settings page.
	*/
	virtual QWidget* settingsPage() const { return 0; }

	/**
	*	dockList()
	*
	*	Returns the list of dock windows of the plugin. 
	*	Reimplement this method if you have dock window(s).
	*/
	virtual QWidgetList dockList() const { return QWidgetList(); }
	
	/**
	*	onInfoEvent()
	*
	*	This method is called once some event happened (see Juff.h, enum InfoEvent).
	*	Reimplement this method if you need those notifications.
	*/
	virtual void onInfoEvent(Juff::InfoEvent, const Juff::Param&, const Juff::Param&) {}

protected:
	///	trivial accessor
	ManagerInterface* manager() const { return manager_; }
	
private:
	ManagerInterface* manager_;
};

Q_DECLARE_INTERFACE(JuffPlugin, "JuffEd.JuffPlugin/2.0")

#endif	//	_JUFF_PLUGIN_H_
