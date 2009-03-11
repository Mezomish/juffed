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

#include "SciDocHandler.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QToolBar>

#include "CommandStorage.h"
#include "LexerStorage.h"
#include "SciDoc.h"
#include "TextDocSettings.h"

namespace Juff {

class SciDocHandler::Interior {
public:
	Interior() {
		markersMenu_ = new QMenu(QObject::tr("&Markers"));

	//	initMarkersMenu();
		
//		QAction* act = new QAction(tr("Toggle marker"), 0);


		syntaxMenu_ = new QMenu(QObject::tr("&Syntax"));
		syntaxActGr_ = new QActionGroup(0);
		
		syntaxL_ = new QLabel("");
		syntaxL_->setToolTip(QObject::tr("Syntax highlighting scheme"));
		statusWidgets_ << syntaxL_;
	}
	
	~Interior() {
		delete syntaxActGr_;
	}
	
	QMenu* markersMenu_;
	QMenu* syntaxMenu_;
	ToolBarList toolBars_;
	MenuList menus_;
	QMap<QString, QAction*> syntaxActions_;
	QActionGroup* syntaxActGr_;
	QWidgetList statusWidgets_;
	QLabel* syntaxL_;
};

SciDocHandler::SciDocHandler() : DocHandler() {	
	JUFFENTRY;
	
	docInt_ = new Interior();
	
		QAction* showLineNumsAct = new QAction(tr("Show line numbers"), 0);
		showLineNumsAct->setShortcut(QKeySequence("F11"));
		showLineNumsAct->setCheckable(true);
		showLineNumsAct->setChecked(TextDocSettings::showLineNumbers());
		connect(showLineNumsAct, SIGNAL(activated()), this, SLOT(showLineNums()));
		
		QAction* wordWrapAct = new QAction(tr("Wrap words"), 0);
		wordWrapAct->setShortcut(QKeySequence("F10"));
		wordWrapAct->setCheckable(true);
		wordWrapAct->setChecked(TextDocSettings::widthAdjust());
		connect(wordWrapAct, SIGNAL(activated()), this, SLOT(wordWrap()));

		CommandStorage::instance()->registerCommand(ID_MARKER_TOGGLE, this, SLOT(toggleMarker()));
		CommandStorage::instance()->registerCommand(ID_MARKER_NEXT, this, SLOT(nextMarker()));
		CommandStorage::instance()->registerCommand(ID_MARKER_PREV, this, SLOT(prevMarker()));
		CommandStorage::instance()->registerCommand(ID_MARKER_REMOVE_ALL, this, SLOT(removeAllMarkers()));

		CommandStorage* st = CommandStorage::instance();
		st->registerCommand(ID_ZOOM_IN, this, SLOT(zoomIn()));
		st->registerCommand(ID_ZOOM_OUT, this, SLOT(zoomOut()));
		st->registerCommand(ID_ZOOM_100, this, SLOT(zoom100()));

		QMenu* viewMenu = new QMenu(tr("&View"));
		viewMenu->addAction(showLineNumsAct);
		viewMenu->addAction(wordWrapAct);
		viewMenu->addAction(st->action(ID_ZOOM_IN));
		viewMenu->addAction(st->action(ID_ZOOM_OUT));
		viewMenu->addAction(st->action(ID_ZOOM_100));
		
		connect(docInt_->markersMenu_, SIGNAL(aboutToShow()), SLOT(initMarkersMenu()));

		docInt_->menus_ << viewMenu << docInt_->syntaxMenu_ << docInt_->markersMenu_;

		initSyntaxMenu();


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
	JUFFENTRY;

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
//	if (fmtMenu != 0) {
//		fmtMenu->addMenu(jInt_->syntaxMenu_);
		QStringList sList;
		LexerStorage::instance()->getLexersList(sList);
		foreach (QString s, sList) {
			QAction* a = docInt_->syntaxMenu_->addAction(s, this, SLOT(syntaxSelected()));
			a->setCheckable(true);
			docInt_->syntaxActions_[s] = a;
			docInt_->syntaxActGr_->addAction(a);
		}	
//	}
}

QString SciDocHandler::type() const {
	return "sci";
}

Document* SciDocHandler::createDoc(const QString& fileName) {
	Document* doc = new SciDoc(fileName);
	setDocType(doc, type());
	return doc;
}

ToolBarList SciDocHandler::toolBars() const {
	return docInt_->toolBars_;
}

MenuList SciDocHandler::menus() const {
	return docInt_->menus_;
}

QWidgetList SciDocHandler::statusWidgets() const {
	return docInt_->statusWidgets_;
}

QString SciDocHandler::fileFilters() const {
	QString filters = "All files (*)";
	filters += ";;Batch files (*.bat)";
	filters += ";;Shell scripts (*.sh *.run)";
	filters += ";;C/C++ files (*.c *.cpp *.cxx)";
	filters += ";;C# (*.cs)";
	filters += ";;D (*.d)";
	filters += ";;Diff/Patch files (*.diff *.patch)";
	filters += ";;Header files (*.h *.hpp)";
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

void SciDocHandler::docActivated(Document* d) {
	SciDoc* doc = qobject_cast<SciDoc*>(d);
	if ( doc ) {
		initMarkersMenu();
		QString syntax = doc->syntax();
		docInt_->syntaxL_->setText(QString(" %1 ").arg(syntax));
		QAction* act = docInt_->syntaxActions_[syntax];
		if ( act ) {
			act->setChecked(true);
		}
	}
}



void SciDocHandler::showLineNums() {
	JUFFENTRY;
	
	QAction* act = qobject_cast<QAction*>(sender());
	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() && act) {
		TextDocSettings::setShowLineNumbers(act->isChecked());
		doc->showLineNumbers(act->isChecked());
	}
}

void SciDocHandler::wordWrap() {
	JUFFENTRY;

	QAction* act = qobject_cast<QAction*>(sender());
	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() && act) {
		TextDocSettings::setWidthAdjust(act->isChecked());
		doc->wrapText(act->isChecked());
	}
}

void SciDocHandler::zoomIn() {
	JUFFENTRY;
	
	QAction* act = qobject_cast<QAction*>(sender());
	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() && act) {
		doc->zoomIn();
	}
}

void SciDocHandler::zoomOut() {
	JUFFENTRY;
	
	QAction* act = qobject_cast<QAction*>(sender());
	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() && act) {
		doc->zoomOut();
	}
}

void SciDocHandler::zoom100() {
	JUFFENTRY;
	
	QAction* act = qobject_cast<QAction*>(sender());
	Juff::SciDoc* doc = qobject_cast<Juff::SciDoc*>(emit getCurDoc());
	if ( doc && !doc->isNull() && act) {
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
//			displaySyntax(a->text());
//			changeCurrentSyntaxAction(a);
		}
		else {
			a->setChecked(false);
		}
	}
}


}	//	namespace Juff
