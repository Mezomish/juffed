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

#include "Manager.h"

#include <QtCore/QMap>
#include <QtGui/QActionGroup>
#include <QtGui/QApplication>
#include <QtGui/QInputDialog>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QToolBar>

#include "AppInfo.h"
#include "CharsetsSettings.h"
#include "CommandStorage.h"
#include "DocHandler.h"
#include "Document.h"
#include "Functions.h"
#include "NullDoc.h"
#include "Parameter.h"
#include "PluginManager.h"
#include "RichDocHandler.h"
#include "MainSettings.h"
#include "SimpleDocHandler.h"
#include "SciDocHandler.h"
#include "gui/Viewer.h"
#include "gui/GUI.h"

#include "Log.h"

namespace Juff {

class Manager::Interior {
public:
	Interior(Manager* m) {
		viewer_ = new GUI::Viewer();
		gui_ = 0;
		pluginManager_ = 0;
		
		mainTB_ = new QToolBar("MainTB");
		fileMenu_ = new QMenu(QObject::tr("&File"));
		editMenu_ = new QMenu(QObject::tr("&Edit"));
		charsetMenu_ = new QMenu(QObject::tr("&Charset"));
		
		chActGr_ = new QActionGroup(m);
		
		recentFilesMenu_ = new QMenu(QObject::tr("Recent files"));
		
		QString recentFiles = MainSettings::recentFiles();
		if ( !recentFiles.isEmpty() ) {
			QStringList fileList = recentFiles.split(";");
			int count = fileList.count();
			for(int i = count - 1; i >= 0; --i) {
				const QString& fileName = fileList.at(i);
				addToRecentFiles(fileName);
			}
		}

	}
	~Interior() {
		delete recentFilesMenu_;
		delete chActGr_;
		delete fileMenu_;
		delete editMenu_;
		delete charsetMenu_;
		delete mainTB_;
		delete viewer_;
		if ( pluginManager_ )
			delete pluginManager_;
	}
	
	Document* getDocByView(QWidget* w) {
		if ( w ) {
			foreach (Document* doc, docs1_) {
				if ( doc->widget() == w )
					return doc;
			}
			foreach (Document* doc, docs2_) {
				if ( doc->widget() == w )
					return doc;
			}
		}
		return NullDoc::instance();
	}
	
	void addToRecentFiles(const QString& fileName) {
		recentFiles_.removeAll(fileName);
		recentFiles_.push_front(fileName);
		if (recentFiles_.count() > MainSettings::recentFilesCount())
			recentFiles_.removeLast();

		MainSettings::setRecentFiles(recentFiles_.join(";"));
	}
	
