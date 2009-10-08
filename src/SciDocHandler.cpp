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

#include "SciDocHandler.h"

#include <QtGui/QApplication>
#include <QtGui/QInputDialog>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QToolBar>

//#include <Qsci/qscimacro.h>

#include "CommandStorage.h"
#include "LexerStorage.h"
#include "SciDoc.h"
#include "TextDocSettings.h"
#include "gui/StatusLabel.h"

namespace Juff {

class SciDocHandler::Interior {
public:
	enum {
		SHOW_LINE_NUMBERS,
		WORD_WRAP,
		SHOW_INVISIBLE_SYMBOLS,
		GO_TO_MATCHING_BRACE,
		SELECT_TO_MATCHING_BRACE,
		COMMENT_LINE,
		COMMENT_BLOCK,
		CHANGE_SPLIT,
	};
	
	Interior() {
		markersMenu_ = new QMenu(QObject::tr("&Markers"));

		syntaxMenu_ = new QMenu(QObject::tr("&Syntax"));
		syntaxActGr_ = new QActionGroup(0);

		eolMenu_ = new QMenu(QObject::tr("Line endings"));

		syntaxL_ = new GUI::StatusLabel("");
		syntaxL_->setMenu(syntaxMenu_);
		syntaxL_->setToolTip(QObject::tr("Syntax highlighting scheme"));

		eolL_ = new GUI::StatusLabel("");
		eolL_->setMenu(eolMenu_);
		eolL_->setToolTip(QObject::tr("Line endings"));

//		macrosMenu_ = new QMenu(QObject::tr("Macro"));
//		startMacroAct_ = new QAction(QIcon(":record.png"), "Start recording", 0);
//		stopMacroAct_ = new QAction(QIcon(":stop.png"), "Stop recording", 0);
//		macrosMenu_->addAction(startMacroAct_);
//		macrosMenu_->addAction(stopMacroAct_);

		goToMatchingBraceAct_ = new QAction(QObject::tr("Go to matching brace"), 0);
		goToMatchingBraceAct_->setShortcut(QKeySequence("Ctrl+E"));
		selToMatchingBraceAct_ = new QAction(QObject::tr("Select to matching brace"), 0);
		selToMatchingBraceAct_->setShortcut(QKeySequence("Shift+Ctrl+E"));

		lineCommentAct_ = new QAction(QObject::tr("Comment line(s)"), 0);
		lineCommentAct_->setShortcut(QKeySequence("Ctrl+/"));
		blockCommentAct_ = new QAction(QObject::tr("Comment block"), 0);
		blockCommentAct_->setShortcut(QKeySequence("Shift+Ctrl+/"));
		
		showLineNumsAct_ = new QAction(tr("Show line numbers"), 0);
		showLineNumsAct_->setShortcut(QKeySequence("F11"));
		showLineNumsAct_->setCheckable(true);
		showLineNumsAct_->setChecked(TextDocSettings::showLineNumbers());
		
		wordWrapAct_ = new QAction(tr("Wrap words"), 0);
		wordWrapAct_->setShortcut(QKeySequence("F10"));
		wordWrapAct_->setCheckable(true);
		wordWrapAct_->setChecked(TextDocSettings::widthAdjust());
	
		showInvisibleAct_ = new QAction(tr("Show invisible symbols"), 0);
		showInvisibleAct_->setCheckable(true);
		showInvisibleAct_->setChecked(TextDocSettings::showInvisibleSymbols());
	
		changeSplitAct_ = new QAction(QObject::tr("Change split orientation"), 0);

		statusWidgets_ << syntaxL_ << eolL_;

		CommandStorage* st = CommandStorage::instance();
		QList<QAction*> eolActList;
		eolActGr_ = new QActionGroup(0);
		eolActList << st->action(ID_EOL_UNIX) << st->action(ID_EOL_WIN) << st->action(ID_EOL_MAC);
		foreach (QAction* act, eolActList) {
			eolMenu_->addAction(act);
			eolActGr_->addAction(act);
			act->setCheckable(true);
		}
		
		QList<QAction*> list;
		QList<int> shifts;
		list << showLineNumsAct_ << wordWrapAct_ << showInvisibleAct_ << goToMatchingBraceAct_ 
		     << selToMatchingBraceAct_ << lineCommentAct_ << blockCommentAct_ << changeSplitAct_;
		shifts << SHOW_LINE_NUMBERS << WORD_WRAP << SHOW_INVISIBLE_SYMBOLS << GO_TO_MATCHING_BRACE
		       << SELECT_TO_MATCHING_BRACE << COMMENT_LINE << COMMENT_BLOCK << CHANGE_SPLIT;
		for (int i = 0; i < list.size(); ++i) {
			CommandStorage::instance()->registerExtCommand(ID_SCI_BASE_ITEM + shifts[i], list[i]);
		}
//		macro_ = 0;
	}

