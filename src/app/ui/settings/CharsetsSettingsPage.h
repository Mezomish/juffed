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

#ifndef __JUFF_CHARSETS_SETTINGS_PAGE_H__
#define __JUFF_CHARSETS_SETTINGS_PAGE_H__

//	Qt
#include <QtGui/QWidget>

// local
#include "SettingsPage.h"

//	ui
#include "ui_CharsetsSettingsPage.h"

class CharsetsSettingsPage : public SettingsPage {
Q_OBJECT
public:
	CharsetsSettingsPage(QWidget* parent = 0);
	virtual ~CharsetsSettingsPage();

	virtual void init();
	virtual void apply();

public slots:
	void selectAll();
	void deselectAll();
	void onItemClicked( QListWidgetItem* );

private:
	Ui::CharsetsSettingsPage ui;
};

#endif // __JUFF_CHARSETS_SETTINGS_PAGE_H__
