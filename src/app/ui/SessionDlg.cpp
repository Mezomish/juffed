/*
JuffEd - An advanced text editor
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

#include "SessionDlg.h"

//	Qt headers
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtGui/QHeaderView>

#include "Log.h"

//	local headers
#ifdef Q_OS_WIN
#include "AppInfo.win.h"
#else
#include "AppInfo.h"
#endif

QStringList SessionDlg::sessionList() {
	QStringList sessions;
	QDir sessionDir(AppInfo::configDirPath() + "/sessions/");
	if (sessionDir.exists()) {
		sessions = sessionDir.entryList(QDir::Files | QDir::NoSymLinks);
	}
	return sessions;
}

SessionDlg::SessionDlg(QWidget* parent) : QDialog(parent), result_(0) {
	ui.setupUi(this);

	ui.sessionTree->header()->hide();

	QStringList sList = sessionList();
	foreach (QString session, sList) {
		if (session.compare("_empty_session_") != 0) {
			QStringList items;
			items << session;
			QTreeWidgetItem* it = new QTreeWidgetItem(items);
			ui.sessionTree->addTopLevelItem(it);
		}
	}
	if (sList.count() > 0) {
		ui.sessionTree->setCurrentItem(ui.sessionTree->topLevelItem(0));
	}
	else {
		ui.openSessionBtn->setEnabled(false);
		ui.removeSessionBtn->setEnabled(false);
	}

	connect(ui.openSessionBtn,   SIGNAL(clicked()), SLOT(openSession()));
	connect(ui.newSessionBtn,    SIGNAL(clicked()), SLOT(newSession()));
	connect(ui.removeSessionBtn, SIGNAL(clicked()), SLOT(removeSession()));
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

void SessionDlg::removeSession() {
	QTreeWidgetItem* it = ui.sessionTree->currentItem();
	if ( NULL != it ) {
		QString name = it->text(0);
		delete it;
		QString sessFile = AppInfo::configDirPath() + "/sessions/" + name;
		QFile::remove(sessFile);
		if ( ui.sessionTree->topLevelItemCount() == 0 ) {
			ui.openSessionBtn->setEnabled(false);
			ui.removeSessionBtn->setEnabled(false);
		}
	}
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
