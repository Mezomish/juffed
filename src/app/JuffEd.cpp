#include <QDebug>

#include "JuffEd.h"

#include <QApplication>
#include <QDockWidget>
#include <QDomDocument>
#include <QDomElement>
#include <QFileInfo>
#include <QMessageBox>
#include <QToolBar>

#include "AppInfo.h"
#include "CharsetSettings.h"
#include "Constants.h"
#include "Document.h"
#include "DocViewer.h"
#include "EditorSettings.h"
#include "JuffMW.h"
#include "Log.h"
#include "NullDoc.h"
#include "MainSettings.h"
#include "PluginManager.h"
#include "Project.h"
#include "SearchEngine.h"
#include "Settings.h"
#include "StatusLabel.h"
#include "Utils.h"
#include "ui/settings/SettingsDlg.h"

#include "SciDocEngine.h"

static const int RecentFilesCount = 10;

JuffEd::JuffEd() : Juff::PluginNotifier(), Juff::DocHandlerInt() {
	mw_ = new JuffMW();
	initActions();
	
	viewer_ = new Juff::DocViewer(this);
	mw_->setMainWidget(viewer_);
	settingsDlg_ = new SettingsDlg(mw_);
	connect(settingsDlg_, SIGNAL(applied()), SLOT(onSettingsApplied()));
	
	prj_ = NULL;
	
	connect(viewer_, SIGNAL(docActivated(Juff::Document*)), SLOT(onDocActivated(Juff::Document*)));
	connect(mw_, SIGNAL(closeRequested(bool&)), SLOT(onCloseRequested(bool&)));
	
	// UI must be initialized before engines and plugins because 
	// we need menus to be already created when loading engines and plugins.
	initUI();
	
	loadEngines();
	
	pluginMgr_ = new PluginManager(this, this);
	// buildUI() *must* go before loadPlugins() because
	// it creates structures and widgets expected by loadPlugins()
	buildUI();
	
	loadPlugins();
	
	search_ = new SearchEngine(this, mw_);
	
	if ( !loadSession("") ) {
		createProject(MainSettings::get(MainSettings::LastProject));
	}
	
	if ( viewer_->docCount(Juff::PanelAll) == 0 ) {
		openDoc("", Juff::PanelLeft);
	}
	
	onSettingsApplied();

	mw_->setGeometry(MainSettings::geometry());
	if ( MainSettings::get(MainSettings::Maximized) ) {
		mw_->showMaximized();
	}
	mw_->restoreState();
}

JuffEd::~JuffEd() {
	if ( prj_ != NULL )
		delete prj_;
	Settings::instance()->write("juff", "juffed");
}

void JuffEd::initActions() {
	CommandStorageInt* st = Utils::commandStorage();
	
	st->addAction(FILE_NEW,         tr("&New"),     this, SLOT(slotFileNew()));
	st->addAction(FILE_OPEN,        tr("&Open"),    this, SLOT(slotFileOpen()));
	st->addAction(FILE_SAVE,        tr("&Save"),    this, SLOT(slotFileSave()));
	st->addAction(FILE_SAVE_AS,     tr("Save as"), this, SLOT(slotFileSaveAs()));
	st->addAction(FILE_SAVE_ALL,    tr("Save all"), this, SLOT(slotFileSaveAll()));
	st->addAction(FILE_RELOAD,      tr("&Reload"),  this, SLOT(slotFileReload()));
	st->addAction(FILE_RENAME,      tr("Rename"),  this, SLOT(slotFileRename()));
	st->addAction(FILE_CLOSE,       tr("Close"),  this, SLOT(slotFileClose()));
	st->addAction(FILE_CLOSE_ALL,   tr("Close All"),  this, SLOT(slotFileCloseAll()));
	st->addAction(FILE_PRINT,       tr("&Print"),  this, SLOT(slotFilePrint()));
	st->addAction(FILE_EXIT,        tr("Exit"),  this, SLOT(slotFileExit()));
	
	st->addAction(EDIT_UNDO,        tr("Undo"), this, SLOT(slotEditUndo()));
	st->addAction(EDIT_REDO,        tr("Redo"), this, SLOT(slotEditRedo()));
	st->addAction(EDIT_CUT,         tr("Cut"), this, SLOT(slotEditCut()));
	st->addAction(EDIT_COPY,        tr("Copy"), this, SLOT(slotEditCopy()));
	st->addAction(EDIT_PASTE,       tr("Paste"), this, SLOT(slotEditPaste()));
	
	st->addAction(SEARCH_FIND,      tr("Find"), this, SLOT(slotFind()));
	st->addAction(SEARCH_FIND_NEXT, tr("Find next"), this, SLOT(slotFindNext()));
	st->addAction(SEARCH_FIND_PREV, tr("Find previous"), this, SLOT(slotFindPrev()));
	st->addAction(SEARCH_REPLACE,   tr("Replace"), this, SLOT(slotReplace()));
	st->addAction(SEARCH_GOTO_LINE, tr("Go to line"), this, SLOT(slotGotoLine()));
	st->addAction(SEARCH_GOTO_FILE, tr("Go to file"), this, SLOT(slotGotoFile()));
	
	st->addAction(VIEW_LINE_NUMBERS, tr("Display line numbers"), this, SLOT(slotShowLineNumbers()));
	st->addAction(VIEW_WRAP_WORDS,   tr("Wrap words"), this, SLOT(slotWrapWords()));
	st->addAction(VIEW_WHITESPACES,  tr("Show whitespaces and TABs"), this, SLOT(slotShowWhitespaces()));
	st->addAction(VIEW_LINE_ENDINGS, tr("Show ends of lines"), this, SLOT(slotShowLineEndings()));
	st->addAction(VIEW_ZOOM_IN,      tr("Zoom In"), this, SLOT(slotZoomIn()));
	st->addAction(VIEW_ZOOM_OUT,     tr("Zoom Out"), this, SLOT(slotZoomOut()));
	st->addAction(VIEW_ZOOM_100,     tr("Zoom 100%"), this, SLOT(slotZoom100()));
	st->addAction(VIEW_FULLSCREEN,   tr("Fullscreen"), this, SLOT(slotFullscreen()));
	
	st->addAction(TOOLS_SETTINGS,    tr("Settings"), this, SLOT(slotSettings()));
	st->addAction(HELP_ABOUT,        tr("About"), mw_, SLOT(slotAbout()));
	st->addAction(HELP_ABOUT_QT,     tr("About Qt"), mw_, SLOT(slotAboutQt()));
	
	st->action(VIEW_WRAP_WORDS)->setCheckable(true);
	st->action(VIEW_LINE_NUMBERS)->setCheckable(true);
	st->action(VIEW_WHITESPACES)->setCheckable(true);
	st->action(VIEW_LINE_ENDINGS)->setCheckable(true);
	st->action(VIEW_WRAP_WORDS)->setChecked(EditorSettings::get(EditorSettings::WrapWords));
	st->action(VIEW_LINE_NUMBERS)->setChecked(EditorSettings::get(EditorSettings::ShowLineNumbers));
	st->action(VIEW_WHITESPACES)->setChecked(EditorSettings::get(EditorSettings::ShowWhitespaces));
	st->action(VIEW_LINE_ENDINGS)->setChecked(EditorSettings::get(EditorSettings::ShowLineEnds));
}

