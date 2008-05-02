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
#include <QtGui/QCloseEvent>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>		
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
#include "Settings.h"
#include "SettingsDlg.h"
#include "TextDoc.h"
#include "TextDocView.h"

#include "juffed.xpm"

class JuffEdInterior {
public:
	JuffEdInterior(DocHandler* h, QWidget* parent) : 
		charsetsMenu_(0), 
		syntaxMenu_(0), 
		markersMenu_(0), 
		recentFilesMenu_(0), 
		lastCharsetAction_(0),
		lastSyntaxAction_(0) {

		handler_ = h;
		viewer_ = new DocViewer(parent);
		handler_->addViewer(viewer_);

		settingsDlg_ = new SettingsDlg(parent);
		aboutDlg_ = new AboutDlg(parent);
		aboutDlg_->setWindowTitle(QObject::tr("About"));
		aboutDlg_->setProgramName(AppInfo::name() + " v" + AppInfo::version());
		QString newLine("<br>"), margin("&nbsp;&nbsp;");
		QString text = QString("   %1   \n\n   Copyright (c) 2007-2008 Mikhail Murzin   ").arg(QObject::tr("Simple text editor with syntax highlighting"));
		QString auth("<br>&nbsp;Mikhail Murzin a.k.a. Mezomish<br>&nbsp;&nbsp;<a href='mailto:mezomish@gmail.com'>mezomish@gmail.com</a>");
		QString thanks = QString("<br>&nbsp;Jarek") 
						+ newLine + margin
						+ QString("<a href='mailto:ajep9691@wp.pl'>ajep9691@wp.pl</a>") 
						+ newLine + margin
						+ QObject::tr("Polish translation")
						+ newLine + newLine;
						
		thanks += QString("&nbsp;Michael Gangolf")
						+ newLine + margin
						+ QString("<a href='mailto:miga@migaweb.de'>miga@migaweb.de</a>")
						+ newLine + margin
						+ QObject::tr("German translation")
						+ newLine + newLine;

		thanks += QString("&nbsp;Marc Dumoulin")
						+ newLine + margin
						+ QString("<a href='mailto:shadosan@gmail.com'>shadosan@gmail.com</a>")
						+ newLine + margin
						+ QObject::tr("French translation");

		aboutDlg_->setText(text);
		aboutDlg_->setAuthors(auth);
		aboutDlg_->setThanks(thanks);
		aboutDlg_->setLicense(License::licenseText, false);
		aboutDlg_->resize(500, 250);
		aboutDlg_->setIcon(QIcon(juffed_px));
		
		toolBar_ = 0;
	}
	~JuffEdInterior() {
		delete aboutDlg_;
		delete settingsDlg_;
		delete viewer_;
	}

	DocHandler* handler_;
	DocViewer* viewer_;
	SettingsDlg* settingsDlg_;
	AboutDlg* aboutDlg_;
	
	QLabel* cursorPosL_;
	QLabel* fileNameL_;
	QLabel* charsetL_;
	QLabel* syntaxL_;
	QToolBar* toolBar_;
	QMap<QString, QMenu*> mainMenuItems_;
	QMap<QString, QAction*> charsetActions_;
	QMap<QString, QAction*> syntaxActions_;
	QMenu* charsetsMenu_;
	QMenu* syntaxMenu_;
	QMenu* markersMenu_;
	QMenu* recentFilesMenu_;
	QAction* lastCharsetAction_;
	QAction* lastSyntaxAction_;
	QRect geometry_;
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
	statusBar()->addWidget(jInt_->cursorPosL_);
	statusBar()->addWidget(jInt_->fileNameL_);
	statusBar()->addWidget(jInt_->charsetL_);
	statusBar()->addWidget(jInt_->syntaxL_);
	
	setCentralWidget(jInt_->viewer_->widget());

	connect(jInt_->viewer_, SIGNAL(docViewChanged(QWidget*)), SLOT(docSwitched(QWidget*)));
	connect(jInt_->viewer_, SIGNAL(docViewCloseRequested(QWidget*)), SLOT(docCloseRequested(QWidget*)));
	connect(jInt_->handler_, SIGNAL(fileNameChanged(Juff::Document*)), SLOT(docFileNameChanged(Juff::Document*)));
	connect(jInt_->handler_, SIGNAL(recentFileAdded()), SLOT(initRecentFilesMenu()));
	connect(jInt_->handler_, SIGNAL(cursorPositionChanged(int, int)), SLOT(displayCursorPos(int, int)));
}

