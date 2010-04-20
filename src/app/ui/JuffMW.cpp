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
#include "CommandStorage.h"
#include "Document.h"
#include "FindDlg.h"
#include "Functions.h"
#include "JumpToFileDlg.h"
#include "License.h"
#include "MainSettings.h"
#include "MessageWidget.h"
#include "SelectFilesDlg.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>

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
	QString text = QString("   %1   <br><br>   Copyright &copy; 2007-2009 Mikhail Murzin   <br><br><a href=\"http://sourceforge.net/projects/juffed/\">http://sourceforge.net/projects/juffed/</a>").arg(QObject::tr("Advanced text editor"));
	QString auth("<br>&nbsp;Mikhail Murzin a.k.a. Mezomish<br>&nbsp;&nbsp;<a href='mailto:mezomish@gmail.com'>mezomish@gmail.com</a>");
	QList<Helper> helpers;
	helpers << Helper("Eugene Pivnev", "ti.eugene@gmail.com", "mailto:ti.eugene@gmail.com", QObject::tr("Packaging"))
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
	dlg->setLicense(License::licenseText, false);
	dlg->resize(550, 350);
	dlg->setIcon(QIcon(":juffed_48.png"));
	
	return dlg;
}

JuffMW::JuffMW() : QMainWindow() {
	setGeometry(MainSettings::geometry());
	setWindowIcon(QIcon(":juffed_32.png"));
	
	mainWidget_ = new QWidget();
	vBox_ = new QVBoxLayout();
	vBox_->setContentsMargins(0, 0, 0, 0);
	mainWidget_->setLayout(vBox_);
	setCentralWidget(mainWidget_);
	
	aboutDlg_ = createAboutDlg(this);
	findDlg_ = new FindDlg(this, false);
	
	statusWidget_ = new QWidget();
	statusLayout_ = new QHBoxLayout(statusWidget_);
	statusLayout_->setContentsMargins(1, 1, 1, 1);
	statusBar()->addWidget(statusWidget_, 1);
}

void JuffMW::setViewer(QWidget* w) {
	vBox_->addWidget(w);
}

void JuffMW::about() {
	aboutDlg_->exec();
}

void JuffMW::aboutQt() {
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

void JuffMW::showFindDialog(const QString& text, bool replace) {
	findDlg_->setText(text);
	findDlg_->setReplaceMode(replace);
	if ( findDlg_->exec() == QDialog::Accepted ) {
		emit searchRequested(findDlg_->params());
	}
}

void JuffMW::hideFindDialog() {
}

void JuffMW::getSearchParams(Juff::SearchParams&) {
}





////////////////////////////////////////////////////////////////////////////////
// Information display

void JuffMW::addToolBar(QToolBar* tb) {
	LOGGER;
	
	QMainWindow::addToolBar(tb);
	allToolBars_ << tb;
}

void JuffMW::addStatusWidget(QWidget* w, int maxWidth) {
	if ( maxWidth > 0 )
		w->setMaximumWidth(maxWidth);
	statusLayout_->addWidget(w);
}

void JuffMW::message(const QIcon& icon, const QString& title, const QString& message, int timeout) {
	MessageWidget* msg = new MessageWidget(QIcon(), title, message, timeout, this);
	vBox_->insertWidget(0, msg);
	vBox_->setStretchFactor(msg, 0);
//	vBox_->addWidget(msg);
}



////////////////////////////////////////////////////////////////////////////////

void JuffMW::closeEvent(QCloseEvent* e) {
	LOGGER;
	
	bool confirmed = true;
	emit closeRequested(confirmed);
	if ( confirmed ) {
//		MainSettings::setWindowRect(geometry());
		e->accept();
	}
	else {
		e->ignore();
	}
}

void JuffMW::resizeEvent(QResizeEvent*) {
	LOGGER;
	MainSettings::setGeometry(geometry());
}

void JuffMW::moveEvent(QMoveEvent*) {
	LOGGER;
	MainSettings::setGeometry(geometry());
}

bool JuffMW::isFullScreen() const {
	return windowState() & Qt::WindowFullScreen;
}

void JuffMW::toggleFullscreen() {
	LOGGER;
	
	setWindowState(windowState() ^ Qt::WindowFullScreen);
	
	bool isFSNow = windowState() & Qt::WindowFullScreen;
	if ( isFSNow ) {
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
	}
	else {
//		menuBar()->show();
		statusBar()->show();
		
		foreach (QToolBar* tb, hiddenToolBars_) {
			tb->show();
		}
	}
}


void JuffMW::applySettings() {
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
