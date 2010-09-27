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

#include "FileTypesPage.h"

#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>

#include "FileTypeSettings.h"
#include "../../qsci/LexerStorage.h"

FileTypesPage::FileTypesPage() : QWidget() {
	ui.setupUi(this);

	connect(ui.typeList, SIGNAL(currentTextChanged(const QString&)), SLOT(typeChanged(const QString&)));
	connect(ui.addFNBtn, SIGNAL(clicked()), SLOT(addFileNamePattern()));
	connect(ui.removeFNBtn, SIGNAL(clicked()), SLOT(removeFileNamePattern()));
	connect(ui.addFLBtn, SIGNAL(clicked()), SLOT(addFirstLinePattern()));
	connect(ui.removeFLBtn, SIGNAL(clicked()), SLOT(removeFirstLinePattern()));

	//	file types
	QStringList types = FileTypeSettings::getTypeList();
	if ( types.isEmpty() ) {
		types = LexerStorage::instance()->lexersList();
		types.removeAll("none");
	}
	
	foreach( QString type, types ) {
		fileNamePatterns_[type] = FileTypeSettings::getFileNamePatterns(type);
		firstLinePatterns_[type] = FileTypeSettings::getFirstLinePatterns(type);
	}
	ui.typeList->addItems(types);
}

void FileTypesPage::typeChanged(const QString& type) {
	if ( !type.isNull() ) {
		ui.fileNameList->clear();
		if ( fileNamePatterns_.contains(type) )
			ui.fileNameList->addItems(fileNamePatterns_[type]);
		
		ui.firstLineList->clear();
		if ( firstLinePatterns_.contains(type) )
			ui.firstLineList->addItems(firstLinePatterns_[type]);
	}
}

void FileTypesPage::addFileNamePattern() {
	if ( QListWidgetItem* item = ui.typeList->currentItem() ) {
		QString str = QInputDialog::getText(this, tr("New file name pattern"), tr("New file name pattern"));
		if ( !str.isEmpty() ) {
			ui.fileNameList->addItem(str);
			fileNamePatterns_[item->text()] << str;
		}
	}
	else {
		QMessageBox::information(this, tr("Information"), tr("No file type is selected"));
	}
}

void FileTypesPage::removeFileNamePattern() {
	int row = ui.fileNameList->currentRow();
	if ( row >= 0 ) {
		QListWidgetItem* typeItem = ui.typeList->currentItem();
		QListWidgetItem* item = ui.fileNameList->takeItem(row);
		fileNamePatterns_[typeItem->text()].removeAll(item->text());
	}
}

void FileTypesPage::addFirstLinePattern() {
	if ( QListWidgetItem* item = ui.typeList->currentItem() ) {
		QString str = QInputDialog::getText(this, tr("New first line pattern"), tr("New first line pattern"));
		if ( !str.isEmpty() ) {
			ui.firstLineList->addItem(str);
			firstLinePatterns_[item->text()] << str;
		}
	}
	else {
		QMessageBox::information(this, tr("Information"), tr("No file type is selected"));
	}
}

void FileTypesPage::removeFirstLinePattern() {
	int row = ui.firstLineList->currentRow();
	if ( row >= 0 ) {
		QListWidgetItem* typeItem = ui.typeList->currentItem();
		QListWidgetItem* item = ui.firstLineList->takeItem(row);
		firstLinePatterns_[typeItem->text()].removeAll(item->text());
	}
}

void FileTypesPage::apply() {
	QStringList types = fileNamePatterns_.keys();
	FileTypeSettings::setTypeList(types);
	foreach(QString type, types) {
		QStringList list = fileNamePatterns_[type];
		FileTypeSettings::setFileNamePatterns(type, list);
		list = firstLinePatterns_[type];
		FileTypeSettings::setFirstLinePatterns(type, list);
	}
}
