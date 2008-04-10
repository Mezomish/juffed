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
//#include <QtCore/QVector>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>

//	local headers
#include "AppInfo.h"
#include "DocViewer.h"
#include "Log.h"
#include "NullDoc.h"
#include "MainSettings.h"
#include "SessionDlg.h"
#include "SocketListener.h"
#include "TextDocView.h"

//	Application-dependent headers
#include "TextDoc.h"

typedef QList<Document*> DocList;
//typedef QVector<Document*> DocVector;

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
	void removeDoc(Document* doc) {
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
	Document* curDoc_;
	SocketListener* listener_;
};

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
	if (hInt_ != 0)
		delete hInt_;
	
	if (TextDoc::count() != 0) {
		Log::debug("Docs were not removed properly");
	}
	Settings::write();
}

void DocHandler::autoSaveEvent() {
	foreach (Document* doc, hInt_->docs_) {
		if (!doc->fileName().isEmpty() && doc->isModified())
			doc->save();
	}
	if (!currentDoc()->isNull() && currentDoc()->view() != 0)
		currentDoc()->view()->setFocus();
}

void DocHandler::addViewer(DocViewer* viewer) {
	hInt_->viewer_ = viewer;
}

int DocHandler::docCount() const {
	return hInt_->docs_.count();
}

Document* DocHandler::currentDoc() {
	return hInt_->curDoc_;
}

const QString& DocHandler::sessionName() const { 
	return hInt_->sessionName_; 
}

QString docTitle(Document* doc) {
	QString fileName = doc->fileName();
	
	if (fileName.isEmpty()) {
		fileName = QObject::tr("Noname");
	}
	else {
		fileName = QFileInfo(fileName).fileName();
	}
	
	if (doc->isModified())
		fileName += "*";

	return fileName;
}

