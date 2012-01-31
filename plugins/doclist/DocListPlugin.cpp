#include "DocListPlugin.h"

#include <QtCore>

//#include "CommandStorage.h"
#include "Document.h"
#include "DocListPanel.h"
//#include "EditorSettings.h"
#include "Log.h"

DocListPlugin::DocListPlugin() : QObject(), JuffPlugin() {
	panel_ = new DocListPanel();
	
	connect(panel_->tree_, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(docClicked(QTreeWidgetItem*, int)));
	
//	applySettings();
}

void DocListPlugin::init() {
	connect(api(), SIGNAL(docOpened(Juff::Document*, Juff::PanelIndex)), SLOT(onDocOpened(Juff::Document*, Juff::PanelIndex)));
	connect(api(), SIGNAL(docActivated(Juff::Document*)), SLOT(onDocActivated(Juff::Document*)));
	connect(api(), SIGNAL(docClosed(Juff::Document*)), SLOT(onDocClosed(Juff::Document*)));
	connect(api(), SIGNAL(docRenamed(Juff::Document*, const QString&)), SLOT(onDocRenamed(Juff::Document*, const QString&)));
	connect(api(), SIGNAL(docModified(Juff::Document*)), SLOT(onDocModified(Juff::Document*)));
}

DocListPlugin::~DocListPlugin() { 
}

QString DocListPlugin::name() const {
	return "Documents List";
}

QString DocListPlugin::targetEngine() const {
	return "all";
}

QString DocListPlugin::description() const {
	return "Plugin that allows to view all opened files as a list";
}

QWidgetList DocListPlugin::dockList() const {
	QWidgetList wList;
	wList << panel_;
	return wList;
}

bool DocListPlugin::dockVisible(QWidget* w) const {
	if ( w == panel_ ) {
		return true;
	}
	else {
		return JuffPlugin::dockVisible(w);
	}
}

void DocListPlugin::onDocOpened(Juff::Document* doc, Juff::PanelIndex) {
//	LOGGER;
	
	QStringList list; 
	list << doc->title() << doc->fileName();
	
	QTreeWidgetItem* item = new QTreeWidgetItem(list);
	item->setIcon(0, QIcon(QString(":doc_icon")));
	item->setToolTip(0, doc->fileName());
	panel_->tree_->addTopLevelItem(item);
	panel_->tree_->setCurrentItem(item);
}

void DocListPlugin::onDocActivated(Juff::Document* doc) {
//	LOGGER;
	
	QList<QTreeWidgetItem*> items = panel_->tree_->findItems(doc->fileName(), Qt::MatchFixedString, 1);
	if ( !items.isEmpty() )
		panel_->tree_->setCurrentItem(items[0]);
}

void DocListPlugin::onDocClosed(Juff::Document* doc) {
//	LOGGER;
	
	QList<QTreeWidgetItem*> items = panel_->tree_->findItems(doc->fileName(), Qt::MatchFixedString, 1);

	foreach (QTreeWidgetItem* item, items) {
		delete item;
	}
}

void DocListPlugin::onDocRenamed(Juff::Document* doc, const QString& oldName) {
//	LOGGER;
	
	QList<QTreeWidgetItem*> items = panel_->tree_->findItems(oldName, Qt::MatchFixedString, 1);

	foreach (QTreeWidgetItem* item, items) {
		item->setText(0, doc->title());
		item->setText(1, doc->fileName());
		item->setToolTip(0, doc->fileName());
	}
}

void DocListPlugin::onDocModified(Juff::Document* doc) {
//	LOGGER;
	
	QList<QTreeWidgetItem*> items = panel_->tree_->findItems(doc->fileName(), Qt::MatchFixedString, 1);

	foreach (QTreeWidgetItem* item, items) {
		QString fName = doc->titleWithModification();
		
		item->setText(0, fName);
	}
}



/*

void DocListPlugin::onTabMoved(int from, int to) {
	QTreeWidgetItem* item = panel_->tree_->takeTopLevelItem(from);
	if ( item ) {
		panel_->tree_->insertTopLevelItem(to, item);
	}
}*/

void DocListPlugin::docClicked(QTreeWidgetItem* it, int) {
//	LOGGER;
	if ( 0 != it )
		api()->openDoc(it->text(1));
}

/*
void DocListPlugin::applySettings() {
	QPalette plt = panel_->tree_->palette();
	plt.setColor(QPalette::Base, EditorSettings::get(EditorSettings::DefaultBgColor));
	plt.setColor(QPalette::Text, EditorSettings::get(EditorSettings::DefaultFontColor));
	plt.setColor(QPalette::Highlight, EditorSettings::get(EditorSettings::SelectionBgColor));
//	plt.setColor(QPalette::HighlightedText, EditorSettings::get(EditorSettings::SelectionTextColor);
	panel_->tree_->setPalette(plt);
	panel_->filter_->setPalette(plt);
}
*/

Q_EXPORT_PLUGIN2(doclist, DocListPlugin)
