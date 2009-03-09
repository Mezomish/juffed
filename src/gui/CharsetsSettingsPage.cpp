/*
JuffEd - A simple text editor
Copyright 2007-2009 Mikhail Murzin

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

#include "CharsetsSettings.h"
#include "Log.h"

CharsetsSettingsPage::CharsetsSettingsPage(QWidget* parent) : QWidget(parent) {
	JUFFENTRY;
	ui.setupUi(this);

	connect(ui.checkAllBtn, SIGNAL(clicked()), SLOT(selectAll()));
	connect(ui.uncheckAllBtn, SIGNAL(clicked()), SLOT(deselectAll()));
}

CharsetsSettingsPage::~CharsetsSettingsPage() {
}

void CharsetsSettingsPage::init() {
	JUFFENTRY;
	ui.charsetsList->clear();
	foreach (QString charset, CharsetsSettings::getCharsetsList()) {
		QListWidgetItem* item = new QListWidgetItem(charset, ui.charsetsList);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		if (CharsetsSettings::charsetEnabled(charset))
			item->setCheckState(Qt::Checked);
		else
			item->setCheckState(Qt::Unchecked);
		ui.charsetsList->addItem(item);
	}
}

void CharsetsSettingsPage::applySettings() {
	for (int i = 0; i < ui.charsetsList->count(); i++) {
		QListWidgetItem* item = ui.charsetsList->item(i);
		QString text = item->text();
		bool checked = item->checkState() == Qt::Checked;
		CharsetsSettings::setCharsetEnabled(text, checked);
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
