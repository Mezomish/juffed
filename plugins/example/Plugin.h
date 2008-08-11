//	Change this : put your own macro here
#ifndef _PLUGIN_H_
#define _PLUGIN_H_

class QAction;
class QMenu;
class QTextBrowser;
	
#include <QtCore/QObject>

//	Change this: your path to JuffPlugin.h
#include "../../src/JuffPlugin.h"

//	Change this : your class name
class Plugin : public QObject, public JuffPlugin {
Q_OBJECT
Q_INTERFACES(JuffPlugin)
public:
	Plugin();
	virtual ~Plugin();

	//	initialization/deinitialization
	virtual void init(QObject* handler);
	virtual void deinit(QObject* handler);

	//	plugin name
	virtual QString name() const;
	//	plugin description
	virtual QString description() const;

	//	controls
	//	plugin toolbar
	virtual QToolBar* toolBar() const;
	//	plugin menu that will be added to main menubar
	virtual QMenu* menu() const;
	//	widget that will be put to dock
	virtual QWidget* dockWidget(Qt::DockWidgetArea&) const;
	//	plugin settings widget
	virtual QWidget* settingsPage() const;
	//	apply settings
	virtual void applySettings();

signals:
	void textRequest(QString&);
	void requestCursorPos(int&, int&);
	void insertText(const QString&);
	void moveCursor(int, int);

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
