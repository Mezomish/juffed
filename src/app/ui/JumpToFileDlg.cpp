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

#include "JumpToFileDlg.h"

#include "Functions.h"
#include "Log.h"

#include <QFileInfo>
#include <QKeyEvent>
#include <QListWidgetItem>

JumpToFileDlg::JumpToFileDlg(const QStringList& files, QWidget* parent) : QDialog(parent) {
	ui_.setupUi(this);
	
	foreach (QString fileName, files) {
		QListWidgetItem* item = new QListWidgetItem(QFileInfo(fileName).fileName(), false);
		item->setData(Qt::UserRole + 1, fileName);
		ui_.fileList->addItem(item);
	}
	ui_.fileList->setCurrentRow(0);
	
	connect(ui_.fileList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(onItemDoubleClicked(QListWidgetItem*)));
	connect(ui_.fileNameEd, SIGNAL(textEdited(const QString&)), SLOT(onTextEdited(const QString&)));
	connect(ui_.fileNameEd, SIGNAL(returnPressed()), SLOT(onReturnPressed()));
	
	ui_.fileNameEd->installEventFilter(this);
	ui_.fileList->installEventFilter(this);
}

QString JumpToFileDlg::fileName() const {
	QListWidgetItem* item = ui_.fileList->currentItem();
	if ( item != 0 ) {
		return item->data(Qt::UserRole + 1).toString();
	}
	else {
		return "";
	}
}

void JumpToFileDlg::onItemDoubleClicked(QListWidgetItem*) {
	accept();
}

void JumpToFileDlg::onTextEdited(const QString& text) {
	LOGGER;
	
	int count = ui_.fileList->count();
	bool hasVisible = false;
	for (int i = 0; i < count; ++i) {
		QListWidgetItem* item = ui_.fileList->item(i);
		if ( item->text().contains(text, Qt::CaseInsensitive) ) {
			item->setHidden(false);
			if ( !hasVisible ) {
				ui_.fileList->setCurrentItem(item);
				hasVisible = true;
			}
		}
		else {
			item->setHidden(true);
		}
	}
	if ( !hasVisible )
		ui_.fileList->setCurrentItem(0);
}

void JumpToFileDlg::onReturnPressed() {
	LOGGER;
}

bool JumpToFileDlg::eventFilter(QObject *obj, QEvent *ev) {
	LOGGER;
	if ( obj == ui_.fileNameEd ) {
		if ( ev->type() == QEvent::KeyPress ) {
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(ev);
			if ( keyEvent->key() == Qt::Key_Down ) {
				ui_.fileList->setFocus();
			}
		}
	}
	else if ( obj == ui_.fileList ) {
		if ( ev->type() == QEvent::KeyPress ) {
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(ev);
			if ( keyEvent->key() == Qt::Key_Up && ui_.fileList->currentRow() == 0 ) {
				ui_.fileNameEd->setFocus();
			}
		}
	}
	return QDialog::eventFilter(obj, ev);
}
