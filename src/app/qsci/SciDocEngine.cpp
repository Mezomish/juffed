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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "SciDocEngine.h"

#include "EditorSettings.h"
#include "LexerStorage.h"
#include "Log.h"
#include "SciDoc.h"
#include "SettingsCheckItem.h"
#include "Utils.h"
#include "settings/FileTypesPage.h"

#include <QAction>
#include <QCheckBox>
#include <QMenu>

class PrintingPage: public SettingsPage {
public:
	PrintingPage() : SettingsPage(0) {
		QVBoxLayout* vBox = new QVBoxLayout(this);
		keepColorsChk_  = new QCheckBox(QObject::tr("Keep syntax highlighting"));
		keepBgColorChk_ = new QCheckBox(QObject::tr("Keep background color"));
		alwaysWrapChk_  = new QCheckBox(QObject::tr("Always wrap text"));
		vBox->addWidget(keepColorsChk_);
		vBox->addWidget(keepBgColorChk_);
		vBox->addWidget(alwaysWrapChk_);
		vBox->addStretch();
		vBox->setContentsMargins( 9, 0, 0, 0 );
	}
	void init() {
		items_ << new SettingsCheckItem("printing", "keepColors", keepColorsChk_)
			  << new SettingsCheckItem("printing", "keepBgColor", keepBgColorChk_)
			  << new SettingsCheckItem("printing", "alwaysWrap", alwaysWrapChk_);
	}
	QCheckBox* keepColorsChk_;
	QCheckBox* keepBgColorChk_;
	QCheckBox* alwaysWrapChk_;
};


QIcon eolIcon(SciDoc::Eol eol) {
	switch ( eol ) {
		case SciDoc::EolWin  : return QIcon(":win");
		case SciDoc::EolMac  : return QIcon(":mac");
		case SciDoc::EolUnix : return QIcon(":tux");
		default :;
	}
	return QIcon();
}

QString eolText(SciDoc::Eol eol) {
	switch ( eol ) {
		case SciDoc::EolWin  : return SciDocEngine::tr("Win");
		case SciDoc::EolMac  : return SciDocEngine::tr("Mac");
		case SciDoc::EolUnix : return SciDocEngine::tr("Unix");
		default :;
	}
	return "";
}

QString indentationText(bool use_tabs) {
	if (use_tabs)
		return SciDocEngine::tr("Tabs");
	else
		return SciDocEngine::tr("Spaces");
}

QString indentationWidthText(int width) {
	switch (width) {
		case 1:
			return SciDocEngine::tr("1");
		case 2:
			return SciDocEngine::tr("2");
		case 3:
			return SciDocEngine::tr("3");
		case 4:
			return SciDocEngine::tr("4");
		case 5:
			return SciDocEngine::tr("5");
		case 6:
			return SciDocEngine::tr("6");
		case 7:
			return SciDocEngine::tr("7");
		case 8:
			return SciDocEngine::tr("8");
		default:
			return SciDocEngine::tr("4");
	}
}

