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

#include "KeySettings.h"

QKeySequence KeySettings::keySequence(Juff::ActionID id) {
	QString idStr = QString::number(id);
	if ( idStr.isEmpty() )
		return QKeySequence();
	QString s = Settings::stringValue("keys", idStr);
	return QKeySequence(s);
}

void KeySettings::setKeySequence(Juff::ActionID id, const QKeySequence& seq) {
	QString idStr = QString::number(id);
	if ( idStr.isEmpty() )
		return;
	Settings::setValue("keys", idStr, seq.toString());
}

bool KeySettings::contains(Juff::ActionID id) {
	return keyList("keys").contains(QString::number(id));
}

