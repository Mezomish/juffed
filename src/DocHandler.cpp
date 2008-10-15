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

#include "DocHandler.h"

//	Qt headers
#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QTimer>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>

//	local headers
#ifdef Q_OS_WIN
#include "AppInfo.win.h"
#else
#include "AppInfo.h"
#endif

#include "DocViewer.h"
#include "Log.h"
#include "NullDoc.h"
#include "MainSettings.h"
#include "SessionDlg.h"
#include "SocketListener.h"
#include "TextDocView.h"

//	Application-dependent headers
#include "TextDoc.h"

typedef QList<Juff::Document*> DocList;

class DocHandlerInterior {
public:
	DocHandlerInterior(QObject* parent, bool listen) : viewer_(0), sessionName_(""), curDoc_(0), listener_(0) {
		autoSaveTimer_ = new QTimer(parent);
		parent->connect(autoSaveTimer_, SIGNAL(timeout()), parent, SLOT(autoSaveEvent()));
		
		if (listen) {
			listener_ = new SocketListener(parent);
			parent->connect(listener_, SIGNAL(fileRecieved(const QString&)), SLOT(docOpen(const QString&)));
			parent->connect(listener_, SIGNAL(newFileRequested()), SLOT(docNew()));
			listener_->start();
		}
	}

	~DocHandlerInterior() {
		if (listener_ != 0) {
			listener_->exit();
			listener_->terminate();
			listener_->wait(1000);
			delete listener_;
		}
		delete autoSaveTimer_;
	}
	
	void checkTheTimer() {
		int timerInterval = MainSettings::autoSaveInterval();
		if (timerInterval > 0) {	
			//	autosave is switched on
			if (!autoSaveTimer_->isActive() || autoSaveTimer_->interval() != timerInterval)
				autoSaveTimer_->start(timerInterval * 60 * 1000);
		}
		else {	
			//	autosave is switched off
			if (autoSaveTimer_->isActive())
				autoSaveTimer_->stop();
		}
	}

	//	Removes given document and activates
	//	that one, which was active before
	void removeDoc(Juff::Document* doc) {
		docs_.removeAll(doc);
		doc->extModMonitoringStop();
		if (curDoc_ == doc)
			curDoc_ = 0;
		
		if (MainSettings::closeTabsInOrderOfUse()) {
			int n = docs_.count();
			if (n > 0) {
				viewer_->setCurrentView(docs_[n - 1]->view());
			}
		}
			
		delete doc;
	}
	
	DocList docs_;
	DocViewer* viewer_;
	QString sessionName_;
	QStringList recentFiles_;
	QTimer* autoSaveTimer_;
	Juff::Document* curDoc_;
	SocketListener* listener_;
};


//	just helping function to avoid duplicated code
TextDocView* getView(Juff::Document* doc) {
	JUFFENTRY;

	if (doc == 0 || doc->isNull())
		return 0;
	return qobject_cast<TextDocView*>(doc->view());
}



DocHandler::DocHandler(bool listen) : QObject(), hInt_(0) {
	hInt_ = new DocHandlerInterior(this, listen);

	QString recentFiles = MainSettings::recentFiles();
	if (!recentFiles.isEmpty()) {
		QStringList fileList = recentFiles.split(";");
		int count = fileList.count();
		for(int i = count - 1; i >= 0; --i) {
			const QString& fileName = fileList.at(i);
			addToRecentFiles(fileName);
		}
	}
}

DocHandler::~DocHandler() {
	JUFFDTOR;

	if (hInt_ != 0)
		delete hInt_;
	
	if (TextDoc::count() != 0) {
		Log::debug("Docs were not removed properly");
	}
	Settings::write();

	Log::debug("Bye!\n", true);
}

void DocHandler::autoSaveEvent() {
	foreach (Juff::Document* doc, hInt_->docs_) {
		if (!doc->isNoname() && doc->isModified())
			doc->save();
	}
	if (!currentDoc()->isNull() && currentDoc()->view() != 0)
		currentDoc()->view()->setFocus();
}

void DocHandler::addViewer(DocViewer* viewer) {
	hInt_->viewer_ = viewer;
}

int DocHandler::docCount() const {
	JUFFENTRY;

	return hInt_->docs_.count();
}