	QMap<QString, DocHandler*> handlers_;
	QMap<QString, Document*> docs1_;
	QMap<QString, Document*> docs2_;
	QString sessionName_;
	GUI::Viewer* viewer_;
	GUI::GUI* gui_;
	PluginManager* pluginManager_;
	QToolBar* mainTB_;
	QMenu* fileMenu_;
	QMenu* editMenu_;
	QMenu* charsetMenu_;
	QMap<QString, QAction*> charsetActions_;
	QActionGroup* chActGr_;
	QMenu* recentFilesMenu_;
	QStringList recentFiles_;
};

Manager::Manager(GUI::GUI* gui) : QObject(), ManagerInterface() {
	mInt_ = new Interior(this);
	mInt_->gui_ = gui;
	gui->updateTitle("", "", false);
	connect(gui, SIGNAL(closeRequested(bool&)), this, SLOT(onCloseEvent(bool&)));
	connect(gui, SIGNAL(docOpenRequested(const QString&)), this, SLOT(openDoc(const QString&)));
	gui->setCentralWidget(mInt_->viewer_->widget());

	
	//	TODO : add a proper engines loading
	//	engines
/*	SimpleDocHandler* simpleDH = new SimpleDocHandler();
	addDocHandler(simpleDH);
	
	RichDocHandler* richDH = new RichDocHandler();
	addDocHandler(richDH);*/
	
	SciDocHandler* sciDH = new SciDocHandler();
	addDocHandler(sciDH);
	
	//	TODO : add a proper engines list initialization
	mInt_->pluginManager_ = new PluginManager(QStringList() << /*"simple" << "rich" <<*/ "sci", this, gui);
	


	//	register commands
	CommandStorage* st = CommandStorage::instance();
	st->registerCommand(ID_FILE_NEW,		this, SLOT(fileNew()));
	st->registerCommand(ID_FILE_OPEN,		this, SLOT(fileOpen()));
	st->registerCommand(ID_FILE_SAVE, 		this, SLOT(fileSave()));
	st->registerCommand(ID_FILE_SAVE_AS,	this, SLOT(fileSaveAs()));
	st->registerCommand(ID_FILE_CLOSE,		this, SLOT(fileClose()));
	st->registerCommand(ID_FILE_CLOSE_ALL,	this, SLOT(fileCloseAll()));
	st->registerCommand(ID_EXIT,			this, SLOT(exit()));
	//
	st->registerCommand(ID_SESSION_NEW,		this, SLOT(sessionNew()));
	st->registerCommand(ID_SESSION_OPEN,	this, SLOT(sessionOpen()));
	st->registerCommand(ID_SESSION_SAVE,	this, SLOT(sessionSave()));
	st->registerCommand(ID_SESSION_SAVE_AS,	this, SLOT(sessionSaveAs()));
	//
	st->registerCommand(ID_EDIT_UNDO,		this, SLOT(editUndo()));
	st->registerCommand(ID_EDIT_REDO,		this, SLOT(editRedo()));
	st->registerCommand(ID_EDIT_CUT, 		this, SLOT(editCut()));
	st->registerCommand(ID_EDIT_COPY,		this, SLOT(editCopy()));
	st->registerCommand(ID_EDIT_PASTE,		this, SLOT(editPaste()));
	//
	st->registerCommand(ID_FIND,			this, SLOT(find()));
	st->registerCommand(ID_FIND_NEXT,		this, SLOT(findNext()));
	st->registerCommand(ID_FIND_PREV, 		this, SLOT(findPrev()));
	st->registerCommand(ID_REPLACE, 		this, SLOT(replace()));
	st->registerCommand(ID_GOTO_LINE, 		this, SLOT(gotoLine()));
	//
	st->registerCommand(ID_DOC_NEXT,		mInt_->viewer_,		SLOT(nextDoc()));
	st->registerCommand(ID_DOC_PREV,		mInt_->viewer_,		SLOT(prevDoc()));
	

	//	add commands to menu
	mInt_->fileMenu_->addAction(st->action(ID_FILE_NEW));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_OPEN));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_SAVE));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_SAVE_AS));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_CLOSE));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_CLOSE_ALL));
	mInt_->fileMenu_->addAction(st->action(ID_SEPARATOR));
	QMenu* sessMenu = new QMenu(QObject::tr("Session"));
	sessMenu->addAction(st->action(ID_SESSION_NEW));
	sessMenu->addAction(st->action(ID_SESSION_OPEN));
	sessMenu->addAction(st->action(ID_SESSION_SAVE));
	sessMenu->addAction(st->action(ID_SESSION_SAVE_AS));
	mInt_->fileMenu_->addMenu(sessMenu);
	mInt_->fileMenu_->addAction(st->action(ID_SEPARATOR));
	mInt_->fileMenu_->addAction(st->action(ID_EXIT));
	
	mInt_->editMenu_->addAction(st->action(ID_EDIT_UNDO));
	mInt_->editMenu_->addAction(st->action(ID_EDIT_REDO));
	mInt_->editMenu_->addAction(st->action(ID_SEPARATOR));
	mInt_->editMenu_->addAction(st->action(ID_EDIT_CUT));
	mInt_->editMenu_->addAction(st->action(ID_EDIT_COPY));
	mInt_->editMenu_->addAction(st->action(ID_EDIT_PASTE));
	mInt_->editMenu_->addAction(st->action(ID_SEPARATOR));
	mInt_->editMenu_->addAction(st->action(ID_FIND));
	mInt_->editMenu_->addAction(st->action(ID_FIND_NEXT));
	mInt_->editMenu_->addAction(st->action(ID_FIND_PREV));
	mInt_->editMenu_->addAction(st->action(ID_REPLACE));
	mInt_->editMenu_->addAction(st->action(ID_SEPARATOR));
	mInt_->editMenu_->addAction(st->action(ID_GOTO_LINE));


	//	add commands to toolbar
	QToolBar* tb = mInt_->mainTB_;
	tb->addAction(st->action(ID_FILE_NEW));
	tb->addAction(st->action(ID_FILE_OPEN));
	tb->addAction(st->action(ID_FILE_SAVE));
	tb->addAction(st->action(ID_SEPARATOR));
	tb->addAction(st->action(ID_EDIT_UNDO));
	tb->addAction(st->action(ID_EDIT_REDO));
	tb->addAction(st->action(ID_SEPARATOR));
	tb->addAction(st->action(ID_EDIT_CUT));
	tb->addAction(st->action(ID_EDIT_COPY));
	tb->addAction(st->action(ID_EDIT_PASTE));


	gui->setToolBars(tb);

	//	recent files
	QAction* saveAct = CommandStorage::instance()->action(ID_FILE_SAVE);
	if ( mInt_->fileMenu_ && saveAct ) {
		mInt_->fileMenu_->insertMenu(saveAct, mInt_->recentFilesMenu_);
		initRecentFilesMenu();
	}

	mInt_->viewer_->widget()->addAction(st->action(ID_DOC_NEXT));
	mInt_->viewer_->widget()->addAction(st->action(ID_DOC_PREV));

