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

#include "PrintSettings.h"

namespace Juff {

bool PrintSettings::get(BoolKey key) {
	switch ( key ) {
		case KeepBgColor :
			return Settings::instance()->boolValue("printing", "keepBgColor");
		
		case KeepColors :
			return Settings::instance()->boolValue("printing", "keepColors");
		
		case AlwaysWrap :
			return Settings::instance()->boolValue("printing", "alwaysWrap");
		
		default:
			return false;
	}
}

void PrintSettings::set(BoolKey key, bool value) {
	switch ( key ) {
		case KeepBgColor :
			Settings::instance()->setValue("printing", "keepBgColor", value);
		
		case KeepColors :
			Settings::instance()->setValue("printing", "keepColors", value);
		
		case AlwaysWrap :
			Settings::instance()->setValue("printing", "alwaysWrap", value);
		
		default: ;
	}
}

/*bool PrintSettings::alwaysWrap() {
	return Settings::boolValue("printing", "alwaysWrap");
}



void PrintSettings::setKeepBgColor(bool keep) {
	Settings::setValue("printing", "keepBgColor", keep);
}

void PrintSettings::setKeepColors(bool keep) {
	Settings::setValue("printing", "keepColors", keep);
}

void PrintSettings::setAlwaysWrap(bool wrap) {
	Settings::setValue("printing", "alwaysWrap", wrap);
}
*/

}