Juff::Document* DocHandler::currentDoc() const {
	JUFFENTRY;
	
	if (hInt_->curDoc_ != 0)
		return hInt_->curDoc_;
	else
		return NullDoc::instance();
}

const QString& DocHandler::sessionName() const { 
	JUFFENTRY;

	return hInt_->sessionName_; 
}

QString docTitle(Juff::Document* doc) {
	JUFFENTRY;

	if (doc == 0 || doc->isNull())
		return "";

	QString title;
	if (doc->isNoname()) {
		title = QObject::tr("Noname");
	}
	else {
		title = QFileInfo(doc->fileName()).fileName();
	}
	
	if (doc->isModified())
		title += "*";

	return title;
}

void DocHandler::restoreSession() {
	JUFFENTRY;

	int startupVariant = MainSettings::startupVariant();
	switch (startupVariant) {
	case 1:
		openSession(MainSettings::lastSessionName());
		break;
	
	case 2:
		newSession();
		break;
		
	case 0: 
	default:
		openSession();
	}
}

Juff::Document* DocHandler::newDocument(const QString& fileName) {
	JUFFENTRY;

	Juff::Document* doc = NullDoc::instance();

	if (!fileName.isEmpty()) {
		//	there was a non-empty argument received
		if (!QFileInfo(fileName).isFile()) {
			//	argument is not a file. return NullDoc
			return doc;
		}
		else {
			//	argument is a file. Try to find it among already opened.
			doc = findDocument(fileName);
		}
	}

	if (doc->isNull()) {
		//	the received file wasn't found among opened ones
		QString fName("");

		if (!fileName.isEmpty())
			fName = QFileInfo(fileName).canonicalFilePath();
		
		DocView* docView = createDocView();
		doc = createDocument(fName, docView);
	
		connect(docView, SIGNAL(modified(bool)), SLOT(docModified(bool)));
		connect(doc, SIGNAL(fileNameChanged(const QString&)), SLOT(docFileNameChanged(const QString&)));
		connect(docView, SIGNAL(cursorPositionChanged(int, int)), SIGNAL(cursorPositionChanged(int, int)));

		//	close current document if
		//	1. it is the only opened document AND
		//	2. it is new document (not saved yet) AND
		//	3. it is not modified
		if (hInt_->docs_.count() == 1) {
			Juff::Document* curDoc = currentDoc();
			if (curDoc->isNoname() && !curDoc->isModified()) {
				closeDocument(curDoc);
			}
		}
		
		JUFFDEBUG("ChP 1");
		hInt_->docs_.append(doc);
		JUFFDEBUG("ChP 2");
		hInt_->viewer_->addDocView(docTitle(doc), docView);
		JUFFDEBUG("ChP 3");
		doc->applySettings();
		JUFFDEBUG("ChP 4");
		doc->open();
		JUFFDEBUG("ChP 5");

		hInt_->viewer_->widget()->activateWindow();

		//	Looks like we have segfault here.... :(
		//	emit informational signals
		Log::debug("Emitting informational signal 'docOpened'");
		emit docOpened(doc->fileName());
		Log::debug("Signal 'docOpened' was handled properly");

		Log::debug("Emitting informational signal 'docSwitched'");
		emit docSwitched(doc->fileName());
		Log::debug("Signal 'docSwitched' was handled properly");
		
		if (docCount() == 1)
			hInt_->viewer_->updateCurrentViewInfo();
	}
	return doc;
}

bool DocHandler::closeDocument(Juff::Document* doc) {
	JUFFENTRY;

	if (doc == 0 || doc->isNull())
		return false;

	QString fName = doc->fileName();
	if (doc->isModified()) {
		//	TODO :	Possible change just to bool
		Juff::Document::SaveRequest saveRequest = doc->confirmForClose();
		if (saveRequest == Juff::Document::SaveYes) {
			doc->save();
		}
		else if (saveRequest != Juff::Document::SaveNo) {
			return false;
		}
	}
	
	hInt_->removeDoc(doc);

	//	Looks like we have segfault here.... :(
	//	emit informational signal
	Log::debug("Emitting informational signal 'docClosed'");
	emit docClosed(fName);
	Log::debug("Signal 'docClosed' was handled properly");

	return true;
}

