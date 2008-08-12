#include "Plugin.h"

#include <QtCore>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QTextBrowser>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>

Plugin::Plugin() : QObject(), JuffPlugin() {
	//	create actions and connect them to slots
	viewAction_ = new QAction("View HTML", this);
	dateAction_ = new QAction("Current date", this);
	headerAction_ = new QAction("Add header", this);
	connect(viewAction_, SIGNAL(activated()), this, SLOT(view()));
	connect(dateAction_, SIGNAL(activated()), this, SLOT(insertDate()));
	connect(headerAction_, SIGNAL(activated()), this, SLOT(addHeader()));
	
	//	create menu
	menu_ = new QMenu("Plugin");
	menu_->addAction(viewAction_);
	menu_->addAction(dateAction_);
	menu_->addAction(headerAction_);

	//	create toolbar
	toolBar_ = new QToolBar("Plugin Example");

	//	You better specify the name, otherwise its position will 
	//	not be restored after application restart
	toolBar_->setObjectName("PluginToolBar");

	//	add 'View' action to toolbar
	toolBar_->addAction(viewAction_);
	
	//	create widget that will be embedded into dock 
	//	and put controls to it
	widget_ = new QWidget();
	view_ = new QTextBrowser();
	QVBoxLayout* vBox = new QVBoxLayout();
	vBox->addWidget(view_);
	widget_->setLayout(vBox);
}

Plugin::~Plugin() { 
}

QString Plugin::name() const {
//	Change this : your plugin name here
	return "PluginExample";
}

void Plugin::init(QObject* handler) {
	//	Change this : add your SIGNAL/SLOT connections here
	//	this is the way how plugin interacts with main application
	//	See class DocHandler and its SIGNALS and SLOTS for details
	connect(this, SIGNAL(textRequest(QString&)), handler, SLOT(getText(QString&)));
	connect(this, SIGNAL(insertText(const QString&)), handler, SLOT(insertText(const QString&)));
	connect(this, SIGNAL(moveCursor(int, int)), handler, SLOT(setCursorPos(int, int)));
	connect(this, SIGNAL(requestCursorPos(int&, int&)), handler, SLOT(getCursorPos(int&, int&)));
}

void Plugin::deinit(QObject* handler) {
	//	Disconnect all SIGNAL/SLOT connections here. The best way is just to call
	//	  this->disconnect(handler);
	//	  handler->disconnect(this);
	//	if you have connection in both directions.
	//	Since here we have only connections between plugin's signals and 
	//	handler's slots, it's enough to call just one of them:	
	this->disconnect(handler);
}

QString Plugin::description() const {
//	Change this : you own description here
	return "This is a plugin example.";
}

QToolBar* Plugin::toolBar() const {
	return toolBar_;
}
QWidget* Plugin::settingsPage() const {
	//	return you settings widget (if you have one) here
	return 0;
}
QMenu* Plugin::menu() const {
	//	return you own menu (if you have one) here
	return menu_;
}
void Plugin::applySettings() {
}


QWidget* Plugin::dockWidget(Qt::DockWidgetArea& area) const {
	//	change widget's dock default location
	area = Qt::BottomDockWidgetArea;

	return widget_;
}

void Plugin::view() {
	//	request document text
	QString text;
	emit textRequest(text);

	//	update the text
	view_->setHtml(text);
	
	if (!widget_->isVisible()) {
		//	show widget's parent (dock) if it is not visible,
		if (widget_->parentWidget() != 0)
			widget_->parentWidget()->show();
	}
}

void Plugin::insertDate() {
	QDateTime dt = QDateTime::currentDateTime();
	emit insertText(dt.toString("hh:mm:ss dd-MM-yyyy"));
}

void Plugin::addHeader() {
	QString text = "/* This program was written to make everybody happy */\n";
	emit moveCursor(0, 0);
	emit insertText(text);
}

Q_EXPORT_PLUGIN2(plugin, Plugin)