	~Interior() {
		delete syntaxActGr_;
		delete eolActGr_;
		delete showInvisibleAct_;
//		if ( macro_ )
//			delete macro_;	//	in case we closed the app without stopping macro recording
		delete goToMatchingBraceAct_;
		delete selToMatchingBraceAct_;
	}
	
	void displaySyntax(const QString& syntax) {
		syntaxL_->setText(syntax);
		syntaxL_->setToolTip(QObject::tr("Syntax highlighting scheme: %1").arg(syntax));
	}

	QMenu* markersMenu_;
	QMenu* syntaxMenu_;
	QMenu* eolMenu_;
//	QMenu* macrosMenu_;
//	QAction* startMacroAct_;
//	QAction* stopMacroAct_;
	ToolBarList toolBars_;
	MenuList menus_;
	QMap<QString, QAction*> syntaxActions_;
	QActionGroup* syntaxActGr_;
	QActionGroup* eolActGr_;
	QWidgetList statusWidgets_;
	ActionList contextMenuActions_;
	GUI::StatusLabel* syntaxL_;
	GUI::StatusLabel* eolL_;
	QAction* showLineNumsAct_;
	QAction* wordWrapAct_;
	QAction* showInvisibleAct_;
	QAction* goToMatchingBraceAct_;
	QAction* selToMatchingBraceAct_;
	QAction* lineCommentAct_;
	QAction* blockCommentAct_;
	QAction* changeSplitAct_;
//	QsciMacro* macro_;
//	QMap<QString, QString> macros_;
};

SciDocHandler::SciDocHandler() : DocHandler() {
	JUFFENTRY;

	CommandStorage* st = CommandStorage::instance();
	st->registerCommand(ID_EOL_WIN,             this, SLOT(eolSelected()));
	st->registerCommand(ID_EOL_MAC,             this, SLOT(eolSelected()));
	st->registerCommand(ID_EOL_UNIX,            this, SLOT(eolSelected()));
	st->registerCommand(ID_MARKER_TOGGLE,       this, SLOT(toggleMarker()));
	st->registerCommand(ID_MARKER_NEXT,         this, SLOT(nextMarker()));
	st->registerCommand(ID_MARKER_PREV,         this, SLOT(prevMarker()));
	st->registerCommand(ID_MARKER_REMOVE_ALL,   this, SLOT(removeAllMarkers()));
	st->registerCommand(ID_ZOOM_IN,             this, SLOT(zoomIn()));
	st->registerCommand(ID_ZOOM_OUT,            this, SLOT(zoomOut()));
	st->registerCommand(ID_ZOOM_100,            this, SLOT(zoom100()));

	docInt_ = new Interior();

	connect(docInt_->showLineNumsAct_, SIGNAL(activated()), this, SLOT(showLineNums()));
	connect(docInt_->wordWrapAct_, SIGNAL(activated()), this, SLOT(wordWrap()));
	connect(docInt_->showInvisibleAct_, SIGNAL(activated()), this, SLOT(showInvisibleSymbols()));

	connect(docInt_->markersMenu_, SIGNAL(aboutToShow()), SLOT(initMarkersMenu()));

	docInt_->menus_ << docInt_->markersMenu_;

	initSyntaxMenu();

//	connect(docInt_->startMacroAct_, SIGNAL(activated()), this, SLOT(startMacroRecord()));
//	connect(docInt_->stopMacroAct_, SIGNAL(activated()), this, SLOT(stopMacroRecord()));
//	docInt_->stopMacroAct_->setEnabled(false);

	connect(docInt_->goToMatchingBraceAct_, SIGNAL(activated()), this, SLOT(goToMatchingBrace()));
	connect(docInt_->selToMatchingBraceAct_, SIGNAL(activated()), this, SLOT(selectToMatchingBrace()));

	connect(docInt_->lineCommentAct_, SIGNAL(activated()), this, SLOT(toggleLineComment()));
	connect(docInt_->blockCommentAct_, SIGNAL(activated()), this, SLOT(toggleBlockComment()));
	connect(docInt_->changeSplitAct_, SIGNAL(activated()), this, SLOT(changeSplitOrientation()));

	QToolBar* zoomTB = new QToolBar("Zoom");
	zoomTB->addAction(st->action(ID_ZOOM_IN));
	zoomTB->addAction(st->action(ID_ZOOM_OUT));
	zoomTB->addAction(st->action(ID_ZOOM_100));

	docInt_->toolBars_ << zoomTB;
}

SciDocHandler::~SciDocHandler() {
	delete docInt_;
}

void SciDocHandler::initMarkersMenu() {
	JUFFENTRY2;

	if ( docInt_->markersMenu_ ) {
		docInt_->markersMenu_->clear();

		Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
		if ( doc && !doc->isNull() ) {
			docInt_->markersMenu_->addAction(CommandStorage::instance()->action(ID_MARKER_TOGGLE));
			docInt_->markersMenu_->addAction(CommandStorage::instance()->action(ID_MARKER_NEXT));
			docInt_->markersMenu_->addAction(CommandStorage::instance()->action(ID_MARKER_PREV));
			docInt_->markersMenu_->addAction(CommandStorage::instance()->action(ID_MARKER_REMOVE_ALL));

			IntList list = doc->markers();
			if ( !list.isEmpty() ) {
				docInt_->markersMenu_->addSeparator();
				foreach (int line, list) {
					QString lineStr = doc->getLine(line).simplified();
					if ( lineStr.length() > 40 )
						lineStr = lineStr.left(40) + " ...";
					docInt_->markersMenu_->addAction(QString("%1: %2").arg(line + 1).arg(lineStr), this, SLOT(gotoMarker()));
				}
			}
		}
	}
}

void SciDocHandler::initSyntaxMenu() {
	QStringList sList = LexerStorage::instance()->lexersList();
	foreach (QString s, sList) {
		QAction* a = docInt_->syntaxMenu_->addAction(s, this, SLOT(syntaxSelected()));
		a->setCheckable(true);
		docInt_->syntaxActions_[s] = a;
		docInt_->syntaxActGr_->addAction(a);
	}
}


QString SciDocHandler::type() const {
	return "sci";
}

QString SciDocHandler::fileFilters() const {
	QString filters = "All files (*)";
	filters += ";;Batch files (*.bat)";
	filters += ";;Shell scripts (*.sh *.run)";
	filters += ";;C/C++ source files (*.c *.cc *.cpp *.cxx)";
	filters += ";;C/C++ header files (*.h *.hpp)";
	filters += ";;C# (*.cs)";
	filters += ";;D (*.d)";
	filters += ";;Diff/Patch files (*.diff *.patch)";
	filters += ";;HTML/CSS/JS (*.htm *.html *.xhtml *.dhtml *.css *js)";
	filters += ";;IDL (*.idl)";
	filters += ";;Lua (*.lua)";
	filters += ";;Makefiles (*Makefile*)";
	filters += ";;Perl (*.pl)";
	filters += ";;PHP (*.php*)";
	filters += ";;Python (*.py)";
	filters += ";;Ruby (*.rb)";
	filters += ";;SQL (*.sql)";
	filters += ";;XML (*.xml)";
	return filters;
}

Document* SciDocHandler::createDoc(const QString& fileName) {
	Document* doc = new SciDoc(fileName);
	setDocType(doc, type());
	doc->addContextMenuActions(docInt_->contextMenuActions_);
	return doc;
}



////////////////////////////////////////////////////////////
//	GUI controls

MenuList SciDocHandler::menus() const {
	return docInt_->menus_;
}

ActionList SciDocHandler::menuActions(MenuID id) const {
	ActionList list;
	CommandStorage* st = CommandStorage::instance();
	
	switch ( id ) {
		case ID_MENU_VIEW :
			list << docInt_->showLineNumsAct_;
			list << docInt_->wordWrapAct_;
			list << docInt_->showInvisibleAct_;
			list << st->action(ID_ZOOM_IN);
			list << st->action(ID_ZOOM_OUT);
			list << st->action(ID_ZOOM_100);
			list << docInt_->changeSplitAct_;
//			viewMenu->addSeparator();
			list << docInt_->syntaxMenu_->menuAction();
			break;
		
		case ID_MENU_EDIT :
			list << docInt_->goToMatchingBraceAct_;
			list << docInt_->selToMatchingBraceAct_;
			list << docInt_->lineCommentAct_;
			list << docInt_->blockCommentAct_;
			break;

		case ID_MENU_FORMAT :
			list << docInt_->eolMenu_->menuAction();
			break;

		case ID_MENU_TOOLS :
//			list << docInt_->macrosMenu_->menuAction();
			break;

		default: ;
	}
	return list;
}

ToolBarList SciDocHandler::toolBars() const {
	return docInt_->toolBars_;
}

QWidgetList SciDocHandler::statusWidgets() const {
	return docInt_->statusWidgets_;
}

//

void SciDocHandler::addContextMenuActions(const ActionList& list) {
	docInt_->contextMenuActions_ << list;
}

void SciDocHandler::docActivated(Document* d) {
	SciDoc* doc = qobject_cast<SciDoc*>(d);
	if ( doc ) {
		initMarkersMenu();
		QString syntax = doc->syntax();
		docInt_->displaySyntax(syntax);
		QAction* act = docInt_->syntaxActions_[syntax];
		if ( act ) {
			act->setChecked(true);
		}

		EolMode eol = doc->eolMode();
		CommandID id = (eol == EolWin ? ID_EOL_WIN : ( eol == EolMac ? ID_EOL_MAC : ID_EOL_UNIX) );
		changeCurEol(doc, id, eol);
		foreach (QAction* act, docInt_->eolActGr_->actions()) {
			if ( (CommandID)(act->data().toInt()) == id ) {
				act->setChecked(true);
			}
		}

		doc->showInvisibleSymbols(docInt_->showInvisibleAct_->isChecked());
	}
}



////////////////////////////////////////////////////////////
//	Engine-specific slots

void SciDocHandler::showLineNums() {
	JUFFENTRY2;

	QAction* act = qobject_cast<QAction*>(sender());
	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() && act ) {
		TextDocSettings::setShowLineNumbers(act->isChecked());
		doc->showLineNumbers(act->isChecked());
	}
}

