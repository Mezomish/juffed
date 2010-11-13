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

#include "LibConfig.h"

#include "IconManagerInt.h"

class QIcon;
class QString;

class LIBJUFF_EXPORT IconManager : public IconManagerInt {
public:
	IconManager();

	/**
	* Returns an icon of the current icon theme and the current size 
	* for the specified \param key. If the current theme doesn't contain
	* an appropriate icon then returns a built-in icon (from "<default>" theme).
	* If there is no default icon the returns an empty icon QIcon().
	*/
	virtual QIcon icon(const QString& key) const;

	/**
	* Returns the current icon size. The default size is 16.
	*/
	virtual int size() const;

	/**
	* Sets the icon size to \param size.
	*/
	virtual void setSize(int);

private:
	QIcon defaultIcon(const QString&) const;

	int size_;
};

#endif // __JUFFED_ICON_MANAGER_H__
