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

#ifndef _ICON_MANAGER_H_
#define _ICON_MANAGER_H_

class IMInterior;
	
#include <QtGui/QIcon>

class IconManager {
public:
	static IconManager* instance();
	virtual ~IconManager();

	void loadTheme(const QString&);
	QIcon icon(const QString& actionStr) const;
	QStringList iconThemes() const;

protected:
	IconManager();

private:
	static IconManager* instance_;
	IMInterior* imInt_;
};

#endif // _ICON_MANAGER_H_
