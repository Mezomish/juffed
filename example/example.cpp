#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#include "Constants.h"
#include "Log.h"
#include "SciDocEngine.h"
#include "Settings.h"
#include "Utils.h"

// This class will be catching signals from actions.
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
	
	// Of course it's gonna crash if the doc_ is not initialized yet.
	// I've omitted extra checks for simplicity - it's just an example.
	
	void slotCut()   { doc_->cut();   }
	void slotCopy()  { doc_->copy();  }
	void slotPaste() { doc_->paste(); }
	void slotUndo()  { doc_->undo();  }
	void slotRedo()  { doc_->redo();  }

private:
	Juff::Document* doc_;
};

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	
	// Read JuffEd's settings
	Settings::instance()->read("juff", "juffed");
	
	// Create an object of the class defined abofe
	// that will catch signals from actions and call 
	// corresponding methods from a document.
	// It's a single document in this example but it can
	// be multiple documents in your case so you need to
	// keep track of the currently active document.
	SignalProcessor proc;
	
	// Initialize some standard commands (see Constants.h for 
	// more of them) and connect them to slots. You don't need 
	// to care about icons and shortcuts - it'll all be 
	// taken care of inside the CommandStorage class. It'll also
	// pick up custom shortcuts (if there are any) redefined in JuffEd.
	//
	// This needs to be done at the very beginning before any of 
	// documents were created (since the document populates its own
	// context menu using CommandStorage's actions).
	//
	CommandStorageInt* cmdStrg = Juff::Utils::commandStorage();
	cmdStrg->addAction(EDIT_CUT,   "Cut",   &proc, SLOT(slotCut()));
	cmdStrg->addAction(EDIT_COPY,  "Copy",  &proc, SLOT(slotCopy()));
	cmdStrg->addAction(EDIT_PASTE, "Paste", &proc, SLOT(slotPaste()));
	cmdStrg->addAction(EDIT_UNDO,  "Undo",  &proc, SLOT(slotUndo()));
	cmdStrg->addAction(EDIT_REDO,  "Redo",  &proc, SLOT(slotRedo()));
	
	
	// create an engine
	SciDocEngine* engine = new SciDocEngine();
	
	// Main Window
	QMainWindow mw;
	
	// Menus
	QMenu* editMenu = new QMenu("Edit");
	QMenu* viewMenu = new QMenu("View");
	// Add some standard actions we initialized above.
	// As per been said above, you don't need to care about
	// icons, shortcuts or slots - it's all been already 
	// taken care of.
	editMenu->addAction(cmdStrg->action(EDIT_CUT));
	editMenu->addAction(cmdStrg->action(EDIT_COPY));
	editMenu->addAction(cmdStrg->action(EDIT_PASTE));
	editMenu->addAction(cmdStrg->action(EDIT_UNDO));
	editMenu->addAction(cmdStrg->action(EDIT_REDO));
	editMenu->addSeparator();
	// An interesting part: getting engine-specific actions
	// and adding them to a menu. You don't need to take care
	// of slots - it's all done inside the engine.
	Juff::ActionList actions;
	actions = engine->mainMenuActions(Juff::MenuEdit);
	foreach (QAction* a, actions) {
		editMenu->addAction(a);
	}
	actions = engine->mainMenuActions(Juff::MenuView);
	foreach (QAction* a, actions) {
		viewMenu->addAction(a);
	}
	
	mw.menuBar()->addMenu(editMenu);
	mw.menuBar()->addMenu(viewMenu);
	
	// Toolbar
	QToolBar* tb = mw.addToolBar("main");
	tb->addAction(cmdStrg->action(EDIT_CUT));
	tb->addAction(cmdStrg->action(EDIT_COPY));
	tb->addAction(cmdStrg->action(EDIT_PASTE));
	tb->addAction(cmdStrg->action(EDIT_UNDO));
	tb->addAction(cmdStrg->action(EDIT_REDO));

	// Create a document
	Juff::Document* doc = engine->createDoc("example.cpp");
	// Notify the engine that this is the document that was activated
	engine->onDocActivated(doc);
	// Pass the document to the signal processor. Not it will not crash
	// when some of his slots are activated :)
	proc.setDocument(doc);
	
	
	// Set the document as a central widget and show the main window
	mw.setCentralWidget(doc);
	mw.resize(800, 600);
	mw.show();
	
	return app.exec();
}

#include "example.moc"
