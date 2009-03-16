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

#ifndef _JUFF_ICON_MANAGER_H_
#define _JUFF_ICON_MANAGER_H_

class IMInterior;

#include "Juff.h"
#include <QtGui/QIcon>

/**
* class IconManager
*
* Manages icon theme, reads icon theme from disk and provides 
* main window icons.
*/

class IconManager {
public:
	/**
	* instance()
	*
	* Returns an instance of IconManager
	*/
	static IconManager* instance();

	/**
	* ~IconManager()
	*
	* Destructor
	*/
	virtual ~IconManager();

	/**
	* TODO
	*/
	void setCurrentIconTheme(const QString&, int size);
	
	/**
	* iconThemes()
	*
	* Returns the list of all available icon themes.
	*/
	QStringList themeList() const;

	/**
	* icon()
	*
	* Returns an icon by action name.
	*/
	QIcon getIcon(Juff::CommandID);
	
protected:
	IconManager();

private:
	QIcon getDefaultIcon(Juff::CommandID);

	static IconManager* instance_;
	IMInterior* imInt_;
};

#endif // _ICON_MANAGER_H_
