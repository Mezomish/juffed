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

#ifndef _JUFF_MANAGER_H_
#define _JUFF_MANAGER_H_

#include <QtCore/QFile>
#include <QtCore/QObject>
#include <QtCore/QString>

#include "Juff.h"
#include "ManagerInterface.h"
#include "Parameter.h"

namespace Juff {

class DocHandler;
class Document;
	
namespace GUI {
	class GUI;
}

class Manager : public QObject, public ManagerInterface {
Q_OBJECT
public:
	Manager(GUI::GUI*);
	virtual ~Manager();

	void addDocHandler(DocHandler* handler);


	virtual void getDocList(QStringList&) const;
	virtual void getCurDocName(QString&) const;
	virtual void getDocText(const QString&, QString&);
	virtual void getCursorPos(int&, int&);
	virtual void getSelection(int&, int&, int&, int&);
	virtual void getSelectedText(QString&);

	virtual void setCursorPos(int, int);
	virtual void setSelection(int, int, int, int);
	virtual void removeSelectedText();
	virtual void insertText(const QString&);

	virtual void activateDoc(const QString&);
	virtual void closeDoc(const QString&);

	void restoreSession();
	
public slots:
	/**
	*	Opens a new doc or activates already opened one
	*/
	void openDoc(const QString& fileName);


private slots:
	//	slots called by menu/toolbar actions
	void fileNew();
	void fileNewRich();
	void fileOpen();
	bool fileSave();
	bool fileSaveAs();
	void fileReload();
	bool fileClose();
	void fileCloseAll();
	void fileRecent();
	void filePrint();
	void exit();

	void sessionNew();
	void sessionOpen();
	void sessionSave();
	void sessionSaveAs();

	void editUndo();
	void editRedo();
	void editCut();
	void editCopy();
	void editPaste();

	void find();
	void findNext();
	void findPrev();
	void replace();
	void gotoLine();
	void charsetSelected();

	//	slots called by event signals
	void onCurDocChanged(QWidget*);

	void docModified(bool);
	void docFileNameChanged(const QString&);
	void applySettings();
	void onCloseEvent(bool&);
	
	Document* curDoc() const;

private:
	/**
	*	Creates a doc of given type and adds it to the storage and to viewer
	*/
	void createDoc(const QString& type, const QString& fileName);

	/**
	*	Saves given doc with a given file name
	*/
	bool saveDoc(Document* doc, const QString& fileName);

	/**
	*/
	bool closeSess();
	bool openSess(const QString&);
	bool saveSess(const QString&);
	void writePanelViews(QFile& file, int panel);

	bool confirmExit();

	/**
	*	Closes given doc
	*/
	void closeDoc(Document*);

	/**
	*/
	bool closeAllDocs();

	/**
	*/
	void findImpl(bool replace);

	/**
	*/
	void initCharsetMenu();
	
	/**
	*/
	void initRecentFilesMenu();
	
	class Interior;
	Interior* mInt_;
};


}

#endif
