#include "SciDocEngine.h"

#include "LexerStorage.h"
#include "Log.h"
#include "SciDoc.h"

#include <QAction>
#include <QMenu>

SciDocEngine::SciDocEngine() : QObject(), DocEngine() {
	syntaxMenu_ = new QMenu(tr("Syntax"));
	connect(syntaxMenu_, SIGNAL(aboutToShow()), SLOT(onMenuAboutToBeShown()));
	QStringList syntaxes = syntaxList();
	foreach (QString syntax, syntaxes) {
		QAction* action = syntaxMenu_->addAction(syntax, this, SLOT(slotSyntaxChanged()));
		action->setCheckable(true);
		syntaxActions_[syntax] = action;
	}
	
	syntaxLabel_ = new Juff::StatusLabel("");
	syntaxLabel_->setToolTip(QObject::tr("Syntax highlighting"));
	syntaxLabel_->setMenu(syntaxMenu_);
	syntaxLabel_->hide();
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
		{
			addAction(id, menu, createAction(tr("UPPER CASE"), QKeySequence("Ctrl+U"), SLOT(slotUpperCase())));
			addAction(id, menu, createAction(tr("lower case"), QKeySequence("Shift+Ctrl+U"), SLOT(slotLowerCase())));
			addAction(id, menu, createAction(tr("Move up"), QKeySequence("Alt+Up"), SLOT(slotMoveUp())));
			addAction(id, menu, createAction(tr("Move down"), QKeySequence("Alt+Down"), SLOT(slotMoveDown())));
			
			
			break;
		}
		
		case Juff::MenuView :
		{
			addAction(id, menu, syntaxMenu_->menuAction());
			break;
		}
		
		default:;
	}
}

QWidgetList SciDocEngine::statusWidgets() {
	QWidgetList  list;
	list << syntaxLabel_;
	return list;
}

void SciDocEngine::activate(bool act) {
	LOGGER;
	syntaxLabel_->show();
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


void SciDocEngine::slotSyntaxChanged() {
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(curDoc());
	QAction* action = qobject_cast<QAction*>(sender());
	if ( doc != 0 && action != 0 ) {
		doc->setSyntax(action->text());
		syntaxLabel_->setText(action->text());;
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
	LOGGER;
	
	SciDoc* doc = qobject_cast<SciDoc*>(sender());
	if ( doc != 0 ) {
		syntaxLabel_->setText(doc->syntax());
	}
}
