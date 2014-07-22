/*
JuffEd - An advanced text editor
Copyright 2007-2010 Mikhail Murzin

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef __JUFF_SETTINGS_DLG_H__
#define __JUFF_SETTINGS_DLG_H__

#include <QtCore/QMap>
#include <QDialog>

class QDialogButtonBox;
class MultiPage;
class PluginPage;
class ColorSettingsPage;

class SettingsItem;
class SettingsPage;

class SettingsDlg : public QDialog {
Q_OBJECT
public:
	SettingsDlg(QWidget*);
	virtual ~SettingsDlg();

	int exec();
	bool isPluginEnabled(const QString&);
	void addPluginSettingsPage(const QString&, const QString&, QWidget*);
	void setEditorsPages(const QWidgetList&);
	void addPages(const QStringList&, const QWidgetList&);

	void addColorSetting(const QString& title, const QString& section, const QString& key, const QColor& color);

signals:
	void applied();

protected slots:
	void apply();
	void ok();
//	void somethingChanged(bool);

private:
	void init();

	QDialogButtonBox* buttonBox_;
	MultiPage* mp_;

	QWidget* pluginsMainPage_;
	QMap<QString, PluginPage*> pluginPages_;
	QList<SettingsPage*> pages_;
	ColorSettingsPage* colorsPage_;
//	QList<SettingsItem*> items_;
};

#endif // __JUFF_SETTINGS_DLG_H__