/*	
	//	plugins panels and toolbars
	jInt_->panelsMenu_ = new QMenu(tr("Dock windows"));
	jInt_->toolbarsMenu_ = new QMenu(tr("Toolbars"));
	QMenu* tMenu = jInt_->mainMenuItems_.value(tr("&Tools"), 0);
	QAction* settAct = CommandStorage::instance()->action(ID_SETTINGS);
	if (tMenu != 0 && settAct != 0) {
		tMenu->insertMenu(settAct, jInt_->toolbarsMenu_);
		tMenu->insertMenu(settAct, jInt_->panelsMenu_);
		tMenu->insertSeparator(settAct);
	}*/



	MenuList menus;
	menus << mInt_->fileMenu_ << mInt_->editMenu_;
	
	
	connect(mInt_->viewer_, SIGNAL(curDocChanged(QWidget*)), SLOT(onCurDocChanged(QWidget*)));
	connect(gui, SIGNAL(settingsApplied()), SLOT(applySettings()));
	
	connect(mInt_->pluginManager_, SIGNAL(signalReceived(Juff::DocCommand, const Juff::Param&, const Juff::Param&, const Juff::Param&, const Juff::Param&)),
						this, SLOT(pluginSignalReceived(Juff::DocCommand, const Juff::Param&, const Juff::Param&, const Juff::Param&, const Juff::Param&)));
						
	connect(mInt_->pluginManager_, SIGNAL(signalReceived(Juff::GetInfoEvent, Juff::Param&, Juff::Param&, Juff::Param&, Juff::Param&)),
						this, SLOT(pluginSignalReceived(Juff::GetInfoEvent, Juff::Param&, Juff::Param&, Juff::Param&, Juff::Param&)));
/*	QMenu* fileMenu = gui->getMenu(QObject::tr("File"));
	fileMenu->addAction("New", this, SLOT(fileNew()), QKeySequence("Ctrl+N"));
	fileMenu->addAction("New Rich", this, SLOT(fileNewRich()), QKeySequence("Ctrl+M"));
	fileMenu->addAction("Open", this, SLOT(fileOpen()), QKeySequence("Ctrl+O"));
*/
	mInt_->pluginManager_->loadPlugins();
	
	initCharsetMenu();
	menus << mInt_->charsetMenu_;

	gui->setMenus(menus);

	restoreSess();
	applySettings();
}

Manager::~Manager() {
}

bool Manager::confirmExit() {
	JUFFENTRY;

	MainSettings::setLastSessionName(mInt_->sessionName_);
	if ( MainSettings::saveSessionOnClose() )
		saveSess(mInt_->sessionName_);
	
	return closeSess();
}

void Manager::onCloseEvent(bool& confirm) {
	confirm = confirmExit();
}

void Manager::exit() {
	JUFFENTRY;

	if ( confirmExit() )
		qApp->quit();
}

void Manager::initCharsetMenu() {
	JUFFENTRY;

	mInt_->charsetMenu_->clear();
	mInt_->charsetActions_.clear();
	foreach (QAction* a, mInt_->chActGr_->actions())
		mInt_->chActGr_->removeAction(a);


	QStringList charsets = CharsetsSettings::getCharsetsList();
	foreach (QString charset, charsets) {
		if ( CharsetsSettings::charsetEnabled(charset) ) {
			QAction* action = mInt_->charsetMenu_->addAction(charset, this, SLOT(charsetSelected()));
			action->setCheckable(true);
			mInt_->charsetActions_[charset] = action;
			mInt_->chActGr_->addAction(action);
		}
	}
}

