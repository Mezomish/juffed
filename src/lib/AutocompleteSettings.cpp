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

#include "AutocompleteSettings.h"

void AutocompleteSettings::set(IntKey key, int value) {
	switch ( key ) {
		case Threshold :
			Settings::setValue("autocomplete", "threshold", value);
			break;
	}
}

void AutocompleteSettings::set(BoolKey key, bool value) {
	switch ( key ) {
		case ReplaceWord :
			Settings::setValue("autocomplete", "replaceWord", value);
			break;

		case CaseSensitive :
			Settings::setValue("autocomplete", "caseSensitive", value);
			break;

		case UseDocument :
			Settings::setValue("autocomplete", "useDocument", value);
			break;

		case UseApis :
			Settings::setValue("autocomplete", "useApis", value);
			break;
	}
}


int AutocompleteSettings::get(IntKey key) {
	switch ( key ) {
		case Threshold :
			return Settings::intValue("autocomplete", "threshold");
	}
}

bool AutocompleteSettings::get(BoolKey key) {
	switch ( key ) {
		case ReplaceWord :
			return Settings::boolValue("autocomplete", "replaceWord");

		case CaseSensitive :
			return Settings::boolValue("autocomplete", "caseSensitive");

		case UseDocument :
			return Settings::boolValue("autocomplete", "useDocument");

		case UseApis :
			return Settings::boolValue("autocomplete", "useApis");
	}
}
