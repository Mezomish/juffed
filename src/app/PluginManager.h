#ifndef __JUFFED_PLUGIN_MANAGER_H__
#define __JUFFED_PLUGIN_MANAGER_H__

class JuffPlugin;

namespace Juff {
	class DocHandlerInt;
	class PluginNotifier;
};

#include "Enums.h"
#include "Types.h"


class PluginManager {
public:
	PluginManager(Juff::DocHandlerInt*, Juff::PluginNotifier*);
	void loadPlugins();

	Juff::MenuList menus() const;
	Juff::ActionList actions(Juff::MenuID) const;

private:
	void loadPlugin(const QString&);

	QList<JuffPlugin*> plugins_;
	Juff::DocHandlerInt* handler_;
	Juff::PluginNotifier* notifier_;
};

#endif // __JUFFED_PLUGIN_MANAGER_H__
