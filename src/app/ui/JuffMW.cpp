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

#include <QDebug>

#include "JuffMW.h"
#include "Log.h"

#include "AboutDlg.h"
#include "AppInfo.h"
//#include "CommandStorage.h"
#include "Document.h"
//#include "FindDlg.h"
#include "Functions.h"
#include "IconManager.h"
#include "JumpToFileDlg.h"
//#include "License.h"
#include "MainSettings.h"
#include "Popup.h"
#include "SearchPopup.h"
#include "SelectFilesDlg.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

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
	QString text = QString("   %1   <br><br>").arg(QObject::tr("Advanced text editor"));
	text += "   Copyright &copy; 2007-2010 Mikhail Murzin   <br><br>";
	text += "<a href=\"http://juffed.com/\">http://juffed.com</a><br><br>";
	text += "<a href=\"http://sourceforge.net/tracker/?group_id=205470&atid=993768\">Report a bug</a><br><br>";
	text += "<a href=\"http://sourceforge.net/tracker/?group_id=205470&atid=993771\">Request a feature</a>";
	
	QString auth("<br>&nbsp;Mikhail Murzin a.k.a. Mezomish<br>&nbsp;&nbsp;<a href='mailto:mezomish@gmail.com'>mezomish@gmail.com</a>");
	QList<Helper> helpers;
	helpers
			<< Helper("Eugene Pivnev", "ti.eugene@gmail.com", "mailto:ti.eugene@gmail.com", QObject::tr("Packaging, testing"))
			<< Helper("Alexander Sokoloff", "sokoloff.a@gmail.com", "mailto:sokoloff.a@gmail.com", QObject::tr("Testing, design ideas, feature requests, SymbolBrowser plugin"))
			<< Helper("Petr Vanek", "petr@scribus.info", "mailto:petr@scribus.info", QObject::tr("Patches, Mac OS X port, XML Formatter plugin"))
			<< Helper("David Stegbauer", "daaste@gmail.com", "mailto:daaste@gmail.com", QObject::tr("Patches"))
			<< Helper("Jarek", "ajep9691@wp.pl", "mailto:ajep9691@wp.pl", QObject::tr("Polish translation"))
			<< Helper("Michael Gangolf", "miga@migaweb.de", "mailto:miga@migaweb.de", QObject::tr("German translation"))
			<< Helper("Marc Dumoulin", "shadosan@gmail.com", "mailto:shadosan@gmail.com", QObject::tr("French translation"))
			<< Helper("Pavel Fric", "http://fripohled.blogspot.com/", "http://fripohled.blogspot.com/", QObject::tr("Czech translation"))
			<< Helper("Giuliano S. Nascimento", "giusoft.tecnologia@gmail.com", "mailto:giusoft.tecnologia@gmail.com", QObject::tr("Brazilian Portuguese translation"))
			<< Helper("YANG Weichun", "htyoung@163.com", "mailto:htyoung@163.com", QObject::tr("Chinese Simplified translation"))
			<< Helper("\"SoftIcon\"", "http://softicon.ru/", "http://softicon.ru/", QObject::tr("Application icon"))
			<< Helper("Evgeny Muravjev Studio", "http://emuravjev.ru/", "http://emuravjev.ru/", QObject::tr("Website"))
	;

	QString thanks;
	foreach(Helper helper, helpers) {
		thanks += QString("&nbsp;%1<br>").arg(helper.name);
		thanks += QString("&nbsp;&nbsp;<a href='%1'>%2</a><br>").arg(helper.urlHref).arg(helper.urlTitle);
		thanks += QString("&nbsp;&nbsp;%1<br><br>").arg(helper.contribution);
	}

	dlg->setText(text);
	dlg->setAuthors(auth);
	dlg->setThanks(thanks);
//	dlg->setLicense(License::licenseText, false);
	dlg->resize(550, 350);
	dlg->setIcon(QIcon(":juffed_48.png"));
	
	return dlg;
}

JuffMW::JuffMW() : QMainWindow() {
	setGeometry(MainSettings::geometry());
	if ( MainSettings::get(MainSettings::Maximized) ) {
		showMaximized();
	}
	setWindowIcon(QIcon(":juffed_32.png"));
	
	aboutDlg_ = createAboutDlg(this);
//	findDlg_ = new FindDlg(this, false);
	popup_ = NULL;
	searchPopup_ = new SearchPopup();
	
//	connect(searchPopup_, SIGNAL(searchRequested(const Juff::SearchParams&)), SIGNAL(searchRequested(const Juff::SearchParams&)));
	
	statusWidget_ = new QWidget();
	statusLayout_ = new QHBoxLayout(statusWidget_);
	statusLayout_->setContentsMargins(1, 1, 1, 1);
	statusBar()->addWidget(statusWidget_, 1);
	
	applySettings();
}

