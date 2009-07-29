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

#include "Manager.h"

#include <QtCore/QFile>
#include <QtCore/QMap>
#include <QtGui/QActionGroup>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
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
#include "MainSettings.h"
#include "NullDoc.h"
#include "Parameter.h"
#include "PluginManager.h"
#include "RichDocHandler.h"
#include "SimpleDocHandler.h"
#include "SciDocHandler.h"
#include "gui/StatusLabel.h"
#include "gui/Viewer.h"
#include "gui/GUI.h"

#include "Log.h"
#include "gui/GUIManager.h"

namespace Juff {

class Manager::Interior {
public:
	Interior(Manager* m, GUI::GUI* gui) : stayAlive_(false) {
		viewer_ = new GUI::Viewer();
		gui_ = gui;
		pluginManager_ = 0;

		mainTB_ = new QToolBar("Main");
		fileMenu_ = new QMenu(QObject::tr("&File"));
		editMenu_ = new QMenu(QObject::tr("&Edit"));
		viewMenu_ = new QMenu(QObject::tr("&View"));
		formatMenu_ = new QMenu(QObject::tr("Fo&rmat"));
		charsetMenu_ = new QMenu(QObject::tr("&Charset"));
		recentFilesMenu_ = new QMenu(QObject::tr("Recent files"));
		connect(recentFilesMenu_, SIGNAL(aboutToShow()), m, SLOT(initRecentFilesMenu()));

		chActGr_ = new QActionGroup(m);

		QString recentFiles = MainSettings::recentFiles();
		if ( !recentFiles.isEmpty() ) {
			QStringList fileList = recentFiles.split(";");
			int count = fileList.count();
			for(int i = count - 1; i >= 0; --i) {
				const QString& fileName = fileList.at(i);
				addToRecentFiles(fileName);
			}
		}

		posL_ = new GUI::StatusLabel("");
		nameL_ = new GUI::StatusLabel("");
		charsetL_ = new GUI::StatusLabel("");
		linesL_ = new GUI::StatusLabel("");
		posL_->setToolTip(QObject::tr("Cursor position"));
		nameL_->setToolTip(QObject::tr("File full name"));
		charsetL_->setToolTip(QObject::tr("Current character set"));
		linesL_->setToolTip(QObject::tr("Lines count"));
		charsetL_->setMenu(charsetMenu_);
		
		gui_->addStatusWidget(posL_);
		gui_->addStatusWidget(nameL_);
		gui_->addStatusWidget(linesL_);
		gui_->addStatusWidget(charsetL_);

		posL_->hide();
		nameL_->hide();
		charsetL_->hide();
		linesL_->hide();
		
		fileNameStatusMenu_ = new QMenu();
		nameL_->setMenu(fileNameStatusMenu_);
	}
	~Interior() {
		delete recentFilesMenu_;
		delete chActGr_;
		delete fileMenu_;
		delete editMenu_;
		delete viewMenu_;
		delete charsetMenu_;
		delete mainTB_;
		delete viewer_;
		if ( pluginManager_ )
			delete pluginManager_;
		delete fileNameStatusMenu_;
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
	DocHandler* getHandlerByDoc(Document* doc) {
		if ( doc && !doc->isNull() ) {
			QString type = doc->type();
			if ( handlers_.contains(type) ) {
				return handlers_[type];
			}
		}
		return 0;
	}

	void addToRecentFiles(const QString& fileName) {
		recentFiles_.removeAll(fileName);
		recentFiles_.push_front(fileName);
		if ( recentFiles_.count() > MainSettings::recentFilesCount() )
			recentFiles_.removeLast();

		MainSettings::setRecentFiles(recentFiles_.join(";"));
	}

	void displayFileName(const QString& fileName) {
		if ( Juff::isNoname(fileName) )
			nameL_->setText(QString(" %1 ").arg(Juff::getDocTitle(fileName)));
		else
			nameL_->setText(QString(" %1 ").arg(fileName));
	}
	
	void displayCharset(const QString& charset) {
		charsetL_->setText(QString(" %1 ").arg(charset));
		charsetL_->setToolTip(QObject::tr("Current character set: %1").arg(charset));
	}
	
	QMap<QString, DocHandler*> handlers_;
	QMap<QString, QWidgetList> statusWidgets_;
	QString docOldType_;
	QMap<QString, Document*> docs1_;
	QMap<QString, Document*> docs2_;
	QString sessionName_;
	GUI::Viewer* viewer_;
	GUI::GUI* gui_;
	PluginManager* pluginManager_;
	QToolBar* mainTB_;
	QMenu* fileMenu_;
	QMenu* editMenu_;
	QMenu* viewMenu_;
	QMenu* formatMenu_;
	QMenu* charsetMenu_;
	QMap<QString, QAction*> charsetActions_;
	QActionGroup* chActGr_;
	QMenu* recentFilesMenu_;
	QStringList recentFiles_;
	GUI::StatusLabel* posL_;
	GUI::StatusLabel* nameL_;
	GUI::StatusLabel* charsetL_;
	GUI::StatusLabel* linesL_;
	bool stayAlive_;
	
	QMenu* fileNameStatusMenu_;
};

Manager::Manager(GUI::GUI* gui) : QObject(), ManagerInterface() {
	JUFFENTRY;

	registerCommands();
	mInt_ = new Interior(this, gui);

	CommandStorage::instance()->registerCommand(ID_DOC_NEXT, mInt_->viewer_, SLOT(nextDoc()));
	CommandStorage::instance()->registerCommand(ID_DOC_PREV, mInt_->viewer_, SLOT(prevDoc()));

	initMainMenu();
	initMainToolBar();


	gui->updateTitle("", "", false);
	gui->setCentralWidget(mInt_->viewer_->widget());


	//	TODO : add a proper engines list initialization
	mInt_->pluginManager_ = new PluginManager(QStringList() << /*"simple" <<*/ "rich" << "sci", this, gui);
	mInt_->pluginManager_->loadPlugins();

	//	TODO : add a proper engines loading
	//	engines
/*	SimpleDocHandler* simpleDH = new SimpleDocHandler();
	addDocHandler(simpleDH);
*/	
	RichDocHandler* richDH = new RichDocHandler();
	addDocHandler(richDH);
	
	SciDocHandler* sciDH = new SciDocHandler();
	addDocHandler(sciDH);


	//	recent files
	QAction* saveAct = CommandStorage::instance()->action(ID_FILE_SAVE);
	if ( mInt_->fileMenu_ && saveAct ) {
		mInt_->fileMenu_->insertMenu(saveAct, mInt_->recentFilesMenu_);
	}
	//	format menu
	mInt_->formatMenu_->addMenu(mInt_->charsetMenu_);
	//


	mInt_->viewer_->widget()->addAction(CommandStorage::instance()->action(ID_DOC_NEXT));
	mInt_->viewer_->widget()->addAction(CommandStorage::instance()->action(ID_DOC_PREV));

	connect(mInt_->viewer_, SIGNAL(curDocChanged(QWidget*)), SLOT(onCurDocChanged(QWidget*)));
	connect(mInt_->viewer_, SIGNAL(requestDocName(QWidget*, QString&)), SLOT(onDocNameRequested(QWidget*, QString&)));
	connect(mInt_->viewer_, SIGNAL(requestDocClose(QWidget*)), SLOT(onDocCloseRequested(QWidget*)));
	connect(mInt_->viewer_, SIGNAL(requestNewDoc()), SLOT(fileNew()));
	connect(gui, SIGNAL(settingsApplied()), SLOT(applySettings()));
	connect(gui, SIGNAL(closeRequested(bool&)), this, SLOT(onCloseEvent(bool&)));
	connect(mInt_->viewer_, SIGNAL(requestOpenDoc(const QString&)), this, SLOT(openDoc(const QString&)));
#if QT_VERSION >= 0x040500
	connect(mInt_->viewer_, SIGNAL(tabMoved(int, int)), mInt_->pluginManager_, SLOT(notifyTabMoved(int, int)));
#endif

	mInt_->fileNameStatusMenu_->addAction(tr("Copy to clipboard"), this, SLOT(copyFileName()));
	
	//////////////////
	//	GUI Controls

	//	menus from engines
	MenuList standardMenus;
	standardMenus << mInt_->fileMenu_ << mInt_->editMenu_ 
	              << mInt_->viewMenu_ << mInt_->formatMenu_;
	MenuList sciMenus = sciDH->menus();
	MenuList richMenus = richDH->menus();
	sciMenus << mInt_->pluginManager_->getMenus("sci");
	richMenus << mInt_->pluginManager_->getMenus("rich");
	mInt_->gui_->addMenus("all", standardMenus);
	mInt_->gui_->addMenus("sci", sciMenus);
	mInt_->gui_->addMenus("rich", richMenus);
	//	toolbars from engines
	mInt_->gui_->addToolBar("all", mInt_->mainTB_);
	mInt_->gui_->addToolBars("sci", sciDH->toolBars());
	mInt_->gui_->addToolBars("rich", richDH->toolBars());

	//	controls from plugins
	MenuID ids[] = { ID_MENU_FILE, ID_MENU_EDIT, ID_MENU_VIEW, ID_MENU_FORMAT, ID_MENU_TOOLS, ID_MENU_NONE };
	QString engines[] = { "sci", "rich", "all", "" };
	int ei = 0;
	while ( !engines[ei].isEmpty() ) {
		QString engine = engines[ei];

		//	toolbars
		mInt_->gui_->addToolBars(engine, mInt_->pluginManager_->getToolBars(engine));

		//	docks
		QWidgetList docks = mInt_->pluginManager_->getDocks(engine);
		mInt_->gui_->addDocks(engine, docks);

		//	items to main menus
		int i = 0;
		while ( ids[i] != ID_MENU_NONE ) {
			MenuID id = ids[i];
			QMenu* menu = 0;
			switch ( id ) {
				case ID_MENU_FILE : menu = mInt_->fileMenu_; break;
				case ID_MENU_EDIT : menu = mInt_->editMenu_; break;
				case ID_MENU_VIEW : menu = mInt_->viewMenu_; break;
				case ID_MENU_FORMAT : menu = mInt_->formatMenu_; break;
				case ID_MENU_TOOLS : menu = mInt_->gui_->toolsMenu(); break;
				default: ;
			}
			if ( menu ) {
				ActionList actions = mInt_->pluginManager_->getMainMenuActions(engine, id);
				mInt_->gui_->addActions(engine, actions);
				foreach (QAction* act, actions)
					menu->addAction(act);
			}
			++i;
		}
		++ei;
	}
	//

	mInt_->gui_->addMenus("all", mInt_->pluginManager_->getMenus("all"));

	applySettings();

	//	restore toolbars and docks positions
	mInt_->gui_->restoreState();
}

Manager::~Manager() {
	JUFFDTOR;
}

void Manager::registerCommands() {
	CommandStorage* st = CommandStorage::instance();
	st->registerCommand(ID_FILE_NEW,        this, SLOT(fileNew()));
//	st->registerCommand(ID_FILE_NEW_RICH,   this, SLOT(fileNewRich()));
	st->registerCommand(ID_FILE_OPEN,       this, SLOT(fileOpen()));
	st->registerCommand(ID_FILE_SAVE,       this, SLOT(fileSave()));
	st->registerCommand(ID_FILE_SAVE_AS,    this, SLOT(fileSaveAs()));
	st->registerCommand(ID_FILE_SAVE_ALL,   this, SLOT(fileSaveAll()));
	st->registerCommand(ID_FILE_RELOAD,     this, SLOT(fileReload()));
	st->registerCommand(ID_FILE_CLOSE,      this, SLOT(fileClose()));
	st->registerCommand(ID_FILE_CLOSE_ALL,  this, SLOT(fileCloseAll()));
	st->registerCommand(ID_FILE_PRINT,      this, SLOT(filePrint()));
	st->registerCommand(ID_EXIT,            this, SLOT(exit()));
	//
	st->registerCommand(ID_FILE_NEW_RICH,   this, SLOT(fileNewRich()));
	//
	st->registerCommand(ID_SESSION_NEW,     this, SLOT(sessionNew()));
	st->registerCommand(ID_SESSION_OPEN,    this, SLOT(sessionOpen()));
	st->registerCommand(ID_SESSION_SAVE,    this, SLOT(sessionSave()));
	st->registerCommand(ID_SESSION_SAVE_AS, this, SLOT(sessionSaveAs()));
	//
	st->registerCommand(ID_EDIT_UNDO,       this, SLOT(editUndo()));
	st->registerCommand(ID_EDIT_REDO,       this, SLOT(editRedo()));
	st->registerCommand(ID_EDIT_CUT,        this, SLOT(editCut()));
	st->registerCommand(ID_EDIT_COPY,       this, SLOT(editCopy()));
	st->registerCommand(ID_EDIT_PASTE,      this, SLOT(editPaste()));
	//
	st->registerCommand(ID_FIND,            this, SLOT(find()));
	st->registerCommand(ID_FIND_NEXT,       this, SLOT(findNext()));
	st->registerCommand(ID_FIND_PREV,       this, SLOT(findPrev()));
	st->registerCommand(ID_REPLACE,         this, SLOT(replace()));
	st->registerCommand(ID_GOTO_LINE,       this, SLOT(gotoLine()));
}

void Manager::initMainMenu() {
	CommandStorage* st = CommandStorage::instance();
	mInt_->fileMenu_->addAction(st->action(ID_FILE_NEW));
//	mInt_->fileMenu_->addAction(st->action(ID_FILE_NEW_RICH));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_OPEN));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_SAVE));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_SAVE_AS));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_SAVE_ALL));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_RELOAD));
	mInt_->fileMenu_->addAction(st->action(ID_SEPARATOR));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_CLOSE));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_CLOSE_ALL));
	mInt_->fileMenu_->addAction(st->action(ID_FILE_PRINT));
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
}