void JuffEd::initUI() {
	menus_[Juff::MenuFile] = new QMenu(tr("&File"));
	menus_[Juff::MenuEdit] = new QMenu(tr("&Edit"));
	menus_[Juff::MenuView] = new QMenu(tr("&View"));
	menus_[Juff::MenuSearch] = new QMenu(tr("&Search"));
	menus_[Juff::MenuFormat] = new QMenu(tr("For&mat"));
	menus_[Juff::MenuTools] = new QMenu(tr("&Tools"));
	menus_[Juff::MenuHelp] = new QMenu(tr("&Help"));

	recentFilesMenu_ = new QMenu(JuffEd::tr("Recent files"));
	connect(recentFilesMenu_, SIGNAL(aboutToShow()), SLOT(initRecentFilesMenu()));
	QString recentFiles = MainSettings::get(MainSettings::RecentFiles);
	if ( !recentFiles.isEmpty() ) {
		QStringList fileList = recentFiles.split(";");
		int count = fileList.count();
		for(int i = count - 1; i >= 0; --i) {
			const QString& fileName = fileList.at(i);
			if ( !fileName.isEmpty() )
				addToRecentFiles(fileName);
		}
	}

	openWithCharsetGr_ = new QActionGroup(this);
	setCharsetGr_ = new QActionGroup(this);
	charsetMenu_ = new QMenu(JuffEd::tr("Charset"));
	openWithCharsetMenu_ = new QMenu(JuffEd::tr("Open with charset..."));
	setCharsetMenu_ = new QMenu(JuffEd::tr("Set charset"));
	initCharsetMenus();
	charsetMenu_->addMenu(openWithCharsetMenu_);
	charsetMenu_->addMenu(setCharsetMenu_);

	docksMenu_ = new QMenu(tr("Docks"));
	menus_[Juff::MenuTools]->addMenu(docksMenu_);

	CommandStorageInt* st = Utils::commandStorage();
	
	QToolBar* mainToolBar = new QToolBar("mainToolBar");
	mainToolBar->setObjectName("mainToolBar");
	mainToolBar->addAction(st->action(FILE_NEW));
	mainToolBar->addAction(st->action(FILE_OPEN));
	mainToolBar->addAction(st->action(FILE_SAVE));
	mainToolBar->addSeparator();
	mainToolBar->addAction(st->action(FILE_PRINT));
	mainToolBar->addSeparator();
	mainToolBar->addAction(st->action(EDIT_UNDO));
	mainToolBar->addAction(st->action(EDIT_REDO));
	mainToolBar->addSeparator();
	mainToolBar->addAction(st->action(EDIT_CUT));
	mainToolBar->addAction(st->action(EDIT_COPY));
	mainToolBar->addAction(st->action(EDIT_PASTE));
	mainToolBar->addSeparator();
	mainToolBar->addAction(st->action(SEARCH_FIND));
	
	mw_->addToolBar(mainToolBar);
}

void JuffEd::loadEngines() {
	SciDocEngine* sciEng = new SciDocEngine();
	sciEng->setDocHandler(this);
	engines_[sciEng->type()] = sciEng;
	
	QStringList titles;
	QWidgetList pages;
	if ( sciEng->getSettingsPages(titles, pages) ) {
		settingsDlg_->addPages(titles, pages);
	}
	QList<Juff::DocEngine::ColorOption> colorOpts;
	sciEng->getColorOptions(colorOpts);
	foreach (Juff::DocEngine::ColorOption option, colorOpts) {
		settingsDlg_->addColorSetting(option.title, option.section, option.key, option.defaultColor);
	}
}

