/*
JuffEd - A simple text editor
Copyright 2007-2008 Mikhail Murzin

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

#ifndef _JUFFED_PLUGIN_INTERFACE_H_
#define _JUFFED_PLUGIN_INTERFACE_H_

class QAction;
class QObject;
class QMenu;
class QToolBar;
class QWidget;

#include <QtCore/QList>
#include <QtCore/QString>
#include <Qt>

typedef QList<QAction*> ActionList;

class JuffPlugin {
public:
	virtual ~JuffPlugin() { qDebug("Plugin deleted"); }
	//	initialization
	virtual void setParent(QObject* parent) = 0;

	//	deinitialization
	virtual void deinit() = 0;

	//	info
	virtual QString name() const = 0;
	virtual QString description() const = 0;

	//	controls
	virtual ActionList getMenuActions(const QString& menuName) const = 0;
	virtual QToolBar* toolBar() const = 0;
	virtual QMenu* menu() const = 0;
	virtual QWidget* settingsPage() const = 0;
	virtual void applySettings() = 0;
	virtual QWidget* dockWidget(Qt::DockWidgetArea& area) const = 0;

	QString path() const { return path_; }
	void setPath(const QString& path) { path_ = path; }
private:
	QString path_;
};

Q_DECLARE_INTERFACE(JuffPlugin, "JuffEd.JuffPlugin/1.0")

#endif