void Manager::initMainToolBar() {
	QToolBar* tb = mInt_->mainTB_;
	CommandStorage* st = CommandStorage::instance();
	QString tbStr = MainSettings::toolBar();
	if ( tbStr.isEmpty() ) {
		tb->addAction(st->action(ID_FILE_NEW));
		tb->addAction(st->action(ID_FILE_OPEN));
		tb->addAction(st->action(ID_FILE_SAVE));
		tb->addAction(st->action(ID_SEPARATOR));
		tb->addAction(st->action(ID_FILE_PRINT));
		tb->addAction(st->action(ID_SEPARATOR));
		tb->addAction(st->action(ID_EDIT_UNDO));
		tb->addAction(st->action(ID_EDIT_REDO));
		tb->addAction(st->action(ID_SEPARATOR));
		tb->addAction(st->action(ID_EDIT_CUT));
		tb->addAction(st->action(ID_EDIT_COPY));
		tb->addAction(st->action(ID_EDIT_PASTE));
	}
	else {
		QStringList items = tbStr.split('|');
		foreach (QString item, items) {
			CommandID id = ID_NONE;
			if ( item == "sep" )
				id = ID_SEPARATOR;
			else if ( item == "new" )
				id = ID_FILE_NEW;
			else if ( item == "open" )
				id = ID_FILE_OPEN;
			else if ( item == "save" )
				id = ID_FILE_SAVE;
			else if ( item == "print" )
				id = ID_FILE_PRINT;
			else if ( item == "undo" )
				id = ID_EDIT_UNDO;
			else if ( item == "redo" )
				id = ID_EDIT_REDO;
			else if ( item == "cut" )
				id = ID_EDIT_CUT;
			else if ( item == "copy" )
				id = ID_EDIT_COPY;
			else if ( item == "paste" )
				id = ID_EDIT_PASTE;
			else if ( item == "find" )
				id = ID_FIND;
			else if ( item == "replace" )
				id = ID_REPLACE;
			
			if ( id != ID_NONE )
				tb->addAction(st->action(id));
		}
	}
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

	if ( mInt_->recentFilesMenu_ == 0 )
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


bool Manager::closeWithConfirmation(Document* doc) {
	if ( !doc || doc->isNull() )
		return true;
	
	bool result = true;
	if ( doc->isModified() ) {
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

	return result;
}

bool Manager::confirmExit() {
	JUFFENTRY;

	MainSettings::setLastSessionName(mInt_->sessionName_);
	if ( MainSettings::saveSessionOnClose() ) {
		saveSess(mInt_->sessionName_);
	}
	
	return closeSess();
}

void Manager::onCloseEvent(bool& confirm) {
	confirm = confirmExit();
	if ( confirm )
		mInt_->gui_->saveState();
}

void Manager::exit() {
	JUFFENTRY;

	if ( confirmExit() ) {
		mInt_->gui_->saveState();
		qApp->quit();
	}
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
	mInt_->statusWidgets_[type] = handler->statusWidgets();
	foreach (QWidget* w, mInt_->statusWidgets_[type]) {
		mInt_->gui_->addStatusWidget(w);
		w->hide();
	}
	handler->addContextMenuActions(mInt_->pluginManager_->getContextMenuActions(type));

	QAction* act = 0;
	foreach(act, handler->menuActions(ID_MENU_EDIT)) {
		mInt_->editMenu_->addAction(act);
		mInt_->gui_->addAction(type, act);
	}
	foreach(act, handler->menuActions(ID_MENU_VIEW)) {
		mInt_->viewMenu_->addAction(act);
		mInt_->gui_->addAction(type, act);
	}
	foreach(act, handler->menuActions(ID_MENU_FORMAT)) {
		mInt_->formatMenu_->addAction(act);
		mInt_->gui_->addAction(type, act);
	}
	QMenu* toolsMenu = mInt_->gui_->toolsMenu();
	foreach(act, handler->menuActions(ID_MENU_TOOLS)) {
		toolsMenu->addAction(act);
		mInt_->gui_->addAction(type, act);
	}
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
	
	mInt_->pluginManager_->applySettings();
}

Document* Manager::curDoc() const {
	JUFFENTRY;

	QWidget* w = mInt_->viewer_->curDoc();
	if ( !w )
		JUFFDEBUG("widget is 0");
	return mInt_->getDocByView(w);
}


void Manager::openDoc(const QString& fileName) {
			
	mInt_->gui_->activateMW();
	
	//	check if this file is already opened
	if ( mInt_->docs1_.contains(fileName) ) {
		mInt_->viewer_->activateDoc(mInt_->docs1_[fileName]);
	}
	else if ( mInt_->docs2_.contains(fileName) ) {
		mInt_->viewer_->activateDoc(mInt_->docs2_[fileName]);
	}
	else {
		if ( QFileInfo(fileName).isFile() ) {
			Document* cur = curDoc();
			createDoc("sci", fileName);
			mInt_->addToRecentFiles(fileName);

			//	close the previous document if it was alone and not modified
			if ( docCount() == 2 && cur && isNoname(cur->fileName()) && !cur->isModified() )
				closeDoc(cur);
		}
		else if ( QFileInfo(fileName).isDir() ) {
			QDir dir(fileName);
			QStringList files = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
			foreach (QString dirItem, files) {
				openDoc(dir.absoluteFilePath(dirItem));
			}
		}
	}
}

void Manager::createDoc(const QString& type, const QString& fileName) {
	JUFFENTRY;

	if ( !fileName.isEmpty() && !QFileInfo(fileName).exists() ) {
		QMessageBox::information(NULL, tr("Warning"), tr("Document '%1' doesn't exist").arg(fileName));
	}
	
	DocHandler* h = mInt_->handlers_[type];
	if ( h ) {
		Document* doc = h->createDoc(fileName);
		if ( doc ) {
			QString fName = doc->fileName();
			
			connect(doc, SIGNAL(modified(bool)), SLOT(docModified(bool)));
			connect(doc, SIGNAL(fileNameChanged(const QString&)), SLOT(docFileNameChanged(const QString&)));
			connect(doc, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(onCursorPositionChanged(int, int)));
			connect(doc, SIGNAL(linesCountChanged(int)), this, SLOT(onLinesCountChanged(int)));
			connect(doc, SIGNAL(contextMenuCalled(int, int)), this, SLOT(onContextMenuCalled(int, int)));
			
			mInt_->docs1_[fName] = doc;
			mInt_->viewer_->addDoc(doc, 1);

			mInt_->pluginManager_->notifyDocCreated(fName);
			
			if ( mInt_->posL_->isHidden() ) {
				mInt_->posL_->show();
				mInt_->nameL_->show();
				mInt_->charsetL_->show();
				mInt_->linesL_->show();
			}
		}
	}
}

bool Manager::saveDoc(Document* doc, const QString& fileName, const QString& charset) {
	JUFFENTRY;

	if ( !doc || doc->isNull() )
		return false;

	QString name = fileName;
	
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
			if ( btn == owrBtn ) {
				//	Try to change permissions and save
				QFile::Permissions perm = QFile::permissions(name);
				if ( QFile::setPermissions(name, perm | QFile::WriteUser) ) {
					resolved = true;
				}
				else {
					//	Can't change permissions
					mInt_->gui_->displayError(tr("Can't change permissions: Access denied"));
					return false;
				}
			}
			else if ( btn == savBtn ) {
				//	Choose file name
				if ( fileSaveAs() )
					return true;
			}
			else {
				return false;
			}
		} while ( !resolved );
	}

	//	make a backup copy if it is necessary
	if ( MainSettings::makeBackupOnSave() ) {
		QString bkpName = name + "~";
		if ( QFile::exists(bkpName) ) {
			QFile::remove(bkpName);
		}
		QFile::copy(name, bkpName);
	}

	MainSettings::setLastSaveDir(QFileInfo(name).absolutePath());

	QString err;
	if ( !doc->save(name, charset, err) ) {
		Log::debug("Not saved...");
		mInt_->gui_->displayError(err);
		return false;
	}

	mInt_->pluginManager_->notifyDocSaved(name);

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
	mInt_->pluginManager_->notifyDocClosed(doc->fileName());
	delete doc;
	
	Document* d = curDoc();
	if ( d && !d->isNull() ) {
		d->widget()->setFocus();
	}
	else {
		mInt_->posL_->hide();
		mInt_->nameL_->hide();
		mInt_->charsetL_->hide();
		mInt_->linesL_->hide();
		
		if ( !mInt_->stayAlive_ && MainSettings::exitOnLastDocClosed() )
			exit();
	}
}

