/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

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

#include "CharsetsSettingsPage.h"

#include "CharsetSettings.h"
#include "Log.h"

CharsetsSettingsPage::CharsetsSettingsPage(QWidget* parent) : SettingsPage(parent) {
	ui.setupUi(this);

	connect(ui.checkAllBtn, SIGNAL(clicked()), SLOT(selectAll()));
	connect(ui.uncheckAllBtn, SIGNAL(clicked()), SLOT(deselectAll()));
	connect(ui.charsetsList, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(onItemClicked(QListWidgetItem*)));
}

CharsetsSettingsPage::~CharsetsSettingsPage() {
}

void CharsetsSettingsPage::init() {
	ui.charsetsList->clear();
	foreach (QString charset, CharsetSettings::getCharsetsList()) {
		QListWidgetItem* item = new QListWidgetItem(charset, ui.charsetsList);
		item->setFlags(Qt::ItemIsEnabled);
		if (CharsetSettings::charsetEnabled(charset))
			item->setCheckState(Qt::Checked);
		else
			item->setCheckState(Qt::Unchecked);
		ui.charsetsList->addItem(item);
	}
}

void CharsetsSettingsPage::apply() {
	for (int i = 0; i < ui.charsetsList->count(); i++) {
		QListWidgetItem* item = ui.charsetsList->item(i);
		QString text = item->text();
		bool checked = item->checkState() == Qt::Checked;
		CharsetSettings::setCharsetEnabled(text, checked);
	}
}

void CharsetsSettingsPage::selectAll() {
	for (int i = 0; i < ui.charsetsList->count(); i++) {
		QListWidgetItem* item = ui.charsetsList->item(i);
		item->setCheckState(Qt::Checked);
	}
}

void CharsetsSettingsPage::deselectAll() {
	for (int i = 0; i < ui.charsetsList->count(); i++) {
		QListWidgetItem* item = ui.charsetsList->item(i);
		item->setCheckState(Qt::Unchecked);
	}
}

void CharsetsSettingsPage::onItemClicked( QListWidgetItem* item ) {
	if ( item != 0 ) {
		item->setCheckState( item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked );
	}
}
