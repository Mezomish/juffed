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

#ifndef __JUFFED_AUTOCOMPLETE_SETTINGS_H__
#define __JUFFED_AUTOCOMPLETE_SETTINGS_H__

#include "LibConfig.h"

#include "Settings.h"

class LIBJUFF_EXPORT AutocompleteSettings {
public:
	enum IntKey {
		Threshold,
	};
	
	enum BoolKey {
		ReplaceWord,
		CaseSensitive,
		UseDocument,
		UseApis,
	};
	
	static void set(IntKey, int);
	static void set(BoolKey, bool);

	static int     get(IntKey);
	static bool    get(BoolKey);
};

#endif // __JUFFED_AUTOCOMPLETE_SETTINGS_H__