void DocHandler::restoreSession() {
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

Document* DocHandler::newDocument(const QString& fileName) {
	Document* doc = NullDoc::instance();
	
	if (!fileName.isEmpty()) {
		if (!QFileInfo(fileName).isFile())
			return doc;
		else
			doc = findDocument(fileName);
	}

	if (doc->isNull()) {

		DocView* docView = createDocView();
		doc = createDocument(fileName, docView);
	
		connect(docView, SIGNAL(modified(bool)), SLOT(docModified(bool)));
		connect(doc, SIGNAL(fileNameChanged()), SLOT(docFileNameChanged()));
		connect(docView, SIGNAL(cursorPositionChanged(int, int)), SIGNAL(cursorPositionChanged(int, int)));

		//	close current document if
		//	1. it is the only opened document AND
		//	2. it is new document (not saved yet) AND
		//	3. it is not modified
		if (hInt_->docs_.count() == 1) {
			Document* curDoc = currentDoc();
			if (curDoc->fileName().isEmpty() && !curDoc->isModified()) {
				closeDocument(curDoc);
			}
		}
		
		hInt_->docs_.append(doc);
		hInt_->viewer_->addDocView(docTitle(doc), docView);

		doc->applySettings();
		doc->open();

		hInt_->viewer_->widget()->activateWindow();
		
		if (docCount() == 1)
			hInt_->viewer_->updateCurrentViewInfo();
	}
	return doc;
}

bool DocHandler::closeDocument(Document* doc) {
	if (doc == 0 || doc->isNull())
		return false;

	if (doc->isModified()) {
		//	TODO :	Possible change just to bool
		Document::SaveRequest saveRequest = doc->confirmForClose();
		if (saveRequest == Document::SaveYes) {
			doc->save();
			hInt_->removeDoc(doc);
			return true;
		}
		else if (saveRequest == Document::SaveNo) {
			hInt_->removeDoc(doc);
			return true;
		}
		else
			return false;
	}
	else {
		hInt_->removeDoc(doc);
		return true;
	}
}

bool DocHandler::closeAllDocs() {
	if (MainSettings::saveSessionOnClose())
		saveSession(hInt_->sessionName_);

	foreach (Document* doc, hInt_->docs_) {
		if (!closeDocument(doc)) {
			return false;
		}
	}

	return true;
}

Document* DocHandler::findDocument(const QString& fileName) {
	Document* doc = NullDoc::instance();
	if (!fileName.isEmpty()) {
		foreach (Document* d, hInt_->docs_) {
			if (d->fileName().compare(fileName) == 0) {
				doc = d;
				break;
			}
		}
	}
	return doc;
}

void DocHandler::docModified(bool) {
	TextDocView* tdView = qobject_cast<TextDocView*>(sender());
	if (tdView != 0) {
		hInt_->viewer_->setDocViewTitle(tdView, docTitle(tdView->document()));
	}
}

void DocHandler::docFileNameChanged() {
	Document* doc = qobject_cast<Document*>(sender());
	if (doc != 0) {
		hInt_->viewer_->setDocViewTitle(doc->view(), docTitle(doc));
		emit fileNameChanged(doc);
	}
}

void DocHandler::applySettings() {
	hInt_->checkTheTimer();
	
	foreach (Document* doc, hInt_->docs_) {
		doc->applySettings();
	}
}

void DocHandler::docActivated(Document* doc) {
	if (doc == 0)
		return;

	if (hInt_->curDoc_ != 0)
		hInt_->curDoc_->extModMonitoringStop();
	
	hInt_->docs_.removeAll(doc);
	hInt_->docs_.append(doc);

	hInt_->curDoc_ = doc;
	doc->extModMonitoringStart();
}


////////////////////////////////////////////////////////////
//	PUBLIC SLOTS
////////////////////////////////////////////////////////////
void DocHandler::docNew() {
	newDocument("");
}

void DocHandler::docOpen(const QString& name/*= ""*/) {
	//
	//	TODO :	Move everything into the TextDoc
	//
	
	QStringList files;
	if (name.isEmpty()) {
		QString fileTypes("All files (*)");
		fileTypes += ";;C/C++ files (*.c *.cpp *.cxx *.CPP *.CXX)";
		fileTypes += ";;Header files (*.h *.hpp *.H *.HPP)";
		fileTypes += ";;Python (*.py)";
		fileTypes += ";;HTML (*.htm *.html *.xhtml)";
		fileTypes += ";;PHP (*.php)";
		fileTypes += ";;XML (*.xml *.ui)";
//		fileTypes += "";
//		fileTypes += "";
//		fileTypes += "";
		files = QFileDialog::getOpenFileNames(hInt_->viewer_->widget(), 
				tr("Open file"), MainSettings::lastOpenDir(), fileTypes);
	
		if (! files.isEmpty()) {
			QString file;
		
			foreach (file, files) {
				Document* doc = findDocument(file);
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
		Document* doc = findDocument(name);
		if (doc->isNull()) {
			if (!newDocument(name)->isNull()) {
				addToRecentFiles(name);
			}
		}
		else {
			hInt_->viewer_->setCurrentView(doc->view());
			hInt_->viewer_->widget()->activateWindow();
		}
	}
}

void DocHandler::docSave() {
	Document* doc = currentDoc();
	if (!doc->isNull()) {
		doc->save();
	}
}

void DocHandler::docSaveAs() {
	Document* doc = currentDoc();
	if (!doc->isNull()) {
		doc->saveAs();
	}
}

void DocHandler::docReload() {
	Document* doc = currentDoc();
	if (doc->isNull())
		return;

	doc->reload();
}

void DocHandler::docClose() {
	closeDocument(currentDoc());
	
	//	Need to be called, because if index of current view doesn't 
	//	change or the closed doc was the last one, DocViewer's signal 
	//	docViewChanged(QWidget*) will not be called.
	//	In other cases signal docViewChanged(QWidget*) will be emitted
	//	twice (not very good but let it be).
	hInt_->viewer_->updateCurrentViewInfo();
}

void DocHandler::docCloseAll() {
	closeAllDocs();
}

void DocHandler::nextDoc() {
}

void DocHandler::prevDoc() {
}

void DocHandler::processTheCommand() {
	QAction* a = qobject_cast<QAction*>(sender());
	if (a != 0) {
		Document* doc = currentDoc();

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
			if (!doc->isNull())
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
	if (closeAllDocs()) {
		hInt_->sessionName_ = "";
		docNew();
	}
}

void DocHandler::openSession(const QString& session) {
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
		Document* doc = 0;
		QFile sess(AppInfo::configDir() + "/sessions/" + name);
		if (sess.open(QIODevice::ReadOnly)) {
			QString fileName("");
			while (!sess.atEnd()) {
				fileName = sess.readLine().simplified();
				if (!fileName.isEmpty()) {
					doc = newDocument(fileName);
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
	if (name.isEmpty())
		return;

	hInt_->sessionName_ = name;
	
	QFile sess(AppInfo::configDir() + "/sessions/" + name);
	if (sess.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		Document* doc = 0;
		QWidgetList wList;
		hInt_->viewer_->getViewsOrder(wList);
		foreach (QWidget* w, wList) {
			DocView* view = qobject_cast<DocView*>(w);
			if (view != 0) {
				sess.write(view->document()->fileName().toLocal8Bit() + "\n");
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
	QAction* a = qobject_cast<QAction*>(sender());
	if (a == NULL)
		return;
	
	QString fileName = a->text();
	if (!fileName.isEmpty()) {
		docOpen(fileName);
	}
}

void DocHandler::addToRecentFiles(const QString& fileName) {
	hInt_->recentFiles_.removeAll(fileName);
	hInt_->recentFiles_.push_front(fileName);
	if (hInt_->recentFiles_.count() > MainSettings::recentFilesCount())
		hInt_->recentFiles_.removeLast();

	MainSettings::setRecentFiles(hInt_->recentFiles_.join(";"));
	emit recentFileAdded();
}

void DocHandler::getRecentFiles(QStringList& list) const {
	list = hInt_->recentFiles_;
}


////////////////////////////////////////////////////////////
//	Application-dependent functions
////////////////////////////////////////////////////////////
Document* DocHandler::createDocument(const QString& fileName, DocView* view) {
	return new TextDoc(fileName, view);
}

DocView* DocHandler::createDocView() {
	return new TextDocView(0);
}
//	Application-dependent functions
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
	Document* doc = newDocument("SomeDoc");
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