void Manager::initRecentFilesMenu() {
	JUFFENTRY;

	if (mInt_->recentFilesMenu_ == 0)
		return;

	mInt_->recentFilesMenu_->clear();
	
	foreach (QString fileName, mInt_->recentFiles_) {
		mInt_->recentFilesMenu_->addAction(fileName, this, SLOT(fileRecent()));
	}
	
	if ( mInt_->recentFiles_.count() == 0 )
		mInt_->recentFilesMenu_->setEnabled(false);
	else
		mInt_->recentFilesMenu_->setEnabled(true);
}

void Manager::addDocHandler(DocHandler* handler) {
	if ( !handler )
		return;

	QString type = handler->type();
	if ( mInt_->handlers_.contains(type) ) {
		mInt_->handlers_[type]->disconnect();
		delete mInt_->handlers_[type];
	}
	mInt_->handlers_[type] = handler;
	connect(handler, SIGNAL(getCurDoc()), SLOT(curDoc()));
}

void Manager::applySettings() {
	mInt_->viewer_->applySettings();
	mInt_->gui_->setToolBarIconSize(MainSettings::iconSize());
	mInt_->gui_->setToolButtonStyle((Qt::ToolButtonStyle)MainSettings::toolButtonStyle());
	IconManager::instance()->setCurrentIconTheme(MainSettings::iconTheme(), MainSettings::iconSize());
	CommandStorage::instance()->updateIcons();

	QMap<QString, Document*>::iterator it = mInt_->docs1_.begin();
	for (; it != mInt_->docs1_.end(); it++) {
		it.value()->applySettings();
	}
	 
	it = mInt_->docs2_.begin();
	for (; it != mInt_->docs2_.end(); it++) {
		it.value()->applySettings();
	}
	
	initCharsetMenu();
}

Document* Manager::curDoc() const {
	JUFFENTRY;
	QWidget* w = mInt_->viewer_->curView();
	if ( !w )
		JUFFDEBUG("widget is 0");
	return mInt_->getDocByView(w);
}


void Manager::openDoc(const QString& fileName) {
	//	check if this file is already opened
	if ( mInt_->docs1_.contains(fileName) ) {
		mInt_->viewer_->activateDoc(mInt_->docs1_[fileName]);
	}
	else if ( mInt_->docs2_.contains(fileName) ) {
		mInt_->viewer_->activateDoc(mInt_->docs2_[fileName]);
	}
	else {
		createDoc("sci", fileName);
		mInt_->addToRecentFiles(fileName);
		initRecentFilesMenu();
	}
}

void Manager::createDoc(const QString& type, const QString& fileName) {
	JUFFENTRY;

	DocHandler* h = mInt_->handlers_[type];
	if ( h ) {
		Document* doc = h->createDoc(fileName);
		if ( doc ) {
			QString fName = doc->fileName();
			
			connect(doc, SIGNAL(modified(bool)), SLOT(docModified(bool)));
			connect(doc, SIGNAL(fileNameChanged(const QString&)), SLOT(docFileNameChanged(const QString&)));
			
			if ( doc->type() == "sci" ) {
				mInt_->docs1_[fName] = doc;
				mInt_->viewer_->addDoc(doc, 1);
			}
			else {
				mInt_->docs2_[fName] = doc;
				mInt_->viewer_->addDoc(doc, 2);
			}
			mInt_->pluginManager_->emitInfoSignal(INFO_DOC_CREATED, Param(fName));
//			Log::debug(QString("Doc count: %1").arg(mInt_->docs_.count()));
		}
	}
}

bool Manager::saveDoc(Document* doc, const QString& fileName) {
	JUFFENTRY;

	if ( !doc || doc->isNull() )
		return false;

	QString name = fileName;
//	Log::debug(name);
	JUFFDEBUG(name);
	
	if ( QFile::exists(fileName) && !QFileInfo(fileName).isWritable() ) {
		//	file exists and is not writabe. Ask what to do.
		
		QString msg = tr("File '%1' is read-only.").arg(QFileInfo(name).fileName()) + "\n";
		msg += tr("What do you want to do?");
		QMessageBox msgBox(QMessageBox::Information, tr("Warning"), msg, QMessageBox::NoButton, mInt_->viewer_->widget());
		QPushButton* owrBtn = msgBox.addButton(tr("Overwrite"), QMessageBox::YesRole);
		QPushButton* savBtn = msgBox.addButton(tr("Save as"), QMessageBox::ApplyRole);
		msgBox.addButton(QMessageBox::Cancel);
		bool resolved = false;
		do {
			msgBox.exec();
			QAbstractButton* btn = msgBox.clickedButton();
			if (btn == owrBtn) {
				//	Try to change permissions and save
				QFile::Permissions perm = QFile::permissions(name);
				if (QFile::setPermissions(name, perm | QFile::WriteUser)) {
					resolved = true;
				}
				else {
					//	Can't change permissions
					mInt_->gui_->displayError(tr("Can't change permissions: Access denied"));
					return false;
				}
			}
			else if (btn == savBtn) {
				//	Choose file name
				if ( fileSaveAs() )
					return true;
			}
			else {
				return false;
			}
		} while (!resolved);
	}

	QString err;
	if ( !doc->save(name, err) ) {
		Log::debug("Not saved...");
		mInt_->gui_->displayError(err);
		return false;
	}

	return true;
}

