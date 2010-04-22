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

#ifndef __JUFFED_JUFFED_H__
#define __JUFFED_JUFFED_H__

class QActionGroup;
class QMenu;
class SettingsDlg;
class SearchEngine;

namespace Juff {
	class Project;
	class StatusLabel;
}


#include <QMap>

#include "Enums.h"
#include "JuffMW.h"
#include "DocViewer.h"
#include "DocManager.h"
#include "DocHandlerInt.h"
#include "PluginManager.h"
#include "PluginNotifier.h"


class JuffEd : public Juff::PluginNotifier, public Juff::DocHandlerInt {
Q_OBJECT
public:
	JuffEd();
	virtual ~JuffEd();

	QWidget* mainWindow() const;

	// implementation of DocHandlerInt interface
	virtual Juff::Document* curDoc() const;
	virtual Juff::Document* getDoc(const QString&) const;
	virtual Juff::Project* curPrj() const;
	virtual void openDoc(const QString&);
	virtual void closeDoc(const QString&);
	virtual void saveDoc(const QString&);
	virtual int docCount() const;
	virtual QStringList docList() const;

public slots:
	void slotFileNew();
	void slotFileOpen();
	void slotFileSave();
	void slotFileSaveAs();
	void slotFileSaveAll();
	void slotFileReload();
	void slotFileRename();
	void slotFileClose();
	void slotFileCloseAll();
	void slotFilePrint();
	void slotFileExit();


	void slotPrjNew();
	void slotPrjOpen();
	void slotPrjRename();
	void slotPrjClose();
	void slotPrjSaveAs();
	void slotPrjAddFile();

	void slotEditUndo();
	void slotEditRedo();
	void slotEditCut();
	void slotEditCopy();
	void slotEditPaste();

	void slotFind();
	void slotFindNext();
	void slotFindPrev();
	void slotReplace();
	void slotGotoLine();
	void slotJumpToFile();
	
	void slotWrapWords();
	void slotShowLineNumbers();
	void slotShowWhitespaces();
	void slotShowLineEndings();
	void slotZoomIn();
	void slotZoomOut();
	void slotZoom100();
	void slotFullscreen();
	
	void slotOpenWithCharset();
	void slotSetCharset();
	
	void slotSettings();
	
	// single application slot
	void onMessageReceived(const QString&);
	
private slots:
	void onDocModified(bool);
	void onDocCursorMoved(int, int);
	void onDocTextChanged();
	void onDocSyntaxChanged(const QString&);
	void onDocLineCountChanged(int);
	void onDocActivated(Juff::Document*);
	void onDocRenamed(const QString&);
	void onPrjFileAdded(const QString&);
	void onPrjFileRemoved(const QString&);
	
	void onCloseRequested(bool&);
	void onDocCloseRequested(Juff::Document*, bool&);
	void onDocOpenRequested(const QString&);
	void onSettingsApplied();

	void initRecentFilesMenu();

private:
	/**
	* This methods opens or activates a doc with given file 
	* name or creates a new empty doc if \param fileName is empty.
	*/
	Juff::Document* createDoc(const QString& fileName);

	void createProject(const QString& fileName);
	bool closeProject();

	QString projectName() const;
	void initDoc(Juff::Document*);
	void initCharsetMenus();
	void initPlugins();
	void loadProject();
	QString openDialogDirectory() const;
	void reportError(const QString&);
	
	/**
	* If the \param document is modified asks a question if we
	* want to save the doc.
	* Returns:
	*    true    : if the doc was closed (saved or not)
	*    false   : if the user interrupted closing
	*/
	bool closeDocWithConfirmation(Juff::Document* document);
	
	/**
	* Saves the document. The document MUST NOT be Noname - use saveDocAs instead.
	* Returns:
	*    true    : if the document was saved successfully
	*    false   : if there was an error
	*/
	bool saveDoc(Juff::Document* document);
	
	/**
	* Asks for a file name and saves the \param document with it.
	* Returns:
	*    true    : if the document was saved successfully
	*    false   : if saving failed or was interrupted
	*/
	bool saveDocAs(Juff::Document* document);
	
	/**
	* Updated menus according to current document's properties and document's type.
	*/
	void updateMenus(Juff::Document*);

	void updateGUI(Juff::Document*);
	void updateLineCount(Juff::Document*);
	void updateCursorPos(Juff::Document*);
	void addToRecentFiles(const QString&);

	// fields
	Juff::Project* prj_;
	JuffMW* mw_;
	DocViewer* viewer_;
	DocManager* docManager_;
	QMap<Juff::MenuID, QMenu*> menus_;
	QMenu* charsetMenu_;
	QMenu* openWithCharsetMenu_;
	QMenu* setCharsetMenu_;
	QMenu* recentFilesMenu_;
	QMenu* dockMenu_;
	QMenu* tbMenu_;
	QActionGroup* openWithCharsetGr_;
	QActionGroup* setCharsetGr_;
	
	Juff::StatusLabel* posL_;
	Juff::StatusLabel* nameL_;
	Juff::StatusLabel* charsetL_;
	Juff::StatusLabel* linesL_;
	
	PluginManager pluginMgr_;
	SettingsDlg* settingsDlg_;
	SearchEngine* search_;
	QStringList recentFiles_;
};

#endif // __JUFFED_JUFFED_H__