void JuffEd::loadPlugins() {
	pluginMgr_->loadPlugins(settingsDlg_);
	
	// docks
	QWidgetList widgets = pluginMgr_->docks();
	foreach (QWidget* w, widgets) {
		QString title = w->windowTitle();
		QDockWidget* dock = new QDockWidget(title);
		dock->setObjectName(title);
		dock->setWidget(w);
		mw_->addDockWidget(Qt::LeftDockWidgetArea, dock);
		
		docksMenu_->addAction(dock->toggleViewAction());
	}
	
	// toolbars
	Juff::ToolBarList toolbars = pluginMgr_->toolbars();
	foreach (QToolBar* bar, toolbars) {
		if ( bar != NULL ) {
			mw_->addToolBar(bar);
		}
	}

	// load plugin actions into menues
	QMapIterator<Juff::MenuID, QMenu*> menuiter(menus_);
	while (menuiter.hasNext()) {
		menuiter.next();
		Juff::ActionList acts = pluginMgr_->actions(menuiter.key());
		if ( acts.count() > 0 ) {
			menuiter.value()->addSeparator();
			menuiter.value()->addActions(acts);
		}
	}
	
	// menus
	Juff::MenuList menus = pluginMgr_->menus();
	foreach (QMenu* menu, menus) {
		mw_->insertMenu(menus_[Juff::MenuTools], menu);
	}
}

void JuffEd::buildUI() {
	CommandStorageInt* st = Utils::commandStorage();
	
	// FILE
	QMenu* menu = menus_[Juff::MenuFile];
	menu->addAction(st->action(FILE_NEW));
	menu->addAction(st->action(FILE_OPEN));
	menu->addMenu(recentFilesMenu_);
	menu->addAction(st->action(FILE_SAVE));
	menu->addAction(st->action(FILE_SAVE_AS));
	menu->addAction(st->action(FILE_SAVE_ALL));
	menu->addAction(st->action(FILE_RENAME));
	menu->addAction(st->action(FILE_RELOAD));
	menu->addSeparator();
	menu->addAction(st->action(FILE_CLOSE));
	menu->addAction(st->action(FILE_CLOSE_ALL));
	menu->addAction(st->action(FILE_PRINT));
	menu->addSeparator();
	menu->addAction(st->action(FILE_EXIT));
	
	// EDIT
	menu = menus_[Juff::MenuEdit];
	menu->addAction(st->action(EDIT_UNDO));
	menu->addAction(st->action(EDIT_REDO));
	menu->addSeparator();
	menu->addAction(st->action(EDIT_CUT));
	menu->addAction(st->action(EDIT_COPY));
	menu->addAction(st->action(EDIT_PASTE));
	menu->addSeparator();
	
	// VIEW
	menu = menus_[Juff::MenuView];
	menu->addAction(st->action(VIEW_WRAP_WORDS));
	menu->addAction(st->action(VIEW_LINE_NUMBERS));
	menu->addAction(st->action(VIEW_WHITESPACES));
	menu->addAction(st->action(VIEW_LINE_ENDINGS));
	menu->addSeparator();
	menu->addAction(st->action(VIEW_ZOOM_IN));
	menu->addAction(st->action(VIEW_ZOOM_OUT));
	menu->addAction(st->action(VIEW_ZOOM_100));
	menu->addSeparator();
	menu->addAction(st->action(VIEW_FULLSCREEN));
	
	// SEARCH
	menu = menus_[Juff::MenuSearch];
	menu->addAction(st->action(SEARCH_FIND));
	menu->addAction(st->action(SEARCH_FIND_NEXT));
	menu->addAction(st->action(SEARCH_FIND_PREV));
	menu->addAction(st->action(SEARCH_REPLACE));
	menu->addSeparator();
	menu->addAction(st->action(SEARCH_GOTO_LINE));
	menu->addAction(st->action(SEARCH_GOTO_FILE));
	menu->addSeparator();
	
	// FORMAT
	menu = menus_[Juff::MenuFormat];
	menu->addMenu(charsetMenu_);
	
	// TOOLS
	menu = menus_[Juff::MenuTools];
	menu->addAction(st->action(TOOLS_SETTINGS));
	
	// HELP
	menu = menus_[Juff::MenuHelp];
	menu->addAction(st->action(HELP_ABOUT));
	menu->addAction(st->action(HELP_ABOUT_QT));
	
	Juff::MenuID menuIDs[] = { Juff::MenuFile, Juff::MenuEdit, Juff::MenuView, Juff::MenuSearch, 
	                   Juff::MenuFormat, Juff::MenuTools, Juff::MenuHelp };
	int i = 0;
	while ( true ) {
		Juff::MenuID id = menuIDs[i++];
		QMenu* m = menus_.value(id, NULL);
		if ( m != NULL ) {
			// add engines' items
			foreach(Juff::DocEngine* eng, engines_) {
				eng->initMenuActions(id, m);
			}
			// add menu to the menubar
			mw_->addMenu(m);
		}
		
		if ( id == Juff::MenuLAST )
			break;
	}
	
	
	// statusbar
	posL_ = new Juff::StatusLabel("");
	nameL_ = new Juff::StatusLabel("");
	charsetL_ = new Juff::StatusLabel("");
	linesL_ = new Juff::StatusLabel("");
	posL_->setMinimumWidth(130);
	posL_->setToolTip(JuffEd::tr("Cursor position"));
	nameL_->setToolTip(JuffEd::tr("File full name"));
	charsetL_->setToolTip(JuffEd::tr("Current character set"));
	linesL_->setToolTip(JuffEd::tr("Lines count"));
	charsetL_->setMenu(charsetMenu_);
	connect(linesL_, SIGNAL(clicked()), SLOT(slotGotoLine()));
	connect(posL_, SIGNAL(clicked()), SLOT(slotGotoLine()));
	
	mw_->addStatusWidget(posL_, 100);
	mw_->addStatusWidget(nameL_, -1);
	mw_->addStatusWidget(linesL_, 80);
	mw_->addStatusWidget(charsetL_, 60);
	posL_->hide();
	nameL_->hide();
	charsetL_->hide();
	linesL_->hide();
	
	// status widgets from engines
	foreach(Juff::DocEngine* eng, engines_) {
		QWidgetList statusWidgets = eng->statusWidgets();
		foreach (QWidget* w, statusWidgets) {
			mw_->addStatusWidget(w, -1);
			w->show();
		}
	}
}