SciDocEngine::SciDocEngine() : QObject(), Juff::DocEngine() {
	syntaxGroup_ = new QActionGroup(this);
	eolGroup_ = new QActionGroup(this);
	indentationGroup_ = new QActionGroup(this);
	indentationWidthGroup_ = new QActionGroup(this);
	
	syntaxMenu_ = new QMenu(tr("&Syntax"));
	connect(syntaxMenu_, SIGNAL(aboutToShow()), SLOT(onMenuAboutToBeShown()));
	QStringList syntaxes = syntaxList();
	foreach (QString syntax, syntaxes) {
		QAction* action = syntaxMenu_->addAction(syntax, this, SLOT(slotSyntaxChanged()));
		action->setCheckable(true);
		syntaxActions_[syntax] = action;
		syntaxGroup_->addAction(action);
	}
	
	indentationMenu_ = new QMenu(tr("Indentation"));
	for ( int width = 8; width >= 1; width-- ) {
		QAction* act = indentationMenu_->addAction(indentationWidthText(width), this, SLOT(slotIndentationWidthChanged()));
		act->setCheckable(true);
		act->setData(width);
		indentationWidthActions_[width] = act;
		indentationWidthGroup_->addAction(act);
	}
	indentationMenu_->addSeparator();
	bool use_tab_states[] = {false, true};
	for ( int i = 0; i < 2; i++ ) {
		QAction* act = indentationMenu_->addAction(indentationText(use_tab_states[i]), this, SLOT(slotIndentationChanged()));
		act->setCheckable(true);
		act->setData(use_tab_states[i]);
		indentationActions_[use_tab_states[i]] = act;
		indentationGroup_->addAction(act);
	}

	
	eolMenu_ = new QMenu(tr("Line endings"));
	SciDoc::Eol eols[] = { SciDoc::EolWin, SciDoc::EolMac, SciDoc::EolUnix };
	for (int i = 0; i < 3; i++ ) {
		SciDoc::Eol eol = eols[i];
		QAction* act = eolMenu_->addAction(eolIcon(eol), eolText(eol), this, SLOT(slotEolChanged()));
		act->setCheckable(true);
		act->setData(eol);
		eolActions_[eol] = act;
		eolGroup_->addAction(act);
	}
	
	markersMenu_ = new QMenu(tr("Markers"));
	connect(markersMenu_, SIGNAL(aboutToShow()), SLOT(updateMarkersMenu()));
	
	syntaxLabel_ = new Juff::StatusLabel("");
	syntaxLabel_->setToolTip(QObject::tr("Syntax highlighting"));
	syntaxLabel_->setMenu(syntaxMenu_);
	syntaxLabel_->hide();
	syntaxLabel_->setMaximumWidth(70);
	
	indentationLabel_ = new Juff::StatusLabel("");
	indentationLabel_->setToolTip(QObject::tr("Indentation style"));
	indentationLabel_->setMenu(indentationMenu_);
	indentationLabel_->hide();
	indentationLabel_->setMaximumWidth(50);
	
	eolLabel_ = new Juff::StatusLabel("");
	eolLabel_->setToolTip(QObject::tr("Line endings"));
	eolLabel_->setMenu(eolMenu_);
	eolLabel_->hide();
	eolLabel_->setMaximumWidth(20);
}

Juff::Document* SciDocEngine::createDoc(const QString& fileName) const {
	SciDoc* doc = new SciDoc(fileName);
	connect(doc, SIGNAL(focused()), SLOT(onDocFocused()));
	connect(doc, SIGNAL(markersMenuRequested(const QPoint&)), SLOT(onMarkersMenuRequested(const QPoint&)));
	return doc;
}

QString SciDocEngine::type() const {
	return "QSci";
}

QStringList SciDocEngine::syntaxList() const {
	return LexerStorage::instance()->lexersList();
}

QAction* SciDocEngine::createAction(const QString& title, const QKeySequence& key, const char* slot) {
	QAction* act = new QAction(title, 0);
	connect(act, SIGNAL(triggered()), this, slot);
	act->setShortcut(key);
	return act;
}

