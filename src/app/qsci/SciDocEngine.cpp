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

#include "SciDocEngine.h"

#include "../ui/settings/ColorButton.h"
#include "LexerStorage.h"
#include "Log.h"
#include "QSciSettings.h"
#include "SciDoc.h"
#include "../ui/settings/SettingsPage.h"
#include "../ui/settings/SettingsCheckItem.h"

#include <QAction>
#include <QMenu>

#include "ui_QSciSettings.h"

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

class QSciSettingsPage : public SettingsPage {
public:
	QSciSettingsPage() : SettingsPage(0) {
		ui_.setupUi(this);
		
		connect(ui_.curLineChk, SIGNAL(toggled(bool)), ui_.curLineColorBtn, SLOT(setEnabled(bool)));
		connect(ui_.matchingBraceChk, SIGNAL(toggled(bool)), ui_.matchingBraceFgColorBtn, SLOT(setEnabled(bool)));
		connect(ui_.matchingBraceChk, SIGNAL(toggled(bool)), ui_.matchingBraceBgColorBtn, SLOT(setEnabled(bool)));
		connect(ui_.indentsChk, SIGNAL(toggled(bool)), ui_.indentsColorBtn, SLOT(setEnabled(bool)));
		
		indentsColorBtn_ = new ColorButton(ui_.indentsColorBtn, QSciSettings::get(QSciSettings::IndentsColor));
		matchingBraceBgColorBtn_ = new ColorButton(ui_.matchingBraceBgColorBtn, QSciSettings::get(QSciSettings::MatchingBraceBgColor));
		matchingBraceFgColorBtn_ = new ColorButton(ui_.matchingBraceFgColorBtn, QSciSettings::get(QSciSettings::MatchingBraceFgColor));
		curLineColorBtn_ = new ColorButton(ui_.curLineColorBtn, QSciSettings::get(QSciSettings::CurLineColor));
		
		items_
			<< new SettingsCheckItem("QSci", "highlightCurLine", ui_.curLineChk)
			<< new SettingsCheckItem("QSci", "highlightMatchingBrace", ui_.matchingBraceChk)
			<< new SettingsCheckItem("QSci", "showIndents", ui_.indentsChk)
		;
		
		ui_.curLineColorBtn->setEnabled(ui_.curLineChk->isChecked());
		ui_.matchingBraceBgColorBtn->setEnabled(ui_.matchingBraceChk->isChecked());
		ui_.matchingBraceFgColorBtn->setEnabled(ui_.matchingBraceChk->isChecked());
		ui_.indentsColorBtn->setEnabled(ui_.indentsChk->isChecked());
	}
	
	virtual void init() {}
	virtual void apply() {
		QSciSettings::set(QSciSettings::IndentsColor, indentsColorBtn_->color());
		QSciSettings::set(QSciSettings::MatchingBraceBgColor, matchingBraceBgColorBtn_->color());
		QSciSettings::set(QSciSettings::MatchingBraceFgColor, matchingBraceFgColorBtn_->color());
		QSciSettings::set(QSciSettings::CurLineColor, curLineColorBtn_->color());
		
		SettingsPage::apply();
	}
	
private:
	Ui::QSciSettings ui_;

	ColorButton* indentsColorBtn_;
	ColorButton* matchingBraceBgColorBtn_;
	ColorButton* matchingBraceFgColorBtn_;
	ColorButton* curLineColorBtn_;
};