bool DocHandler::closeAllDocs() {
	JUFFENTRY;

	if (MainSettings::saveSessionOnClose())
		saveSession(hInt_->sessionName_);

	foreach (Juff::Document* doc, hInt_->docs_) {
		if (!closeDocument(doc)) {
			return false;
		}
	}

	return true;
}

Juff::Document* DocHandler::findDocument(const QString& fileName) const {
	JUFFENTRY;

	Juff::Document* doc = NullDoc::instance();
	if (!fileName.isEmpty()) {
		foreach (Juff::Document* d, hInt_->docs_) {
			if (d->fileName().compare(fileName) == 0) {
				doc = d;
				break;
			}
		}
	}
	return doc;
}

void DocHandler::docModified(bool) {
	JUFFENTRY;

	TextDocView* tdView = qobject_cast<TextDocView*>(sender());
	if (tdView != 0) {
		hInt_->viewer_->setDocViewTitle(tdView, docTitle(tdView->document()));
		
		//	Looks like we have segfault here.... :(
		//	emit informational signal
		Log::debug("Emitting informational signal 'docModified'");
		emit docModified(tdView->document()->fileName(), tdView->document()->isModified());
		Log::debug("Signal 'docModified' was handled properly");
	}
}

void DocHandler::docFileNameChanged(const QString& oldName) {
	JUFFENTRY;

	Juff::Document* doc = qobject_cast<Juff::Document*>(sender());
	if (doc != 0) {
		hInt_->viewer_->setDocViewTitle(doc->view(), docTitle(doc));
		emit fileNameChanged(doc);
		
		//	Looks like we have segfault here.... :(
		//	emit informational signal
		Log::debug("Emitting informational signal 'docFileNameChanged'");
		emit docFileNameChanged(oldName, doc->fileName());
		Log::debug("Signal 'docFileNameChanged' was handled properly");
	}
}

void DocHandler::applySettings() {
	JUFFENTRY;

	hInt_->checkTheTimer();
	
	foreach (Juff::Document* doc, hInt_->docs_) {
		doc->applySettings();
	}
}

void DocHandler::docActivated(Juff::Document* doc) {
	JUFFENTRY;

	if (doc == 0)
		return;

	if (hInt_->curDoc_ != 0)
		hInt_->curDoc_->extModMonitoringStop();
	
	hInt_->docs_.removeAll(doc);
	hInt_->docs_.append(doc);

	hInt_->curDoc_ = doc;
	doc->extModMonitoringStart();
	
	//	Looks like we have segfault here.... :(
	//	emit informational signal
	Log::debug("Emitting informational signal 'docSwitched'");
	emit docSwitched(doc->fileName());
	Log::debug("Signal 'docSwitched' was handled properly");
}


////////////////////////////////////////////////////////////
//	PUBLIC SLOTS
////////////////////////////////////////////////////////////
void DocHandler::docNew() {
	JUFFENTRY;

	newDocument("");
}