Juff::ActionList SciDocEngine::mainMenuActions(Juff::MenuID id) {
	Juff::ActionList list;
	switch (id) {
		case Juff::MenuEdit :
			list << addAction(id, createAction(tr("UPPER CASE"), QKeySequence("Ctrl+U"), SLOT(slotUpperCase())));
			list << addAction(id, createAction(tr("lower case"), QKeySequence("Shift+Ctrl+U"), SLOT(slotLowerCase())));
			list << addAction(id, createAction(tr("Move line up"), QKeySequence("Alt+Up"), SLOT(slotMoveUp())));
			list << addAction(id, createAction(tr("Move line down"), QKeySequence("Alt+Down"), SLOT(slotMoveDown())));
			list << addAction(id, createAction(tr("Duplicate text"), QKeySequence("Ctrl+D"), SLOT(slotDuplicate())));
			list << addAction(id, createAction(tr("Remove lines"), QKeySequence("Ctrl+L"), SLOT(slotRemoveLines())));
			list << addAction(id, createAction(tr("Remove the beginning of the line"), QKeySequence("Shift+Ctrl+Backspace"), SLOT(slotRemoveLineLeft())));
			list << addAction(id, createAction(tr("Remove the end of the line"), QKeySequence("Shift+Ctrl+Delete"), SLOT(slotRemoveLineRight())));
			list << addAction(id, createAction(tr("Comment lines"), QKeySequence("Ctrl+/"), SLOT(slotCommentLines())));
			list << addAction(id, createAction(tr("Comment block"), QKeySequence("Shift+Ctrl+/"), SLOT(slotCommentBlock())));
			list << addAction(id, createAction(tr("Unindent lines"), QKeySequence("Shift+Tab"), SLOT(slotUnindent())));
			list << addAction(id, createAction(tr("Insert 'Tab' character"), QKeySequence("Shift+Ctrl+Tab"), SLOT(slotInsertTab())));

			break;
		
		case Juff::MenuView :
			showLineNumsAct_    = createAction(tr("Display line numbers"),      QKeySequence(), SLOT(slotShowLineNumbers()));
			wrapWordsAct_       = createAction(tr("Wrap words"),                QKeySequence(), SLOT(slotWrapWords()));
			showWhitespacesAct_ = createAction(tr("Show whitespaces and TABs"), QKeySequence(), SLOT(slotShowWhitespaces()));
			showLineEndingsAct_ = createAction(tr("Show ends of lines"),        QKeySequence(), SLOT(slotShowLineEndings()));
			showLineNumsAct_    ->setCheckable(true);
			wrapWordsAct_       ->setCheckable(true);
			showWhitespacesAct_ ->setCheckable(true);
			showLineEndingsAct_ ->setCheckable(true);
			showLineNumsAct_    ->setChecked(EditorSettings::get(EditorSettings::ShowLineNumbers));
			wrapWordsAct_       ->setChecked(EditorSettings::get(EditorSettings::WrapWords));
			showWhitespacesAct_ ->setChecked(EditorSettings::get(EditorSettings::ShowWhitespaces));
			showLineEndingsAct_ ->setChecked(EditorSettings::get(EditorSettings::ShowLineEnds));
			
			list << addAction(id, showLineNumsAct_);
			list << addAction(id, wrapWordsAct_);
			list << addAction(id, showWhitespacesAct_);
			list << addAction(id, showLineEndingsAct_);
			list << addAction(id, createAction(tr("Fold/Unfold all"),           QKeySequence(), SLOT(slotFoldUnfoldAll())));
			list << addAction(id, syntaxMenu_->menuAction());
			break;
		
		case Juff::MenuFormat :
			list << addAction(id, eolMenu_->menuAction());
			list << addAction(id, indentationMenu_->menuAction());
			break;
		
		case Juff::MenuSearch :
//			menu->addSeparator();
			list << markersMenu_->menuAction();
			break;
		
		default:;
	}
	return list;
}

QWidgetList SciDocEngine::statusWidgets() {
	QWidgetList  list;
	list << syntaxLabel_ << indentationLabel_ << eolLabel_;
	return list;
}

void SciDocEngine::activate(bool act) {
//	LOGGER;
	syntaxLabel_->show();
	indentationLabel_->show();
	eolLabel_->show();
	DocEngine::activate(act);
}

void SciDocEngine::deactivate(bool deact) {
//	LOGGER;
	syntaxLabel_->hide();
	DocEngine::deactivate(deact);
}

void SciDocEngine::onDocActivated(Juff::Document* doc) {
	DocEngine::onDocActivated(doc);
	SciDoc* sciDoc = qobject_cast<SciDoc*>(curDoc());
	if ( sciDoc != 0 ) {
		sciDoc->setWrapWords      (wrapWordsAct_       ->isChecked());
		sciDoc->setShowLineNumbers(showLineNumsAct_    ->isChecked());
		sciDoc->setShowWhitespaces(showWhitespacesAct_ ->isChecked());
		sciDoc->setShowLineEndings(showLineEndingsAct_ ->isChecked());
	}
}


void SciDocEngine::slotUpperCase() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->toUpperCase();
	}
}

void SciDocEngine::slotLowerCase() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->toLowerCase();
	}
}

void SciDocEngine::slotMoveUp() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->moveUp();
	}
}

void SciDocEngine::slotMoveDown() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->moveDown();
	}
}

void SciDocEngine::slotCommentLines() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->toggleCommentLines();
	}
}

