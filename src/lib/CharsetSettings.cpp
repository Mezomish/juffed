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

#include "CharsetSettings.h"

QStringList CharsetSettings::csList_ = QStringList()
	<< "Apple Roman"
	<< "Big5"
	<< "Big5-HKSCS"
	<< "EUC-JP"
	<< "EUC-KR"
	<< "GB18030-0"
	<< "IBM 850"
	<< "IBM 866"
	<< "IBM 874"
	<< "ISO 2022-JP"
	<< "ISO 8859-1"
	<< "ISO 8859-10"
	<< "ISO 8859-13"
	<< "ISO 8859-14"
	<< "ISO 8859-15"
	<< "ISO 8859-16"
	<< "ISO 8859-2"
	<< "ISO 8859-3"
	<< "ISO 8859-4"
	<< "ISO 8859-5"
	<< "ISO 8859-6"
	<< "ISO 8859-7"
	<< "ISO 8859-8"
	<< "ISO 8859-9"
	<< "Iscii-Bng"
	<< "Iscii-Dev"
	<< "Iscii-Gjr"
	<< "Iscii-Knd"
	<< "Iscii-Mlm"
	<< "Iscii-Ori"
	<< "Iscii-Pnj"
	<< "Iscii-Tlg"
	<< "Iscii-Tml"
	<< "JIS X 0201"
	<< "JIS X 0208"
	<< "KOI8-R"
	<< "KOI8-U"
	<< "MuleLao-1"
	<< "ROMAN8"
	<< "Shift-JIS"
	<< "TIS-620"
	<< "TSCII"
	<< "UTF-16"
	<< "UTF-16BE"
	<< "UTF-16LE"
	<< "UTF-8"
	<< "WINSAMI2"
	<< "Windows-1250"
	<< "Windows-1251"
	<< "Windows-1252"
	<< "Windows-1253"
	<< "Windows-1254"
	<< "Windows-1255"
	<< "Windows-1256"
	<< "Windows-1257"
	<< "Windows-1258";

QStringList CharsetSettings::getCharsetsList() { 
	return csList_;
}

bool CharsetSettings::charsetEnabled(const QString& charset) { 
	return Settings::instance()->boolValue("charset", charset); 
}

void CharsetSettings::setCharsetEnabled(const QString& charset, bool enable) { 
	Settings::instance()->setValue("charset", charset, enable); 
}
