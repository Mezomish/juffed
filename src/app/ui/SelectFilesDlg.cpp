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

#include "SelectFilesDlg.h"

#include "Log.h"

#include <QApplication>
#include <QListWidgetItem>

SelectFilesDlg::SelectFilesDlg(const QStringList& files, QWidget* parent) : QDialog(parent) {
	ui_.setupUi(this);
	ui_.textL->setText(tr("Please select files you'd like to save."));
	
	connect(ui_.selectAllChk, SIGNAL(clicked()), SLOT(onAllClicked()));
	connect(ui_.fileList, SIGNAL(itemChanged(QListWidgetItem*)),
	        SLOT(onItemChanged(QListWidgetItem*)));
	
	connect(ui_.saveBtn, SIGNAL(clicked()), SLOT(slotSave()));
	connect(ui_.dontSaveBtn, SIGNAL(clicked()), SLOT(slotDontSave()));
	connect(ui_.dontCloseBtn, SIGNAL(clicked()), SLOT(reject()));
	
	foreach (QString fileName, files) {
		QListWidgetItem* item = new QListWidgetItem(fileName);
		item->setToolTip(fileName);
		item->setData(Qt::UserRole + 1, fileName);
		item->setCheckState(Qt::Unchecked);
		ui_.fileList->addItem(item);
	}
	
	QApplication::setActiveWindow(this);
}

QStringList SelectFilesDlg::checkedFiles() const {
	QStringList list;

	int count = ui_.fileList->count();
	for (int i = 0; i < count; ++i) {
		QListWidgetItem* item = ui_.fileList->item(i);
		if ( item->checkState() == Qt::Checked ) {
			list << item->data(Qt::UserRole + 1).toString();
		}
	}
	return list;
}

void SelectFilesDlg::slotSave() {
	accept();
}

void SelectFilesDlg::slotDontSave() {
	ui_.selectAllChk->setCheckState(Qt::Unchecked);
	onAllClicked();
	accept();
}

void SelectFilesDlg::onAllClicked() {
	int count = ui_.fileList->count();
	bool isChecked = ui_.selectAllChk->checkState() == Qt::Checked;
	
	for (int i = 0; i < count; ++i) {
		QListWidgetItem* item = ui_.fileList->item(i);
		item->setCheckState( isChecked ? Qt::Checked : Qt::Unchecked );
	}
}

void SelectFilesDlg::onItemChanged(QListWidgetItem*) {
	LOGGER;
	
	QStringList files = checkedFiles();
	
	if ( files.count() == ui_.fileList->count() ) {
		ui_.selectAllChk->setChecked(true);
		ui_.saveBtn->setText(tr("Save all"));
	}
	else {
		ui_.selectAllChk->setChecked(false);
		ui_.saveBtn->setText(tr("Save selected"));
		if ( files.count() == 0 )
			ui_.saveBtn->setEnabled(false);
		else
			ui_.saveBtn->setEnabled(true);
	}
}