void SciDocEngine::slotCommentBlock() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->toggleCommentBlock();
	}
}

void SciDocEngine::slotDuplicate() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->duplicateText();
	}
}

void SciDocEngine::slotUnindent() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->unindent();
	}
}

void SciDocEngine::slotInsertTab() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->insertTab();
	}
}

void SciDocEngine::slotRemoveLines() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->removeLine();
	}
}

void SciDocEngine::slotRemoveLineLeft() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->removeLineLeft();
	}
}

void SciDocEngine::slotRemoveLineRight() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->removeLineRight();
	}
}

void SciDocEngine::slotFoldUnfoldAll() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->foldUnfoldAll();
	}
}

void SciDocEngine::slotMarkerAddRemove() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		int line, col;
		doc->getCursorPos(line, col);
		doc->toggleMarker(line);
	}
}

void SciDocEngine::slotMarkerRemoveAll() {
//	LOGGER;
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->removeAllMarkers();
	}
}

void SciDocEngine::slotMarkerNext() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		int line, col;
		doc->getCursorPos(line, col);
		QList<int> markers = doc->markers();
		if ( markers.count() == 0 )
			return;
		foreach (int marker, markers) {
			if ( marker > line ) {
				doc->setCursorPos(marker, 0);
				return;
			}
		}
		
		// If we're here then no markers were found 
		// after the current line - need to go to the 
		// very 1st marker
		doc->setCursorPos(markers[0], 0);
	}
}

void SciDocEngine::slotMarkerPrev() {
//	LOGGER;
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		int line, col;
		doc->getCursorPos(line, col);
		QList<int> markers = doc->markers();
		if ( markers.count() == 0 )
			return;
		
		int prevMarker = markers[markers.count() - 1];
		foreach (int marker, markers) {
			if ( marker < line ) {
				prevMarker = marker;
			}
			else {
				doc->setCursorPos(prevMarker, 0);
				return;
			}
		}
		
		// If we're here then all markers are before 
		// the current line - need to go to the 
		// last marker
		doc->setCursorPos(markers[markers.count() - 1], 0);
	}
}




void SciDocEngine::slotSyntaxChanged() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	QAction* action = qobject_cast<QAction*>(sender());
	if ( doc != 0 && action != 0 ) {
		doc->setSyntax(action->text());
		syntaxLabel_->setText(action->text());;
	}
}

void SciDocEngine::slotEolChanged() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	QAction* action = qobject_cast<QAction*>(sender());
	if ( doc != 0 && action != 0 ) {
		SciDoc::Eol eol = (SciDoc::Eol)action->data().toInt();
		eolLabel_->setPixmap(eolIcon(eol).pixmap(16, 16));
		doc->setEol(eol);
	}
}

void SciDocEngine::slotIndentationChanged() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	QAction* action = qobject_cast<QAction*>(sender());
	if ( doc != 0 && action != 0 ) {
		bool use_tabs = (bool)action->data().toBool();
		indentationLabel_->setText(indentationText(use_tabs));
		doc->setIndentationsUseTabs(use_tabs);
	}
}

void SciDocEngine::slotIndentationWidthChanged() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	QAction* action = qobject_cast<QAction*>(sender());
	if ( doc != 0 && action != 0 ) {
		int width = (int)action->data().toInt();
		doc->setTabWidth(width);
	}
}

void SciDocEngine::onMenuAboutToBeShown() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		QString docSyntax = doc->syntax();
		QMap<QString, QAction*>::iterator it = syntaxActions_.begin();
		for (; it != syntaxActions_.end(); it++) {
			if ( it.key() == docSyntax )
				it.value()->setChecked(true);
			else
				it.value()->setChecked(false);
		}
	}
}


void SciDocEngine::onDocFocused() {
//	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(sender());
	if ( doc != 0 ) {
		onDocActivated(doc);
		
		syntaxLabel_->setText(doc->syntax());
		
		SciDoc::Eol eol = doc->eol();
		eolLabel_->setPixmap(eolIcon(eol).pixmap(16, 16));
		eolActions_[eol]->setChecked(true);
		
		bool use_tabs = doc->indentationsUseTabs();
		indentationLabel_->setText(indentationText(use_tabs));
		indentationActions_[use_tabs]->setChecked(true);
		
		int width = doc->tabWidth();
		indentationWidthActions_[width]->setChecked(true);
		
		updateMarkersMenu();
	}
}