void DocHandler::docOpen(const QString& name/*= ""*/) {
	JUFFENTRY;

	//
	//	TODO :	Move everything into the TextDoc
	//
	
	if (QFileInfo(name).isDir())
		return;

	QStringList files;
	if (name.isEmpty()) {
		QString fileTypes("All files (*)");
		fileTypes += ";;Batch files (*.bat)";
		fileTypes += ";;Shell scripts (*.sh *.run)";
		fileTypes += ";;C/C++ files (*.c *.cpp *.cxx)";
		fileTypes += ";;C# (*.cs)";
		fileTypes += ";;D (*.d)";
		fileTypes += ";;Diff/Patch files (*.diff *.patch)";
		fileTypes += ";;Header files (*.h *.hpp)";
		fileTypes += ";;HTML/CSS/JS (*.htm *.html *.xhtml *.dhtml *.css *js)";
		fileTypes += ";;IDL (*.idl)";
		fileTypes += ";;Lua (*.lua)";
		fileTypes += ";;Makefiles (*Makefile*)";
		fileTypes += ";;Perl (*.pl)";
		fileTypes += ";;PHP (*.php)";
		fileTypes += ";;Python (*.py)";
		fileTypes += ";;Ruby (*.rb)";
		fileTypes += ";;SQL (*.sql)";
		fileTypes += ";;XML (*.xml)";

		QString startDir("");
		QString curDocFileName("");
		JUFFDEBUG("ChP 1");
		Juff::Document* curDoc = currentDoc();
		if (curDoc != 0 && !curDoc->isNull())
			curDocFileName = curDoc->fileName();

		JUFFDEBUG("ChP 2");
		if (MainSettings::syncOpenDialogToCurDoc() && QFileInfo(curDocFileName).exists()) {
			startDir = QFileInfo(curDocFileName).absolutePath();
		}
		else {
			startDir = MainSettings::lastOpenDir();
		}
		JUFFDEBUG("ChP 3");

		files = QFileDialog::getOpenFileNames(hInt_->viewer_->widget(), 
				tr("Open file"), startDir, fileTypes);
	
		JUFFDEBUG("ChP 4");
		if (! files.isEmpty()) {
			QString file;
		
			foreach (file, files) {
				Juff::Document* doc = findDocument(file);
				if (doc->isNull()) {
					if (!newDocument(file)->isNull()) {
						addToRecentFiles(file);
					}
				}
				else {
					hInt_->viewer_->setCurrentView(doc->view());
					hInt_->viewer_->widget()->activateWindow();
				}
			}

			MainSettings::setLastOpenDir(QFileInfo(file).absolutePath());
		}
	}
	else {
		Juff::Document* doc = findDocument(name);
		if (doc->isNull()) {
			if ( !newDocument(name)->isNull() ) {
				addToRecentFiles(name);
				MainSettings::setLastOpenDir(QFileInfo(name).absolutePath());
			}
		}
		else {
			hInt_->viewer_->setCurrentView(doc->view());
			hInt_->viewer_->widget()->activateWindow();
		}
	}
}

void DocHandler::docSave(const QString& fileName) {
	JUFFENTRY;

	Juff::Document* doc = NullDoc::instance();

	if (fileName.isEmpty())
		doc = currentDoc();
	else
		doc = findDocument(fileName);

	if (!doc->isNull()) {
		doc->save();
	}
}

void DocHandler::docSaveAs() {
	JUFFENTRY;

	Juff::Document* doc = currentDoc();
	if (!doc->isNull()) {
		doc->saveAs();
	}
}

void DocHandler::docReload() {
	JUFFENTRY;

	Juff::Document* doc = currentDoc();
	if (doc->isNull())
		return;

	doc->reload();
}

void DocHandler::docPrint() {
	JUFFENTRY;

	Juff::Document* doc = currentDoc();
	if (doc->isNull())
		return;

	doc->print();
}

void DocHandler::docPrintSelected() {
	JUFFENTRY;

	Juff::Document* doc = currentDoc();
	if (doc->isNull())
		return;

	doc->printSelected();
}

void DocHandler::docClose(const QString& fileName) {
	JUFFENTRY;

	if (fileName.isEmpty()) {
		closeDocument(currentDoc());
	}
	else {
		closeDocument(findDocument(fileName));
	}
	
	//	Need to be called, because if index of current view doesn't 
	//	change or the closed doc was the last one, DocViewer's signal 
	//	docViewChanged(QWidget*) will not be called.
	//	In other cases signal docViewChanged(QWidget*) will be emitted
	//	twice (not very good but let it be).
	hInt_->viewer_->updateCurrentViewInfo();
}

void DocHandler::docCloseAll() {
	JUFFENTRY;

	closeAllDocs();
}

void DocHandler::nextDoc() {
}

void DocHandler::prevDoc() {
}

void DocHandler::processTheCommand() {
	JUFFENTRY;

	QAction* a = qobject_cast<QAction*>(sender());
	if (a != 0) {
		Juff::Document* doc = currentDoc();

		CommandID id = CommandID(a->data().toInt());

		switch (id) {
		
		case ID_DOC_NEXT:
			hInt_->viewer_->nextView();
			break;
			
		case ID_DOC_PREV:
			hInt_->viewer_->prevView();
			break;
		
		case ID_SESSION_NEW: {
			newSession();
			break;
		}

		case ID_SESSION_OPEN: {
			openSession();
			break;
		}

		case ID_SESSION_SAVE: {
			if (hInt_->sessionName_.isEmpty())
				saveSessionAs();
			else
				saveSession(hInt_->sessionName_);
			break;
		}

		case ID_SESSION_SAVE_AS: {
			saveSessionAs();
			break;
		}

		default:
			if (doc != 0 && !doc->isNull())
				doc->processTheCommand(id);
			break;
		}
	}
}
//	PUBLIC SLOTS
////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
//	SESSIONS
void DocHandler::newSession() {
	JUFFENTRY;

	if (closeAllDocs()) {
		hInt_->sessionName_ = "_empty_session_";
		docNew();
	}
}

