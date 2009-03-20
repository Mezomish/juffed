//	Change this : put your own macro here
#ifndef _PLUGIN_H_
#define _PLUGIN_H_

class QAction;
class QMenu;
class QTextBrowser;

#include <QtCore/QObject>

#include "JuffPlugin.h"

//	Change this : your class name
class Plugin : public QObject, public JuffPlugin {
Q_OBJECT
Q_INTERFACES(JuffPlugin)
public:
	Plugin();
	virtual ~Plugin();

	/*
	* You MUST reimplement the following 3 methods 
	* since they are pure virtual in JuffPlugin class.
	*/
	//	plugin name
	virtual QString name() const;
	//	plugin description
	virtual QString description() const;
	//	engine that the plugin is created for. You can specify
	//	"sci", "rich" or "all".
	virtual QString targetEngine() const;

	/*
	* Reimplementation of the following methods is optional. 
	* Do it if you have your own controls or event handlers.
	*/
	//	controls
	//	plugin menu that will be added to main menubar
	virtual QMenu* menu() const;
	//	plugin toolbar
	virtual QToolBar* toolBar() const;
	//	widgets to be put to docks
	virtual QWidgetList dockList() const;
	//	some actions that we want to add to context menu
	virtual Juff::ActionList contextMenuActions() const;

public slots:
	void view();
	void insertDate();
	void addHeader();

private:
	QAction* viewAction_;
	QAction* dateAction_;
	QAction* headerAction_;
	QMenu* menu_;
	QToolBar* toolBar_;
	QWidget* widget_;
	QTextBrowser* view_;
};

#endif