SciDocEngine::SciDocEngine() : QObject(), DocEngine() {
	syntaxGroup_ = new QActionGroup(this);
	eolGroup_ = new QActionGroup(this);
	
	syntaxMenu_ = new QMenu(tr("&Syntax"));
	connect(syntaxMenu_, SIGNAL(aboutToShow()), SLOT(onMenuAboutToBeShown()));
	QStringList syntaxes = syntaxList();
	foreach (QString syntax, syntaxes) {
		QAction* action = syntaxMenu_->addAction(syntax, this, SLOT(slotSyntaxChanged()));
		action->setCheckable(true);
		syntaxActions_[syntax] = action;
		syntaxGroup_->addAction(action);
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
	
	syntaxLabel_ = new Juff::StatusLabel("");
	syntaxLabel_->setToolTip(QObject::tr("Syntax highlighting"));
	syntaxLabel_->setMenu(syntaxMenu_);
	syntaxLabel_->hide();
	
	eolLabel_ = new Juff::StatusLabel("");
	eolLabel_->setToolTip(QObject::tr("Line endings"));
	eolLabel_->setMenu(eolMenu_);
	eolLabel_->hide();
	
	settingsPage_ = new QSciSettingsPage();
}

Juff::Document* SciDocEngine::createDoc(const QString& fileName) const {
	SciDoc* doc = new SciDoc(fileName);
	connect(doc, SIGNAL(focused()), SLOT(onDocFocused()));
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

void SciDocEngine::initMenuActions(Juff::MenuID id, QMenu* menu) {
	switch (id) {
		case Juff::MenuEdit :
			addAction(id, menu, createAction(tr("UPPER CASE"), QKeySequence("Ctrl+U"), SLOT(slotUpperCase())));
			addAction(id, menu, createAction(tr("lower case"), QKeySequence("Shift+Ctrl+U"), SLOT(slotLowerCase())));
			addAction(id, menu, createAction(tr("Move line up"), QKeySequence("Alt+Up"), SLOT(slotMoveUp())));
			addAction(id, menu, createAction(tr("Move line down"), QKeySequence("Alt+Down"), SLOT(slotMoveDown())));
			addAction(id, menu, createAction(tr("Duplicate text"), QKeySequence("Ctrl+D"), SLOT(slotDuplicate())));
			addAction(id, menu, createAction(tr("Remove lines"), QKeySequence("Ctrl+L"), SLOT(slotRemoveLines())));
			addAction(id, menu, createAction(tr("Comment lines"), QKeySequence("Ctrl+/"), SLOT(slotCommentLines())));
			addAction(id, menu, createAction(tr("Comment block"), QKeySequence("Shift+Ctrl+/"), SLOT(slotCommentBlock())));
			addAction(id, menu, createAction(tr("Unindent lines"), QKeySequence("Shift+Tab"), SLOT(slotUnindent())));
			break;
		
		case Juff::MenuView :
			addAction(id, menu, syntaxMenu_->menuAction());
			break;
		
		case Juff::MenuFormat :
			addAction(id, menu, eolMenu_->menuAction());
			break;
		
		default:;
	}
}

QWidgetList SciDocEngine::statusWidgets() {
	QWidgetList  list;
	list << syntaxLabel_ << eolLabel_;
	return list;
}

void SciDocEngine::activate(bool act) {
	LOGGER;
	syntaxLabel_->show();
	eolLabel_->show();
	DocEngine::activate(act);
}

void SciDocEngine::deactivate(bool deact) {
	LOGGER;
	syntaxLabel_->hide();
	DocEngine::deactivate(deact);
}

void SciDocEngine::slotUpperCase() {
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->toUpperCase();
	}
}

void SciDocEngine::slotLowerCase() {
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->toLowerCase();
	}
}

void SciDocEngine::slotMoveUp() {
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->moveUp();
	}
}

void SciDocEngine::slotMoveDown() {
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->moveDown();
	}
}

void SciDocEngine::slotCommentLines() {
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->toggleCommentLines();
	}
}

void SciDocEngine::slotCommentBlock() {
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->toggleCommentBlock();
	}
}

void SciDocEngine::slotDuplicate() {
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->duplicateText();
	}
}

void SciDocEngine::slotUnindent() {
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->unindent();
	}
}

void SciDocEngine::slotRemoveLines() {
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	if ( doc != 0 ) {
		doc->removeLine();
	}
}



void SciDocEngine::slotSyntaxChanged() {
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	QAction* action = qobject_cast<QAction*>(sender());
	if ( doc != 0 && action != 0 ) {
		doc->setSyntax(action->text());
		syntaxLabel_->setText(action->text());;
	}
}

void SciDocEngine::slotEolChanged() {
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	QAction* action = qobject_cast<QAction*>(sender());
	if ( doc != 0 && action != 0 ) {
		SciDoc::Eol eol = (SciDoc::Eol)action->data().toInt();
		eolLabel_->setPixmap(eolIcon(eol).pixmap(16, 16));
		doc->setEol(eol);
	}
}

void SciDocEngine::onMenuAboutToBeShown() {
	LOGGER;
	
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
		syntaxLabel_->setText(doc->syntax());
		
		SciDoc::Eol eol = doc->eol();
		eolLabel_->setPixmap(eolIcon(eol).pixmap(16, 16));
		
		eolActions_[eol]->setChecked(true);
	}
}

QWidget* SciDocEngine::settingsPage() const {
	settingsPage_->setWindowTitle("QScintilla");
	return settingsPage_;
}





