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

#ifndef __JUFFED_FILE_TYPES_PAGE_H__
#define __JUFFED_FILE_TYPES_PAGE_H__

#include "SettingsPage.h"

#include "ui_FileTypesPage.h"

namespace Juff {

class FileTypesPage : public SettingsPage {
Q_OBJECT
public:
	FileTypesPage();

	virtual void init();
	virtual void apply();

protected slots:
	void typeChanged(const QString&);
	void addFileNamePattern();
	void removeFileNamePattern();
	void addFirstLinePattern();
	void removeFirstLinePattern();

private:
	Ui::FileTypesPage ui;
	QMap<QString, QStringList> fileNamePatterns_;
	QMap<QString, QStringList> firstLinePatterns_;
};

}

#endif // __JUFFED_FILE_TYPES_PAGE_H__
