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

#ifndef __JUFF_AUTOCOMPLETE_SETTINGS_H__
#define __JUFF_AUTOCOMPLETE_SETTINGS_H__

#include "Settings.h"

class AutocompleteSettings : public Settings {
public:
	static bool useDocument();
	static bool useApis();
	static bool replaceWord();
	static bool caseSensitive();
	static int threshold();

	static void setUseDocument(bool);
	static void setUseApis(bool);
	static void setReplaceWord(bool);
	static void setCaseSensitive(bool);
	static void setThreshold(int);
};

#endif // __JUFF_AUTOCOMPLETE_SETTINGS_H__
