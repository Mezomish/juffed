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
	dateAction_ = new QAction("Insert current date", this);
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
	//	set widget title
	widget_->setWindowTitle("HTML View");
	//	add text browser on it
	view_ = new QTextBrowser();
	QVBoxLayout* vBox = new QVBoxLayout();
	vBox->addWidget(view_);
	widget_->setLayout(vBox);
}

Plugin::~Plugin() {
	delete menu_;
	delete toolBar_;
	delete widget_;
}

QString Plugin::name() const {
//	Change this : your plugin name here
	return "PluginExample";
}

QString Plugin::description() const {
//	Change this : you own description here
	return "This is a plugin example.";
}

QString Plugin::targetEngine() const {
	return "sci";
}

QToolBar* Plugin::toolBar() const {
	return toolBar_;
}

QMenu* Plugin::menu() const {
	//	return you own menu (if you have one) here
	return menu_;
}

Juff::ActionList Plugin::contextMenuActions() const {
	Juff::ActionList list;
	list << dateAction_;
	return list;
}

QWidgetList Plugin::dockList() const {
	QWidgetList list;
	list << widget_;
	return list;
}

void Plugin::view() {
	//	request document text
	QString text;
	manager()->getCurrentDocText(text);

	//	update the text
	view_->setHtml(text);
	
	if ( !widget_->isVisible() ) {
		//	show widget's parent (dock) if it is not visible,
		if ( widget_->parentWidget() != 0 )
			widget_->parentWidget()->show();
	}
}

void Plugin::insertDate() {
	QDateTime dt = QDateTime::currentDateTime();
	manager()->insertText(dt.toString("hh:mm:ss dd-MM-yyyy"));
}

void Plugin::addHeader() {
	QString text = "/* This program was written to make everybody happy */\n";
	manager()->setCursorPos(0, 0);
	manager()->insertText(text);
}

Q_EXPORT_PLUGIN2(plugin, Plugin)
