#ifndef __JUFFED_COMMAND_STORAGE_H__
#define __JUFFED_COMMAND_STORAGE_H__

class QAction;

#include "Enums.h"

#include <QKeySequence>

class CommandStorage {
public:
	static CommandStorage* instance();

	QAction* action(Juff::ActionID) const;

private:
	CommandStorage();
	void createActions();

	QString title(Juff::ActionID) const;
	QKeySequence shortcut(Juff::ActionID) const;

	class Interior;
	Interior* int_;
	
	static CommandStorage* instance_;
};

#endif // __JUFFED_COMMAND_STORAGE_H__