bool Manager::closeAllDocs() {
	JUFFENTRY;

	//	If this method was called we don't want to exit the app
	//	after the last document was closed (if this option was chosen
	//	in settings dialog). Set the 'stayAlive' flag and unset it afterwards.
	mInt_->stayAlive_ = true;
	while ( !curDoc()->isNull() ) {
		if ( !fileClose() ) {
			return false;
		}
	}
	mInt_->stayAlive_ = false;
	mInt_->gui_->updateTitle("", mInt_->sessionName_, false);

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

	Document* doc = curDoc();
	
	if ( MainSettings::syncOpenDialogToCurDoc() ) {
		if ( !doc->isNull() && !isNoname(doc->fileName()) )
			startDir = QFileInfo(doc->fileName()).absolutePath();
	}

	QString filters = "All files (*)";
	DocHandler* handler = mInt_->getHandlerByDoc(doc);
	if ( handler ) {
		filters = handler->fileFilters();
	}
	
	QStringList files = mInt_->gui_->getOpenFileNames(startDir, filters);
	if ( files.count() > 0 ) {
		QString fileName = "";
		foreach (fileName, files) {
			openDoc(fileName);
		}
		MainSettings::setLastOpenDir(QFileInfo(fileName).absolutePath());
	}
}

void Manager::fileRecent() {
	JUFFENTRY;

	QAction* a = qobject_cast<QAction*>(sender());
	if ( !a )
		return;
	
	QString fileName = a->text();
	if ( !fileName.isEmpty() ) {
		openDoc(fileName);
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
			if ( saveDoc(doc, doc->fileName(), doc->charset()) ) {
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
		QString filters = "All files (*)";
		DocHandler* handler = mInt_->getHandlerByDoc(doc);
		if ( handler ) {
			filters = handler->fileFilters();
		}
		
		bool asCopy = false;
		QString charset = doc->charset();
		QString fName = mInt_->gui_->getSaveFileName(doc->fileName(), filters, asCopy, charset);
		if ( !fName.isEmpty() ) {
			if ( saveDoc(doc, fName, charset) ) {
				if ( !asCopy ) {
					doc->setFileName(fName);
					doc->setCharset(charset);
					mInt_->displayCharset(charset);
					if ( QAction* chAct = mInt_->charsetActions_[doc->charset()] )
						chAct->setChecked(true);
					doc->setModified(false);
					mInt_->displayFileName(fName);
				}
				return true;
			}
		}
	}
	return false;
}

void Manager::fileSaveAll() {
	JUFFENTRY;
	
	QMap<QString, Document*>::iterator it = mInt_->docs1_.begin();
	while ( it != mInt_->docs1_.end() ) {
		Document* doc = it.value();
		if ( doc && doc->isModified() ) {
			if ( Juff::isNoname(doc->fileName()) ) {
				mInt_->viewer_->activateDoc(doc);
				fileSaveAs();
			}
			else {
				if ( saveDoc(doc, doc->fileName(), doc->charset()) ) {
					doc->setModified(false);
				}
			}
		}
		it++;
	}
}

void Manager::fileReload() {
	Document* doc = curDoc();
	
	if ( !doc->isNull() ) {
		doc->reload();
	}
}

bool Manager::fileClose() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	bool result = closeWithConfirmation(doc);
	

	if ( curDoc()->isNull() )
		mInt_->gui_->updateTitle("", mInt_->sessionName_, false);

	return result;
}