void DocHandler::openSession(const QString& session) {
	JUFFENTRY;

	QString name(session);
	
	if (name.isEmpty()) {
		//	if no parameters were given (or empty parameter)
		SessionDlg dlg(hInt_->viewer_->widget());
		int sCount = dlg.sessionCount();
		if (sCount > 0) {
			dlg.exec();
			int res = dlg.result();

			if (res == 0 || !closeAllDocs())
				return;		//	session opening canceled or closing all docs interrupted

			if (dlg.result() == 1) {
				//	open session
				name = dlg.curSessionName();
			}
		}
		//	if session count == 0 then new session will be opened (see the code below)
	}

	hInt_->sessionName_ = name;

	if (name.isEmpty())
		newSession();
	else {
		Juff::Document* doc = 0;
		QFile sess(AppInfo::configDir() + "/sessions/" + name);
		if (sess.open(QIODevice::ReadOnly)) {
			QString fileName("");
			while (!sess.atEnd()) {
				QString lineStr = sess.readLine().simplified();
				fileName = lineStr.section(':', 0, -3);
				int scrPos = lineStr.section(':', -2, -2).toInt();
				int line = lineStr.section(':', -1, -1).toInt();
				if (!fileName.isEmpty()) {
					doc = newDocument(fileName);
					if (doc != 0 && !doc->isNull()) {
						TextDocView* tdView = qobject_cast<TextDocView*>(doc->view());
						if (tdView != 0) {
							tdView->setCursorPos(line, 0);
							tdView->setScrollPos(scrPos);
						}
					}
				}
			}
	
			sess.close();
		}
		
		//	Need to update the window title. 
		//	Call the update with the last created document
		emit fileNameChanged(doc);
	}
}

void DocHandler::saveSession(const QString& name) {
	JUFFENTRY;

	if (name.isEmpty())
		return;

	hInt_->sessionName_ = name;
	
	QFile sess(AppInfo::configDir() + "/sessions/" + name);
	if (sess.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		Juff::Document* doc = 0;
		QWidgetList wList;
		hInt_->viewer_->getViewsOrder(wList);
		foreach (QWidget* w, wList) {
			TextDocView* view = qobject_cast<TextDocView*>(w);
			if (view != 0) {
				int scrPos = view->scrollPos();
				int line, col;
				view->getCursorPos(line, col);
				sess.write(QString("%1:%2:%3\n")
					.arg(view->document()->fileName()).arg(scrPos).arg(line).toLocal8Bit());
			}
		}
		sess.close();

		//	Need to update the window title. 
		//	Call the update with the last created document
		emit fileNameChanged(doc);
	}
	else {
		//	TODO :	Add error display here
	}
}

void DocHandler::saveSessionAs() {
	JUFFENTRY;

	QString name = QInputDialog::getText(hInt_->viewer_->widget(), tr("Save session as"), tr("Session name"), QLineEdit::Normal, hInt_->sessionName_);
	if (!name.isEmpty())
		saveSession(name);

	//	Need to update the window title. 
	//	Call the update with the last created document
	emit fileNameChanged(currentDoc());
}
//	SESSIONS
//////////////////////////////////////////////////////////////////////

void DocHandler::docOpenRecent() {
	JUFFENTRY;

	QAction* a = qobject_cast<QAction*>(sender());
	if (a == NULL)
		return;
	
	QString fileName = a->text();
	if (!fileName.isEmpty()) {
		docOpen(fileName);
	}
}

void DocHandler::addToRecentFiles(const QString& fileName) {
	JUFFENTRY;

	hInt_->recentFiles_.removeAll(fileName);
	hInt_->recentFiles_.push_front(fileName);
	if (hInt_->recentFiles_.count() > MainSettings::recentFilesCount())
		hInt_->recentFiles_.removeLast();

	MainSettings::setRecentFiles(hInt_->recentFiles_.join(";"));
	emit recentFileAdded();
}

