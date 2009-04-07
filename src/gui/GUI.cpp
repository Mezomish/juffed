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

#include "GUI.h"

#include "AboutDlg.h"
#include "AppInfo.h"
#include "CommandStorage.h"
#include "FindDlg.h"
#include "Functions.h"
#include "IconManager.h"
#include "License.h"
#include "Log.h"
#include "MainSettings.h"
#include "MW.h"
#include "SessionDlg.h"

#include <QtGui/QCheckBox>
#include <QtGui/QDockWidget>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>
#include <QtGui/QLayout>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>

namespace Juff {
namespace GUI {

	struct Helper {
		QString name;
		QString urlTitle;
		QString urlHref;
		QString contribution;
		Helper (const QString& nm = "", const QString& urlTtl = "", const QString& urlHrf = "", const QString& contr = "") {
			name = nm;
			urlTitle = urlTtl;
			urlHref = urlHrf;
			contribution = contr;
		}
	};
	
AboutDlg* createAboutDlg(QWidget* parent) {
	AboutDlg* dlg = new AboutDlg(parent);
	dlg->setWindowTitle(QObject::tr("About"));
	dlg->setProgramName(AppInfo::name() + " v" + AppInfo::version());
	QString text = QString("   %1   \n\n   Copyright (c) 2007-2009 Mikhail Murzin   ").arg(QObject::tr("Advanced text editor"));
	QString auth("<br>&nbsp;Mikhail Murzin a.k.a. Mezomish<br>&nbsp;&nbsp;<a href='mailto:mezomish@gmail.com'>mezomish@gmail.com</a>");
	QList<Helper> helpers;
	helpers << Helper("Eugene Pivnev", "ti.eugene@gmail.com", "mailto:ti.eugene@gmail.com", QObject::tr("Packaging"))
			<< Helper("Jarek", "ajep9691@wp.pl", "mailto:ajep9691@wp.pl", QObject::tr("Polish translation"))
			<< Helper("Michael Gangolf", "miga@migaweb.de", "mailto:miga@migaweb.de", QObject::tr("German translation"))
			<< Helper("Marc Dumoulin", "shadosan@gmail.com", "mailto:shadosan@gmail.com", QObject::tr("French translation"))
			<< Helper("Giuliano S. Nascimento", "giusoft.tecnologia@gmail.com", "mailto:giusoft.tecnologia@gmail.com", QObject::tr("Brazilian Portuguese translation"))
			<< Helper("YANG Weichun", "htyoung@163.com", "mailto:htyoung@163.com", QObject::tr("Chinese Simplified translation"))
			<< Helper("\"SoftIcon\"", "http://softicon.ru/", "http://softicon.ru/", QObject::tr("Application icon"));

	QString thanks;
	foreach(Helper helper, helpers) {
		thanks += QString("&nbsp;%1<br>").arg(helper.name);
		thanks += QString("&nbsp;&nbsp;<a href='%1'>%2</a><br>").arg(helper.urlHref).arg(helper.urlTitle);
		thanks += QString("&nbsp;&nbsp;%1<br><br>").arg(helper.contribution);
	}

	dlg->setText(text);
	dlg->setAuthors(auth);
	dlg->setThanks(thanks);
	dlg->setLicense(License::licenseText, false);
	dlg->resize(500, 300);
	dlg->setIcon(QIcon(":juffed_48.png"));
	
	return dlg;
}

GUI::GUI() : QObject() {
	mw_ = new MW();
	connect(mw_, SIGNAL(closeRequested(bool&)), this, SIGNAL(closeRequested(bool&)));
	connect(mw_, SIGNAL(docOpenRequested(const QString&)), this, SIGNAL(docOpenRequested(const QString&)));
	
	toolsMenu_ = new QMenu(tr("&Tools"));
	helpMenu_ = new QMenu(tr("Help"));
	toolBarsMenu_ = new QMenu(tr("Toolbars"));
	docksMenu_ = new QMenu(tr("Dock windows"));
	toolsMenu_->addMenu(toolBarsMenu_);
	toolsMenu_->addMenu(docksMenu_);

	CommandStorage* st = CommandStorage::instance();
	st->registerCommand(ID_SETTINGS,    this, SLOT(settings()));
	st->registerCommand(ID_ABOUT,       this, SLOT(about()));
	st->registerCommand(ID_ABOUT_QT,    this, SLOT(aboutQt()));
//	JUFFDEBUG("Filling the 'help' menu");
	helpMenu_->addAction(st->action(ID_ABOUT));
	helpMenu_->addAction(st->action(ID_ABOUT_QT));
	
//	JUFFDEBUG("Filling the 'tools' menu");
	toolsMenu_->addAction(st->action(ID_SETTINGS));
	
	mw_->menuBar()->addMenu(toolsMenu_);
	mw_->menuBar()->addMenu(helpMenu_);


	JUFFDEBUG("Creating settings dialog");
	settDlg_ = new SettingsDlg(mw_);
	settDlg_->hide();
	connect(settDlg_, SIGNAL(applied()), SLOT(applySettings()));
	
	JUFFDEBUG("Creating about dialog");
	aboutDlg_ = createAboutDlg(mw_);
}

GUI::~GUI() {
	JUFFDTOR;
	saveState();
	delete aboutDlg_;
	delete settDlg_;
	delete mw_;
}

void GUI::saveState() {
	QByteArray state = mw_->saveState();
	MainSettings::setMwState(state);
}

void GUI::restoreState() {
	//	restore the position of toolbars and docks
	mw_->restoreState(MainSettings::mwState());
}

void GUI::show() {
	mw_->show();
}

void GUI::setCentralWidget(QWidget* w) {
	mw_->setCentralWidget(w);
}

void GUI::setToolBarIconSize(int sz) {
	JUFFDEBUG(sz);
	int size = ( sz == 0 ? 16 : (sz == 1 ? 24 : 32) );

	mw_->setIconSize(QSize(size, size));
}

void GUI::setToolButtonStyle(Qt::ToolButtonStyle style) {
	mw_->setToolButtonStyle(style);
}

QStringList GUI::getOpenFileNames(const QString& dir, const QString& filters) {
	return QFileDialog::getOpenFileNames(mw_, tr("Open files"), dir, filters);
}

QString GUI::getSaveFileName(const QString& curFileName, const QString& filters, bool& asCopy) {
	QFileDialog saveDlg(mw_, tr("Save as"));
	saveDlg.setConfirmOverwrite(true);
	saveDlg.setAcceptMode(QFileDialog::AcceptSave);
#if QT_VERSION >= 0x040400
	saveDlg.setNameFilter(filters);
#else
	saveDlg.setFilter(filters);
#endif
	QLayout* layout = saveDlg.layout();
	QCheckBox* saveAsCopyChk = new QCheckBox(tr("Save as a copy"));
	saveAsCopyChk->setChecked(asCopy);
	layout->addWidget(saveAsCopyChk);

	if ( !Juff::isNoname(curFileName) ) {
		QFileInfo fi(curFileName);
		saveDlg.selectFile(fi.fileName());
		saveDlg.setDirectory(fi.path());
	}
	else {
		saveDlg.setDirectory(MainSettings::lastSaveDir());
	}
	saveDlg.setLabelText(QFileDialog::Accept, tr("Save"));
		
	QString fName("");
	if (saveDlg.exec() == QDialog::Accepted) {
		fName = saveDlg.selectedFiles()[0];
		asCopy = saveAsCopyChk->isChecked();
	}
	
	return fName;
}

QString GUI::getSaveSessionName(const QString& session) {
	JUFFDEBUG(session);
	return QInputDialog::getText(mw_, tr("Save session as"), tr("Session name"), QLineEdit::Normal, session);
}
	
QString GUI::getOpenSessionName(bool& accepted) {
	QString name = "";
	SessionDlg dlg(mw_);
	int sCount = dlg.sessionCount();
	if ( sCount > 0 ) {
		dlg.exec();
		int res = dlg.result();

		if ( res == 0 ) {
			accepted = false;
		} 
		else if ( res == 1 ) {
			//	open session
			name = dlg.curSessionName();
			accepted = true;
		}
		else {
			//	new session
			accepted = true;
		}
	}
	return name;
}

bool GUI::getFindParams(QString& str1, QString& str2, DocFindFlags& flags) {
	JUFFENTRY;
	
	FindDlg dlg(mw_);
	dlg.setReplaceMode(flags.replace);
	if ( dlg.exec() == QDialog::Accepted ) {
		str1 = dlg.text();
		flags = dlg.flags();
		if ( dlg.isReplaceMode() ) {
			str2 = dlg.replaceTo();
			flags.replace = true;
		}
		lastFlags_ = flags;

		return true;
	}

	return false;
}

QString GUI::lastFindText() const {
	return FindDlg::lastText();
}

DocFindFlags GUI::lastFlags() const {
	return lastFlags_;
}


void GUI::addMenu(const QString& type, QMenu* menu) {
	guiManager_.addMenu(type, menu);
	mw_->menuBar()->insertMenu(toolsMenu_->menuAction(), menu);
}

void GUI::addMenus(const QString& type, const Juff::MenuList menus) {
	guiManager_.addMenus(type, menus);
	foreach (QMenu* menu, menus) {
		mw_->menuBar()->insertMenu(toolsMenu_->menuAction(), menu);
	}
}

void GUI::addToolBar(const QString& type, QToolBar* tb) {
	mw_->addToolBar(tb);
	toolBarsMenu_->addAction(tb->toggleViewAction());
	if ( !tb->windowTitle().isEmpty() )
		tb->setObjectName(tb->windowTitle());
	guiManager_.addToolBar(type, tb);
}

void GUI::addToolBars(const QString& type, const Juff::ToolBarList toolBars) {
	foreach (QToolBar* tb, toolBars) {
		mw_->addToolBar(tb);
		toolBarsMenu_->addAction(tb->toggleViewAction());
		if ( !tb->windowTitle().isEmpty() )
			tb->setObjectName(tb->windowTitle());
	}
	guiManager_.addToolBars(type, toolBars);
}

void GUI::addDocks(const QString& type, const QWidgetList& list) {
	foreach (QWidget* w, list) {
		QString title = w->windowTitle();
		QDockWidget* dock = new QDockWidget(title);
		dock->setObjectName(title);
		dock->setWidget(w);
		mw_->addDockWidget(Qt::LeftDockWidgetArea, dock);
		
		docksMenu_->addAction(dock->toggleViewAction());
	}

	//	Note: we call this after creating a dock and embedding 
	//	a widget into it because we call widget's parentWidget() 
	//	when adding a dock to GUIManager's control.
	guiManager_.addDocks(type, list);
}

void GUI::addAction(const QString& type, QAction* act) {
	guiManager_.addAction(type, act);
}

void GUI::addActions(const QString& type, const Juff::ActionList& list) {
	guiManager_.addActions(type, list);
}

void GUI::setCurType(const QString& type) {
	guiManager_.setCurType(type);
}



QMenu* GUI::toolsMenu() const {
	return toolsMenu_;
}



void GUI::updateTitle(const QString& fileName, const QString& session, bool modified) {
	QString title("JuffEd");

	//	session name
	if ( !session.isEmpty() )
		title += QString(" - [%1]").arg(session);

	//	file name
	if ( !fileName.isEmpty() )
		title += QString(" - %1").arg(getDocTitle(fileName));
	
	//	modified
	if ( modified )
		title += "*";

	mw_->setWindowTitle(title);
}

void GUI::displayError(const QString& err) {
	QMessageBox::warning(mw_, tr("Warning"), err);
}

void GUI::addStatusWidget(QWidget* w) {
	mw_->statusBar()->addWidget(w);
}

void GUI::setAdditionalStatusWidgets(const QWidgetList& list) {
	foreach (QWidget* w, statusWidgets_) {
		mw_->statusBar()->removeWidget(w);
	}
	statusWidgets_ = list;
	foreach (QWidget* w, statusWidgets_) {
		mw_->statusBar()->addWidget(w);
		w->show();
	}
}

void GUI::addPluginSettingsPage(const QString& name, QWidget* w) {
	settDlg_->addPluginSettingsPage(name, w);
}

void GUI::settings() {
	settDlg_->show();
}

void GUI::applySettings() {
	JUFFENTRY;

	emit settingsApplied();
}

void GUI::about() {
	aboutDlg_->exec();
}

void GUI::aboutQt() {
	QMessageBox::aboutQt(mw_, tr("About Qt"));
}

}	//	namespace GUI
}	//	namespace Juff
