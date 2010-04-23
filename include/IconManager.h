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

#ifndef __JUFFED_ICON_MANAGER_H__
#define __JUFFED_ICON_MANAGER_H__

#include <QIcon>

#include "Enums.h"

class IconManager {
public:
	static IconManager* instance();
	
	/**
	* Returns an icon of the current icon theme and the current size 
	* for the specified \param id. If the current theme doesn't contain
	* an appropriate icon then returns a built-in icon (from "<default>" theme).
	* If there is no default icon the returns an empty icon QIcon().
	*/
	QIcon icon(Juff::ActionID id) const;

	/**
	* Returns the current icon size. The default size is 16.
	*/
	int iconSize() const;

	/**
	* Sets the icon size to \param size.
	*/
	void setIconSize(int size);

	/**
	* Returns the current icon theme. It can be the name of some system icon
	* theme or the default value "<default>" (built-in icons).
	*/
	QString iconTheme() const;
	
	/**
	* Sets the icon theme to \param theme.
	*/
	void setIconTheme(const QString& theme);
	
private:
	IconManager();
	QIcon defaultIcon(Juff::ActionID) const;

	class Interior;
	Interior* int_;
	
	static IconManager* instance_;
};

#endif // __JUFFED_ICON_MANAGER_H__