void JuffEd::initCharsetMenus() {
	openWithCharsetMenu_->clear();
	setCharsetMenu_->clear();
	
	foreach (QAction* a, openWithCharsetGr_->actions())
		openWithCharsetGr_->removeAction(a);

	QStringList charsets = CharsetSettings::getCharsetsList();
	foreach (QString charset, charsets) {
		if ( CharsetSettings::charsetEnabled(charset) ) {
			QAction* openAct = openWithCharsetMenu_->addAction(charset, this, SLOT(slotOpenWithCharset()));
			openAct->setCheckable(true);
			openWithCharsetGr_->addAction(openAct);
			
			QAction* setAct = setCharsetMenu_->addAction(charset, this, SLOT(slotSetCharset()));
			setAct->setCheckable(true);
			setCharsetGr_->addAction(setAct);
			
//			mInt_->charsetActions_[charset] = action;
		}
	}
}

void JuffEd::initRecentFilesMenu() {
	recentFilesMenu_->clear();
	
	foreach (QString fileName, recentFiles_) {
		recentFilesMenu_->addAction(fileName, this, SLOT(slotFileRecent()));
	}
	
	if ( recentFiles_.count() == 0 )
		recentFilesMenu_->setEnabled(false);
	else
		recentFilesMenu_->setEnabled(true);
}

QWidget* JuffEd::mainWindow() const {
	return mw_;
}

void JuffEd::onMessageReceived(const QString& msg) {
	LOGGER;
	
	QStringList params = msg.split("\n");
	params.removeFirst();
	foreach (QString param, params) {
		if ( QFileInfo(param).exists() )
			openDoc(param);
	}
}

void JuffEd::onCloseRequested(bool& confirm) {
	LOGGER;

	QMap <QString, Juff::Document*> unsaved;
	QList<Juff::Document*> docs = viewer_->docList(Juff::PanelAll);
	foreach (Juff::Document* doc, docs)
		if ( doc->isModified() )
			unsaved[doc->fileName()] = doc;
	
	confirm = true;
	if ( !unsaved.isEmpty() ) {
		QStringList toSave;
		
		// ask for list of files needed to save
		if ( mw_->askForSave(unsaved.keys(), toSave) ) {
			foreach (QString fileName, toSave) {
				Juff::Document* doc = unsaved.value(fileName, 0);
				if ( 0 != doc ) {
					if ( Juff::Document::isNoname(fileName) ) {
						if ( !saveDocAs(doc) ) {
							// saving failed or was cancelled
							confirm = false;
						}
					}
					else {
						if ( !saveDoc(doc) ) {
							// saving failed
							confirm = false;
						}
					}
				}
			}
		}
		else {
			// closing was cancelled
			confirm = false;
		}
	}
	if ( confirm ) {
		mw_->saveState();
		saveSession(AppInfo::configDirPath() + "/session");
	}
}

void JuffEd::onSettingsApplied() {
	int sz = MainSettings::get(MainSettings::IconSize);
	int size = ( sz == 1 ? 24 : (sz == 2 ? 32 : 16) );
	Utils::iconManager()->setSize(size);
	Utils::commandStorage()->updateIcons();
	initCharsetMenus();
	
	// TODO : apply shortcuts
//	CommandStorage::instance()->updateShortcuts();

	pluginMgr_->applySettings();

	viewer_->applySettings();
	mw_->applySettings();
	
	// notify plugins
	emit settingsApplied();
}



////////////////////////////////////////////////////////////////////////////////
// Public slots from QActions (menus and toolbars)
////////////////////////////////////////////////////////////////////////////////
//
void JuffEd::slotFileNew() {
//	LOGGER;
	
	openDoc("");
}

void JuffEd::slotFileOpen() {
	// TODO : proper filters here
	QString filters = "All files (*)";
	QStringList files = mw_->getOpenFileNames(openDialogDirectory(), filters);
	
	QString fileName;
	foreach (fileName, files) {
		openDoc(fileName);
		addToRecentFiles(fileName);
	}
	
	// store the last used directory
	MainSettings::set(MainSettings::LastDir, QFileInfo(fileName).absolutePath());
}

void JuffEd::slotFileRecent() {
//	LOGGER;
	
	QAction* act = qobject_cast<QAction*>(sender());
	if ( act != 0 ) {
		openDoc(act->text());
		addToRecentFiles(act->text());
	}
}

void JuffEd::slotFileSave() {
	Juff::Document* doc = curDoc();
	if ( doc->isNull() )
		return;
	
	if ( doc->isNoname() ) {
		slotFileSaveAs();
	}
	else {
		saveDoc(doc);
	}
}

void JuffEd::slotFileSaveAs() {
	Juff::Document* doc = curDoc();
	if ( doc->isNull() )
		return;
	
	saveDocAs(doc);
}

void JuffEd::slotFileRename() {
	Juff::Document* doc = curDoc();
	if ( doc->isNull() || doc->isNoname() )
		return;
	
	QString oldName = doc->fileName();
	QFileInfo fi(oldName);
	QString newName = mw_->getRenameFileName(fi.fileName());
	if ( !newName.isEmpty() ) {
		QString fullNewName = fi.absolutePath() + "/" + newName;
		if ( QFile::rename(doc->fileName(), fullNewName) ) {
			doc->setFileName(fullNewName);
		}
	}
}

void JuffEd::slotFileSaveAll() {
	QList<Juff::Document*> docs = viewer_->docList(Juff::PanelAll);
	foreach (Juff::Document* doc, docs) {
		if ( doc->isNoname() )
			saveDocAs(doc);
		else
			saveDoc(doc);
	}
}

void JuffEd::slotFileReload() {
	Juff::Document* doc = curDoc();
	if ( doc->isNull() )
		return;
	
	doc->reload();
}