void Manager::closeDoc(Document* doc) {
	JUFFENTRY;

	if ( !doc || doc->isNull() )
		return;
	
	Log::debug(doc->fileName());
	
	mInt_->docs1_.remove(doc->fileName());
	mInt_->docs2_.remove(doc->fileName());
	mInt_->viewer_->removeDoc(doc);
	mInt_->pluginManager_->emitInfoSignal(INFO_DOC_CLOSED, doc->fileName());
	delete doc;
}

bool Manager::closeAllDocs() {
	JUFFENTRY;

//	if ( MainSettings::saveSessionOnClose() )
//		saveSess(mInt_->sessionName_);

	while ( !curDoc()->isNull() ) {
		if ( !fileClose() ) {
			return false;
		}
	}
	mInt_->gui_->updateTitle("", mInt_->sessionName_, false);

/*	foreach (Document* doc, mInt_->docs1_) {
		if ( !closeDoc(doc) ) {
			return false;
		}
	}
	foreach (Document* doc, mInt_->docs2_) {
		if ( !closeDoc(doc) ) {
			return false;
		}
	}*/

	return true;
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Manager::fileNew() {
	JUFFENTRY;
	createDoc("sci", "");
}

void Manager::fileNewRich() {
	JUFFENTRY;

	createDoc("rich", "");
}

void Manager::fileOpen() {
	JUFFENTRY;
	QString startDir = MainSettings::lastOpenDir();

	if ( MainSettings::syncOpenDialogToCurDoc() ) {
		Document* doc = curDoc();
		if ( !doc->isNull() && !isNoname(doc->fileName()) )
			startDir = QFileInfo(doc->fileName()).absolutePath();
	}

	QString filters = "All files (*)";	//	TODO : modify the filters
	QStringList files = mInt_->gui_->getOpenFileNames(startDir, filters);
	QString fileName = "";
	foreach (fileName, files) {
		openDoc(fileName);
	}
	if ( !fileName.isEmpty() )
		MainSettings::setLastOpenDir(QFileInfo(fileName).absolutePath());
}

void Manager::fileRecent() {
	JUFFENTRY;

	QAction* a = qobject_cast<QAction*>(sender());
	if ( !a )
		return;
	
	QString fileName = a->text();
	if ( !fileName.isEmpty() ) {
		createDoc("sci", fileName);
		mInt_->addToRecentFiles(fileName);
		initRecentFilesMenu();
	}
}

bool Manager::fileSave() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	
	if ( !doc->isNull() ) {
		//	we have a document opened
		
		if ( !Juff::isNoname(doc->fileName()) && !doc->isModified() )
			return false;

		if ( Juff::isNoname(doc->fileName()) ) {
			//	document doesn't have a file name. Call "Save as"
			
			return fileSaveAs();
		}
		else {
			if ( saveDoc(doc, doc->fileName()) ) {
				doc->setModified(false);
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		Log::debug("Null");
		return false;
	}
}

bool Manager::fileSaveAs() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	
	if ( !doc->isNull() ) {
		QString filters = "All files (*)";	//	TODO : modify the filters
		bool asCopy = false;
		QString fName = mInt_->gui_->getSaveFileName(doc->fileName(), filters, asCopy);
		if ( !fName.isEmpty() ) {
			if ( saveDoc(doc, fName) ) {
				if ( !asCopy ) {
					doc->setFileName(fName);
					doc->setModified(false);
				}
				return true;
			}
		}
	}
	return false;
}

bool Manager::fileClose() {
	JUFFENTRY;
	
	bool result = true;
	Document* doc = curDoc();
	
	if ( !doc->isNull() ) {
		if ( doc->isModified() ) {
			Log::debug("Modified. Ask for permission");

			//	TODO : move this question to GUI
			QString str = tr("The document ") + doc->fileName();
			str += tr(" has been modified.\nDo you want to save your changes?");
			int ret = QMessageBox::warning(mInt_->viewer_->widget(), tr("Close document"),
						str, QMessageBox::Save | QMessageBox::Discard
						| QMessageBox::Cancel, QMessageBox::Save);
			switch (ret) {
			case QMessageBox::Save:
				if ( fileSave() ) {
					closeDoc(doc);
				}
				else {
					result = false;
				}
				break;

			case QMessageBox::Discard:
				closeDoc(doc);
				break;
			case QMessageBox::Cancel:
				result = false;
				break;
			}
		}
		else {
			closeDoc(doc);
		}
	}

	if ( curDoc()->isNull() )
		mInt_->gui_->updateTitle("", mInt_->sessionName_, false);
	
	return result;
}

void Manager::fileCloseAll() {
	closeAllDocs();
}


void Manager::sessionNew() {
	JUFFENTRY;
	
	if ( closeAllDocs() ) {
		mInt_->sessionName_ = "";
		fileNew();
	}
}

void Manager::sessionOpen() {
	JUFFENTRY;
	
	saveSess(mInt_->sessionName_);
	
	if ( closeSess() ) {
		bool accepted = false;
		QString sessName = mInt_->gui_->getOpenSessionName(accepted);
		if ( accepted ) {
			if ( !sessName.isEmpty() ) {
				//	open session
				if ( openSess(sessName) ) {
					mInt_->sessionName_ = sessName;
					Document* doc = curDoc();
					QString fileName = doc->isNull() ? "" : doc->fileName();
					mInt_->gui_->updateTitle(fileName, sessName, false);
				}
			}
			else {
				//	new session
				sessionNew();
			}
		}
	}
}

void Manager::sessionSave() {
	JUFFENTRY;
	
	if ( mInt_->sessionName_.isEmpty() ) {
		sessionSaveAs();
	}
	else {
		saveSess(mInt_->sessionName_);
	}
}

void Manager::sessionSaveAs() {
	JUFFENTRY;
	
	QString sessName = mInt_->gui_->getSaveSessionName(mInt_->sessionName_);
	if ( !sessName.isEmpty() ) {
		saveSess(sessName);
		mInt_->sessionName_ = sessName;
		Document* doc = curDoc();
		QString fileName = doc->isNull() ? "" : doc->fileName();
		mInt_->gui_->updateTitle(fileName, sessName, false);
	}
}

void Manager::writePanelViews(QFile& file, int panel) {
	QWidgetList views;
	
	mInt_->viewer_->getViewsList(panel, views);
	foreach (QWidget* w, views) {
		Document* doc = mInt_->getDocByView(w);
		if ( doc && !doc->isNull() ) {
			int scrPos = doc->curScrollPos();
			int line = doc->curLine();
			file.write(QString("%1:%2:%3\n")
				.arg(doc->fileName()).arg(scrPos).arg(line).toLocal8Bit());
		}
	}
}

bool Manager::openSess(const QString& name) {
	JUFFENTRY;

	QString sessName = name.isEmpty() ? "_empty_session_" : name;
	
	JUFFDEBUG(sessName);
	QFile sess(AppInfo::configDirPath() + "/sessions/" + sessName);
	if ( sess.open(QIODevice::ReadOnly) ) {
		QString fileName("");
		while ( !sess.atEnd() ) {
			QString lineStr = sess.readLine().simplified();
			fileName = lineStr.section(':', -3, -3);
			JUFFDEBUG(fileName);
			int scrPos = lineStr.section(':', -2, -2).toInt();
			int line = lineStr.section(':', -1, -1).toInt();
			if ( !fileName.isEmpty() ) {
				createDoc("sci", fileName);
				Document* doc = curDoc();
				if ( !doc->isNull() ) {
					doc->gotoLine(line);
					doc->setScrollPos(scrPos);
				}
			}
		}

		sess.close();
		return true;
	}
	return false;
}

bool Manager::saveSess(const QString& name) {
	JUFFENTRY;
	
	QString sessName = name.isEmpty() ? "_empty_session_" : name;
	
	JUFFDEBUG(sessName);
	QFile sess(AppInfo::configDirPath() + "/sessions/" + sessName);
	if ( sess.open(QIODevice::WriteOnly | QIODevice::Truncate) ) {
		JUFFDEBUG("AA");
		writePanelViews(sess, 1);
		sess.close();
		return true;
	}
	else {
		//	TODO :	Add error display here
		return false;
	}
}

bool Manager::closeSess() {
//	if ( !saveSess(mInt_->sessionName_) )
//		return false;
	
	if ( !closeAllDocs() )
		return false;
	
	mInt_->sessionName_ = "";
	mInt_->gui_->updateTitle("", "", false);
	return true;
}

void Manager::restoreSess() {
	JUFFENTRY;

	int startupVariant = MainSettings::startupVariant();
	switch ( startupVariant ) {
	case 1:
		{
			QString sessName = MainSettings::lastSessionName();
			if ( openSess(sessName) ) {
				mInt_->sessionName_ = sessName;
				Document* doc = curDoc();
				QString fileName = doc->isNull() ? "" : doc->fileName();
				mInt_->gui_->updateTitle(fileName, sessName, false);
			}
		}
		break;
	
	case 2:
		sessionNew();
		break;
		
	case 0: 
	default:
		sessionOpen();
	}
}




void Manager::editUndo() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() )
		doc->undo();
}

