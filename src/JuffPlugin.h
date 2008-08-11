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
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <Qt>

typedef QList<QAction*> ActionList;
typedef QMap<QString, QVariant> SettingsMap;

class JuffPlugin {
public:
	virtual ~JuffPlugin() { qDebug("Plugin deleted"); }
	
	
	/*
	 * Reimplement these (and only these) functions 
	 * to create your own plugin.
	 */
	
	//	initialization
	virtual void init(QObject* handler) = 0;
	virtual void deinit(QObject* handler) = 0;

	//	info
	virtual QString name() const = 0;
	virtual QString description() const = 0;

	//	controls
//	virtual ActionList getMenuActions(const QString& menuName) const = 0;
	virtual QToolBar* toolBar() const = 0;
	virtual QMenu* menu() const = 0;
	virtual QWidget* dockWidget(Qt::DockWidgetArea& area) const = 0;
	virtual QWidget* settingsPage() const = 0;
	virtual void applySettings() = 0;

	/*
	 * That's the end of the part that is interesting to you.
	 * It's better not to touch anything else :).
	 */


	SettingsMap settings() const { return settings_; }
	void setSettings(const SettingsMap& settingsMap) { settings_ = settingsMap; }

protected:
	
	/*
	 * Use the following functions to get settings values
	 * that were stored to main application's config file
	 */
	bool getBoolValue(const QString& name, bool defaultValue = false) {
		return settings_.value(name, QVariant(defaultValue)).toBool();
	}
	int getIntValue(const QString& name, int defaultValue = 0) {
		return settings_.value(name, QVariant(defaultValue)).toInt();
	}
	double getDoubleValue(const QString& name, double defaultValue = 0.0) {
		return settings_.value(name, QVariant(defaultValue)).toDouble();
	}
	QString getStringValue(const QString& name, const QString& defaultValue = "") {
		return settings_.value(name, QVariant(defaultValue)).toString();
	}

	/*
	 * Use the following functions to store settings values
	 * to main application's config file
	 */
	void setBoolValue(const QString& name, bool value) {
		settings_[name] = QVariant(value);
	}
	void setIntValue(const QString& name, int value) {
		settings_[name] = QVariant(value);
	}
	void setDoubleValue(const QString& name, double value) {
		settings_[name] = QVariant(value);
	}
	void setStringValue(const QString& name, const QString& value) {
		settings_[name] = QVariant(value);
	}

private:
	SettingsMap settings_;
};

Q_DECLARE_INTERFACE(JuffPlugin, "JuffEd.JuffPlugin/1.0")

#endif
