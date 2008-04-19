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

#ifndef _DOC_HANDLER_H_
#define _DOC_HANDLER_H_

class DocHandlerInterior;
class DocView;
class DocViewer;

//	Qt headers
#include <QtCore/QObject>
#include <QtCore/QString>

//	local headers
#include "Document.h"

class DocHandler : public QObject {
Q_OBJECT
public:
	DocHandler(bool listen = true);
	~DocHandler();

	void addViewer(DocViewer*);
	bool closeDocument(Juff::Document*);
	bool closeAllDocs();
	void applySettings();
	Juff::Document* currentDoc();
	int docCount() const;
	const QString& sessionName() const;
	void restoreSession();
	void getRecentFiles(QStringList& list) const;
	
public slots:
	void docNew();
	void docOpen(const QString& = "");
	void docOpenRecent();
	void docSave();
	void docSaveAs();
	void docReload();
	void docClose();
	void docCloseAll();
	void processTheCommand();

	void newSession();
	void openSession(const QString& session = "");
	void saveSession(const QString&);
	void saveSessionAs();

	void docActivated(Juff::Document*);
	
protected slots:
	void docModified(bool);
	void docFileNameChanged();
	void autoSaveEvent();
	
signals:
	void fileNameChanged(Juff::Document*);
	void recentFileAdded();
	void cursorPositionChanged(int, int);
	
private:
	Juff::Document* newDocument(const QString& fileName = "");
	Juff::Document* findDocument(const QString&);
	void addToRecentFiles(const QString&);

	void nextDoc();
	void prevDoc();
	
	/////	Tests
	bool testAdd10NewDocs();
	bool testAddExistingDoc();
	bool testRemoveOneDoc();

public:	
	bool runAllTests();
	/////	Tests
	
protected:
	//
	//	TODO :	Make the following methods pure virtual,
	//			inherit DocHandler with TextDocHandler
	//			and reimplement these methods there
	//
	
	/**
	 * Application-dependent function. 
	 * Creates the document of that type,
	 * which is used in this app
	 */
	Juff::Document* createDocument(const QString&, DocView*);
	/**
	 * Application-dependent function. 
	 * Creates the document view of that type,
	 * which is used in this app
	 */
	DocView* createDocView();
	
private:
	DocHandlerInterior* hInt_;
};

#endif