void DocHandler::getRecentFiles(QStringList& list) const {
	JUFFENTRY;

	list = hInt_->recentFiles_;
}


////////////////////////////////////////////////////////////
//	Application-dependent functions
////////////////////////////////////////////////////////////
Juff::Document* DocHandler::createDocument(const QString& fileName, DocView* view) {
	JUFFENTRY;

	return new TextDoc(fileName, view);
}

DocView* DocHandler::createDocView() {
	JUFFENTRY;

	return new TextDocView(0);
}
//	Application-dependent functions
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//	Plugins
////////////////////////////////////////////////////////////

void DocHandler::getText(QString& text) const {
	JUFFENTRY;

	text = "";
	TextDocView* tdView = getView(currentDoc());
	if (tdView != 0)
		tdView->getText(text);
}

void DocHandler::getText(const QString& fileName, QString& text) const {
	JUFFENTRY;

	text = "";
	TextDocView* tdView = getView(findDocument(fileName));
	if (tdView != 0) {
		tdView->getText(text);
	}
}

void DocHandler::getCurrentLine(QString& line) {
	JUFFENTRY;

	line = "";
	TextDocView* tdView = getView(currentDoc());
	if (tdView != 0)
		tdView->getCurrentLine(line);
}

void DocHandler::getCursorPos(int& line, int& col) const {
	JUFFENTRY;

	line = col = -1;
	TextDocView* tdView = getView(currentDoc());
	if (tdView != 0)
		tdView->getCursorPos(line, col);
}

void DocHandler::getSelection(int&, int&, int&, int&) const {
	Log::debug("DocHandler::getSelection: not implemented yet");
}

void DocHandler::getSelectedText(QString& text) const {
	JUFFENTRY;

	text = "";
	TextDocView* tdView = getView(currentDoc());
	if (tdView != 0)
		tdView->getSelectedText(text);
}

void DocHandler::replaceSelectedText(const QString& text) {
	JUFFENTRY;

	TextDocView* tdView = getView(currentDoc());
	if (tdView != 0) {
		tdView->replaceSelectedText(text);
	}
}

void DocHandler::setCursorPos(int line, int col) {
	JUFFENTRY;

	TextDocView* tdView = getView(currentDoc());
	if (tdView != 0)
		tdView->setCursorPos(line, col);
}

void DocHandler::insertText(const QString& text) {
	JUFFENTRY;

	TextDocView* tdView = getView(currentDoc());
	if (tdView != 0)
		tdView->insertText(text);
}

void DocHandler::getDocumentList(QStringList& list) {
	list.clear();
	foreach(Juff::Document* doc, hInt_->docs_) {
		if (doc != 0 && !doc->isNull())
			list << doc->fileName();
	}
}

void DocHandler::getCurrentFileName(QString& name) {
	Juff::Document* doc = currentDoc();
	if (doc != 0 && !doc->isNull())
		name = doc->fileName();
	else
		name = "";
}

//	Plugins
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
/////	Tests
////////////////////////////////////////////////////////////
bool DocHandler::testAdd10NewDocs() {
	closeAllDocs();

	if (docCount() != 0)
		return false;

	for (int i = 0; i < 10; ++i) {
		newDocument(QString("Doc%1").arg(i));
	}
	
	if (docCount() != 10)
		return false;

	return true;
}

bool DocHandler::testAddExistingDoc() {
	if (!testAdd10NewDocs())
		return false;

	newDocument(QString("Doc4"));
	
	if (docCount() != 10)
		return false;

	return true;
}

bool DocHandler::testRemoveOneDoc() {
	Juff::Document* doc = newDocument("SomeDoc");
	int count = docCount();

	if (doc->isNull())
		return false;

	if (closeDocument(doc)) {
	}

	if (docCount() != count - 1)
		return false;

	return true;
}

bool DocHandler::runAllTests() {
	if (!testAdd10NewDocs()) {
		Log::debug("testAdd10NewDocs() failed");
		return false;
	}
	if (!testAddExistingDoc()) {
		Log::debug("testAddExistingDoc() failed");
		return false;
	}
	if (!testRemoveOneDoc()) {
		Log::debug("testRemoveOneDoc() failed");
		return false;
	}

	closeAllDocs();
	return true;
}

/////	Tests
////////////////////////////////////////////////////////////
