#include "SettingsPage.h"

#include "Log.h"
#include "SettingsItem.h"

SettingsPage::SettingsPage(QWidget* parent) : QWidget(parent) {
}

void SettingsPage::apply() {
	LOGGER;
	foreach (SettingsItem* item, items_)
		item->writeValue();
}