void Manager::fileCloseAll() {
	closeAllDocs();
}

void Manager::filePrint() {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() )
		doc->print();
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
		if ( doc && !doc->isNull() && !isNoname(doc->fileName()) ) {
			int scrPos = doc->curScrollPos();
			int line = doc->curLine();
			file.write((QString("%1:%2:%3\n")
				.arg(doc->fileName()).arg(scrPos).arg(line)).toLocal8Bit());
		}
	}
}

bool Manager::openSess(const QString& name) {
	JUFFENTRY;

	QString sessName = name.isEmpty() ? "_empty_session_" : name;
	
	QFile sess(AppInfo::configDirPath() + "/sessions/" + sessName);
	if ( sess.open(QIODevice::ReadOnly) ) {
		QString fileName("");
		while ( !sess.atEnd() ) {
			QString lineStr = QString::fromLocal8Bit(sess.readLine()).simplified();
#ifdef Q_OS_WIN
			fileName = lineStr.section(':', -4, -3);
#else
			fileName = lineStr.section(':', -3, -3);
#endif
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
	
	QFile sess(AppInfo::configDirPath() + "/sessions/" + sessName);
	if ( sess.open(QIODevice::WriteOnly | QIODevice::Truncate) ) {
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
	if ( !closeAllDocs() )
		return false;
	
	mInt_->sessionName_ = "";
	mInt_->gui_->updateTitle("", "", false);
	return true;
}

void Manager::restoreSession() {
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
				if ( docCount() == 0 )
					fileNew();
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
		int line1, col1, line2, col2;
		doc->getSelection(line1, col1, line2, col2);
		if ( line1 == line2 && col1 != col2 ) {
			str1 = doc->selectedText();
		}
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
			flags.replace = false;
			flags.backwards = false;
			doc->find(lastText, flags);
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
			flags.replace = false;
			flags.backwards = true;
			doc->find(lastText, flags);
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
			mInt_->displayCharset(a->text());
		}
	}
}



void Manager::docModified(bool mod) {
	JUFFENTRY;

	Document* doc = qobject_cast<Document*>(sender());

	if ( doc ) {
		mInt_->gui_->updateTitle(doc->fileName(), mInt_->sessionName_, mod);
		mInt_->viewer_->setDocModified(doc, mod);
		mInt_->pluginManager_->notifyDocModified(doc->fileName(), mod);
	}
}

void Manager::onCursorPositionChanged(int line, int col) {
	mInt_->posL_->setText(tr(" Row: %1, Col: %2 ").arg(line+1).arg(col+1));
}

void Manager::onLinesCountChanged(int lines) {
	mInt_->linesL_->setText(tr(" Lines: %1 ").arg(lines));
	mInt_->linesL_->setToolTip(QObject::tr("Lines count: %1").arg(lines));
}

void Manager::onContextMenuCalled(int line, int col) {
	mInt_->pluginManager_->notifyContextMenuCalled(line, col);
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
		mInt_->pluginManager_->notifyDocRenamed(oldName, doc->fileName());
	}
}


