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
//	curTab_ = NULL;
	
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

void DocViewer::showPanel(PanelIndex panel) {
	int w = spl_->width() / 2;
	
	if ( (panel == PanelLeft && spl_->sizes()[0] == 0) 
	     || (panel == PanelRight && spl_->sizes()[1] == 0) ) {
		spl_->setSizes(QList<int>() << w << w);
	}
}

void DocViewer::hidePanel(PanelIndex panel) {
	QList<int> list;
	if ( panel == PanelLeft ) {
		list << 0 << spl_->width();
		curTab_ = tab2_;
	}
	else if ( panel == PanelRight ) {
		list << spl_->width() << 0;
		curTab_ = tab1_;
	}
	spl_->setSizes(list);
}

void DocViewer::addDoc(Juff::Document* doc, PanelIndex panel) {
//	LOGGER;
	
	QTabWidget* tabWidget = NULL;
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

		QString title = Juff::docTitle(doc->fileName(), doc->isModified());
		int index = tabWidget->addTab(doc, Juff::docIcon(doc), title);
		if ( Juff::isNoname(doc) )
			tabWidget->setTabToolTip(index, title);
		else
			tabWidget->setTabToolTip(index, doc->fileName());
		
		tabWidget->setCurrentWidget(doc);
		showPanel(panelOf(doc));
		
		doc->init();
		doc->setFocus();
		
		// It's better to have it after adding to TabWidget to avoid
		// emitting the signal 'docActivated()' during the document creation.
//		connect(doc, SIGNAL(focused()), SLOT(onDocFocused()));
		
		docStack_.removeAll(doc);
		docStack_.prepend(doc);
	}
}

void DocViewer::removeDoc(Juff::Document* doc) {
	docStack_.removeAll(doc);
}

Juff::Document* DocViewer::currentDoc() const {
//	LOGGER;
	Juff::Document* doc = qobject_cast<Juff::Document*>(curTab_->currentWidget());
	return doc != 0 ? doc : NullDoc::instance();
}

Juff::Document* DocViewer::currentDoc(PanelIndex panel) const {
	QTabWidget* tabWidget = NULL;
	switch ( panel ) {
		case PanelCurrent : tabWidget = curTab_; break;
		case PanelLeft    : tabWidget = tab1_;   break;
		case PanelRight   : tabWidget = tab2_;   break;
		default: ;
	}
	
	if ( tabWidget == NULL ) {
		return NullDoc::instance();
	}
	
	Juff::Document* doc = qobject_cast<Juff::Document*>(tabWidget->currentWidget());
	return doc != 0 ? doc : NullDoc::instance();
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
	QTabWidget* tabWidget = NULL;
	switch ( panel ) {
		case PanelLeft    : tabWidget = tab1_; break;
		case PanelRight   : tabWidget = tab2_; break;
		case PanelCurrent : tabWidget = curTab_; break;
		default:;
	}
	
	if ( tabWidget == NULL )
		return NullDoc::instance();
	
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
//	LOGGER;
	
	int n = curTab_->count();
	if ( n == 0 )
		return;
	
	curTab_->setCurrentIndex( (curTab_->currentIndex() + 1) % n );
}

void DocViewer::prevDoc() {
//	LOGGER;
	
	int n = curTab_->count();
	if ( n == 0 )
		return;
	
	curTab_->setCurrentIndex( (curTab_->currentIndex() + n - 1) % n );
}

void DocViewer::goToNumberedDoc() {
//	LOGGER;
	
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
	Juff::PanelIndex panel = (tw == tab1_ ? Juff::PanelRight : Juff::PanelLeft);
	addDoc(doc, panel);
	
	curTab_ = anotherPanel(tw);
	if ( curTab_->width() == 0 ) {
		spl_->setSizes(QList<int>() << spl_->width() / 2 << spl_->width() / 2);
	}
//	if ( tw->count() == 0 ) {
//		closePanel(tw);
//	}
}

void DocViewer::onTabRemoved(Juff::TabWidget* tw) {
	LOGGER;
	
	Juff::TabWidget* tw2 = anotherPanel(tw);
	if ( tw->count() == 0 ) {
		if ( tw2->count() > 0 ) {
//			closePanel(tw);
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
//	LOGGER;
	ctrlTabMenu_.clear();
	
	int i = 0;
	foreach (Juff::Document* doc, docStack_) {
		QAction* act = ctrlTabMenu_.addAction(Juff::docIcon(doc), Juff::docTitle(doc), this, SLOT(onCtrlTabSelected()));
		act->setData(doc->fileName());
		if ( i == curItem )
			ctrlTabMenu_.setActiveAction(act);
		i++;
	}
}

void DocViewer::onCtrlTabSelected() {
//	LOGGER;
	
	QAction* act = qobject_cast<QAction*>(sender());
	if ( act != 0 ) {
		QString fileName = act->data().toString();
		handler_->openDoc(fileName);
	}
}

void DocViewer::onDocStackCalled(bool direct) {
//	LOGGER;
	
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


void DocViewer::onCurrentChanged(int index) {
	QTabWidget* tw = qobject_cast<QTabWidget*>(sender());
	if ( tw != 0 ) {
		Juff::Document* doc = qobject_cast<Juff::Document*>(tw->currentWidget());
		if ( doc != 0 ) {
			doc->setFocus();
//			emit docActivated(doc);
		}
	}
}



void DocViewer::onDocModified(bool modified) {
//	LOGGER;

	Juff::Document* doc = qobject_cast<Juff::Document*>(sender());
	if ( doc != 0 ) {
		int index = tab1_->indexOf(doc);
		if ( index >= 0 ) {
			// doc belongs to 1st panel
			tab1_->setTabText(index, Juff::docTitle(doc));
			tab1_->setTabIcon(index, Juff::docIcon(doc));
		}
		else {
			index = tab2_->indexOf(doc);
			if ( index >= 0 ) {
				// doc belongs tn 2nd panel
				tab2_->setTabText(index, Juff::docTitle(doc));
				tab2_->setTabIcon(index, Juff::docIcon(doc));
			}
			Log::warning("Document that emitted a signal is not owned by DocViewer");
		}
	}
}

void DocViewer::onDocFocused() {
//	LOGGER;
	
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

Juff::TabWidget* DocViewer::anotherPanel(Juff::TabWidget* tw) const {
	return (tw == tab1_ ? tab2_ : tab1_);
}


} // namespace Juff
