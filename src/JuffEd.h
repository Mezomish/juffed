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

#ifndef _JUFF_ED_H_
#define _JUFF_ED_H_

class DocHandler;
//class Document;
class DocViewer;
class JuffEdInterior;
class TextDoc;
class TextDocView;

#include <QtGui/QMainWindow>

#include "Document.h"

class JuffEd : public QMainWindow {
Q_OBJECT
public:
	JuffEd(DocHandler*);
	virtual ~JuffEd();
	
private slots:
	void applySettings();
	void settings();
	void about();
	void aboutQt();
	void exit();

	void docSwitched(QWidget*);
	void docCloseRequested(QWidget*);
	void docFileNameChanged(Juff::Document*);
	void charsetSelected();
	void syntaxSelected();
	void toggleMarker();
	void nextMarker();
	void prevMarker();
	void removeAllMarkers();
	void gotoMarker();
	void initRecentFilesMenu();
	void initMarkersMenu();

	void displayFileName(const QString&);
	void displayCursorPos(int, int);
	void displayCharset(const QString&);
	void displaySyntax(const QString&);
	
private:
	virtual void closeEvent(QCloseEvent*);
	virtual void dragEnterEvent(QDragEnterEvent*);
	virtual void dropEvent(QDropEvent*);
	virtual void resizeEvent(QResizeEvent*);
	virtual void moveEvent(QMoveEvent*);
	
	void createMenuBar();
	void createToolBar();
	void initCharsetsMenu();
	void setupToolBarStyle();
	void createCommands();

	void changeCurrentCharsetAction(QAction*);
	void changeCurrentSyntaxAction(QAction*);
	
	TextDoc* getCurrentTextDoc();

	JuffEdInterior* jInt_;
};

#endif