void Manager::onCurDocChanged(QWidget* w) {
	JUFFENTRY;

	if ( w ) {
		Document* doc = mInt_->getDocByView(w);
		if ( !doc->isNull() ) {
			QString type = doc->type();
			DocHandler* handler = mInt_->handlers_[type];
			if ( handler ) {
				handler->docActivated(doc);
			}
			else {
				Log::debug("<no type>");
			}

			//	status bar
			mInt_->displayFileName(doc->fileName());
			mInt_->displayCharset(doc->charset());
			int line = -1, col = -1;
			doc->getCursorPos(line, col);
			mInt_->posL_->setText(tr(" Row: %1, Col: %2 ").arg(line+1).arg(col+1));
			mInt_->linesL_->setText(tr(" Lines: %1 ").arg(doc->lineCount()));
			mInt_->linesL_->setToolTip(tr("Lines count: %1 ").arg(doc->lineCount()));

			if ( type != mInt_->docOldType_ ) {
				if ( mInt_->statusWidgets_.contains(mInt_->docOldType_) ) {
					foreach (QWidget* w, mInt_->statusWidgets_[mInt_->docOldType_] ) {
						w->hide();
					}
				}
				if ( mInt_->statusWidgets_.contains(type) ) {
					foreach (QWidget* w, mInt_->statusWidgets_[type] ) {
						w->show();
					}
				}
				mInt_->docOldType_ = type;
			}
				
			if ( QAction* chAct = mInt_->charsetActions_[doc->charset()] )
				chAct->setChecked(true);
			else if ( mInt_->chActGr_->checkedAction() )
				mInt_->chActGr_->checkedAction()->setChecked(false);

			doc->updateActivated();
			mInt_->gui_->updateTitle(doc->fileName(), mInt_->sessionName_, doc->isModified());
			mInt_->pluginManager_->notifyDocActivated(doc->fileName());

			mInt_->gui_->setCurType(type);
		}
		else {
			mInt_->gui_->updateTitle("", "", false);
			
			//	status bar
			mInt_->displayFileName("");
			mInt_->displayCharset("");
			mInt_->posL_->setText("  ");
			if ( mInt_->statusWidgets_.contains(mInt_->docOldType_) ) {
				foreach (QWidget* w, mInt_->statusWidgets_[mInt_->docOldType_] ) {
					w->hide();
				}
			}
			mInt_->docOldType_ = "";
		}
	}
	else {
		//	status bar
		mInt_->displayFileName("");
		mInt_->displayCharset("");
		mInt_->posL_->setText("  ");
		if ( mInt_->statusWidgets_.contains(mInt_->docOldType_) ) {
			foreach (QWidget* w, mInt_->statusWidgets_[mInt_->docOldType_] ) {
				w->hide();
			}
		}
		mInt_->docOldType_ = "";
		mInt_->gui_->setCurType("all");
	}
}