void SciDocHandler::wordWrap() {
	JUFFENTRY2;

	QAction* act = qobject_cast<QAction*>(sender());
	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() && act ) {
		TextDocSettings::setWidthAdjust(act->isChecked());
		doc->wrapText(act->isChecked());
	}
}

void SciDocHandler::showInvisibleSymbols() {
	JUFFENTRY;

	QAction* act = qobject_cast<QAction*>(sender());
	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() && act ) {
		doc->showInvisibleSymbols(act->isChecked());
	}
}

void SciDocHandler::zoomIn() {
	JUFFENTRY;

	QAction* act = qobject_cast<QAction*>(sender());
	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() && act ) {
		doc->zoomIn();
	}
}

void SciDocHandler::zoomOut() {
	JUFFENTRY;

	QAction* act = qobject_cast<QAction*>(sender());
	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() && act ) {
		doc->zoomOut();
	}
}

void SciDocHandler::zoom100() {
	JUFFENTRY;

	QAction* act = qobject_cast<QAction*>(sender());
	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() && act ) {
		doc->zoom100();
	}
}

void SciDocHandler::toggleMarker() {
	JUFFENTRY;

	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() ) {
		doc->toggleMarker();
		initMarkersMenu();
	}
}

void SciDocHandler::nextMarker() {
	JUFFENTRY;

	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() ) {
		doc->nextMarker();
	}
}

