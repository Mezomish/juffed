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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "AutocompleteSettings.h"

void AutocompleteSettings::set(IntKey key, int value) {
	switch ( key ) {
		case Threshold :
			Settings::instance()->setValue("autocomplete", "threshold", value);
			break;
	}
}

void AutocompleteSettings::set(BoolKey key, bool value) {
	switch ( key ) {
		case ReplaceWord :
			Settings::instance()->setValue("autocomplete", "replaceWord", value);
			break;

		case CaseSensitive :
			Settings::instance()->setValue("autocomplete", "caseSensitive", value);
			break;

		case UseDocument :
			Settings::instance()->setValue("autocomplete", "useDocument", value);
			break;

		case UseApis :
			Settings::instance()->setValue("autocomplete", "useApis", value);
			break;
	}
}


int AutocompleteSettings::get(IntKey key) {
	switch ( key ) {
		case Threshold :
			return Settings::instance()->intValue("autocomplete", "threshold");
	}
	return -1;
}

bool AutocompleteSettings::get(BoolKey key) {
	switch ( key ) {
		case ReplaceWord :
			return Settings::instance()->boolValue("autocomplete", "replaceWord");

		case CaseSensitive :
			return Settings::instance()->boolValue("autocomplete", "caseSensitive");

		case UseDocument :
			return Settings::instance()->boolValue("autocomplete", "useDocument");

		case UseApis :
			return Settings::instance()->boolValue("autocomplete", "useApis");
	}
	return false;
}