void Manager::editRedo() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() )
		doc->redo();
}

void Manager::editCut() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() )
		doc->cut();
}

void Manager::editCopy() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() )
		doc->copy();
}

void Manager::editPaste() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() )
		doc->paste();
}

void Manager::findImpl(bool replc) {
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		DocFindFlags flags(replc);
		QString str1, str2;
		if ( mInt_->gui_->getFindParams(str1, str2, flags) ) {
			if ( flags.replace ) {
				doc->replace(str1, str2, flags);
			}
			else {
				doc->find(str1, flags);
			}
		}
	}
}

void Manager::find() {
	JUFFENTRY;
	findImpl(false);
}
	
void Manager::replace() {
	JUFFENTRY;
	findImpl(true);
}

void Manager::findNext() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		QString lastText = mInt_->gui_->lastFindText();
		if ( lastText.isEmpty() ) {
			find();
		}
		else {
			DocFindFlags flags = mInt_->gui_->lastFlags();
			doc->find(lastText, DocFindFlags(false, flags.matchCase, false, flags.isRegExp));
		}
	}
}

void Manager::findPrev() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		QString lastText = mInt_->gui_->lastFindText();
		if ( lastText.isEmpty() ) {
			find();
		}
		else {
			DocFindFlags flags = mInt_->gui_->lastFlags();
			doc->find(lastText, DocFindFlags(false, flags.matchCase, true, flags.isRegExp));
		}
	}
}