void JuffEd::slotFileClose() {
	Juff::Document* doc = curDoc();
	if ( doc->isNull() )
		return;
	
	closeDocWithConfirmation(doc);
}

void JuffEd::slotFileCloseAll() {
	closeAllDocs(Juff::PanelAll);
}

void JuffEd::slotFilePrint() {
//	LOGGER;
	Juff::Document* doc = curDoc();
	if ( doc->isNull() )
		return;
	
	doc->print();
}

void JuffEd::slotFileExit() {
//	LOGGER;

	bool exit;
	onCloseRequested(exit);
	
	if ( exit ) {
		qApp->quit();
	}
}

void JuffEd::slotEditUndo() {
//	LOGGER;
	
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->undo();
	}
}

void JuffEd::slotEditRedo() {
//	LOGGER;
	
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->redo();
	}
}

void JuffEd::slotEditCut(){
//	LOGGER;
	
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->cut();
	}
}

void JuffEd::slotEditCopy() {
//	LOGGER;
	
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->copy();
	}
}

void JuffEd::slotEditPaste() {
//	LOGGER;
	
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->paste();
	}
}

void JuffEd::slotFind() {
	search_->find();
}

void JuffEd::slotFindNext() {
	search_->findNext();
}

void JuffEd::slotFindPrev() {
	search_->findPrev();
}

void JuffEd::slotReplace() {
	search_->replace();
}

void JuffEd::slotGotoLine() {
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		int line = mw_->getGotoLineNumber(doc->lineCount());
		if ( line >= 0 )
			doc->gotoLine(line);
	}
}

void JuffEd::slotGotoFile() {
	QStringList list = viewer_->docNamesList(Juff::PanelAll);
	for ( int i = 0; i < list.count(); ++i ) {
		if ( Juff::Document::isNoname(list[i]) ) {
			list.removeAt(i);
			--i;
		}
	}
	QString fileName = mw_->getJumpToFileName(list);
	if ( !fileName.isEmpty() )
		openDoc(fileName);
}

void JuffEd::slotWrapWords(){
	bool checked = Utils::commandStorage()->action(VIEW_WRAP_WORDS)->isChecked();
	EditorSettings::set(EditorSettings::WrapWords, checked);
	
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->setWrapWords(checked);
	}
}

void JuffEd::slotShowLineNumbers(){
	bool checked = Utils::commandStorage()->action(VIEW_LINE_NUMBERS)->isChecked();
	EditorSettings::set(EditorSettings::ShowLineNumbers, checked);
	
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->setShowLineNumbers(checked);
	}
}

void JuffEd::slotShowWhitespaces(){
	bool checked = Utils::commandStorage()->action(VIEW_WHITESPACES)->isChecked();
	EditorSettings::set(EditorSettings::ShowWhitespaces, checked);
	
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->setShowWhitespaces(checked);
	}
}

void JuffEd::slotShowLineEndings(){
	bool checked = Utils::commandStorage()->action(VIEW_LINE_ENDINGS)->isChecked();
	EditorSettings::set(EditorSettings::ShowLineEnds, checked);
	
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->setShowLineEndings(checked);
	}
}

void JuffEd::slotZoomIn(){
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->zoomIn();
	}
}

void JuffEd::slotZoomOut(){
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->zoomOut();
	}
}

void JuffEd::slotZoom100(){
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() ) {
		doc->zoom100();
	}
}

void JuffEd::slotFullscreen() {
	mw_->toggleFullscreen();
}

void JuffEd::slotOpenWithCharset() {
	Juff::Document* doc = qobject_cast<Juff::Document*>(curDoc());
	QAction* action = qobject_cast<QAction*>(sender());
	if ( doc != 0 && action != 0 ) {
		QString charset = doc->charset();
		doc->setCharset(action->text());
		doc->reload();
	}
}

void JuffEd::slotSetCharset() {
	Juff::Document* doc = qobject_cast<Juff::Document*>(curDoc());
	QAction* action = qobject_cast<QAction*>(sender());
	if ( doc != 0 && action != 0 ) {
		QString charset = doc->charset();
		doc->setCharset(action->text());
	}
}

void JuffEd::slotSettings() {
	settingsDlg_->exec();
}

//
////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////
// Private slots from document events
////////////////////////////////////////////////////////////////////////////////
//
// Send notifications about these events to PluginManager and JuffMW.
// Don't send to DocViewer - it catches them by its own.
//
void JuffEd::onDocModified(bool) {
	Juff::Document* doc = qobject_cast<Juff::Document*>(sender());
	if ( doc != 0 ) {
		updateMW(doc);
		
		// notify plugins
		emit docModified(doc);
	}
}

void JuffEd::onDocFocused() {
}

void JuffEd::onDocCursorPosChanged(int, int) {
	Juff::Document* doc = qobject_cast<Juff::Document*>(sender());
	if ( doc != 0 ) {
		updateCursorPos(doc);
	}
}

void JuffEd::onDocLineCountChanged(int) {
	Juff::Document* doc = qobject_cast<Juff::Document*>(sender());
	if ( doc != 0 ) {
		updateLineCount(doc);
	}
}

void JuffEd::onDocTextChanged() {
	Juff::Document* doc = qobject_cast<Juff::Document*>(sender());
	if ( doc != 0 ) {
		// notify plugins
		emit docTextChanged(doc);
	}
}

void JuffEd::onDocSyntaxChanged(const QString& oldSyntax) {
	Juff::Document* doc = qobject_cast<Juff::Document*>(sender());
	if ( doc != 0 ) {
		// notify plugins
		emit docSyntaxChanged(doc, oldSyntax);
	}
}

