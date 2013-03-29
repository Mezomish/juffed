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

#include "FileTypeSettings.h"


QStringList FileTypeSettings::getTypeList() {
	if ( Settings::instance()->valueExists("filetypes", "types") )
		return Settings::instance()->value("filetypes", "types").toStringList();
	else
		return QStringList();
}

QStringList FileTypeSettings::getFileNamePatterns(const QString& type) {
	if ( Settings::instance()->valueExists("filetypes-filenames", type) ) {
		return Settings::instance()->value("filetypes-filenames", type).toStringList();
	}
	else {
		QStringList list;
		if ( type == "Ada") {
			list << "*.a";
		}
		else if ( type == "Asm") {
			list << "*.s" << "*.asm" << "*.ash";
		}
		else if ( type == "Bash") {
			list << "*.sh" << "*.run";
		}
		else if ( type == "Batch") {
			list << "*.bat";
		}
		else if ( type == "C++") {
			list << "*.h" << "*.hh" << "*.hpp" << "*.hxx" << "*.h++" << "*.c" << "*.cc" << "*.cpp" << "*.cxx" << "*.c++";
		}
		else if ( type == "C#") {
			list << "*.cs";
		}
		else if ( type == "CMake") {
			list << "CMakeLists.txt" << "*.cmake";
		}
		else if ( type == "CSS") {
			list << "*.css" << "*.qss";
		}
		else if ( type == "D") {
			list << "*.d" << "*.di";
		}
		else if ( type == "Diff") {
			list << "*.diff" << "*.patch";
		}
		else if ( type == "Fortran") {
			list << "*.f" << "*.f77" << "*.f90";
		}
		else if ( type == "Haskell") {
			list << "*.hs" << "*.lhs";
		}
		else if ( type == "HTML") {
			list << "*.htm*" << "*.[xd]htm*";
		}
		else if ( type == "IDL") {
			list << "*.idl";
		}
		else if ( type == "Java") {
			list << "*.java";
		}
		else if ( type == "JavaScript") {
			list << "*.js" << "*.qml";
		}
		else if ( type == "Lisp") {
			list << "*.lisp" << "*.lsp";
		}
		else if ( type == "Lua") {
			list << "*.lua" << "*.tasklua";
		}
		else if ( type == "Makefile") {
			list << "*Makefile*";
		}
		else if ( type == "Matlab") {
			list << "*.m";
		}
		else if ( type == "NBC") {
			list << "*.nbc";
		}
		else if ( type == "NSIS") {
			list << "*.nsi" << "*.nsh";
		}
		else if ( type == "NXC") {
			list << "*.nxc";
		}
		else if ( type == "Pascal") {
			list << "*.p{1,2}" << "*.pa[s345]";
		}
		else if ( type == "Perl") {
			list << "*.p[lm]";
		}
		else if ( type == "Python") {
			list << "*.py";
		}
		else if ( type == "PHP") {
			list << "*.php*";
		}
		else if ( type == "Qore") {
			list << "*.q" << "*.qc" << "*.ql";
		}
		else if ( type == "Qorus") {
			list << "*.qfd" << "*.qwf" << "*.qsd" << "*.qclass";
		}
		else if ( type == "Ruby") {
			list << "*.rb";
		}
		else if ( type == "SQL") {
			list << "*.sql";
		}
		else if ( type == "TeX") {
			list << "*.tex";
		}
		else if ( type == "TCL") {
			list << "*.tcl";
		}
		else if ( type == "XML") {
			list << "*.xml" << "*.qrc" << "*.ts";
		}

		return list;
	}
}

QStringList FileTypeSettings::getFirstLinePatterns(const QString& type) {
	if ( Settings::instance()->valueExists("filetypes-firstlines", type) ) {
		return Settings::instance()->value("filetypes-firstlines", type).toStringList();
	}
	else {
		QStringList list;

		if ( type == "Bash") 
			list << "*bash*" << "*/sh*";
		else if ( type == "Diff") 
			list << "Index: *";
		else if ( type == "HTML") 
			list << "<!doctype html*" << "<html*";
		else if ( type == "Pascal") 
			list << "program *" << "unit *";
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
		else if ( type == "Qore")
			list << "*qore*";

		return list;
	}
}

void FileTypeSettings::setTypeList(const QStringList& list) {
	Settings::instance()->setValue("filetypes", "types", list);
}

void FileTypeSettings::setFileNamePatterns(const QString& type, const QStringList& list) {
	Settings::instance()->setValue("filetypes-filenames", type, list);
}

void FileTypeSettings::setFirstLinePatterns(const QString& type, const QStringList& list) {
	Settings::instance()->setValue("filetypes-firstlines", type, list);
}

