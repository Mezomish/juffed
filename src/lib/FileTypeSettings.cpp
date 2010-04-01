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

#include "FileTypeSettings.h"


QStringList FileTypeSettings::getTypeList() {
	return Settings::value("filetypes", "types").toStringList();
}

QStringList FileTypeSettings::getFileNamePatterns(const QString& type) {
	if ( Settings::valueExists("filetypes-filenames", type) ) {
		return Settings::value("filetypes-filenames", type).toStringList();
	}
	else {
		QStringList list;
		if ( type == "Bash") 
			list << "*.sh" << "*.run";
		else if ( type == "Batch") 
			list << "*.bat";
		else if ( type == "C++") 
			list << "*.h" << "*.hpp" << "*.hxx" << "*.h++" << "*.c" << "*.cc" << "*.cpp" << "*.cxx" << "*.c++";
		else if ( type == "C#") 
			list << "*.cs";
		else if ( type == "CMake") 
			list << "CMakeLists.txt" << "*.cmake";
		else if ( type == "CSS") 
			list << "*.css";
		else if ( type == "D") 
			list << "*.d" << "*.di";
		else if ( type == "Diff") 
			list << "*.diff" << "*.patch";
		else if ( type == "HTML") 
			list << "*.htm*" << "*.[xd]htm*";
		else if ( type == "IDL") 
			list << "*.idl";
		else if ( type == "Java") 
			list << "*.java";
		else if ( type == "JavaScript") 
			list << "*.js";
		else if ( type == "Lua") 
			list << "*.lua" << "*.tasklua";
		else if ( type == "Makefile") 
			list << "*Makefile*";
		else if ( type == "Perl") 
			list << "*.p[lm]";
		else if ( type == "Python") 
			list << "*.py";
		else if ( type == "PHP") 
			list << "*.php*";
		else if ( type == "Ruby") 
			list << "*.rb";
		else if ( type == "SQL") 
			list << "*.sql";
		else if ( type == "TeX") 
			list << "*.tex";
		else if ( type == "XML") 
			list << "*.xml";
		else if ( type == "Fortran") 
			list << "*.f" << "*.f77" << "*.f90";

		return list;
	}
}

QStringList FileTypeSettings::getFirstLinePatterns(const QString& type) {
	if ( Settings::valueExists("filetypes-firstlines", type) ) {
		return Settings::value("filetypes-firstlines", type).toStringList();
	}
	else {
		QStringList list;

		if ( type == "Bash") 
			list << "*bash*" << "*/sh*";
		else if ( type == "Diff") 
			list << "Index: *";
		else if ( type == "HTML") 
			list << "<!doctype html*" << "<html*";
		else if ( type == "Perl") 
			list << "*perl*";
		else if ( type == "Python") 
			list << "*python*";
		else if ( type == "PHP") 
			list << "<?php*" << "<? *" << "<?";
		else if ( type == "TeX") 
			list << "\\documentclass";
		else if ( type == "XML") 
			list << "<!doctype*" << "<?xml*";

		return list;
	}
}

void FileTypeSettings::setTypeList(const QStringList& list) {
	Settings::setValue("filetypes", "types", list);
}

void FileTypeSettings::setFileNamePatterns(const QString& type, const QStringList& list) {
	Settings::setValue("filetypes-filenames", type, list);
}

void FileTypeSettings::setFirstLinePatterns(const QString& type, const QStringList& list) {
	Settings::setValue("filetypes-firstlines", type, list);
}

