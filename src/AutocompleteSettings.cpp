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

#include "AutocompleteSettings.h"

bool AutocompleteSettings::useDocument() {
	return Settings::boolValue("autocomplete", "useDocument", true);
}
bool AutocompleteSettings::useApis() {
	return Settings::boolValue("autocomplete", "useApis", true);
}
bool AutocompleteSettings::replaceWord() {
	return Settings::boolValue("autocomplete", "replaceWord", true);
}
int AutocompleteSettings::threshold() {
	return Settings::intValue("autocomplete", "threshold", 2);
}


void AutocompleteSettings::setUseDocument(bool use) {
	Settings::setValue("autocomplete", "useDocument", use);
}
void AutocompleteSettings::setUseApis(bool use) {
	Settings::setValue("autocomplete", "useApis", use);
}
void AutocompleteSettings::setReplaceWord(bool replace) {
	Settings::setValue("autocomplete", "replaceWord", replace);
}
void AutocompleteSettings::setThreshold(int thr) {
	Settings::setValue("autocomplete", "threshold", thr);
}
