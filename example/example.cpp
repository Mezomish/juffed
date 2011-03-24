#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#include "SciDocEngine.h"
#include "Utils.h"
#include "Constants.h"
#include "Log.h"

// "signal processor" class
class SignalProcessor : public QObject {
Q_OBJECT
public:
	SignalProcessor() : QObject() {
		doc_ = NULL;
	}

	void setDocument(Juff::Document* doc) {
		doc_ = doc;
	}
	
public slots:
	
	void slotCut() {
		doc_->cut();
	}
	
	void slotCopy() {
		doc_->copy();
	}
	
	void slotPaste() {
		doc_->paste();
	}
	
	void slotUndo() {
		LOGGER;
		doc_->undo();
	}
	
	void slotRedo() {
		doc_->redo();
	}

private:
	Juff::Document* doc_;
};

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	
	SignalProcessor proc;

	// create an engine
	SciDocEngine* engine = new SciDocEngine();

	// Initialize some standard commands (see Constants.h for more of them)
	// and connect them to slots.
	//
	// This needs to be done at the very beginning before any of 
	// documents were created (since the document populates its own
	// context menu using CommandStorage's actions).
	//
	CommandStorageInt* st = Juff::Utils::commandStorage();
	st->addAction(EDIT_CUT,   "Cut",   &proc, SLOT(slotCut()));
	st->addAction(EDIT_COPY,  "Copy",  &proc, SLOT(slotCopy()));
	st->addAction(EDIT_PASTE, "Paste", &proc, SLOT(slotPaste()));
	st->addAction(EDIT_UNDO,  "Undo",  &proc, SLOT(slotUndo()));
	st->addAction(EDIT_REDO,  "Redo",  &proc, SLOT(slotRedo()));
		
	
	// Main Window
	QMainWindow mw;

	// menu
	QMenu* editMenu = new QMenu("Edit");
	editMenu->addAction(st->action(EDIT_CUT));
	editMenu->addAction(st->action(EDIT_COPY));
	editMenu->addAction(st->action(EDIT_PASTE));
	editMenu->addAction(st->action(EDIT_UNDO));
	editMenu->addAction(st->action(EDIT_REDO));
	editMenu->addSeparator();
	Juff::ActionList actions = engine->mainMenuActions(Juff::MenuEdit);
	foreach (QAction* a, actions) {
		editMenu->addAction(a);
	}
	
	QMenu* viewMenu = new QMenu("View");
	actions = engine->mainMenuActions(Juff::MenuView);
	foreach (QAction* a, actions) {
		viewMenu->addAction(a);
	}
	
	mw.menuBar()->addMenu(editMenu);
	mw.menuBar()->addMenu(viewMenu);

	
	// toolbar
	QToolBar* tb = mw.addToolBar("main");
	tb->addAction(st->action(EDIT_CUT));
	tb->addAction(st->action(EDIT_COPY));
	tb->addAction(st->action(EDIT_PASTE));
	tb->addAction(st->action(EDIT_UNDO));
	tb->addAction(st->action(EDIT_REDO));

	
	// Create a document
	Juff::Document* doc = engine->createDoc("example.cpp");
	// notify the engine that this is the document that was activated
	engine->onDocActivated(doc);
	// set the document to the signal processor
	proc.setDocument(doc);
	
	
	// put the document there and show the main window
	mw.setCentralWidget(doc);
	mw.resize(800, 600);
	mw.show();
	
	return app.exec();
}

#include "example.moc"
