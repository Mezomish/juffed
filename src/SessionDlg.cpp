/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

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

#include "SessionDlg.h"

//	Qt headers
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtGui/QHeaderView>

//	local headers
#include "AppInfo.h"

SessionDlg::SessionDlg(QWidget* parent) : QDialog(parent), result_(0) {
	ui.setupUi(this);

	ui.sessionTree->header()->hide();

	QDir sessionDir(AppInfo::configDir() + "/sessions/");
	QStringList files = sessionDir.entryList(QDir::Files | QDir::NoSymLinks);
	foreach (QString file, files) {
		QStringList items;
		items << file;
		QTreeWidgetItem* it = new QTreeWidgetItem(items);
//		it->setHeight(20);
		ui.sessionTree->addTopLevelItem(it);
	}
	if (files.count() > 0)
		ui.sessionTree->setCurrentItem(ui.sessionTree->topLevelItem(0));

	connect(ui.openSessionBtn, SIGNAL(clicked()), SLOT(openSession()));
	connect(ui.newSessionBtn, SIGNAL(clicked()), SLOT(newSession()));
	connect(ui.sessionTree, SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(activated(QTreeWidgetItem*, int)));
}

SessionDlg::~SessionDlg() {
}

QString SessionDlg::curSessionName() const {
	QTreeWidgetItem* it = ui.sessionTree->currentItem();
	return it == 0 ? "" : it->text(0);
}

void SessionDlg::openSession() {
	result_ = 1;
	QDialog::accept();
}

void SessionDlg::newSession() {
	result_ = 2;
	QDialog::accept();
}

int SessionDlg::result() const {
	return result_;
}

void SessionDlg::activated(QTreeWidgetItem*, int) {
	openSession();
}

int SessionDlg::sessionCount() const {
	return ui.sessionTree->topLevelItemCount();
}
