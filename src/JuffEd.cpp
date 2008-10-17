/*
JuffEd - A simple text editor
Copyright 2007-2008 Murzin Mikhail

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

#include "JuffEd.h"

//	Qt headers
#include <QtCore/QMap>
#include <QtCore/QUrl>
#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QCloseEvent>
#include <QtGui/QDockWidget>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>

//	local headers
#ifdef Q_OS_WIN
#include "AppInfo.win.h"
#else
#include "AppInfo.h"
#endif

#include "AboutDlg.h"
#include "CharsetsSettings.h"
#include "CommandStorage.h"
#include "DocHandler.h"
#include "DocViewer.h"
#include "FindDlg.h"
#include "IconManager.h"
#include "LexerStorage.h"
#include "license.h"
#include "Log.h"
#include "MainSettings.h"
#include "JuffPlugin.h"
#include "PluginManager.h"
#include "PluginSettings.h"
#include "Settings.h"
#include "SettingsDlg.h"
#include "TextDoc.h"
#include "TextDocView.h"

#include "juffed.xpm"

class JuffEdInterior {
public:
	JuffEdInterior(DocHandler* h, QWidget* parent) : 
		toolBar_(0),
		charsetsMenu_(0), 
		syntaxMenu_(0), 
		eolMenu_(0),
		markersMenu_(0), 
		recentFilesMenu_(0), 
		panelsMenu_(0), 
		toolbarsMenu_(0), 
		lastCharsetAction_(0),
		lastSyntaxAction_(0) {

		handler_ = h;
		viewer_ = new DocViewer(parent);
		handler_->addViewer(viewer_);

		settingsDlg_ = new SettingsDlg(parent);
		createAboutDlg(parent);
	}
	~JuffEdInterior() {
		delete aboutDlg_;
		delete settingsDlg_;
		delete viewer_;
	}

	struct Helper {
		QString name;
		QString email;
		QString contribution;
		Helper (const QString& nm = "", const QString& eml = "", const QString& contr = "") {
			name = nm;
			email = eml;
			contribution = contr;
		}
	};
	
	void createAboutDlg(QWidget* parent) {
		aboutDlg_ = new AboutDlg(parent);
		aboutDlg_->setWindowTitle(QObject::tr("About"));
		aboutDlg_->setProgramName(AppInfo::name() + " v" + AppInfo::version());
		QString text = QString("   %1   \n\n   Copyright (c) 2007-2008 Mikhail Murzin   ").arg(QObject::tr("Simple text editor with syntax highlighting"));
		QString auth("<br>&nbsp;Mikhail Murzin a.k.a. Mezomish<br>&nbsp;&nbsp;<a href='mailto:mezomish@gmail.com'>mezomish@gmail.com</a>");
		QList<Helper> helpers;
		helpers << Helper("Jarek", "ajep9691@wp.pl", QObject::tr("Polish translation"))
				<< Helper("Michael Gangolf", "miga@migaweb.de", QObject::tr("German translation"))
				<< Helper("Marc Dumoulin", "shadosan@gmail.com", QObject::tr("French translation"))
				<< Helper("Giuliano S. Nascimento", "giusoft.tecnologia@gmail.com", QObject::tr("Brazilian Portuguese translation"));

		QString thanks;
		foreach(Helper helper, helpers) {
			thanks += QString("&nbsp;%1<br>").arg(helper.name);
			thanks += QString("&nbsp;&nbsp;<a href='mailto:%1'>%2</a><br>").arg(helper.email).arg(helper.email);
			thanks += QString("&nbsp;&nbsp;%1<br><br>").arg(helper.contribution);
		}

		aboutDlg_->setText(text);
		aboutDlg_->setAuthors(auth);
		aboutDlg_->setThanks(thanks);
		aboutDlg_->setLicense(License::licenseText, false);
		aboutDlg_->resize(500, 300);
		aboutDlg_->setIcon(QIcon(juffed_px));
	}

	DocHandler* handler_;
	DocViewer* viewer_;
	SettingsDlg* settingsDlg_;
	AboutDlg* aboutDlg_;
	
	QLabel* cursorPosL_;
	QLabel* fileNameL_;
	QLabel* charsetL_;
	QLabel* syntaxL_;
	QLabel* eolL_;
	QToolBar* toolBar_;
	QMap<QString, QMenu*> mainMenuItems_;
	QMap<QString, QAction*> charsetActions_;
	QMap<QString, QAction*> syntaxActions_;
	QMenu* charsetsMenu_;
	QMenu* syntaxMenu_;
	QMenu* eolMenu_;
	QMenu* markersMenu_;
	QMenu* recentFilesMenu_;
	QMenu* panelsMenu_;
	QMenu* toolbarsMenu_;
	QMenu* sessionsMenu_;
	QAction* lastCharsetAction_;
	QAction* lastSyntaxAction_;
	QRect geometry_;
	QMap<QString, bool> pluginActivated_;
};

JuffEd::JuffEd(DocHandler* handler) : QMainWindow() {
	setWindowTitle("JuffEd");

	jInt_ = new JuffEdInterior(handler, this);
	setAcceptDrops(true);
	
	//	settings
	setGeometry(MainSettings::windowRect());
	jInt_->geometry_ = geometry();
	if (MainSettings::isMaximized())
		showMaximized();
	applySettings();
	
	connect(jInt_->settingsDlg_, SIGNAL(applied()), SLOT(applySettings()));

	//	menus and toolbar	
	createCommands();
	createMenuBar();
	createToolBar();

	//	statusbar
	jInt_->cursorPosL_ = new QLabel("");
	jInt_->fileNameL_ = new QLabel("");
	jInt_->charsetL_ = new QLabel("");
	jInt_->syntaxL_ = new QLabel("");
	jInt_->eolL_ = new QLabel("");
	statusBar()->addWidget(jInt_->cursorPosL_);
	statusBar()->addWidget(jInt_->fileNameL_);
	statusBar()->addWidget(jInt_->charsetL_);
	statusBar()->addWidget(jInt_->syntaxL_);
	statusBar()->addWidget(jInt_->eolL_);
	
	setCentralWidget(jInt_->viewer_->widget());

	connect(jInt_->viewer_, SIGNAL(docViewChanged(QWidget*)), SLOT(docSwitched(QWidget*)));
	connect(jInt_->viewer_, SIGNAL(docViewCloseRequested(QWidget*)), SLOT(docCloseRequested(QWidget*)));
	connect(jInt_->viewer_, SIGNAL(requestFileName(QWidget*, QString&)), SLOT(fileNameRequested(QWidget*, QString&)));
	connect(jInt_->handler_, SIGNAL(fileNameChanged(Juff::Document*)), SLOT(docFileNameChanged(Juff::Document*)));
	connect(jInt_->handler_, SIGNAL(recentFileAdded()), SLOT(initRecentFilesMenu()));
	connect(jInt_->handler_, SIGNAL(cursorPositionChanged(int, int)), SLOT(displayCursorPos(int, int)));

	loadPlugins();
	jInt_->settingsDlg_->addPluginsSettings();
	initPlugins();
	
	restoreState(MainSettings::mwState());
}

JuffEd::~JuffEd() {
	JUFFDTOR;

	MainSettings::setWindowRect(jInt_->geometry_);
	MainSettings::setMaximized(isMaximized());
	if (!jInt_->handler_->sessionName().isEmpty()) {
		MainSettings::setLastSessionName(jInt_->handler_->sessionName());
	}

	delete jInt_;
}

void JuffEd::resizeEvent(QResizeEvent* e) {
	JUFFENTRY;

	QMainWindow::resizeEvent(e);
	if (!isMaximized())
		jInt_->geometry_ = geometry();
}

void JuffEd::moveEvent(QMoveEvent* e) {
	JUFFENTRY;

	QMainWindow::moveEvent(e);
	if (!isMaximized())
		jInt_->geometry_ = geometry();
}

void JuffEd::closeEvent(QCloseEvent* e) {
	JUFFENTRY;

	if (jInt_->handler_->closeAllDocs()) {
		QByteArray state = saveState();
		MainSettings::setMwState(state);
		PluginManager::instance()->unloadPlugins();
		e->accept();
	}
	else {
		e->ignore();
	}
}

void JuffEd::about() {
	jInt_->aboutDlg_->exec();
}

void JuffEd::aboutQt() {
	QMessageBox::aboutQt(this, tr("About Qt"));
}

void JuffEd::exit() {
	close();
}

void JuffEd::applySettings() {
	JUFFENTRY;

	jInt_->handler_->applySettings();
	jInt_->viewer_->applySettings();

	setupToolBarStyle();	
	IconManager::instance()->loadTheme(MainSettings::iconTheme());
	createCommands();
	initCharsetsMenu();

	initPlugins();
}

void JuffEd::initPlugins() {
	JUFFENTRY;

	PluginList plugins = PluginManager::instance()->plugins();
	foreach (JuffPlugin* plugin, plugins) {
		if (plugin != 0) {
			bool newEnabled = PluginSettings::pluginEnabled(plugin->name());
			bool oldEnabled = jInt_->pluginActivated_[plugin->name()];
			if (newEnabled != oldEnabled) {
				//	plugin enable state was changed
				PluginManager::instance()->enablePlugin(plugin->name(), newEnabled);
				if (newEnabled) {
					plugin->applySettings();
				}
				activatePlugin(plugin, newEnabled);
			}
		}
	}
}

void JuffEd::loadPlugins() {
	JUFFENTRY;

	PluginManager::instance()->setHandler(jInt_->handler_);
	PluginManager::instance()->loadPlugins();

	PluginList plugins = PluginManager::instance()->plugins();
	foreach (JuffPlugin* plugin, plugins) {
		jInt_->pluginActivated_[plugin->name()] = false;
	}
}

void JuffEd::activatePlugin(JuffPlugin* plugin, bool activate) {
	JUFFENTRY;

	if (plugin == 0)
		return;

	jInt_->pluginActivated_[plugin->name()] = activate;
	
	if (activate) {
		//	toolbar
		QToolBar* toolBar = plugin->toolBar();
		if (toolBar != 0) {
			addToolBar(toolBar);
			toolBar->show();

			//	toolbars toggle actions
			if (jInt_->toolbarsMenu_ != 0) {
				jInt_->toolbarsMenu_->addAction(toolBar->toggleViewAction());
			}
		}

		//	dock widget
		Qt::DockWidgetArea area(Qt::LeftDockWidgetArea);
		QWidget* w = plugin->dockWidget(area);
		if (w != 0) {
			QDockWidget* dock = new QDockWidget(plugin->name());
			dock->setObjectName(plugin->name() + "Dock");
			dock->setWidget(w);
			addDockWidget(area, dock);

			//	panels toggle actions
			if (jInt_->panelsMenu_ != 0) {
				jInt_->panelsMenu_->addAction(dock->toggleViewAction());
			}
		}
		
		//	menu
		QMenu* tMenu = jInt_->mainMenuItems_.value(tr("&Tools"), 0);
		if (tMenu != 0 && plugin->menu() != 0) {
			menuBar()->insertMenu(tMenu->menuAction(), plugin->menu());
		}
	}
	else {
		//	toolbar`
		if (plugin->toolBar() != 0) {
			removeToolBar(plugin->toolBar());
		}
		
		//	dock widget
		Qt::DockWidgetArea area;
		QWidget* dw = plugin->dockWidget(area);
		if (dw != 0) {
			QDockWidget* dock = qobject_cast<QDockWidget*>(dw->parentWidget());
			if (dock != 0) {
				removeDockWidget(dock);
				if (jInt_->panelsMenu_ != 0) {
					jInt_->panelsMenu_->removeAction(dock->toggleViewAction());
				}
			}
		}
		
		//	menu
		QMenu* tMenu = jInt_->mainMenuItems_.value(tr("&Tools"), 0);
		if (tMenu != 0 && plugin->menu() != 0) {
			menuBar()->removeAction(plugin->menu()->menuAction());
		}
	}
}

void JuffEd::createCommands() {
	JUFFENTRY;

	CommandStorage* st = CommandStorage::instance();
	DocHandler* h = jInt_->handler_;
	IconManager* im = IconManager::instance();
	
	Command cmds[] = {
		Command(ID_FILE_NEW,	tr("New"),		im->icon("fileNew"),	QKeySequence("Ctrl+N"), h, SLOT(docNew())),
		Command(ID_FILE_OPEN,	tr("Open"),		im->icon("fileOpen"),	QKeySequence("Ctrl+O"), h, SLOT(docOpen())),
		Command(ID_FILE_SAVE,	tr("Save"),		im->icon("fileSave"),	QKeySequence("Ctrl+S"), h, SLOT(docSave())),
		Command(ID_FILE_SAVE_AS,tr("Save as"),	im->icon("fileSaveAs"),	QKeySequence("Ctrl+Shift+S"),			h, SLOT(docSaveAs())),
		Command(ID_FILE_RELOAD, tr("Reload"),	im->icon("reload"),		QKeySequence("F5"),		h, SLOT(docReload())),
		Command(ID_FILE_PRINT,	tr("Print"),	im->icon("filePrint"),	QKeySequence("Ctrl+P"),	h, SLOT(docPrint())),
		Command(ID_FILE_PRINT_SELECTED,	tr("Print selected lines"),	im->icon("filePrintSelected"),	QKeySequence(),	h, SLOT(docPrintSelected())),
		Command(ID_FILE_CLOSE,	tr("Close"),	im->icon("fileClose"),	QKeySequence("Ctrl+W"), h, SLOT(docClose())),
		Command(ID_FILE_CLOSE_ALL, tr("Close all"), im->icon("fileCloseAll"), QKeySequence(),	h, SLOT(docCloseAll())),
		Command(ID_EXIT,		tr("Exit"),		im->icon("exit"),		QKeySequence("Ctrl+Q"),			this, SLOT(exit())),
		//
		Command(ID_EDIT_CUT,	tr("Cut"),		im->icon("editCut"),	QKeySequence("Ctrl+X"), h, SLOT(processTheCommand())),
		Command(ID_EDIT_COPY,	tr("Copy"),		im->icon("editCopy"),	QKeySequence("Ctrl+C"), h, SLOT(processTheCommand())),
		Command(ID_EDIT_PASTE,	tr("Paste"),	im->icon("editPaste"),	QKeySequence("Ctrl+V"), h, SLOT(processTheCommand())),
		Command(ID_EDIT_UNDO,	tr("Undo"),		im->icon("editUndo"),	QKeySequence("Ctrl+Z"), h, SLOT(processTheCommand())),
		Command(ID_EDIT_REDO,	tr("Redo"),		im->icon("editRedo"),	QKeySequence("Ctrl+Y"), h, SLOT(processTheCommand())),
		Command(ID_FIND,		tr("Find"),		im->icon("find"),		QKeySequence("Ctrl+F"), h, SLOT(processTheCommand())),
		Command(ID_FIND_NEXT,	tr("Find next"), im->icon("findNext"),	QKeySequence("F3"), h, SLOT(processTheCommand())),
		Command(ID_FIND_PREV,	tr("Find previous"), im->icon("findPrev"), QKeySequence("Shift+F3"), h, SLOT(processTheCommand())),
		Command(ID_REPLACE,		tr("Replace"),	im->icon("replace"),	QKeySequence("Ctrl+R"), h, SLOT(processTheCommand())),
		Command(ID_GOTO_LINE,	tr("Go to line"), im->icon("gotoLine"),	QKeySequence("Ctrl+G"), h, SLOT(processTheCommand())),
		Command(ID_UNINDENT,	tr("Unindent"), im->icon("unindent"),	QKeySequence("Shift+Tab"), h, SLOT(processTheCommand())),
		//
		Command(ID_VIEW_SHOW_LINE_NUMBERS,	tr("Show line numbers"),	QIcon(),	QKeySequence("F11"), h,	SLOT(processTheCommand())),
		Command(ID_VIEW_WIDTH_ADJUST,		tr("Adjust text by width"),	QIcon(),	QKeySequence("F10"), h,	SLOT(processTheCommand())),
		Command(ID_VIEW_SHOW_HIDDEN_SYMBOLS,tr("Show hidden symbols"),	QIcon(),	QKeySequence(), h,	SLOT(processTheCommand())),
		//
		Command(ID_ZOOM_IN,		tr("Increase font"),im->icon("zoomIn"),	QKeySequence("Ctrl+="), h,	SLOT(processTheCommand())),
		Command(ID_ZOOM_OUT,	tr("Decrease font"),im->icon("zoomOut"),QKeySequence("Ctrl+-"), h,	SLOT(processTheCommand())),
		Command(ID_ZOOM_100,	tr("Normal size"),	im->icon("zoom100"),QKeySequence("Ctrl+0"), h,	SLOT(processTheCommand())),
		//
		Command(ID_EOL_WIN,		tr("Win"),			im->icon("win"),	QKeySequence(),		this,	SLOT(eolSelected())),
		Command(ID_EOL_UNIX,	tr("Unix"),			im->icon("unix"),	QKeySequence(),		this,	SLOT(eolSelected())),
		Command(ID_EOL_MAC,		tr("Mac"),			im->icon("mac"),	QKeySequence(),		this,	SLOT(eolSelected())),
		//
		Command(ID_MARKER_TOGGLE,			tr("Add/Remove marker"),	im->icon("addRemoveMarker"),	QKeySequence("Ctrl+B"), this, SLOT(toggleMarker())),                    
		Command(ID_MARKER_NEXT,				tr("Next marker"),			im->icon("nextMarker"),			QKeySequence("Ctrl+Alt+PgDown"),this, SLOT(nextMarker())),
		Command(ID_MARKER_PREV,				tr("Previous marker"),		im->icon("prevMarker"),			QKeySequence("Ctrl+Alt+PgUp"),this, SLOT(prevMarker())),
		Command(ID_MARKER_REMOVE_ALL,		tr("Remove all markers"),	im->icon("removeAllMarkers"),	QKeySequence(), 		this, SLOT(removeAllMarkers())),
		//
		Command(ID_DOC_PREV,	tr("Previous"),	im->icon("docPrevious"),	QKeySequence("Alt+Left"), h, SLOT(processTheCommand())),
		Command(ID_DOC_NEXT,	tr("Next"),	im->icon("docNext"),	QKeySequence("Alt+Right"), h, SLOT(processTheCommand())),
		//
		Command(ID_SETTINGS,	tr("Settings"),	im->icon("settings"),	QKeySequence(), this, SLOT(settings())),
		//
		Command(ID_SESSION_NEW,	tr("New session"),	im->icon("newSession"),	QKeySequence(), h, SLOT(processTheCommand())),
		Command(ID_SESSION_OPEN, tr("Open session"), im->icon("openSession"), QKeySequence(), h, SLOT(processTheCommand())),
		Command(ID_SESSION_SAVE, tr("Save session"), im->icon("saveSession"), QKeySequence(), h, SLOT(processTheCommand())),
		Command(ID_SESSION_SAVE_AS, tr("Save session as"), im->icon("saveSessionAs"), QKeySequence(), h, SLOT(processTheCommand())),
		//
		Command(ID_ABOUT,		tr("About"),	im->icon("about"),		QKeySequence(), this, SLOT(about())),
		Command(ID_ABOUT_QT,	tr("About Qt"),	im->icon("aboutQt"),	QKeySequence(), this, SLOT(aboutQt())),
		Command()
	};

	for (unsigned i = 0; cmds[i].id != 0; i++) {
		Command& cmd = cmds[i];
		if (st->action(cmd.id) == 0)
			st->registerCommand(cmds[i]);
		else
			st->action(cmd.id)->setIcon(cmd.icon);
	}

	CommandStorage::instance()->action(ID_VIEW_SHOW_LINE_NUMBERS)->setCheckable(true);
	CommandStorage::instance()->action(ID_VIEW_WIDTH_ADJUST)->setCheckable(true);
	CommandStorage::instance()->action(ID_VIEW_SHOW_HIDDEN_SYMBOLS)->setCheckable(true);
	CommandStorage::instance()->action(ID_EOL_WIN)->setCheckable(true);
	CommandStorage::instance()->action(ID_EOL_UNIX)->setCheckable(true);
	CommandStorage::instance()->action(ID_EOL_MAC)->setCheckable(true);

	if (jInt_->recentFilesMenu_ != 0)
		jInt_->recentFilesMenu_->setIcon(im->icon("fileOpen"));
}

void JuffEd::createMenuBar() {
	JUFFENTRY;

	menuBar()->clear();

	CommandID fileMenu[] = { ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, ID_FILE_SAVE_AS, 
					ID_FILE_RELOAD, ID_FILE_PRINT, ID_FILE_PRINT_SELECTED, ID_FILE_CLOSE, ID_FILE_CLOSE_ALL, ID_SEPARATOR, 
//					ID_SESSION_NEW, ID_SESSION_OPEN, ID_SESSION_SAVE, ID_SESSION_SAVE_AS, 
//					ID_SEPARATOR, 
					ID_EXIT, ID_NONE };

	CommandID editMenu[] = { ID_EDIT_CUT, ID_EDIT_COPY, ID_EDIT_PASTE, ID_SEPARATOR, ID_EDIT_UNDO, 
					ID_EDIT_REDO, ID_SEPARATOR, ID_FIND, ID_FIND_NEXT, ID_FIND_PREV, ID_REPLACE,
					ID_SEPARATOR, ID_GOTO_LINE, ID_NONE };

	CommandID viewMenu[] = { ID_VIEW_SHOW_LINE_NUMBERS, ID_VIEW_WIDTH_ADJUST, 
					ID_VIEW_SHOW_HIDDEN_SYMBOLS, ID_ZOOM_IN, ID_ZOOM_OUT, ID_ZOOM_100, ID_NONE };

	CommandID formatMenu[] = { ID_NONE };
	
	CommandID markersMenu[] = { ID_NONE };	

	CommandID toolsMenu[] = { ID_SETTINGS, ID_NONE };

	CommandID helpMenu[] = { ID_ABOUT, ID_ABOUT_QT, ID_NONE };

	CommandID* menu_ids[] = { fileMenu, editMenu, viewMenu, formatMenu, 
								markersMenu, toolsMenu, helpMenu, 0 };
	QString menus[] = { tr("&File"), tr("&Edit"), tr("&View"), tr("Format"), 
								tr("&Markers"), tr("&Tools"), tr("&Help"), "" };


	for (unsigned i = 0; !menus[i].isEmpty(); i++) {
		QMenu* menu = menuBar()->addMenu(menus[i]);
		jInt_->mainMenuItems_[menus[i]] = menu;

		for (int j = 0; menu_ids[i][j] != ID_NONE; j++) {
			CommandID id = menu_ids[i][j];
			if (id == ID_SEPARATOR)
				menu->addSeparator();
			else {
				QAction* action = CommandStorage::instance()->action(id);
				if (action != 0) {
					menu->addAction(action);
				}
			}
		}
	}

	QAction* action = CommandStorage::instance()->action(ID_DOC_PREV);
	if (action != 0)
		addAction(action);
	action = CommandStorage::instance()->action(ID_DOC_NEXT);
	if (action != 0)
		addAction(action);
	action = CommandStorage::instance()->action(ID_UNINDENT);
	if (action != 0)
		addAction(action);
	action = CommandStorage::instance()->action(ID_ZOOM_IN);
	if (action != 0)
		addAction(action);
	action = CommandStorage::instance()->action(ID_ZOOM_OUT);
	if (action != 0)
		addAction(action);

	//	charset menu
	jInt_->charsetsMenu_ = new QMenu(tr("Charset"));
	QMenu* fmtMenu = jInt_->mainMenuItems_.value(tr("Format"), 0);
	if (fmtMenu != 0) {
		fmtMenu->addMenu(jInt_->charsetsMenu_);
	}
	initCharsetsMenu();

	//	lexers menu
	jInt_->syntaxMenu_ = new QMenu(tr("Syntax"));
	if (fmtMenu != 0) {
		fmtMenu->addMenu(jInt_->syntaxMenu_);
		QStringList sList;
		LexerStorage::instance()->getLexersList(sList);
		foreach (QString s, sList) {
			QAction* a = jInt_->syntaxMenu_->addAction(s, this, SLOT(syntaxSelected()));
			a->setCheckable(true);
			jInt_->syntaxActions_[s] = a;
		}	
	}

	//	EOL menu
	if (fmtMenu != 0) {
		jInt_->eolMenu_ = new QMenu(tr("End of line"));
		QActionGroup* gr = new QActionGroup(this);

		QAction* actions[] = {
				CommandStorage::instance()->action(ID_EOL_WIN),
				CommandStorage::instance()->action(ID_EOL_UNIX),
				CommandStorage::instance()->action(ID_EOL_MAC) };
		for (int i = 0; i < 3; ++i) {
			jInt_->eolMenu_->addAction(actions[i]);
			gr->addAction(actions[i]);
		}
		
		fmtMenu->addMenu(jInt_->eolMenu_);
	}
	
	//	markers menu
	initMarkersMenu();
	QMenu* markMenu = jInt_->mainMenuItems_.value(tr("&Markers"), 0);
	if (markMenu != 0) {
		connect(markMenu, SIGNAL(aboutToShow()), SLOT(initMarkersMenu()));
	}

	//	recent files
	jInt_->recentFilesMenu_ = new QMenu(tr("Recent files"));
	jInt_->recentFilesMenu_->setIcon(IconManager::instance()->icon("fileOpen"));
	QMenu* fMenu = jInt_->mainMenuItems_.value(tr("&File"), 0);
	QAction* saveAct = CommandStorage::instance()->action(ID_FILE_SAVE);
	if (fMenu != 0 && saveAct != 0) {
		fMenu->insertMenu(saveAct, jInt_->recentFilesMenu_);
		initRecentFilesMenu();
	}
	
	//	sessions
	jInt_->sessionsMenu_ = new QMenu(tr("Session"));
	QAction* exitAct = CommandStorage::instance()->action(ID_EXIT);
	if (fMenu != 0 && exitAct != 0) {
		fMenu->insertMenu(exitAct, jInt_->sessionsMenu_);
		fMenu->insertSeparator(exitAct);
		initSessionsMenu();
	}
	
	//	plugins panels and toolbars
	jInt_->panelsMenu_ = new QMenu(tr("Dock windows"));
	jInt_->toolbarsMenu_ = new QMenu(tr("Toolbars"));
	QMenu* tMenu = jInt_->mainMenuItems_.value(tr("&Tools"), 0);
	QAction* settAct = CommandStorage::instance()->action(ID_SETTINGS);
	if (tMenu != 0 && settAct != 0) {
		tMenu->insertMenu(settAct, jInt_->toolbarsMenu_);
		tMenu->insertMenu(settAct, jInt_->panelsMenu_);
		tMenu->insertSeparator(settAct);
	}
}

void JuffEd::createToolBar() {
	JUFFENTRY;

	CommandID actions[] = { ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, ID_SEPARATOR, ID_FILE_PRINT, 
					ID_SEPARATOR, ID_EDIT_CUT, ID_EDIT_COPY, ID_EDIT_PASTE, 
					ID_SEPARATOR, ID_EDIT_UNDO, ID_EDIT_REDO, ID_SEPARATOR, 
					ID_FIND, ID_SEPARATOR, ID_ZOOM_IN, ID_ZOOM_OUT, ID_ZOOM_100, ID_NONE };

	jInt_->toolBar_ = addToolBar("Main");
	jInt_->toolBar_->setObjectName("MainToolBar");
	jInt_->toolbarsMenu_->addAction(jInt_->toolBar_->toggleViewAction());

	for (unsigned i = 0; actions[i] != ID_NONE; i++) {
		CommandID id = actions[i];
		if (id == ID_SEPARATOR)
			jInt_->toolBar_->addSeparator();
		else
			jInt_->toolBar_->addAction(CommandStorage::instance()->action(actions[i]));
	}

	setupToolBarStyle();	
}

void JuffEd::initCharsetsMenu() {
	JUFFENTRY;

	if (jInt_->charsetsMenu_ == 0)
		return;

	jInt_->charsetsMenu_->clear();
	jInt_->charsetActions_.clear();

	QStringList charsets = CharsetsSettings::getCharsetsList();
	foreach (QString charset, charsets) {
		if (CharsetsSettings::charsetEnabled(charset)) {
			QAction* action = jInt_->charsetsMenu_->addAction(charset, this, SLOT(charsetSelected()));
			action->setCheckable(true);
			jInt_->charsetActions_[charset] = action;
		}
	}
}

void JuffEd::initMarkersMenu() {
	JUFFENTRY;

	QMenu* markersMenu = jInt_->mainMenuItems_.value(tr("&Markers"), 0);
	if (markersMenu != 0) {
		markersMenu->clear();
		markersMenu->addAction(CommandStorage::instance()->action(ID_MARKER_TOGGLE));
		markersMenu->addAction(CommandStorage::instance()->action(ID_MARKER_NEXT));
		markersMenu->addAction(CommandStorage::instance()->action(ID_MARKER_PREV));
		markersMenu->addAction(CommandStorage::instance()->action(ID_MARKER_REMOVE_ALL));
		markersMenu->addSeparator();
		
		TextDoc* doc = getCurrentTextDoc();
		if (doc == 0 || doc->isNull())
			return;

		TextDocView* tdView = qobject_cast<TextDocView*>(doc->view());		

		if (tdView == 0)
			return;

		IntList markers = tdView->markers();
		foreach (int m, markers) {
			QString str = tdView->markedLine(m).simplified();
			if (str.length() > 40)
				str = str.left(40) + " ...";
			markersMenu->addAction(QString("%1: %2").arg(m).arg(str), this, SLOT(gotoMarker()));
		}
	}
}

void JuffEd::initRecentFilesMenu() {
	JUFFENTRY;

	if (jInt_->recentFilesMenu_ == 0)
		return;

	jInt_->recentFilesMenu_->clear();
	
	QStringList fileList;
	jInt_->handler_->getRecentFiles(fileList);
	foreach (QString fileName, fileList) {
		jInt_->recentFilesMenu_->addAction(fileName, jInt_->handler_, SLOT(docOpenRecent()));
	}
	if (fileList.count() == 0)
		jInt_->recentFilesMenu_->setEnabled(false);
	else
		jInt_->recentFilesMenu_->setEnabled(true);
}
	
void JuffEd::initSessionsMenu() {
	if (jInt_->sessionsMenu_ == 0)
		return;
	
	jInt_->sessionsMenu_->clear();
	
	jInt_->sessionsMenu_->addAction(CommandStorage::instance()->action(ID_SESSION_NEW));
	jInt_->sessionsMenu_->addAction(CommandStorage::instance()->action(ID_SESSION_OPEN));
	jInt_->sessionsMenu_->addAction(CommandStorage::instance()->action(ID_SESSION_SAVE));
	jInt_->sessionsMenu_->addAction(CommandStorage::instance()->action(ID_SESSION_SAVE_AS));
}

void JuffEd::changeCurrentCharsetAction(QAction* a) {
	JUFFENTRY;

	if (jInt_->lastCharsetAction_ != 0)
		jInt_->lastCharsetAction_->setChecked(false);

	if (a != 0) {
		jInt_->lastCharsetAction_ = a;
		a->setChecked(true);
	}
}

void JuffEd::changeCurrentSyntaxAction(QAction* a) {
	JUFFENTRY;

	if (jInt_->lastSyntaxAction_ != 0)
		jInt_->lastSyntaxAction_->setChecked(false);

	if (a != 0) {
		jInt_->lastSyntaxAction_ = a;
		a->setChecked(true);
	}
}

void JuffEd::charsetSelected() {
	JUFFENTRY;

	QAction* a = qobject_cast<QAction*>(sender());
	if (a != 0) {
		TextDoc* doc = getCurrentTextDoc();
		if (doc != 0 && !doc->isNull()) {
			if (doc->setCharset(a->text())) {
				displayCharset(doc->charset());
				changeCurrentCharsetAction(a);
			}
			else {
				a->setChecked(false);
			}
		}
		else {
			a->setChecked(false);
		}
	}
}

void JuffEd::syntaxSelected() {
	JUFFENTRY;

	QAction* a = qobject_cast<QAction*>(sender());
	if (a != 0) {
		TextDoc* doc = getCurrentTextDoc();
		if (doc != 0 && !doc->isNull()) {
			TextDocView* tdView = qobject_cast<TextDocView*>(doc->view());
			if (tdView != 0) {
				tdView->setSyntax(a->text());
				displaySyntax(a->text());
				changeCurrentSyntaxAction(a);
			}
			else {
				a->setChecked(false);
			}
		}
		else {
			a->setChecked(false);
		}
	}
}

void JuffEd::eolSelected() {
	JUFFENTRY;

	QAction* a = qobject_cast<QAction*>(sender());
	if (a != 0) {
		QString eolModeStr = a->text();
		Log::debug(eolModeStr);
		TextDocView* tdView = getCurrentTextDocView();
		if (tdView != 0) {
			int mode = -1;
			if (eolModeStr.compare("Win") == 0) {
				mode = TextDocView::EOL_WIN;
			}
			else if (eolModeStr.compare("Unix") == 0) {
				mode = TextDocView::EOL_UNIX;
			}
			else if (eolModeStr.compare("Mac") == 0) {
				mode = TextDocView::EOL_MAC;
			}
			tdView->setEolMode(mode);
			displayEol(mode);
		}
	}
}

void JuffEd::settings() {
	JUFFENTRY;

	jInt_->settingsDlg_->exec();
}

void JuffEd::setupToolBarStyle() {
	JUFFENTRY;

	if (jInt_->toolBar_ != 0) {
		//	toolbar style
		int style = MainSettings::toolButtonStyle();
		if (style <= 4) {
			jInt_->toolBar_->show();
			jInt_->toolBar_->setToolButtonStyle((Qt::ToolButtonStyle)style);
		}
		else {
			jInt_->toolBar_->hide();
		}

		//	toolbar icon size
		int iconSize = MainSettings::iconSize();
		QSize sz(24, 24);
		switch (iconSize) {
			case 0: sz = QSize(16, 16); break;
			case 2: sz = QSize(32, 32); break;
			case 1:
			default: ;
		}
		
		jInt_->toolBar_->setIconSize(sz);
		PluginList plugins = PluginManager::instance()->plugins();
		foreach (JuffPlugin* plugin, plugins) {
			if (plugin != 0) {
				if (plugin->toolBar() != 0) {
					plugin->toolBar()->setIconSize(sz);
				}
			}
		}
	}
}

TextDoc* JuffEd::getCurrentTextDoc() {
	JUFFENTRY;

	TextDoc* tDoc = qobject_cast<TextDoc*>(jInt_->handler_->currentDoc());
	return tDoc;
}

TextDocView* JuffEd::getCurrentTextDocView() {
	TextDoc* doc = getCurrentTextDoc();
	if (doc != 0 && !doc->isNull()) {
		TextDocView* tdView = qobject_cast<TextDocView*>(doc->view());
		return tdView;
	}
	return 0;
}

void JuffEd::displayCursorPos(int row, int col) {
	JUFFENTRY;

	QString text(tr("Row: %1, Col: %2"));
	if (row == -1 && col == -1)
		jInt_->cursorPosL_->setText(" ");
	else
		jInt_->cursorPosL_->setText(text.arg(row + 1).arg(col + 1));
}

void JuffEd::displayFileName(const QString& fileName) {
	JUFFENTRY;

	jInt_->fileNameL_->setText(QString(" %1 ").arg(fileName));
	QString title("JuffEd");

	//	session name
	QString sessName = jInt_->handler_->sessionName();
	if (!sessName.isEmpty()	&& sessName.compare("_empty_session_") != 0)
		title += " - [" + sessName + "]";

	//	file name
	if (fileName.left(6).compare("Noname") != 0)
		title += " - " + QFileInfo(fileName).fileName();
	else
		title += " - " + tr("Noname");

	setWindowTitle(title);
}

void JuffEd::displayCharset(const QString& charset) {
	JUFFENTRY;

	jInt_->charsetL_->setText(QString(" %1 ").arg(charset));
	changeCurrentCharsetAction(jInt_->charsetActions_[charset]);
}

void JuffEd::displaySyntax(const QString& syntax) {
	JUFFENTRY;

	jInt_->syntaxL_->setText(QString(" %1 ").arg(syntax));
	changeCurrentSyntaxAction(jInt_->syntaxActions_[syntax]);
}

void JuffEd::displayEol(int eolMode) {
	JUFFENTRY;

	QString str("??");
	switch (eolMode) {
		case TextDocView::EOL_WIN: 
			CommandStorage::instance()->action(ID_EOL_WIN)->setChecked(true);
			str = tr("Win");
			break;

		case TextDocView::EOL_UNIX: 
			CommandStorage::instance()->action(ID_EOL_UNIX)->setChecked(true);
			str = tr("Unix");
			break;

		case TextDocView::EOL_MAC: 
			CommandStorage::instance()->action(ID_EOL_MAC)->setChecked(true);
			str = tr("Mac");
			break;
	}
	
	jInt_->eolL_->setText(QString(" %1 ").arg(str));
}

void JuffEd::docFileNameChanged(Juff::Document* doc) {
	JUFFENTRY;

	if (doc != 0 && !doc->isNull()) {
		displayFileName(doc->fileName());
	}
}

void JuffEd::docCloseRequested(QWidget* w) {
	JUFFENTRY;

	TextDocView* tdView = qobject_cast<TextDocView*>(w);
	if (tdView != 0) {
		Juff::Document* doc = tdView->document();
		if (doc != 0) {
			jInt_->handler_->closeDocument(doc);
		}
	}
}

void JuffEd::fileNameRequested(QWidget* w, QString& fileName) {
	JUFFENTRY;

	if (w == 0) {
		fileName = "";
	}
	else {
		TextDocView* tdView = qobject_cast<TextDocView*>(w);
		if (tdView == 0) {
			fileName = "";
		}
		else {
			Juff::Document* doc = tdView->document();
			if (doc == 0)
				fileName = "";
			else
				fileName = doc->fileName();
		}
	}
}

void JuffEd::docSwitched(QWidget* w) {
	JUFFENTRY;

	TextDocView* tdView = qobject_cast<TextDocView*>(w);

	if (tdView == 0) {
		displayCursorPos(-1, -1);
		displayFileName("");
		displayCharset("");
		displaySyntax("none");
		displayEol(-1);
		return;
	}

	//	show line numbers menu item
	QAction* showLinesAction = CommandStorage::instance()->action(ID_VIEW_SHOW_LINE_NUMBERS);
	if (showLinesAction != 0) {
		showLinesAction->setChecked(tdView->lineNumIsVisible());
	}

	//	width adjust menu item
	QAction* adjustAction = CommandStorage::instance()->action(ID_VIEW_WIDTH_ADJUST);
	if (adjustAction != 0) {
		adjustAction->setChecked(tdView->isAdjustedByWidth());
	}

	//	show hidden symbols menu item
	QAction* showWSAction = CommandStorage::instance()->action(ID_VIEW_SHOW_HIDDEN_SYMBOLS);
	if (showWSAction != 0) {
		showWSAction->setChecked(tdView->hiddenSymbolsVisible());
	}
	
	displayFileName(tdView->document()->fileName());
	int row(0), col(0);
	tdView->getCursorPos(row, col);
	displayCursorPos(row, col);
	displaySyntax(tdView->syntax());

	//	charset menu item
	TextDoc* doc = qobject_cast<TextDoc*>(tdView->document());
	if (doc == 0) {
		displayCharset("");
	}
	else {
		displayCharset(doc->charset());
		jInt_->handler_->docActivated(doc);
	}
	
	//	EOLs
	int eolMode = tdView->eolMode();
	displayEol(eolMode);
}

void JuffEd::toggleMarker() {
	JUFFENTRY;

	QWidget* view = jInt_->viewer_->currentView();	
	TextDocView* tdView = qobject_cast<TextDocView*>(view);

	if (tdView != 0) {
		tdView->toggleMarker();
	}
}

void JuffEd::nextMarker() {
	JUFFENTRY;

	QWidget* view = jInt_->viewer_->currentView();	
	TextDocView* tdView = qobject_cast<TextDocView*>(view);

	if (tdView != 0) {
		tdView->gotoNextMarker();
	}
}

void JuffEd::prevMarker() {
	JUFFENTRY;

	QWidget* view = jInt_->viewer_->currentView();	
	TextDocView* tdView = qobject_cast<TextDocView*>(view);

	if (tdView != 0) {
		tdView->gotoPrevMarker();
	}
}

void JuffEd::removeAllMarkers() {
	JUFFENTRY;

	QWidget* view = jInt_->viewer_->currentView();	
	TextDocView* tdView = qobject_cast<TextDocView*>(view);

	if (tdView != 0) {
		tdView->removeAllMarkers();
	}
}

void JuffEd::gotoMarker() {
	JUFFENTRY;

	QAction* a = qobject_cast<QAction*>(sender());
	if (a != 0) {
		int line = a->text().section(':', 0, 0).toInt();
		QWidget* view = jInt_->viewer_->currentView();	
		TextDocView* tdView = qobject_cast<TextDocView*>(view);
		if (tdView != 0) {
			tdView->gotoLine(line);
		}
	}
}

///////////////////////////////////////////////////////////////////////
//	Drag & Drop
///////////////////////////////////////////////////////////////////////

void JuffEd::dragEnterEvent(QDragEnterEvent* e) {
	JUFFENTRY;

	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void JuffEd::dropEvent(QDropEvent* e) {
	JUFFENTRY;

	if (e->mimeData()->hasUrls()) {
		QList<QUrl> urls = e->mimeData()->urls();
		foreach (QUrl url, urls) {
			QString name = url.path();

#ifdef Q_OS_WIN32			
			//	hack to protect of strings with filenames like /C:/doc/file.txt
			if (name[0] == '/')
				name.remove(0, 1);
#endif

			if (!name.isEmpty()) {
				jInt_->handler_->docOpen(name);
			}
		}
	}
}
