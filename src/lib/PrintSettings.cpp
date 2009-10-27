/*
JuffEd - An advanced text editor
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

#include "PrintSettings.h"

bool PrintSettings::keepBgColor() {
	return Settings::boolValue("printing", "keepBgColor");
}

bool PrintSettings::keepColors() {
	return Settings::boolValue("printing", "keepColors");
}

bool PrintSettings::alwaysWrap() {
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
