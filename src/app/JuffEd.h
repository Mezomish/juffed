#ifndef __JUFF_JUFFED_H__
#define __JUFF_JUFFED_H__

class QActionGroup;
class QMenu;

class JuffMW;
class SettingsDlg;
class SearchEngine;
class PluginManager;
class QDomElement;

namespace Juff {
	class Document;
	class DocViewer;
	class DocEngine;
	class Project;
	class StatusLabel;
}


#include <QMap>
#include <QWidget>

#include "DocHandlerInt.h"
#include "Enums.h"
#include "PluginNotifier.h"

class JuffEd : public Juff::PluginNotifier, public Juff::DocHandlerInt {
Q_OBJECT
public:
	JuffEd();
	virtual ~JuffEd();

	// virtual methods from DocHandlerInt implemented
	virtual QWidget* mainWindow() const;
	virtual Juff::Document* curDoc() const;
	virtual Juff::Document* getDoc(const QString&) const;
	virtual Juff::Project* curPrj() const;
	virtual void openDoc(const QString&, Juff::PanelIndex panel = Juff::PanelCurrent);
	virtual void closeDoc(const QString&);
	virtual void closeAllDocs(Juff::PanelIndex);
	virtual void closeAllOtherDocs(int index, Juff::PanelIndex panel);
	virtual void saveDoc(const QString&);
	virtual int docCount(Juff::PanelIndex) const;
	virtual QStringList docList() const;

public slots:
	void slotFileNew();
	void slotFileOpen();
	void slotFileRecent();
	void slotFileSave();
	void slotFileSaveAs();
	void slotFileRename();
	void slotFileSaveAll();
	void slotFileReload();
	void slotFileClose();
	void slotFileCloseAll();
	void slotFilePrint();
	void slotFileExit();
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
	void slotGotoFile();
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
	void slotCopyFilePath();

	void slotSettings();
	
	void onMessageReceived(const QString&);

public slots:
	void onDocModified(bool);
	void onDocFocused();
	void onDocCursorPosChanged(int, int);
	void onDocLineCountChanged(int);
	void onDocTextChanged();
	void onDocSyntaxChanged(const QString&);
	void onDocCharsetChanged(const QString&);
	void onDocRenamed(const QString&);
	
	void onDocActivated(Juff::Document*);

private slots:
	void initRecentFilesMenu();
	void onCloseRequested(bool&);
	void onSettingsApplied();

private:
	void initActions();
	void initUI();
	void loadPlugins();
	void loadEngines();
	void buildUI();
	void initCharsetMenus();

	QString openDialogDirectory() const;
	Juff::DocEngine* engineForFileName(const QString&) const;
	void reportError(const QString& error);
	bool saveDoc(Juff::Document*);
	bool saveDocAs(Juff::Document*);
	bool closeDocWithConfirmation(Juff::Document*);
	
	void updateMW(Juff::Document*);
	void updateDocView(Juff::Document*);
	void updateLineCount(Juff::Document*);
	void updateCursorPos(Juff::Document*);
	void addToRecentFiles(const QString&);

	void createProject(const QString& fileName);
	bool closeProject();
	QString projectName() const;
	void loadProject();
	
	bool saveSession(const QString&);
	bool loadSession(const QString&);
	bool parseSession(QDomElement&);



	QMap<QString, Juff::DocEngine*> engines_;
	QMap<Juff::MenuID, QMenu*> menus_;

	QMenu* recentFilesMenu_;
	QActionGroup* openWithCharsetGr_;
	QActionGroup* setCharsetGr_;
	QMenu* charsetMenu_;
	QMenu* openWithCharsetMenu_;
	QMenu* setCharsetMenu_;
	QMenu* docksMenu_;
	QMenu* toolBarsMenu_;
	
	Juff::DocViewer* viewer_;
	JuffMW* mw_;
	SettingsDlg* settingsDlg_;
	SearchEngine* search_;
	PluginManager* pluginMgr_;
	
	Juff::StatusLabel* posL_;
	Juff::StatusLabel* nameL_;
	Juff::StatusLabel* charsetL_;
	Juff::StatusLabel* linesL_;
	
	Juff::Project* prj_;
	QStringList recentFiles_;
};

#endif // __JUFF_JUFFED_H__