void SciDocHandler::prevMarker() {
	JUFFENTRY;

	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() ) {
		doc->prevMarker();
	}
}

void SciDocHandler::removeAllMarkers() {
	JUFFENTRY;

	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() ) {
		doc->removeAllMarkers();
	}
}

void SciDocHandler::gotoMarker() {
	JUFFENTRY;

	QAction* a = qobject_cast<QAction*>(sender());
	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() && a ) {
		int line = a->text().section(':', 0, 0).toInt();
		doc->gotoLine(line - 1);
	}
}

void SciDocHandler::syntaxSelected() {
	JUFFENTRY;

	QAction* a = qobject_cast<QAction*>(sender());
	if (a != 0) {
		Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
		if ( doc && !doc->isNull() ) {
			doc->setSyntax(a->text());
			docInt_->displaySyntax(a->text());
		}
		else {
			a->setChecked(false);
		}
	}
}

void SciDocHandler::eolSelected() {
	JUFFENTRY;

	QAction* a = qobject_cast<QAction*>(sender());
	if (a != 0) {
		Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
		if ( doc && !doc->isNull() ) {
			CommandID id = (CommandID)(a->data().toInt());
			changeCurEol(doc, id);
		}
	}
}

void SciDocHandler::changeCurEol(SciDoc* doc, CommandID id) {
	EolMode mode = ( id == ID_EOL_WIN ? EolWin : (id == ID_EOL_MAC ? EolMac : EolUnix) );
	changeCurEol(doc, id, mode);
}