void JuffMW::setMainWidget(QWidget* w) {
	mainWidget_ = new QWidget();
	QVBoxLayout* vBox = new QVBoxLayout(mainWidget_);
	vBox->setContentsMargins(0, 0, 0, 0);
	vBox->addWidget(w);
	vBox->addWidget(searchPopup_);
	vBox->setStretchFactor(w, 1);
	vBox->setStretchFactor(searchPopup_, 0);
	setCentralWidget(mainWidget_);
	searchPopup_->hide();
	
	popup_ = new Popup(mainWidget_);
	resizePopup(mainWidget_->width());
	mainWidget_->installEventFilter(this);
}

void JuffMW::slotAbout() {
	aboutDlg_->exec();
}

void JuffMW::slotAboutQt() {
	QMessageBox::aboutQt(this, tr("About Qt"));
}

QString JuffMW::getOpenFileName(const QString& dir, const QString& filters) {
	return QFileDialog::getOpenFileName(this, tr("Open file"), dir, filters);
}

QStringList JuffMW::getOpenFileNames(const QString& dir, const QString& filters) {
	return QFileDialog::getOpenFileNames(this, tr("Open files"), dir, filters);
}

QString JuffMW::getSaveFileName(const QString& curFileName, const QString& filters) {
	QString fileName;
	if ( !curFileName.isEmpty() && !Juff::isNoname(curFileName) ) {
		fileName = curFileName;
	}
	else {
		fileName = MainSettings::get(MainSettings::LastDir);
	}
	return QFileDialog::getSaveFileName(this, tr("Save %1 as...").arg(Juff::docTitle(curFileName, false)), fileName, filters);
}

QString JuffMW::getSavePrjName(const QString& title) {
	// TODO :
	QString dir = "";
	return QFileDialog::getSaveFileName(this, title, dir, "XML JuffEd Project Files (*.xml)");
}

QString JuffMW::getRenameFileName(const QString& curFileName) {
	return QInputDialog::getText(this, tr("Rename file"), tr("Input new file name:"), QLineEdit::Normal, curFileName);
}

int JuffMW::getGotoLineNumber(int lineCount) {
	bool ok = false;
	int line = QInputDialog::getInteger(this, tr("Go to line"), 
			tr("Go to line") + QString(" (1 - %1):").arg(lineCount), 
			1, 1, lineCount, 1, &ok);
	if ( ok )
		return line - 1;
	else
		return -1;
}

QString JuffMW::getJumpToFileName(const QStringList& fileList) {
	JumpToFileDlg dlg(fileList, this);
	if ( dlg.exec() == QDialog::Accepted ) {
		return dlg.fileName();
	}
	else {
		return "";
	}
}

int JuffMW::askForSave(const QString& fileName) {
	QString str = tr("The document ") + fileName;
	str += tr(" has been modified.\nDo you want to save your changes?");
	return QMessageBox::warning(this, tr("Close document"),
			str, QMessageBox::Save | QMessageBox::Discard
			| QMessageBox::Cancel, QMessageBox::Save);
}

bool JuffMW::askForSave(const QStringList& filesIn, QStringList& filesOut) {
	LOGGER;
	
	filesOut.clear();
	
	SelectFilesDlg dlg(filesIn, this);
	if ( dlg.exec() == QDialog::Accepted ) {
		filesOut = dlg.checkedFiles();
		return true;
	}
	else {
		return false;
	}
}

/*void JuffMW::showFindDialog(const QString& text, bool replace) {
//	findDlg_->setText(text);
//	findDlg_->setReplaceMode(replace);
	if ( searchPopup_->isHidden() ) {
		searchPopup_->show();
	}
	searchPopup_->setFindText(text);
	searchPopup_->setFindFocus();
	
	if ( replace )
		searchPopup_->showReplace();
	else
		searchPopup_->hideReplace();
//	if ( findDlg_->exec() == QDialog::Accepted ) {
//		emit searchRequested(findDlg_->params());
//	}
	
}

void JuffMW::hideFindDialog() {
}

void JuffMW::getSearchParams(Juff::SearchParams&) {
}

bool JuffMW::searchPopupVisible() const {
	return searchPopup_->isVisible();
}

void JuffMW::closeSearchPopup() {
	searchPopup_->hide();
}*/

SearchPopup* JuffMW::searchPopup() const {
	return searchPopup_;
//	return NULL;
}