void JuffEd::onDocCharsetChanged(const QString& oldCharset) {
	Juff::Document* doc = qobject_cast<Juff::Document*>(sender());
	if ( doc != 0 ) {
		// notify plugins
		emit docCharsetChanged(doc, oldCharset);
	}
}

void JuffEd::onDocRenamed(const QString& oldName) {
	LOGGER;
	
	Juff::Document* doc = qobject_cast<Juff::Document*>(sender());
	if ( doc == 0 ) {
		return;
	}
	
	emit docRenamed(doc, oldName);
	
	viewer_->updateDocTitle(doc);
	updateMW(doc);
}

// this slot is connected to a signal from DocViewer
void JuffEd::onDocActivated(Juff::Document* doc) {
	LOGGER;
	updateMW(doc);
	updateDocView(doc);
	
	search_->setCurDoc(doc);
	
	// notify plugins
	emit docActivated(doc);
}

//
////////////////////////////////////////////////////////////////////////////////







////////////////////////////////////////////////////////////////////////////////
// DocHandlerInt interface implementation
////////////////////////////////////////////////////////////////////////////////
//
Juff::Document* JuffEd::curDoc() const {
	return viewer_->currentDoc();
}

Juff::Document* JuffEd::getDoc(const QString& fileName) const {
	return viewer_->document(fileName);
}

Juff::Project* JuffEd::curPrj() const {
	return NULL;
}

void JuffEd::openDoc(const QString& fileName, Juff::PanelIndex panel) {
	// First try to activate an existing doc
	if ( !viewer_->activateDoc(fileName) ) {
		Juff::DocEngine* eng = engineForFileName(fileName);
		if ( eng == NULL )
			return;
		Juff::Document* doc = eng->createDoc(fileName);
		
		connect(doc, SIGNAL(modified(bool)), SLOT(onDocModified(bool)));
		connect(doc, SIGNAL(focused()), SLOT(onDocFocused()));
		connect(doc, SIGNAL(cursorPosChanged(int, int)), SLOT(onDocCursorPosChanged(int, int)));
		connect(doc, SIGNAL(lineCountChanged(int)), SLOT(onDocLineCountChanged(int)));
		connect(doc, SIGNAL(textChanged()), SLOT(onDocTextChanged()));
		connect(doc, SIGNAL(syntaxChanged(const QString&)), SLOT(onDocSyntaxChanged(const QString&)));
		connect(doc, SIGNAL(charsetChanged(const QString&)), SLOT(onDocCharsetChanged(const QString&)));
		connect(doc, SIGNAL(renamed(const QString&)), SLOT(onDocRenamed(const QString&)));
		connect(doc, SIGNAL(escapePressed()), mw_, SLOT(hideSearchPopup()));
		
		// determine if we need to close single unchanged noname doc
		Juff::Document* nonameDocToClose = NULL;
		if ( viewer_->docCount(panel) == 1 ) {
			nonameDocToClose = viewer_->documentAt(0, panel);
			if ( !nonameDocToClose->isNoname() || nonameDocToClose->isModified() || doc->isNoname() ) {
				nonameDocToClose = NULL;
			}
		}
		
		viewer_->addDoc(doc, panel);
		if ( prj_ != NULL )
			prj_->addFile(doc->fileName());
		
		updateDocView(doc);
		doc->setFocus();
		search_->setCurDoc(doc);
		
		// close single unchanged noname doc
		if ( nonameDocToClose != NULL ) {
			closeDocWithConfirmation(nonameDocToClose);
		}
		
		// notify plugins
		emit docOpened(doc, panel);
		
		QFileInfo fi(doc->fileName());
		// check for existance
		if ( !fi.exists() ) {
			mw_->message(QIcon(), "", tr("Document '%1' doesn't exist.").arg(doc->fileName()));
		}
		else {
			// check for read-only
			if ( !doc->isNoname() ) {
				if ( !fi.isWritable() ) {
					mw_->message(QIcon(), "", tr("Document '%1' is read-only.").arg(doc->fileName()));
				}
			}
		}
	}
}

void JuffEd::closeDoc(const QString& fileName) {
	Juff::Document* doc = viewer_->document(fileName);
	if ( !doc->isNull() )
		closeDocWithConfirmation(doc);
}

void JuffEd::closeAllDocs(Juff::PanelIndex panel) {
	Juff::DocList list = viewer_->docList(panel);
	foreach (Juff::Document* doc, list) {
		if ( !closeDocWithConfirmation(doc) )
			break;
	}
}

void JuffEd::closeAllOtherDocs(int index, Juff::PanelIndex panel) {
	Juff::DocList docs = viewer_->docList(panel);
	Juff::Document* neededDoc = viewer_->documentAt(index, panel);
	
	if ( docs.count() == 0 || neededDoc->isNull() ) {
		return;
	}
	
	foreach (Juff::Document* doc, docs) {
		if ( doc != neededDoc ) {
			closeDocWithConfirmation(doc);
		}
	}
}

void JuffEd::saveDoc(const QString&) {
}

int JuffEd::docCount(Juff::PanelIndex panel) const {
	return viewer_->docCount(panel);
}

QStringList JuffEd::docList() const {
	return QStringList();
}
//
////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////
// PRIVATE

QString JuffEd::openDialogDirectory() const {
	Juff::Document* doc = curDoc();
	if ( !doc->isNull() && !doc->isNoname() && MainSettings::get(MainSettings::SyncToCurDoc) )
		return QFileInfo(doc->fileName()).absolutePath();
	else
		return MainSettings::get(MainSettings::LastDir);
}

Juff::DocEngine* JuffEd::engineForFileName(const QString&) const {
	return engines_.value("QSci", NULL);
}

void JuffEd::reportError(const QString& error) {
	mw_->message(QIcon(), "", error);
}

