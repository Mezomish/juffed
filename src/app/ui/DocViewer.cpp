#include "DocViewer.h"

#include <QKeyEvent>
#include <QSplitter>
#include <QVBoxLayout>

#include "DocHandlerInt.h"
#include "Functions.h"
#include "Log.h"
#include "MainSettings.h"
#include "NullDoc.h"
#include "TabWidget.h"

namespace Juff {

PanelIndex DocViewer::anotherPanel(PanelIndex panel) const {
	switch ( panel ) {
		case PanelLeft : return PanelRight;
		case PanelRight : return PanelLeft;
		case PanelCurrent : return ( curTab_ == tab1_ ? PanelRight : PanelLeft );
		default : return PanelNone;
	}
}

DocViewer::DocViewer(Juff::DocHandlerInt* handler) : QWidget(), ctrlTabMenu_(this) {
	handler_ = handler;
	spl_ = new QSplitter(this);
	QVBoxLayout* vBox = new QVBoxLayout(this);
	vBox->addWidget(spl_);
	vBox->setContentsMargins(0, 0, 0, 0);
	setLayout(vBox);

	tab1_ = new Juff::TabWidget(handler);
	tab2_ = new Juff::TabWidget(handler);
	spl_->addWidget(tab1_);
	spl_->addWidget(tab2_);
	curTab_ = tab1_;
	curDoc_ = NullDoc::instance();
	
	spl_->setSizes(QList<int>() << spl_->width() << 0);
	tab2_->hide();
	
	Juff::TabWidget* tabWidgets[] = { tab1_, tab2_, NULL };
	for (int i = 0; tabWidgets[i] != NULL; ++i) {
		Juff::TabWidget* tw = tabWidgets[i];
		
//		connect(tw, SIGNAL(requestDocClone(Juff::Document*, Juff::TabWidget*)), SLOT(onDocCloneRequested(Juff::Document*, Juff::TabWidget*)));
		connect(tw, SIGNAL(requestDocMove(Juff::Document*, Juff::TabWidget*)), SLOT(onDocMoveRequested(Juff::Document*, Juff::TabWidget*)));
		connect(tw, SIGNAL(tabRemoved(Juff::TabWidget*)), SLOT(onTabRemoved(Juff::TabWidget*)));
		connect(tw, SIGNAL(docStackCalled(bool)), SLOT(onDocStackCalled(bool)));
		connect(tw, SIGNAL(currentChanged(int)), SLOT(onCurrentChanged(int)));
	}
	
	// next/prev doc
	nextAct_ = new QAction("Next", this);
	prevAct_ = new QAction("Prev", this);
	nextAct_->setShortcut(QKeySequence("Ctrl+PgDown"));
	prevAct_->setShortcut(QKeySequence("Ctrl+PgUp"));
	connect(nextAct_, SIGNAL(triggered()), SLOT(nextDoc()));
	connect(prevAct_, SIGNAL(triggered()), SLOT(prevDoc()));
	addAction(nextAct_);
	addAction(prevAct_);
	
	// numbered docs
	for (int i = 0; i < 10; ++i) {
		QAction* act = new QAction(QString::number(i), this);
		act->setShortcut(QKeySequence(QString("Alt+%1").arg(i)));
		connect(act, SIGNAL(triggered()), SLOT(goToNumberedDoc()));
		addAction(act);
	}
	
	ctrlTabMenu_.installEventFilter(this);
}

void DocViewer::applySettings() {
	// 1st panel
	QList<Juff::Document*> docs = docList(Juff::PanelLeft);
	foreach (Juff::Document* doc, docs) {
		doc->applySettings();
	}
	
	// 2nd panel
	docs = docList(Juff::PanelRight);
	foreach (Juff::Document* doc, docs) {
		doc->applySettings();
	}

	QTabWidget::TabPosition position = (QTabWidget::TabPosition)MainSettings::get(MainSettings::TabPosition);
	tab1_->setTabPosition(position);
	tab2_->setTabPosition(position);
}

PanelIndex DocViewer::currentPanel() const {
	return curTab_ == tab1_ ? PanelLeft : PanelRight;
}

PanelIndex DocViewer::panelOf(Juff::Document* doc) {
	if ( tab1_->indexOf(doc) >= 0 ) {
		return PanelLeft;
	}
	else if ( tab2_->indexOf(doc) >= 0 ) {
		return PanelRight;
	}
	else {
		return PanelNone;
	}
}

/**
* Shows the specified panel and expands it. If the panel is already visible
* and expanded then does nothing.
*/
void DocViewer::showPanel(PanelIndex panel) {
	if ( panel == PanelLeft ) {
		if ( tab1_->isVisible() && tab1_->width() > 0 ) {
			return;
		}
		
		tab1_->setVisible(true);
		int w = spl_->width() / 2;
		spl_->setSizes(QList<int>() << w << w);
	}
	else if ( panel == PanelRight ) {
		if ( tab2_->isVisible() && tab2_->width() > 0 ) {
			return;
		}
		
		tab2_->setVisible(true);
		int w = spl_->width() / 2;
		spl_->setSizes(QList<int>() << w << w);
	}
}


/**
* Collapses and hides the specified tab. Doesn't care about focused document - you need 
* to take care of it by yourself.
*/
void DocViewer::hidePanel(PanelIndex panel) {
	if ( panel == PanelLeft ) {
		if ( !tab2_->isVisible() || tab2_->width() == 0 ) {
			// right tab is closed
			return;
		}
		
		spl_->setSizes(QList<int>() << 0 << spl_->width());
		tab1_->hide();
		curTab_ = tab2_;
	}
	else if ( panel == PanelRight ) {
		if ( !tab1_->isVisible() || tab1_->width() == 0 ) {
			// left tab is closed
			return;
		}
		
		spl_->setSizes(QList<int>() << spl_->width() << 0);
		tab2_->hide();
		curTab_ = tab1_;
	}
}

void DocViewer::addDoc(Juff::Document* doc, PanelIndex panel) {
	Juff::TabWidget* tabWidget = NULL;
	switch ( panel ) {
		case PanelCurrent : tabWidget = curTab_; break;
		case PanelLeft    : tabWidget = tab1_;   break;
		case PanelRight   : tabWidget = tab2_;   break;
		default: ;
	}

	if ( tabWidget != NULL ) {
	
		connect(doc, SIGNAL(modified(bool)), SLOT(onDocModified(bool)));
		
		// If we want to prevent 'docActivated()' signal to be emitted when
		// adding a new doc then comment the following line and uncomment the
		// same line below it.
		connect(doc, SIGNAL(focused()), SLOT(onDocFocused()));
		
		showPanel(panel);
		
		QString title = doc->title();
		int index = tabWidget->addTab(doc, doc->icon(), title);
		if ( doc->isNoname() )
			tabWidget->setTabToolTip(index, title);
		else
			tabWidget->setTabToolTip(index, doc->fileName());
		
		tabWidget->setCurrentWidget(doc);
		curTab_ = tabWidget;
		
		doc->init();
		doc->setFocus();
		
		
		// It's better to have it after adding to TabWidget to avoid
		// emitting the signal 'docActivated()' during the document creation.
//		connect(doc, SIGNAL(focused()), SLOT(onDocFocused()));
		
		docStack_.removeAll(doc);
		docStack_.prepend(doc);
	}
}

void DocViewer::updateDocTitle(Juff::Document* doc) {
	PanelIndex panel = panelOf(doc);
	TabWidget* tabWidget = NULL;
	if ( panel == Juff::PanelLeft ) {
		tabWidget = tab1_;
	}
	else if ( panel == Juff::PanelRight ) {
		tabWidget = tab2_;
	}
	if ( tabWidget != NULL ) {
		int index = tabWidget->indexOf(doc);
		tabWidget->setTabText(index, doc->titleWithModification());
		tabWidget->setTabIcon(index, QIcon( doc->isModified() ? ":doc_icon_red" : ":doc_icon"));
	}
}

void DocViewer::removeDocFromList(Juff::Document* doc) {
	docStack_.removeAll(doc);
}

Juff::Document* DocViewer::currentDoc() const {
	Juff::Document* doc = qobject_cast<Juff::Document*>(curTab_->currentWidget());
	return doc != 0 ? doc : NullDoc::instance();
}

Juff::Document* DocViewer::currentDoc(PanelIndex panel) const {
	TabWidget* tabWidget = getTabWidget(panel);
	if ( tabWidget == NULL ) {
		return NullDoc::instance();
	}
	
	Juff::Document* doc = qobject_cast<Juff::Document*>(tabWidget->currentWidget());
	return doc != 0 ? doc : NullDoc::instance();
}

int DocViewer::currentIndex(PanelIndex panel) const {
	TabWidget* tabWidget = getTabWidget(panel);
	if ( tabWidget == NULL ) {
		return -1;
	}
	
	return tabWidget->currentIndex();
}

void DocViewer::setCurrentIndex(PanelIndex panel, int index) {
	TabWidget* tabWidget = getTabWidget(panel);
	if ( tabWidget == NULL ) {
		return;
	}
	
	if ( index >= 0 && index < tabWidget->count() ) {
		tabWidget->setCurrentIndex(index);
	}
}

Juff::Document* DocViewer::document(const QString& fileName) const {
	QList<Juff::Document*> docs = docList(Juff::PanelLeft);
	foreach (Juff::Document* doc, docs) {
		if ( doc->fileName() == fileName ) {
			return doc;
		}
	}
	
	docs = docList(Juff::PanelRight);
	foreach (Juff::Document* doc, docs) {
		if ( doc->fileName() == fileName ) {
			return doc;
		}
	}
	
	return NullDoc::instance();
}

Juff::Document* DocViewer::documentAt(int index, PanelIndex panel) const {
	TabWidget* tabWidget = getTabWidget(panel);
	if ( tabWidget == NULL ) {
		return NullDoc::instance();
	}
	
	Juff::Document* doc = qobject_cast<Juff::Document*>(tabWidget->widget(index));
	if ( doc != 0 )
		return doc;
	else
		return NullDoc::instance();
}

bool DocViewer::activateDoc(const QString& fileName) {
	QList<Juff::Document*> docs = docList(Juff::PanelLeft);
	foreach (Juff::Document* doc, docs) {
		if ( doc->fileName() == fileName ) {
			if ( tab1_->width() == 0 ) {
				spl_->setSizes(QList<int>() << spl_->width() / 2 << spl_->width() / 2);
			}
			tab1_->setCurrentWidget(doc);
			doc->setFocus();
			return true;
		}
	}
	
	docs = docList(Juff::PanelRight);
	foreach (Juff::Document* doc, docs) {
		if ( doc->fileName() == fileName ) {
			if ( tab2_->width() == 0 )
				spl_->setSizes(QList<int>() << spl_->width() / 2 << spl_->width() / 2);
			tab2_->setCurrentWidget(doc);
			doc->setFocus();
			return true;
		}
	}
	
	return false;
}

void DocViewer::nextDoc() {
	int n = curTab_->count();
	if ( n == 0 )
		return;
	
	curTab_->setCurrentIndex( (curTab_->currentIndex() + 1) % n );
}

void DocViewer::prevDoc() {
	int n = curTab_->count();
	if ( n == 0 )
		return;
	
	curTab_->setCurrentIndex( (curTab_->currentIndex() + n - 1) % n );
}

void DocViewer::goToNumberedDoc() {
	QAction* act = qobject_cast<QAction*>(sender());
	if ( act == 0 )
		return;
	
	int index = act->text().toInt();
	if ( index == 0 )
		index = 10;
	curTab_->setCurrentIndex(index - 1);
}

int DocViewer::docCount(PanelIndex panel) const {
	switch (panel) {
		case PanelCurrent :
			return curTab_->count();
		case PanelLeft :
			return tab1_->count();
		case PanelRight :
			return tab2_->count();
		case PanelAll :
			return tab1_->count() + tab2_->count();
		default :
			return 0;
	}
}

DocList DocViewer::docList(PanelIndex panel) const {
	if ( panel == PanelCurrent )
		return docList(currentPanel());
	
	QList<Juff::Document*> list;
	
	// 1st panel
	if ( panel == PanelAll || panel == PanelLeft ) {
		int n = tab1_->count();
		for (int i = 0; i < n; ++i) {
			Juff::Document* doc = qobject_cast<Juff::Document*>(tab1_->widget(i));
			if ( doc != 0 )
				list << doc;
		}
	}
	// 2nd panel
	if ( panel == PanelAll || panel == PanelRight ) {
		int n = tab2_->count();
		for (int i = 0; i < n; ++i) {
			Juff::Document* doc = qobject_cast<Juff::Document*>(tab2_->widget(i));
			if ( doc != 0 )
				list << doc;
		}
	}
	return list;
}

QStringList DocViewer::docNamesList(PanelIndex panel) const {
	if ( panel == PanelCurrent )
		return docNamesList(currentPanel());
	
	QStringList list;
	// 1st panel
	if ( panel == PanelAll || panel == PanelLeft ) {
		int n = tab1_->count();
		for (int i = 0; i < n; ++i) {
			Juff::Document* doc = qobject_cast<Juff::Document*>(tab1_->widget(i));
			if ( doc != 0 )
				list << doc->fileName();
		}
	}
	// 2nd panel
	if ( panel == PanelAll || panel == PanelRight ) {
		int n = tab2_->count();
		for (int i = 0; i < n; ++i) {
			Juff::Document* doc = qobject_cast<Juff::Document*>(tab2_->widget(i));
			if ( doc != 0 )
				list << doc->fileName();
		}
	}
	return list;
}



void DocViewer::onDocMoveRequested(Juff::Document* doc, Juff::TabWidget* tw) {
	PanelIndex panel = (tw == tab1_ ? PanelRight : PanelLeft);
	PanelIndex otherPanel = anotherPanel(panel);
	
	addDoc(doc, panel);
	if ( docCount(otherPanel) == 0 ) {
		hidePanel(otherPanel);
	}
}

Juff::PanelIndex DocViewer::panelIndexOf(TabWidget* tw) const {
	if ( tw == tab1_ )
		return Juff::PanelLeft;
	else if ( tw == tab2_ )
		return Juff::PanelRight;
	else
		return Juff::PanelNone;
}

void DocViewer::onTabRemoved(Juff::TabWidget* tw) {
	LOGGER;
	
	Juff::TabWidget* tw2 = anotherTabWidget(tw);
	if ( tw->count() == 0 ) {
		if ( tw2->count() > 0 ) {
			tw2->currentWidget()->setFocus();
		}
		else {
			emit docActivated(NullDoc::instance());
		}
	}
	else {
		tw->currentWidget()->setFocus();
	}
}

void DocViewer::buildCtrlTabMenu(int curItem) {
	ctrlTabMenu_.clear();
	
	int i = 0;
	foreach (Juff::Document* doc, docStack_) {
		QAction* act = ctrlTabMenu_.addAction(doc->icon(), doc->title(), this, SLOT(onCtrlTabSelected()));
		act->setData(doc->fileName());
		if ( i == curItem )
			ctrlTabMenu_.setActiveAction(act);
		i++;
	}
}

void DocViewer::onCtrlTabSelected() {
	QAction* act = qobject_cast<QAction*>(sender());
	if ( act != 0 ) {
		QString fileName = act->data().toString();
		handler_->openDoc(fileName);
	}
}

void DocViewer::onDocStackCalled(bool direct) {
	if ( direct )
		buildCtrlTabMenu(1);
	else
		buildCtrlTabMenu(docStack_.count() - 1);
	
	int w = ( width() - ctrlTabMenu_.width() ) / 2;
	int h = ( height() - ctrlTabMenu_.height() ) / 2;
	ctrlTabMenu_.popup(mapToGlobal(QPoint(w, h)));
}

bool DocViewer::eventFilter(QObject *obj, QEvent *e) {
	if ( ctrlTabMenu_.isVisible() && e->type() == QEvent::KeyRelease ) {
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(e);
		
		if ( (keyEvent->modifiers() & Qt::ControlModifier) == false ) {
			QString fileName = ctrlTabMenu_.activeAction()->data().toString();
			handler_->openDoc(fileName);
			ctrlTabMenu_.hide();
		}
		return true;
	}
	else {
		return QWidget::eventFilter(obj, e);
	}
}


void DocViewer::onCurrentChanged(int) {
	QTabWidget* tw = qobject_cast<QTabWidget*>(sender());
	if ( tw != 0 ) {
		Juff::Document* doc = qobject_cast<Juff::Document*>(tw->currentWidget());
		if ( doc != 0 ) {
			doc->setFocus();
//			emit docActivated(doc);
		}
	}
}



void DocViewer::onDocModified(bool) {
	Juff::Document* doc = qobject_cast<Juff::Document*>(sender());
	if ( doc != 0 ) {
		updateDocTitle(doc);
	}
}

void DocViewer::onDocFocused() {
	Juff::Document* doc = qobject_cast<Juff::Document*>(sender());
	if ( doc != 0 ) {
		if ( doc != curDoc_ ) {
			curDoc_ = doc;
			emit docActivated(doc);
		}
		
		docStack_.removeAll(doc);
		docStack_.prepend(doc);
			
		if ( tab1_->indexOf(doc) >= 0 )
			curTab_ = tab1_;
		else
			curTab_ = tab2_;
	}
}


/*void DocViewer::onDocModificationChanged(Juff::Document*) {
}

void DocViewer::onDocAddedToProject(Juff::Document*, Juff::Project*) {
}

void DocViewer::onDocRemovedFromProject(Juff::Document*, Juff::Project*) {
}*/

Juff::TabWidget* DocViewer::getTabWidget(Juff::PanelIndex panel) const {
	TabWidget* tabWidget = NULL;
	switch ( panel ) {
		case PanelLeft    : tabWidget = tab1_; break;
		case PanelRight   : tabWidget = tab2_; break;
		case PanelCurrent : tabWidget = curTab_; break;
		default:;
	}
	return tabWidget;
}

Juff::TabWidget* DocViewer::anotherTabWidget(Juff::TabWidget* tw) const {
	return (tw == tab1_ ? tab2_ : tab1_);
}


} // namespace Juff
