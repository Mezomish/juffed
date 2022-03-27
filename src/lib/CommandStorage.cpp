#include "CommandStorage.h"

#include <QAction>
#include <QIcon>

#include "Constants.h"
#include "IconManager.h"
#include "Settings.h"
#include "KeySettings.h"

CommandStorage::CommandStorage(IconManagerInt* mgr) : CommandStorageInt(), iconManager_(mgr) {
	keys_[FILE_NEW]     = QKeySequence("Ctrl+N");
	keys_[FILE_OPEN]    = QKeySequence("Ctrl+O");
	keys_[FILE_SAVE]    = QKeySequence("Ctrl+S");
	keys_[FILE_SAVE_AS] = QKeySequence("Ctrl+Shift+S");
	keys_[FILE_RELOAD]  = QKeySequence("F5");
	keys_[FILE_CLOSE]   = QKeySequence("Ctrl+W");
	keys_[FILE_PRINT]   = QKeySequence("Ctrl+P");
	keys_[FILE_EXIT]    = QKeySequence("Ctrl+Q");
	
	keys_[EDIT_UNDO]    = QKeySequence("Ctrl+Z");
	keys_[EDIT_REDO]    = QKeySequence("Ctrl+Y");
	keys_[EDIT_CUT]     = QKeySequence("Ctrl+X");
	keys_[EDIT_COPY]    = QKeySequence("Ctrl+C");
	keys_[EDIT_PASTE]   = QKeySequence("Ctrl+V");
	
	keys_[SEARCH_FIND]       = QKeySequence("Ctrl+F");
	keys_[SEARCH_FIND_NEXT]  = QKeySequence("F3");
	keys_[SEARCH_FIND_PREV]  = QKeySequence("Shift+F3");
	keys_[SEARCH_REPLACE]    = QKeySequence("Ctrl+R");
	keys_[SEARCH_GOTO_LINE]  = QKeySequence("Ctrl+G");
	keys_[SEARCH_GOTO_FILE]  = QKeySequence("Shift+Ctrl+G");
	
	keys_[VIEW_ZOOM_IN]      = QKeySequence("Ctrl+=");
	keys_[VIEW_ZOOM_OUT]     = QKeySequence("Ctrl+-");
	keys_[VIEW_ZOOM_100]     = QKeySequence("Ctrl+0");
	keys_[VIEW_FULLSCREEN]   = QKeySequence("F11");
	keys_[MOVE_TO_OTHER_TAB] = QKeySequence("Alt+Z");
	
	keys_[HELP_ABOUT]        = QKeySequence("F1");
	
	QStringList keysList = Settings::instance()->keyList("keys");
	foreach (QString key, keysList) {
		keys_[key] = KeySettings::keySequence(key);
	}
}

void CommandStorage::addAction(const QString& key, const QString& name, QObject* obj, const char* slot) {
	QAction* a = new QAction(iconManager_->icon(key), name, obj);
	a->setShortcut(shortcut(key));

#if QT_VERSION >= 0x040400
	// fix for "Terminal: Ctrl-C and Ctrl-\ is not supported" plugins issue #1
	// it takes shortcuts used in main app as related to editor only.
	// Do not conflict with plugins.
	// TEMP solution
	if (key == EDIT_COPY) {
		a->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	}
#endif

	if ( obj != NULL && slot != NULL ) {
		QObject::connect(a, SIGNAL(triggered()), obj, slot);
	}
	actions_[key] = a;
}

void CommandStorage::addAction(const QString& key, QAction* action) {
	if ( action == NULL || key.isEmpty() ) {
		return;
	}
	actions_[key] = action;
	if ( keys_.contains(key) ) {
		action->setShortcut(keys_[key]);
	}
	else {
		keys_[key] = action->shortcut().toString();
	}
}

QAction* CommandStorage::action(const QString& key) const {
	return actions_.value(key, NULL);
}

QKeySequence CommandStorage::shortcut(const QString& key) const {
	return keys_.value(key, QKeySequence());
}

void CommandStorage::setShortcut(const QString& key, const QKeySequence& shortcut) {
	keys_[key] = shortcut;
	QAction* act = action(key);
	if ( act != NULL ) {
		act->setShortcut(shortcut);
	}
}

QStringList CommandStorage::actionIDs() const {
	return actions_.keys();
}