void Manager::onDocCloseRequested(QWidget* w) {
	JUFFENTRY;
	
	Document* doc = mInt_->getDocByView(w);
	closeWithConfirmation(doc);
	
	if ( curDoc()->isNull() )
		mInt_->gui_->updateTitle("", mInt_->sessionName_, false);
}

void Manager::onDocNameRequested(QWidget* w, QString& fileName) {
	JUFFENTRY;
	Document* doc = mInt_->getDocByView(w);
	fileName = doc->isNull() ? "" : doc->fileName();
}









int Manager::docCount() const {
	return mInt_->docs1_.count() + mInt_->docs2_.count();
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

void Manager::getDocText(const QString& fileName, int line, QString& text) {
	JUFFENTRY;
	
	if ( mInt_->docs1_.contains(fileName) ) {
		text = mInt_->docs1_[fileName]->text(line);
	}
	else if ( mInt_->docs2_.contains(fileName) ) {
		text = mInt_->docs2_[fileName]->text(line);
	}
	else {
		text = QString();
	}
}

void Manager::getCurrentDocText(QString& text) {
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		getDocText(doc->fileName(), text);
	}
	else {
		text = "";
	}
}

void Manager::getCurrentDocText(int line, QString& text) {
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		getDocText(doc->fileName(), line, text);
	}
	else {
		text = "";
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

void Manager::replaceSelectedText(const QString& text) {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->replaceSelectedText(text);
	}
}