void Manager::gotoLine() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		bool ok = false;
		int line = QInputDialog::getInteger(doc->widget(), tr("Go to line"), 
					tr("Go to line") + QString(" (1 - %1):").arg(doc->lineCount()), 
					1, 1, doc->lineCount(), 1, &ok);
		if ( ok )
			doc->gotoLine(line - 1);
	}
}

void Manager::charsetSelected() {
	JUFFENTRY;

	QAction* a = qobject_cast<QAction*>(sender());
	if ( a != 0 ) {
		Document* doc = curDoc();
		if ( doc && !doc->isNull() ) {
			doc->setCharset(a->text());
/*			if ( doc->setCharset(a->text()) ) {
//				displayCharset(doc->charset());
//				changeCurrentCharsetAction(a);
			}
			else {
//				a->setChecked(false);
			}*/
		}
		else {
//			a->setChecked(false);
		}
	}
}



void Manager::docModified(bool mod) {
	JUFFENTRY;

	Document* doc = qobject_cast<Document*>(sender());

	if ( doc ) {
		Log::debug(doc->fileName() + (mod ? "isModified()" : " not modified"));
		mInt_->gui_->updateTitle(doc->fileName(), mInt_->sessionName_, mod);
		mInt_->viewer_->setDocModified(doc, mod);
		mInt_->pluginManager_->emitInfoSignal(INFO_DOC_MODIFIED, doc->fileName(), mod);
	}
}

void Manager::docFileNameChanged(const QString& oldName) {
	JUFFENTRY;

	Document* doc = qobject_cast<Document*>(sender());
	
	if ( doc ) {
		if ( mInt_->docs1_.contains(oldName) ) {
			mInt_->docs1_.remove(oldName);
			mInt_->docs1_[doc->fileName()] = doc;
		}
		else if ( mInt_->docs2_.contains(oldName) ) {
			mInt_->docs2_.remove(oldName);
			mInt_->docs2_[doc->fileName()] = doc;
		}
		else {
			return;
		}
		mInt_->gui_->updateTitle(doc->fileName(), mInt_->sessionName_, doc->isModified());
		mInt_->viewer_->updateDocTitle(doc);
		mInt_->pluginManager_->emitInfoSignal(INFO_DOC_NAME_CHANGED, oldName, doc->fileName());
	}
}


