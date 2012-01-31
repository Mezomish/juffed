#include "FavoritesPlugin.h"
#include "ManageDlg.h"

#include <QtCore>
#include <QtGui/QMenu>

#include <Document.h>
#include <Log.h>
#include <PluginSettings.h>

FavoritesPlugin::FavoritesPlugin() : QObject(), JuffPlugin() {
	menu_ = new QMenu("Favorites");
	addAct_ = new QAction(tr("Add current file"), 0);
	manageAct_ = new QAction(tr("Manage favorites"), 0);
	connect(addAct_, SIGNAL(triggered()), SLOT(addCurrent()));
	connect(manageAct_, SIGNAL(triggered()), SLOT(manageFavorites()));
	load();
	initMenu();
}

FavoritesPlugin::~FavoritesPlugin() {
}

QString FavoritesPlugin::name() const {
	return "Favorites";
}

QString FavoritesPlugin::targetEngine() const {
	return "all";
}

QString FavoritesPlugin::description() const {
	return "Keeping your favorites";
}

QMenu* FavoritesPlugin::menu() const {
	return menu_;
}

void FavoritesPlugin::addCurrent() {
	LOGGER;
	
	QString fileName;
	Juff::Document* doc = api()->currentDocument();
	if ( !doc->isNull() && !doc->isNoname() ) {
		if ( !favorites_.contains(doc->fileName()) ) {
			favorites_ << doc->fileName();
			initMenu();
			save();
		}
	}
}

void FavoritesPlugin::manageFavorites() {
	ManageDlg dlg(QApplication::activeWindow(), this);
	dlg.exec();
	load();
	initMenu();
}

void FavoritesPlugin::openFavorite() {
	QAction* act = qobject_cast<QAction*>(sender());
	if ( NULL == act )
		return;
	
	QString fileName = act->text();
	api()->openDoc(fileName);
}

void FavoritesPlugin::load() {
	QString str = PluginSettings::getString(this, "favorites");
	if ( !str.isEmpty() )
		favorites_ = str.split(";");
	else
		favorites_ = QStringList();
}

void FavoritesPlugin::save() {
	PluginSettings::set(this, "favorites", favorites_.join(";"));
}

void FavoritesPlugin::initMenu() {
	LOGGER;
	menu_->clear();
	
	menu_->addAction(addAct_);
	menu_->addAction(manageAct_);
	if ( favorites_.count() > 0 ) {
		menu_->addSeparator();
		foreach ( QString item, favorites_ ) {
			menu_->addAction(item, this, SLOT(openFavorite()));
		}
	}
}

Q_EXPORT_PLUGIN2(favorites, FavoritesPlugin)
