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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef __JUFF_SETTINGS_DLG_H__
#define __JUFF_SETTINGS_DLG_H__

#include <QtCore/QMap>
#include <QtGui/QDialog>

class QPushButton;
class MultiPage;
class PluginPage;

class SettingsItem;
class SettingsPage;

class SettingsDlg : public QDialog {
Q_OBJECT
public:
	SettingsDlg(QWidget*);
	virtual ~SettingsDlg();

	int exec();
	bool isPluginEnabled(const QString&);
	void addPluginSettingsPage(const QString&, QWidget*);
	void setEditorsPages(const QWidgetList&);

signals:
	void applied();

protected slots:
	void apply();
	void ok();
//	void somethingChanged(bool);

private:
	void init();

	QPushButton* okBtn_;
	QPushButton* applyBtn_;
	QPushButton* cancelBtn_;
	MultiPage* mp_;

	QWidget* pluginsMainPage_;
	QMap<QString, PluginPage*> pluginPages_;
	QList<SettingsPage*> pages_;
//	QList<SettingsItem*> items_;
};

#endif // __JUFF_SETTINGS_DLG_H__