bool JuffEd::saveDoc(Juff::Document* doc) {
	if ( doc->isModified() ) {
		if ( MainSettings::get(MainSettings::MakeBackupCopy) ) {
			QString bkpName = doc->fileName() + "~";
			if ( QFile::exists(bkpName) ) {
				QFile::remove(bkpName);
			}
			QFile::copy(doc->fileName(), bkpName);
		}
		
		QString error;
		if ( doc->save(error) ) {
			// saving succeeded - return true
			return true;
		}
		else {
			// saving failed or was interrupted - check for error and return false
			if ( !error.isEmpty() )
				reportError(error);
			return false;
		}
	}
	return true;
}

bool JuffEd::saveDocAs(Juff::Document* doc) {
	LOGGER;
	
	QString filters = "All files (*)";
	QString fileName = mw_->getSaveFileName(doc->isNoname() ? "" : doc->fileName(), doc->title(), filters);
	if ( !fileName.isEmpty() ) {
		// store the last used directory
		MainSettings::set(MainSettings::LastDir, QFileInfo(fileName).absolutePath());
		
		QString error;
		QString oldName = doc->fileName();
		if ( doc->saveAs(fileName, error) ) {
			// saving succeeded - return true
			return true;
		}
		else {
			// saving failed or was interrupted - check for error and return false
			if ( !error.isEmpty() )
				reportError(error);
			return false;
		}
	}
	else {
		// file selecting dialog was cancelled
		return false;
	}
}

bool JuffEd::closeDocWithConfirmation(Juff::Document* doc) {
	if ( doc == 0 || doc->isNull() )
		return true;
	
	bool decidedToClose = true;
	
	if ( doc->isModified() ) {
		switch (mw_->askForSave(doc->fileName())) {
			case QMessageBox::Save :
			{
				bool confirmed = doc->isNoname() ? saveDocAs(doc) : saveDoc(doc);
				decidedToClose = confirmed;
				break;
			}
			
			case QMessageBox::Cancel :
				decidedToClose = false;
				break;
			
			case QMessageBox::Discard :
			default:;
		}
	}

	if ( decidedToClose ) {
		if ( prj_ != NULL )
			prj_->removeFile(doc->fileName());
		
		Juff::PanelIndex panel = viewer_->panelOf(doc);
		Juff::PanelIndex anotherPanel = ( panel == Juff::PanelLeft ? Juff::PanelRight : Juff::PanelLeft );
		
		if ( viewer_->docCount(panel) == 1 && viewer_->docCount(anotherPanel) == 0
			 && !doc->isModified() && doc->isNoname() ) {
			// this is a special case when there is only one doc is
			// opened, is't Noname and unmodified. Don't close it but
			// return true in case the close was called by closeEvent.
			return true;
		}
		
		viewer_->removeDocFromList(doc);
		search_->setCurDoc(NULL);
		
		// notify plugins
		emit docClosed(doc);
		
		delete doc;
		
		// decide if we need to hide a panel that doesn't contain any docs
		if ( viewer_->docCount(panel) == 0 ) {
			// yes, we need to hide it
			// now check if there is no documents at all
			if ( viewer_->docCount(anotherPanel) == 0 ) {
				// no docs! create one
				openDoc("", panel);
				updateMW(curDoc());
			}
			else {
				// second panel contains docs
				viewer_->showPanel(anotherPanel);
				viewer_->hidePanel(panel);
				viewer_->currentDoc(anotherPanel)->setFocus();
				search_->setCurDoc(viewer_->currentDoc(anotherPanel));
			}
		}
		else {
			search_->setCurDoc(viewer_->currentDoc(panel));
		}
		
		return true;
	}
	else {
		return false;
	}
}

void JuffEd::updateMW(Juff::Document* doc) {
	QString title;
	if ( !doc->isNull() ) {
		title = QString("%1 - ").arg(doc->titleWithModification());
		if ( !projectName().isEmpty() )
			title += QString("[%1] - ").arg(projectName());
		
		posL_->show();
		nameL_->show();
		charsetL_->show();
		linesL_->show();
		
		nameL_->setText(doc->fileName());
		charsetL_->setText(doc->charset());
		
		updateLineCount(doc);
		updateCursorPos(doc);
	}
	else {
		posL_->hide();
		nameL_->hide();
		charsetL_->hide();
		linesL_->hide();
	}
	
	title += "JuffEd";
	mw_->setWindowTitle(title);
}

void JuffEd::updateDocView(Juff::Document* doc) {
	CommandStorageInt* st = Utils::commandStorage();
	doc->setWrapWords(st->action(VIEW_WRAP_WORDS)->isChecked());
	doc->setShowLineNumbers(st->action(VIEW_LINE_NUMBERS)->isChecked());
	doc->setShowWhitespaces(st->action(VIEW_WHITESPACES)->isChecked());
	doc->setShowLineEndings(st->action(VIEW_LINE_ENDINGS)->isChecked());
	
//	docManager_->setCurDocType(doc->type());
}

void JuffEd::updateLineCount(Juff::Document* doc) {
	linesL_->setText(JuffEd::tr("Lines: %1").arg(doc->lineCount()));
}

void JuffEd::updateCursorPos(Juff::Document* doc) {
	int line, col;
	doc->getCursorPos(line, col);
	posL_->setText(JuffEd::tr("Line: %1, Col: %2").arg(line+1).arg(col+1));
}

void JuffEd::addToRecentFiles(const QString& fileName) {
	recentFiles_.removeAll(fileName);
	recentFiles_.push_front(fileName);
	if ( recentFiles_.count() > RecentFilesCount )
		recentFiles_.removeLast();

	MainSettings::set(MainSettings::RecentFiles, recentFiles_.join(";"));
}