void SciDocHandler::changeCurEol(SciDoc* doc, CommandID id, EolMode mode) {
	doc->setEolMode(mode);
	//	status bar
	docInt_->eolL_->setPixmap(IconManager::instance()->getIcon(id).pixmap(16, 16));
	QString toolTip = QObject::tr("Line endings");
	switch ( mode ) {
		case EolUnix :
			toolTip += ": " + QObject::tr("Unix");
			break;
		case EolMac :
			toolTip += ": " + QObject::tr("Mac");
			break;
		case EolWin :
			toolTip += ": " + QObject::tr("Win");
			break;
	}
	docInt_->eolL_->setPixmap(IconManager::instance()->getIcon(id).pixmap(16, 16));
	docInt_->eolL_->setToolTip(toolTip);
}


/*void SciDocHandler::startMacroRecord() {
	SciDoc* doc = qobject_cast<SciDoc*>(getCurDoc());
	if ( doc ) {
		docInt_->macro_ = doc->newMacro();
		if ( docInt_->macro_ ) {
			docInt_->macro_->startRecording();
			docInt_->startMacroAct_->setEnabled(false);
			docInt_->stopMacroAct_->setEnabled(true);
		}
	}
}

void SciDocHandler::stopMacroRecord() {
	if ( docInt_->macro_ ) {
		docInt_->macro_->endRecording();
		docInt_->startMacroAct_->setEnabled(true);
		docInt_->stopMacroAct_->setEnabled(false);
		QString mcr = docInt_->macro_->save();
		delete docInt_->macro_;
		docInt_->macro_ = 0;
		bool done = false;
		do {
			QString name = QInputDialog::getText(QApplication::activeWindow(), tr("Macro name"), tr("Macro name"));
			if ( !name.isEmpty() ) {
				if ( docInt_->macros_.contains(name) ) {
					QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), tr("Macro with name '%1' already exists").arg(name));
				}
				else {
					docInt_->macrosMenu_->addAction(name, this, SLOT(runMacro()));
					docInt_->macros_[name] = mcr;
					done = true;
				}
			}
			else {
				//	saving cancelled
				done = true;
			}
		} while ( !done );
	}
}

void SciDocHandler::runMacro() {
	QAction* a = qobject_cast<QAction*>(sender());
	SciDoc* doc = qobject_cast<SciDoc*>(getCurDoc());

	if ( a && doc && docInt_->macros_.contains(a->text()) ) {
		doc->runMacro(docInt_->macros_[a->text()]);
	}
}*/

void SciDocHandler::goToMatchingBrace() {
	SciDoc* doc = qobject_cast<SciDoc*>(getCurDoc());

	if ( doc  ) {
		doc->goToMatchingBrace();
	}
}

void SciDocHandler::selectToMatchingBrace() {
	SciDoc* doc = qobject_cast<SciDoc*>(getCurDoc());

	if ( doc  ) {
		doc->selectToMatchingBrace();
	}
}

void SciDocHandler::toggleLineComment() {
	SciDoc* doc = qobject_cast<SciDoc*>(getCurDoc());

	if ( doc  ) {
		doc->toggleLineComment();
	}
}

void SciDocHandler::toggleBlockComment() {
	SciDoc* doc = qobject_cast<SciDoc*>(getCurDoc());

	if ( doc  ) {
		doc->toggleBlockComment();
	}
}

void SciDocHandler::changeSplitOrientation() {
	SciDoc* doc = qobject_cast<SciDoc*>(getCurDoc());

	if ( doc  ) {
		doc->changeSplitOrientation();
	}
}


}	//	namespace Juff
