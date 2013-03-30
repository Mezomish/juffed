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

#ifndef __JUFF_KEY_SETTINGS_H__
#define __JUFF_KEY_SETTINGS_H__

#include "LibConfig.h"

#include <QKeySequence>

#include "Enums.h"

class LIBJUFF_EXPORT KeySettings {
public:
	static QKeySequence keySequence(const QString&);
	static void setKeySequence(const QString&, const QKeySequence&);

	static bool contains(const QString&);
};

#endif /* __JUFF_KEY_SETTINGS_H__ */
