/*
JuffEd - An advanced text editor
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

#ifndef _JUFF_SCI_DOC_HANDLER_H_
#define _JUFF_SCI_DOC_HANDLER_H_

#include "DocHandler.h"
#include "Document.h"
#include "SciDoc.h"

namespace Juff {

class SciDocHandler : public DocHandler {
Q_OBJECT
public:
	SciDocHandler();
	virtual ~SciDocHandler();

	virtual QString type() const;
	virtual QString fileFilters() const;
	virtual Document* createDoc(const QString&);

	virtual MenuList menus() const;
	virtual ActionList menuActions(MenuID) const;
	virtual ToolBarList toolBars() const;
	virtual QWidgetList statusWidgets() const;

	virtual void addContextMenuActions(const ActionList&);

	virtual void docActivated(Document*);

private slots:
	void showLineNums();
	void wordWrap();
	void showInvisibleSymbols();
	void zoomIn();
	void zoomOut();
	void zoom100();

	void initMarkersMenu();
	void toggleMarker();
	void nextMarker();
	void prevMarker();
	void removeAllMarkers();
	void gotoMarker();
	void syntaxSelected();
	void eolSelected();

//	void startMacroRecord();
//	void stopMacroRecord();
//	void runMacro();

	void goToMatchingBrace();
	void selectToMatchingBrace();

	void toggleLineComment();
	void toggleBlockComment();
	void duplicateLine();
	void moveLineUp();
	void deleteCurrentLine();
	void toUpperCase();
	void toLowerCase();
	
	void changeSplitOrientation();

private:
	void initSyntaxMenu();
	void changeCurEol(SciDoc*, CommandID);
	void changeCurEol(SciDoc*, CommandID, EolMode);

	class Interior;
	Interior* docInt_;
};

}

#endif