void JuffEd::createProject(const QString& fileName) {
	prj_ = new Juff::Project(fileName);
	MainSettings::set(MainSettings::LastProject, prj_->fileName());
	
//	connect(prj_, SIGNAL(fileAdded(const QString&)), SLOT(onPrjFileAdded(const QString&)));
//	connect(prj_, SIGNAL(fileRemoved(const QString&)), SLOT(onPrjFileRemoved(const QString&)));
	
	loadProject();
	
	// notify plugins
	emit projectOpened(prj_);
}

bool JuffEd::closeProject() {
	return false;
}

QString JuffEd::projectName() const {
	return ( prj_ == NULL ? "" : prj_->name() );
}

void JuffEd::loadProject() {
	if ( prj_ == NULL ) return;
	
	QStringList files = prj_->files();
	foreach (QString file, files) {
		// TODO : open files not only at left panel
		openDoc(file, Juff::PanelLeft);
	}
	
	if ( viewer_->docCount(Juff::PanelAll) == 0 ) {
		openDoc("", Juff::PanelLeft);
	}
}






////////////////////////////////////////////////////////////////////////////////
// Session

bool JuffEd::loadSession(const QString& name) {
	QString fileName;
	if ( name.isEmpty() ) {
		fileName = AppInfo::configDirPath() + "/session";
	}
	else {
		fileName = name;
	}
	
	
	QDomDocument doc("JuffEd_Session");

	QFile file(fileName);
	if ( !file.open(QIODevice::ReadOnly) ) {
		Log::warning(QString("Can't open file '%1'").arg(fileName));
		return false;
	}
	else {
		Log::debug(QString("File '%1' opened successfully").arg(fileName), true);
	}

	QString err;
	int errLine, errCol;
	if ( !doc.setContent(&file, &err, &errLine, &errCol) ) {
		Log::debug(QString("File %1: XML reading error: '%2', line %3, column %4")
				.arg(fileName).arg(err).arg(errLine).arg(errCol));
		file.close();
		return false;
	}
	else {
		Log::debug(QString("File '%1' was parsed successfully").arg(fileName), true);
	}
	file.close();

	QDomElement docElem = doc.documentElement();
	return parseSession(docElem);
}

bool JuffEd::parseSession(QDomElement& sessEl) {
	QDomNode subNode = sessEl.firstChild();

	int leftCurIndex = -1, rightCurIndex = -1;
	
	while ( !subNode.isNull() ) {
		QDomElement subEl = subNode.toElement();
		QString tagName = subEl.tagName().toLower();
		if ( tagName.compare("file") == 0 ) {
			QString fileName = subEl.attribute("path", "");
			QString panelStr = subEl.attribute("panel", "left");
			Juff::PanelIndex panel = panelStr.compare("right") == 0 ? Juff::PanelRight : Juff::PanelLeft;
			
			if ( !fileName.isEmpty() ) {
				openDoc(fileName, panel);
				Juff::Document* doc = getDoc(fileName);
				if ( !doc->isNull() ) {
					Juff::SessionParams params;
					QDomNamedNodeMap attrs = subEl.attributes();
					for ( int i = 0; i < attrs.count(); ++i ) {
						QDomAttr attr = attrs.item(i).toAttr();
						if ( !attr.isNull() ) {
							QString attrName = attr.name();
							QString attrValue = attr.value();
							params[attrName] = attrValue;
						}
					}
					doc->setSessionParams(params);
				}
			}
		}
		else if ( tagName.compare("view") == 0 ) {
			leftCurIndex = subEl.attribute("leftCurIndex", "-1").toInt();
			rightCurIndex = subEl.attribute("rightCurIndex", "-1").toInt();
		}

		subNode = subNode.nextSibling();
	}
	
	if ( leftCurIndex >= 0 ) {
		viewer_->setCurrentIndex(Juff::PanelLeft, leftCurIndex);
	}
	if ( rightCurIndex >= 0 ) {
		viewer_->setCurrentIndex(Juff::PanelRight, rightCurIndex);
	}
	
	return true;
}

void storeDocs(const Juff::DocList&, const QString&, QDomElement&, QDomDocument&);

bool JuffEd::saveSession(const QString& fileName) {
	QFile file(fileName);
	if ( file.open(QIODevice::WriteOnly) ) {
		
		QDomDocument domDoc("JuffEd_Session");
		QDomElement sessEl = domDoc.createElement("Session");
		domDoc.appendChild(sessEl);
		
		storeDocs(viewer_->docList(Juff::PanelLeft),  "left",  sessEl, domDoc);
		storeDocs(viewer_->docList(Juff::PanelRight), "right", sessEl, domDoc);
		
		QDomElement viewEl = domDoc.createElement("view");
		viewEl.setAttribute("leftCurIndex", viewer_->currentIndex(Juff::PanelLeft));
		viewEl.setAttribute("rightCurIndex", viewer_->currentIndex(Juff::PanelRight));
		sessEl.appendChild(viewEl);
		
		file.write(domDoc.toByteArray());
		file.close();
		return true;
	}
	
	// TODO : report error here
	return false;
}

void storeDocs(const Juff::DocList& docs, const QString& panelStr, QDomElement& sessEl, QDomDocument& domDoc) {
	LOGGER;
	
	foreach (Juff::Document* doc, docs) {
		if ( !doc->isNoname() ) {
			QDomElement el = domDoc.createElement("file");
			el.setAttribute("path", doc->fileName());
			el.setAttribute("panel", panelStr);
			
			Juff::SessionParams params = doc->sessionParams();
			Juff::SessionParams::const_iterator it = params.begin();
			
			while ( it != params.end() ) {
				QString key = it.key();
				QString value = it.value();
				if ( !key.isEmpty() && !value.isEmpty() ) {
					el.setAttribute(key, value);
				}
				it++;
			}
			
			sessEl.appendChild(el);
		}
	}
}
