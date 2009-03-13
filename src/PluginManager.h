/*
JuffEd - A simple text editor
Copyright 2007-2009 Mikhail Murzin

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

#ifndef _JUFF_PLUGIN_MANAGER_H_
#define _JUFF_PLUGIN_MANAGER_H_

class JuffPlugin;
class QMainWindow;

#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtGui/QWidgetList>

#include "Juff.h"
#include "ManagerInterface.h"
#include "Parameter.h"

typedef QList<JuffPlugin*> PluginList;

namespace Juff {

namespace GUI {
	class GUI;
}

class PluginManager : public QObject {
Q_OBJECT
public:
	PluginManager(const QStringList& engines, ManagerInterface*, GUI::GUI*);
	virtual ~PluginManager();

	void loadPlugin(const QString&);
	void loadPlugins();

	MenuList getMenus(const QString& engine);
	ToolBarList getToolBars(const QString& engine);
	QWidgetList getDocks(const QString& engine);
	ActionList getContextMenuActions(const QString& engine);

	void emitInfoSignal(InfoEvent, const Param&, const Param& = Param());
	void activatePlugins(const QString& type);

private:
	class Interior;
	Interior* pmInt_;
};

}	//	namespace Juff

#endif