void SciDocEngine::updateMarkersMenu() {
	markersMenu_->clear();
	
	markersMenu_->addAction(createAction(tr("Add/Remove marker"), QKeySequence("Ctrl+B"), SLOT(slotMarkerAddRemove())));
	markersMenu_->addAction(createAction(tr("Next marker"), QKeySequence("Ctrl+Alt+PgDown"), SLOT(slotMarkerNext())));
	markersMenu_->addAction(createAction(tr("Previous marker"), QKeySequence("Ctrl+Alt+PgUp"), SLOT(slotMarkerPrev())));
	markersMenu_->addAction(createAction(tr("Remove all markers"), QKeySequence(""), SLOT(slotMarkerRemoveAll())));
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc == 0 )
		return;
	
	QList<int> markers = doc->markers();
	if ( markers.count() > 0 ) {
		markersMenu_->addSeparator();
		foreach (int marker, markers) {
			QString lineStr;
			doc->getTextLine(marker, lineStr);
			lineStr = lineStr.simplified();
			if ( lineStr.length() > 40 )
				lineStr = lineStr.left(40) + " ...";
			QAction* act = new QAction(QString("%1: %2").arg(marker+1).arg(lineStr), 0);
			connect(act, SIGNAL(triggered()), this, SLOT(slotGotoMarker()));
			markersMenu_->addAction(act);
		}
	}
}

void SciDocEngine::onMarkersMenuRequested(const QPoint& p) {
	markersMenu_->popup(p);
}

void SciDocEngine::slotGotoMarker() {
	QAction* act = qobject_cast<QAction*>(sender());
	if ( act != 0 ) {
		bool ok;
		int lineNumber = act->text().section(':', 0, 0).toInt(&ok) - 1;
		if ( ok ) {
			SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
			if ( doc != 0 ) {
				doc->setCursorPos(lineNumber, 0);
			}
		}
	}
}



void SciDocEngine::slotShowLineNumbers() {
//	LOGGER;
	bool checked = !EditorSettings::get(EditorSettings::ShowLineNumbers);
	EditorSettings::set(EditorSettings::ShowLineNumbers, checked);
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->setShowLineNumbers(checked);
	}
}

void SciDocEngine::slotWrapWords() {
	bool checked = !EditorSettings::get(EditorSettings::WrapWords);
	EditorSettings::set(EditorSettings::WrapWords, checked);
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->setWrapWords(checked);
	}
}

void SciDocEngine::slotShowWhitespaces() {
	bool checked = !EditorSettings::get(EditorSettings::ShowWhitespaces);
	EditorSettings::set(EditorSettings::ShowWhitespaces, checked);
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->setShowWhitespaces(checked);
	}
}

void SciDocEngine::slotShowLineEndings() {
	bool checked = !EditorSettings::get(EditorSettings::ShowLineEnds);
	EditorSettings::set(EditorSettings::ShowLineEnds, checked);
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->setShowLineEndings(checked);
	}
}






bool SciDocEngine::getSettingsPages(QStringList& titles, QWidgetList& pages) const {
	titles << tr("Printing");
	titles << tr("File types");
	pages << new PrintingPage();
	pages << new FileTypesPage();
	return true;
}

void SciDocEngine::getColorOptions(QList<ColorOption>& options) {
	options << ColorOption(tr("Current line highlighting color"), "QSci", "curLineColor", QColor(230, 230, 240));
	options << ColorOption(tr("Matching brace background color"), "QSci", "matchingBraceBgColor", QColor(240, 200, 150));
	options << ColorOption(tr("Indents color"), "QSci", "indentsColor", QColor(200, 200, 200));
	options << ColorOption(tr("Whitespace symbols color"), "QSci", "whiteSpaceColor", QColor(155, 155, 185));
	options << ColorOption(tr("Markers color"), "QSci", "markersColor", QColor(120, 120, 140));
//	options << ColorOption(tr("Margins background color"), "QSci", "marginsBgColor", QColor(220, 220, 220));
}