void Manager::onCurDocChanged(QWidget* w) {
	JUFFENTRY;

	ToolBarList toolBars;
	MenuList menus;
	QWidgetList docks;

	toolBars << mInt_->mainTB_;
	menus << mInt_->fileMenu_ << mInt_->editMenu_;
	
	if ( w ) {
		Document* doc = mInt_->getDocByView(w);
		if ( !doc->isNull() ) {
			QString type = doc->type();
			DocHandler* handler = mInt_->handlers_[type];
			if ( handler ) {
//				Log::debug(type);
				handler->docActivated(doc);
				
				//	toolbars
				toolBars << handler->toolBars();
				toolBars << mInt_->pluginManager_->getToolBars(type);
				toolBars << mInt_->pluginManager_->getToolBars("all");
				
				//	menus
				menus << handler->menus();
				menus << mInt_->pluginManager_->getMenus(type);
				menus << mInt_->pluginManager_->getMenus("all");
				
				//	docks
				docks << mInt_->pluginManager_->getDocks(type);
				docks << mInt_->pluginManager_->getDocks("all");
			}
			else {
				Log::debug("<no type>");
			}

			if ( QAction* chAct = mInt_->charsetActions_[doc->charset()] )
				chAct->setChecked(true);
			else if ( mInt_->chActGr_->checkedAction() )
				mInt_->chActGr_->checkedAction()->setChecked(false);

			doc->updateActivated();
			mInt_->gui_->updateTitle(doc->fileName(), mInt_->sessionName_, doc->isModified());
			mInt_->pluginManager_->emitInfoSignal(INFO_DOC_ACTIVATED, doc->fileName());
		}
		else {
			mInt_->gui_->updateTitle("", "", false);
		}
	}

	menus << mInt_->charsetMenu_;
	
	
	mInt_->gui_->setToolBars(toolBars);
	mInt_->gui_->setMenus(menus);
	mInt_->gui_->setDocks(docks);
}










void Manager::getDocList(QStringList& list) const {
	JUFFENTRY;
	
	list.clear();
	list << mInt_->docs1_.keys();
	list << mInt_->docs2_.keys();
}

void Manager::getCurDocName(QString& fileName) const {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		fileName = doc->fileName();
	}
	else {
		fileName = "";
	}
}

void Manager::getDocText(const QString& fileName, QString& text) {
	JUFFENTRY;
	
	if ( mInt_->docs1_.contains(fileName) ) {
		text = mInt_->docs1_[fileName]->text();
	}
	else if ( mInt_->docs2_.contains(fileName) ) {
		text = mInt_->docs2_[fileName]->text();
	}
	else {
		text = QString();
	}
}

void Manager::getCursorPos(int& line, int& col) {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->getCursorPos(line, col);
	}
	else {
		line = col = -1;
	}
}

void Manager::getSelection(int& line1, int& col1, int& line2, int& col2) {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->getSelection(line1, col1, line2, col2);
	}
}

void Manager::getSelectedText(QString& text) {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		text = doc->selectedText();
	}
}

void Manager::setCursorPos(int line, int col) {
	JUFFENTRY;

	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->setCursorPos(line, col);
	}
}

void Manager::setSelection(int line1, int col1, int line2, int col2) {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->setSelection(line1, col1, line2, col2);
	}
}

void Manager::removeSelectedText() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->removeSelectedText();
	}
}

void Manager::insertText(const QString& text) {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->insertText(text);
	}
}

void Manager::activateDoc(const QString& fileName) {
	JUFFENTRY;
	
	if ( mInt_->docs1_.contains(fileName) ) {
		Document* doc = mInt_->docs1_[fileName];
		mInt_->viewer_->activateDoc(doc);
	}
	else if ( mInt_->docs2_.contains(fileName) ) {
		Document* doc = mInt_->docs2_[fileName];
		mInt_->viewer_->activateDoc(doc);
	}
}

void Manager::closeDoc(const QString& fileName) {
	JUFFENTRY;
	
	if ( mInt_->docs1_.contains(fileName) ) {
		Document* doc = mInt_->docs1_[fileName];
		closeDoc(doc);
	}
	else if ( mInt_->docs2_.contains(fileName) ) {
		Document* doc = mInt_->docs2_[fileName];
		closeDoc(doc);
	}
}






}	//	namespace Juff
