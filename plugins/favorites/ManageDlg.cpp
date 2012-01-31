#include "ManageDlg.h"

#include <JuffPlugin.h>
#include <PluginSettings.h>

ManageDlg::ManageDlg(QWidget* parent, JuffPlugin* plugin) : QDialog(parent), plugin_(plugin) {
	ui.setupUi(this);
	QString str = PluginSettings::getString(plugin, "favorites");
	if ( !str.isEmpty() )
		favorites_ = str.split(";");
	
	ui.favoritesList->addItems(favorites_);
	
	connect(ui.deleteBtn, SIGNAL(clicked()), SLOT(deleteItem()));
	connect(ui.closeBtn, SIGNAL(clicked()), SLOT(close()));
}

void ManageDlg::deleteItem() {
	QListWidgetItem* item = ui.favoritesList->currentItem();
	if ( NULL != item ) {
		QString text = item->text();
		favorites_.removeAll(text);
		delete item;
	}
}

void ManageDlg::close() {
	PluginSettings::set(plugin_, "favorites", favorites_.join(";"));
	accept();
}