////////////////////////////////////////////////////////////////////////////////
// Information display

void JuffMW::addToolBar(QToolBar* tb) {
	QMainWindow::addToolBar(tb);
	allToolBars_ << tb;
}

void JuffMW::addMenu(QMenu* menu) {
	menuBar()->addMenu(menu);
}

void JuffMW::addStatusWidget(QWidget* w, int maxWidth) {
	if ( maxWidth > 0 )
		w->setMaximumWidth(maxWidth);
	statusLayout_->addWidget(w);
}

void JuffMW::message(const QIcon& icon, const QString& title, const QString& message, int timeout) {
	Q_UNUSED(icon);
	popup_->popup(title, message, timeout);
}



////////////////////////////////////////////////////////////////////////////////

bool JuffMW::eventFilter(QObject* obj, QEvent* e) {
	if ( obj == mainWidget_ ) {
		if ( e->type() == QEvent::Resize ) {
			QResizeEvent* rszEvent = static_cast<QResizeEvent*>(e);
			if ( popup_ != NULL ) {
				resizePopup(rszEvent->size().width());
			}
		}
	}
	return QMainWindow::eventFilter(obj, e);
}

void JuffMW::resizePopup(int parentWidth) {
	if ( parentWidth - 160 <= popup_->maximumWidth() )
		popup_->setGeometry(80, popup_->y(), parentWidth - 160, 80);
	else {
		int ppWidth = popup_->maximumWidth();
		popup_->setGeometry( (parentWidth - ppWidth) / 2, popup_->y(), ppWidth, 80);
	}
}

void JuffMW::closeEvent(QCloseEvent* e) {
	LOGGER;
	
	bool confirmed = true;
	emit closeRequested(confirmed);
	if ( confirmed ) {
		e->accept();
	}
	else {
		e->ignore();
	}
}

void JuffMW::resizeEvent(QResizeEvent* e) {
	LOGGER;
	
	QMainWindow::resizeEvent(e);
	
	if ( !isMaximized() ) {
		MainSettings::setGeometry(geometry());
	}
}

void JuffMW::changeEvent(QEvent* e) {
	if ( e->type() == QEvent::WindowStateChange ) {
		MainSettings::set(MainSettings::Maximized, isMaximized());
	}
	QMainWindow::changeEvent(e);
}

void JuffMW::moveEvent(QMoveEvent* e) {
	MainSettings::setGeometry(geometry());
	QMainWindow::moveEvent(e);
}

void JuffMW::keyPressEvent(QKeyEvent* e) {
	if ( searchPopup_->isVisible() ) {
		if ( e->key() == Qt::Key_Escape ) {
			if ( searchPopup_->isVisible() ) {
				searchPopup_->dismiss();
			}
		}
	}
	QMainWindow::keyPressEvent(e);
}


bool JuffMW::isFullScreen() const {
	return windowState() & Qt::WindowFullScreen;
}

void JuffMW::toggleFullscreen() {
	setWindowState(windowState() ^ Qt::WindowFullScreen);
	
	if ( isFullScreen() ) {
//		if ( MainSettings::get(MainSettings::FSHideMenubar) )
//			menuBar()->hide();
		if ( MainSettings::get(MainSettings::FSHideStatusbar) )
			statusBar()->hide();
		if ( MainSettings::get(MainSettings::FSHideToolbar) ) {
			hiddenToolBars_.clear();
			foreach (QToolBar* tb, allToolBars_) {
				if ( tb->isVisible() ) {
					tb->hide();
					hiddenToolBars_ << tb;
				}
			}
		}
		MainSettings::set(MainSettings::FullScreen, true);
	}
	else {
//		menuBar()->show();
		statusBar()->show();
		
		foreach (QToolBar* tb, hiddenToolBars_) {
			tb->show();
		}
		MainSettings::set(MainSettings::FullScreen, false);
	}
}


void JuffMW::applySettings() {
	int size = IconManager::instance()->size();
	
	setToolButtonStyle((Qt::ToolButtonStyle)MainSettings::get(MainSettings::ToolButtonStyle));
	setIconSize(QSize(size, size));
	foreach (QToolBar* tb, allToolBars_)
		tb->setIconSize(QSize(size, size));
}

void JuffMW::saveState() {
	MainSettings::setMwState(QMainWindow::saveState());
//	guiManager_.saveLastStates();
}

void JuffMW::restoreState() {
	//	restore the position of toolbars and docks
	QMainWindow::restoreState(MainSettings::mwState());
//	guiManager_.loadLastStates();
}