JuffEd::~JuffEd() {
	MainSettings::setWindowRect(jInt_->geometry_);
	MainSettings::setMaximized(isMaximized());
	if (!jInt_->handler_->sessionName().isEmpty()) {
		MainSettings::setLastSessionName(jInt_->handler_->sessionName());
	}

	delete jInt_;

}

void JuffEd::resizeEvent(QResizeEvent* e) {
	QMainWindow::resizeEvent(e);
	if (!isMaximized())
		jInt_->geometry_ = geometry();
}

void JuffEd::moveEvent(QMoveEvent* e) {
	QMainWindow::moveEvent(e);
	if (!isMaximized())
		jInt_->geometry_ = geometry();
}

void JuffEd::closeEvent(QCloseEvent* e) {
	if (jInt_->handler_->closeAllDocs())
		e->accept();
	else
		e->ignore();
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
	jInt_->handler_->applySettings();
	jInt_->viewer_->applySettings();

	setupToolBarStyle();	
	IconManager::instance()->loadTheme(MainSettings::iconTheme());
	createCommands();
	initCharsetsMenu();
}

void JuffEd::createCommands() {
	CommandStorage* st = CommandStorage::instance();
	DocHandler* h = jInt_->handler_;
	IconManager* im = IconManager::instance();
	
	Command cmds[] = {
		Command(ID_FILE_NEW,	tr("New"),		im->icon("fileNew"),	QKeySequence("Ctrl+N"), h, SLOT(docNew())),
		Command(ID_FILE_OPEN,	tr("Open"),		im->icon("fileOpen"),	QKeySequence("Ctrl+O"), h, SLOT(docOpen())),
		Command(ID_FILE_SAVE,	tr("Save"),		im->icon("fileSave"),	QKeySequence("Ctrl+S"), h, SLOT(docSave())),
		Command(ID_FILE_SAVE_AS,tr("Save as"),	im->icon("fileSaveAs"),	QKeySequence("Ctrl+Shift+S"),			h, SLOT(docSaveAs())),
		Command(ID_FILE_RELOAD, tr("Reload"),	im->icon("reload"),		QKeySequence("F5"),		h, SLOT(docReload())),
		Command(ID_FILE_CLOSE,	tr("Close"),	im->icon("fileClose"),	QKeySequence("Ctrl+W"), h, SLOT(docClose())),
		Command(ID_FILE_CLOSE_ALL, tr("Close all"), im->icon("fileCloseAll"), QKeySequence(),	h, SLOT(docCloseAll())),
		Command(ID_EXIT,		tr("Exit"),		im->icon("exit"),		QKeySequence(),			this, SLOT(exit())),
		//
		Command(ID_EDIT_CUT,	tr("Cut"),		im->icon("editCut"),	QKeySequence("Ctrl+X"), h, SLOT(processTheCommand())),
		Command(ID_EDIT_COPY,	tr("Copy"),		im->icon("editCopy"),	QKeySequence("Ctrl+C"), h, SLOT(processTheCommand())),
		Command(ID_EDIT_PASTE,	tr("Paste"),	im->icon("editPaste"),	QKeySequence("Ctrl+V"), h, SLOT(processTheCommand())),
		Command(ID_EDIT_UNDO,	tr("Undo"),		im->icon("editUndo"),	QKeySequence("Ctrl+Z"), h, SLOT(processTheCommand())),
		Command(ID_EDIT_REDO,	tr("Redo"),		im->icon("editRedo"),	QKeySequence("Ctrl+R"), h, SLOT(processTheCommand())),
		Command(ID_FIND,		tr("Find"),		im->icon("find"),		QKeySequence("Ctrl+F"), h, SLOT(processTheCommand())),
		Command(ID_FIND_NEXT,	tr("Find next"), im->icon("findNext"),	QKeySequence("F3"), h, SLOT(processTheCommand())),
		Command(ID_FIND_PREV,	tr("Find previous"), im->icon("findPrev"), QKeySequence("Shift+F3"), h, SLOT(processTheCommand())),
		Command(ID_GOTO_LINE,	tr("Go to line"), im->icon("gotoLine"),	QKeySequence("Ctrl+G"), h, SLOT(processTheCommand())),
		//
		Command(ID_VIEW_SHOW_LINE_NUMBERS,	tr("Show line numbers"),	QIcon(),	QKeySequence("F11"), h, SLOT(processTheCommand())),
		Command(ID_VIEW_WIDTH_ADJUST,		tr("Adjust text by width"),	QIcon(),	QKeySequence("F10"), h, SLOT(processTheCommand())),
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

	if (jInt_->recentFilesMenu_ != 0)
		jInt_->recentFilesMenu_->setIcon(im->icon("fileOpen"));
}

void JuffEd::createMenuBar() {
	menuBar()->clear();

	CommandID fileMenu[] = { ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, ID_FILE_SAVE_AS, 
					ID_FILE_RELOAD, ID_FILE_CLOSE, ID_FILE_CLOSE_ALL, ID_SEPARATOR, 
					ID_SESSION_NEW, ID_SESSION_OPEN, ID_SESSION_SAVE, ID_SESSION_SAVE_AS, 
					ID_SEPARATOR, ID_EXIT, ID_NONE };

	CommandID editMenu[] = { ID_EDIT_CUT, ID_EDIT_COPY, ID_EDIT_PASTE, ID_SEPARATOR, ID_EDIT_UNDO, 
					ID_EDIT_REDO, ID_SEPARATOR, ID_FIND, ID_FIND_NEXT, ID_FIND_PREV, 
					ID_SEPARATOR, ID_GOTO_LINE, ID_NONE };

	CommandID viewMenu[] = { ID_VIEW_SHOW_LINE_NUMBERS, ID_VIEW_WIDTH_ADJUST, ID_NONE };

	CommandID markersMenu[] = { ID_NONE };	

	CommandID toolsMenu[] = { ID_SETTINGS, ID_NONE };

	CommandID helpMenu[] = { ID_ABOUT, ID_ABOUT_QT, ID_NONE };

	CommandID* menu_ids[] = { fileMenu, editMenu, viewMenu, markersMenu, toolsMenu, helpMenu, 0 };
	QString menus[] = { tr("&File"), tr("&Edit"), tr("&View"), tr("&Markers"), tr("&Tools"), tr("&Help"), "" };


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

	//	charset menu
	jInt_->charsetsMenu_ = new QMenu(tr("Charset"));
	QMenu* vMenu = jInt_->mainMenuItems_.value(tr("&View"), 0);
	if (vMenu != 0) {
		vMenu->addSeparator();
		vMenu->addMenu(jInt_->charsetsMenu_);
	}
	initCharsetsMenu();

	//	lexers menu
	jInt_->syntaxMenu_ = new QMenu(tr("Syntax"));
	if (vMenu != 0) {
		vMenu->addMenu(jInt_->syntaxMenu_);
		QStringList sList;
		LexerStorage::instance()->getLexersList(sList);
		foreach (QString s, sList) {
			QAction* a = jInt_->syntaxMenu_->addAction(s, this, SLOT(syntaxSelected()));
			a->setCheckable(true);
			jInt_->syntaxActions_[s] = a;
		}	
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
	QAction* openAct = CommandStorage::instance()->action(ID_FILE_SAVE);
	if (fMenu != 0 && openAct != 0) {
		fMenu->insertMenu(openAct, jInt_->recentFilesMenu_);
	}
	initRecentFilesMenu();
}

void JuffEd::createToolBar() {
	CommandID actions[] = {ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, ID_SEPARATOR, 
					ID_EDIT_CUT, ID_EDIT_COPY, ID_EDIT_PASTE, ID_SEPARATOR, 
					ID_EDIT_UNDO, ID_EDIT_REDO, ID_SEPARATOR, ID_FIND, ID_NONE};
	jInt_->toolBar_ = addToolBar("Main");
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
	

void JuffEd::changeCurrentCharsetAction(QAction* a) {
	if (jInt_->lastCharsetAction_ != 0)
		jInt_->lastCharsetAction_->setChecked(false);

	if (a != 0) {
		jInt_->lastCharsetAction_ = a;
		a->setChecked(true);
	}
}

void JuffEd::changeCurrentSyntaxAction(QAction* a) {
	if (jInt_->lastSyntaxAction_ != 0)
		jInt_->lastSyntaxAction_->setChecked(false);

	if (a != 0) {
		jInt_->lastSyntaxAction_ = a;
		a->setChecked(true);
	}
}

void JuffEd::charsetSelected() {
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

void JuffEd::settings() {
	jInt_->settingsDlg_->exec();
}

void JuffEd::setupToolBarStyle() {
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
		switch (iconSize) {
			case 0: jInt_->toolBar_->setIconSize(QSize(16, 16)); break;
			case 1: jInt_->toolBar_->setIconSize(QSize(24, 24)); break;
			case 2: jInt_->toolBar_->setIconSize(QSize(32, 32)); break;
			default: jInt_->toolBar_->setIconSize(QSize(24, 24)); break;
		}
	}
}

TextDoc* JuffEd::getCurrentTextDoc() {
	TextDoc* tDoc = qobject_cast<TextDoc*>(jInt_->handler_->currentDoc());
	return tDoc;
}

void JuffEd::displayCursorPos(int row, int col) {
	QString text(tr("Row: %1, Col: %2"));
	if (row == -1 && col == -1)
		jInt_->cursorPosL_->setText(" ");
	else
		jInt_->cursorPosL_->setText(text.arg(row + 1).arg(col + 1));
}

void JuffEd::displayFileName(const QString& fileName) {
	jInt_->fileNameL_->setText(QString(" %1 ").arg(fileName));
	QString title("JuffEd");
	QString sessName = jInt_->handler_->sessionName();
	if (!sessName.isEmpty()	&& sessName.compare("_empty_session_") != 0)
		title += " - [" + sessName + "]";
	if (!fileName.isEmpty())
		title += " - " + QFileInfo(fileName).fileName();
	setWindowTitle(title);
}

void JuffEd::displayCharset(const QString& charset) {
	jInt_->charsetL_->setText(QString(" %1 ").arg(charset));
	changeCurrentCharsetAction(jInt_->charsetActions_[charset]);
}

void JuffEd::displaySyntax(const QString& syntax) {
	jInt_->syntaxL_->setText(QString(" %1 ").arg(syntax));
	changeCurrentSyntaxAction(jInt_->syntaxActions_[syntax]);
}

void JuffEd::docFileNameChanged(Juff::Document* doc) {
	if (doc != 0 && !doc->isNull()) {
		displayFileName(doc->fileName());
	}
}

void JuffEd::docCloseRequested(QWidget* w) {
	TextDocView* tdView = qobject_cast<TextDocView*>(w);
	if (tdView != 0) {
		Juff::Document* doc = tdView->document();
		if (doc != 0) {
			jInt_->handler_->closeDocument(doc);
		}
	}
}

void JuffEd::docSwitched(QWidget* w) {
	TextDocView* tdView = qobject_cast<TextDocView*>(w);

	if (tdView == 0) {
		displayCursorPos(-1, -1);
		displayFileName("");
		displayCharset("");
		displaySyntax("none");
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
}

void JuffEd::toggleMarker() {
	QWidget* view = jInt_->viewer_->currentView();	
	TextDocView* tdView = qobject_cast<TextDocView*>(view);

	if (tdView != 0) {
		tdView->toggleMarker();
	}
}

void JuffEd::nextMarker() {
	QWidget* view = jInt_->viewer_->currentView();	
	TextDocView* tdView = qobject_cast<TextDocView*>(view);

	if (tdView != 0) {
		tdView->gotoNextMarker();
	}
}

void JuffEd::prevMarker() {
	QWidget* view = jInt_->viewer_->currentView();	
	TextDocView* tdView = qobject_cast<TextDocView*>(view);

	if (tdView != 0) {
		tdView->gotoPrevMarker();
	}
}

void JuffEd::removeAllMarkers() {
	QWidget* view = jInt_->viewer_->currentView();	
	TextDocView* tdView = qobject_cast<TextDocView*>(view);

	if (tdView != 0) {
		tdView->removeAllMarkers();
	}
}

void JuffEd::gotoMarker() {
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
	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void JuffEd::dropEvent(QDropEvent* e) {
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