void Manager::insertText(const QString& text) {
	JUFFENTRY;
	
	Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->insertText(text);
	}
}

bool Manager::closeDoc(const QString& fileName) {
	JUFFENTRY;
	
	if ( mInt_->docs1_.contains(fileName) ) {
		Document* doc = mInt_->docs1_[fileName];
		return closeWithConfirmation(doc);
	}
	else if ( mInt_->docs2_.contains(fileName) ) {
		Document* doc = mInt_->docs2_[fileName];
		return closeWithConfirmation(doc);
	}
	else {
		return true;
	}
}

void Manager::saveDoc(const QString& fileName) {
	Document* doc = 0;
	if ( mInt_->docs1_.contains(fileName) ) {
		doc = mInt_->docs1_[fileName];
	}
	else if ( mInt_->docs2_.contains(fileName) ) {
		doc = mInt_->docs2_[fileName];
	}
	
	if ( Juff::isNoname(fileName) ) {
		//	document doesn't have a file name. Call "Save as"
		
		fileSaveAs();
	}
	else {
		if ( saveDoc(doc, fileName, doc->charset()) ) {
			doc->setModified(false);
		}
	}
}


void Manager::copyFileName() {
	QApplication::clipboard()->setText(mInt_->nameL_->text().trimmed());
}


}	//	namespace Juff
